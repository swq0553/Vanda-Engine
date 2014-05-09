//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// Warnings.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "Warnings.h"

// CWarnings dialog

IMPLEMENT_DYNAMIC(CWarnings, CDialog)

CWarnings::CWarnings(CWnd* pParent /*=NULL*/)
	: CDialog(CWarnings::IDD, pParent)
{

}

CWarnings::~CWarnings()
{
}

void CWarnings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_richEditWarnings);
}


BEGIN_MESSAGE_MAP(CWarnings, CDialog)
END_MESSAGE_MAP()


// CWarnings message handlers

BOOL CWarnings::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_richEditWarnings.LimitText( 2000000 );
	m_richEditWarnings.SetBackgroundColor( FALSE, RGB(50,50,60));

	/////==============Read===============
	CString sReadText; //Where the rich text will be streamed into
	EDITSTREAM esRead;
	esRead.dwCookie = (DWORD)&sReadText; // Pass a pointer to the CString to the callback function 
	esRead.pfnCallback = MEditStreamOutCallback; // Specify the pointer to the callback function.
	ex_pRichEdit2->StreamOut(SF_RTF,esRead); // Perform the streaming
	//====================================

	m_richEditWarnings.SetReadOnly(FALSE);
	m_richEditWarnings.SetSel( 0, -1 );
	m_richEditWarnings.ReplaceSel( sReadText.GetString() );
	m_richEditWarnings.SetReadOnly(TRUE);
	return TRUE;
}


