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
#include "MonitorThread.h"
#include "MonitorOrder.h"
using json = nlohmann::json;
extern int LoginState;//0Ϊδ��¼��1Ϊ��¼�ɹ���3Ϊ�˳���¼
extern CNerveBotDlg *MainWnd;
extern OrderWnd *pOrderWnd;
extern bool bMineManagerAccount;//ʶ��ǰ��¼���˻��Ƿ�Ϊ��ع����ַ
//�Ƿ�����Զ��ҵ����ж� 
extern bool IsAutoMakeOrder;
json GlobalJson;
/*
GlobalJson["ID"]
GlobalJson["PrePrice"]
GlobalJson["NewPrice"]
GlobalJson["asset"]["asset01"]
GlobalJson["asset"]["asset02"]
*/
unsigned int __stdcall DisplayAsset(void* lp) {
	std::string AssetStr = GetItemName("�ʲ���");
	while(1){
		std::vector<std::string> Tradings;
		std::string JosnStr = GetLedger(GetConfig("Address"), "");
		StringSplitA(Tradings, GetConfig("Trading"), "-");
		std::string TradingAsset00 = GetResultByLoopField(JosnStr, "data", "assetName", Tradings[0], "available");
		std::string TradingAsset01 = GetResultByLoopField(JosnStr, "data", "assetName", Tradings[1], "available");
		//�����ȡ�ʲ��쳣�������»�ȡ
		if (TradingAsset00 == "") {
			if ((int)lp == 1) {
				Sleep(1000);
				DisplayAsset(NULL);
			}
			else {
				continue;
			}


		}
		float TotalAsset00 = atof(GetResultByLoopField(JosnStr, "data", "assetName", Tradings[0], "freeze").c_str()) + atof(GetResultByLoopField(JosnStr, "data", "assetName", Tradings[0], "available").c_str());
		float TotalAsset01 = atof(GetResultByLoopField(JosnStr, "data", "assetName", Tradings[1], "freeze").c_str()) + atof(GetResultByLoopField(JosnStr, "data", "assetName", Tradings[1], "available").c_str());
		GlobalJson["asset"][Tradings[0]] = TotalAsset00;
		GlobalJson["asset"][Tradings[1]] = TotalAsset01;
		std::string AssetString = AssetStr + Tradings[0] + ":" + format("%f", TotalAsset00) + "  " + Tradings[1] + ":" + format("%f", TotalAsset01);
		SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_ASSET, AssetString.c_str());
		std::string Params = format("\"base_asset\":%f,\"quote_asset\":%f", TotalAsset00, TotalAsset01);
		UpdataToServer("account_detail", Params);
		if ((int)lp == 1)
			break;
		Sleep(10000);
	}

	return 0;
}
unsigned int __stdcall DisplayPrePrice(void* lp) {
	std::string StartPrice = GetItemName("�ҵ��۸�");
	std::string Params = format("\"%s\",\"%s\"", GetConfig("Address").c_str(), GetConfig("Trading").c_str());
	std::string JsonStr = PostServer("GetAccountInfo", Params);
	//OutputDebugStringA(format("Message:%s", JsonStr.c_str()).c_str());
	json J;
	while (1) {
		try {
			J = json::parse(JsonStr);
			if (J["success"].get<std::string>() == "true") {
				std::string PrePrice = J["data"]["pre_price"].get<std::string>().c_str();
				
				GlobalJson["PrePrice"] = atof(PrePrice.c_str());
				PrePrice = format("%s%s", StartPrice.c_str(), J["data"]["pre_price"].get<std::string>().c_str());
				SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_PREPRICE, PrePrice.c_str());
				break;
			}
		}
		catch (...) {

		}
		Sleep(1000);
	}

	return 0;
}

unsigned int __stdcall IniWndStatus(void* lp) {
	//��ʼΪGlobalJsonȫ�ֱ���
	GlobalJson["NewPrice"] = 0;
	while (1) {
		if (LoginState == 1) {
			SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_EDIT1, GetConfig("OrderNum").c_str());
			SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_EDIT3, GetConfig("Amplitude").c_str());
			SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_EDIT2, GetConfig("OrderDistance").c_str());
			SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_EDIT4, GetConfig("DropInPercent").c_str());
			if (GetConfig("AutoOrder") == "1") {
				SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_BUTTON4, GetItemName("�ر��Զ��ҵ�").c_str());
				IsAutoMakeOrder = true;
			}
			else {
				SetDlgItemTextA(pOrderWnd->m_hWnd, IDC_BUTTON4, GetItemName("�����Զ��ҵ�").c_str());
				IsAutoMakeOrder = false;
			}
			std::string Address = GetItemName("��ַ��") + GetConfig("Address");
			SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_ADDRESS, Address.c_str());
			std::string Trading = GetItemName("��ǰ���׶ԣ�") + GetConfig("Trading");
			SetDlgItemTextA(MainWnd->m_hWnd, IDC_STATIC_TRADING, Trading.c_str());
			_beginthreadex(NULL, 0, DisplayAsset, NULL, 0, 0);
			_beginthreadex(NULL, 0, DisplayPrePrice, NULL, 0, 0);
			
			_beginthreadex(NULL, 0, MonitorThread, NULL, 0, 0);
			_beginthreadex(NULL, 0, AutoMakeOrder, NULL, 0, 0);
			

			break;
		}
		
		Sleep(10);
	}
	return 0;

}