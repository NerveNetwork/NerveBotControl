#pragma once
typedef struct {
	std::string OrderId;
	int Index;
	CListCtrl *List;
	int Status;//0�ɽ�����������Ҫ��ɾ����1Ϊ��Ҫɾ����2Ϊɾ�����
}CANCEL_ORDER_INFO;
unsigned int __stdcall CancelOrders(void*lp);