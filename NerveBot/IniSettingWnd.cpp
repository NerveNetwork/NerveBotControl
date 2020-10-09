// IniSettingWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NerveBot.h"
#include "IniSettingWnd.h"
#include "afxdialogex.h"
#include "Stringconvert.h"
#include "NerveBotDlg.h"
#include "API.h"
#include "json.hpp"
#include "OpenSSLAES.h"
#include "Config.h"
#include "Language.h"
#include "GetAddress.h"
#include "PostServer.h"
using json = nlohmann::json;
extern int LoginState;
extern CNerveBotDlg *MainWnd;
extern std::string UserPassword;
extern  int ChainId;
extern  std::string MainPrefix;
extern int CurrentLanguageId;
std::string GetPubKey(std::string PrivateKey);

// IniSettingWnd �Ի���

IMPLEMENT_DYNAMIC(IniSettingWnd, CDialogEx)

IniSettingWnd::IniSettingWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_Password(_T(""))
	, m_RePassword(_T(""))
	, m_PriKey(_T(""))
{

}

IniSettingWnd::~IniSettingWnd()
{
}

void IniSettingWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, Tradings);
	DDX_Control(pDX, IDC_EDIT1, PriKey);
	DDX_Control(pDX, IDC_EDIT6, Password);
	DDX_Control(pDX, IDC_EDIT7, RePassword);
	DDX_Text(pDX, IDC_EDIT6, m_Password);
	DDX_Text(pDX, IDC_EDIT7, m_RePassword);
	DDX_Text(pDX, IDC_EDIT1, m_PriKey);
	DDX_Control(pDX, IDC_COMBO3, LanguageList);
}


