#include "stdafx.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include "resource.h"
#include "Config.h"
#include "OrderControl.h"
#include "json.hpp"
#include "StringConvert.h"
#include "OrderWnd.h"
#include "Language.h"
#include "OpenSSLAES.h"
#include "sha256.h"
#include "API.h"
#include "PostServer.h"
#include "IniWndStatus.h"
#include "MonitorOrder.h"
#include "Logs.h"
using json = nlohmann::json;
extern json GlobalJson;
extern OrderWnd *pOrderWnd;
extern std::string UserPassword;
extern int LoginState ;//0Ϊδ��¼��1Ϊ��¼�ɹ���3Ϊ�˳���¼
std::string Singture(std::string PrivateKey, std::string Hash);
std::string hex2string(const std::string &strHex);
float Parabola(float x,float h) {
	float y = -0.1*pow((x - h), 2) + 12;
	return y;
}
std::vector<DO_ORDER_INFO> DoOrderInfo;//����������װ���صĵ�txhash,����broadcast
//OrderNum �µ�����
//AssetAmount ʹ���ʲ�����
//StartPrice ��ʼ�۸�
//PriceStep ÿ������ٷֱ�
//h ��Ϊ10������Ϊ-10
std::vector<OrderInfo> ComputeOrderInfo(int OrderNum,float AssetAmount,float StartPrice,float PriceStep,float h) {
	std::vector<OrderInfo> OrderInfoList;
	OrderInfo Tmp;
	float Distance;
	if (h > 0)
		Distance = 20.5;
	else
		Distance = -20.5;
	float Section = Distance / (float)OrderNum;
	float i = 0;
	float AllUnit = 0;
	std::default_random_engine e(time(0));
	std::uniform_real_distribution<double> u(0.0005, 0.0009);
	double PriceAmplitude = u(e);
	while (1) {
		float x = Section*i;
		float y = Parabola(x, h);
		float OrderPrice;
		if (h > 0)
			OrderPrice = StartPrice * (1 - PriceAmplitude) - (StartPrice*(PriceStep/100)*i);
		else
			OrderPrice = StartPrice * (1 + PriceAmplitude) + (StartPrice*(PriceStep/100)*i);
		Tmp.Amount = y;
		Tmp.Price = OrderPrice;
		Tmp.Status = 0;
		OrderInfoList.push_back(Tmp);
		AllUnit += y;
		i++;
		if (i >= OrderNum)
			break;

	}
	float PerUnitAsset = AssetAmount / AllUnit;
	for (int i = 0; i < OrderInfoList.size(); i++)
	{
		OrderInfoList[i].Amount = OrderInfoList[i].Amount*PerUnitAsset;
	}
	return OrderInfoList;
}
//����y=a(x-h)^2+k ������ҵ������ͼ۸�
std::vector<OrderInfo>  MakeTmpOrder(float h) {
	DisplayAsset((void*)1);//�ҵ�ǰ�������ʲ�����
	//���ݵ�ǰ�ʲ��İٷֱȽ��йҵ�
	std::vector<std::string> Tradings;
	std::string JosnStr = GetLedger(GetConfig("Address"), "");
	StringSplitA(Tradings, GetConfig("Trading"), "-");
	float DropInPercent = atof(GetConfig("DropInPercent").c_str())/100;
	float BaseAsset;
	float QuoteAsset;
	int OrderNum = atoi(GetConfig("OrderNum").c_str());
	float AssetAmount;
	CListCtrl * OrderList = NULL;
	int Type = 0;
	std::vector<OrderInfo> OrderInfo;
	float NewPrice = GlobalJson["NewPrice"].get<float>();
	if (NewPrice <= 0) {
		MessageBoxA(NULL, GetItemName("��ǰ�۸�Ϊ0���ҵ��쳣").c_str(), GetItemName("��ʾ").c_str(), MB_OK);
		return OrderInfo;
	}
	try
	{
		BaseAsset = GlobalJson["asset"][Tradings[0]].get<float>()*DropInPercent;
		QuoteAsset = GlobalJson["asset"][Tradings[1]].get<float>()*DropInPercent;
		//��
		if (h > 0) {
			OrderList = &pOrderWnd->BuyList;
			Type = 1;
			AssetAmount = QuoteAsset / NewPrice;
		}
		//����
		else {
			OrderList = &pOrderWnd->SellList;
			Type = 2;
			AssetAmount = BaseAsset;
		}
		float StartPrice = NewPrice;
		float PriceStep = atof(GetConfig("OrderDistance").c_str());
		CString Trading = s2ws(GetConfig("Trading")).c_str();
		OrderInfo = ComputeOrderInfo(OrderNum, AssetAmount, StartPrice, PriceStep, h);
	}
	catch (...)
	{
		Sleep(1000);
		MakeTmpOrder(h);
	}

	OrderList->DeleteAllItems();
	for (int i = 0; i < OrderInfo.size(); i++) {
		int nRow = OrderList->InsertItem(i, L"");
		OrderList->SetItemText(nRow, 1, GetItemNameW(L"�ȴ��ҵ�").GetBuffer());
		OrderList->SetItemText(nRow, 2, s2ws(format("%f", OrderInfo[i].Amount)).c_str());
		OrderList->SetItemText(nRow, 3, s2ws(format("%f", OrderInfo[i].Price)).c_str());
		pOrderWnd->UpdateData();
		OrderInfo[i].Index = i;
		OrderInfo[i].Type = Type;
		OrderInfo[i].List = OrderList;
	}
	return OrderInfo;
}
std::string GetRoundOrderHash() {
	SHA256 sha256;
	std::string ShaStr = format("%s%s%d", GetConfig("Address").c_str(), GetConfig("Trading").c_str(), ::GetTickCount());
	return sha256(ShaStr);
}

