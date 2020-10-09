#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// OrderWnd �Ի���

class OrderWnd : public CDialogEx
{
	DECLARE_DYNAMIC(OrderWnd)

public:
	OrderWnd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~OrderWnd();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl BuyList;
	CListCtrl SellList;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton4();
	afx_msg void LoadLanguage();
};
