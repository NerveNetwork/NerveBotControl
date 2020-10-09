#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
typedef struct {
	float Price;
	float Amount;
	std::string TxHex;
	CListCtrl *List;
	int Index;
	std::string OrderId;
	time_t CreateTime;
	std::string RoundHash;
	int Type;// 1���򵥣�2������
	int Status;//0��ʼ�� 1��ʾ������װ��� 2��ʾbroadcast��� 3��ʾ��ѯ���

}OrderInfo;
typedef struct {
	CListCtrl *List;
	int Index;
	int Type;// 1���򵥣�2������
	OrderInfo Order;
}MAKE_ORDER_INFO;

typedef struct {
	std::string TxHex;
	CListCtrl *Order;
	int Index;

}DO_ORDER_INFO;
std::vector<OrderInfo> ComputeOrderInfo(int OrderNum, float AssetAmount, float StartPrice, float PriceStep, float h);
std::vector<OrderInfo>  MakeTmpOrder(float h);
unsigned int __stdcall MakeOrderInfo(void* lp);
void DoOrder();
std::string GetOmitHash(std::string Hash);
std::string GetRoundOrderHash();
//��ʼ��ʱ���ӷ�������ȡ�ҵ���Ϣ
unsigned int __stdcall GetOrderListFromServer(void* lp);
std::string GetOrderState(int State);
