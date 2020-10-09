#include "stdafx.h"
#include <string>
#include <vector>
#include "resource.h"
#include "IniWndStatus.h"
#include "Language.h"
#include "Config.h"
#include "OrderWnd.h"
#include "NerveBotDlg.h"
#include "API.h"
#include "JsonParse.h"
#include "StringConvert.h"
#include "PostServer.h"
#include "json.hpp"
#include "OrderControl.h"
#include "MonitorThread.h"
#include "MonitorOrder.h"
#include "Logs.h"

using json = nlohmann::json;
extern json GlobalJson;
extern CNerveBotDlg *MainWnd;
extern OrderWnd *pOrderWnd;
ULONGLONG Na = 100000000;

unsigned int __stdcall MonitorNewPrice(void* lp) {
	std::string NewPriceStr = GetItemName("���¼۸�");
	std::string AmplitudeStr = GetItemName("��ǰ�����");
	std::string StartPrice = GetItemName("�ҵ��۸�");
	while (1)
	{
		json TradingInfo = GetTradingInfo(GetConfig("TradingHash"));
		std::string JsonStr = TradingInfo.dump();
		if (JsonStr != "\"\"") {
			double NewPrice = TradingInfo["newPrice"].get<double>();
			Na = pow(10,TradingInfo["quoteDecimal"].get<float>());
			std::string::size_type size;
			double DoubleNewPrice = NewPrice == 0?0: NewPrice / Na;
			GlobalJson["NewPrice"] = DoubleNewPrice;

			std::string PrePrice = format("%s%f", NewPriceStr.c_str(), DoubleNewPrice);
			SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_CURPRICE, PrePrice.c_str());
			try
			{
				float P = GlobalJson["PrePrice"].get<float>();//���л������쳣
				if (P <= 0) {
					SaveAccountInfoToServer("pre_price", format("%f", GlobalJson["NewPrice"].get<float>()));//���µ�ʱ�����¼۸񱣴�Ϊ�ϴιҵ��۸��������жϺ�������
					P = GlobalJson["NewPrice"].get<float>();
					SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_PREPRICE, format("%s%f", StartPrice.c_str(),P).c_str());
				}
				float Amplitude = (fabs(GlobalJson["NewPrice"].get<float>() - P) / P) * 100;

				GlobalJson["Amplitude"] = Amplitude;
				SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_AMPLITUDE, format("%s%f%%", AmplitudeStr.c_str(), Amplitude).c_str());
			}
			catch (...)
			{
				std::string Logs = "��ȡ���¼۸��쳣��";
				Logs += "---" + JsonStr;
				LogsOut(Logs);
			}

			
		}

		Sleep(3000);
	}
}


unsigned int __stdcall MonitorThread(void* lp) {
	_beginthreadex(NULL, 0, MonitorNewPrice, NULL, 0, 0);
	_beginthreadex(NULL, 0, MonitorOrderState, &pOrderWnd->BuyList, 0, 0);
	_beginthreadex(NULL, 0, MonitorOrderState, &pOrderWnd->SellList, 0, 0);
	return 0;
}

