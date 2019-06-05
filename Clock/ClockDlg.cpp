
// ClockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Clock.h"
#include "ClockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClockDlg 对话框



LONGLONG	CClockDlg::m_dfFreq;
LONGLONG	CClockDlg::m_dfCounter;

CClockDlg::CClockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClockDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClockDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CClockDlg::OnBnClickedButtonStart)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#define		GetDlgItemRect(nID,Rt)	{	GetDlgItem(nID)->GetWindowRect(&Rt);	ScreenToClient(&Rt);}
// CClockDlg 消息处理程序

HANDLE InstallFont()
{
	HINSTANCE hResInstance = AfxGetResourceHandle();
	HRSRC	hRes = FindResource(hResInstance,MAKEINTRESOURCE(IDR_FONT_DIGITAL),_T("SFONT"));
	if (hRes)
	{
		HGLOBAL hMem = LoadResource(hResInstance,hRes);
		void *pData = LockResource(hMem);
		DWORD dwLen = SizeofResource(hResInstance,hRes);
		DWORD nFonts;
		HANDLE hFont = AddFontMemResourceEx(pData,	// 字体数据
										   dwLen,	// 字体数据尺寸
										   NULL,	// 保留参数，必须为0
										   &nFonts);// 成功安装字体数
		if (hFont)
			return hFont;
		else
			return NULL;
	}
	else
		return NULL;
}

BOOL CClockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化高精度计时器
	LARGE_INTEGER LarInt;		
	QueryPerformanceFrequency(&LarInt);	
	m_dfFreq = LarInt.QuadPart;
	QueryPerformanceCounter(&LarInt);
	m_dfCounter = LarInt.QuadPart;

	GetDlgItem(IDC_STATIC_CLOCK)->GetWindowRect(&m_rtClock);
	GetDlgItem(IDC_STATIC_TIME)->GetWindowRect(&m_rtDate);
	//GetDlgItem(IDC_STATIC_MINSECOND)->GetWindowRect(&m_rtMinSecond);

	// 毫秒表盘的调试与宽度，已经废弃
// 	int nMinSecondWith = 150;
// 	int nMinSecondHeight = 150;

	// 日期输出框与时钟框上边距
	int nDiffDate = m_rtDate.top - m_rtClock.top;	
	int nOldWidth = m_rtClock.right - m_rtClock.left;
	int nOldHeight = m_rtClock.bottom - m_rtClock.top;
		
	int nNewWidth = 600;
	int nNewHeight = 600;
	int nWidthDiff = nNewWidth - nOldWidth;
	int nHeightDiff = nNewHeight - nOldHeight;

	m_rtClock.right = m_rtClock.left + nNewWidth;
	m_rtClock.bottom = m_rtClock.top + nNewHeight;
	int nCenterClockX = (m_rtClock.left + m_rtClock.right)/2;
	int nCenterClockY = (m_rtClock.top + m_rtClock.bottom)/2;
	int nRClock = nNewHeight/2;		// 表盘半径
	// 时间的显示位置应该在表盘中心下方，离中心位置为表盘半径的一半

	// 计算时间输出框的位置
	int nWidthDate = m_rtDate.right - m_rtDate.left;
	int nHeightDate = m_rtDate.bottom - m_rtDate.top;
	m_rtDate.top = nCenterClockY + nRClock/2-nHeightDate/2;
	m_rtDate.bottom = m_rtDate.top + nHeightDate;
	m_rtDate.left = nCenterClockX - nWidthDate/2;
	m_rtDate.right = nCenterClockX + nWidthDate/2;
	ScreenToClient(&m_rtDate);
	GetDlgItem(IDC_STATIC_TIME)->MoveWindow(&m_rtDate);
	
	RECT rtWindow;	
	GetWindowRect(&rtWindow);
	RECT rtButton;
	GetDlgItem(IDC_BUTTON_START)->GetWindowRect(&rtButton);
	// 启动按钮与时钟框下边距
	int nButtonDiff = rtWindow.bottom - rtButton.bottom;
	int nWidthButton = rtButton.right - rtButton.left;
	int nHeightButton = rtButton.bottom - rtButton.top;

	rtWindow.right += nWidthDiff;
	rtWindow.bottom += nHeightDiff;
	int nCenterX = (rtWindow.left + rtWindow.right)/2;
	int nCenterY = (rtWindow.top + rtWindow.bottom)/2;
	// 计算启动按钮位置
	rtButton.bottom = rtWindow.bottom - nButtonDiff;
	rtButton.top = rtButton.bottom - nHeightButton;
	rtButton.left = nCenterX - nWidthButton/2 ;
	rtButton.right = nCenterX + nWidthButton/2 ;
	ScreenToClient(&rtButton);
	GetDlgItem(IDC_BUTTON_START)->MoveWindow(&rtButton);

	MoveWindow(&rtWindow);
	ScreenToClient(&m_rtClock);
	GetDlgItem(IDC_STATIC_CLOCK)->MoveWindow(&m_rtClock);
