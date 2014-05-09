//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// EditMaterial.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "EditMaterial.h"
#include  "AFXPRIV.H"
#include <string>
#include <set>
#include <map>
#include <vector>
using namespace std;
// CEditMaterial dialog

IMPLEMENT_DYNAMIC(CEditMaterial, CDialog)

CEditMaterial::CEditMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CEditMaterial::IDD, pParent)
{
	m_firstBias = m_firstScale = CTrue;
}

CEditMaterial::~CEditMaterial()
{
}

void CEditMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NORMALMAP, m_editBoxNormalMap);
	DDX_Control(pDX, IDC_BTN_NORMALMAP, m_btnNormalMap);
	DDX_Control(pDX, IDC_BTN_DELETE_NORMALMAP, m_btnRemoveNormalMap);
	DDX_Control(pDX, IDC_EDIT_GLOSSMAP, m_editBoxGlossMap);
	DDX_Control(pDX, IDC_EDIT_ALPHAMAP, m_editBoxAlphaMap);
	DDX_Control(pDX, IDC_EDIT_SHADOWMAP, m_editBoxShadowMap);
	DDX_Control(pDX, IDC_EDIT_DIRTMAP, m_editBoxDirtMap);
	DDX_Control(pDX, IDC_EDIT_DIFFUSE, m_editBoxDiffuse);
	DDX_Control(pDX, IDC_BTN_GLOSSMAP, m_btnGlossMap);
	DDX_Control(pDX, IDC_BTN_ALPHAMAP, m_btnAlphaMap);
	DDX_Control(pDX, IDC_BTN_SHADOWMAP, m_btnShadowMap);
	DDX_Control(pDX, IDC_BTN_DIRTMAP, m_btnDirtMap);
	DDX_Control(pDX, IDC_BTN_DIFFUSE, m_btnDiffuse);
	DDX_Control(pDX, IDC_BTN_DELETE_GLOSSMAP, m_btnRemoveGlossMap);
	DDX_Control(pDX, IDC_BTN_DELETE_ALPHAMAP, m_btnRemoveAlphaMap);
	DDX_Control(pDX, IDC_BTN_DELETE_SHADOWMAP, m_btnRemoveShadowMap);
	DDX_Control(pDX, IDC_BTN_DELETE_DIRTMAP, m_btnRemoveDirtMap);
	DDX_Control(pDX, IDC_BTN_DELETE_DIFFUSE, m_btnRemoveDiffuse);
	DDX_Control(pDX, IDC_EDIT_NORMALMAP_SCALE, m_editBoxScale);
	DDX_Control(pDX, IDC_EDIT_NORMALMAP_BIAS, m_editBoxBias);
	DDX_Control(pDX, IDC_EDIT_PHYSX, m_editBoxPhysX);
	DDX_Control(pDX, IDC_BTN_DELETE_PHYSX, m_btnRemovePhysX);
	DDX_Control(pDX, IDC_BTN_PHYSX, m_btnGeneratePhysX);
	DDX_Control(pDX, IDC_COMBO_PHYSX, m_comboPhysX);
	DDX_Control(pDX, IDC_EDIT_PHYSX_PERCENTAGE, m_physXPercentage);
	DDX_Control(pDX, IDC_EDIT_PHYSX_DENSITY, m_physXDensity);
	DDX_Control(pDX, IDC_CHECK_TRIGGER, m_checkBoxTrigger);
}


