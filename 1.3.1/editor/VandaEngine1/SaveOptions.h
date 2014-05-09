//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSaveOptions dialog

class CSaveOptions : public CDialog
{
	DECLARE_DYNAMIC(CSaveOptions)

public:
	CSaveOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveOptions();

// Dialog Data
	enum { IDD = IDD_DIALOG_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
	CRichEditCtrl m_editBoxDestination;
	CString m_strDestination;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBtnDestination();
	afx_msg void OnEnChangeEditSetpassword();
	CEdit m_editBoxPassword;
	afx_msg void OnBnClickedBtnGenerateRandomString();
};
