//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "afxcmn.h"


// CWarnings dialog

class CWarnings : public CDialog
{
	DECLARE_DYNAMIC(CWarnings)

public:
	CWarnings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWarnings();

// Dialog Data
	enum { IDD = IDD_DIALOG_WARNINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_richEditWarnings;
	virtual BOOL OnInitDialog();
};