BEGIN_MESSAGE_MAP(CEditMaterial, CDialog)
	ON_BN_CLICKED(IDC_BTN_NORMALMAP, &CEditMaterial::OnBnClickedBtnNormalmap)
	ON_BN_CLICKED(IDC_BTN_GLOSSMAP, &CEditMaterial::OnBnClickedBtnGlossmap)
	ON_BN_CLICKED(IDC_BTN_ALPHAMAP, &CEditMaterial::OnBnClickedBtnAlphamap)
	ON_BN_CLICKED(IDC_BTN_SHADOWMAP, &CEditMaterial::OnBnClickedBtnShadowmap)
	ON_BN_CLICKED(IDC_BTN_DIRTMAP, &CEditMaterial::OnBnClickedBtnDirtmap)
	ON_BN_CLICKED(IDC_BTN_DIFFUSE, &CEditMaterial::OnBnClickedBtnDiffuse)
	ON_BN_CLICKED(IDC_BTN_DELETE_NORMALMAP, &CEditMaterial::OnBnClickedBtnDeleteNormalmap)
	ON_BN_CLICKED(IDC_BTN_DELETE_GLOSSMAP, &CEditMaterial::OnBnClickedBtnDeleteGlossmap)
	ON_BN_CLICKED(IDC_BTN_DELETE_ALPHAMAP, &CEditMaterial::OnBnClickedBtnDeleteAlphamap)
	ON_BN_CLICKED(IDC_BTN_DELETE_SHADOWMAP, &CEditMaterial::OnBnClickedBtnDeleteShadowmap)
	ON_BN_CLICKED(IDC_BTN_DELETE_DIRTMAP, &CEditMaterial::OnBnClickedBtnDeleteDirtmap)
	ON_BN_CLICKED(IDC_BTN_DELETE_DIFFUSE, &CEditMaterial::OnBnClickedBtnDeleteDiffuse)
	ON_EN_CHANGE(IDC_EDIT_NORMALMAP_BIAS, &CEditMaterial::OnEnChangeEditNormalmapBias)
	ON_EN_CHANGE(IDC_EDIT_NORMALMAP_SCALE, &CEditMaterial::OnEnChangeEditNormalmapScale)
	ON_BN_CLICKED(IDC_BTN_PHYSX, &CEditMaterial::OnBnClickedBtnPhysx)
	ON_BN_CLICKED(IDC_BTN_DELETE_PHYSX, &CEditMaterial::OnBnClickedBtnDeletePhysx)
	ON_CBN_SELCHANGE(IDC_COMBO_PHYSX, &CEditMaterial::OnCbnSelchangeComboPhysx)
	ON_EN_CHANGE(IDC_EDIT_PHYSX_PERCENTAGE, &CEditMaterial::OnEnChangeEditPhysxPercentage)
	ON_EN_CHANGE(IDC_EDIT_PHYSX_DENSITY, &CEditMaterial::OnEnChangeEditPhysxDensity)
END_MESSAGE_MAP()

CVoid CEditMaterial::SetPhysX( CInstanceGeometry* instanceGeo )
{
	if( instanceGeo )
	{
		m_strPhysX = instanceGeo->m_physXName;
		m_fDensity = instanceGeo->m_physXDensity;
		m_strDensity.Format( "%f",m_fDensity );

		m_iPercentage = instanceGeo->m_physXPercentage;
		m_strPercentage.Format( "%d", m_iPercentage );

		m_instanceGeometry = instanceGeo;
	}
	else
	{
		m_strPhysX = "\n";
		m_instanceGeometry = NULL;
	}

	if( Cmp((LPCTSTR)m_strPhysX, "\n" ) )
	{
		m_removePhysXBtnState = CFalse;
		m_physXBtnState = CTrue;
	}
	else
	{
		m_physXBtnState = CFalse;
		m_removePhysXBtnState = CTrue;
	}
}

CVoid CEditMaterial::SetEditBoxBias( CFloat bias )
{
	m_parallaxMapBias = bias;
	m_strParallaxMapBias.Format( "%f", m_parallaxMapBias);
}

CVoid CEditMaterial::SetEditBoxScale( CFloat scale )
{
	m_parallaxMapScale = scale;
	m_strParallaxMapScale.Format( "%f", m_parallaxMapScale);
}

CVoid CEditMaterial::SetEditBoxNormalMapName( CString string )
{
	m_strNormalMapName = string;
}

CVoid CEditMaterial::SetEditBoxGlossMapName( CString string )
{
	m_strGlossMapName = string;
}

CVoid CEditMaterial::SetEditBoxAlphaMapName( CString string )
{
	m_strAlphaMapName = string;
}

CVoid CEditMaterial::SetEditBoxShadowMapName( CString string )
{
	m_strShadowMapName = string;
}

CVoid CEditMaterial::SetEditBoxDirtMapName( CString string )
{
	m_strDirtMapName = string;
}

CVoid CEditMaterial::SetEditBoxDiffuseName( CString string )
{
	m_strDiffuseName = string;
}
//Button States
CVoid CEditMaterial::SetNormalBtnState( CBool state )
{
	m_normalBtnState = state;
}

CVoid CEditMaterial::SetGlossBtnState( CBool state )
{
	m_glossBtnState = state;
}

CVoid CEditMaterial::SetAlphaBtnState( CBool state )
{
	m_alphaBtnState = state;
}

CVoid CEditMaterial::SetShadowBtnState( CBool state )
{
	m_shadowBtnState = state;
}

CVoid CEditMaterial::SetDirtBtnState( CBool state )
{
	m_dirtBtnState = state;
}

CVoid CEditMaterial::SetDiffuseBtnState( CBool state )
{
	m_diffuseBtnState = state;
}

//Remove Button States
CVoid CEditMaterial::SetRemoveNormalBtnState( CBool state )
{
	m_removeNormalBtnState = state;
}

CVoid CEditMaterial::SetRemoveGlossBtnState( CBool state )
{
	m_removeGlossBtnState = state;
}

CVoid CEditMaterial::SetRemoveAlphaBtnState( CBool state )
{
	m_removeAlphaBtnState = state;
}