std::string GetOmitHash(std::string Hash) {
	return Hash.substr(0, 6) + "..." + Hash.substr(Hash.size() - 6);
}
unsigned int __stdcall MakeOrderInfo(void* lp) {
	
	pOrderWnd->GetDlgItem(IDC_BUTTON1)->EnableWindow(0);

	//�ҵ�ǰ����RoundHash�����ڹ���ÿ�ֹҵ�
	std::string OrderRoundHash = GetRoundOrderHash();
	SaveAccountInfoToServer("round_hash", OrderRoundHash);
	SaveAccountInfoToServer("pre_price", format("%f", GlobalJson["NewPrice"].get<float>()));//���µ�ʱ�����¼۸񱣴�Ϊ�ϴιҵ��۸��������жϺ�������
	DisplayPrePrice(NULL);//�����µ��۸�Ľ�����ʾ



	std::vector<OrderInfo> OrderInfoSell = MakeTmpOrder(10);
	if (OrderInfoSell.size() <= 0)
		return 0;
	std::vector<OrderInfo> OrderInfoBuy = MakeTmpOrder(-10);
	std::vector<OrderInfo> OrderInfoAll;
	OrderInfoAll.insert(OrderInfoAll.end(), OrderInfoSell.begin(), OrderInfoSell.end());
	OrderInfoAll.insert(OrderInfoAll.end(), OrderInfoBuy.begin(), OrderInfoBuy.end());
	std::string Address = GetConfig("Address");
	std::string Trading = replace_str(GetConfig("Trading"), "-", "");
	std::string PriKey = AesDecrypt(GetConfig("EncPriKey"), UserPassword);

	while (1) {
		bool bExit = true;
		for (int i = 0; i < OrderInfoAll.size(); i++) {
			OrderInfo Order = OrderInfoAll[i];
			if (Order.Status == 0) 
			{
				bExit = false;
				std::string JsonStr = GetMameOrderHash(Address, Order.Amount, Order.Price, Trading, Order.Type);//��װ����
				std::string Sha256Hash = "";
				std::string SingBroadcastHash = "";
				json J;
				try
				{
					J = json::parse(JsonStr);
					if (J["success"].get<bool>() == false) {
						Order.List->SetItemText(Order.Index, 1, s2ws(J["msg"].get<std::string>()).c_str());
						OrderInfoAll[i].Status = 3;
					}
					else {
						std::string TxHex = J["data"].get<std::string>();
						TxHex = TxHex.substr(0, TxHex.size() - 2);
						Order.TxHex = TxHex;
						OrderInfoAll[i].Status = 1;
						////////////////////////////////////////////////////////////////�㲥����
						SHA256 sha256;
						std::string Hash = sha256(hex2string(sha256(hex2string(TxHex))));		
						Sha256Hash = Hash;
						std::string SingHash = Singture(PriKey, Hash);
						SingHash = TxHex + SingHash;
						SingBroadcastHash = SingHash;
						std::string Result = Broadcast(SingHash);
						J = json::parse(Result);
						if (J["success"].get<bool>())
						{
							OrderInfoAll[i].OrderId = J["data"].get<std::string>();
							Order.List->SetItemText(Order.Index, 1, GetItemNameW(L"�ҵ���"));
							OrderInfoAll[i].Status = 2;
							////////////////////////////////////////////////////////////////��ʼ������״̬�������ϴ�������������

							OrderInfo* lpParam = new OrderInfo;
							lpParam->Amount = Order.Amount;
							lpParam->CreateTime = Order.CreateTime;
							lpParam->Index = Order.Index;
							lpParam->List = Order.List;
							lpParam->OrderId = OrderInfoAll[i].OrderId;
							lpParam->Price = Order.Price;
							lpParam->RoundHash = OrderRoundHash;
							lpParam->Type = Order.Type;
							_beginthreadex(NULL, 0, IniOrderStatus, lpParam, 0, 0);
							OrderInfoAll[i].Status = 3;
						}
						else {
							std::string Msg = J["msg"].get<std::string>();
							if(Msg == "quantity too little ")
								OrderInfoAll[i].Status = 3;
							std::string Logs = "�µ��쳣��" + Result + JsonStr + Sha256Hash + "---" + SingBroadcastHash;
							//std::string Logs = "�µ��쳣��" + Msg;
							LogsOut(Logs);
						}
						
					}
				}
				catch (const std::exception& a) {
					std::string Logs = "����Json�쳣��";
					Logs+= a.what();
					Logs += "---" + JsonStr;
					LogsOut(Logs);
				}
				catch (...)
				{
					LogsOut("δ֪�쳣��"+ JsonStr);
				}
			}
			else if (Order.Status == 1)
			{
			
			}
			else if(Order.Status == 2)
			{
				//bExit = false;
				//OrderInfo* lpParam = new OrderInfo;
				//memcpy(lpParam, &Order, sizeof(OrderInfo));
				//_beginthreadex(NULL, 0, IniOrderStatus, lpParam, 0, 0);
				//OrderInfoAll[i].Status = 3;
				////////////////////////////////////////////////////////////////��ѯ���������ϴ�������������
				//std::string OrderId = Order.OrderId;
				//std::string Result = GetOrderInfo(OrderId);
				//json J;
				//try
				//{
				//	J = json::parse(Result);
				//	if (J["success"].get<bool>())
				//	{
				//		std::string Hash = J["data"]["id"].get<std::string>();
				//		LONGLONG CreateTime = J["data"]["createTime"].get<LONGLONG>();
				//		Order.List->SetItemText(Order.Index, 0, s2ws(Hash).c_str());
				//		Order.List->SetItemText(Order.Index, 4, s2ws(TimeToString(CreateTime / 1000, "%d-%02d-%02d %02d:%02d:%02d")).c_str());
				//		std::string Params = format("\"hash\":\"%s\",\"round_hash\":\"%s\",\"type\":%d,\"price\":%f,\"quantity\":%f,\"create_time\":%lld", Hash.c_str(), OrderRoundHash.c_str(),Order.Type,Order.Price,Order.Amount, CreateTime);
				//		SaveDateToServer("order_list", Params);
				//		OrderInfoAll[i].Status = 3;
				//	}
				//	else
				//	{
				//		Order.List->SetItemText(Order.Index, 0, L"...");
				//	}
				//}
				//catch (...)
				//{

				//}
				

			}
		}
		if (bExit)
		{
			pOrderWnd->GetDlgItem(IDC_BUTTON1)->EnableWindow(1);
			return 0;
		}
	}
	return 0;
}
void DoOrder()
{
	_beginthreadex(NULL, 0, MakeOrderInfo, NULL, 0, 0);
}

