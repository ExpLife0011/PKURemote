// PKURemoteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PKURemote.h"
#include "PKURemoteDlg.h"
#include "PKURemoteUtil.h"

#include "CredData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_NC (WM_USER + 1001)

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPKURemoteDlg 对话框




CPKURemoteDlg::CPKURemoteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPKURemoteDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPKURemoteDlg::~CPKURemoteDlg()
{
	RemoveTray();
}

void CPKURemoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_README, m_staticReadMe);
	DDX_Control(pDX, ID_BTN_START, m_btnStart);
	DDX_Control(pDX, ID_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_STATIC_STATE, m_staticState);
}

BEGIN_MESSAGE_MAP(CPKURemoteDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BTN_START, &CPKURemoteDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(ID_BTN_STOP, &CPKURemoteDlg::OnBnClickedBtnStop)
	ON_MESSAGE(WM_NC, &CPKURemoteDlg::OnTrayNotify)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPKURemoteDlg 消息处理程序

BOOL CPKURemoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_staticReadMe.SetWindowText(READ_ME);
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	m_btnStop.EnableWindow(FALSE);
	m_staticState.SetWindowText(_T(""));

	AddTray();

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPKURemoteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPKURemoteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CPKURemoteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPKURemoteDlg::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	int iSelected = 0;
	int RadioState;

	if ((RadioState = ((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck()) == 1)
	{
		iSelected = 0;
	}
	else if ((RadioState = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck()) == 1)
	{
		iSelected = 1;
	}
	else if ((RadioState = ((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck()) == 1)
	{
		iSelected = 2;
	}
	else if ((RadioState = ((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck()) == 1)
	{
		iSelected = 3;
	}

	if (startWindivert(iSelected, 2) == 0)
		m_staticState.SetWindowText(_T("连接成功！！"));
	else
		m_staticState.SetWindowText(_T("连接失败！！请确认是否以管理员运行！"));

	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);

	((CButton *)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO4))->EnableWindow(FALSE);
}

void CPKURemoteDlg::OnBnClickedBtnStop()
{
	// TODO: Add your control notification handler code here
	if (stopWindivert() == 0)
		m_staticState.SetWindowText(_T("断开成功！！"));
	else
		m_staticState.SetWindowText(_T("断开失败！！"));

	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);

	((CButton *)GetDlgItem(IDC_RADIO1))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO2))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO3))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO4))->EnableWindow(TRUE);
}

void CPKURemoteDlg::AddTray()
{ 
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hWnd = m_hWnd;
	lstrcpy(m_nid.szTip,_T("PKURemote"));
	m_nid.uCallbackMessage = WM_NC;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &m_nid);
}

void CPKURemoteDlg::RemoveTray()
{ 
	m_nid.hIcon = NULL;
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

LRESULT CPKURemoteDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = (UINT) lParam;
	switch(uMsg)
	{
	case WM_RBUTTONUP:
		{
			//右键处理
			CMenu menuTray;
			CPoint point;
			int id;
			GetCursorPos(&point);

			menuTray.LoadMenu(IDR_MENU_TRAY);
			id = menuTray.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			switch (id){
			case IDR_TRAY_EXIT:
				OnOK();
				break;
			case IDR_TRAY_RESTORE:
				//窗口前端显示
				SetForegroundWindow();
				ShowWindow(SW_SHOWNORMAL);
				break;
			default:
				break;
			}
			break;
		}
		
	case WM_LBUTTONDBLCLK:
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			SetForegroundWindow();
			ShowWindow(SW_SHOWNORMAL);
		}
		
		break;
	default:
		break;
	}
	return 0;
}

void CPKURemoteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);//隐藏最小化到任务栏的图标
	}

}

