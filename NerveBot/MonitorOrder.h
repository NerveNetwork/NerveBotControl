#pragma once
typedef struct {
	CListCtrl *List;
	int Index;
}MONITOR_ORDER_INFO;
//��ض���״̬
unsigned int __stdcall MonitorOrderState(void* lp);
//���ָ������
unsigned int __stdcall MonitorAssignOrder(void *lp);
//��ʼ������״̬
unsigned int __stdcall IniOrderStatus(void* lp);
//��������䶯���Զ�����ҵ��߼�
unsigned int __stdcall AutoMakeOrder(void* lp);