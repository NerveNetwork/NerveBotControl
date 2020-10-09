
// NerveBotDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "OrderWnd.h"
#include "MineManagerWnd.h"

// CNerveBotDlg �Ի���
class CNerveBotDlg : public CDialogEx
{
// ����
public:
	CNerveBotDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NERVEBOT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl MainTable;
	OrderWnd MyOrderWnd;
	MineManagerWnd MyMineManagerWnd;
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox LanguageList;
//	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnEditchangeCombo2();
	afx_msg void OnCbnSelchangeCombo2();
};
