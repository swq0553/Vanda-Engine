//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "custombitmapbutton.h"
#include "custombutton.h"
#include "afxwin.h"

// CWelcomeDialog dialog

class CWelcomeDialog : public CDialog
{
	DECLARE_DYNAMIC(CWelcomeDialog)

public:
	CWelcomeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWelcomeDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CCustomBitmapButton m_btnVisitHelp;
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CCustomButton m_btnOK;
	afx_msg void OnBnClickedWelcomeButtonHelp();
	CCustomBitmapButton m_btnCheck;
	CCustomBitmapButton m_btnUncheck;
	afx_msg void OnBnClickedWelcomeButtonCheck();
	afx_msg void OnBnClickedWelcomeButtonUncheck();
	CCustomBitmapButton m_btnVideo;
	CCustomBitmapButton m_btnTutorials;
	afx_msg void OnBnClickedWelcomeButtonVideo();
	afx_msg void OnBnClickedWelcomeButtonTutorials();
	CCustomBitmapButton m_btnOverviewToVandaEngine1;
	afx_msg void OnBnClickedWelcomeButtonVandaOverview();
};
