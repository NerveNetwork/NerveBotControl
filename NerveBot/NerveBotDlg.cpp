
// NerveBotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NerveBot.h"
#include "NerveBotDlg.h"
#include "afxdialogex.h"
#include "Login.h"
#include "IniSettingWnd.h"
#include "MineManagerWnd.h"
#include "IniWndStatus.h"
#include "libcurl.h"
#include "Language.h"
#include "Config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int LoginState;
extern int CurrentLanguageId;
CNerveBotDlg *MainWnd = NULL;
bool bMineManagerAccount = false;//ʶ��ǰ��¼���˻��Ƿ�Ϊ��ع����ַ
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNerveBotDlg �Ի���



CNerveBotDlg::CNerveBotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NERVEBOT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CNerveBotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, MainTable);
	DDX_Control(pDX, IDC_COMBO2, LanguageList);
}

BEGIN_MESSAGE_MAP(CNerveBotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CNerveBotDlg::OnTcnSelchangeTab1)
//	ON_CBN_SELCHANGE(IDC_COMBO2, &CNerveBotDlg::OnCbnSelchangeCombo2)
	ON_CBN_EDITCHANGE(IDC_COMBO2, &CNerveBotDlg::OnCbnEditchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CNerveBotDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CNerveBotDlg ��Ϣ�������

BOOL CNerveBotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	IniLibCurl();
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


	ShowLoginWnd(NULL);
	::SetWindowPos(this->m_hWnd, HWND_TOP, 5000, 0, 0, 0, SWP_NOSIZE);
	MainWnd = this;
	MainTable.InsertItem(0, GetItemNameW(L"��ǰ�ҵ�"));
	MainTable.InsertItem(1, L"");
	MainTable.SetDlgItemTextW(1, L"02126564");
	
	CRect Ret;
	MainTable.GetClientRect(&Ret);
	Ret.top += 22;
	Ret.bottom -= 2;
	Ret.left += 1;
	Ret.right -= 4;
	MyOrderWnd.Create(IDD_DIALOG1, &MainTable);
	MyOrderWnd.MoveWindow(&Ret);
	MyOrderWnd.ShowWindow(true);

	MyMineManagerWnd.Create(IDD_DIALOG4, &MainTable);
	MyMineManagerWnd.MoveWindow(&Ret);
	MyMineManagerWnd.ShowWindow(false);


	_beginthreadex(NULL, NULL, IniWndStatus, NULL, 0, 0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CNerveBotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNerveBotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNerveBotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int TimesOfPaint = 0;
LRESULT CNerveBotDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���


	

	return CDialogEx::DefWindowProc(message, wParam, lParam);
	
}


BOOL CNerveBotDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	//if (message == WM_SHOWWINDOW) {
	//	AfxMessageBox(L"ss");
	//	return TRUE;
	//	TimesOfPaint++;
	//}
	//else
	//{
	//	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
	//}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void CNerveBotDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int cur_sel = MainTable.GetCurSel();
	switch (cur_sel)
	{
	case 0:
		MyOrderWnd.ShowWindow(true);
		MyMineManagerWnd.ShowWindow(false);


		break;
	case 1:
		//if (bMineManagerAccount == false) {
		//	AfxMessageBox(GetItemNameW(L"�ǿ�ع����ַ��"));
		//	break;
		//}
		MyOrderWnd.ShowWindow(false);
		MyMineManagerWnd.ShowWindow(true);

		break;

	default:
		break;
	}
	*pResult = 0;
}


//void CNerveBotDlg::OnCbnSelchangeCombo2()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CString Lang = L"";
//	LanguageList.GetLBText(LanguageList.GetCurSel(), Lang);
//
//	if (Lang == L"English") {
//		SetConfig("Language", "1");
//	}
//	else {
//		SetConfig("Language", "0");
//	}
//	WinExec("NerveBot.exe", SW_HIDE);
//	exit(-1);
//}


void CNerveBotDlg::OnCbnEditchangeCombo2()
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


void CNerveBotDlg::OnCbnSelchangeCombo2()
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