CVoid CEditMaterial::SetRemoveShadowBtnState( CBool state )
{
	m_removeShadowBtnState = state;
}

CVoid CEditMaterial::SetRemoveDirtBtnState( CBool state )
{
	m_removeDirtBtnState = state;
}

CVoid CEditMaterial::SetRemoveDiffuseBtnState( CBool state )
{
	m_removeDiffuseBtnState = state;
}

// CEditMaterial message handlers
void CEditMaterial::OnBnClickedBtnNormalmap()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DDS File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strNormalMapName, eNORMAL_MAP );

		g_updateTextureViaEditor = CTrue;
		m_strNormalMapName = (CString)dlgOpen.GetPathName();
		m_strPureNormalMapName = dlgOpen.GetFileTitle();
		CBool error = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetNormalMap( m_strNormalMapName/*m_strPureNormalMapName*/, m_parallaxMapBias, m_parallaxMapScale, CFalse, CFalse ) )
				error = CTrue;
		}
		if( !error )
		{
			PrintInfo( "\nNew normal map added successfully" );
			m_editBoxNormalMap.SetWindowText( m_strPureNormalMapName + ".dds" );
			m_btnRemoveNormalMap.EnableWindow( TRUE );
			m_editBoxBias.EnableWindow( TRUE );
			m_editBoxScale.EnableWindow( TRUE );
		}
		else
		{
			PrintInfo( "\nError! An error occured while creating the normal map", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}

void CEditMaterial::OnBnClickedBtnGlossmap()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DDS File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strGlossMapName, eGLOSS_MAP );

		g_updateTextureViaEditor = CTrue;
		m_strGlossMapName = (CString)dlgOpen.GetPathName();
		m_strPureGlossMapName = dlgOpen.GetFileTitle();
		CBool error = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetGlossMap( m_strGlossMapName/*m_strPureGlossMapName*/ ) )
				error = CTrue;
		}
		if( !error )
		{
			PrintInfo( "\nNew gloss map added successfully" );
			m_editBoxGlossMap.SetWindowText( m_strPureGlossMapName + ".dds" );
			m_btnRemoveGlossMap.EnableWindow( TRUE );
		}
		else
		{
			PrintInfo( "\nError! An error occured while creating the gloss map", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}

void CEditMaterial::OnBnClickedBtnAlphamap()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("TGA File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strAlphaMapName, eALPHA_MAP );

		g_updateTextureViaEditor = CTrue;
		m_strAlphaMapName = (CString)dlgOpen.GetPathName();
		m_strPureAlphaMapName = dlgOpen.GetFileTitle();
		CBool error = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetAlphaMap( m_strAlphaMapName/*m_strPureAlphaMapName*/ ) )
				error = CTrue;
		}
		if( !error )
		{
			PrintInfo( "\nNew alpha map added successfully" );
			m_editBoxAlphaMap.SetWindowText( m_strPureAlphaMapName + ".dds" );
			m_btnRemoveAlphaMap.EnableWindow( TRUE );
		}
		else
		{
			PrintInfo( "\nError! An error occured while creating the alpha map", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}

void CEditMaterial::OnBnClickedBtnShadowmap()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DDS File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strShadowMapName, eSHADOW_MAP );

		g_updateTextureViaEditor = CTrue;
		m_strShadowMapName = (CString)dlgOpen.GetPathName();
		m_strPureShadowMapName = dlgOpen.GetFileTitle();
		CBool error = CFalse;

		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetShadowMap( m_strShadowMapName/*m_strPureShadowMapName*/ ) )
			error = CTrue;
		}

		if( !error )
		{
			PrintInfo( "\nNew Shadow map added successfully" );
			m_editBoxShadowMap.SetWindowText( m_strPureShadowMapName + ".dds" );
			m_btnRemoveShadowMap.EnableWindow( TRUE );
		}
		else
		{
			PrintInfo( "\nError! An error occured while creating the shadow map", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();

	}
}

