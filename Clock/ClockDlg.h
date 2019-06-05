
// ClockDlg.h : 头文件
//

#pragma once
#include <process.h>
#include <math.h>
#include "TimeUtility.h"
using namespace std;

#pragma warning(disable:4244)
// CClockDlg 对话框
class CClockDlg : public CDialog
{
// 构造
public:
	CClockDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLOCK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	HANDLE	m_hNewFont;
	afx_msg HCURSOR OnQueryDragIcon();
	RECT	m_rtClock,m_rtDate;
	// 毫秒表位置，已废弃
	RECT	m_rtMinSecond;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	volatile bool m_bThreadTimerRun;
	HANDLE	m_hThreadTimer;
	DWORD	m_dwPriorityClass;
	DWORD	m_dwThreadPriority;
	static LONGLONG	m_dfFreq;
	static LONGLONG	m_dfCounter;
	POINT m_ptOldStart,m_ptOldStop;
	POINT m_ptMinSecOldStart,m_ptMinSecOldStop;
	double m_dfStartTime;
	double m_dfLastPressButton;
	bool m_bRunOnce;
	int		m_nTimeTextSize;
	// 用来比较timeGetTime()与GetExactTime的精度，结果证明GetExactTime更为精确
	static  UINT __stdcall ThreadTest(void *p)
	{
		CClockDlg *pThis = (CClockDlg *)p;
		struct TimeSpanTest
		{
			int		nTimeSpan1;
			int		nTimeSpan2;
		};
		TimeSpanTest TimeSpanArray[500];
		int nCount = 0;
		DWORD dwTime = timeGetTime();
		double dfTime = GetExactTime();

		while(pThis->m_bThreadTimerRun)
		{
			Sleep(5);
			TimeSpanArray[nCount].nTimeSpan1 = (int)(TimeSpanEx(dfTime)*1000);
			TimeSpanArray[nCount].nTimeSpan2 = timeGetTime() - dwTime;
			dfTime = GetExactTime();
			dwTime = timeGetTime();
			
			if (nCount <500 )
				nCount ++;
			else
			{
				TCHAR szText[1024] = {0};
				for (int i = 0;i < 500;i ++)
				{
					_stprintf(szText,_T("%d:%d\t\t"),TimeSpanArray[i].nTimeSpan1,TimeSpanArray[i].nTimeSpan2);
					if ((i+1) % 50 == 0 )
						_tcscat(szText,_T("\r\n"));
					OutputDebugString(szText);
				}
				dfTime = GetExactTime();
				dwTime = timeGetTime();
				nCount = 0;
			}
		}
		return 0;
	}
	// 在矩形区域内绘制表盘
// 	static double GetExactTime()
// 	{
// 		LONGLONG dfCounter1;
// 		LARGE_INTEGER LarInt;
// 		QueryPerformanceCounter(&LarInt);
// 		dfCounter1 = LarInt.QuadPart;
// 		return (double )(dfCounter1 - m_dfCounter) / m_dfFreq;
// 	}