BEGIN_MESSAGE_MAP(IniSettingWnd, CDialogEx)
	ON_BN_CLICKED(IDOK, &IniSettingWnd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &IniSettingWnd::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_EN_KILLFOCUS(IDC_EDIT1, &IniSettingWnd::OnEnKillfocusEdit1)
	ON_EN_SETFOCUS(IDC_EDIT1, &IniSettingWnd::OnEnSetfocusEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO3, &IniSettingWnd::OnCbnSelchangeCombo3)
END_MESSAGE_MAP()


// IniSettingWnd ��Ϣ�������
void IniSettingWnd::LoadLanguage() {
	CString mOk, mCancel,mPriKey, mPassword, mRePassword,mTradingSymbol, mTitle;
	mPriKey = GetItemNameW(L"˽Կ��");
	mPassword = GetItemNameW(L"���룺");
	mRePassword = GetItemNameW(L"ȷ�����룺");
	mTradingSymbol = GetItemNameW(L"ѡ���׶ԣ�");

	mOk = GetItemNameW(L"ȷ��");
	mCancel = GetItemNameW(L"ȡ��");
	mTitle = GetItemNameW(L"����");

	SetDlgItemText(IDC_STATIC1, mPriKey);
	SetDlgItemText(IDC_STATIC2, mPassword);
	SetDlgItemText(IDC_STATIC3, mRePassword);
	SetDlgItemText(IDC_STATIC4, mTradingSymbol);

	SetDlgItemText(IDOK, mOk);
	SetDlgItemText(IDCANCEL, mCancel);
	
	this->SetWindowTextW(mTitle);

}

BOOL IniSettingWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	LanguageList.InsertString(0, L"English");
	LanguageList.InsertString(1, L"����");
	std::string Lang = GetConfig("Language");
	if (Lang == "" || Lang == "0") {
		CurrentLanguageId = 0;
		LanguageList.SetCurSel(1);
	}
	else {
		CurrentLanguageId = 1;
		LanguageList.SetCurSel(0);
	}
	LoadLanguage();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CenterWindow(GetDesktopWindow());
	std::string JsonStr = GetTradings();
	json J ;
	try {
		J = json::parse(JsonStr);
		if (J["msg"].get<std::string>() == "success") {
			int Index = 0;
			for (auto &i : J["data"]["result"].items()) {
				for(auto &t:i.value()["tradingDtoList"].items()){
					std::string Symbol = replace_str(t.value()["tradingName"].get<std::string>(),"/","-");
					Tradings.InsertString(Index, s2ws(Symbol).c_str());
					Index++;
				}
			}
			Tradings.SetCurSel(0);
		}
	}
	catch (...) {
		std::string Caption = GetItemName("�����쳣��������");
		std::string Title = GetItemName("��ʾ");
		MessageBoxA(NULL, Caption.c_str(), Title.c_str(), MB_OK);
		exit(-1);
		return TRUE;
	}
	
	
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

unsigned int __stdcall UpLoadAccountInfo(void* lp) {
	CString TradingName = s2ws(GetConfig("Trading")).c_str();
	TradingName.Replace(L"-", L"/");
	std::string TradingHash = GetTradingHash(ws2s(TradingName.GetBuffer()));

	SetConfig("TradingHash", TradingHash);
	std::string Param = format("\"%s\",\"%s\"", GetConfig("Address").c_str(), GetConfig("Trading").c_str());
	PostServer("CreateNewAccount", Param);
	LoginState = 1;
	return 0;
}

void IniSettingWnd::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if (m_PriKey == L"") {
		SetDlgItemText(IDC_STATIC_Notifaction, GetItemNameW(L"������˽Կ"));
		return;
	}
	if (m_Password == L"") {
		SetDlgItemText(IDC_STATIC_Notifaction, GetItemNameW(L"����������"));
		return;
	}
	if (m_Password != m_RePassword) {
		SetDlgItemText(IDC_STATIC_Notifaction, GetItemNameW(L"������������벻һ��"));
		return;
	}
	CString PassWord;
	GetDlgItemText(IDC_EDIT6, PassWord);
	//UpdateData();
	UserPassword = ws2s(PassWord.GetBuffer());
	std::string EncPriKey = AesEncrypt(ws2s(m_PriKey.GetBuffer()), ws2s(PassWord.GetBuffer()));
	SetConfig("EncPriKey", EncPriKey);
	SetConfig("PubKey", GetPubKey(ws2s(m_PriKey.GetBuffer())));
	CString TradingStr = L"";
	Tradings.GetWindowText(TradingStr);
	SetConfig("Trading", ws2s(TradingStr.GetBuffer()));
	SetConfig("Address", GetAddressBuyPubKey(ChainId, 1, GetConfig("PubKey"), MainPrefix));
	_beginthreadex(NULL, NULL, UpLoadAccountInfo, NULL, 0, 0);
	MainWnd->CenterWindow();	
	DestroyWindow();
	
}


void IniSettingWnd::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PostMessage(WM_CLOSE);
}


//void IniSettingWnd::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO: �ڴ˴������Ϣ����������
//}


//void IniSettingWnd::OnClose()
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	
//	CDialogEx::OnClose();
//}


//void IniSettingWnd::OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	CDialogEx::OnShowWindow(bShow, nStatus);
//	
//	// TODO: �ڴ˴������Ϣ����������
//}


//BOOL IniSettingWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//
//	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
//}


BOOL IniSettingWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (message == WM_CLOSE)
	{
		exit(-1);
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

CString PriKey = L"";
void IniSettingWnd::OnEnKillfocusEdit1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	PriKey.SetPasswordChar('*');

}


void IniSettingWnd::OnEnSetfocusEdit1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PriKey.SetPasswordChar('\0');
}


void IniSettingWnd::OnCbnSelchangeCombo3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString Lang = L"";
	LanguageList.GetLBText(LanguageList.GetCurSel(), Lang);

	if (Lang == L"English") {
		SetConfig("Language", "1");
	}
	else {
		SetConfig("Language", "0");
	}
	WinExec("NerveBot.exe", SW_HIDE);
	exit(-1);
}