//  调整毫秒表盘位置，已废弃
// 	m_rtMinSecond.left = nCenterX - nMinSecondWith/2;
// 	m_rtMinSecond.right = nCenterX + nMinSecondWith/2;
// 	m_rtMinSecond.top = m_rtMinSecond.bottom - nMinSecondHeight;
// 	ScreenToClient(&m_rtMinSecond);
// 	GetDlgItem(IDC_STATIC_MINSECOND)->MoveWindow(&m_rtMinSecond);

	m_bThreadTimerRun = false;
	m_hThreadTimer = NULL;
	m_bRunOnce = false;
	m_nTimeTextSize = nNewWidth*2/3;
	m_hNewFont = InstallFont();
	if (!m_hNewFont)
	{
		AfxMessageBox(_T("加载字体失败"));
	}
	m_dfLastPressButton = 0.0f;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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

		InitClockFace();
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClockDlg::OnBnClickedButtonStart()
{
	if (!m_bRunOnce)
		m_dfStartTime = GetExactTime();
	m_bRunOnce = true;
	HANDLE hProcess			= GetCurrentProcess();
	if (m_hThreadTimer)
	{
		m_bThreadTimerRun = false;
		// 恢复线程和进程的优先级
		SetThreadPriority(m_hThreadTimer, m_dwThreadPriority);
		SetPriorityClass(hProcess, m_dwPriorityClass);

		WaitForSingleObject(m_hThreadTimer,INFINITE);
		CloseHandle(m_hThreadTimer);
		m_hThreadTimer = NULL;
		SetDlgItemText(IDC_BUTTON_START,_T("开始计时"));
		double dfT = TimeSpanEx(m_dfLastPressButton);
		AfxTrace("%s Timespan of PressButton = %.3f.\n",__FUNCTION__,dfT);
	}
	else
	{
		m_dfLastPressButton = GetExactTime();
		m_bThreadTimerRun = true;
		m_hThreadTimer = (HANDLE)_beginthreadex(NULL,0,ThreadTimer,this,CREATE_SUSPENDED,NULL);
		//m_hThreadTimer = (HANDLE)_beginthreadex(NULL,0,ThreadTest,this,CREATE_SUSPENDED,NULL);
		m_dwPriorityClass	= GetPriorityClass(hProcess);
		m_dwThreadPriority	= GetThreadPriority(m_hThreadTimer);	

		m_dwPriorityClass	= GetPriorityClass(hProcess);		
		m_dwThreadPriority	= GetThreadPriority(m_hThreadTimer);

		DWORD dwError			= 0;
		// 把进程优先级调整到实时级
		if (!SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS))
		{
			dwError = GetLastError();
			if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
			{
				dwError = GetLastError();
				if (!SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS))
				{
					dwError = GetLastError();
				}
			}
		}
		// 把线程优先级调整到实时级
		if (!SetThreadPriority(m_hThreadTimer, THREAD_PRIORITY_TIME_CRITICAL))
		{
			dwError = GetLastError();
			if (!SetThreadPriority(m_hThreadTimer, THREAD_PRIORITY_HIGHEST))
			{
				dwError = GetLastError();
				if (!SetThreadPriority(m_hThreadTimer, THREAD_PRIORITY_ABOVE_NORMAL))
				{
					dwError = GetLastError();
				}
			}
		}
		ResumeThread(m_hThreadTimer);
		SetDlgItemText(IDC_BUTTON_START,_T("停止计时"));
	}
}

void CClockDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_hThreadTimer)
	{
		m_bThreadTimerRun = false;
		WaitForSingleObject(m_hThreadTimer,INFINITE);
		CloseHandle(m_hThreadTimer);
		m_hThreadTimer = NULL;
		//SetDlgItemText(IDC_BUTTON_START,_T("开始计时"));
	}

	if (m_hNewFont)
		RemoveFontMemResourceEx(m_hNewFont);
}