	/* 绘制毫秒表盘，已废弃
	void DrawMinSecFace(CDC *pDC)
	{
		CPen FramePen(PS_SOLID,1,RGB(0,0,255));
		CPen ScalePen(PS_SOLID,1,RGB(0,0,0));
		CGdiObject *pOldBrush = pDC->SelectStockObject(NULL_BRUSH);
		RECT rtFrame = m_rtMinSecond;
		int nCenterX = (rtFrame.right + rtFrame.left)/2;
		int nCenterY = (rtFrame.top + rtFrame.bottom)/2;
		int nR1 = (rtFrame.right - rtFrame.left)/2;
		int nR2 = nR1 - 4;
		int nR3 = nR1 - 8;
		int nR4 = nR1 - 15;
		double dfAngle = 0.0f;
		const double PI = 3.1415926f;
		double dfAngleStep = 2*PI/10;
		TCHAR szText[16] = {0};

		CFont FontDigit;
		FontDigit.CreatePointFont(150, _T("Digiface"), NULL);
		CFont *pOldFont = pDC->SelectObject(&FontDigit);
		CPen *pOldPen = (CPen *)pDC->SelectObject(&ScalePen);

		for (int i =0;i < 10;i ++)
		{
			double dfSin = sin(dfAngle);
			double dfCos = cos(dfAngle);
			dfAngle += dfAngleStep;
			int nr1 =nR1,nr2 = nR2;

			int nStartX = nCenterX + (int)ceil(nR2*dfSin);
			int nStartY = nCenterY - (int)ceil(nR2*dfCos);
			pDC->MoveTo(nStartX,nStartY);
			int nStopX = nCenterX + (int)ceil(nR1*dfSin);
			int nStopY = nCenterY - (int)ceil(nR1*dfCos);
			pDC->LineTo(nStopX,nStopY);
			
			pDC->SelectObject(ScalePen);
			_stprintf(szText,_T("%d"),i);
			int nFontCenterX = nCenterX + (int)(ceil(nR4*dfSin));
			int nFontCenterY = nCenterY - (int)(ceil(nR4*dfCos));
			int nFontLeft = nFontCenterX - 6;
			int nFontTop = nFontCenterY - 6;
			if (dfCos < 0)
				nFontTop = nFontTop -= 6;
			
			pDC->TextOut(nFontLeft,nFontTop,szText);
			
		}

		pDC->SelectObject(FramePen);
		pDC->Ellipse(&m_rtMinSecond);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldFont);
		pDC->SelectObject(pOldPen);
	}
	*/
	void	DrawClockFace(CDC *pDC)
	{
		CPen FramePen(PS_SOLID,1,RGB(0,0,255));
		CPen ScalePen(PS_SOLID,1,RGB(0,0,0));
		CPen ScalePen2(PS_SOLID,2,RGB(255,0,0));
		
		CGdiObject *pOldBrush = pDC->SelectStockObject(NULL_BRUSH);
		//CBrush  BrushCenter(RGB(0,0,0));
		RECT rtFrame = m_rtClock;
		int nCenterX = (rtFrame.right + rtFrame.left)/2;
		int nCenterY = (rtFrame.top + rtFrame.bottom)/2;
		int nR1 = (rtFrame.right - rtFrame.left)/2;
		int nR2 = nR1 - 5;
		int nR3 = nR1 - 10;
		int nR4 = nR1 - 25;
		double dfAngle = 0.0f;
		const double PI = 3.1415926f;
		double dfAngleStep = 2*PI/100;
		CFont FontDigit;
		FontDigit.CreatePointFont(150, _T("Digiface"), NULL);
		CFont *pOldFont = pDC->SelectObject(&FontDigit);
		CPen *pOldPen = (CPen *)pDC->SelectObject(&ScalePen);
		
		// 把360度的圆分成100等份
		// 计算每个点的位置
		TCHAR szText[16] = {0};
		for (int i = 0;i < 100; i ++)
		{
			double dfSin = sin(dfAngle);
			double dfCos = cos(dfAngle);
			dfAngle += dfAngleStep;
			int nr1 =nR1,nr2 = nR2;
			
			if (i % 10 == 0)
			{
				nr2 = nR3;
				pDC->SelectObject(ScalePen2);
			}
			
			int nStartX = nCenterX + (int)ceil(nr2*dfSin);
			int nStartY = nCenterY - (int)ceil(nr2*dfCos);
			pDC->MoveTo(nStartX,nStartY);
			int nStopX = nCenterX + (int)ceil(nr1*dfSin);
			int nStopY = nCenterY - (int)ceil(nr1*dfCos);
			pDC->LineTo(nStopX,nStopY);
			if (i % 10 == 0)
			{
				pDC->SelectObject(ScalePen);
				_stprintf(szText,_T("%d"),i/10);
				int nFontCenterX = nCenterX + (int)(ceil(nR4*dfSin));
				int nFontCenterY = nCenterY - (int)(ceil(nR4*dfCos));
				int nFontLeft = nFontCenterX - 6;
				int nFontTop = nFontCenterY - 6;
				if (dfCos < 0)
					nFontTop = nFontTop -= 6;
				pDC->TextOut(nFontLeft,nFontTop,szText);
			}
		}

		pDC->SelectObject(FramePen);
		pDC->Ellipse(&m_rtClock);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldFont);
		pDC->SelectObject(&pOldPen);
	}

	static void  DrawLine(CDC *pDC,CPen *pPen,POINT ptStart,POINT ptStop)
	{
		pDC->SelectObject(pPen);
		pDC->MoveTo(ptStart);
		pDC->LineTo(ptStop);
	}

	void	DrawTime(CDC *pDC, double dfTime,double dfLastTime)
	{
		RECT rtFrame = m_rtClock;
		int nCenterX = (rtFrame.right + rtFrame.left)/2;
		int nCenterY = (rtFrame.top + rtFrame.bottom)/2;
		int nR1 = (rtFrame.right - rtFrame.left)/2;		// 表盘半径
		int nR2 = nR1 - 40;								// 长指针半径
		int nR3 = nR1 - 180;							// 短指针半径
		const double PI = 3.1415926f;
		double dfAngleStep = 2*PI/100;
		CFont FontDigit;
		FontDigit.CreatePointFont(m_nTimeTextSize, _T("Digiface"), NULL);

		if (dfLastTime != 0.0f)
		{// 擦除旧指针
			CPen PenErase(PS_SOLID,2,GetSysColor(COLOR_BTNFACE));
			DrawLine(pDC,&PenErase,m_ptOldStart,m_ptOldStop);
			//DrawLine(pDC,&PenErase,m_ptMinSecOldStart,m_ptMinSecOldStop);
		}

		// 计算大盘中心圆点
		RECT rtCenter1;
		rtCenter1.left = nCenterX - 2;
		rtCenter1.right = nCenterX + 2;
		rtCenter1.top = nCenterY - 2;
		rtCenter1.bottom = nCenterY + 2;

		// 计算大盘新指针
		double dfMinSecond = dfTime - (UINT64)dfTime;
		int nMinSecond = (int)(dfMinSecond*100);
		double dfAngle =  nMinSecond*dfAngleStep;
		m_ptOldStart.x =  nCenterX + (int)(ceil(nR2*sin(dfAngle)));
		m_ptOldStart.y = nCenterY - (int)ceil(nR2*cos(dfAngle));
		dfAngle += PI;
		m_ptOldStop.x =  nCenterX + (int)(ceil(nR3*sin(dfAngle)));
		m_ptOldStop.y = nCenterY - (int)ceil(nR3*cos(dfAngle));

		// 计算小盘指针
		//rtFrame = m_rtMinSecond;

		nCenterX = (rtFrame.right + rtFrame.left)/2;
		nCenterY = (rtFrame.top + rtFrame.bottom)/2;
		nR1 = (rtFrame.right - rtFrame.left)/2;		// 表盘半径
		nR2 = nR1 - 30;								// 长指针半径
		nR3 = nR1 - 80;								// 短指针半径
		dfAngleStep = 2*PI/10;

		// 计算小盘新指针
		dfMinSecond = dfTime - (UINT64)dfTime;
		nMinSecond = ((int)(dfMinSecond*1000))%10;
		dfAngle =  nMinSecond*dfAngleStep;
		m_ptMinSecOldStart.x =  nCenterX + (int)(ceil(nR2*sin(dfAngle)));
		m_ptMinSecOldStart.y = nCenterY - (int)ceil(nR2*cos(dfAngle));
		dfAngle += PI;
		m_ptMinSecOldStop.x =  nCenterX + (int)(ceil(nR3*sin(dfAngle)));
		m_ptMinSecOldStop.y = nCenterY - (int)ceil(nR3*cos(dfAngle));
		
		CPen PenPoint(PS_SOLID,2,RGB(0,0,0));
		DrawLine(pDC,&PenPoint,m_ptOldStart,m_ptOldStop);
		//DrawLine(pDC,&PenPoint,m_ptMinSecOldStart,m_ptMinSecOldStop);

		// 计算小盘中心圆点
		RECT rtCenter2;
		rtCenter2.left = nCenterX - 4;
		rtCenter2.right = nCenterX + 4;
		rtCenter2.top = nCenterY - 4;
		rtCenter2.bottom = nCenterY + 4;
		
		CBrush  BrushCenter(RGB(0,0,0));
		pDC->SelectObject(&BrushCenter);
		pDC->Ellipse(&rtCenter1);
		//pDC->Ellipse(&rtCenter2);

	}

	static  UINT _stdcall ThreadTimer(void *p)
	{
		CClockDlg *pThis = (CClockDlg *)p;
		TCHAR szText[64] = {0};
		CDC *pDC = pThis->GetDC();
		COLORREF olcBkColor = pDC->GetBkColor();
		COLORREF olcTextColor = pDC->GetTextColor();
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		pDC->SetTextColor(RGB(0,0,0));
		
		pThis->DrawClockFace(pDC);
		//pThis->DrawMinSecFace(pDC);
		// 控除旧指针
		if (pThis->m_bRunOnce)
		{
			CPen PenErase(PS_SOLID,2,GetSysColor(COLOR_BTNFACE));
			DrawLine(pDC,&PenErase,pThis->m_ptOldStart,pThis->m_ptOldStop);
		}

		CFont NewFontTime;
		NewFontTime.CreatePointFont(pThis->m_nTimeTextSize, _T("Digiface"), NULL);
		CFont *pOldFont = (CFont *)pDC->SelectObject(&NewFontTime);
		double dfTLast = 0.0f;
		double dfTLast2 = GetExactTime();

// 		if (pThis->IsDlgButtonChecked(IDC_CHECK_TIME) == BST_CHECKED)
// 		{
// 			SYSTEMTIME dstSysTime;
// 			GetSystemTime(&dstSysTime);
// 			TCHAR szTime[32] = {0};
// 			pThis->GetDlgItemText(IDC_DATETIMEPICKER1,szTime,32);
// 			TCHAR szTime2[8] = {0};
// 			// 01234567
// 			// 00:00:00
// 			_tcsncpy(szTime2,szTime,2);
// 			dstSysTime.wHour = (WORD)_ttoi(szTime2) - 8;
// 			_tcsncpy(szTime2,&szTime[3],2);
// 			dstSysTime.wMinute = (WORD)_ttoi(szTime2);
// 			_tcsncpy(szTime2,&szTime[6],2);
// 			dstSysTime.wSecond = (WORD)_ttoi(szTime2);
// 			dstSysTime.wMilliseconds = 0;
// 			FILETIME ft;
// 			SystemTimeToFileTime(&dstSysTime,&ft);
// 			LARGE_INTEGER li;
// 			li.LowPart = ft.dwLowDateTime;
// 			li.HighPart = ft.dwHighDateTime;
// 			// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
// 			UINT64 Time64 = (li.QuadPart - EPOCHFILETIME) /10;
// 			while(pThis->m_bThreadTimerRun)
// 			{
// 				UINT64 tNow = ETB::GetSysTimeMicros();
// 				if (tNow >= Time64)
// 					break;
// 			}
// 		}

		pThis->m_dfStartTime = GetExactTime();
		while(pThis->m_bThreadTimerRun)
		{
			double dfTNow = GetExactTime();	
			// 刷新毫秒表盘，废弃
// 			if ((dfTNow - dfTLast2) >= 0.010f)
// 			{
// 				pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
// 				pDC->SetTextColor(RGB(0,0,0));
// 				//pThis->DrawMinSecFace(pDC);
// 				dfTLast2 = dfTNow;
// 			}
	
			if (TimeSpanEx(dfTLast) > 0.002f)
			{
				double dfTimeSpan = TimeSpanEx(pThis->m_dfStartTime);
				int nHour = dfTimeSpan/3600;
				int nMinute = (dfTimeSpan - nHour*3600)/60;
				int nSecond = dfTimeSpan - nHour*3600 - nMinute*60;
				int dfMinSec = (dfTimeSpan - (UINT64)dfTimeSpan)*1000;

				_stprintf(szText,_T("%02d:%02d:%02d.%03d"),nHour,nMinute,nSecond,dfMinSec);
				pDC->TextOut(pThis->m_rtDate.left,pThis->m_rtDate.top,szText);
				pThis->DrawTime(pDC,dfTimeSpan,dfTLast);
				dfTLast = dfTNow;
			}
			Sleep(2);
		}
		pDC->SelectObject(pOldFont);
		return 0;
	}
	void   InitClockFace()
	{
		CDC *pDC = GetDC();
		COLORREF olcBkColor = pDC->GetBkColor();
		COLORREF olcTextColor = pDC->GetTextColor();
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		pDC->SetTextColor(RGB(0,0,0));

		DrawClockFace(pDC);
		//DrawMinSecFace(pDC);
		// 控除旧指针
		if (m_bRunOnce)
		{
			CPen PenErase(PS_SOLID,2,GetSysColor(COLOR_BTNFACE));
			DrawLine(pDC,&PenErase,m_ptOldStart,m_ptOldStop);
		}

		CFont NewFontTime;
		NewFontTime.CreatePointFont(m_nTimeTextSize, _T("Digiface"), NULL);
		CFont *pOldFont = (CFont *)pDC->SelectObject(&NewFontTime);
		
		CString strText = _T("00:00:00.000");
		pDC->TextOut(m_rtDate.left,m_rtDate.top,strText);
		DrawTime(pDC,0,0);
	
		pDC->SelectObject(pOldFont);
	}
	afx_msg void OnDestroy();
};
