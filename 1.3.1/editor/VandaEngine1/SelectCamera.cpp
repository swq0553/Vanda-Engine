//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// SelectCamera.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "SelectCamera.h"
#include  "AFXPRIV.H"

// CSelectCamera dialog

IMPLEMENT_DYNAMIC(CSelectCamera, CDialog)

CSelectCamera::CSelectCamera(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCamera::IDD, pParent)
{

}

CSelectCamera::~CSelectCamera()
{
}

void CSelectCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECT_CAMERA, m_listBoxCameraObjects);
}


BEGIN_MESSAGE_MAP(CSelectCamera, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectCamera::OnBnClickedOk)
	ON_BN_CLICKED(ID_ACIVATE_AND_RENDER, &CSelectCamera::OnBnClickedAcivateAndRender)
END_MESSAGE_MAP()


// CSelectCamera message handlers


CVoid CSelectCamera::InserItemToCameraList( char * objectName )
{
	cameraObjectIndex++;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = cameraObjectIndex;
	lvItem.iSubItem = 0;
	lvItem.pszText = objectName;
	m_listBoxCameraObjects.InsertItem(&lvItem);
	m_listBoxCameraObjects.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP | LVS_EX_GRIDLINES  );
	m_listBoxCameraObjects.SetItemState(cameraObjectIndex, LVIS_SELECTED, LVIS_SELECTED | LVIS_FOCUSED );
	m_listBoxCameraObjects.EnsureVisible(0, FALSE);
}

BOOL CSelectCamera::OnInitDialog()
{
	CDialog::OnInitDialog();
	cameraObjectIndex = -1;
	RECT rect;
	m_listBoxCameraObjects.GetClientRect(&rect);
	m_listBoxCameraObjects.InsertColumn(0, "Camera Names", LVCFMT_LEFT, rect.right - rect.left, 0 );
	InserItemToCameraList( "Default Engine Camera" );
	for( CUInt i = 0; i < g_cameraInstances.size(); i++ )
	{
		InserItemToCameraList( g_cameraInstances[i]->m_abstractCamera->GetName() );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSelectCamera::OnBnClickedOk()
{
	CInstanceCamera* tempInstanceCamera;

	int nSelected = -1; 
	POSITION p = m_listBoxCameraObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxCameraObjects.GetNextSelectedItem(p);
	}
	if( nSelected >= 0 )
	{
		TCHAR szBuffer[1024];
		DWORD cchBuf(1024);
		LVITEM lvi;
		lvi.iItem = nSelected;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_listBoxCameraObjects.GetItem(&lvi);

		if( Cmp( "Default Engine Camera", szBuffer ) )
			g_render.SetActiveInstanceCamera( NULL );
		else
		{
			for( CUInt i = 0; i < g_cameraInstances.size(); i++ )
			{
				if( Cmp( g_cameraInstances[i]->m_abstractCamera->GetName(), szBuffer ) )
				{
					tempInstanceCamera = g_cameraInstances[i];
					g_render.SetActiveInstanceCamera( tempInstanceCamera );
					break;
				}
			}
		}
	}
	OnOK();
}

INT_PTR CSelectCamera::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialogTemplate dlt;
	int nResult;
	// load dialog template
	if (!dlt.Load(MAKEINTRESOURCE(CSelectCamera::IDD))) return -1;
	// set the font, for example "Arial", 10 pts.
	dlt.SetFont("Arial", 8);
	// get pointer to the modified dialog template
	LPSTR pdata = (LPSTR)GlobalLock(dlt.m_hTemplate);
	// let MFC know that you are using your own template
	m_lpszTemplateName = NULL;
	InitModalIndirect(pdata);
	// display dialog box
	nResult = CDialog::DoModal();
	// unlock memory object
	GlobalUnlock(dlt.m_hTemplate);
	return nResult;
}

void CSelectCamera::OnBnClickedAcivateAndRender()
{
	if( g_vandaDemo )
	{
		MessageBox( "This feature lets you render the scene from the perspective of selected camera.\nIf your selected camera includes animation, Vanda Engine Version 1.x activates its animation and captures a video from the perspective of camera.\nThis feature is not available in RTI demo Edition Version 1.x", "Vanda Engine Error", MB_OK | MB_ICONINFORMATION );
	}
	else
	{
	}
}
