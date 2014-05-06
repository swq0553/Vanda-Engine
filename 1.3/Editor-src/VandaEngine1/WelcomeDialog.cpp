//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// WelcomeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "WelcomeDialog.h"


// CWelcomeDialog dialog

IMPLEMENT_DYNAMIC(CWelcomeDialog, CDialog)

CWelcomeDialog::CWelcomeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWelcomeDialog::IDD, pParent)
{

}

CWelcomeDialog::~CWelcomeDialog()
{
}

void CWelcomeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_HELP, m_btnVisitHelp);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_CHECK, m_btnCheck);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_UNCHECK, m_btnUncheck);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_VIDEO, m_btnVideo);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_TUTORIALS, m_btnTutorials);
	DDX_Control(pDX, IDC_WELCOME_BUTTON_VANDA_OVERVIEW, m_btnOverviewToVandaEngine1);
}


BEGIN_MESSAGE_MAP(CWelcomeDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_HELP, &CWelcomeDialog::OnBnClickedWelcomeButtonHelp)
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_CHECK, &CWelcomeDialog::OnBnClickedWelcomeButtonCheck)
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_UNCHECK, &CWelcomeDialog::OnBnClickedWelcomeButtonUncheck)
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_VIDEO, &CWelcomeDialog::OnBnClickedWelcomeButtonVideo)
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_TUTORIALS, &CWelcomeDialog::OnBnClickedWelcomeButtonTutorials)
	ON_BN_CLICKED(IDC_WELCOME_BUTTON_VANDA_OVERVIEW, &CWelcomeDialog::OnBnClickedWelcomeButtonVandaOverview)
END_MESSAGE_MAP()


// CWelcomeDialog message handlers

BOOL CWelcomeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnVisitHelp.LoadBitmaps( IDB_BITMAP_WELCOME_VISIT_HELP_UP, IDB_BITMAP_WELCOME_VISIT_HELP_DOWN, IDB_BITMAP_WELCOME_VISIT_HELP_FOCUS );
	m_btnVisitHelp.ShowWindow( SW_SHOW );
	m_btnVisitHelp.UpdateWindow();

	m_btnVideo.LoadBitmaps( IDB_BITMAP_WELCOME_VIDEO_UP, IDB_BITMAP_WELCOME_VIDEO_DOWN, IDB_BITMAP_WELCOME_VIDEO_FOCUS );
	m_btnVideo.ShowWindow( SW_SHOW );
	m_btnVideo.UpdateWindow();

	m_btnOverviewToVandaEngine1.LoadBitmaps( IDB_BITMAP_OVERVIEW_UP, IDB_BITMAP_OVERVIEW_DOWN, IDB_BITMAP_OVERVIEW_FOCUS );
	m_btnOverviewToVandaEngine1.ShowWindow( SW_SHOW );
	m_btnOverviewToVandaEngine1.UpdateWindow();

	m_btnTutorials.LoadBitmaps( IDB_BITMAP_WELCOME_TUTORIAL_UP, IDB_BITMAP_WELCOME_TUTORIAL_DOWN, IDB_BITMAP_WELCOME_TUTORIAL_FOCUS );
	m_btnTutorials.ShowWindow( SW_SHOW );
	m_btnTutorials.UpdateWindow();


	m_btnUncheck.LoadBitmaps( IDB_BITMAP_WELCOME_UNCHECK );
	m_btnUncheck.ShowWindow( SW_SHOW );
	m_btnUncheck.UpdateWindow();

	m_btnCheck.LoadBitmaps( IDB_BITMAP_WELCOME_CHECK );
	m_btnCheck.ShowWindow( SW_HIDE );
	m_btnCheck.UpdateWindow();

	m_btnOK.ShowWindow( SW_SHOW);
	m_btnOK.UpdateWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWelcomeDialog::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    CBrush myBrush( RGB(40,40,40) );    // dialog background color
    CBrush *pOld = pDC->SelectObject(&myBrush);
    BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);

    pDC->SelectObject(pOld);    // restore old brush

    return bRes;                       // CDialog::OnEraseBkgnd(pDC);

	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CWelcomeDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr  = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (pWnd->GetDlgCtrlID())
	{
	  case IDC_STATIC:
		   pDC->SetTextColor (RGB(222, 122, 5));
		   pDC->SetBkColor (RGB(40,40,40)); 
		   hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		   break;

	  default:
		   hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
		   break;
	}
	return hbr;
}

void CWelcomeDialog::OnBnClickedWelcomeButtonHelp()
{
		ShellExecute(NULL, "open", "Assets\\Doc\\vanda.chm", NULL, NULL, SW_SHOWNORMAL);
		//ShellExecute(NULL, "open", "http://vandaengine.com/vandaengine/Doc/Reference/", NULL, NULL, SW_SHOWNORMAL);
}

void CWelcomeDialog::OnBnClickedWelcomeButtonCheck()
{
	m_btnCheck.ShowWindow( SW_HIDE );
	m_btnCheck.UpdateWindow();

	m_btnUncheck.ShowWindow( SW_SHOW );
	m_btnUncheck.UpdateWindow();

	g_options.m_showStartupDialog = CTrue;
}

void CWelcomeDialog::OnBnClickedWelcomeButtonUncheck()
{
	m_btnUncheck.ShowWindow( SW_HIDE );
	m_btnUncheck.UpdateWindow();
	g_options.m_showStartupDialog = CFalse;

	m_btnCheck.ShowWindow( SW_SHOW );
	m_btnCheck.UpdateWindow();
}


void CWelcomeDialog::OnBnClickedWelcomeButtonVideo()
{
	ShellExecute(NULL, "open", "http://vandaengine.com/an-overview-to-vanda-engine-rti/", NULL, NULL, SW_SHOWNORMAL);
}

void CWelcomeDialog::OnBnClickedWelcomeButtonTutorials()
{
	ShellExecute(NULL, "open", "http://vandaengine.com/tutorials/", NULL, NULL, SW_SHOWNORMAL);
}

void CWelcomeDialog::OnBnClickedWelcomeButtonVandaOverview()
{
	ShellExecute(NULL, "open", "http://vandaengine.com/an-overview-to-vanda-engine/", NULL, NULL, SW_SHOWNORMAL);
}