void CEditMaterial::OnBnClickedBtnDirtmap()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DDS File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strDirtMapName, eDIRT_MAP );

		g_updateTextureViaEditor = CTrue;
		m_strDirtMapName = (CString)dlgOpen.GetPathName();
		m_strPureDirtMapName = dlgOpen.GetFileTitle();
		CBool error = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetDirtMap( m_strDirtMapName/*m_strPureDirtMapName*/ ) )
			error = CTrue;
		}
		if( !error )
		{
			PrintInfo( "\nNew Dirt map added successfully" );
			m_editBoxDirtMap.SetWindowText( m_strPureDirtMapName + ".dds" );
			m_btnRemoveDirtMap.EnableWindow( TRUE );
		}

		else
		{
			PrintInfo( "\nError! An error occured while creating the dirt map", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}

void CEditMaterial::OnBnClickedBtnDiffuse()
{
	CFileDialog dlgOpen(TRUE, _T("*.dds"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DDS File (*.dds)|*.dds||"), NULL, NULL);
	if( IDOK == dlgOpen.DoModal() )
	{
		RemoveUnsharedImage( m_strDiffuseName, eDIFFUSE );
		g_updateTextureViaEditor = CTrue;
		m_strDiffuseName = (CString)dlgOpen.GetPathName();
		m_strPureDiffuseName = dlgOpen.GetFileTitle();


		CBool error = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( !g_scene[i]->SetDiffuse( m_strDiffuseName/*m_strPureDiffuseName*/ ) )
				error = CTrue;
		}
		if( !error )
		{
			PrintInfo( "\nNew diffuse texture added successfully" );
			m_editBoxDiffuse.SetWindowText( m_strPureDiffuseName + ".dds" );
			m_btnRemoveDiffuse.EnableWindow( TRUE );
		}
		else
		{
			PrintInfo( "\nError! An error occured while creating the diffuse texture", COLOR_RED );
			numErrors += 1;
		}
		g_updateTextureViaEditor = CFalse;

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}

void CEditMaterial::OnBnClickedBtnDeleteNormalmap()
{
	if( MessageBox( "Remove current normal map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strNormalMapName, eNORMAL_MAP);

		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveNormalMap();

		m_editBoxNormalMap.SetWindowTextA( NULL );
		m_btnRemoveNormalMap.EnableWindow( FALSE );
		m_editBoxBias.EnableWindow( FALSE );
		m_editBoxScale.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CEditMaterial::OnBnClickedBtnDeleteGlossmap()
{
	if( MessageBox( "Remove current gloss map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strGlossMapName, eGLOSS_MAP);

		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveGlossMap();

		m_editBoxGlossMap.SetWindowTextA( NULL );
		m_btnRemoveGlossMap.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CEditMaterial::OnBnClickedBtnDeleteAlphamap()
{
	if( MessageBox( "Remove current alpha map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strAlphaMapName, eALPHA_MAP);

		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveAlphaMap();

		m_editBoxAlphaMap.SetWindowTextA( NULL );
		m_btnRemoveAlphaMap.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CEditMaterial::OnBnClickedBtnDeleteShadowmap()
{
	if( MessageBox( "Remove current shadow map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strShadowMapName, eSHADOW_MAP);

		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveShadowMap();

		m_editBoxShadowMap.SetWindowTextA( NULL );
		m_btnRemoveShadowMap.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CEditMaterial::OnBnClickedBtnDeleteDirtmap()
{
	if( MessageBox( "Remove current dirt map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strDirtMapName, eDIRT_MAP);

		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveDirtMap();

		m_editBoxDirtMap.SetWindowTextA( NULL );
		m_btnRemoveDirtMap.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CEditMaterial::OnBnClickedBtnDeleteDiffuse()
{
	if( MessageBox( "Remove current diffuse map?", "Warning", MB_YESNO ) == IDYES )
	{
		RemoveUnsharedImage(m_strDiffuseName, eDIFFUSE);
		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->RemoveDiffuse();
		m_editBoxDiffuse.SetWindowTextA( NULL );
		m_btnRemoveDiffuse.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

BOOL CEditMaterial::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editBoxNormalMap.SetWindowTextA(m_strNormalMapName);
	m_editBoxGlossMap.SetWindowTextA(m_strGlossMapName);
	m_editBoxAlphaMap.SetWindowTextA(m_strAlphaMapName);
	m_editBoxShadowMap.SetWindowTextA(m_strShadowMapName);
	m_editBoxDirtMap.SetWindowTextA(m_strDirtMapName);
	m_editBoxDiffuse.SetWindowTextA(m_strDiffuseName);
	m_editBoxScale.SetWindowTextA( m_strParallaxMapScale );
	m_editBoxBias.SetWindowTextA( m_strParallaxMapBias );
	m_editBoxPhysX.SetWindowTextA( m_strPhysX );

	m_btnNormalMap.EnableWindow(m_normalBtnState);
	m_btnGlossMap.EnableWindow(m_glossBtnState);
	m_btnAlphaMap.EnableWindow(m_alphaBtnState);
	m_btnShadowMap.EnableWindow(m_shadowBtnState);
	m_btnDirtMap.EnableWindow(m_dirtBtnState);
	m_btnDiffuse.EnableWindow(m_diffuseBtnState);
	m_btnGeneratePhysX.EnableWindow( m_physXBtnState );
	m_btnRemoveNormalMap.EnableWindow(m_removeNormalBtnState);
	m_btnRemoveGlossMap.EnableWindow(m_removeGlossBtnState);
	m_btnRemoveAlphaMap.EnableWindow(m_removeAlphaBtnState);
	m_btnRemoveShadowMap.EnableWindow(m_removeShadowBtnState);
	m_btnRemoveDirtMap.EnableWindow(m_removeDirtBtnState);
	m_btnRemoveDiffuse.EnableWindow(m_removeDiffuseBtnState);
	m_btnRemovePhysX.EnableWindow( m_removePhysXBtnState );
	m_editBoxScale.EnableWindow( m_removeNormalBtnState );
	m_editBoxBias.EnableWindow( m_removeNormalBtnState );

	m_comboPhysX.InsertString( 0, "Convex Hull" );
	m_comboPhysX.InsertString(	 1, "Triangulate: Length" );
	m_comboPhysX.InsertString( 2, "Triangulate: Length * Curvature" );
	m_comboPhysX.InsertString( 3, "Box" );
	m_comboPhysX.InsertString( 4, "Sphere" );
	m_comboPhysX.InsertString( 5, "Capsule: Method 1" );
	m_comboPhysX.InsertString( 6, "Capsule: Method 2" );
	m_comboPhysX.InsertString( 7, "Capsule: Method 3" );

	if( m_instanceGeometry && m_instanceGeometry->m_lodAlgorithm != eLOD_NONE )
		m_comboPhysX.SetCurSel(m_instanceGeometry->m_lodAlgorithm);
	else
		m_comboPhysX.SetCurSel(0);

	if( m_instanceGeometry && (m_instanceGeometry->m_lodAlgorithm == eLOD_LENGTH_CURVATURE ||  m_instanceGeometry->m_lodAlgorithm == eLOD_LENGTH))
		m_physXPercentage.EnableWindow( TRUE );
	else
		m_physXPercentage.EnableWindow( FALSE );

	if( m_instanceGeometry && m_instanceGeometry->m_isTrigger )
		m_checkBoxTrigger.SetCheck( BST_CHECKED );
	else
		m_checkBoxTrigger.SetCheck( BST_UNCHECKED );

	m_physXPercentage.SetWindowTextA( m_strPercentage );
	m_physXDensity.SetWindowTextA( m_strDensity );
	return TRUE;
}

void CEditMaterial::OnOK()
{
	if( !Cmp((LPCTSTR)m_strNormalMapName, "\n" ) && (m_strParallaxMapBias.IsEmpty() || m_strParallaxMapScale.IsEmpty() ))
	{
		MessageBox( "Please enter the valid data for all the required fields", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	CDialog::OnOK();
}

INT_PTR CEditMaterial::DoModal()
{
	CDialogTemplate dlt;
	int nResult;
	// load dialog template
	if (!dlt.Load(MAKEINTRESOURCE(CEditMaterial::IDD))) return -1;
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
void CEditMaterial::OnEnChangeEditNormalmapBias()
{
	m_editBoxBias.GetWindowTextA( m_strParallaxMapBias );
	m_parallaxMapBias = atof( m_strParallaxMapBias );
	if(!m_firstBias )
	{
		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->SetNormalMap( "", m_parallaxMapBias, m_parallaxMapScale, CTrue, CFalse );
	}
	m_firstBias = CFalse;
}

void CEditMaterial::OnEnChangeEditNormalmapScale()
{
	m_editBoxScale.GetWindowTextA( m_strParallaxMapScale );
	m_parallaxMapScale = atof( m_strParallaxMapScale );
	if( !m_firstScale )
	{
		for( CUInt i = 0; i < g_scene.size(); i++ )
			g_scene[i]->SetNormalMap( "", m_parallaxMapBias, m_parallaxMapScale, CFalse, CTrue );
	}
	m_firstScale = CFalse;
}

void CEditMaterial::OnBnClickedBtnPhysx()
{
	if( g_multipleView->m_nx->m_hasScene )
	{
		MessageBox( "PhysX generation does not work with external PhysX scenes.\nPlease remove the external PhysX scene and try again.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	if( !g_menu.m_geometryBasedSelection )
	{
		MessageBox( "PhysX generation only works in 'geometry based selection'", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	CInt curSel = m_comboPhysX.GetCurSel();
	CPhysXAlgorithm algorithm;
	switch( curSel )
	{
		case 0:
			algorithm = eLOD_CONVEX_HULL;
			break;
		case 1:
			algorithm = eLOD_LENGTH;
			break;
		case 2:
			algorithm = eLOD_LENGTH_CURVATURE;
			break;
		case 3:
			algorithm = eLOD_BOX;
			break;
		case 4:
			algorithm = eLOD_SPHERE;
			break;
		case 5:
			algorithm = eLOD_CAPSULE_METHOD1;
			break;
		case 6:
			algorithm = eLOD_CAPSULE_METHOD2;
			break;
		case 7:
			algorithm = eLOD_CAPSULE_METHOD3;
		default:
			break;
	}

	m_physXDensity.GetWindowTextA( m_strDensity );
	m_fDensity = atof( m_strDensity );
	if( m_fDensity < 0.0f )
		m_fDensity = 0.0f;
	if( m_fDensity > 0 && m_instanceGeometry && m_instanceGeometry->m_abstractGeometry->m_hasAnimation  ) 
	{
		MessageBoxA( "You can't assign a dynamic actor to an animated instanced geometry. Please set density to 0 and try again.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	if( (algorithm == eLOD_LENGTH || algorithm == eLOD_LENGTH_CURVATURE ) && m_instanceGeometry && m_instanceGeometry->m_abstractGeometry->m_hasAnimation )
	{
		MessageBoxA( "You can't assign a triangulated PhysX shape to an animated instanced geometry. Please choose another shape and try again.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;

	}
	CInt isTrigger;
	isTrigger = m_checkBoxTrigger.GetCheck();

	if( isTrigger && m_instanceGeometry && m_instanceGeometry->m_abstractGeometry->m_hasAnimation)
	{
		MessageBoxA( "You can't assign a trigger to an animated instanced geometry or skin. Please uncheck trigger checkbox and try again.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	if( isTrigger && m_fDensity)
	{
		MessageBoxA( "You can't create a dynamic trigger. Please set denistity to 0 or uncheck trigger checkbox.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}

	m_physXPercentage.GetWindowTextA( m_strPercentage );
	m_iPercentage = (CInt)atof( m_strPercentage );
	if( m_iPercentage <= 0 )
		m_iPercentage = 1;
	else if( m_iPercentage > 100 )
		m_iPercentage = 100;

	CInt index = -1;
	CBool m_bIsTrigger;
	if( isTrigger )
		m_bIsTrigger = CTrue;
	else
		m_bIsTrigger = CFalse;
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		index = g_scene[i]->GeneratePhysX(algorithm, m_fDensity, m_iPercentage, m_bIsTrigger);
		if( index != -1 )
			break;
	}
	if( m_bIsTrigger )
	{
		ex_pBtnScriptEditor->EnableWindow( TRUE );
		ex_pMenu->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_ENABLED );
		ex_pAddScript->SetInstanceGeo(m_instanceGeometry);
	}

	if( index != -1 )
	{
		if( !g_menu.m_physicsDebugMode )
		{
			g_menu.m_physicsDebugMode = CTrue;

			ex_pMenu->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_CHECKED );
			PrintInfo3( "\nPhysX Debug mode enabled" );
		}
		PrintInfo( "\nNew PhysX mesh generated successfully");
	
		CString nameIndex;
		nameIndex.Format( "%d", index );
		m_btnRemovePhysX.EnableWindow( TRUE );
		m_btnGeneratePhysX.EnableWindow( FALSE );
		if( m_instanceGeometry )
			m_strPhysX = m_instanceGeometry->m_physXName;
		m_editBoxPhysX.SetWindowTextA( m_strPhysX );
		if( m_instanceGeometry &&  (m_fDensity > 0 || m_instanceGeometry->m_abstractGeometry->m_hasAnimation))
			ex_pVandaEngine1Dlg->InsertItemToPhysXList( m_instanceGeometry->m_physXName, ePHYSXELEMENTLIST_DYNAMIC_RIGIDBODY );
		else if( m_instanceGeometry && m_bIsTrigger )
			ex_pVandaEngine1Dlg->InsertItemToPhysXList( m_instanceGeometry->m_physXName, ePHYSXELEMENTLIST_TRIGGER );
		else if( m_instanceGeometry )
			ex_pVandaEngine1Dlg->InsertItemToPhysXList( m_instanceGeometry->m_physXName, ePHYSXELEMENTLIST_STATIC_RIGIDBODY );
		g_updateOctree = CTrue;

	}
	else
	{
		PrintInfo( "\nFailed to generate the PhysX actor", COLOR_RED);
		m_editBoxPhysX.SetWindowTextA( NULL );
		//m_btnRemovePhysX.EnableWindow( FALSE );
	}
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
	g_multipleView->RenderWindow();

}

void CEditMaterial::OnBnClickedBtnDeletePhysx()
{
	if( g_multipleView->m_nx->m_hasScene )
	{
		MessageBox( "PhysX generation does not work with external PhysX scenes.\nPlease remove the external PhysX scene and try again.", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}

	if( !g_menu.m_geometryBasedSelection )
	{
		MessageBox( "PhysX generation only works in 'geometry based selection'", "Vanda Engine Error", MB_OK | MB_ICONERROR );
		return;
	}
	RemovePhysXMesh();
	ex_pBtnScriptEditor->EnableWindow( FALSE );
	ex_pMenu->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_DISABLED | MF_GRAYED );
	ex_pAddScript->SetInstanceGeo(NULL);

	g_updateOctree = CTrue;
	g_multipleView->RenderWindow();
}

CVoid CEditMaterial::RemovePhysXMesh()
{
	CBool found = CFalse;
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		for (int nItem = ex_pVandaEngine1Dlg->m_listBoxPhysXElements.GetItemCount()-1; nItem >= 0 ;nItem-- )
		{
			TCHAR szBuffer[1024];
			DWORD cchBuf(1024);
			LVITEM lvi;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = szBuffer;
			lvi.cchTextMax = cchBuf;

			ex_pVandaEngine1Dlg->m_listBoxPhysXElements.GetItem(&lvi);
			if( m_instanceGeometry && Cmp( szBuffer, m_instanceGeometry->m_physXName ) )
			{
				ex_pVandaEngine1Dlg->m_listBoxPhysXElements.DeleteItem(nItem);
				ex_pVandaEngine1Dlg->SortPhysXList();
				break;
			}
		}

		found = g_scene[i]->RemovePhysX();
		if( found )
			break;
	}
	if( found )
	{
		if( m_instanceGeometry->m_isTrigger )
		{
			if( m_instanceGeometry->m_hasEnterScript || m_instanceGeometry->m_hasExitScript )
			{
				for(CUInt index = 0; index < g_allVSceneNames.size(); index++ )
				{
					m_instanceGeometry->m_node->GetScene()->IsInVSceneList(g_allVSceneNames[index].c_str(), CFalse, CTrue );
				}
			}
			m_instanceGeometry->m_isTrigger = CFalse;
			m_instanceGeometry->m_hasEnterScript = m_instanceGeometry->m_hasExitScript = CFalse;
			Cpy( m_instanceGeometry->m_enterScript, "\n" );
			Cpy( m_instanceGeometry->m_exitScript, "\n" );
			ex_pAddScript->m_strTriggerEnterScriptName.Empty();
			ex_pAddScript->m_strTriggerExitScriptName.Empty();
		}

		PrintInfo( "\nActor removed successfully" );
		m_btnGeneratePhysX.EnableWindow( TRUE );
		m_btnRemovePhysX.EnableWindow( FALSE );
		m_editBoxPhysX.SetWindowTextA( NULL );
	}
	else
	{
		PrintInfo( "\nFailed to remove the actor" );
	}

}


void CEditMaterial::OnCbnSelchangeComboPhysx()
{
	CInt curSel = m_comboPhysX.GetCurSel();
	if( curSel == eLOD_LENGTH_CURVATURE || curSel == eLOD_LENGTH )
		m_physXPercentage.EnableWindow( TRUE );
	else
		m_physXPercentage.EnableWindow( FALSE );
}

void CEditMaterial::OnEnChangeEditPhysxPercentage()
{
	m_physXPercentage.GetWindowTextA( m_strPercentage );
	m_iPercentage = (CInt)atof( m_strPercentage );
	if( m_iPercentage <= 0 )
		m_iPercentage = 1;
	else if( m_iPercentage > 100 )
		m_iPercentage = 100;
}

void CEditMaterial::OnEnChangeEditPhysxDensity()
{
	m_physXDensity.GetWindowTextA( m_strDensity );
	m_fDensity = atof( m_strDensity );
	if( m_fDensity < 0.0f )
		m_fDensity = 0.0f;
}

void CEditMaterial::RemoveUnsharedImage( CString string, CTextureMapType type )
{
	CChar imageNameTemp[MAX_NAME_SIZE];
	Cpy( imageNameTemp, string.GetBuffer(string.GetLength()));
	GetWithoutDot( imageNameTemp );
	CChar* imageName = GetAfterPath( imageNameTemp );
	string.ReleaseBuffer();
	std::vector <std::string> m_strImageNames;
	CInt maxImages = 0;
	std::vector <std::map <std::string, size_t>> occurences;
	if( Cmp( imageName, "*Different Textures*" ) )
	{
		if( g_selectedName != -1 )
		{
			CBool foundTarget = CFalse;
			for( CUInt sceneIndex  = 0; sceneIndex < g_scene.size(); sceneIndex++ )
			{
				for( CUInt i = 0; i < g_scene[sceneIndex]->m_instanceGeometries.size(); i++ )
				{
					CBool selected = CFalse;
					if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_nameIndex == g_selectedName )
					{
						selected = CTrue;
					}

					if( selected )
					{
						for( CUInt j = 0; j < g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups.size(); j++ )
						{
							switch (type)
							{
							case eDIFFUSE:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasDiffuse )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strDiffuse );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							case eNORMAL_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasNormalMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strNormalMap );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							case eSHADOW_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasShadowMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strShadowMap );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							case eDIRT_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasDirtMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strDirtMap );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							case eGLOSS_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasGlossMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strGlossMap );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							case eALPHA_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasAlphaMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strAlphaMap );
									Append( temp, ".dds" );
									m_strImageNames.push_back( temp );
								}
								break;
							}
						}
					}
					if( foundTarget )
						break;
				}
				if( foundTarget )
					break;
			}
			//fill map with data
			for( CUInt i = 0; i < m_strImageNames.size(); i++ )
			{
				CBool foundTarget = CFalse;
				for( CUInt j = 0; j < occurences.size(); j++ )
				{
					if( occurences[j].begin()->first == m_strImageNames[i] )
					{
						foundTarget = CTrue;
						occurences[j].begin()->second +=1;
					}
				}
				if( !foundTarget )
				{
					std::map<std::string, size_t> temp;
					temp[m_strImageNames[i]] = 1;
					occurences.push_back( temp );
				}
			}
		}
	}
	else
	{
		Append( imageName, ".dds" );
		m_strImageNames.push_back(imageName);

		if( g_menu.m_geometryBasedSelection )
		{
			CBool foundTarget = CFalse;
			for( CUInt sceneIndex = 0; sceneIndex < g_scene.size(); sceneIndex++ )
			{
				for( CUInt i = 0; i < g_scene[sceneIndex]->m_instanceGeometries.size(); i++ )
				{
					CGeometry* m_geo = g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry;
					CBool selected = CFalse;
					if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_nameIndex == g_selectedName )
					{
						selected = CTrue;
						foundTarget = CTrue;
					}

					if( selected )
					{
						for( CUInt j = 0; j < m_geo->m_groups.size(); j++ )
						{
							switch (type)
							{
							case eDIFFUSE:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasDiffuse )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strDiffuse );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							case eSHADOW_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasShadowMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strShadowMap );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							case eDIRT_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasDirtMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strDirtMap );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							case eGLOSS_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasGlossMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strGlossMap );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							case eNORMAL_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasNormalMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strNormalMap );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							case eALPHA_MAP:
								if( g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_hasAlphaMap )
								{
									CChar temp[MAX_NAME_SIZE];
									Cpy( temp, g_scene[sceneIndex]->m_instanceGeometries[i]->m_abstractGeometry->m_groups[j]->m_strAlphaMap );
									Append( temp, ".dds" );
									if( Cmp( GetAfterPath(temp), imageName))
										maxImages++;
								}
								break;
							}//switch
						}//for all of the groups
					}//if selected
					if( foundTarget )
						break;
				}
				if( foundTarget )
					break;
			}
		}
		else
		{
			maxImages = 1;
		}
		std::map<std::string, size_t> temp;
		temp[m_strImageNames[0]] = maxImages;
		occurences.push_back( temp );
	}
	for( CUInt imgIndex = 0; imgIndex < occurences.size(); imgIndex++ )
	{
		CChar imageName[MAX_NAME_SIZE];
		Cpy( imageName, occurences[imgIndex].begin()->first.c_str() );
		CInt maxImages =  (CInt)occurences[imgIndex].begin()->second;
		int numImages = 0;
		CBool foundTarget = CFalse;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				CGeometry* m_geo = g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry;
				for( CUInt k = 0; k < m_geo->m_groups.size(); k++ )
				{
					if( m_geo->m_groups[k]->m_hasDiffuse && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_diffuseImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( m_geo->m_groups[k]->m_hasNormalMap && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_normalMapImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( m_geo->m_groups[k]->m_hasAlphaMap && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_alphaMapImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( m_geo->m_groups[k]->m_hasGlossMap && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_glossMapImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( m_geo->m_groups[k]->m_hasShadowMap && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_shadowMapImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
					if( m_geo->m_groups[k]->m_hasDirtMap && Cmp(imageName, GetAfterPath(m_geo->m_groups[k]->m_dirtMapImg->m_fileName) ) )
					{
						numImages++;
						if( numImages > maxImages )
						{
							foundTarget = CTrue;
							break;
						}
					}
				}
				if( foundTarget )
					break;
			}
			if( foundTarget )
				break;
		}
		if( !foundTarget )
		{
			for( CUInt p = 0; p < g_images.size(); p++ )
			{
				if( Cmp( GetAfterPath(g_images[p]->m_fileName), imageName ) )
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf( temp, "\n%s%s%s", "Image '", imageName, "' removed from memory" );
					PrintInfo( temp );
					CDelete( *(g_images.begin() + p) );
					g_images.erase( g_images.begin() + p );
					break;
				}
			}
		}
	}
}
