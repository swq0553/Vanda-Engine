//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// SaveOptions.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "SaveOptions.h"


// CSaveOptions dialog

IMPLEMENT_DYNAMIC(CSaveOptions, CDialog)

CSaveOptions::CSaveOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveOptions::IDD, pParent)
{

}

CSaveOptions::~CSaveOptions()
{
}

void CSaveOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DESTINATION, m_editBoxDestination);
	DDX_Control(pDX, IDC_EDIT_SETPASSWORD, m_editBoxPassword);
}


BEGIN_MESSAGE_MAP(CSaveOptions, CDialog)
	ON_BN_CLICKED(IDC_BTN_DESTINATION, &CSaveOptions::OnBnClickedBtnDestination)
	ON_EN_CHANGE(IDC_EDIT_SETPASSWORD, &CSaveOptions::OnEnChangeEditSetpassword)
	ON_BN_CLICKED(IDC_BTN_GENERATE_RANDOM_STRING, &CSaveOptions::OnBnClickedBtnGenerateRandomString)
END_MESSAGE_MAP()

void CSaveOptions::OnOK()
{
	if( m_strPassword.IsEmpty() || m_strDestination.IsEmpty() )
	{
		MessageBox( "Please enter the valid data for all the required fields", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	else
		CDialog::OnOK();
}


void CSaveOptions::OnBnClickedBtnDestination()
{
	CInt result;
	CFileDialog dlgSave(FALSE, _T("*.vin"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
		_T("File (*.vin)|*.vin||"), this, NULL);
	result = dlgSave.DoModal();
	if( result == IDOK )
	{
		m_strDestination = (CString)dlgSave.GetPathName();
		m_editBoxDestination.SetWindowTextA( m_strDestination );
	}
}




void CSaveOptions::OnEnChangeEditSetpassword()
{
	m_editBoxPassword.GetWindowTextA( m_strPassword );
}

void CSaveOptions::OnBnClickedBtnGenerateRandomString()
{
	static string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890=*&^%$#@!~-+)(_";
    string result;
    result.resize(100);

    for (int i = 0; i < 100; i++)
        result[i] = charset[rand() % charset.length()];
	m_editBoxPassword.SetWindowTextA( result.c_str() );
	m_strPassword = result.c_str();

}
