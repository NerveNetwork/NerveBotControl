// LoginWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <string>
#include "NerveBot.h"
#include "LoginWnd.h"
#include "afxdialogex.h"
#include "Config.h"
#include "OpenSSLAES.h"
#include "StringConvert.h"
#include "Language.h"
#include "NerveBotDlg.h"
#include "GetAddress.h"
#include "json.hpp"
#include "API.h"
#include "PostServer.h"

using json = nlohmann::json;
extern int LoginState;
extern CNerveBotDlg *MainWnd;
extern int CurrentLanguageId;
extern std::string UserPassword;//�û����룬���ڽ���˽Կ
// LoginWnd �Ի���

IMPLEMENT_DYNAMIC(LoginWnd, CDialogEx)

LoginWnd::LoginWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{

}

LoginWnd::~LoginWnd()
{
}

void LoginWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, LanguageList);
	DDX_Control(pDX, IDC_COMBO3, Tradings);
}


BEGIN_MESSAGE_MAP(LoginWnd, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &LoginWnd::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &LoginWnd::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO2, &LoginWnd::OnCbnSelchangeCombo2)
//	ON_CBN_EDITCHANGE(IDC_COMBO3, &LoginWnd::OnCbnEditchangeCombo3)
ON_CBN_SELCHANGE(IDC_COMBO3, &LoginWnd::OnCbnSelchangeCombo3)
END_MESSAGE_MAP()


// LoginWnd ��Ϣ�������

unsigned int __stdcall SetItemFocus(void* lp) {
	//Sleep(2000);
	LoginWnd *Wnd = (LoginWnd*)lp;
	while (1) {
		Wnd->GetDlgItem(IDC_EDIT1)->SetFocus();
		Sleep(10);
	}
	

	return 0;
}
void LoginWnd::LoadLanguage(){
	CString mOk,mCancel,mPassword,mTitle;
	mOk = GetItemNameW(L"ȷ��");
	mCancel = GetItemNameW(L"ȡ��");
	mPassword = GetItemNameW(L"���룺");
	mTitle = GetItemNameW(L"��¼");
	SetDlgItemText(IDOK, mOk);
	SetDlgItemText(IDCANCEL, mCancel);
	SetDlgItemText(STATIC_PASSWORD, mPassword);
	this->SetWindowTextW(mTitle);

}
BOOL LoginWnd::OnInitDialog()
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
	CenterWindow(GetDesktopWindow());
	JsonStr = GetTradings();
	json J;
	try {
		J = json::parse(JsonStr);
		if (J["msg"].get<std::string>() == "success") {
			int Index = 0;
			for (auto &i : J["data"]["result"].items()) {
				for (auto &t : i.value()["tradingDtoList"].items()) {
					std::string Symbol = replace_str(t.value()["tradingName"].get<std::string>(), "/", "-");
					Tradings.InsertString(Index, s2ws(Symbol).c_str());
					if (Symbol == GetConfig("Trading")) {
						Tradings.SetCurSel(Index);
						CurIndex = Index;
					}
					Index++;
				}
			}
			
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


void LoginWnd::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	PostMessage(WM_CLOSE);
}

bool IsFocus = false;
BOOL LoginWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	//OutputDebugStringA(format("Message%X", message).c_str());
	if (message == WM_CLOSE)
	{
		exit(-1);
	}
	if ( message == 0x200)
		IsFocus = true;
	if (message == 0x14 && !IsFocus)
		GetDlgItem(IDC_EDIT1)->SetFocus();

	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void LoginWnd::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CString Password = L"";
	GetDlgItemText(IDC_EDIT1, Password);
	if (Password == L"") {
		SetDlgItemText(IDC_STATIC_Notifaction, GetItemNameW(L"����������"));
		return;
	}
	std::string DecPriKey =  AesDecrypt(GetConfig("EncPriKey"), ws2s(Password.GetBuffer()));
	if (DecPriKey.size() < 10)
	{
		SetDlgItemText(IDC_STATIC_Notifaction, GetItemNameW(L"�������"));
		return;
	}
	UserPassword = ws2s(Password.GetBuffer());
	MainWnd->CenterWindow();
	LoginState = 1;
	DestroyWindow();
}


void LoginWnd::OnCbnSelchangeCombo2()
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


//void LoginWnd::OnCbnEditchangeCombo3()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	int Type = MessageBoxA(NULL, GetItemName("�л�����").c_str(), GetItemName("��ʾ").c_str(), MB_YESNO);
//	if (Type != 7) {
//		CString CurTrading = L"";
//		Tradings.GetLBText(Tradings.GetCurSel(), CurTrading);
//		json J;
//		try {
//			J = json::parse(JsonStr);
//			if (J["msg"].get<std::string>() == "success") {
//				for (auto &i : J["data"]["result"].items()) {
//					for (auto &t : i.value()["tradingDtoList"].items()) {
//						std::string Symbol = replace_str(t.value()["tradingName"].get<std::string>(), "/", "-");
//						if (Symbol == ws2s(CurTrading.GetBuffer())) {
//							SetConfig("Trading", Symbol);
//							SetConfig("TradingHash", t.value()["hash"]);
//							return;
//						}
//					}
//				}
//
//			}
//		}
//		catch (...) {
//			std::string Caption = GetItemName("�����쳣��������");
//			std::string Title = GetItemName("��ʾ");
//			MessageBoxA(NULL, Caption.c_str(), Title.c_str(), MB_OK);
//			exit(-1);
//		}
//	}
//}

unsigned int __stdcall UpdateAccount(void* lp) {
	LoginWnd * Wnd = (LoginWnd*)lp;
	Wnd->GetDlgItem(IDOK)->SetWindowTextW(GetItemNameW(L"������..."));
	Wnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
	std::string Param = format("\"%s\",\"%s\"", GetConfig("Address").c_str(), GetConfig("Trading").c_str());
	PostServer("CreateNewAccount", Param);
	Wnd->GetDlgItem(IDOK)->SetWindowTextW(L"");
	Wnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
	Wnd->GetDlgItem(IDOK)->SetWindowTextW(GetItemNameW(L"ȷ��"));
	Wnd->GetDlgItem(IDOK)->EnableWindow(TRUE);
	return 0;
}
void LoginWnd::OnCbnSelchangeCombo3()
{

	int Type = MessageBoxA(NULL, GetItemName("�л�����").c_str(), GetItemName("��ʾ").c_str(), MB_YESNO);
	if (Type != 7) {
		CString CurTrading = L"";
		Tradings.GetLBText(Tradings.GetCurSel(), CurTrading);
		json J;
		try {
			J = json::parse(JsonStr);
			if (J["msg"].get<std::string>() == "success") {
				for (auto &i : J["data"]["result"].items()) {
					for (auto &t : i.value()["tradingDtoList"].items()) {
						std::string Symbol = replace_str(t.value()["tradingName"].get<std::string>(), "/", "-");
						if (Symbol == ws2s(CurTrading.GetBuffer())) {
							SetConfig("Trading", Symbol);
							SetConfig("TradingHash", t.value()["hash"]);
							_beginthreadex(NULL, NULL, UpdateAccount, this, 0, 0);
							return;
						}
					}
				}

			}
		}
		catch (...) {
			std::string Caption = GetItemName("�����쳣��������");
			std::string Title = GetItemName("��ʾ");
			MessageBoxA(NULL, Caption.c_str(), Title.c_str(), MB_OK);
			exit(-1);
		}
	}
	else {
		Tradings.SetCurSel(CurIndex);
	}
}
