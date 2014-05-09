//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// AddWaterAttachment.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "WaterAttachment.h"
#include  "AFXPRIV.H"

// CWaterAttachment dialog

IMPLEMENT_DYNAMIC(CWaterAttachment, CDialog)

CWaterAttachment::CWaterAttachment(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterAttachment::IDD, pParent)
{

}

CWaterAttachment::~CWaterAttachment()
{
	for (int nItem = m_listBoxWaterObjects.GetItemCount()-1; nItem >= 0 ;nItem-- )
	{
		m_listBoxWaterObjects.DeleteItem(nItem);
	}
}

void CWaterAttachment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WATER_ATTACHMENT, m_listBoxWaterObjects);
}


BEGIN_MESSAGE_MAP(CWaterAttachment, CDialog)
	ON_BN_CLICKED(IDATTACH, &CWaterAttachment::OnBnClickedAttach)
	ON_BN_CLICKED(IDDETACH, &CWaterAttachment::OnBnClickedDetach)
END_MESSAGE_MAP()


// CWaterAttachment message handlers

BOOL CWaterAttachment::OnInitDialog()
{
	CDialog::OnInitDialog();
	waterObjectIndex = -1;
	RECT rect;
	m_listBoxWaterObjects.GetClientRect(&rect);

	m_listBoxWaterObjects.InsertColumn(0, "Water Names", LVCFMT_LEFT, rect.right - rect.left, 0 );
	for( CUInt i = 0; i < g_engineWaters.size(); i++ )
	{
		InserItemToWaterList( g_engineWaters[i]->GetName() );
	}


	return TRUE; 
}

CVoid CWaterAttachment::InserItemToWaterList( char * objectName )
{
	waterObjectIndex++;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = waterObjectIndex;
	lvItem.iSubItem = 0;
	lvItem.pszText = objectName;
	m_listBoxWaterObjects.InsertItem(&lvItem);
	m_listBoxWaterObjects.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP | LVS_EX_GRIDLINES  );
	m_listBoxWaterObjects.SetItemState(waterObjectIndex, LVIS_SELECTED, LVIS_SELECTED | LVIS_FOCUSED );
	m_listBoxWaterObjects.EnsureVisible(0, FALSE);
}

void CWaterAttachment::OnBnClickedAttach()
{
	CWater* tempWater;

	int nSelected = -1; 
	POSITION p = m_listBoxWaterObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxWaterObjects.GetNextSelectedItem(p);
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
		m_listBoxWaterObjects.GetItem(&lvi);

		for( CUInt i = 0; i < g_engineWaters.size(); i++ )
		{
			if( Cmp( g_engineWaters[i]->GetName(), szBuffer ) )
			{
				tempWater = g_engineWaters[i];
			}
		}
		CBool foundTarget = CFalse;
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				if(  g_scene[i]->m_instanceGeometries[j]->m_nameIndex == g_selectedName )
				{
					//have we attached this object before?
					for( CUInt k = 0; k < tempWater->m_geometry.size(); k++ )
					{
						if( Cmp( tempWater->m_geometry[k]->GetName(), g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->GetName() ) )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( foundTarget )
					{
						MessageBox( "This object has been attached to the selected water before!", "VandaEngine Error", MB_OK | MB_ICONINFORMATION);
					}
					else
					{
						tempWater->SetGeometry( g_scene[i]->m_geometries[j] );
						PrintInfo( "\nObject successfully attached to the water " );
					}

				}
			}
		}
	}
	else
		MessageBox( "Please select a water name!", "VandaEngine Error", MB_OK | MB_ICONERROR);

	g_multipleView->RenderWindow();

}



void CWaterAttachment::OnBnClickedDetach()
{
	CWater* tempWater;

	int nSelected = -1; 
	POSITION p = m_listBoxWaterObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxWaterObjects.GetNextSelectedItem(p);
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
		m_listBoxWaterObjects.GetItem(&lvi);

		for( CUInt i = 0; i < g_engineWaters.size(); i++ )
		{
			if( Cmp( g_engineWaters[i]->GetName(), szBuffer ) )
			{
				tempWater = g_engineWaters[i];
			}
		}
		CBool foundTarget = CFalse;
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				if(  g_scene[i]->m_instanceGeometries[j]->m_nameIndex == g_selectedName )
				{
					//have we attached this object before?
					for( CUInt k = 0; k < tempWater->m_geometry.size(); k++ )
					{
						if( Cmp( tempWater->m_geometry[k]->GetName(), g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->GetName() ) )
						{
							foundTarget = CTrue;
							tempWater->m_geometry.erase( tempWater->m_geometry.begin() + k );
							PrintInfo( "\nObject successfully detached from the water" );
							break;
						}
					}
				}
			}
		}
		if( !foundTarget )
		{
			MessageBox( "You haven't attached this object to the selected water before!", "VandaEngine Error", MB_OK );
		}

	}
	else
		MessageBox( "Please select a water name!", "VandaEngine Error", MB_OK );

	g_multipleView->RenderWindow();

}

INT_PTR CWaterAttachment::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialogTemplate dlt;
	int nResult;
	// load dialog template
	if (!dlt.Load(MAKEINTRESOURCE(CWaterAttachment::IDD))) return -1;
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