void ShowOrderToListControl(std::vector<OrderInfo> Order) {
	if (Order.size() > 0) {
		Order[0].List->DeleteAllItems();
		for (int i = 0; i < Order.size(); i++) {
			int nRow = Order[i].List->InsertItem(i, L"");
			Order[i].List->SetItemText(nRow, 0, s2ws(Order[i].OrderId).c_str());
			Order[i].List->SetItemText(nRow, 1, GetItemNameW(L"�����").GetBuffer());
			Order[i].List->SetItemText(nRow, 2, s2ws(format("%f", Order[i].Amount)).c_str());
			Order[i].List->SetItemText(nRow, 3, s2ws(format("%f", Order[i].Price)).c_str());
			Order[i].List->SetItemText(nRow, 4, s2ws(TimeToString(Order[i].CreateTime/1000, "%d-%02d-%02d %02d:%02d:%02d")).c_str());
			
		}
	}

}
std::string GetOrderState(int State) {
	std::string Result = "";
	if (State == 1) {
		Result = GetItemName("�ҵ���");
	}
	else if (State == 2) {
		Result = GetItemName("���ֳɽ�");
	}
	else if (State == 3) {
		Result = GetItemName("�ѳɽ�");
	}
	else if (State == 4) {
		Result = GetItemName("�ѳ���");
	}
	else if (State == 5) {
		Result = GetItemName("���ֳɽ��ѳ���");
	}
	return Result;
}
bool CompUp(const OrderInfo & a, const OrderInfo & b)
{
	return a.Price < b.Price;
}
bool CompDown(const OrderInfo & a, const OrderInfo & b)
{
	return a.Price > b.Price;
}
//��ʼ��ʱ���ӷ�������ȡ�ҵ���Ϣ
unsigned int __stdcall GetOrderListFromServer(void* lp)
{
	while (1)
	{
		if (LoginState == 1)
			break;
		Sleep(50);
	}
	//��ȡRoundHash
	std::string RoundHash = GetAccountInfo("round_hash");
	if (RoundHash == "")
	{
		std::string Logs = "�ӷ�������ȡ�����쳣��" + RoundHash;
		LogsOut(Logs);
		return 0;
	}
	std::vector<OrderInfo> BuyOrder;
	std::vector<OrderInfo> SellOrder;
	std::string JsonOrder = PostServer("GetDateFromServer", format("\"%s\"", RoundHash.c_str()));
	json J;
	try
	{
		J = json::parse(JsonOrder);
		if (J["success"].get<std::string>() != "false") {
			for (auto&i : J["data"].items()) {
				auto Item = i.value();
				OrderInfo Tmp;
				Tmp.OrderId = Item["hash"].get<std::string>();
				Tmp.Amount = atof(Item["quantity"].get<std::string>().c_str());
				Tmp.Price = atof(Item["price"].get<std::string>().c_str());
				Tmp.Type = atoi(Item["type"].get<std::string>().c_str());
				Tmp.CreateTime = _atoi64(Item["create_time"].get<std::string>().c_str());
				CListCtrl * OrderList = NULL;
				if (Item["type"] == "1") {
					Tmp.List = &pOrderWnd->BuyList;
					BuyOrder.push_back(Tmp);
				}
				else {
					Tmp.List = &pOrderWnd->SellList;
					SellOrder.push_back(Tmp);
				}				
			}
			std::sort(BuyOrder.begin(), BuyOrder.end(), CompDown);			
			std::sort(SellOrder.begin(), SellOrder.end(), CompUp);
			ShowOrderToListControl(BuyOrder);
			ShowOrderToListControl(SellOrder);

		}
	}
	catch (...)
	{
		std::string Logs = "�ӷ�������ȡ�����쳣��" + JsonOrder;
		LogsOut(Logs);
	}
	return 0;
}