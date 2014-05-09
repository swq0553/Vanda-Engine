//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// VandaEngine1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "VandaEngine1.h"
#include "VandaEngine1Dlg.h"
#include "GraphicsEngine/Animation.h"
//#include "graphicsEngine\\imagelib.h"
#include "physxengine\\nx.h"
#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shfolder.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Edition.MaxVersion.MinVersion.BugFixes;
//RTI.1.2.0
CInt g_maxVersion = 1;
CInt g_minVersion = 2;
CInt g_bugFixesVersion = 0;
CChar g_edition[MAX_NAME_SIZE] = "RTIBasic";
CBool g_useOriginalPathOfDAETextures = CFalse;
CBool g_updateTextureViaEditor = CFalse;
CChar g_currentVSceneName[MAX_NAME_SIZE]; //save functions
std::vector<std::string> g_allVSceneNames; //save functions
std::vector<std::string> g_allVSceneNamesToBePublished; //save functions
std::string g_firstSceneNameToBeLoaded; //save functions
CChar g_VScenePath[MAX_NAME_SIZE];
CScene* g_currentScene = NULL;
CChar g_currentPassword[MAX_NAME_SIZE];
CRichEditCtrl *ex_pRichEdit = NULL; // extern
CRichEditCtrl *ex_pRichEdit2 = NULL; // extern
CRichEditCtrl *ex_pRichEdit3 = NULL; // extern
CRichEditCtrl *ex_pRichEditScript = NULL; //extern
CVandaEngine1Dlg* ex_pVandaEngine1Dlg = NULL;
CRichEditCtrl* ex_pStaticSelectedObject = NULL;

CButton* ex_pBtnPrevAnim = NULL;
CButton* ex_pBtnPlayAnim = NULL;
CButton* ex_pBtnPauseAnim = NULL;
CButton* ex_pBtnNextAnim = NULL;

CButton* ex_pBtnMaterialEditor = NULL;
CButton* ex_pBtnScriptEditor = NULL;
CMenu *ex_pMenu = NULL; //extern

CEditMaterial *ex_pEditMaterial = NULL; //extern
CAddScript* ex_pAddScript = NULL;

CBool g_vandaDemo = CFalse;
CBool g_importCOLLADA = CFalse;
CBool g_openVINFile = CFalse;
CChar g_fileNameInCommandLine[MAX_NAME_SIZE];
std::vector<COpenALSoundBuffer*>g_soundBuffers;
CRender g_render; //extern
std::vector<CScene*> g_scene;
CScene* g_arrowScene = NULL;
CScene* g_zArrow = NULL;
CScene* g_negativeZArrow = NULL;
CVec3f g_arrowPosition;

CBool g_showArrow = CFalse;
CScene* g_centerArrowScene = NULL;
COpenGLUtility g_glUtil;
CInt numErrors, numWarnings, totalErrors, totalWarnings;
CImage* g_soundImg = NULL;
CImage* g_pointLightImg = NULL;
CImage* g_directionalLightImg = NULL;
CImage* g_spotLightImg = NULL;
CImage* g_pointLightCOLLADAImg = NULL;
CImage* g_directionalLightCOLLADAImg = NULL;
CImage* g_spotLightCOLLADAImg = NULL;
CImage* g_skyImg = NULL;
CImage* g_waterImg = NULL;
CImage* g_cameraImg = NULL;
CVec4f g_defaultDirectionalLight;
CColor4f g_globalAmbientColor;
CBool g_useGlobalAmbientColor = CFalse;
std::vector<CInstanceLight*> g_engineLights;
std::vector<CWater*> g_engineWaters;
std::vector<CInstanceCamera*> g_cameraInstances;
std::vector<CImage*> g_images;
std::vector<CImage*>g_waterImages; // This variable holds the information of water images of Engine
std::vector<CStaticSound*>g_engineStaticSounds;
COpenALSystem* g_soundSystem = NULL;
std::vector<std::string> g_engineObjectNames;
CShadowMap * g_dynamicShadowMap = NULL;
COctree* g_octree;
CMultipleWindows *g_multipleView;
CBool g_updateOctree;
CFloat g_elapsedTime;
GLuint g_shaderType;
CUpdateCamera *g_camera;
CSkyDome *g_skyDome = NULL;
NxExtendedVec3 g_characterPos;
CVec3f g_cameraPitchYawTilt;
CBool g_useOldRenderingStyle = CFalse;
CMenuVariables g_menu;
CShadowProperties g_shadowProperties;
CDOFProperties g_dofProperties;
CFogProperties g_fogProperties;
CBloomProperties g_bloomProperties;
CLightProperties g_lightProperties;
CPathProperties g_pathProperties;
CExtraTexturesNamingConventions g_extraTexturesNamingConventions;
COptions g_options;
CPolygonMode g_polygonMode = ePOLYGON_FILL;
CBool g_firstPass;
CInstanceLight* g_currentInstanceLight = NULL;
CMaterialChannels g_materialChannels = eCHANNELS_ALL;
CInt g_sceneManagerObjectsPerSplit = 15;
CLuaState g_lua;
CBool g_testScript = CFalse;
CBloom* g_bloom = NULL;
CExternalPhysX*  g_externalPhysX = NULL;
CSceneBanner g_sceneBanner;
CInt g_width;
CInt g_height;
// CAboutDlg dialog used for App About

DWORD __stdcall MEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString sThisWrite;
	sThisWrite.GetBufferSetLength(cb);

	CString *psBuffer = (CString *)dwCookie;
	
	for (int i=0;i<cb;i++) {
		sThisWrite.SetAt(i,*(pbBuff+i));
	}

	*psBuffer += sThisWrite;

	*pcb = sThisWrite.GetLength();
	sThisWrite.ReleaseBuffer();
	return 0;
}

DWORD __stdcall MEditStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString *psBuffer = (CString *)dwCookie;

	if (cb < psBuffer->GetLength()) cb = psBuffer->GetLength();

	for (int i=0;i<cb;i++)
	{
		*(pbBuff+i) = psBuffer->GetAt(i);
	}

	*pcb = cb;

	*psBuffer = psBuffer->Mid(cb);

	return 0;
}

CVoid SetDialogData2( CBool selected, CPolyGroup* group, CBool engineObject, CBool showDialog )
{
	if( engineObject )
		g_showArrow = CTrue;
	else
		g_showArrow = CFalse;

	if( selected )
	{
		//selected object
		COLORREF color = COLOR_WHITE;
		CHARFORMAT cf;
		cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
		cf.dwEffects = NULL;
		cf.crTextColor = color;

		ex_pEditMaterial->SetPhysX( NULL );

		ex_pStaticSelectedObject->SetWindowTextA( "\n" );
		CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
		ex_pStaticSelectedObject->SetSel(nSel, nSel);

		ex_pStaticSelectedObject->SetSelectionCharFormat(cf);
		ex_pStaticSelectedObject->ReplaceSel( group->m_materialName );

		///////

		if( g_render.GetSelectedScene()->m_hasAnimation )
		{
			g_render.GetSelectedScene()->SetCurrentClipIndex();

			if( g_render.GetSelectedScene()->m_animationStatus == eANIM_PLAY )
			{
				ex_pBtnPlayAnim->EnableWindow( FALSE );
				ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
				ex_pBtnPauseAnim->EnableWindow( TRUE );
				ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
			}
			else
			{
				ex_pBtnPlayAnim->EnableWindow( TRUE );
				ex_pBtnPlayAnim->ShowWindow( SW_SHOW );
				ex_pBtnPauseAnim->EnableWindow( FALSE );
				ex_pBtnPauseAnim->ShowWindow( SW_HIDE );
			}
		}
		else
		{
			ex_pBtnPlayAnim->EnableWindow( FALSE );
			ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
			ex_pBtnPauseAnim->EnableWindow( FALSE );
			ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
			ex_pBtnPrevAnim->EnableWindow( FALSE );
			ex_pBtnNextAnim->EnableWindow( FALSE );
		}

		if( showDialog )
		{
			//material editor
			ex_pBtnMaterialEditor->EnableWindow( TRUE );
			ex_pMenu->EnableMenuItem( ID_TOOLS_MATERIALEDITOR, MF_ENABLED );

			ex_pEditMaterial->SetNormalBtnState( CTrue );
			ex_pEditMaterial->SetDirtBtnState( CTrue );
			ex_pEditMaterial->SetDiffuseBtnState( CTrue );
			ex_pEditMaterial->SetAlphaBtnState( CTrue );
			ex_pEditMaterial->SetGlossBtnState( CTrue );
			ex_pEditMaterial->SetShadowBtnState( CTrue );

			ex_pEditMaterial->SetEditBoxNormalMapName( GetAfterPath(group->m_strNormalMap) );
			ex_pEditMaterial->SetEditBoxDirtMapName( GetAfterPath(group->m_strDirtMap) );
			ex_pEditMaterial->SetEditBoxDiffuseName( GetAfterPath(group->m_strDiffuse) );
			ex_pEditMaterial->SetEditBoxAlphaMapName( GetAfterPath(group->m_strAlphaMap) );
			ex_pEditMaterial->SetEditBoxGlossMapName( GetAfterPath(group->m_strGlossMap) );
			ex_pEditMaterial->SetEditBoxShadowMapName( GetAfterPath(group->m_strShadowMap) );

			ex_pEditMaterial->SetEditBoxBias( group->m_parallaxMapBias );
			ex_pEditMaterial->SetEditBoxScale( group->m_parallaxMapScale );

			if( Cmp( group->m_strDirtMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveDirtBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveDirtBtnState( CFalse );

			if( Cmp( group->m_strNormalMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveNormalBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveNormalBtnState( CFalse );

			if( Cmp( group->m_strDiffuse, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveDiffuseBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveDiffuseBtnState( CFalse );

			if( Cmp( group->m_strAlphaMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveAlphaBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveAlphaBtnState( CFalse );

			if( Cmp( group->m_strGlossMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveGlossBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveGlossBtnState( CFalse );

			if( Cmp( group->m_strShadowMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveShadowBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveShadowBtnState( CFalse );
		} //if showDialog
	}
	else
	{
		//material editor
		ex_pEditMaterial->SetPhysX( NULL );

		ex_pBtnMaterialEditor->EnableWindow( FALSE );
		ex_pMenu->EnableMenuItem( ID_TOOLS_MATERIALEDITOR,  MF_DISABLED | MF_GRAYED );

		ex_pBtnScriptEditor->EnableWindow( FALSE );
		ex_pMenu->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_DISABLED | MF_GRAYED );

		ex_pEditMaterial->SetNormalBtnState( CFalse );
		ex_pEditMaterial->SetDirtBtnState( CFalse );
		ex_pEditMaterial->SetDiffuseBtnState( CFalse );
		ex_pEditMaterial->SetAlphaBtnState( CFalse );
		ex_pEditMaterial->SetGlossBtnState( CFalse );
		ex_pEditMaterial->SetShadowBtnState( CFalse );

		ex_pEditMaterial->SetEditBoxNormalMapName( "\n" );
		ex_pEditMaterial->SetEditBoxDirtMapName( "\n" );
		ex_pEditMaterial->SetEditBoxDiffuseName( "\n" );
		ex_pEditMaterial->SetEditBoxAlphaMapName( "\n" );
		ex_pEditMaterial->SetEditBoxGlossMapName( "\n" );
		ex_pEditMaterial->SetEditBoxShadowMapName( "\n" );

		ex_pEditMaterial->SetRemoveDirtBtnState( CFalse );
		ex_pEditMaterial->SetRemoveNormalBtnState( CFalse );
		ex_pEditMaterial->SetRemoveDiffuseBtnState( CFalse );
		ex_pEditMaterial->SetRemoveAlphaBtnState( CFalse );
		ex_pEditMaterial->SetRemoveGlossBtnState( CFalse );
		ex_pEditMaterial->SetRemoveShadowBtnState( CFalse );
		/////

		ex_pBtnPlayAnim->EnableWindow( FALSE );
		ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
		ex_pBtnPauseAnim->EnableWindow( FALSE );
		ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
		ex_pBtnPrevAnim->EnableWindow( FALSE );
		ex_pBtnNextAnim->EnableWindow( FALSE );

		ex_pStaticSelectedObject->SetWindowTextA( "\n" );
	}
}

CVoid SetDialogData( CBool selected, CInstanceGeometry* instanceGeo, CGeometry* geo, CBool engineObject, CBool showDialog )
{
	if( engineObject )
		g_showArrow = CTrue;
	else
		g_showArrow = CFalse;

	if( selected )
	{
		//set cull face check mark of the main dialog's menu
		if( geo->m_cullFaces )
			CheckCullFace( eCULLFACETYPEFORMENU_CHECKED );
		else
			CheckCullFace( eCULLFACETYPEFORMENU_UNCHECKED );
	}
	else
		CheckCullFace( eCULLFACETYPEFORMENU_GRAYED );

	if( selected )
	{
		//selected object
		COLORREF color = COLOR_WHITE;
		CHARFORMAT cf;
		cf.dwMask = CFM_COLOR;
		cf.dwEffects = NULL;
		cf.crTextColor = color;

		ex_pStaticSelectedObject->SetWindowTextA( "\n" );
		CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
		ex_pStaticSelectedObject->SetSel(nSel, nSel);

		ex_pStaticSelectedObject->SetSelectionCharFormat(cf);
		ex_pStaticSelectedObject->ReplaceSel( geo->GetName() );

		////////
		if( g_render.GetSelectedScene()->m_hasAnimation )
		{
			g_render.GetSelectedScene()->SetCurrentClipIndex();
			if( g_render.GetSelectedScene()->m_animationStatus == eANIM_PLAY )
			{
				ex_pBtnPlayAnim->EnableWindow( FALSE );
				ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
				ex_pBtnPauseAnim->EnableWindow( TRUE );
				ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
			}
			else
			{
				ex_pBtnPlayAnim->EnableWindow( TRUE );
				ex_pBtnPlayAnim->ShowWindow( SW_SHOW );
				ex_pBtnPauseAnim->EnableWindow( FALSE );
				ex_pBtnPauseAnim->ShowWindow( SW_HIDE );
			}
		}
		else
		{
			ex_pBtnPlayAnim->EnableWindow( FALSE );
			ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
			ex_pBtnPauseAnim->EnableWindow( FALSE );
			ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
			ex_pBtnPrevAnim->EnableWindow( FALSE );
			ex_pBtnNextAnim->EnableWindow( FALSE );
		}

		if( showDialog )
		{
			//material editor
			ex_pBtnMaterialEditor->EnableWindow( TRUE );
			ex_pMenu->EnableMenuItem( ID_TOOLS_MATERIALEDITOR, MF_ENABLED );

			if( instanceGeo->m_isTrigger )
			{
				ex_pBtnScriptEditor->EnableWindow( TRUE );
				ex_pMenu->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_ENABLED );
				ex_pAddScript->SetInstanceGeo( instanceGeo );
			}

			ex_pEditMaterial->SetNormalBtnState( CTrue );
			ex_pEditMaterial->SetDirtBtnState( CTrue );
			ex_pEditMaterial->SetDiffuseBtnState( CTrue );
			ex_pEditMaterial->SetAlphaBtnState( CTrue );
			ex_pEditMaterial->SetGlossBtnState( CTrue );
			ex_pEditMaterial->SetShadowBtnState( CTrue );

			ex_pEditMaterial->SetEditBoxNormalMapName( GetAfterPath(geo->m_strNormalMap) );
			ex_pEditMaterial->SetEditBoxDirtMapName( GetAfterPath(geo->m_strDirtMap) );
			ex_pEditMaterial->SetEditBoxDiffuseName( GetAfterPath(geo->m_strDiffuse) );
			ex_pEditMaterial->SetEditBoxAlphaMapName( GetAfterPath(geo->m_strAlphaMap) );
			ex_pEditMaterial->SetEditBoxGlossMapName( GetAfterPath(geo->m_strGlossMap) );
			ex_pEditMaterial->SetEditBoxShadowMapName( GetAfterPath(geo->m_strShadowMap) );

			ex_pEditMaterial->SetEditBoxBias( geo->m_parallaxMapBias );
			ex_pEditMaterial->SetEditBoxScale( geo->m_parallaxMapScale );
			ex_pEditMaterial->SetPhysX( instanceGeo );

			if( Cmp( geo->m_strDirtMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveDirtBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveDirtBtnState( CFalse );

			if( Cmp( geo->m_strNormalMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveNormalBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveNormalBtnState( CFalse );

			if( Cmp( geo->m_strDiffuse, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveDiffuseBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveDiffuseBtnState( CFalse );

			if( Cmp( geo->m_strAlphaMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveAlphaBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveAlphaBtnState( CFalse );

			if( Cmp( geo->m_strGlossMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveGlossBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveGlossBtnState( CFalse );

			if( Cmp( geo->m_strShadowMap, "\n" ) == CFalse )
				ex_pEditMaterial->SetRemoveShadowBtnState( CTrue );
			else
				ex_pEditMaterial->SetRemoveShadowBtnState( CFalse );
		} //if showDialog
	}
	else
	{
		//material editor
		ex_pBtnMaterialEditor->EnableWindow( FALSE );
		ex_pMenu->EnableMenuItem( ID_TOOLS_MATERIALEDITOR, MF_DISABLED | MF_GRAYED );

		ex_pBtnScriptEditor->EnableWindow( FALSE );
		ex_pMenu->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_DISABLED | MF_GRAYED );

		ex_pEditMaterial->SetNormalBtnState( CFalse );
		ex_pEditMaterial->SetDirtBtnState( CFalse );
		ex_pEditMaterial->SetDiffuseBtnState( CFalse );
		ex_pEditMaterial->SetAlphaBtnState( CFalse );
		ex_pEditMaterial->SetGlossBtnState( CFalse );
		ex_pEditMaterial->SetShadowBtnState( CFalse );

		ex_pEditMaterial->SetEditBoxNormalMapName( "\n" );
		ex_pEditMaterial->SetEditBoxDirtMapName( "\n" );
		ex_pEditMaterial->SetEditBoxDiffuseName( "\n" );
		ex_pEditMaterial->SetEditBoxAlphaMapName( "\n" );
		ex_pEditMaterial->SetEditBoxGlossMapName( "\n" );
		ex_pEditMaterial->SetEditBoxShadowMapName( "\n" );

		ex_pEditMaterial->SetRemoveDirtBtnState( CFalse );
		ex_pEditMaterial->SetRemoveNormalBtnState( CFalse );
		ex_pEditMaterial->SetRemoveDiffuseBtnState( CFalse );
		ex_pEditMaterial->SetRemoveAlphaBtnState( CFalse );
		ex_pEditMaterial->SetRemoveGlossBtnState( CFalse );
		ex_pEditMaterial->SetRemoveShadowBtnState( CFalse );

		ex_pEditMaterial->SetPhysX( NULL );

		///////////////////////////

		ex_pBtnPlayAnim->EnableWindow( FALSE );
		ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
		ex_pBtnPauseAnim->EnableWindow( FALSE );
		ex_pBtnPauseAnim->ShowWindow( SW_SHOW );
		ex_pBtnPrevAnim->EnableWindow( FALSE );
		ex_pBtnNextAnim->EnableWindow( FALSE );

		ex_pStaticSelectedObject->SetWindowTextA( "\n" );
	}
}

CVoid CheckCullFace( CCullFaceTypeForMenu type )
{
	if( type == eCULLFACETYPEFORMENU_CHECKED )
	{
		ex_pMenu->EnableMenuItem( ID_TOOLS_CULLFACES, MF_ENABLED );
		ex_pMenu->CheckMenuItem( ID_TOOLS_CULLFACES, MF_CHECKED );
	}
	else if( type == eCULLFACETYPEFORMENU_UNCHECKED )
	{
		ex_pMenu->EnableMenuItem( ID_TOOLS_CULLFACES, MF_ENABLED );
		ex_pMenu->CheckMenuItem( ID_TOOLS_CULLFACES, MF_UNCHECKED );
	}
	else if( type == eCULLFACETYPEFORMENU_GRAYED )
	{
		ex_pMenu->CheckMenuItem( ID_TOOLS_CULLFACES, MF_UNCHECKED );
		ex_pMenu->EnableMenuItem( ID_TOOLS_CULLFACES, MF_GRAYED );
	}
}


CVoid PrintShaderLog( CString strText )
{
	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
	cf.dwEffects = NULL;
	cf.crTextColor = COLOR_RED;

	CInt nSel = ex_pRichEdit->GetWindowTextLength();
	ex_pRichEdit->SetSel(nSel, nSel);

	ex_pRichEdit->SetSelectionCharFormat(cf);
	ex_pRichEdit->ReplaceSel(strText.GetString());
	ex_pRichEdit->UpdateData(TRUE);
	ex_pRichEdit->UpdateWindow();
}

CVoid PrintInfo( CString strText, COLORREF color, CBool resetCounter )
{
	if( g_reportInfo )
	{
		static CInt reportCounter = 1;
		if( resetCounter )
			reportCounter = 0;
		CChar tempName[MAX_NAME_SIZE];
		Cpy( tempName, strText.GetString() );
		CChar* str = tempName;
		CChar str2 [ MAX_NAME_SIZE ];

		if( CmpIn( str, "\n" ) )
		{
			reportCounter++;
			str += sizeof( CChar); //skip first endline
			sprintf(str2, "\nLn %i> %s", reportCounter, str);
		}
		else
		{
			sprintf(str2, "%s", str);
		}

		CHARFORMAT2 cf;
		cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
		cf.dwEffects = NULL;
		cf.crTextColor = color;
		//cf.yHeight = CInt( rect.bottom - rect.top ) * 10.5 ;

		CInt nSel = ex_pRichEdit->GetWindowTextLength();
		ex_pRichEdit->SetSel(nSel, nSel);

		ex_pRichEdit->SetSelectionCharFormat(cf);
		ex_pRichEdit->ReplaceSel(str2);
		ex_pRichEdit->UpdateData(TRUE);
		ex_pRichEdit->UpdateWindow();
	}
}

CVoid PrintInfo2( CString strText, COLORREF color )
{
	//CRect rect;
	//ex_pRichEdit->GetWindowRect(&rect);
	static CInt reportCounter = 1;
	CChar temp[MAX_NAME_SIZE];
	if( reportCounter != 1 )
		sprintf( temp, "\nLn %i> %s", reportCounter, strText.GetString() );
	else
		sprintf( temp, "Ln %i> %s", reportCounter, strText.GetString() );

	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
	cf.dwEffects = NULL;
	cf.crTextColor = color;
	//cf.yHeight = CInt( rect.bottom - rect.top ) * 10.5 ;

	CInt nSel = ex_pRichEdit2->GetWindowTextLength();
	ex_pRichEdit2->SetSel(nSel, nSel);

	ex_pRichEdit2->SetSelectionCharFormat(cf);
	ex_pRichEdit2->ReplaceSel(temp);
	ex_pRichEdit2->UpdateData(TRUE);
	ex_pRichEdit2->UpdateWindow();

	reportCounter++;
}


CVoid PrintInfo3( CString strText )
{
	//COLORREF color;
	//static CInt index = 0;
	//if( index % 2 == 0 )
	//	color = COLOR_RED_GREEN;
	//else
	//	color = COLOR_WHITE;
	//index++;
		
	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = NULL;
	cf.crTextColor = COLOR_WHITE;

	CInt nSel = ex_pRichEdit3->GetWindowTextLength();
	ex_pRichEdit3->SetSel(nSel, nSel);

	ex_pRichEdit3->SetSelectionCharFormat(cf);
	ex_pRichEdit3->ReplaceSel(strText.GetString() );
	ex_pRichEdit3->UpdateData(TRUE);
	ex_pRichEdit3->UpdateWindow();

}

CVoid PrintScriptInfo( CString strText, COLORREF color )
{
	//CRect rect;
	//ex_pRichEdit->GetWindowRect(&rect);
	static CInt reportCounter = 1;
	CChar temp[MAX_NAME_SIZE];
	if( reportCounter != 1 )
		sprintf( temp, "\nLn %i> %s", reportCounter, strText.GetString() );
	else
		sprintf( temp, "Ln %i> %s", reportCounter, strText.GetString() );

	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
	cf.dwEffects = NULL;
	cf.crTextColor = color;
	//cf.yHeight = CInt( rect.bottom - rect.top ) * 10.5 ;

	CInt nSel = ex_pRichEditScript->GetWindowTextLength();
	ex_pRichEditScript->SetSel(nSel, nSel);

	ex_pRichEditScript->SetSelectionCharFormat(cf);
	ex_pRichEditScript->ReplaceSel(temp);
	ex_pRichEditScript->UpdateData(TRUE);
	ex_pRichEditScript->UpdateWindow();

	reportCounter++;
}

// Definition: relative pixel = 1 pixel at 96 DPI and scaled based on actual DPI.
class CDPI
{
public:
    CDPI() : _fInitialized(false), _dpiX(96), _dpiY(96), _resX(1024), _resY(768) { }
    
    // Get screen DPI.
    int GetDPIX() { _Init(); return _dpiX; }
    int GetDPIY() { _Init(); return _dpiY; }

	int GetResX() { _Init(); return _resX; }
	int GetResY() { _Init(); return _resY; }

    // Convert between raw pixels and relative pixels.
    int ScaleX(int x) { _Init(); return MulDiv(x, _dpiX, 96); }
    int ScaleY(int y) { _Init(); return MulDiv(y, _dpiY, 96); }
    int UnscaleX(int x) { _Init(); return MulDiv(x, 96, _dpiX); }
    int UnscaleY(int y) { _Init(); return MulDiv(y, 96, _dpiY); }

    // Determine the screen dimensions in relative pixels.
    int ScaledScreenWidth() { return _ScaledSystemMetricX(SM_CXSCREEN); }
    int ScaledScreenHeight() { return _ScaledSystemMetricY(SM_CYSCREEN); }

    // Scale rectangle from raw pixels to relative pixels.
    void ScaleRect(__inout RECT *pRect)
    {
        pRect->left = ScaleX(pRect->left);
        pRect->right = ScaleX(pRect->right);
        pRect->top = ScaleY(pRect->top);
        pRect->bottom = ScaleY(pRect->bottom);
    }
    // Determine if screen resolution meets minimum requirements in relative
    // pixels.
    bool IsResolutionAtLeast(int cxMin, int cyMin) 
    { 
        return (ScaledScreenWidth() >= cxMin) && (ScaledScreenHeight() >= cyMin); 
    }

    // Convert a point size (1/72 of an inch) to raw pixels.
    int PointsToPixels(int pt) { _Init(); return MulDiv(pt, _dpiY, 72); }

    // Invalidate any cached metrics.
    void Invalidate() { _fInitialized = false; }

private:
    void _Init()
    {
        if (!_fInitialized)
        {
            HDC hdc = GetDC(NULL);
            if (hdc)
            {
                _dpiX = GetDeviceCaps(hdc, HORZSIZE );
                _dpiY = GetDeviceCaps(hdc, LOGPIXELSY);

				_resX = GetDeviceCaps(hdc, HORZRES );
				_resY = GetDeviceCaps(hdc, VERTRES );
                ReleaseDC(NULL, hdc);
            }
            _fInitialized = true;
        }
    }

    int _ScaledSystemMetricX(int nIndex) 
    { 
        _Init(); 
        return MulDiv(GetSystemMetrics(nIndex), 96, _dpiX); 
    }

    int _ScaledSystemMetricY(int nIndex) 
    { 
        _Init(); 
        return MulDiv(GetSystemMetrics(nIndex), 96, _dpiY); 
    }
private:
    bool _fInitialized;

    int _dpiX;
    int _dpiY;

	int _resX;
	int _resY;
};

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual CVoid DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

CVoid CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVandaEngine1Dlg dialog


CVandaEngine1Dlg::CVandaEngine1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVandaEngine1Dlg::IDD, pParent)
{

	SetCapture();
	SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );

	m_pToolTip = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_VANDAENGINE);

	ex_pRichEdit = &m_rich;
	ex_pRichEdit2 = &m_rich2;
	ex_pRichEdit3 = &m_rich3;

	ex_pStaticSelectedObject = &m_staticSelectedObject;

	//Initialize material editor
	ex_pEditMaterial = m_dlgEditMaterial = CNew( CEditMaterial );
	ex_pAddScript = m_dlgAddScript = CNew (CAddScript);

	ex_pBtnPrevAnim = &m_mainBtnPrevAnim;
	ex_pBtnNextAnim = &m_mainBtnNextAnim;
	ex_pBtnPlayAnim = &m_mainBtnPlayAnim;
	ex_pBtnPauseAnim = &m_mainBtnPauseAnim;
	ex_pBtnMaterialEditor = &m_mainBtnMaterial;
	ex_pBtnScriptEditor = &m_mainBtnScriptManager;

	m_savePathName = "\n";
	m_horizontalSizeOfLeftCulomn = 0;
	m_horizontalSizeOfRightCulomn = 0;
	m_startLeftButtons = 6; //six percent

	m_engineObjectListIndex = -1;
	m_sceneListIndex = -1;
	m_physXElementListIndex = -1;

	g_useGlobalAmbientColor = CFalse;
	g_globalAmbientColor.r = g_globalAmbientColor.g = g_globalAmbientColor.b = 0.5f;
	g_globalAmbientColor.a = 1.0f;
	m_askRemoveScene = CTrue;

}

CVandaEngine1Dlg::~CVandaEngine1Dlg()
{
	g_allVSceneNames.clear();
	g_allVSceneNamesToBePublished.clear();
	if( g_multipleView->m_nx->m_hasScene )
	{
		ResetPhysX();
		CDelete(g_externalPhysX);
	}
	//delete material editor
	CDelete( m_dlgEditMaterial );
	CDelete( m_dlgAddScript );
	CDelete( m_pToolTip );
	//Delete ambient sound
	CDelete( g_multipleView );
	ex_pRichEdit = NULL;
	for( std::vector<CScene*>::iterator it = g_scene.begin(); it != g_scene.end(); it++ )
	{
		CDelete( *it );
	}
	//Clear the vctor objects
	g_scene.clear();
	g_render.Destroy();

	CDelete( g_skyDome );

	for( std::vector<CInstanceLight*>::iterator it = g_engineLights.begin(); it != g_engineLights.end(); it++ )
	{
		CDelete( (*it)->m_abstractLight );
		CDelete( *it );
	}
	g_engineLights.clear();

	for( std::vector<CWater*>::iterator it = g_engineWaters.begin(); it != g_engineWaters.end(); it++ )
	{
		CDelete( *it );
	}
	g_engineWaters.clear();

	for( std::vector<CStaticSound*>::iterator it = g_engineStaticSounds.begin(); it != g_engineStaticSounds.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_engineStaticSounds.size() > 0 )
		g_engineStaticSounds.clear();

	if( g_engineObjectNames.size() > 0 )
		g_engineObjectNames.clear();
	//delete the static sound buffers
	for( std::vector<COpenALSoundBuffer*>::iterator it = g_soundBuffers.begin(); it != g_soundBuffers.end(); it++ )
	{
		CDelete( *it );
	}
	g_soundBuffers.clear();
	CDelete( g_arrowScene );
	CDelete( g_negativeZArrow );
	//CDelete (g_zArrow);
	CDelete( g_centerArrowScene );
	for( std::vector<CImage*>::iterator it = g_images.begin(); it != g_images.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_images.size() > 0 )
		g_images.clear();

	CDelete( g_bloom );

	for( std::vector<CImage*>::iterator it = g_waterImages.begin(); it != g_waterImages.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_waterImages.size() > 0 )
		g_waterImages.clear();

	//Delete all items from the list boxes
	for (int nItem = m_listBoxScenes.GetItemCount()-1; nItem >= 0 ;nItem-- )
	{
		m_listBoxScenes.DeleteItem(nItem);
	}
	for (int nItem = m_listBoxObjects.GetItemCount(); nItem >= 0 ;nItem-- )
	{
		m_listBoxObjects.DeleteItem(nItem);
	}
}

CVoid CVandaEngine1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_rich);
	DDX_Control(pDX, IDC_RICHEDIT22, m_rich2);
	DDX_Control(pDX, IDC_LIST_SCENES, m_listBoxScenes);
	DDX_Control(pDX, IDC_LIST_OBJECTS, m_listBoxObjects);
	DDX_Control(pDX, IDC_BTN_REMOVE_SCENE, m_btnRemoveScene);
	DDX_Control(pDX, IDC_BTN_REMOVE_OBJECT, m_btnRemoveObject);
	DDX_Control(pDX, IDC_LIST_ENGINE_OBJECTS, m_listBoxEngineObjects);
	DDX_Control(pDX, IDC_BTN_REMOVE_ENGINE_OBJECT, m_btnRemoveEngineObject);
	DDX_Control(pDX, IDC_BTN_ENGINE_OBJECT_PROPERTIES, m_btnEngineObjectProperties);
	DDX_Control(pDX, IDC_BTN_NEW, m_mainBtnNew);
	DDX_Control(pDX, IDC_BTN_SAVE, m_mainBtnSave);
	DDX_Control(pDX, IDC_BTN_COLLADA, m_mainBtnImportCollada);
	DDX_Control(pDX, IDC_BTN_PHYSX, m_mainBtnImportPhysX);
	DDX_Control(pDX, IDC_BTN_ANIM_PREV, m_mainBtnPrevAnim);
	DDX_Control(pDX, IDC_BTN_OPEN, m_mainBtnOpen);
	DDX_Control(pDX, IDC_BTN_SAVEAS, m_mainBtnSaveAs);
	DDX_Control(pDX, IDC_BTN_ANIM_PLAY, m_mainBtnPlayAnim);
	DDX_Control(pDX, IDC_BTN_ANIM_NEXT, m_mainBtnNextAnim);
	DDX_Control(pDX, IDC_BTN_TIMER, m_mainBtnTimer);
	DDX_Control(pDX, IDC_BTN_MATERIAL, m_mainBtnMaterial);
	DDX_Control(pDX, IDC_BTN_ANIM_PAUSE, m_mainBtnPauseAnim);
	DDX_Control(pDX, IDC_BTN_WEB, m_mainBtnWeb);
	DDX_Control(pDX, IDC_BTN_FACEBOOK, m_mainBtnFacebook);
	DDX_Control(pDX, IDC_STATIC_SELECTEDOBJECT, m_staticSelectedObject);
	DDX_Control(pDX, IDC_BTN_SKY, m_mainBtnSky);
	DDX_Control(pDX, IDC_BTN_WATER, m_mainBtnWater);
	DDX_Control(pDX, IDC_BTN_STATICSOUND, m_mainBtnStaticSound);
	DDX_Control(pDX, IDC_BTN_AMBIENTSOUND, m_mainBtnAmbientSound);
	DDX_Control(pDX, IDC_BTN_PLAYER, m_mainBtnPlayer);
	DDX_Control(pDX, IDC_BTN_LIGHT, m_mainBtnLight);
	DDX_Control(pDX, IDC_BTN_CONSOLE, m_mainBtnConsole);
	DDX_Control(pDX, IDC_BTN_SUMMARY, m_mainBtnSummary);
	DDX_Control(pDX, IDC_RICHEDIT23, m_rich3);
	DDX_Control(pDX, IDC_LIST_PHYSX_ELEMENTS, m_listBoxPhysXElements);
	DDX_Control(pDX, IDC_BTN_REMOVE_PHYSX, m_btnRemovePhysX);
	DDX_Control(pDX, IDC_BTN_VANDA_TEXT, m_btnVandaText);
	DDX_Control(pDX, IDC_BTN_TWITTER, m_mainBtnTwitter);
	DDX_Control(pDX, IDC_BTN_YOUTUBE, m_mainBtnYoutube);
	DDX_Control(pDX, IDC_BTN_PUBLISH_SOLUTION, m_mainBtnPublishSolution);
	DDX_Control(pDX, IDC_BTN_SCRIPT_MANAGER, m_mainBtnScriptManager);
	DDX_Control(pDX, IDC_BTN_CAMERA_RENDERING_MANAGER, m_mainBtnCameraAndRenderingManager);
	DDX_Control(pDX, IDC_BTN_VANDA_VERSION, m_btnVandaVersion);
	DDX_Control(pDX, IDC_BTN_SCENE_PROPERTIES, m_btnSceneProperties);
}

BEGIN_MESSAGE_MAP(CVandaEngine1Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BTN_REMOVE_SCENE, &CVandaEngine1Dlg::OnBnClickedBtnRemoveScene)
	ON_BN_CLICKED(IDC_BTN_REMOVE_OBJECT, &CVandaEngine1Dlg::OnBnClickedBtnRemoveObject)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_REMOVE_ENGINE_OBJECT, &CVandaEngine1Dlg::OnBnClickedBtnRemoveEngineObject)
	ON_BN_CLICKED(IDC_BTN_ENGINE_OBJECT_PROPERTIES, &CVandaEngine1Dlg::OnBnClickedBtnEngineObjectProperties)
	ON_WM_CLOSE()
//	ON_NOTIFY(NM_RETURN, IDC_LIST_SCENES, &CVandaEngine1Dlg::OnNMReturnListScenes)
ON_BN_CLICKED(IDC_BTN_NEW, &CVandaEngine1Dlg::OnBnClickedBtnNew)
ON_BN_CLICKED(IDC_BTN_SAVE, &CVandaEngine1Dlg::OnBnClickedBtnSave)
ON_BN_CLICKED(IDC_BTN_COLLADA, &CVandaEngine1Dlg::OnBnClickedBtnCollada)
ON_BN_CLICKED(IDC_BTN_PHYSX, &CVandaEngine1Dlg::OnBnClickedBtnPhysx)
ON_BN_CLICKED(IDC_BTN_ANIM_PREV, &CVandaEngine1Dlg::OnBnClickedBtnAnimPrev)
ON_BN_CLICKED(IDC_BTN_OPEN, &CVandaEngine1Dlg::OnBnClickedBtnOpen)
ON_BN_CLICKED(IDC_BTN_SAVEAS, &CVandaEngine1Dlg::OnBnClickedBtnSaveas)
ON_BN_CLICKED(IDC_BTN_ANIM_NEXT, &CVandaEngine1Dlg::OnBnClickedBtnAnimNext)
ON_BN_CLICKED(IDC_BTN_ANIM_PLAY, &CVandaEngine1Dlg::OnBnClickedBtnAnimPlay)
ON_WM_SETCURSOR()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_TIMER, &CVandaEngine1Dlg::OnBnClickedBtnTimer)
ON_BN_CLICKED(IDC_BTN_ANIM_PAUSE, &CVandaEngine1Dlg::OnBnClickedBtnAnimPause)
ON_BN_CLICKED(IDC_BTN_WEB, &CVandaEngine1Dlg::OnBnClickedBtnWeb)
ON_BN_CLICKED(IDC_BTN_FACEBOOK, &CVandaEngine1Dlg::OnBnClickedBtnFacebook)
ON_BN_CLICKED(IDC_BTN_MATERIAL, &CVandaEngine1Dlg::OnBnClickedBtnMaterial)
ON_BN_CLICKED(IDC_BTN_LIGHT, &CVandaEngine1Dlg::OnBnClickedBtnLight)
ON_BN_CLICKED(IDC_BTN_WATER, &CVandaEngine1Dlg::OnBnClickedBtnWater)
ON_BN_CLICKED(IDC_BTN_AMBIENTSOUND, &CVandaEngine1Dlg::OnBnClickedBtnAmbientsound)
ON_BN_CLICKED(IDC_BTN_STATICSOUND, &CVandaEngine1Dlg::OnBnClickedBtnStaticsound)
ON_BN_CLICKED(IDC_BTN_SKY, &CVandaEngine1Dlg::OnBnClickedBtnSky)
ON_BN_CLICKED(IDC_BTN_PLAYER, &CVandaEngine1Dlg::OnBnClickedBtnPlayer)
ON_BN_CLICKED(IDC_BTN_CONSOLE, &CVandaEngine1Dlg::OnBnClickedBtnConsole)
ON_BN_CLICKED(IDC_BTN_SUMMARY, &CVandaEngine1Dlg::OnBnClickedBtnSummary)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SCENES, &CVandaEngine1Dlg::OnLvnItemchangedListScenes)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ENGINE_OBJECTS, &CVandaEngine1Dlg::OnLvnItemchangedListEngineObjects)
ON_BN_CLICKED(IDC_BTN_REMOVE_PHYSX, &CVandaEngine1Dlg::OnBnClickedBtnRemovePhysx)
ON_BN_CLICKED(IDC_BTN_TWITTER, &CVandaEngine1Dlg::OnBnClickedBtnTwitter)
ON_BN_CLICKED(IDC_BTN_YOUTUBE, &CVandaEngine1Dlg::OnBnClickedBtnYoutube)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PHYSX_ELEMENTS, &CVandaEngine1Dlg::OnLvnItemchangedListPhysxElements)
ON_BN_CLICKED(IDC_BTN_PUBLISH_SOLUTION, &CVandaEngine1Dlg::OnBnClickedBtnPublishSolution)
ON_BN_CLICKED(IDC_BTN_SCRIPT_MANAGER, &CVandaEngine1Dlg::OnBnClickedBtnScriptManager)
ON_BN_CLICKED(IDC_BTN_CAMERA_RENDERING_MANAGER, &CVandaEngine1Dlg::OnBnClickedBtnCameraRenderingManager)
ON_BN_CLICKED(IDC_BTN_SCENE_PROPERTIES, &CVandaEngine1Dlg::OnBnClickedBtnSceneProperties)
END_MESSAGE_MAP()


// CVandaEngine1Dlg message handlers

BOOL CVandaEngine1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->RemoveMenu( SC_MOVE, MF_BYCOMMAND );
		pSysMenu->RemoveMenu( SC_SIZE, MF_BYCOMMAND );
		pSysMenu->RemoveMenu( SC_RESTORE, MF_BYCOMMAND );
	//	CString strAboutMenu;
	//	strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	}

	m_pToolTip = new CToolTipCtrl;
	if(!m_pToolTip->Create(this))
	{
	   TRACE("Unable To create ToolTip\n");
	   return TRUE;
	}

	m_pToolTip->AddTool(&m_mainBtnNew, "New scene");
	m_pToolTip->AddTool(&m_mainBtnOpen, "Open scene");
	m_pToolTip->AddTool(&m_mainBtnSave, "Save");
	m_pToolTip->AddTool(&m_mainBtnSaveAs, "Save As");
	m_pToolTip->AddTool(&m_mainBtnImportCollada, "Import COLLADA");
	m_pToolTip->AddTool(&m_mainBtnImportPhysX, "Import PhysX");
	m_pToolTip->AddTool(&m_mainBtnPublishSolution, "Publish Solution");
	m_pToolTip->AddTool(&m_mainBtnScriptManager, "Script Manager");
	m_pToolTip->AddTool(&m_mainBtnCameraAndRenderingManager, "Camera And Rendering");
	m_pToolTip->AddTool(&m_mainBtnTimer, "Enable/Disable Timer");
	m_pToolTip->AddTool(&m_mainBtnMaterial, "Open Material Editor");
	m_pToolTip->AddTool(&m_mainBtnNextAnim, "Next Animation");
	m_pToolTip->AddTool(&m_mainBtnPrevAnim, "Previous Animation");
	m_pToolTip->AddTool(&m_mainBtnPlayAnim, "Play Animation");
	m_pToolTip->AddTool(&m_mainBtnPauseAnim, "Pause Animation");
	m_pToolTip->AddTool(&m_mainBtnWeb, "www.vandaengine.com");
	m_pToolTip->AddTool(&m_mainBtnFacebook, "www.facebook.com/VandaEngine");
	m_pToolTip->AddTool(&m_mainBtnTwitter, "www.twitter.com/Vanda_Engine");
	m_pToolTip->AddTool(&m_mainBtnYoutube, "www.youtube.com/VandaEngine");

	m_pToolTip->AddTool(&m_mainBtnSky, "Create Sky");
	m_pToolTip->AddTool(&m_mainBtnWater, "Create Water");
	m_pToolTip->AddTool(&m_mainBtnAmbientSound, "Create Ambient Sound");
	m_pToolTip->AddTool(&m_mainBtnStaticSound, "Create Static Sound");
	m_pToolTip->AddTool(&m_mainBtnPlayer, "Save Current Camera and Character Status");
	m_pToolTip->AddTool(&m_mainBtnLight, "Create Light");
	m_pToolTip->AddTool(&m_mainBtnConsole, "Console Window");
	m_pToolTip->AddTool(&m_mainBtnSummary, "Errors and Warnings");


	m_pToolTip->Activate(TRUE);

	m_brush.CreateSolidBrush(RGB(255, 255, 255));
	//load the menu here
	CMenu menu;
	menu.LoadMenu (IDR_MENU1);
	SetMenu(&menu);
	menu.Detach();
	
	ex_pMenu = GetMenu();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowText(_T("VandaEngine RTI Pro Version 1.3.0"));

	// TODO: Add extra initialization here
	ShowWindow( SW_SHOWMAXIMIZED );
	UpdateWindow();

	CRect windowRect;
	GetClientRect( &windowRect );

	CRect rcRect;

	CInt fivePercent;
	CInt previousRight;
	CInt previousLeft;
	//Initialize main *New* button here
	CFloat aspectRatio = fabs( CFloat( windowRect.bottom - windowRect.top ) / CFloat( windowRect.right - windowRect.left ) );
	fivePercent = ( (5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	m_horizontalPointFivePercent = CInt( 0.5 * ( windowRect.right - windowRect.left ) / 100);

	rcRect.left = CInt( 1 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = rcRect.left + fivePercent;
	rcRect.top = CInt( ( m_startLeftButtons - 5.5)  * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (5 * ( windowRect.bottom - windowRect.top ) / 100 );
	//ScreenToClient( &rcRect );
	m_mainBtnNew.MoveWindow( rcRect );
	m_mainBtnNew.LoadBitmaps( IDB_BITMAP_NEW_UP, IDB_BITMAP_NEW_DOWN, IDB_BITMAP_NEW_FOCUS );
	m_mainBtnNew.ShowWindow( SW_SHOW );
	m_mainBtnNew.UpdateWindow();

	//Initialize main *Open* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnOpen.MoveWindow( rcRect );
	m_mainBtnOpen.LoadBitmaps( IDB_BITMAP_OPEN_UP, IDB_BITMAP_OPEN_DOWN, IDB_BITMAP_OPEN_FOCUS );
	m_mainBtnOpen.ShowWindow( SW_SHOW );
	m_mainBtnOpen.UpdateWindow();

	//Initialize main *Import COLLADA* button here

	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnImportCollada.MoveWindow( rcRect );
	m_mainBtnImportCollada.LoadBitmaps( IDB_BITMAP_COLLADA_UP, IDB_BITMAP_COLLADA_DOWN, IDB_BITMAP_COLLADA_FOCUS );
	m_mainBtnImportCollada.ShowWindow( SW_SHOW );
	m_mainBtnImportCollada.UpdateWindow();

	//Initialize main *Import PhysX* button here

	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnImportPhysX.MoveWindow( rcRect );
	m_mainBtnImportPhysX.LoadBitmaps( IDB_BITMAP_PHYSX_UP, IDB_BITMAP_PHYSX_DOWN, IDB_BITMAP_PHYSX_FOCUS );
	m_mainBtnImportPhysX.ShowWindow( SW_SHOW );
	m_mainBtnImportPhysX.UpdateWindow();

	//Initialize main *Save* button here

	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnSave.MoveWindow( rcRect );
	m_mainBtnSave.LoadBitmaps( IDB_BITMAP_SAVE_UP, IDB_BITMAP_SAVE_DOWN, IDB_BITMAP_SAVE_FOCUS );
	m_mainBtnSave.ShowWindow( SW_SHOW );
	m_mainBtnSave.UpdateWindow();

	//Initialize main *Save As* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnSaveAs.MoveWindow( rcRect );
	m_mainBtnSaveAs.LoadBitmaps( IDB_BITMAP_SAVEAS_UP, IDB_BITMAP_SAVEAS_DOWN, IDB_BITMAP_SAVEAS_FOCUS );
	m_mainBtnSaveAs.ShowWindow( SW_SHOW );
	m_mainBtnSaveAs.UpdateWindow();

	//Initialize main *Publish* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnPublishSolution.MoveWindow( rcRect );
	m_mainBtnPublishSolution.LoadBitmaps( IDB_BITMAP_PUBLISH_UP, IDB_BITMAP_PUBLISH_DOWN, IDB_BITMAP_PUBLISH_FOCUS );
	m_mainBtnPublishSolution.ShowWindow( SW_SHOW );
	m_mainBtnPublishSolution.UpdateWindow();

	//Initialize main *Camera And Rendering* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnCameraAndRenderingManager.MoveWindow( rcRect );
	m_mainBtnCameraAndRenderingManager.LoadBitmaps( IDB_BITMAP_CAMERA_UP, IDB_BITMAP_CAMERA_DOWN, IDB_BITMAP_CAMERA_FOCUS );
	m_mainBtnCameraAndRenderingManager.ShowWindow( SW_SHOW );
	m_mainBtnCameraAndRenderingManager.UpdateWindow();

	//Initialize main *Previous Animation* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnPrevAnim.MoveWindow( rcRect );
	m_mainBtnPrevAnim.LoadBitmaps( IDB_BITMAP_ANIM_PREV_UP, IDB_BITMAP_ANIM_PREV_DOWN, IDB_BITMAP_ANIM_PREV_FOCUS, IDB_BITMAP_ANIM_PREV_DISABLE );
	m_mainBtnPrevAnim.ShowWindow( SW_SHOW );
	m_mainBtnPrevAnim.UpdateWindow();
	m_mainBtnPrevAnim.EnableWindow( FALSE );

	//Initialize main *Play Animation* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnPlayAnim.MoveWindow( rcRect );
	m_mainBtnPlayAnim.LoadBitmaps( IDB_BITMAP_ANIM_PLAY_UP, IDB_BITMAP_ANIM_PLAY_DOWN, IDB_BITMAP_ANIM_PLAY_FOCUS, IDB_BITMAP_ANIM_PLAY_DISABLE );
	m_mainBtnPlayAnim.ShowWindow( SW_HIDE );
	m_mainBtnPlayAnim.UpdateWindow();
	m_mainBtnPlayAnim.EnableWindow( FALSE );

	//Initialize main *Pause Animation* button here

	m_mainBtnPauseAnim.MoveWindow( rcRect );
	m_mainBtnPauseAnim.LoadBitmaps( IDB_BITMAP_ANIM_PAUSE_UP, IDB_BITMAP_ANIM_PAUSE_DOWN, IDB_BITMAP_ANIM_PAUSE_FOCUS, IDB_BITMAP_ANIM_PAUSE_DISABLE );
	m_mainBtnPauseAnim.ShowWindow( SW_SHOW );
	m_mainBtnPauseAnim.UpdateWindow();
	m_mainBtnPauseAnim.EnableWindow( FALSE );

	//Initialize main *Next Animation* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnNextAnim.MoveWindow( rcRect );
	m_mainBtnNextAnim.LoadBitmaps( IDB_BITMAP_ANIM_NEXT_UP, IDB_BITMAP_ANIM_NEXT_DOWN, IDB_BITMAP_ANIM_NEXT_FOCUS, IDB_BITMAP_ANIM_NEXT_DISABLE );
	m_mainBtnNextAnim.ShowWindow( SW_SHOW );
	m_mainBtnNextAnim.UpdateWindow();
	m_mainBtnNextAnim.EnableWindow( FALSE );

	//Initialize main *Timer* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnTimer.MoveWindow( rcRect );
	m_mainBtnTimer.LoadBitmaps( IDB_BITMAP_TIMER_UP, IDB_BITMAP_TIMER_DOWN, IDB_BITMAP_TIMER_FOCUS );
	m_mainBtnTimer.ShowWindow( SW_SHOW );
	m_mainBtnTimer.UpdateWindow();
	m_mainBtnTimer.EnableWindow( TRUE );

	//Initialize main *Material* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnMaterial.MoveWindow( rcRect );
	m_mainBtnMaterial.LoadBitmaps( IDB_BITMAP_MATERIAL_UP, IDB_BITMAP_MATERIAL_DOWN, IDB_BITMAP_MATERIAL_FOCUS, IDB_BITMAP_MATERIAL_DISABLE );
	m_mainBtnMaterial.ShowWindow( SW_SHOW );
	m_mainBtnMaterial.UpdateWindow();
	m_mainBtnMaterial.EnableWindow( CFalse );

	//Initialize main *Script* button here
	previousRight = rcRect.right;
	rcRect.left = previousRight + m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;

	m_mainBtnScriptManager.MoveWindow( rcRect );
	m_mainBtnScriptManager.LoadBitmaps( IDB_BITMAP_SCRIPT_UP, IDB_BITMAP_SCRIPT_DOWN, IDB_BITMAP_SCRIPT_FOCUS,  IDB_BITMAP_SCRIPT_DISABLE );
	m_mainBtnScriptManager.ShowWindow( SW_SHOW );
	m_mainBtnScriptManager.UpdateWindow();
	
	//start from end
	//Initialize main *Web* button here ( Last button )
	rcRect.right = CInt( 99 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.left = rcRect.right - fivePercent;

	m_mainBtnWeb.MoveWindow( rcRect );
	m_mainBtnWeb.LoadBitmaps( IDB_BITMAP_WEB_UP, IDB_BITMAP_WEB_DOWN, IDB_BITMAP_WEB_FOCUS );
	m_mainBtnWeb.ShowWindow( SW_SHOW );
	m_mainBtnWeb.UpdateWindow();
	m_mainBtnWeb.EnableWindow( TRUE );

	//Initialize main *Facebook* button here
	previousLeft = rcRect.left;
	rcRect.right = previousLeft - m_horizontalPointFivePercent;
	rcRect.left = rcRect.right - fivePercent;

	m_mainBtnFacebook.MoveWindow( rcRect );
	m_mainBtnFacebook.LoadBitmaps( IDB_BITMAP_FACEBOOK_UP, IDB_BITMAP_FACEBOOK_DOWN, IDB_BITMAP_FACEBOOK_FOCUS );
	m_mainBtnFacebook.ShowWindow( SW_SHOW );
	m_mainBtnFacebook.UpdateWindow();
	m_mainBtnFacebook.EnableWindow( TRUE );

	//Initialize main *Twitter* button here
	previousLeft = rcRect.left;
	rcRect.right = previousLeft - m_horizontalPointFivePercent;
	rcRect.left = rcRect.right - fivePercent;

	m_mainBtnTwitter.MoveWindow( rcRect );
	m_mainBtnTwitter.LoadBitmaps( IDB_BITMAP_TWITTER_UP, IDB_BITMAP_TWITTER_DOWN, IDB_BITMAP_TWITTER_FOCUS );
	m_mainBtnTwitter.ShowWindow( SW_SHOW );
	m_mainBtnTwitter.UpdateWindow();
	m_mainBtnTwitter.EnableWindow( TRUE );

	//Initialize main *YouTube* button here
	previousLeft = rcRect.left;
	rcRect.right = previousLeft - m_horizontalPointFivePercent;
	rcRect.left = rcRect.right - fivePercent;

	m_mainBtnYoutube.MoveWindow( rcRect );
	m_mainBtnYoutube.LoadBitmaps( IDB_BITMAP_YOUTUBE_UP, IDB_BITMAP_YOUTUBE_DOWN, IDB_BITMAP_YOUTUBE_FOCUS );
	m_mainBtnYoutube.ShowWindow( SW_SHOW );
	m_mainBtnYoutube.UpdateWindow();
	m_mainBtnYoutube.EnableWindow( TRUE );

	//Engine Objects' icons
	CFloat ButtonSizeAndGap = 6.5f;
	CFloat ButtonSize = 5.0f;
	//Initialize main *Sky* button here
	rcRect.left = 2 * m_horizontalPointFivePercent;
	rcRect.right = rcRect.left + fivePercent;
	rcRect.top = CInt( m_startLeftButtons * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnSky.MoveWindow( rcRect );
	m_mainBtnSky.LoadBitmaps( IDB_BITMAP_SKY_UP, IDB_BITMAP_SKY_DOWN, IDB_BITMAP_SKY_FOCUS, IDB_BITMAP_SKY_DISABLE );
	m_mainBtnSky.ShowWindow( SW_SHOW );
	m_mainBtnSky.UpdateWindow();
	m_mainBtnSky.EnableWindow( TRUE );

	//Initialize main *Water* button here
	previousRight = rcRect.right;
	rcRect.top = CInt( ( m_startLeftButtons + ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnWater.MoveWindow( rcRect );
	m_mainBtnWater.LoadBitmaps( IDB_BITMAP_WATER_UP, IDB_BITMAP_WATER_DOWN, IDB_BITMAP_WATER_FOCUS );
	m_mainBtnWater.ShowWindow( SW_SHOW );
	m_mainBtnWater.UpdateWindow();
	m_mainBtnWater.EnableWindow( TRUE );

	//Initialize main *Light* button here
	previousRight = rcRect.right;
	rcRect.top = CInt( ( m_startLeftButtons + 2 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnLight.MoveWindow( rcRect );
	m_mainBtnLight.LoadBitmaps( IDB_BITMAP_LIGHT_UP, IDB_BITMAP_LIGHT_DOWN, IDB_BITMAP_LIGHT_FOCUS );
	m_mainBtnLight.ShowWindow( SW_SHOW );
	m_mainBtnLight.UpdateWindow();
	m_mainBtnLight.EnableWindow( TRUE );

	//Initialize main *Static Sound* button here
	rcRect.top = CInt( ( m_startLeftButtons + 3 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnStaticSound.MoveWindow( rcRect );
	m_mainBtnStaticSound.LoadBitmaps( IDB_BITMAP_STATICSOUND_UP, IDB_BITMAP_STATICSOUND_DOWN, IDB_BITMAP_STATICSOUND_FOCUS );
	m_mainBtnStaticSound.ShowWindow( SW_SHOW );
	m_mainBtnStaticSound.UpdateWindow();
	m_mainBtnStaticSound.EnableWindow( TRUE );

	//Initialize *Ambient Sound* button here
	rcRect.top = CInt( ( m_startLeftButtons + 4 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnAmbientSound.MoveWindow( rcRect );
	m_mainBtnAmbientSound.LoadBitmaps( IDB_BITMAP_AMBIENTSOUND_UP, IDB_BITMAP_AMBIENTSOUND_DOWN, IDB_BITMAP_AMBIENTSOUND_FOCUS, IDB_BITMAP_AMBIENTSOUND_DISABLE );
	m_mainBtnAmbientSound.ShowWindow( SW_SHOW );
	m_mainBtnAmbientSound.UpdateWindow();
	m_mainBtnAmbientSound.EnableWindow( TRUE );

	//Initialize main *Player* button here
	rcRect.top = CInt( ( m_startLeftButtons + 5 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnPlayer.MoveWindow( rcRect );
	m_mainBtnPlayer.LoadBitmaps( IDB_BITMAP_PLAYER_UP, IDB_BITMAP_PLAYER_DOWN, IDB_BITMAP_PLAYER_FOCUS );
	m_mainBtnPlayer.ShowWindow( SW_SHOW );
	m_mainBtnPlayer.UpdateWindow();
	m_mainBtnPlayer.EnableWindow( TRUE );

	m_horizontalSizeOfLeftCulomn = 2 * m_horizontalPointFivePercent + 1 * fivePercent; //2 gaps+1 Buttons
	m_horizontalSizeOfRightCulomn = 5 * m_horizontalPointFivePercent + 5.5 * fivePercent; //5 gaps+3 Buttons
	CInt verticalOnePercent = ( windowRect.bottom - windowRect.top ) / 100;

	//Initialize main *Console* button here
	rcRect.top = CInt( ( m_startLeftButtons + 6 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnConsole.MoveWindow( rcRect );
	m_mainBtnConsole.LoadBitmaps( IDB_BITMAP_CONSOLE_UP, IDB_BITMAP_CONSOLE_DOWN, IDB_BITMAP_CONSOLE_FOCUS );
	m_mainBtnConsole.ShowWindow( SW_SHOW );
	m_mainBtnConsole.UpdateWindow();
	m_mainBtnConsole.EnableWindow( TRUE );

	//Initialize main *Summary* button here
	rcRect.top = CInt( ( m_startLeftButtons + 7 * ButtonSizeAndGap ) * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = rcRect.top + (ButtonSize * ( windowRect.bottom - windowRect.top ) / 100 );

	m_mainBtnSummary.MoveWindow( rcRect );
	m_mainBtnSummary.LoadBitmaps( IDB_BITMAP_SUMMARY_UP, IDB_BITMAP_SUMMARY_DOWN, IDB_BITMAP_SUMMARY_FOCUS );
	m_mainBtnSummary.ShowWindow( SW_SHOW );
	m_mainBtnSummary.UpdateWindow();
	m_mainBtnSummary.EnableWindow( TRUE );

	//Vanda Engine Text (Version)
	rcRect.top = CInt( 92 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = CInt( 96 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.left =  CInt( 0.5 * ( windowRect.right - windowRect.left ) / 100 );
	rcRect.right = CInt( 1.3 * ( windowRect.right - windowRect.left ) / 100 );

	m_btnVandaVersion.MoveWindow( rcRect );
	m_btnVandaVersion.LoadBitmaps( IDB_BITMAP_VANDA_VERSION, IDB_BITMAP_VANDA_VERSION, IDB_BITMAP_VANDA_VERSION, IDB_BITMAP_VANDA_VERSION );
	m_btnVandaVersion.ShowWindow( SW_SHOW );
	m_btnVandaVersion.UpdateWindow();
	m_btnVandaVersion.EnableWindow( FALSE );

	//Vanda Engine Text
	rcRect.top = CInt( 86 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = CInt( 96 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.left =  CInt( 2.5 * ( windowRect.right - windowRect.left ) / 100 );
	rcRect.right = CInt( 3.5 * ( windowRect.right - windowRect.left ) / 100 );

	m_btnVandaText.MoveWindow( rcRect );
	m_btnVandaText.LoadBitmaps( IDB_BITMAP_VANDA_TEXT, IDB_BITMAP_VANDA_TEXT, IDB_BITMAP_VANDA_TEXT, IDB_BITMAP_VANDA_TEXT );
	m_btnVandaText.ShowWindow( SW_SHOW );
	m_btnVandaText.UpdateWindow();
	m_btnVandaText.EnableWindow( FALSE );

	//==============================================
	CFont fnt2;
	rcRect.top = long( 1 * ( windowRect.top - windowRect.bottom ) / 100);
	rcRect.bottom = long( 3 * ( windowRect.top - windowRect.bottom ) / 100);
	//ScreenToClient( &rcRect );
	CInt fontSizeY = rcRect.bottom - rcRect.top; 
	fnt2.CreateFont(-fontSizeY, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Consolas");
	CFont fnt3;
	rcRect.top = long( 1 * ( windowRect.top - windowRect.bottom ) / 100);
	rcRect.bottom = long( 3.5 * ( windowRect.top - windowRect.bottom ) / 100);
	//ScreenToClient( &rcRect );
	CInt fontSizeY3 = rcRect.bottom - rcRect.top; 
	fnt3.CreateFont(-fontSizeY3, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Consolas");

	//==============================================
	//static text: List of collada scenes
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalPointFivePercent;

	rcRect.top = previousColumn1 + verticalOnePercent;
	rcRect.bottom = rcRect.top + 2.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	GetDlgItem( IDC_STATIC_SCENES )->MoveWindow( rcRect );
	GetDlgItem( IDC_STATIC_SCENES )->ShowWindow( SW_SHOW );
	GetDlgItem( IDC_STATIC_SCENES )->SetFont( &fnt2 );
	GetDlgItem( IDC_STATIC_SCENES )->UpdateWindow();

	//list control: COLLADA scenes
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + 2 * m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - 2 * m_horizontalPointFivePercent;
	rcRect.top = previousColumn1 + 5 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 15 * verticalOnePercent;

	//ScreenToClient( &rcRect );

	m_listBoxScenes.MoveWindow( rcRect );
	m_listBoxScenes.SetBkColor( RGB( 40, 40, 40 ));
	m_listBoxScenes.SetTextBkColor( RGB( 40, 40, 40 ) );
	m_listBoxScenes.SetTextColor(COLOR_GREEN);
	
	RECT tempRect;
	m_listBoxScenes.GetClientRect( &tempRect );

	m_listBoxScenes.InsertColumn(0, "Scenes", LVCFMT_LEFT |  LVS_SHOWSELALWAYS, ( tempRect.right - tempRect.left ) * 80 / 100 );
	m_listBoxScenes.SetFont( &fnt3 );
	m_listBoxScenes.ShowWindow( SW_SHOW );
	m_listBoxScenes.UpdateWindow();


	// Remove "Button" of COLLADA Scenes
	rcRect.left = ( windowRect.right - windowRect.left ) - ( 3 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = rcRect.left + ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn1 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn1 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnRemoveScene.LoadBitmaps( IDB_BITMAP_DELETE_UP, IDB_BITMAP_DELETE_DOWN, IDB_BITMAP_DELETE_FOCUS, IDB_BITMAP_DELETE_DISABLE );
	m_btnRemoveScene.MoveWindow( rcRect );
	m_btnRemoveScene.EnableWindow( FALSE );
	m_btnRemoveScene.ShowWindow( SW_SHOW );
	m_btnRemoveScene.SetFont( &fnt3 );
	m_btnRemoveScene.UpdateWindow();

	previousLeft = rcRect.left;
	// Properties "Button" of scenes
	rcRect.right = previousLeft - m_horizontalPointFivePercent;
	rcRect.left = rcRect.right - ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn1 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn1 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnSceneProperties.LoadBitmaps( IDB_BITMAP_EDIT_UP, IDB_BITMAP_EDIT_DOWN, IDB_BITMAP_EDIT_FOCUS, IDB_BITMAP_EDIT_DISABLE );
	m_btnSceneProperties.MoveWindow( rcRect );
	m_btnSceneProperties.SetFont( &fnt3 );
	m_btnSceneProperties.EnableWindow( FALSE );
	m_btnSceneProperties.ShowWindow( SW_SHOW );
	m_btnSceneProperties.UpdateWindow();

	//==============================================

	//static text: list of objects
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalPointFivePercent;
	rcRect.top = previousColumn2 + verticalOnePercent;
	rcRect.bottom = rcRect.top + 2.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	GetDlgItem( IDC_STATIC_OBJECTS )->MoveWindow( rcRect );
	GetDlgItem( IDC_STATIC_OBJECTS )->SetFont( &fnt2 );
	GetDlgItem( IDC_STATIC_OBJECTS )->ShowWindow( SW_SHOW );
	GetDlgItem( IDC_STATIC_OBJECTS )->UpdateWindow();

	//list control: COLLADA objects
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + 2 * m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - 2 * m_horizontalPointFivePercent;;
	rcRect.top = previousColumn2 + 5 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 15 * verticalOnePercent;

	//ScreenToClient( &rcRect );

	m_listBoxObjects.MoveWindow( rcRect );
	m_listBoxObjects.SetBkColor( RGB( 40, 40, 40 ));
	m_listBoxObjects.SetTextBkColor( RGB( 40, 40, 40 ));
	m_listBoxObjects.SetFont( &fnt3 );
	m_listBoxObjects.SetTextColor(COLOR_GREEN);
	m_objectListImage.Create(32,32,ILC_COLOR24,9,5);
	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_CAMERA);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_IMAGE);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_MATERIAL);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_MESH);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_ANIMATION);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_LIGHT);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_EFFECT);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_ANIMATIONCLIP);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_OBJECTLIST_SKINCONTROLLER);
	m_objectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();

	m_listBoxObjects.SetImageList( &m_objectListImage, LVSIL_SMALL );

	m_listBoxObjects.GetClientRect( &tempRect );

	m_listBoxObjects.InsertColumn(0, "Objects", LVCFMT_LEFT | LVS_REPORT | LVS_NOLABELWRAP, ( tempRect.right - tempRect.left ) * 80 / 100);
	m_listBoxObjects.ShowWindow( SW_SHOW );
	m_listBoxObjects.UpdateWindow();

	// Remove "Button" of COLLADA objects
	rcRect.left = ( windowRect.right - windowRect.left ) - ( 3 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = rcRect.left + ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn2 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn2 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnRemoveObject.LoadBitmaps( IDB_BITMAP_DELETE_UP, IDB_BITMAP_DELETE_DOWN, IDB_BITMAP_DELETE_FOCUS, IDB_BITMAP_DELETE_DISABLE );
	m_btnRemoveObject.MoveWindow( rcRect );
	m_btnRemoveObject.SetFont( &fnt3 );
	m_btnRemoveObject.ShowWindow( SW_HIDE );
	m_btnRemoveObject.UpdateWindow();
	//==============================================

	//==============================================
	//static text: list of Engine objects
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalPointFivePercent;
	rcRect.top = previousColumn3 + verticalOnePercent;
	rcRect.bottom = rcRect.top + 2.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	GetDlgItem( IDC_STATIC_ENGINE_OBJECTS )->MoveWindow( rcRect );
	GetDlgItem( IDC_STATIC_ENGINE_OBJECTS )->SetFont( &fnt2 );
	GetDlgItem( IDC_STATIC_ENGINE_OBJECTS )->ShowWindow( SW_SHOW );
	GetDlgItem( IDC_STATIC_ENGINE_OBJECTS )->UpdateWindow();

	//list control: Engine objetcs
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + 2 * m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - 2 * m_horizontalPointFivePercent;
	rcRect.top = previousColumn3 + 5 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 15 * verticalOnePercent;
	//ScreenToClient( &rcRect );

	m_listBoxEngineObjects.MoveWindow( rcRect );
	m_listBoxEngineObjects.SetBkColor( RGB( 40, 40, 40 ));
	m_listBoxEngineObjects.SetTextBkColor( RGB( 40, 40, 40 ));
	m_listBoxEngineObjects.SetTextColor(COLOR_GREEN);

	m_engineObjectListImage.Create(32,32,ILC_COLOR24,5,5);
	cBmp.LoadBitmap(IDB_BITMAP_ENGINEOBJECTLIST_WATER);
	m_engineObjectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_ENGINEOBJECTLIST_STATICSOUND);
	m_engineObjectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_ENGINEOBJECTLIST_AMBIENTSOUND);
	m_engineObjectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_ENGINEOBJECTLIST_SKY);
	m_engineObjectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();
	cBmp.LoadBitmap(IDB_BITMAP_ENGINEOBJECTLIST_LIGHT);
	m_engineObjectListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();

	m_listBoxEngineObjects.SetImageList( &m_engineObjectListImage, LVSIL_SMALL );

	//rcRect.right = CInt( 12.5 * ( windowRect.right - windowRect.left ) / 100);
	m_listBoxEngineObjects.GetClientRect( &tempRect );
	rcRect.left = tempRect.left;
	rcRect.right = tempRect.right;

	m_listBoxEngineObjects.InsertColumn(0, "Objects", LVCFMT_LEFT |  LVS_SHOWSELALWAYS, ( tempRect.right - tempRect.left ) * 80 / 100 );
	m_listBoxEngineObjects.ShowWindow( SW_SHOW );
	m_listBoxEngineObjects.SetFont( &fnt3 );
	m_listBoxEngineObjects.UpdateWindow();


	// Remove "Button" of engine objects
	rcRect.left = ( windowRect.right - windowRect.left ) - ( 3 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = rcRect.left + ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn3 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn3 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnRemoveEngineObject.LoadBitmaps( IDB_BITMAP_DELETE_UP, IDB_BITMAP_DELETE_DOWN, IDB_BITMAP_DELETE_FOCUS, IDB_BITMAP_DELETE_DISABLE );
	m_btnRemoveEngineObject.MoveWindow( rcRect );
	m_btnRemoveEngineObject.SetFont( &fnt3 );
	m_btnRemoveEngineObject.EnableWindow( FALSE );
	m_btnRemoveEngineObject.ShowWindow( SW_SHOW );
	m_btnRemoveEngineObject.UpdateWindow();

	previousLeft = rcRect.left;
	// Properties "Button" of engine objects
	rcRect.right = previousLeft - m_horizontalPointFivePercent;
	rcRect.left = rcRect.right - ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn3 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn3 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnEngineObjectProperties.LoadBitmaps( IDB_BITMAP_EDIT_UP, IDB_BITMAP_EDIT_DOWN, IDB_BITMAP_EDIT_FOCUS, IDB_BITMAP_EDIT_DISABLE );
	m_btnEngineObjectProperties.MoveWindow( rcRect );
	m_btnEngineObjectProperties.SetFont( &fnt3 );
	m_btnEngineObjectProperties.EnableWindow( FALSE );
	m_btnEngineObjectProperties.ShowWindow( SW_SHOW );
	m_btnEngineObjectProperties.UpdateWindow();
	
	//static text: list of PhysX Elements///////////////////
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalPointFivePercent;
	rcRect.top = previousColumn4 + verticalOnePercent;
	rcRect.bottom = rcRect.top + 2.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	GetDlgItem( IDC_STATIC_PHYSX_ELEMENTS )->MoveWindow( rcRect );
	GetDlgItem( IDC_STATIC_PHYSX_ELEMENTS )->SetFont( &fnt2 );
	GetDlgItem( IDC_STATIC_PHYSX_ELEMENTS )->ShowWindow( SW_SHOW );
	GetDlgItem( IDC_STATIC_PHYSX_ELEMENTS )->UpdateWindow();

	//list control: PhysX Elements
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + 2 * m_horizontalPointFivePercent;
	rcRect.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - 2 * m_horizontalPointFivePercent;
	rcRect.top = previousColumn4 + 5 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 15 * verticalOnePercent;
	//ScreenToClient( &rcRect );

	m_listBoxPhysXElements.MoveWindow( rcRect );
	m_listBoxPhysXElements.SetBkColor( RGB( 40, 40, 40 ));
	m_listBoxPhysXElements.SetTextBkColor( RGB( 40, 40, 40 ));
	m_listBoxPhysXElements.SetTextColor(COLOR_GREEN);

	m_listBoxPhysXElements.GetClientRect( &tempRect );
	rcRect.left = tempRect.left;
	rcRect.right = tempRect.right;

	m_physXElementListImage.Create(32,32,ILC_COLOR24,3,5);

	cBmp.LoadBitmap(IDB_BITMAP_PHYSXELEMENTLIST_DYNAMICRIGIDBODY);
	m_physXElementListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();

	cBmp.LoadBitmap(IDB_BITMAP_PHYSXELEMENTLIST_STATICRIGIDBODY);
	m_physXElementListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();

	cBmp.LoadBitmap(IDB_BITMAP_PHYSXELEMENTLIST_TRIGGER);
	m_physXElementListImage.Add(&cBmp, RGB(255,255, 255));
	cBmp.DeleteObject();

	m_listBoxPhysXElements.SetImageList( &m_physXElementListImage, LVSIL_SMALL );

	m_listBoxPhysXElements.InsertColumn(0, "Elements", LVCFMT_LEFT |  LVS_SHOWSELALWAYS, ( tempRect.right - tempRect.left ) * 80 / 100 );
	m_listBoxPhysXElements.ShowWindow( SW_SHOW );
	m_listBoxPhysXElements.SetFont( &fnt3 );
	m_listBoxPhysXElements.UpdateWindow();

	// Remove "Button" of PhysX elements
	rcRect.left = ( windowRect.right - windowRect.left ) - ( 3 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = rcRect.left + ( (3.5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	rcRect.top = previousColumn4 + 20 * verticalOnePercent;
	rcRect.bottom = previousColumn4 + 23.5 * verticalOnePercent;

	//ScreenToClient( &rcRect );
	m_btnRemovePhysX.LoadBitmaps( IDB_BITMAP_DELETE_UP, IDB_BITMAP_DELETE_DOWN, IDB_BITMAP_DELETE_FOCUS, IDB_BITMAP_DELETE_DISABLE );
	m_btnRemovePhysX.MoveWindow( rcRect );
	m_btnRemovePhysX.SetFont( &fnt3 );
	m_btnRemovePhysX.EnableWindow( FALSE );
	m_btnRemovePhysX.ShowWindow( SW_SHOW );
	m_btnRemovePhysX.UpdateWindow();

	////////////////////////
	fnt3.Detach();

	//Initialize rich edits///////////////
	//CRect rcRichRect;
	CFont richFnt;
	//ScreenToClient( &rcRect );
	rcRect.top = long( 1 * ( windowRect.top - windowRect.bottom ) / 100);
	rcRect.bottom = long( 3.5 * ( windowRect.top - windowRect.bottom ) / 100);
	//ScreenToClient( &rcRect );
	CInt fontSizeY2 = rcRect.bottom - rcRect.top; 
	richFnt.CreateFont(-fontSizeY2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Consolas");

	//Initialze the rich edit here
	rcRect.left =  CInt( 1.0 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.top = CInt( 97.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.right =	CInt( 40 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.bottom = CInt( 99.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	//ScreenToClient( &rcRect );

	m_rich.MoveWindow( rcRect );
	m_rich.SetFont( &fnt2/*richFnt*/ );
	m_rich.SetBackgroundColor( FALSE, RGB(40,40,40));
	m_rich.ShowWindow( SW_SHOW );
	m_rich.UpdateWindow();

	rcRect.left = CInt( 40.5 * ( windowRect.right - windowRect.left ) / 100 );
	rcRect.right = CInt( 75 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.top = CInt( 97.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = CInt( 99.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	//ScreenToClient( &rcRect );

	m_rich2.MoveWindow( rcRect );
	m_rich2.SetFont( &fnt2/*richFnt*/ );
	m_rich2.SetBackgroundColor( FALSE, RGB(40,40,40));
	m_rich2.ShowWindow( SW_SHOW );
	m_rich2.UpdateWindow();

	rcRect.left = CInt( 75.5 * ( windowRect.right - windowRect.left ) / 100 );
	rcRect.right = CInt( 90 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.top = CInt( 97.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = CInt( 99.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	//ScreenToClient( &rcRect );

	m_rich3.MoveWindow( rcRect );
	m_rich3.SetFont( &fnt2/*richFnt*/ );
	m_rich3.SetBackgroundColor( FALSE, RGB(40,40,40));
	m_rich3.ShowWindow( SW_SHOW );
	m_rich3.UpdateWindow();

	//selected object
	rcRect.left = CInt( 90.5 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.right = CInt( 99.5 * ( windowRect.right - windowRect.left ) / 100);
	rcRect.top = CInt( 97.5 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcRect.bottom = CInt( 99.5 * ( windowRect.bottom - windowRect.top ) / 100 );

	m_staticSelectedObject.MoveWindow( rcRect );
	m_staticSelectedObject.SetFont( &fnt2/*richFnt*/ );
	m_staticSelectedObject.SetBackgroundColor( FALSE, RGB(40,40,40) );
	m_staticSelectedObject.ShowWindow( SW_SHOW );
	m_staticSelectedObject.UpdateWindow();

	richFnt.Detach();
	//===========================================
	fnt2.Detach();

	//intitialize OpenGL window

	CRect rcClient;
	GetClientRect( &rcClient );

	rcClient.left = m_horizontalSizeOfLeftCulomn + m_horizontalPointFivePercent;
	rcClient.top = CInt( 6 * ( windowRect.bottom - windowRect.top ) / 100 );
	rcClient.right = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn;
	rcClient.bottom = CInt(96.5 * ( windowRect.bottom - windowRect.top ) / 100 );

	//Create the OpenGL window here
	g_width = rcClient.right - rcClient.left;
	g_height = rcClient.bottom - rcClient.top;

	//configuration
	FILE *filePtr;
	CChar ConfigPath[MAX_NAME_SIZE];
	HRESULT doc_result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, ConfigPath);
	if (doc_result != S_OK)
	{
        PrintInfo( "\nCouldn't get the documents folder to write data", COLOR_RED );
	}
	else
	{
		Append( ConfigPath, "\\VandaRTI\\conf_editor.dat" );
	}

	filePtr = fopen( ConfigPath, "rb" );
	if( !filePtr )
	{
		CChar RTIPath[MAX_NAME_SIZE];
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, RTIPath);
		Append( RTIPath, "\\VandaRTI\\" );
		CreateWindowsDirectory( RTIPath );

		filePtr =  fopen( ConfigPath, "wb" );
		fwrite( &g_options, sizeof( COptions ), 1, filePtr  );
	}
	else
	{
		PrintInfo( "Engine options:" );
		fread( &g_options , sizeof( COptions ), 1, filePtr  );
		if( g_options.m_enableShader )
			PrintInfo( "\nShader: ON" );
		else
			PrintInfo( "\nShader: OFF", COLOR_RED );

		if( g_options.m_numSamples )
			PrintInfo( "\nMultisampling: ON" );
		else
			PrintInfo( "\nMultisampling: OFF", COLOR_RED );

		if( g_options.m_anisotropy )
			PrintInfo( "\nAnisotropic Texture Filtering: ON" );
		else
			PrintInfo( "\nAnisotropic Texture Filtering: OFF", COLOR_RED );

		if( g_options.m_disableVSync )
			PrintInfo( "\nVSync: OFF" );
		else
			PrintInfo( "\nVSync: ON" );

		if( g_options.m_enableFBO )
			PrintInfo( "\nFBO: ON" );
		else
			PrintInfo( "\nFBO: OFF", COLOR_RED );

		if( g_options.m_enableVBO )
			PrintInfo( "\nVBO: ON" );
		else
			PrintInfo( "\nVBO: OFF", COLOR_RED );

		if( g_options.m_enableWaterReflection )
			PrintInfo( "\nWater Reflection: ON" );
		else
			PrintInfo( "\nWater Reflection: OFF", COLOR_RED );


	}
	fclose(filePtr);
	////////////////

	//Create the OpenGLw window here
	g_multipleView = CNew( CMultipleWindows );
	if (!g_multipleView->Create(NULL, NULL, WS_VISIBLE | WS_CHILD , rcClient, this, 0))
	{
		PostQuitMessage(0);
		return FALSE;
	}

	g_glUtil.InitGLEW();
	//if (!glewIsSupported("GL_VERSION_3_0"))
	//{
		//MessageBox( "Your implementation does not support OpenGL 3.3\nVanda may crash or you may not be able to use all the features of Vanda\nUpdating your graphics driver may fix this problem" );
	//}
	//ilInit();
	//iluInit();
	if(GLEW_ARB_color_buffer_float)
	{
		glClampColorARB( GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE );
		glClampColorARB( GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE );
	}

	if( !GLEW_EXT_framebuffer_object || !GLEW_ARB_texture_non_power_of_two || !g_options.m_enableFBO)
	{
		PrintInfo( "\nSwitching to old rendering style", COLOR_RED );
		g_useOldRenderingStyle = CTrue;
	}
	if( !g_useOldRenderingStyle )
	{
		if( !GLEW_EXT_framebuffer_multisample )
		{
			PrintInfo("\nGL_EXT_framebuffer_multisample is not supported\nSwitching to old rendering style");
			g_useOldRenderingStyle = CTrue; //See if we can we use multisampling with old rendering style?
		}
		else
			g_multipleView->m_multiSample = true; //use new rendering style with multisampling
	}
	if( g_useOldRenderingStyle && !WGLEW_ARB_multisample  )
			PrintInfo("\nYour implementation doesn't support multisampling");

	if( WGLEW_ARB_multisample && g_useOldRenderingStyle )
	{
		g_multipleView->DestroyWindow();
		g_multipleView->m_multiSample = true;
		if (!g_multipleView->Create(NULL, NULL, WS_BORDER | WS_VISIBLE | WS_CHILD , rcClient, this, 0))
		{
			PrintInfo( "\nWarning! Couldn't create an OpenGL window with multisampling", COLOR_YELLOW );
			g_multipleView->DestroyWindow();
			g_multipleView->m_multiSample = false;
			if (!g_multipleView->Create(NULL, NULL, WS_BORDER | WS_VISIBLE | WS_CHILD  , rcClient, this, 0))
			{
				PrintInfo( "\nWarning! Couldn't create an OpenGL window after multisampling faild", COLOR_YELLOW );

				PostQuitMessage(0);
				return FALSE;
			}
		}
	}

	//PrintInfo GL info here
	g_glUtil.GetGLInfo();
	if( WGLEW_EXT_swap_control )
			wglSwapIntervalEXT(0);

	g_render.Init();

	if( !g_multipleView->InitAll() )
	{
		MessageBox( "fatal error(s) occured. Please fix the error(s)", "Vanda Engine 1 Error", MB_OK );
	}

	if( g_multipleView->m_multiSample && GLEW_NV_multisample_filter_hint)
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST);

	if( WGLEW_EXT_swap_control && g_options.m_disableVSync )
	{
		wglSwapIntervalEXT(0);
	}
	else
		wglSwapIntervalEXT(1);

	GetMenu()->EnableMenuItem( ID_MODIFY_BLOOM, MF_DISABLED | MF_GRAYED );
	g_render.m_useBloom = CFalse;

	if( GLEW_ARB_texture_non_power_of_two && g_render.UsingFBOs() && g_options.m_enableFBO && g_render.UsingShader() )
	{
		GetMenu()->EnableMenuItem( ID_MODIFY_BLOOM, MF_ENABLED );
		g_bloom = CNew( CBloom );
		g_bloom->CreateEmptyTexture( g_width, g_height, 3, GL_RGB );
		g_render.m_useBloom = CTrue;
	}
	if( g_useOldRenderingStyle )
	{
		GetMenu()->EnableMenuItem( ID_MODIFY_DOF, MF_GRAYED | MF_DISABLED );
		GetMenu()->EnableMenuItem( ID_MODIFY_BLOOM, MF_GRAYED | MF_DISABLED );
	}

	if( !g_useOldRenderingStyle )
		g_multipleView->InitFBOs( GL_RGBA, GL_RGBA );

	if( g_render.UsingShader() )
		g_render.m_useWaterReflection = CTrue;
	else
		g_render.m_useWaterReflection = CFalse;

	GetMenu()->CheckMenuItem( ID_TOOLS_CULLFACES, MF_UNCHECKED );
	GetMenu()->EnableMenuItem( ID_TOOLS_CULLFACES, MF_GRAYED );

	GetMenu()->CheckMenuItem( ID_TOOLS_GEOMETRYBASEDSELECTION, MF_CHECKED );

	GetMenu()->CheckMenuItem( ID_VIEW_LIGHT_POSITIONS, MF_CHECKED );
	GetMenu()->CheckMenuItem( ID_VIEW_SOUND_POSITIONS, MF_CHECKED );
	GetMenu()->CheckMenuItem( ID_ICONS_WATERICONS, MF_CHECKED );
	GetMenu()->CheckMenuItem( ID_ICONS_SKYICON, MF_CHECKED );
	GetMenu()->CheckMenuItem( ID_VIEW_PERSPECTIVE_GRIDS, MF_CHECKED );
	GetMenu()->CheckMenuItem( ID_VIEW_SHADOWDEPTHMAP, MF_UNCHECKED );
	GetMenu()->CheckMenuItem( ID_VIEW_STATISTICS, MF_UNCHECKED );
	GetMenu()->CheckMenuItem( ID_ICONS_CAMERAICONS, MF_CHECKED );

	GetMenu()->CheckMenuItem( ID_VIEW_BOUNDINGBOX, MF_UNCHECKED );
	GetMenu()->CheckMenuItem( ID_VIEW_OCTREE, MF_UNCHECKED );

	GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_CHECKED );

	m_askRemoveEngineObject = CTrue;

	//save functions///////////////////////////////////////
	//Get Document Directory
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, g_VScenePath);
	if (result != S_OK)
	{
        PrintInfo( "\nCouldn't get the documents folder to write data", COLOR_RED );
	}
	else
	{
		Append( g_VScenePath, "\\VandaRTI\\VScenes\\" );
	}

	Cpy( g_currentVSceneName, "\n" ); 
	FILE *VScenesFilePtr;
	CChar DATPath[MAX_NAME_SIZE];
	sprintf( DATPath, "%s%s", g_VScenePath, "vscenes.dat" );
	VScenesFilePtr = fopen( DATPath, "rb" );
	CInt numScenes = 0;
	if(!VScenesFilePtr )
	{
		PrintInfo( "\n'vscenes.dat' created successfully" );
		CreateWindowsDirectory( g_VScenePath );

		VScenesFilePtr =  fopen( DATPath, "wb" );
		fwrite( &numScenes, sizeof( CInt ), 1, VScenesFilePtr  );
		fclose( VScenesFilePtr );
		VScenesFilePtr =  fopen( DATPath, "rb" );
	}
	fread( &numScenes, sizeof( CInt ), 1, VScenesFilePtr  );

	
	for( CInt i = 0; i < numScenes; i++ )
	{
		CChar vsceneName[MAX_NAME_SIZE];
		fread( vsceneName, sizeof( CChar ), MAX_NAME_SIZE, VScenesFilePtr );
		g_allVSceneNames.push_back( vsceneName );

	}
	fclose( VScenesFilePtr );
	//save functions///////////////////////////////////
	if(!g_vandaDemo)
	{
		g_shadowProperties.m_enable = CTrue;
	}
	g_sceneBanner.SetBannerPath("Assets/Engine/Textures/Loading.dds");
    srand(time(NULL));
	//initialize lua/////////////////
	LuaOpenLibs(g_lua);
	lua_register(g_lua, "PlaySound", PlaySound);
	lua_register(g_lua, "PauseSound", PauseSound);
	lua_register(g_lua, "StopSound", StopSound);
	lua_register(g_lua, "BlendCycle", BlendCycle);
	lua_register(g_lua, "ClearCycle", ClearCycle);
	lua_register(g_lua, "ExecuteAction", ExecuteAction);
	lua_register(g_lua, "ReverseExecuteAction", ReverseExecuteAction);
	lua_register(g_lua, "LoadScene", LoadScene);
	lua_register(g_lua, "ActivateCamera", ActivateCamera);

	////////////////////////////////

	//Remove desktop.ini read only flag. (used for Uninstall)
	//SetFileAttributes("C:/Users/Public/Desktop/desktop.ini", GetFileAttributes( "C:/Users/Public/Desktop/desktop.ini" ) & ~FILE_ATTRIBUTE_READONLY );
	PrintInfo( "\nRTI Pro Version 1.3.0 initialized successfully" );
	//CAboutDlg dlgAbout;
	//dlgAbout.DoModal();
	PrintInfo2( "Errors and warnings related to the COLLADA files" );
	ReleaseCapture();
	if( g_options.m_showStartupDialog )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgWelcome.DoModal();

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
	}
	if( g_importCOLLADA )
	{
		OnMenuClickedImportCollada();
	}
	else if( g_openVINFile )
	{
		OnMenuClickedOpen(CFalse);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

CVoid CVandaEngine1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

CVoid CVandaEngine1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		CInt cxIcon = GetSystemMetrics(SM_CXICON);
		CInt cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		CInt x = (rect.Width() - cxIcon + 1) / 2;
		CInt y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVandaEngine1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//CVoid CVandaEngine1Dlg::OnClose()
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CDialog::OnClose();
//}



BOOL CVandaEngine1Dlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (wParam == ID_FILE_NEW)
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedNew( CTrue ); //ask to see if we should proceed?
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if (wParam == ID_FILE_OPEN )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedOpen();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if (wParam == ID_FILE_SAVE )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSaveAs(CFalse);
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if (wParam == ID_FILE_SAVEAS )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSaveAs();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if( wParam == ID_IMPORT_COLLADA )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedImportCollada();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_IMPORT_PHYSX )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedImportPhysX();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_FILE_PUBLISHSOLUTION )
	{
		OnBnClickedBtnPublishSolution();
	}
	else if( wParam == ID_FILE_EXIT )
	{
		//configuration
		CChar ConfigPath[MAX_NAME_SIZE];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, ConfigPath);
		if (result != S_OK)
		{
			PrintInfo( "\nCouldn't get the documents folder to write data", COLOR_RED );
		}
		else
		{
			Append( ConfigPath, "\\VandaRTI\\conf_editor.dat" );
		}

		FILE *filePtr;
		filePtr = fopen( ConfigPath, "wb" );
		fwrite( &g_options, sizeof( COptions ), 1, filePtr  );
		fclose(filePtr);
		////////////////

		if( g_scene.size() > 0 || g_engineLights.size() > 0 || g_engineWaters.size() > 0 || g_menu.m_insertAndShowSky || g_menu.m_insertAmbientSound || g_engineStaticSounds.size() > 0 )
		{
			CInt iResponse;
			iResponse = MessageBox( "Save scene?", "Warning" , MB_YESNOCANCEL |MB_ICONSTOP);
			if( iResponse == IDYES )
			{
				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CFalse );
				OnMenuClickedSaveAs();
				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CTrue );

				PostQuitMessage( 0 );
			}
			else if( iResponse == IDNO )
				PostQuitMessage( 0 );
		}
		else
			PostQuitMessage( 0 );
	}
	else if( wParam == ID_INSERT_LIGHT )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedInsertLight();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_INSERT_WATER )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedInsertWater();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_INSERT_SOUND_AMBIENT )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedInsertAmbientSound();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if( wParam == ID_INSERT_SOUND_STATIC )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedInsertStaticSound();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}

	else if( wParam == ID_INSERT_SKYDOME )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedInsertSkyDome();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_INSERT_PLAYER )
	{
		g_characterPos = g_multipleView->m_nx->gCharacterPos;
		g_cameraPitchYawTilt.x = g_camera->m_perspectiveCameraPitch;
		g_cameraPitchYawTilt.y = g_camera->m_perspectiveCameraYaw;
		g_cameraPitchYawTilt.z = g_camera->m_perspectiveCameraTilt;
		PrintInfo( "\nPlayer set to the current position" );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_LAYOUTS_4SPLIT )
	{
		g_menu.m_justPerspective = CFalse;
		PrintInfo3( "\nSwitched to 4 viewports" );
		GetMenu()->CheckMenuItem( ID_LAYOUTS_PERSPECTIVE, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_LAYOUTS_4SPLIT, MF_CHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_LAYOUTS_PERSPECTIVE )
	{
		g_menu.m_justPerspective = CTrue;
		PrintInfo3( "\nSwitched to perspective viewport" );
		GetMenu()->CheckMenuItem( ID_LAYOUTS_PERSPECTIVE, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_LAYOUTS_4SPLIT, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if (wParam == ID_MODIFY_SHADOW )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditShadow = CNew( CEditShadow );
		m_dlgEditShadow->DoModal();
		CDelete( m_dlgEditShadow );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if (wParam == ID_MODIFY_DOF )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditDOF = CNew( CEditDOF );
		m_dlgEditDOF->DoModal();
		CDelete( m_dlgEditDOF );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if (wParam == ID_MODIFY_FOG )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditFog = CNew( CEditFog );
		m_dlgEditFog->DoModal();
		CDelete( m_dlgEditFog );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_TOOLS_CAMERASPEED_INCREASE )
	{
		g_multipleView->m_nx->gCharacterSpeed += 1.0f;
	}
	else if( wParam == ID_TOOLS_CAMERASPEED_DECREASE )
	{
		g_multipleView->m_nx->gCharacterSpeed -= 1.0f;
		if( g_multipleView->m_nx->gCharacterSpeed < 0.0f )
		{
			g_multipleView->m_nx->gCharacterSpeed += 1.0f;
			PrintInfo( "\nError: Couldn't decrease the camera speed", COLOR_RED );
		}
	}
	else if( wParam == ID_TOOLS_CAMERASPEED_DEFAULT )
	{
		g_multipleView->m_nx->gCharacterSpeed = (CFloat)DEFAULT_CHARACTER_SPEED;
		PrintInfo3( "\nCamera speed set to default" );
	}
	else if( wParam == ID_TOOLS_CAMERAZOOM_DEFAULT )
	{
		g_camera->m_cameraManager->SetAngle( DEFAULT_CAMERA_ANGLE );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}


	else if( wParam == ID_TOOLS_CULLFACES )
	{
		OnBnClickedCullFace();
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_GEOMETRY_AMBIENTCOLOR )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		OnMenuClickedGeneralAmbientColor();

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}

	else if( wParam == ID_TOOLS_WATERATTACHMENT )
	{
		if( g_selectedName == -1 )
		{
			MessageBox( "No object has been selected!", "VandaEngine Error", MB_OK | MB_ICONERROR);
		}
		else if( !g_menu.m_geometryBasedSelection )
		{
			MessageBox( "Water attachment does not work with material based selection.\nPlease switch to geometry based selection and try again!", "VandaEngine Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CFalse );
			OnMenuClickedWaterAttachment();
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CTrue );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();
		}
	}
	else if( wParam == ID_TOOLS_SCRIPTMANAGER )
	{
		OnBnClickedBtnScriptManager();
	}
	else if( wParam == ID_TOOLS_SELECTCAMERA )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSelectCamera();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_TOOLS_OPTIONS )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedGeneralOptions();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_TOOLS_SCENEOPTIONS )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSceneOptions();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}

	else if( wParam == ID_TOOLS_GEOMETRYBASEDSELECTION )
	{
		g_menu.m_geometryBasedSelection = !g_menu.m_geometryBasedSelection;
		ex_pBtnMaterialEditor->EnableWindow( FALSE );
		GetMenu()->EnableMenuItem( ID_TOOLS_MATERIALEDITOR, MF_DISABLED | MF_GRAYED );

		ex_pBtnScriptEditor->EnableWindow( FALSE );
		GetMenu()->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_DISABLED | MF_GRAYED );

		ex_pStaticSelectedObject->SetWindowTextA( "\n" );

		if( g_menu.m_geometryBasedSelection )
		{
			GetMenu()->CheckMenuItem( ID_TOOLS_GEOMETRYBASEDSELECTION, MF_CHECKED );
			PrintInfo3( "\nGeometry Based Selection enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_TOOLS_GEOMETRYBASEDSELECTION, MF_UNCHECKED );
			PrintInfo3( "\nMaterial Based Selection enabled" );
		}

		//material editor

		m_dlgEditMaterial->SetNormalBtnState( CFalse );
		m_dlgEditMaterial->SetDirtBtnState( CFalse );
		m_dlgEditMaterial->SetDiffuseBtnState( CFalse );
		m_dlgEditMaterial->SetAlphaBtnState( CFalse );
		m_dlgEditMaterial->SetGlossBtnState( CFalse );
		m_dlgEditMaterial->SetShadowBtnState( CFalse );

		m_dlgEditMaterial->SetEditBoxNormalMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxDirtMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxDiffuseName( "\n" );
		m_dlgEditMaterial->SetEditBoxAlphaMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxGlossMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxShadowMapName( "\n" );

		m_dlgEditMaterial->SetRemoveDirtBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveNormalBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveDiffuseBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveAlphaBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveGlossBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveShadowBtnState( CFalse );
		/////

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_TOOLS_MATERIALEDITOR)
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedEditMaterial();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();

	}
	else if( wParam == ID_VIEW_LIGHT_POSITIONS )
	{
		g_menu.m_showLightIcons = !g_menu.m_showLightIcons; 
		if( g_menu.m_showLightIcons )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_LIGHT_POSITIONS, MF_CHECKED );
			PrintInfo3( "\nLight icons enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_VIEW_LIGHT_POSITIONS, MF_UNCHECKED );
			PrintInfo3( "\nLight icons disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_SOUND_POSITIONS )
	{
		g_menu.m_showSoundIcons = !g_menu.m_showSoundIcons; 
		if( g_menu.m_showSoundIcons )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_SOUND_POSITIONS, MF_CHECKED );
			PrintInfo3( "\nSound icons enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_VIEW_SOUND_POSITIONS, MF_UNCHECKED );
			PrintInfo3( "\nSound icons disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_ICONS_WATERICONS )
	{
		g_menu.m_showWaterIcons = !g_menu.m_showWaterIcons; 
		if( g_menu.m_showWaterIcons )
		{
			GetMenu()->CheckMenuItem( ID_ICONS_WATERICONS, MF_CHECKED );
			PrintInfo3( "\nWater icons enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_ICONS_WATERICONS, MF_UNCHECKED );
			PrintInfo3( "\nWater icons disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_ICONS_SKYICON )
	{
		g_menu.m_showSkyIcon = !g_menu.m_showSkyIcon; 
		if( g_menu.m_showSkyIcon )
		{
			GetMenu()->CheckMenuItem( ID_ICONS_SKYICON, MF_CHECKED );
			PrintInfo3( "\nSky icon enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_ICONS_SKYICON, MF_UNCHECKED );
			PrintInfo3( "\nSky icon disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}	
	else if( wParam == ID_ICONS_CAMERAICONS )
	{
		g_menu.m_showCameraIcons = !g_menu.m_showCameraIcons; 
		if( g_menu.m_showCameraIcons )
		{
			GetMenu()->CheckMenuItem( ID_ICONS_CAMERAICONS, MF_CHECKED );
			PrintInfo3( "\nCamera icon enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_ICONS_CAMERAICONS, MF_UNCHECKED );
			PrintInfo3( "\nCamera icon disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_PERSPECTIVE_GRIDS )
	{
		g_menu.m_showPerspectiveGrids = !g_menu.m_showPerspectiveGrids; 
		if( g_menu.m_showPerspectiveGrids )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_PERSPECTIVE_GRIDS, MF_CHECKED );
			PrintInfo3( "\nPerspective grids enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_VIEW_PERSPECTIVE_GRIDS, MF_UNCHECKED );
			PrintInfo3( "\nPerspective grids disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_OCTREE )
	{
		g_menu.m_showOctree = !g_menu.m_showOctree; 
		if( g_menu.m_showOctree )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_OCTREE, MF_CHECKED );
			PrintInfo3( "\nOctree is visible" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_VIEW_OCTREE, MF_UNCHECKED );
			PrintInfo3( "\nOctree is invisible" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_BOUNDINGBOX )
	{
		g_menu.m_showBoundingBox = !g_menu.m_showBoundingBox; 
		if( g_menu.m_showBoundingBox )
		{
			gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1);
			GetMenu()->CheckMenuItem( ID_VIEW_BOUNDINGBOX, MF_CHECKED );
			PrintInfo3( "\nBounding boxes enabled" );
		}
		else
		{
			gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 0);
			GetMenu()->CheckMenuItem( ID_VIEW_BOUNDINGBOX, MF_UNCHECKED );
			PrintInfo3( "\nBounding boxes disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_SHADOWDEPTHMAP )
	{
		g_menu.m_showDynamicShadowDepthTexture = !g_menu.m_showDynamicShadowDepthTexture; 
		if( g_menu.m_showDynamicShadowDepthTexture )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_SHADOWDEPTHMAP, MF_CHECKED );
			PrintInfo3( "\nDepth map activated" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_VIEW_SHADOWDEPTHMAP, MF_UNCHECKED );
			PrintInfo3( "\nDepth map deactivated" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_STATISTICS )
	{
		g_menu.m_showStatistics = !g_menu.m_showStatistics;
		if( g_menu.m_showStatistics )
			GetMenu()->CheckMenuItem( ID_VIEW_STATISTICS, MF_CHECKED );
		else
			GetMenu()->CheckMenuItem( ID_VIEW_STATISTICS, MF_UNCHECKED );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_REPORT )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgConsole = CNew( CConsole );
		m_dlgConsole->DoModal();
		CDelete( m_dlgConsole );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_WARNINGSANDERRORS )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgWarnings = CNew( CWarnings );
		m_dlgWarnings->DoModal();
		CDelete( m_dlgWarnings);

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_ALLCHANNELS )
	{
		PrintInfo3( "\nSwitched to all channels" );

		g_materialChannels = eCHANNELS_ALL;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_DIFFUSECHANNEL )
	{
		PrintInfo3( "\nSwitched to diffuse map channel" );

		g_materialChannels = eCHANNELS_DIFFUSE;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL,  MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();

	}
	else if( wParam == ID_VIEW_NORMALMAPCHANNEL )
	{
		PrintInfo3( "\nSwitched to normal map channel" );

		g_materialChannels = eCHANNELS_NORMALMAP;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_GLOSSMAPCHANNEL )
	{
		PrintInfo3( "\nSwitched to gloss map channel" );

		g_materialChannels = eCHANNELS_GLOSSMAP;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_SHADOWMAPCHANNEL )
	{
		PrintInfo3( "\nSwitched to shadow map channel" );

		g_materialChannels = eCHANNELS_SHADOWMAP;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_DIRTMAPCHANNEL )
	{
		PrintInfo3( "\nSwitched to dirt map channel" );

		g_materialChannels = eCHANNELS_DIRTMAP;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_ALPHAMAPCHANNEL )
	{
		PrintInfo3( "\nSwitched to alpha map channel" );

		g_materialChannels = eCHANNELS_ALPHAMAP;
		g_menu.m_showVertexPositions = g_menu.m_showNormalVectors = CFalse;
		GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_CHECKED );

		GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_VIEW_NORMALVECTORS  )
	{
		PrintInfo3( "\nSwitched to normal vectors" );
		g_menu.m_showNormalVectors = !g_menu.m_showNormalVectors;
		g_menu.m_showVertexPositions = CFalse;
		if( g_menu.m_showNormalVectors )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
			GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

			GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_CHECKED );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
		}
		else
		{
			g_materialChannels = eCHANNELS_ALL;
			GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_CHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();
		}
	}
	else if( wParam == ID_VIEW_VERTEXPOSITIONS )
	{
		PrintInfo3( "\nSwitched to vertex positions" );

		g_menu.m_showVertexPositions = !g_menu.m_showVertexPositions;
		g_menu.m_showNormalVectors = CFalse;
		if( g_menu.m_showVertexPositions )
		{
			GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_DIFFUSECHANNEL, MF_UNCHECKED  );
			GetMenu()->CheckMenuItem( ID_VIEW_NORMALMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_GLOSSMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_SHADOWMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_DIRTMAPCHANNEL, MF_UNCHECKED );
			GetMenu()->CheckMenuItem( ID_CHANNELS_ALPHAMAPCHANNEL, MF_UNCHECKED );

			GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_CHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_NORMALVECTORS, MF_UNCHECKED );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();
		}
		else
		{
			g_materialChannels = eCHANNELS_ALL;
			GetMenu()->CheckMenuItem( ID_VIEW_ALLCHANNELS, MF_CHECKED );
			GetMenu()->CheckMenuItem( ID_VIEW_VERTEXPOSITIONS, MF_UNCHECKED );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();
		}
	}
	else if ( wParam == ID_POLGYGONMODE_FILL )
	{
		g_polygonMode = ePOLYGON_FILL;
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_FILL, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_LINE, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_POINT, MF_UNCHECKED );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if ( wParam == ID_POLGYGONMODE_LINE )
	{
		g_polygonMode = ePOLYGON_LINE;
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_FILL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_LINE, MF_CHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_POINT, MF_UNCHECKED );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if ( wParam == ID_POLGYGONMODE_POINT )
	{
		g_polygonMode = ePOLYGON_POINT;
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_FILL, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_LINE, MF_UNCHECKED );
		GetMenu()->CheckMenuItem( ID_POLGYGONMODE_POINT, MF_CHECKED );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_PHYSICS_DEBUGMODE )
	{
		g_menu.m_physicsDebugMode = !g_menu.m_physicsDebugMode;
		if( g_menu.m_physicsDebugMode )
		{
			GetMenu()->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_CHECKED );
			PrintInfo3( "\nPhysX Debug mode enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_UNCHECKED );
			PrintInfo3( "\nPhysX Debug mode disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_PHYSICS_APPLYGRAVITY )
	{
		g_menu.m_physicsGravity = ! g_menu.m_physicsGravity;
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedPhysicsGravity();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );


		if( g_menu.m_physicsGravity )
		{
			GetMenu()->CheckMenuItem( ID_PHYSICS_APPLYGRAVITY, MF_CHECKED );
			PrintInfo3( "\nPhysX Gravity enabled" );
		}
		else
		{
			GetMenu()->CheckMenuItem( ID_PHYSICS_APPLYGRAVITY, MF_UNCHECKED );
			PrintInfo3( "\nPhysX Gravity disabled" );
		}
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();


	}
	else if( wParam == ID_MODIFY_BLOOM )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditBloom = CNew( CEditBloom );
		m_dlgEditBloom->DoModal();
		CDelete( m_dlgEditBloom );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_MODIFY_LIGHT )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditLight = CNew( CEditLight );
		m_dlgEditLight->DoModal();
		CDelete( m_dlgEditLight );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_MODIFY_SCENEMANAGER )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		m_dlgEditSceneManager = CNew( CEditSceneManager );
		m_dlgEditSceneManager->DoModal();
		CDelete( m_dlgEditSceneManager );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
	else if( wParam == ID_HELP_ONLINEHELP )
	{
		//ShellExecute(NULL, "open", "data\\help\\vanda.chm", NULL, NULL, SW_SHOWNORMAL);
		ShellExecute(NULL, "open", "http://vandaengine.com/vandaengine/Doc/Reference/", NULL, NULL, SW_SHOWNORMAL);

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if( wParam == ID_HELP_ABOUT )
	{
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		CAboutDlg dlgAbout;
		dlgAbout.DoModal();

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

		g_multipleView->SetElapsedTimeFromBeginning();
	}
	else if( wParam == ID_HELP_HOMEPAGE )
	{
		ShellExecute(NULL, "open", "http://www.vandaengine.com", NULL, NULL, SW_SHOWNORMAL);
		g_multipleView->SetElapsedTimeFromBeginning();
	}
	return CDialog::OnCommand(wParam, lParam);
}

CVoid CVandaEngine1Dlg::OnMenuClickedNew( CBool askQuestion )
{
	CInt iResponse;
	if( !askQuestion )
		iResponse = IDNO; //do not save changes
	else if( g_scene.size() > 0 || g_engineLights.size() > 0 || g_engineWaters.size() > 0 || g_menu.m_insertAndShowSky || g_menu.m_insertAmbientSound || g_engineStaticSounds.size() > 0 ||  g_multipleView->m_nx->m_hasScene )
		iResponse = MessageBox( "Do you want to save your changes?", "Warning" , MB_YESNOCANCEL|MB_ICONSTOP);

	CBool newScene = CFalse;
	if (iResponse == IDYES) //save changes
	{
		newScene = CTrue;
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSaveAs();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );

	}
	if (iResponse == IDNO || newScene)
	{
		SetCapture();
		SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );

		g_dofProperties.Reset();
		g_fogProperties.Reset();
		g_bloomProperties.Reset();
		g_lightProperties.Reset();
		g_shadowProperties.Reset();
		if(!g_vandaDemo)
			g_shadowProperties.m_enable = CTrue;
		g_pathProperties.Reset();
		g_sceneBanner.SetBannerPath("Assets/Engine/Textures/Loading.dds");
		g_sceneBanner.ClearVScenes();
		CDelete(g_externalPhysX);
		g_extraTexturesNamingConventions.Reset();
		g_sceneManagerObjectsPerSplit = 15;
		//g_options.Reset();
		g_useGlobalAmbientColor = CFalse;
		g_globalAmbientColor.r = g_globalAmbientColor.g = g_globalAmbientColor.b = 0.5f;
		g_globalAmbientColor.a = 1.0f;

		m_engineObjectListIndex = -1;
		m_sceneListIndex = -1;
		m_physXElementListIndex = -1;
		g_updateTextureViaEditor = CFalse;

		g_octree->ResetState();
		g_render.SetScene( NULL );
		Cpy(g_shadowProperties.m_directionalLightName, "\n" );
		if( g_multipleView->m_nx->m_hasScene )
		{
			ResetPhysX();
			g_menu.m_physicsDebugMode = CFalse;
			GetMenu()->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_UNCHECKED );
			PrintInfo3( "\nPhysX debug deactivated" );

		}
		for( std::vector<CWater*>::iterator it = g_engineWaters.begin(); it != g_engineWaters.end(); it++ )
		{
			CDelete( *it );
		}
		if( g_engineWaters.size() > 0 )
			g_engineWaters.clear();

		for( std::vector<CStaticSound*>::iterator it = g_engineStaticSounds.begin(); it != g_engineStaticSounds.end(); it++ )
		{
			CDelete( *it );
		}
		if( g_engineStaticSounds.size() > 0 )
			g_engineStaticSounds.clear();

		if( g_engineObjectNames.size() > 0 )
			g_engineObjectNames.clear();
		//clear the scene
		for( std::vector<CScene*>::iterator it = g_scene.begin(); it != g_scene.end(); it++ )
			CDelete( *it );
		//Clear the vctor objects
		if( g_scene.size() > 0 ) 
			g_scene.clear();
		
		//clear all the default engines here
		for( std::vector<CInstanceLight*>::iterator it = g_engineLights.begin(); it != g_engineLights.end(); it++ )
		{
			CDelete( (*it)->m_abstractLight ) ;
			CDelete( *it );
		}
		//Clear the vctor objects
		if( g_engineLights.size() > 0 )
			g_engineLights.clear();

		for( std::vector<CImage*>::iterator it = g_images.begin(); it != g_images.end(); it++ )
		{
			CDelete( *it );
		}

		if( g_images.size() > 0 )
			g_images.clear();

		for( std::vector<CImage*>::iterator it = g_waterImages.begin(); it != g_waterImages.end(); it++ )
		{
			CDelete( *it );
		}
		if( g_waterImages.size() > 0 )
			g_waterImages.clear();

		for( std::vector<COpenALSoundBuffer*>::iterator it = g_soundBuffers.begin(); it != g_soundBuffers.end(); it++ )
		{
			CDelete( *it );
		}
		g_soundBuffers.clear();

		g_menu.m_insertAndShowSky = CFalse;
		CDelete( g_skyDome );

		//delete ambient sound
		if( g_menu.m_insertAmbientSound )
		{
			g_multipleView->m_soundSystem->StopALSound( *(g_multipleView->m_ambientSound->GetSoundSource()) );
			alSourcei(g_multipleView->m_ambientSound->GetSoundSource()->GetSource(), AL_BUFFER, AL_NONE); 
			CDelete( g_multipleView->m_ambientSound );			
		}
		g_menu.m_insertAmbientSound = CFalse;
		GetMenu()->EnableMenuItem( ID_INSERT_SOUND_AMBIENT, MF_ENABLED );
		m_mainBtnAmbientSound.EnableWindow( TRUE );

		m_savePathName = "\n";
		//Reset name indexes which are used for selection
		g_nameIndex = 1;
		CDelete( g_arrowScene );
		g_arrowScene = CNew( CScene );
		g_reportInfo = CFalse;
		if( !g_arrowScene->Load( "Assets/engine/Arrows/Arrows.dae", CFalse ) )
		{
			PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		}
		g_render.SetScene( g_arrowScene );
		g_arrowScene->Update();

		//CDelete (g_zArrow);
		//g_zArrow = CNew( CScene );
		//if( !g_zArrow->Load( "Assets/engine/Arrows/ZArrow.dae", CFalse ) )
		//{
		//	PrintInfo( "\nCouldn't load the Z arrow", COLOR_RED );
		//}
		//g_zArrow->Update();

		CDelete( g_negativeZArrow );
		g_negativeZArrow = CNew( CScene );
		if( !g_negativeZArrow->Load( "Assets/engine/Arrows/NegativeZArrow.dae", CFalse ) )
		{
			PrintInfo( "\nCouldn't load the negative Z arrow", COLOR_RED );
		}
		g_render.SetScene( g_negativeZArrow );
		g_negativeZArrow->Update();

		g_showArrow = CFalse;

		CDelete( g_centerArrowScene );

		g_centerArrowScene = CNew( CScene );
		if( !g_centerArrowScene->Load( "Assets/engine/Arrows/Arrows.dae", CFalse ) )
		{
			PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		}
		g_reportInfo = CTrue;
		g_render.SetScene(g_centerArrowScene); 
		g_centerArrowScene->Update();

		g_selectedName = g_tempLastEngineObjectSelectedName = g_lastEngineObjectSelectedName = g_multipleView->m_tempSelectedName = -1; 		//no object has been selected
		g_translateObject = CFalse;
		GetMenu()->EnableMenuItem( ID_INSERT_SKYDOME, MF_ENABLED );
		m_mainBtnSky.EnableWindow( TRUE );

		//Delete all items from list boxes

		for (int nItem = m_listBoxScenes.GetItemCount()-1; nItem >= 0 ;nItem-- )
		{
			m_listBoxScenes.DeleteItem(nItem);
		}
		m_btnRemoveScene.EnableWindow( FALSE );
		m_btnSceneProperties.EnableWindow( FALSE );
		for (int nItem2 = m_listBoxObjects.GetItemCount()-1; nItem2 >= 0 ;nItem2-- )
		{
			m_listBoxObjects.DeleteItem(nItem2);
		}
		for (int nItem3 = m_listBoxEngineObjects.GetItemCount()-1; nItem3 >= 0 ;nItem3-- )
		{
			m_listBoxEngineObjects.DeleteItem(nItem3);
		}
		m_btnRemoveEngineObject.EnableWindow( FALSE );
		m_btnEngineObjectProperties.EnableWindow( FALSE );

		for (int nItem4 = m_listBoxPhysXElements.GetItemCount()-1; nItem4 >= 0 ;nItem4-- )
		{
			m_listBoxPhysXElements.DeleteItem(nItem4);
		}
		m_btnRemovePhysX.EnableWindow( FALSE );
		//material editor

		m_dlgEditMaterial->SetNormalBtnState( CFalse );
		m_dlgEditMaterial->SetDirtBtnState( CFalse );
		m_dlgEditMaterial->SetDiffuseBtnState( CFalse );
		m_dlgEditMaterial->SetAlphaBtnState( CFalse );
		m_dlgEditMaterial->SetGlossBtnState( CFalse );
		m_dlgEditMaterial->SetShadowBtnState( CFalse );

		m_dlgEditMaterial->SetEditBoxNormalMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxDirtMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxDiffuseName( "\n" );
		m_dlgEditMaterial->SetEditBoxAlphaMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxGlossMapName( "\n" );
		m_dlgEditMaterial->SetEditBoxShadowMapName( "\n" );

		m_dlgEditMaterial->SetRemoveDirtBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveNormalBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveDiffuseBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveAlphaBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveGlossBtnState( CFalse );
		m_dlgEditMaterial->SetRemoveShadowBtnState( CFalse );
		/////

		ex_pBtnNextAnim->EnableWindow( FALSE );
		ex_pBtnPrevAnim->EnableWindow( FALSE );
		ex_pBtnPlayAnim->EnableWindow( FALSE );

		ex_pBtnMaterialEditor->EnableWindow( FALSE );
		GetMenu()->EnableMenuItem( ID_TOOLS_MATERIALEDITOR, MF_DISABLED | MF_GRAYED );

		ex_pBtnScriptEditor->EnableWindow( FALSE );
		GetMenu()->EnableMenuItem( ID_TOOLS_SCRIPTMANAGER, MF_DISABLED | MF_GRAYED );

		NxScene* tempScene = gPhysXscene/*gPhysicsSDK->getScene(i)*/;
		for( CUInt j = 0; j < tempScene->getNbActors(); j++ )
		{
			if( !tempScene->getActors()[j]->getName() ) continue; //character controller has no name. Do not remove character controller

			tempScene->releaseActor( *tempScene->getActors()[j] );
		}
		Cpy( g_currentVSceneName, "\n" ); //save functions

		PrintInfo( "\nScene cleared successfully" );
		ReleaseCapture();
	}
	else if (iResponse == IDCANCEL)
		return;
}

CVoid CVandaEngine1Dlg::OnMenuClickedImportCollada()
{
    // Create an Open dialog; the default file name extension is ".bmp".
	CFileDialog dlgOpen(TRUE, _T("*.dae"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("COLLADA File (*.dae)|*.dae||"), NULL, NULL);

	if( dlgOpen.DoModal() == IDOK )
	{
		//CString fileName( dlgOpen.GetFileName() );
		//for example this code converts 'test.dae' to 'data\test\test.dae'
		//CChar fileWithoutDot[MAX_NAME_SIZE];
		//Cpy( fileWithoutDot, (char*)fileName.GetString() );
		//GetWithoutDot( fileWithoutDot );
		//CChar filePath[MAX_NAME_SIZE];
		//sprintf( filePath, "%s%s", g_pathProperties.m_meshPath, (char*)fileName.GetString() );
		CString fileName( dlgOpen.GetPathName() );
		CString PurefileName( dlgOpen.GetFileName());

		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( Cmp( g_scene[i]->m_pureFileName, PurefileName.GetString()  ) )
			{
				if( MessageBox( "This COLLADA file already exists! Do you want to over write it?", "Vanda Engine 1 Warning", MB_ICONWARNING | MB_YESNO ) == IDYES)
				{
					for (int nItem = m_listBoxScenes.GetItemCount()-1; nItem >= 0 ;nItem-- )
					{
						CChar text[1204];
						m_listBoxScenes.GetItemText(nItem,0, text, 1024);
						if( Cmp( g_scene[i]->m_pureFileName, text  ) )
						{
							//select the item
							m_listBoxScenes.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP);
							m_listBoxScenes.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED | LVIS_FOCUSED  | LVS_SHOWSELALWAYS);
							m_listBoxScenes.SetSelectionMark(nItem);
							m_askRemoveScene = CFalse;
							OnBnClickedBtnRemoveScene();
							m_askRemoveScene = CTrue;
							break;
						}
					}
				}
				else
				return;
			}
		}

		SetCapture();
		SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );
		g_octree->ResetState();
		g_importColladaImages = CTrue;

		CScene * tempScene = new CScene();
		g_useOriginalPathOfDAETextures = CTrue;
		if( tempScene->Load( (char*)fileName.GetString() /*filePath*/, CTrue, CFalse ) )
		{
			tempScene->SetClipIndex( 0 );

			//save functions/////////////////////////////////
			for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
			{
				tempScene->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
			}
			//save functions/////////////////////////////////
			g_currentScene = tempScene; //mark the current scene. Save functions
			tempScene->SetupDiffuseTexture();
			PrintInfo( "\nReading Alpha Maps...", COLOR_GREEN );
			tempScene->SetupAlphaTexture();
			PrintInfo( "\nReading Gloss Maps...", COLOR_GREEN );
			tempScene->SetupGlossTexture();
			PrintInfo( "\nReading Normal Maps...", COLOR_GREEN );
			tempScene->SetupNormalTexture();
			PrintInfo( "\nReading Shadow Maps...", COLOR_GREEN );
			tempScene->SetupShadowTexture();
			PrintInfo( "\nReading Dirt Maps...", COLOR_GREEN );
			tempScene->SetupDirtTexture();

			g_scene.push_back( tempScene );
			InsertItemToSceneList( tempScene->m_pureFileName );

			//remove the current contents of the object list
			for (int nItem = m_listBoxObjects.GetItemCount()-1; nItem >= 0 ;nItem-- )
			{
				m_listBoxObjects.DeleteItem(nItem);
			}

			for( CUInt j = 0; j < tempScene->m_images.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_images[j]->GetName(), eOBJECTLIST_IMAGE);
			}
			for( CUInt j = 0; j < tempScene->m_effects.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_effects[j]->GetName(), eOBJECTLIST_EFFECT);
			}
			for( CUInt j = 0; j < tempScene->m_materials.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_materials[j]->GetName(), eOBJECTLIST_MATERIAL);
			}
			//animations
			for( CUInt j = 0; j < tempScene->m_animations.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_animations[j]->GetName(), eOBJECTLIST_ANIMATION);
			}
			for( CUInt j = 0; j < tempScene->m_animationClips.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_animationClips[j]->GetName(), eOBJECTLIST_ANIMATIONCLIP);
			}
			//lights
			for( CUInt j = 0; j < tempScene->m_lights.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_lights[j]->GetName(), eOBJECTLIST_LIGHT);
			}
			//for( CUInt j = 0; j < tempScene->m_lightInstances.size(); j++ )
			//{
			//	InserItemToObjectList( tempScene->m_lightInstances[j]->GetName());
			//}
			//camera
			for( CUInt j = 0; j < tempScene->m_cameras.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_cameras[j]->GetName(), eOBJECTLIST_CAMERA);
			}
			//for( CUInt j = 0; j < tempScene->m_cameraInstances.size(); j++ )
			//{
			//	InserItemToObjectList( tempScene->m_cameraInstances[j]->GetName());
			//}
			//geometry
			for( CUInt j = 0; j < tempScene->m_geometries.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_geometries[j]->GetName(), eOBJECTLIST_MESH);
			}
			//for( CUInt j = 0; j < tempScene->m_instanceGeometries.size(); j++ )
			//{
			//	InserItemToObjectList( tempScene->m_instanceGeometries[j]->GetName());
			//}
			//controller
			for( CUInt j = 0; j < tempScene->m_controllers.size(); j++ )
			{
				InserItemToObjectList( tempScene->m_controllers[j]->GetName(), eOBJECTLIST_SKINCONTROLLER);
			}

			//Door
			if( CmpIn( tempScene->GetName(), "trigger" ) )
			{
				if(!g_multipleView->m_nx->m_hasScene)
				{
					tempScene->m_isTrigger = CTrue;
					tempScene->Update();
					tempScene->CreateTrigger( g_multipleView->m_nx );
				}
				else
				{
					PrintInfo( "\nCouldn't create triggers. Please remove current external PhysX scene", COLOR_RED );
				}
			}
			else if( CmpIn( tempScene->GetName(), "grass" ) )
			{
				tempScene->m_isGrass = CTrue;
				for( CUInt j = 0; j < tempScene->m_geometries.size(); j++ )
				{
					tempScene->m_geometries[j]->SetAlphaMap(  "grass_alpha" );
					tempScene->m_geometries[j]->SetDiffuse( "grass_color" );
				}
			}

			PrintInfo("\nscene '");
			PrintInfo(tempScene->m_fileName, COLOR_RED_GREEN);
			PrintInfo("' imported successufully");
		}
		else
		{
			delete tempScene;
			tempScene = NULL;

		}
		ReleaseCapture();
	}
}

CVoid CVandaEngine1Dlg::OnMenuClickedImportPhysX()
{
    // Create an Open dialog; the default file name extension is ".bmp".
	CFileDialog dlgOpen(TRUE, _T("*.xml"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("PhysX XML File (*.xml)|*.xml||"), NULL, NULL);

	if( dlgOpen.DoModal() == IDOK )
	{
		if( MessageBox( "Are you sure you want to replace the current PhysX scene with new PhysX scene?\nCurrent PhysX actors are lost", "Warning", MB_YESNO | MB_ICONWARNING) == IDYES )
		{
			SetCapture();
			SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );
			CString fileName( dlgOpen.GetPathName() );
			//for example this code converts 'test.nxb' to 'data\test\test.nxb'
			//CChar fileWithoutDot[MAX_NAME_SIZE];
			///Cpy( fileWithoutDot, (char*)fileName.GetString() );
			//GetWithoutDot( fileWithoutDot );
			CChar filePath[MAX_NAME_SIZE];
			sprintf( filePath, "%s", (char*)fileName.GetString() );
			
			CChar temp[MAX_NAME_SIZE];

			ResetPhysX();
			for( CUInt i = 0; i < g_scene.size(); i++ )
			{
				for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
				{
					g_scene[i]->m_instanceGeometries[j]->m_hasPhysX = CFalse;
					Cpy( g_scene[i]->m_instanceGeometries[j]->m_physXName, "\n" );
				}
			}
			if( g_multipleView->m_nx->LoadScene( filePath, NXU::FT_XML) )
			{
				g_menu.m_physicsDebugMode = CTrue;
				GetMenu()->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_CHECKED );
				PrintInfo3( "\nPhysX debug activated" );

				g_multipleView->m_nx->SetSceneName( filePath );
				sprintf( temp, "\nPhysX scene '%s' imported successufully", filePath );
				PrintInfo( temp );
				m_btnRemovePhysX.EnableWindow( TRUE );
				m_physXElementListIndex = -1;
				for (int nItem = m_listBoxPhysXElements.GetItemCount()-1; nItem >= 0 ;nItem-- )
				{
					m_listBoxPhysXElements.DeleteItem(nItem);
				}
				CInt count = 0;
				for( std::vector<std::string>::iterator it = g_multipleView->m_nx->m_nxActorNames.begin(); it != g_multipleView->m_nx->m_nxActorNames.end();it++ )
				{
					CChar temp[MAX_NAME_SIZE];
					Cpy( temp, (*it).c_str() );
					InsertItemToPhysXList( temp, g_multipleView->m_nx->m_nxActorTypes[count] );
					count++;
				}
				CDelete(g_externalPhysX);
				g_externalPhysX = CNew(CExternalPhysX);
				g_externalPhysX->SetPhysXPath(filePath);
				//save functions/////////////////////////////////
				for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
				{
					g_externalPhysX->AddVSceneToList( g_allVSceneNames[index], CTrue);//Write to zip file and copy the texture
				}
				//save functions/////////////////////////////////

			}
			else
			{
				sprintf( temp, "\ncouldn't load the PhysX scene '%s'", filePath );
				PrintInfo( temp, COLOR_RED );
			}
			ReleaseCapture();
		}
	}
}

CVoid CVandaEngine1Dlg::OnMenuClickedGeneralOptions()
{
	m_dlgOptions = CNew( CEditOptions );
	INT_PTR result = m_dlgOptions->DoModal();
	CDelete( m_dlgOptions );
}

CVoid CVandaEngine1Dlg::OnMenuClickedSceneOptions()
{
	m_dlgSceneOptions = CNew( CEditSceneOptions );
	INT_PTR result = m_dlgSceneOptions->DoModal();
	CDelete( m_dlgSceneOptions );
}

CVoid CVandaEngine1Dlg::OnMenuClickedSelectCamera()
{
	m_dlgSelectCamera = CNew( CSelectCamera );
	INT_PTR result = m_dlgSelectCamera->DoModal();
	CDelete( m_dlgSelectCamera );
}

CVoid CVandaEngine1Dlg::OnMenuClickedWaterAttachment()
{
	m_dlgWaterAttachment = CNew( CWaterAttachment );
	INT_PTR result = m_dlgWaterAttachment->DoModal();
	CDelete( m_dlgWaterAttachment );
}

CVoid CVandaEngine1Dlg::OnMenuClickedInsertLight()
{
	m_dlgAddLight = CNew( CAddLight );
	m_dlgAddLight->SetCreate( CTrue );
	INT_PTR result = m_dlgAddLight->DoModal();
	if ( result  == IDOK )
	{
		CInstanceLight* instance_light = new CInstanceLight();
		CLight* abstract_light = new CLight();

		instance_light->m_parent = NULL; //Not using COLLADA parent
		instance_light->m_abstractLight = abstract_light;
		abstract_light->SetName( m_dlgAddLight->GetName() );
		abstract_light->SetType( m_dlgAddLight->GetType() );
		switch( m_dlgAddLight->GetType() )
		{
		case eLIGHTTYPE_SPOT:
			instance_light->m_abstractLight->SetSpotCutoff( m_dlgAddLight->GetSpotCuttoff() );
			instance_light->m_abstractLight->SetSpotExponent( m_dlgAddLight->GetSpotExponent() );
			instance_light->m_abstractLight->SetSpotDirection( m_dlgAddLight->GetSpotDirection() );
			break;
		}

		abstract_light->SetAmbient ( m_dlgAddLight->GetAmbientColor() );
		abstract_light->SetDiffuse( m_dlgAddLight->GetDiffuseColor() );
		abstract_light->SetSpecular( m_dlgAddLight->GetSpecularColor() );
		abstract_light->SetPosition( m_dlgAddLight->GetLightPos() );
		abstract_light->SetShininess( m_dlgAddLight->GetShininess() );
		abstract_light->SetConstantAttenuation( m_dlgAddLight->GetConstantAttenuation() );
		abstract_light->SetLinearAttenuation( m_dlgAddLight->GetLinearAttenuation() );
		abstract_light->SetQuadraticAttenuation( m_dlgAddLight->GetQuadAttenuation() );

		instance_light->SetIndex();

		g_engineLights.push_back( instance_light );

		InserItemToEngineObjectList( abstract_light->GetName(), eENGINEOBJECTLIST_LIGHT );
		g_engineObjectNames.push_back( m_dlgAddLight->GetName() );

		CDelete( m_dlgAddLight );
		PrintInfo( "\nNew light added successfully" );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddLight );
}

CVoid CVandaEngine1Dlg::OnMenuClickedInsertStaticSound()
{
	m_dlgAddStaticSound = CNew( CAddStaticSound );
	m_dlgAddStaticSound->SetCreate( CTrue );
	INT_PTR result = m_dlgAddStaticSound->DoModal();
	if ( result  == IDOK )
	{
		COpenALSoundSource* m_staticSoundSource = CNew( COpenALSoundSource );
		CStaticSound* m_staticSound = CNew( CStaticSound );

		CChar temp[ MAX_NAME_SIZE];
		sprintf( temp, "%s", m_dlgAddStaticSound->m_strStaticSoundDataPath );
		COpenALSoundBuffer* m_staticSoundBuffer = GetSoundBuffer( GetAfterPath(temp) );
		if( m_staticSoundBuffer == NULL || (m_staticSoundBuffer && !m_staticSoundBuffer->m_loaded ) )
		{
			if( m_staticSoundBuffer == NULL )
			{
				m_staticSoundBuffer = CNew( COpenALSoundBuffer );
				g_soundBuffers.push_back( m_staticSoundBuffer );
			}
			else 
			{
				CChar tempBuffer[MAX_NAME_SIZE];
				sprintf( tempBuffer, "\nTrying to reload '%s%s", GetAfterPath(m_staticSoundBuffer->GetName() ), "'" );
				PrintInfo( tempBuffer, COLOR_YELLOW );
			}
			if( !m_staticSoundBuffer->LoadOggVorbisFromFile( temp ) )
			{
				CChar buffer[MAX_NAME_SIZE];
				sprintf( buffer, "\n%s%s%s", "Couldn't load the file '", temp, "'" );
				PrintInfo( buffer, COLOR_RED );
				m_staticSoundBuffer->m_loaded = CFalse;

			}
			else
			{
				CChar buffer[MAX_NAME_SIZE];
				sprintf( buffer, "\n%s%s%s", "ogg file '", temp, "' loaded successfully." );
				PrintInfo( buffer );
				m_staticSoundBuffer->m_loaded = CTrue;
			}
			m_staticSoundBuffer->SetName( temp );	
		}
		else
		{
				CChar temp[MAX_NAME_SIZE]; 
				sprintf( temp, "\n%s%s%s", "sound buffer '", GetAfterPath(m_staticSoundBuffer->GetName()), "' already exists." );
				PrintInfo( temp, COLOR_YELLOW );
		}

		m_staticSoundSource->BindSoundBuffer (*m_staticSoundBuffer);
		m_staticSoundSource->SetLooping( m_dlgAddStaticSound->GetLoopCondition() );
		m_staticSoundSource->SetPitch( m_dlgAddStaticSound->GetPitch() );
		m_staticSoundSource->SetReferenceDistance( m_dlgAddStaticSound->GetReferenceDistance() );
		m_staticSoundSource->SetMaxDistance( m_dlgAddStaticSound->GetMaxDistance() );
		m_staticSoundSource->SetRolloff( m_dlgAddStaticSound->GetRolloff() );
		m_staticSoundSource->SetSoundPosition( m_dlgAddStaticSound->GetSoundPos() );
		if( m_dlgAddStaticSound->GetPlayCondition() )
			g_multipleView->m_soundSystem->PlayALSound( *m_staticSoundSource );

		m_staticSound->SetName( m_dlgAddStaticSound->GetName() );
		m_staticSound->SetPath( temp );
		m_staticSound->SetPosition( m_dlgAddStaticSound->GetSoundPos() );
		m_staticSound->SetLoop( m_dlgAddStaticSound->GetLoopCondition() );
		m_staticSound->SetMaxDistance( m_dlgAddStaticSound->GetMaxDistance() );
		m_staticSound->SetPitch( m_dlgAddStaticSound->GetPitch() );
		m_staticSound->SetPlay( m_dlgAddStaticSound->GetPlayCondition() );
		m_staticSound->SetRefrenceDistance( m_dlgAddStaticSound->GetReferenceDistance() );
		m_staticSound->SetRolloff( m_dlgAddStaticSound->GetRolloff() );
		m_staticSound->SetSoundSource( m_staticSoundSource );
		m_staticSound->SetSoundBuffer( m_staticSoundBuffer );

		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			m_staticSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		g_engineStaticSounds.push_back( m_staticSound );
		InserItemToEngineObjectList( m_staticSound->GetName(), eENGINEOBJECTLIST_STATICSOUND );
		g_engineObjectNames.push_back( m_dlgAddStaticSound->GetName() );
		CDelete( m_dlgAddStaticSound );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddStaticSound );
}

CVoid CVandaEngine1Dlg::OnMenuClickedInsertSkyDome()
{
	m_dlgAddSkyDome = CNew( CAddSkyDome );
	m_dlgAddSkyDome->SetCreate( CTrue );
	INT_PTR result = m_dlgAddSkyDome->DoModal();
	if ( result == IDOK )
	{
		CChar temp[ MAX_NAME_SIZE];
		sprintf( temp, "%s", m_dlgAddSkyDome->m_strSkyDomePath );
		g_skyDome = CNew( CSkyDome );
		g_skyDome->SetName( m_dlgAddSkyDome->GetName() );
		g_skyDome->SetPath( temp );
		g_skyDome->SetRadius( m_dlgAddSkyDome->GetRadius() );
		g_skyDome->SetPosition( m_dlgAddSkyDome->GetPos() );
		g_skyDome->SetDampening( m_dlgAddSkyDome->GetDampening() );
		g_skyDome->SetSides( m_dlgAddSkyDome->GetSides() );
		g_skyDome->SetSlices( m_dlgAddSkyDome->GetSlices() );
		g_skyDome->SetExponential( m_dlgAddSkyDome->GetExponential() );

		g_skyDome->Initialize();
		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			g_skyDome->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		GetMenu()->EnableMenuItem( ID_INSERT_SKYDOME, MF_DISABLED | MF_GRAYED );
		m_mainBtnSky.EnableWindow( FALSE );

		g_menu.m_insertAndShowSky = CTrue;
		InserItemToEngineObjectList( g_skyDome->GetName(), eENGINEOBJECTLIST_SKY);
		g_engineObjectNames.push_back( m_dlgAddSkyDome->GetName() );
		CDelete( m_dlgAddSkyDome );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddSkyDome );

}

CVoid CVandaEngine1Dlg::OnMenuClickedInsertWater()
{
	m_dlgAddWater = CNew( CAddWater );
	m_dlgAddWater->SetCreate( CTrue );
	INT_PTR result = m_dlgAddWater->DoModal();
	if ( result  == IDOK )
	{
		CWater* water = new CWater;
		water->SetName( m_dlgAddWater->GetName() );
		water->SetDuDvMap( m_dlgAddWater->GetDuDvMap(), CTrue );
		water->SetNormalMap( m_dlgAddWater->GetNormalMap(), CTrue );
		water->SetHeight( m_dlgAddWater->GetHeight() );
		water->SetPos( m_dlgAddWater->GetPos() );
		water->SetLightPos( m_dlgAddWater->GetLightPos() );
		water->SetScale( m_dlgAddWater->GetScale() );
		water->SetSpeed( m_dlgAddWater->GetSpeed() );
		water->SetUV( m_dlgAddWater->GetUV() );
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFLECTION_ID );
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFRACTION_ID );
		water->CreateRenderTexture(g_waterTextureSize, 1, GL_DEPTH_COMPONENT, WATER_DEPTH_ID );
		water->SetSideVertexPositions();

		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			water->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		g_engineWaters.push_back( water );
		InserItemToEngineObjectList( water->GetName() , eENGINEOBJECTLIST_WATER);
		g_engineObjectNames.push_back( m_dlgAddWater->GetName() );
		CDelete( m_dlgAddWater );
		PrintInfo( "\nNew water surface added successfully" );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddWater );
}

CVoid CVandaEngine1Dlg::OnMenuClickedInsertAmbientSound()
{
	m_dlgAddAmbientSound = CNew( CAddAmbientSound );
	m_dlgAddAmbientSound->SetCreate( CTrue );
	INT_PTR result = m_dlgAddAmbientSound->DoModal();
	if ( result  == IDOK )
	{
		if( g_multipleView->m_ambientSound )
		{
			g_multipleView->m_soundSystem->StopALSound( *(g_multipleView->m_ambientSound->GetSoundSource()) );
			alSourcei(g_multipleView->m_ambientSound->GetSoundSource()->GetSource(), AL_BUFFER, AL_NONE);
			CDelete( g_multipleView->m_ambientSound )
		}

		COpenALSoundSource* m_ambientSoundSource = CNew( COpenALSoundSource );
		COpenALSoundBuffer* m_ambientSoundBuffer = CNew( COpenALSoundBuffer );

		CChar temp[ MAX_NAME_SIZE];
		sprintf( temp, "%s", m_dlgAddAmbientSound->m_strAmbientSoundBuffer );
		if( !m_ambientSoundBuffer->LoadOggVorbisFromFile( temp ) )
			return;
		m_ambientSoundSource->BindSoundBuffer( *(m_ambientSoundBuffer) );

		m_ambientSoundSource->SetLooping( true );
		m_ambientSoundSource->SetPitch( m_dlgAddAmbientSound->GetPitch() );
		m_ambientSoundSource->SetVolume( m_dlgAddAmbientSound->GetVolume() );

		g_multipleView->m_ambientSound = CNew( CAmbientSound );
		g_multipleView->m_ambientSound->SetSoundSource( m_ambientSoundSource );
		g_multipleView->m_ambientSound->SetSoundBuffer( m_ambientSoundBuffer );
		g_multipleView->m_ambientSound->SetName( m_dlgAddAmbientSound->GetName() );
		g_multipleView->m_ambientSound->SetPath( temp );
		g_multipleView->m_ambientSound->SetVolume( m_dlgAddAmbientSound->GetVolume() );
		g_multipleView->m_ambientSound->SetPitch( m_dlgAddAmbientSound->GetPitch() );

		g_multipleView->m_soundSystem->PlayALSound( *m_ambientSoundSource );
		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			g_multipleView->m_ambientSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		PrintInfo( "\nAmbient sound '" );
		sprintf( temp, "%s", m_dlgAddAmbientSound->m_strAmbientSoundBuffer );
		PrintInfo( temp, COLOR_RED_GREEN );
		PrintInfo( "' loaded successfully" );
		
		g_menu.m_playAmbientSound = CTrue;

		g_menu.m_insertAmbientSound = CTrue;
		InserItemToEngineObjectList( g_multipleView->m_ambientSound->GetName(), eENGINEOBJECTLIST_AMBIENTSOUND );
		GetMenu()->EnableMenuItem( ID_INSERT_SOUND_AMBIENT, MF_DISABLED | MF_GRAYED );
		m_mainBtnAmbientSound.EnableWindow( FALSE );
		g_engineObjectNames.push_back( m_dlgAddAmbientSound->GetName() );
		CDelete( m_dlgAddAmbientSound );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddAmbientSound );
}


CVoid CVandaEngine1Dlg::OnMenuClickedSaveAs( CBool askQuestion )
{
	CInt result;
	CFileDialog dlgSave(FALSE, _T("*.vin"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
		_T("File (*.vin)|*.vin||"), this, NULL);
	CString m_strpathName;

	if( !askQuestion && m_savePathName.Compare( "\n" ) != 0 )
	{
		result = IDOK; 
		m_strpathName = m_savePathName;
	}
	else if( Cmp( g_currentVSceneName, "\n" ) ) //nothing has been saved
	{
		m_dlgSaveOptions = CNew(CSaveOptions);
		result = m_dlgSaveOptions->DoModal();
	}
	else
	{
		result = dlgSave.DoModal();
	}
	
	if (result == IDOK)
	{
		SetCapture();
		SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );

		if( Cmp( g_currentVSceneName, "\n" ) ) //nothing has been saved
		{
			m_strpathName = m_dlgSaveOptions->m_strDestination;
			m_savePathName = m_strpathName;
			if( g_vandaDemo )
				Cpy( g_currentPassword, "\n" );
			else
			{
				Cpy( g_currentPassword, (CChar*)m_dlgSaveOptions->m_strPassword.GetBuffer( m_dlgSaveOptions->m_strPassword.GetLength() ) );
				m_dlgSaveOptions->m_strPassword.ReleaseBuffer();
			}
		}
		else if( askQuestion || m_savePathName.Compare( "\n" ) == 0 )
		{
			m_strpathName = (CString)dlgSave.GetPathName();
			m_savePathName = m_strpathName;
		}
		//Save functions////////////////////////////////////
		
		CBool overwriteExistingScene = CFalse;
		CChar* pureFileName =  (CChar*)GetAfterPath(m_savePathName.GetBuffer( m_savePathName.GetLength()));
		for( CUInt vs = 0; vs < g_allVSceneNames.size(); vs++ )
		{
			if (Cmp(pureFileName, g_allVSceneNames[vs].c_str() ) )
			{
				overwriteExistingScene = CTrue;
				break;
			}
		}

		CInt saveAlgorithm = -1;
		if( Cmp( g_currentVSceneName, "\n" ) )
		{
			if( overwriteExistingScene ) //scene already exists
			{
				saveAlgorithm = 0;
			}
			else
			{
				saveAlgorithm = 1;
			}
		} //if( Cmp( g_currentVSceneName, "\n" ) )
		else
		{
			if( Cmp( g_currentVSceneName, pureFileName ) ) //2-b
			{
				saveAlgorithm = 2;
			}
			else if( overwriteExistingScene ) //scene already exists 2-a
			{
				saveAlgorithm = 3;
			}
			else //scene does not exists
			{
				saveAlgorithm = 4;
			}
		}
		m_savePathName.ReleaseBuffer();

		//Create All Directories
		CChar currentSceneNameWithoutDot[MAX_NAME_SIZE];
		Cpy( currentSceneNameWithoutDot, pureFileName );
		GetWithoutDot( currentSceneNameWithoutDot );

		CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
		Cpy( g_currentVSceneNameWithoutDot, g_currentVSceneName );
		GetWithoutDot( g_currentVSceneNameWithoutDot );

		CChar VScenePath[MAX_NAME_SIZE];
		sprintf( VScenePath, "%s%s", g_VScenePath, currentSceneNameWithoutDot );

		CChar externalScenePath[MAX_NAME_SIZE];
		sprintf( externalScenePath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/External Scenes/" );

		CChar externalSceneTexturesPath[MAX_NAME_SIZE];
		sprintf( externalSceneTexturesPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Textures/" );

		CChar waterTexturesPath[MAX_NAME_SIZE];
		sprintf( waterTexturesPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Waters/" );

		CChar soundPath[MAX_NAME_SIZE];
		sprintf( soundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/" );

		CChar ambientSoundPath[MAX_NAME_SIZE];
		sprintf( ambientSoundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/Ambient/" );

		CChar staticSoundPath[MAX_NAME_SIZE];
		sprintf( staticSoundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/Static/" );

		CChar externalPhysicsPath[MAX_NAME_SIZE];
		sprintf( externalPhysicsPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/External Physics/" );

		CChar skyPath[MAX_NAME_SIZE];
		sprintf( skyPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sky/" );

		CChar bannerPath[MAX_NAME_SIZE];
		sprintf( bannerPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Banner/" );

		CChar scriptPath[MAX_NAME_SIZE];
		sprintf( scriptPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Scripts/" );


		//Directories of g_currentVSceneName
		CChar currentExternalScenePath[MAX_NAME_SIZE];
		sprintf( currentExternalScenePath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/External Scenes/" );

		CChar currentExternalSceneTexturesPath[MAX_NAME_SIZE];
		sprintf( currentExternalSceneTexturesPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Textures/" );

		CChar currentWaterTexturesPath[MAX_NAME_SIZE];
		sprintf( currentWaterTexturesPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Waters/" );

		CChar currentSoundPath[MAX_NAME_SIZE];
		sprintf( currentSoundPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sounds/" );

		CChar currentAmbientSoundPath[MAX_NAME_SIZE];
		sprintf( currentAmbientSoundPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sounds/Ambient/" );

		CChar currentStaticSoundPath[MAX_NAME_SIZE];
		sprintf( currentStaticSoundPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sounds/Static/" );

		CChar currentExternalPhysicsPath[MAX_NAME_SIZE];
		sprintf( currentExternalPhysicsPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/External Physics/" );

		CChar currentSkyPath[MAX_NAME_SIZE];
		sprintf( currentSkyPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sky/" );

		CChar currentBannerPath[MAX_NAME_SIZE];
		sprintf( currentBannerPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Banner/" );

		CChar currentScriptPath[MAX_NAME_SIZE];
		sprintf( currentScriptPath, "%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Scripts/" );


		if( saveAlgorithm == 0 || saveAlgorithm == 3)
		{
			//Remove the contents of existing directory
			RemoveAllFilesInDirectory( externalScenePath );
			RemoveAllFilesInDirectory( externalSceneTexturesPath );
			RemoveAllFilesInDirectory( waterTexturesPath );
			RemoveAllFilesInDirectory( ambientSoundPath );
			RemoveAllFilesInDirectory( staticSoundPath );
			RemoveAllFilesInDirectory( externalPhysicsPath );
			RemoveAllFilesInDirectory( skyPath );
			RemoveAllFilesInDirectory( bannerPath );
			RemoveAllFilesInDirectory( scriptPath );

		}
		else if( saveAlgorithm == 1 || saveAlgorithm == 4)
		{
			//create a new directory based on the save file name
			CreateWindowsDirectory( VScenePath );
			CreateWindowsDirectory( externalScenePath );
			CreateWindowsDirectory( externalSceneTexturesPath );
			CreateWindowsDirectory( waterTexturesPath );
			CreateWindowsDirectory( soundPath );
			CreateWindowsDirectory( ambientSoundPath );
			CreateWindowsDirectory( staticSoundPath );
			CreateWindowsDirectory( externalPhysicsPath );
			CreateWindowsDirectory( skyPath );
			CreateWindowsDirectory( bannerPath );
			CreateWindowsDirectory( scriptPath );
		}
		if( saveAlgorithm == 0 || saveAlgorithm == 1 || saveAlgorithm == 2 )
		{
			//copy the assets from original place to the existing directory
			//banner
			if( g_sceneBanner.IsInVSceneList(pureFileName, CTrue, CTrue ) )
			{
				CopyOneFileToDstDirectory( g_sceneBanner.GetBannerPath(), bannerPath );
			}
			CChar* tempAfterPath = GetAfterPath(g_sceneBanner.GetBannerPath());
			CChar newPathAndName[MAX_NAME_SIZE];
			Cpy(newPathAndName, bannerPath );
			Append(newPathAndName, tempAfterPath );
			g_sceneBanner.SetBannerPath( newPathAndName );

			//external physx
			if( g_multipleView->m_nx->m_hasScene)
			{
				if( g_externalPhysX->IsInVSceneList(pureFileName, CTrue, CTrue ) )
				{
					CopyOneFileToDstDirectory( g_externalPhysX->GetPhysXPath(), externalPhysicsPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_externalPhysX->GetPhysXPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, externalPhysicsPath );
				Append(newPathAndName, tempAfterPath );
				g_externalPhysX->SetPhysXPath( newPathAndName );
			}
			//sky dome
			if( g_skyDome )
			{
				if( g_skyDome->IsInVSceneList(pureFileName, CTrue, CTrue ) )
				{
					CopyOneFileToDstDirectory( g_skyDome->GetPath(), skyPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_skyDome->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, skyPath );
				Append(newPathAndName, tempAfterPath );
				g_skyDome->SetPath( newPathAndName );
			}
			//ambient sound
			if( g_multipleView->m_ambientSound )
			{
				if( g_multipleView->m_ambientSound->IsInVSceneList(pureFileName, CTrue, CTrue ) )
				{
					CopyOneFileToDstDirectory( g_multipleView->m_ambientSound->GetPath(), ambientSoundPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_multipleView->m_ambientSound->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, ambientSoundPath );
				Append(newPathAndName, tempAfterPath );
				g_multipleView->m_ambientSound->SetPath( newPathAndName );
			}
			//3d sounds
			//scenes
			for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
			{
				if( g_engineStaticSounds[i]->IsInVSceneList(pureFileName,CTrue, CTrue ) )
				{
					CopyOneFileToDstDirectory( g_engineStaticSounds[i]->GetPath(), staticSoundPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_engineStaticSounds[i]->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, staticSoundPath );
				Append(newPathAndName, tempAfterPath );
				g_engineStaticSounds[i]->SetPath( newPathAndName );
			}
			//waters
			for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			{
				if( g_engineWaters[i]->IsInVSceneList(pureFileName,CTrue, CTrue ) )
				{
					CopyOneFileToDstDirectory( g_engineWaters[i]->GetDuDvMapName(), waterTexturesPath );
					CopyOneFileToDstDirectory( g_engineWaters[i]->GetNormalMapName(), waterTexturesPath );
				}
				CChar* dudvTempAfterPath = GetAfterPath(g_engineWaters[i]->GetDuDvMapName());
				CChar dudvNewPathAndName[MAX_NAME_SIZE];
				Cpy(dudvNewPathAndName, waterTexturesPath );
				Append(dudvNewPathAndName, dudvTempAfterPath );
				g_engineWaters[i]->SetDuDvMapName( dudvNewPathAndName );

				CChar* normalTempAfterPath = GetAfterPath(g_engineWaters[i]->GetNormalMapName());
				CChar normalNewPathAndName[MAX_NAME_SIZE];
				Cpy(normalNewPathAndName, waterTexturesPath );
				Append(normalNewPathAndName, normalTempAfterPath );
				g_engineWaters[i]->SetNormalMapName( normalNewPathAndName );
			}

			//scenes
			for( CUInt i = 0 ; i < g_scene.size(); i++ )
			{
				if( g_scene[i]->IsInVSceneList(pureFileName,CTrue, CTrue ) )
				{
					if( g_scene[i]->m_compress )
					{
						//Compress dae files////////////////////////////////////////////////
						//zip path
						CChar zipPathTemp[MAX_NAME_SIZE];
						Cpy( zipPathTemp, g_scene[i]->GetFileName() );
						CChar* zipPathPointer = GetAfterPath(zipPathTemp);
						GetWithoutDot( zipPathPointer );
						CChar zipPath[MAX_NAME_SIZE];
						sprintf( zipPath, "%s%s%s", externalScenePath, zipPathPointer, ".zip" );
						//file name inside zip file
						CChar fileNameInZip[MAX_NAME_SIZE];
						Cpy( fileNameInZip, GetAfterPath( g_scene[i]->GetFileName()  ) );

						CChar temp[MAX_NAME_SIZE];
						sprintf( temp, "\n%s %s %s", "Compressing '", fileNameInZip, "' ..." );
						PrintInfo( temp );
						if( Cmp( g_currentPassword, "\n" ) )
							g_scene[i]->WriteZipFile(zipPath, fileNameInZip, g_scene[i]->GetFileName(), NULL );
						else
							g_scene[i]->WriteZipFile(zipPath, fileNameInZip, g_scene[i]->GetFileName(), g_currentPassword );

						 g_scene[i]->m_compress= CFalse;
					}
					//////////////////////////////////////////////////////////////////

					for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
					{
						if( g_scene[i]->m_instanceGeometries[j]->m_hasEnterScript && g_scene[i]->m_instanceGeometries[j]->m_updateEnterScript )
						{
							CopyOneFileToDstDirectory( g_scene[i]->m_instanceGeometries[j]->m_enterScript, scriptPath );
							g_scene[i]->m_instanceGeometries[j]->m_updateEnterScript = CFalse;

							CChar* TempAfterPath = GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_enterScript);
							CChar NewPathAndName[MAX_NAME_SIZE];
							Cpy(NewPathAndName, scriptPath );
							Append(NewPathAndName, TempAfterPath );
							Cpy( g_scene[i]->m_instanceGeometries[j]->m_enterScript, NewPathAndName );
						}
						if( g_scene[i]->m_instanceGeometries[j]->m_hasExitScript && g_scene[i]->m_instanceGeometries[j]->m_updateExitScript )
						{
							CopyOneFileToDstDirectory( g_scene[i]->m_instanceGeometries[j]->m_exitScript, scriptPath );
							g_scene[i]->m_instanceGeometries[j]->m_updateExitScript = CFalse;

							CChar* TempAfterPath = GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_exitScript);
							CChar NewPathAndName[MAX_NAME_SIZE];
							Cpy(NewPathAndName, scriptPath );
							Append(NewPathAndName, TempAfterPath );
							Cpy( g_scene[i]->m_instanceGeometries[j]->m_exitScript, NewPathAndName );

						}
					}

					for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
					{
						CUInt groupSize = CUInt( g_scene[i]->m_geometries[j]->m_groups.size() );
						for( CUInt k = 0; k < groupSize; k++ )
						{
							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasShadowMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateShadowMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalShadowMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateShadowMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasGlossMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateGlossMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalGlossMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateGlossMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasAlphaMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateAlphaMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalAlphaMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateAlphaMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDirtMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDirtMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalDirtMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDirtMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasNormalMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateNormalMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalNormalMapPath, externalSceneTexturesPath);
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateNormalMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDiffuse && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDiffuse)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalDiffusePath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDiffuse = CFalse;
							}
						} //groups
					} //geometries
				}
			} //scenes
		}
		else if( saveAlgorithm == 3 || saveAlgorithm == 4 )
		{
			//copy the assets from current place to the saved directory
			CopyAllFilesFromSrcToDstDirectory(currentExternalScenePath, externalScenePath);
			CopyAllFilesFromSrcToDstDirectory(currentExternalSceneTexturesPath, externalSceneTexturesPath);
			CopyAllFilesFromSrcToDstDirectory(currentWaterTexturesPath, waterTexturesPath);
			CopyAllFilesFromSrcToDstDirectory(currentAmbientSoundPath, ambientSoundPath);
			CopyAllFilesFromSrcToDstDirectory(currentStaticSoundPath, staticSoundPath);
			CopyAllFilesFromSrcToDstDirectory(currentExternalPhysicsPath, externalPhysicsPath);
			CopyAllFilesFromSrcToDstDirectory(currentSkyPath, skyPath);
			CopyAllFilesFromSrcToDstDirectory(currentBannerPath, bannerPath);
			CopyAllFilesFromSrcToDstDirectory( currentScriptPath, scriptPath );

			//Banner
			if( g_sceneBanner.IsInVSceneList(g_currentVSceneName, CTrue, CFalse ) )
			{
				CopyOneFileToDstDirectory( g_sceneBanner.GetBannerPath(), bannerPath );
			}
			CChar* tempAfterPath = GetAfterPath(g_sceneBanner.GetBannerPath());
			CChar newPathAndName[MAX_NAME_SIZE];
			Cpy(newPathAndName, bannerPath );
			Append(newPathAndName, tempAfterPath );
			g_sceneBanner.SetBannerPath( newPathAndName );
			g_sceneBanner.IsInVSceneList(pureFileName,CTrue, CTrue );

			//external physx
			if( g_multipleView->m_nx->m_hasScene)
			{
				if( g_externalPhysX->IsInVSceneList(g_currentVSceneName, CTrue, CFalse ) )
				{
					CopyOneFileToDstDirectory( g_externalPhysX->GetPhysXPath(), externalPhysicsPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_externalPhysX->GetPhysXPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, externalPhysicsPath );
				Append(newPathAndName, tempAfterPath );
				g_externalPhysX->SetPhysXPath( newPathAndName );
				g_externalPhysX->IsInVSceneList(pureFileName,CTrue, CTrue );

			}

			//sky
			if( g_skyDome )
			{
				if( g_skyDome->IsInVSceneList(g_currentVSceneName, CTrue, CFalse ) )
				{
					CopyOneFileToDstDirectory( g_skyDome->GetPath(), skyPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_skyDome->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, skyPath );
				Append(newPathAndName, tempAfterPath );
				g_skyDome->SetPath( newPathAndName );

				g_skyDome->IsInVSceneList(pureFileName,CTrue, CTrue );
			}
			//ambient sound
			if( g_multipleView->m_ambientSound )
			{
				if( g_multipleView->m_ambientSound->IsInVSceneList(g_currentVSceneName, CTrue, CFalse ) )
				{
					CopyOneFileToDstDirectory( g_multipleView->m_ambientSound->GetPath(), ambientSoundPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_multipleView->m_ambientSound->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, ambientSoundPath );
				Append(newPathAndName, tempAfterPath );
				g_multipleView->m_ambientSound->SetPath( newPathAndName );

				g_multipleView->m_ambientSound->IsInVSceneList(pureFileName,CTrue, CTrue );
			}
			//3d sounds
			for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
			{
				if( g_engineStaticSounds[i]->IsInVSceneList(g_currentVSceneName,CTrue, CFalse ) )
				{
					CopyOneFileToDstDirectory( g_engineStaticSounds[i]->GetPath(), staticSoundPath );
				}
				CChar* tempAfterPath = GetAfterPath(g_engineStaticSounds[i]->GetPath());
				CChar newPathAndName[MAX_NAME_SIZE];
				Cpy(newPathAndName, staticSoundPath );
				Append(newPathAndName, tempAfterPath );
				g_engineStaticSounds[i]->SetPath( newPathAndName );

				g_engineStaticSounds[i]->IsInVSceneList(pureFileName,CTrue, CTrue );

			}

			//waters
			for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			{
				if( g_engineWaters[i]->IsInVSceneList(g_currentVSceneName,CTrue, CFalse ) )
				{
					CopyOneFileToDstDirectory( g_engineWaters[i]->GetDuDvMapName(), waterTexturesPath );
					CopyOneFileToDstDirectory( g_engineWaters[i]->GetNormalMapName(), waterTexturesPath );
				}
				CChar* dudvTempAfterPath = GetAfterPath(g_engineWaters[i]->GetDuDvMapName());
				CChar dudvNewPathAndName[MAX_NAME_SIZE];
				Cpy(dudvNewPathAndName, waterTexturesPath );
				Append(dudvNewPathAndName, dudvTempAfterPath );
				g_engineWaters[i]->SetDuDvMapName( dudvNewPathAndName );

				CChar* normalTempAfterPath = GetAfterPath(g_engineWaters[i]->GetNormalMapName());
				CChar normalNewPathAndName[MAX_NAME_SIZE];
				Cpy(normalNewPathAndName, waterTexturesPath );
				Append(normalNewPathAndName, normalTempAfterPath );
				g_engineWaters[i]->SetNormalMapName( normalNewPathAndName );

				g_engineWaters[i]->IsInVSceneList(pureFileName,CTrue, CTrue );
			}

			//scenes
			for( CUInt i = 0 ; i < g_scene.size(); i++ )
			{
				if( g_scene[i]->IsInVSceneList(g_currentVSceneName,CTrue, CFalse ) )
				{
					if( g_scene[i]->m_compress )
					{
						//Compress dae files////////////////////////////////////////////////
						//zip path
						CChar zipPathTemp[MAX_NAME_SIZE];
						Cpy( zipPathTemp, g_scene[i]->GetFileName() );
						CChar* zipPathPointer = GetAfterPath(zipPathTemp);
						GetWithoutDot( zipPathPointer );
						CChar zipPath[MAX_NAME_SIZE];
						sprintf( zipPath, "%s%s%s", externalScenePath, zipPathPointer, ".zip" );
						//file name inside zip file
						CChar fileNameInZip[MAX_NAME_SIZE];
						Cpy( fileNameInZip, GetAfterPath( g_scene[i]->GetFileName()  ) );

						CChar temp[MAX_NAME_SIZE];
						sprintf( temp, "\n%s %s %s", "Compressing '", fileNameInZip, "' ..." );
						PrintInfo( temp );
						if( Cmp( g_currentPassword, "\n" ) )
							g_scene[i]->WriteZipFile(zipPath, fileNameInZip, g_scene[i]->GetFileName(), NULL );
						else
							g_scene[i]->WriteZipFile(zipPath, fileNameInZip, g_scene[i]->GetFileName(), g_currentPassword );
						g_scene[i]->m_compress = CFalse;
					}
					//////////////////////////////////////////////////////////////////

					for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
					{
						if( g_scene[i]->m_instanceGeometries[j]->m_hasEnterScript && g_scene[i]->m_instanceGeometries[j]->m_updateEnterScript )
						{
							CopyOneFileToDstDirectory( g_scene[i]->m_instanceGeometries[j]->m_enterScript, scriptPath );
							g_scene[i]->m_instanceGeometries[j]->m_updateEnterScript = CFalse;

							CChar* TempAfterPath = GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_enterScript);
							CChar NewPathAndName[MAX_NAME_SIZE];
							Cpy(NewPathAndName, scriptPath );
							Append(NewPathAndName, TempAfterPath );
							Cpy( g_scene[i]->m_instanceGeometries[j]->m_enterScript, NewPathAndName );

						}
						if( g_scene[i]->m_instanceGeometries[j]->m_hasExitScript && g_scene[i]->m_instanceGeometries[j]->m_updateExitScript )
						{
							CopyOneFileToDstDirectory( g_scene[i]->m_instanceGeometries[j]->m_exitScript, scriptPath );
							g_scene[i]->m_instanceGeometries[j]->m_updateExitScript = CFalse;

							CChar* TempAfterPath = GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_exitScript);
							CChar NewPathAndName[MAX_NAME_SIZE];
							Cpy(NewPathAndName, scriptPath );
							Append(NewPathAndName, TempAfterPath );
							Cpy( g_scene[i]->m_instanceGeometries[j]->m_exitScript, NewPathAndName );

						}
					}

					for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
					{
						CUInt groupSize = CUInt( g_scene[i]->m_geometries[j]->m_groups.size() );
						for( CUInt k = 0; k < groupSize; k++ )
						{
							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasShadowMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateShadowMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalShadowMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateShadowMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasGlossMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateGlossMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalGlossMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateGlossMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasAlphaMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateAlphaMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalAlphaMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateAlphaMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDirtMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDirtMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalDirtMapPath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDirtMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasNormalMap && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateNormalMap)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalNormalMapPath, externalSceneTexturesPath);
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateNormalMap = CFalse;
							}

							if( g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDiffuse && g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDiffuse)
							{
								CopyOneFileToDstDirectory( g_scene[i]->m_geometries[j]->m_groups[k]->m_strOriginalDiffusePath, externalSceneTexturesPath );
								g_scene[i]->m_geometries[j]->m_groups[k]->m_updateDiffuse = CFalse;
							}
						} //groups
					} //geometries
				}
				g_scene[i]->IsInVSceneList(pureFileName,CTrue, CTrue );
			} //scenes
		}
		HANDLE hFind;
		WIN32_FIND_DATA data;

		//Delete removed banners
		CChar bannerTempPath[MAX_NAME_SIZE];
		sprintf( bannerTempPath, "%s%s", bannerPath, "*.*" );
		hFind = FindFirstFile( bannerTempPath, &data );
		do
		{
			CChar bannerTempPath[MAX_NAME_SIZE];
			sprintf( bannerTempPath, "%s%s", bannerPath, data.cFileName );
			CBool foundTarget = CFalse;
			if( Cmp( GetAfterPath( g_sceneBanner.GetBannerPath()), data.cFileName ) )
			{
				foundTarget = CTrue;
			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( bannerTempPath) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Delete removed external physx
		if( g_multipleView->m_nx->m_hasScene )
		{
			CChar PhysXTempPath[MAX_NAME_SIZE];
			sprintf( PhysXTempPath, "%s%s", externalPhysicsPath, "*.*" );
			hFind = FindFirstFile( PhysXTempPath, &data );
			do
			{
				CChar PhysXTempPath[MAX_NAME_SIZE];
				sprintf( PhysXTempPath, "%s%s", externalPhysicsPath, data.cFileName );
				CBool foundTarget = CFalse;
				if( Cmp( GetAfterPath( g_externalPhysX->GetPhysXPath()), data.cFileName ) )
				{
					foundTarget = CTrue;
				}

				//Remove Files
				if( !foundTarget )
				{
					if( !DeleteFile( PhysXTempPath) )
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
						//PrintInfo( temp, COLOR_RED );
					}
				}
			}while (FindNextFile( hFind, &data));
			FindClose(hFind);
		}
		else
		{
			RemoveAllFilesInDirectory( externalPhysicsPath );
		}

		//Delete removed sky textures
		if( g_skyDome )
		{
			CChar skyTempPath[MAX_NAME_SIZE];
			sprintf( skyTempPath, "%s%s", skyPath, "*.*" );
			hFind = FindFirstFile( skyTempPath, &data );
			do
			{
				CChar skyTempPath[MAX_NAME_SIZE];
				sprintf( skyTempPath, "%s%s", skyPath, data.cFileName );
				CBool foundTarget = CFalse;
				if( Cmp( GetAfterPath( g_skyDome->GetPath()), data.cFileName ) )
				{
					foundTarget = CTrue;
				}

				//Remove Files
				if( !foundTarget )
				{
					if( !DeleteFile( skyTempPath) )
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
						//PrintInfo( temp, COLOR_RED );
					}
				}
			}while (FindNextFile( hFind, &data));
			FindClose(hFind);
		}
		else
		{
			RemoveAllFilesInDirectory( skyPath );
		}


		//delete removed ambient sounds
		if( g_multipleView->m_ambientSound )
		{
			CChar ambientSoundTempPath[MAX_NAME_SIZE];
			sprintf( ambientSoundTempPath, "%s%s", ambientSoundPath, "*.*" );
			hFind = FindFirstFile( ambientSoundTempPath, &data );
			do
			{
				CChar soundTempPath[MAX_NAME_SIZE];
				sprintf( soundTempPath, "%s%s", ambientSoundPath, data.cFileName );

				CBool foundTarget = CFalse;
				if( Cmp( GetAfterPath( g_multipleView->m_ambientSound->GetPath()), data.cFileName ) )
				{
					foundTarget = CTrue;
				}

				//Remove Files
				if( !foundTarget )
				{
					if( !DeleteFile( soundTempPath) )
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
						//PrintInfo( temp, COLOR_RED );
					}
				}
			}while (FindNextFile( hFind, &data));
			FindClose(hFind);
		}
		else
		{
			RemoveAllFilesInDirectory( ambientSoundPath );
		}


		//Delete removed static sounds
		CChar tempStaticSoundPath[MAX_NAME_SIZE];
		sprintf( tempStaticSoundPath, "%s%s", staticSoundPath, "*.*" );
		hFind = FindFirstFile( tempStaticSoundPath, &data );
		do
		{
			CChar soundTempPath[MAX_NAME_SIZE];
			sprintf( soundTempPath, "%s%s", staticSoundPath, data.cFileName );

			CBool foundTarget = CFalse;
			for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
			{
				if( Cmp( GetAfterPath( g_engineStaticSounds[i]->GetPath()), data.cFileName ) )
				{
					foundTarget = CTrue;
					break;
				}
			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( soundTempPath) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Delete removed water textures
		CChar tempWaterPath[MAX_NAME_SIZE];
		sprintf( tempWaterPath, "%s%s", waterTexturesPath, "*.*" );
		hFind = FindFirstFile( tempWaterPath, &data );
		do
		{
			CChar tempWaterPath[MAX_NAME_SIZE];
			sprintf( tempWaterPath, "%s%s", waterTexturesPath, data.cFileName );

			CBool foundTarget = CFalse;
			for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			{
				if( Cmp( GetAfterPath( g_engineWaters[i]->GetDuDvMapName()), data.cFileName ) )
				{
					foundTarget = CTrue;
					break;
				}
				if( Cmp( GetAfterPath( g_engineWaters[i]->GetNormalMapName()), data.cFileName ) )
				{
					foundTarget = CTrue;
					break;
				}

			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( tempWaterPath) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Delete removed scenes : Zip files
		CChar path[MAX_NAME_SIZE];
		sprintf( path, "%s%s", externalScenePath, "*.*" );
		hFind = FindFirstFile( path, &data );
		do
		{
			CChar filePath[MAX_NAME_SIZE];
			sprintf( filePath, "%s%s", externalScenePath, data.cFileName );
			CBool foundTarget = CFalse;
			for( CUInt i = 0 ; i < g_scene.size(); i++ )
			{
				CChar sceneWithoutDot[MAX_NAME_SIZE];
				Cpy( sceneWithoutDot, g_scene[i]->GetName() );
				GetWithoutDot( sceneWithoutDot );
				Append( sceneWithoutDot, ".zip" );
				if( Cmp( sceneWithoutDot, data.cFileName ) )
				{
					foundTarget = CTrue;
					break;
				}
			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( filePath) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Delete removed scenes : scripts
		CChar tempScriptPath[MAX_NAME_SIZE];
		sprintf( tempScriptPath, "%s%s", scriptPath, "*.*" );
		hFind = FindFirstFile( tempScriptPath, &data );
		do
		{
			CChar scriptTempPath[MAX_NAME_SIZE];
			sprintf( scriptTempPath, "%s%s", scriptPath, data.cFileName );

			CBool foundTarget = CFalse;
			for( CUInt i = 0 ; i <g_scene.size(); i++ )
			{
				for( CUInt j = 0 ; j <g_scene[i]->m_instanceGeometries.size(); j++ )
				{
					if( g_scene[i]->m_instanceGeometries[j]->m_hasEnterScript && Cmp( GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_enterScript), data.cFileName ) )
					{
						foundTarget = CTrue;
						break;
					}
					if( g_scene[i]->m_instanceGeometries[j]->m_hasExitScript && Cmp( GetAfterPath( g_scene[i]->m_instanceGeometries[j]->m_exitScript), data.cFileName ) )
					{
						foundTarget = CTrue;
						break;
					}

				}
			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( scriptTempPath) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Delete removed scenes : Unshared image files
		CChar path2[MAX_NAME_SIZE];
		sprintf( path2, "%s%s", externalSceneTexturesPath, "*.*" );
		hFind = FindFirstFile( path2, &data );
		do
		{
			CChar filePath2[MAX_NAME_SIZE];
			sprintf( filePath2, "%s%s", externalSceneTexturesPath, data.cFileName );
			CBool foundTarget = CFalse;
			for( CUInt i = 0 ; i < g_images.size(); i++ )
			{
				if( Cmp( GetAfterPath( g_images[i]->m_fileName), data.cFileName ) )
				{
					foundTarget = CTrue;
					break;
				}
			}

			//Remove Files
			if( !foundTarget )
			{
				if( !DeleteFile( filePath2) )
				{
					//CChar temp[MAX_NAME_SIZE];
					//sprintf( temp, "\n%s%s", "Error: Couldn't remove the file ", data.cFileName );
					//PrintInfo( temp, COLOR_RED );
				}
			}
		}while (FindNextFile( hFind, &data));
		FindClose(hFind);

		//Mark Current VScene 
		Cpy( g_currentVSceneName, pureFileName );


		//Save functions////////////////////////////////////

		FILE *filePtr;
		filePtr = fopen( m_strpathName, "wb" );
		if(!filePtr )
		{
			//fclose( filePtr );
			CChar temp[MAX_NAME_SIZE];
			sprintf( temp, "%s%s%s", "Couldn't open the file '", m_strpathName, "' to save data" );
			MessageBox( temp , "Vanda Engine Error", MB_OK );
			return;
		}
		//Save version
		CChar engineName[MAX_NAME_SIZE] = "VandaEngine";
		fwrite( &engineName, sizeof( CChar), MAX_NAME_SIZE, filePtr );
		fwrite(  &g_edition, sizeof( CChar), MAX_NAME_SIZE, filePtr );
		fwrite( &g_maxVersion, 1, sizeof( CInt ), filePtr );
		fwrite( &g_minVersion, 1, sizeof( CInt ), filePtr );
		fwrite( &g_bugFixesVersion, 1, sizeof( CInt ), filePtr );
		fwrite(  &g_currentPassword, sizeof( CChar), MAX_NAME_SIZE, filePtr );
		//save engine options

		fwrite( &g_shadowProperties, sizeof( CShadowProperties ), 1, filePtr  );
		fwrite( &g_dofProperties, sizeof( CDOFProperties ), 1, filePtr  );
		fwrite( &g_fogProperties, sizeof( CFogProperties ), 1, filePtr  );
		fwrite( &g_bloomProperties, sizeof( CBloomProperties ), 1, filePtr  );
		fwrite( &g_lightProperties, sizeof( CLightProperties ), 1, filePtr  );
		fwrite( &g_pathProperties, sizeof( CPathProperties ), 1, filePtr  );
		fwrite( &g_vandaDemo, sizeof(CBool), 1, filePtr);
		fwrite(g_sceneBanner.GetBannerPath(), sizeof(CChar), MAX_NAME_SIZE, filePtr );
		fwrite( &g_extraTexturesNamingConventions, sizeof( CExtraTexturesNamingConventions ), 1, filePtr  );
		fwrite( &g_useGlobalAmbientColor, sizeof( CBool ), 1, filePtr  );
		fwrite( &g_globalAmbientColor, sizeof( CColor4f ), 1, filePtr  );
		fwrite( &g_sceneManagerObjectsPerSplit, sizeof( CInt), 1, filePtr );
		//save physx data
		fwrite( &g_multipleView->m_nx->m_hasScene, sizeof( CBool ), 1, filePtr  );
		fwrite( g_multipleView->m_nx->m_sceneName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
		fwrite( &g_characterPos, sizeof( NxExtendedVec3 ), 1, filePtr );
		fwrite( &g_cameraPitchYawTilt, sizeof( CVec3f ), 1, filePtr );
		
		CInt tempSceneSize = (CInt)g_scene.size();
		CInt tempGeoSize;
		fwrite( &tempSceneSize, 1, sizeof( CInt ), filePtr );
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			//CChar path[MAX_NAME_SIZE];
			//CChar file[MAX_NAME_SIZE];
			//Cpy( file, g_scene[i]->m_pureFileName );
			//GetWithoutDot(file);
			//sprintf( path, "Assets/meshes/%s/%s", file, g_scene[i]->m_pureFileName );

			fwrite( g_scene[i]->GetFileName(), 1, sizeof( CChar ) * MAX_NAME_SIZE, filePtr );
			CInt clipIndex = g_scene[i]->GetClipIndexForStartup();
			fwrite( &clipIndex, 1, sizeof(CInt), filePtr );
			fwrite( &g_scene[i]->m_playAnimation, 1, sizeof( CBool ), filePtr );
			fwrite( &g_scene[i]->m_loopAnimationAtStartup, 1, sizeof( CBool ), filePtr );

			tempGeoSize = (CInt)g_scene[i]->m_geometries.size();
			fwrite( &tempGeoSize, 1, sizeof( CInt ), filePtr );

			/////////////////////////////////////
			for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
			{
				fwrite( &g_scene[i]->m_geometries[j]->m_hasShadowMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strShadowMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasDirtMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strDirtMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasAlphaMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strAlphaMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasNormalMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_parallaxMapBias, sizeof( CFloat ), 1, filePtr  );
				fwrite( &g_scene[i]->m_geometries[j]->m_parallaxMapScale, sizeof( CFloat ), 1, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasGlossMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strGlossMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasHeightMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strHeightMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasDuDvMap, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strDuDvMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fwrite( &g_scene[i]->m_geometries[j]->m_hasDiffuse, sizeof( CBool ), 1, filePtr  );
				fwrite( g_scene[i]->m_geometries[j]->m_strDiffuse, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				//save cull face( enabled or disabled data) for the current geo
				fwrite( &g_scene[i]->m_geometries[j]->m_cullFaces, sizeof( CBool ), 1, filePtr );
				
				//write PhysX data
				CUInt physx_point_size =  (CUInt)g_scene[i]->m_geometries[j]->m_physx_points.size();
				fwrite( &physx_point_size, sizeof(CUInt), 1, filePtr );
				for( CUInt m = 0; m < g_scene[i]->m_geometries[j]->m_physx_points.size(); m++ )
				{
					fwrite( g_scene[i]->m_geometries[j]->m_physx_points[m] , sizeof( CVec3f ), 1, filePtr );
				}

				CUInt physx_triangles_size = (CUInt)g_scene[i]->m_geometries[j]->m_physx_triangles.size();
				fwrite( &physx_triangles_size, sizeof(CUInt), 1, filePtr );
				for( CUInt m = 0; m <  g_scene[i]->m_geometries[j]->m_physx_triangles.size(); m++ )
				{
					fwrite( &g_scene[i]->m_geometries[j]->m_physx_triangles[m]->m_count, sizeof(CUInt), 1, filePtr );
					for( CUInt n = 0; n <g_scene[i]->m_geometries[j]->m_physx_triangles[m]->m_count * 3; n++ )
					{
						fwrite( &g_scene[i]->m_geometries[j]->m_physx_triangles[m]->m_indexes[n], sizeof( CUInt ), 1, filePtr );
					}
				}
				///////////////////

				CUInt groupSize = CUInt( g_scene[i]->m_geometries[j]->m_groups.size() );
				fwrite( &groupSize, sizeof( CUInt ), 1, filePtr );
				//store group info

				for( CUInt k = 0; k < groupSize; k++ )
				{
					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasShadowMap, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strShadowMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasGlossMap, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strGlossMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasAlphaMap, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strAlphaMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDirtMap, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strDirtMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasNormalMap, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_parallaxMapBias, sizeof( CFloat ), 1, filePtr  );
					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_parallaxMapScale, sizeof( CFloat ), 1, filePtr  );

					fwrite( &g_scene[i]->m_geometries[j]->m_groups[k]->m_hasDiffuse, sizeof( CBool ), 1, filePtr  );
					fwrite( g_scene[i]->m_geometries[j]->m_groups[k]->m_strDiffuse, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				}
			}// for all of the geos

			//write the instance geometries
			CUInt size = (CUInt)g_scene[i]->m_instanceGeometries.size();
			fwrite(  &size, 1, sizeof( CUInt), filePtr );
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				fwrite( g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->GetName(), 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
				fwrite( g_scene[i]->m_instanceGeometries[j]->m_firstLocalToWorldMatrix, 1, sizeof( CMatrix ), filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_hasPhysX, 1, sizeof( CBool ), filePtr );
				fwrite( g_scene[i]->m_instanceGeometries[j]->m_physXName, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr ); 
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_lodAlgorithm, 1, sizeof (CPhysXAlgorithm), filePtr ); 
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_physXDensity, 1, sizeof( CFloat ), filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_physXPercentage, 1, sizeof( CInt ), filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_isTrigger, 1, sizeof( CBool ), filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_enterScript, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_hasEnterScript, 1, sizeof( CBool ), filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_exitScript, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
				fwrite( &g_scene[i]->m_instanceGeometries[j]->m_hasExitScript, 1, sizeof( CBool ), filePtr );

			}
		} //for all of the scenes


		fwrite( &g_menu.m_insertAndShowSky, sizeof( CBool ), 1, filePtr );
		if( g_menu.m_insertAndShowSky )
		{
			fwrite( g_skyDome->m_name, sizeof( CChar ), MAX_NAME_SIZE, filePtr );
			fwrite( g_skyDome->m_path, sizeof( CChar ), MAX_NAME_SIZE, filePtr );
			fwrite( &g_skyDome->m_numSlices, sizeof( CInt ), 1, filePtr );
			fwrite( &g_skyDome->m_numSides, sizeof( CInt ), 1, filePtr );
			fwrite( &g_skyDome->m_radius, sizeof( CFloat ), 1, filePtr );
			fwrite( g_skyDome->m_position, sizeof( CFloat ), 3, filePtr );
			fwrite( &g_skyDome->m_dampening, sizeof( CFloat ), 1, filePtr );
			fwrite( &g_skyDome->m_exponential, sizeof( CBool ), 1, filePtr );
		}
		//save all the waters 
		CInt tempWaterCount = (CInt)g_engineWaters.size();
		fwrite( &tempWaterCount, sizeof( CInt ), 1, filePtr );
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			fwrite( &g_engineWaters[i]->m_strWaterName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( &g_engineWaters[i]->m_strNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( &g_engineWaters[i]->m_strDuDvMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( g_engineWaters[i]->m_fWaterCPos, sizeof( CFloat ), 3, filePtr );
			fwrite( g_engineWaters[i]->m_fWaterLPos, sizeof( CFloat ), 3, filePtr );
			fwrite( &g_engineWaters[i]->m_fWaterHeight, sizeof( CFloat ), 1, filePtr );
			fwrite( &g_engineWaters[i]->m_fWaterSpeed, sizeof( CFloat ), 1, filePtr );
			fwrite( &g_engineWaters[i]->m_fWaterScale, sizeof( CFloat ), 1, filePtr );
			fwrite( &g_engineWaters[i]->m_fWaterUV, sizeof( CFloat ), 1, filePtr );
			CInt tempGeoCount = (CInt)g_engineWaters[i]->m_geometry.size();
			fwrite( &tempGeoCount, sizeof( CInt ), 1, filePtr );
			for( CUInt j = 0; j < g_engineWaters[i]->m_geometry.size(); j++ )
				fwrite( g_engineWaters[i]->m_geometry[j]->GetName(), sizeof( CChar ), MAX_NAME_SIZE, filePtr );
		}

		//save engine lights
		CInt tempLightCount = (CInt)g_engineLights.size();
		fwrite( &tempLightCount, sizeof( CInt), 1, filePtr );

		for( CUInt i = 0; i < g_engineLights.size(); i++ )
		{
			fwrite( g_engineLights[i]->m_abstractLight->GetName(), sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( g_engineLights[i]->m_abstractLight->GetPosition(), sizeof( CFloat ), 4, filePtr  );
			fwrite( &g_engineLights[i]->m_abstractLight->m_constantAttenuation, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineLights[i]->m_abstractLight->m_linearAttenuation, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineLights[i]->m_abstractLight->m_quadraticAttenuation, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineLights[i]->m_abstractLight->m_shininess, sizeof( CFloat ), 1, filePtr  );
			fwrite( g_engineLights[i]->m_abstractLight->GetAmbient(), sizeof( CFloat ), 4, filePtr  );
			fwrite( g_engineLights[i]->m_abstractLight->GetDiffuse(), sizeof( CFloat ), 4, filePtr  );
			fwrite( g_engineLights[i]->m_abstractLight->GetSpecular(), sizeof( CFloat ), 4, filePtr  );
			fwrite( &g_engineLights[i]->m_abstractLight->m_lightType, sizeof( CLightType ), 1, filePtr  );
			if( g_engineLights[i]->m_abstractLight->m_lightType == eLIGHTTYPE_SPOT)
			{
				fwrite( &g_engineLights[i]->m_abstractLight->m_spotCutoff, sizeof( CFloat ), 1, filePtr  );
				fwrite( g_engineLights[i]->m_abstractLight->GetSpotDirection(), sizeof( CFloat ), 4, filePtr  );
				fwrite( &g_engineLights[i]->m_abstractLight->m_spotExponent, sizeof( CFloat ), 1, filePtr  );
			}
		}
		//save static sounds data
		CInt tempStaticSoundCount = (CInt)g_engineStaticSounds.size();
		fwrite( &tempStaticSoundCount, sizeof( CInt ), 1, filePtr );

		for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
		{
			fwrite( &g_engineStaticSounds[i]->m_name, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_path, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_loop, sizeof( CBool ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundMaxDistance, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundPitch, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_play, sizeof( CBool ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundPos, sizeof( CFloat ), 3, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundReferenceDistance, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundRolloff, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_engineStaticSounds[i]->m_fStaticSoundVolume, sizeof( CFloat ), 1, filePtr  );
		}
		//Save ambient sound
		fwrite( &g_menu.m_insertAmbientSound, sizeof( CBool ), 1, filePtr  );
		if( g_menu.m_insertAmbientSound )
		{
			fwrite( g_multipleView->m_ambientSound->GetName(), sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( g_multipleView->m_ambientSound->GetPath(), sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fwrite( &g_multipleView->m_ambientSound->m_volume, sizeof( CFloat ), 1, filePtr  );
			fwrite( &g_multipleView->m_ambientSound->m_pitch, sizeof( CFloat ), 1, filePtr  );
		}
		fclose( filePtr );

		//save functions////////////////////////////////////////

		CBool foundTarget = CFalse;
		CChar* pureFileName2 =  (CChar*)GetAfterPath(m_strpathName.GetBuffer( m_strpathName.GetLength()));
		for( CUInt vs = 0; vs < g_allVSceneNames.size(); vs++ )
		{
			if (Cmp(pureFileName2, g_allVSceneNames[vs].c_str() ) )
			{
				foundTarget = CTrue;
				break;
			}
		}

		if( !foundTarget )
		{
			g_allVSceneNames.push_back( GetAfterPath(pureFileName) );
		}
		m_strpathName.ReleaseBuffer();

		FILE *VScenesFilePtr;
		CInt numScenes = CInt(g_allVSceneNames.size());
		CChar DATPath[MAX_NAME_SIZE];
		sprintf( DATPath, "%s%s", g_VScenePath, "vscenes.dat" );

		DeleteFile( DATPath );
		VScenesFilePtr =  fopen( DATPath, "wb" );
		if( !VScenesFilePtr )
		{
			MessageBox( "Couldn't open 'assets/vscenes/vscenes.dat' to save data!", "Vanda Engine Error", MB_OK | MB_ICONERROR);
			//return;
		}
		fwrite( &numScenes, sizeof( CInt ), 1, VScenesFilePtr  );

		for( CInt i = 0; i < numScenes; i++ )
		{
			CChar tempVSceneName[MAX_NAME_SIZE];
			Cpy( tempVSceneName, g_allVSceneNames[i].c_str() );
			fwrite( tempVSceneName, sizeof( CChar ), MAX_NAME_SIZE, VScenesFilePtr );
		}
		fclose( VScenesFilePtr );
		//save functions///////////////////////////////////
		

		//copy the saved scene from its source to destination
		CChar* sourcePath = (CChar*)(m_strpathName.GetBuffer( m_strpathName.GetLength()));
		CChar dstPath[MAX_NAME_SIZE];
		sprintf( dstPath, "%s%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "\\", GetAfterPath( g_currentVSceneName ) );
		CopyFile( sourcePath, dstPath, NULL );
		m_strpathName.ReleaseBuffer();

		PrintInfo( "\nScene Saved Successfully" );
		ReleaseCapture();
	}
	else if( result == IDCANCEL )
		return;
}

CBool CVandaEngine1Dlg::OnMenuClickedOpen(CBool askQuestion)
{
	CInt iResponse = IDNO;
	if( g_scene.size() > 0 || g_engineLights.size() > 0 || g_engineWaters.size() > 0 || g_menu.m_insertAndShowSky || g_menu.m_insertAmbientSound || g_multipleView->m_nx->m_hasScene )
		iResponse= MessageBox( "Do you want to save your changes?", "Warning" , MB_YESNOCANCEL|MB_ICONSTOP);

	CBool openScene = CFalse;
	if( iResponse == IDYES )
	{
		openScene = CTrue;
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );
		OnMenuClickedSaveAs();
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
	}

	if ( iResponse == IDNO || openScene )
	{
		CString m_strpathName;
		INT_PTR result;
		if( askQuestion )
		{
			// Create an Open dialog; the default file name extension is ".vin".
			CFileDialog dlgOpen(TRUE, _T("*.vin"), _T(""), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
				_T(" File (*.vin)|*.vin||"), NULL, NULL);
			result = dlgOpen.DoModal();

			m_strpathName = (CString)dlgOpen.GetPathName();
		}
		else
		{
			result = IDOK;
			m_strpathName = g_fileNameInCommandLine;
		}


		if (result == IDOK) //open a new scene
		{
			CChar reportTemp[MAX_NAME_SIZE];
			CChar pathName[MAX_NAME_SIZE];
			Cpy( pathName, m_strpathName.GetString() );
			sprintf( reportTemp, "VIN file '%s'...", GetAfterPath( pathName ) );
			PrintInfo2( reportTemp, COLOR_RED_GREEN );

			SetCapture();
			SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );
			g_octree->ResetState();
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CFalse );
			OnMenuClickedNew( CFalse ); //do not ask question
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CTrue );
			g_importColladaImages = CFalse;

			Cpy( g_currentVSceneName, (CChar*)GetAfterPath(m_strpathName.GetBuffer( m_strpathName.GetLength())) ); //For save functions
			m_strpathName.ReleaseBuffer();  //for save functions

			FILE *filePtr;
			filePtr = fopen( m_strpathName, "rb" );
			if(!filePtr )
			{
				fclose( filePtr );
				MessageBox( "Couldn't open the file to load data", "Vanda Engine Error", MB_OK );
				return CFalse;
			}
			CChar engineName[MAX_NAME_SIZE];
			fread( &engineName, sizeof( CChar), MAX_NAME_SIZE, filePtr );
			if( !CmpIn( engineName, "VandaEngine" ) )
			{
				fclose( filePtr );
				MessageBox( "Invalid Vin file!", "Vanda Engine Error", MB_OK | MB_ICONERROR );
				return CFalse;
			}
			fread(  &g_edition, sizeof( CChar), MAX_NAME_SIZE, filePtr );
			fread( &g_maxVersion, 1, sizeof( CInt ), filePtr );
			fread( &g_minVersion, 1, sizeof( CInt ), filePtr );
			fread( &g_bugFixesVersion, 1, sizeof( CInt ), filePtr );
			fread(  &g_currentPassword, sizeof( CChar), MAX_NAME_SIZE, filePtr );

			//read engine options
			fread( &g_shadowProperties, sizeof( CShadowProperties ), 1, filePtr  );
			switch( g_shadowProperties.m_shadowResolution )
			{
			case eSHADOW_1024:
				g_dynamicShadowMap->depth_size = 1024;
					break;
			case eSHADOW_2048:
				g_dynamicShadowMap->depth_size = 2048;
				break;
			case eSHADOW_4096:
				g_dynamicShadowMap->depth_size = 4096;
				break;
			default:
				break;
			}
			g_dynamicShadowMap->RegenerateDepthTex( g_dynamicShadowMap->depth_size );

			switch( g_shadowProperties.m_shadowSplits )
			{
			case eSHADOW_1_SPLIT:
				g_dynamicShadowMap->cur_num_splits = 1;
				break;
			case eSHADOW_2_SPLITS:
				g_dynamicShadowMap->cur_num_splits = 2;
				break;
			case eSHADOW_3_SPLITS:
				g_dynamicShadowMap->cur_num_splits = 3;
				break;
			case eSHADOW_4_SPLITS:
				g_dynamicShadowMap->cur_num_splits = 4;
				break;
			default:
				break;
			}

			g_dynamicShadowMap->split_weight = g_shadowProperties.m_shadowSplitWeight;

			fread( &g_dofProperties, sizeof( CDOFProperties ), 1, filePtr  );
			fread( &g_fogProperties, sizeof( CFogProperties ), 1, filePtr  );
			fread( &g_bloomProperties, sizeof( CBloomProperties ), 1, filePtr  );
			fread( &g_lightProperties, sizeof( CLightProperties ), 1, filePtr  );
			fread( &g_pathProperties, sizeof( CPathProperties ), 1, filePtr  );
			fread( &g_vandaDemo, sizeof(CBool), 1, filePtr);

			CChar banner[MAX_NAME_SIZE];
			fread(&banner, sizeof(CChar), MAX_NAME_SIZE, filePtr );
			g_sceneBanner.SetBannerPath(banner);
			//save functions/////////////////////////////////
			g_sceneBanner.ClearVScenes();
			for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
			{
				if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
					g_sceneBanner.AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
				else
					g_sceneBanner.AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
			}
			//save functions/////////////////////////////////

			fread( &g_extraTexturesNamingConventions, sizeof( CExtraTexturesNamingConventions ), 1, filePtr  );
			fread( &g_useGlobalAmbientColor, sizeof( CBool ), 1, filePtr  );
			fread( &g_globalAmbientColor, sizeof( CColor4f ), 1, filePtr  );
			fread( &g_sceneManagerObjectsPerSplit, sizeof( CInt), 1, filePtr );

			CFog fog;
			fog.SetColor( g_fogProperties.m_fogColor );
			fog.SetDensity( g_fogProperties.m_fogDensity );

			//read physX 
			CBool insertPhysXScene = CFalse;
			CChar strPhysXSceneName[MAX_NAME_SIZE];
			NxExtendedVec3 characterPos;
			CVec3f characterPitchYawTilt;
			fread( &insertPhysXScene, sizeof( CBool ), 1, filePtr  );
			fread( strPhysXSceneName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
			fread( &characterPos, sizeof( NxExtendedVec3 ), 1, filePtr );
			fread( &characterPitchYawTilt, sizeof( CVec3f ), 1, filePtr );
			g_characterPos = characterPos;
			g_cameraPitchYawTilt.x = g_camera->m_perspectiveCameraPitch = characterPitchYawTilt.x;
			g_cameraPitchYawTilt.y = g_camera->m_perspectiveCameraYaw = characterPitchYawTilt.y;
			g_cameraPitchYawTilt.z = g_camera->m_perspectiveCameraTilt = characterPitchYawTilt.z;
			PrintInfo( "\nPlayer info imported successfully" );
			if( insertPhysXScene )
			{
				//Copy this part to Win32 Project. Save functions
				CChar temp[MAX_NAME_SIZE];
				CChar* PhysXName = GetAfterPath( strPhysXSceneName );
				CChar PhysXPath[MAX_NAME_SIZE];
				CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
				Cpy( g_currentVSceneNameWithoutDot, g_currentVSceneName );
				GetWithoutDot( g_currentVSceneNameWithoutDot );
				sprintf( PhysXPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/External Physics/", PhysXName );
				if( g_multipleView->m_nx->LoadScene( PhysXPath, NXU::FT_XML ) )
				{
					g_menu.m_physicsDebugMode = CTrue;
					m_btnRemovePhysX.EnableWindow( TRUE );
					GetMenu()->CheckMenuItem( ID_PHYSICS_DEBUGMODE, MF_CHECKED );
					PrintInfo3( "\nPhysX debug activated" );

					g_multipleView->m_nx->SetSceneName( PhysXPath );
					sprintf( temp, "\nPhysX scene '%s' imported successufully", PhysXPath );
					PrintInfo( temp );

					m_physXElementListIndex = -1;
					for (int nItem = m_listBoxPhysXElements.GetItemCount()-1; nItem >= 0 ;nItem-- )
					{
						m_listBoxPhysXElements.DeleteItem(nItem);
					}
					CInt count = 0;
					for( std::vector<std::string>::iterator it = g_multipleView->m_nx->m_nxActorNames.begin(); it != g_multipleView->m_nx->m_nxActorNames.end();it++ )
					{
						CChar temp[MAX_NAME_SIZE];
						Cpy( temp, (*it).c_str() );
						InsertItemToPhysXList( temp, g_multipleView->m_nx->m_nxActorTypes[count] );
						count++;
					}
					//save functions/////////////////////////////////
					CDelete(g_externalPhysX);
					g_externalPhysX = CNew(CExternalPhysX);
					for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
					{
						if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
							g_externalPhysX->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
						else
							g_externalPhysX->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
					}
					g_externalPhysX->SetPhysXPath(PhysXPath);
					//save functions/////////////////////////////////

				}
				else
				{
					sprintf( temp, "\nCouldn't load the PhysX scene '%s'", PhysXPath );
					PrintInfo( temp, COLOR_RED );
					g_multipleView->m_nx->SetSceneName( PhysXPath );
				}
			}
			g_multipleView->m_nx->ResetCharacterPos( characterPos );

			CChar tempSceneName[MAX_NAME_SIZE];

			CInt tempSceneSize, tempGeoSize;
			fread( &tempSceneSize, sizeof( CInt ), 1, filePtr );

			//Copy this to Win32 Project as well
			CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
			Cpy( g_currentVSceneNameWithoutDot, g_currentVSceneName );
			GetWithoutDot( g_currentVSceneNameWithoutDot );

			for( CInt i = 0 ; i < tempSceneSize; i++ )
			{
				CInt clipIndex;
				CBool playAnimation, loopAnimation;

				fread( tempSceneName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( &clipIndex, 1, sizeof(CInt), filePtr );
				fread( &playAnimation, 1, sizeof( CBool ), filePtr );
				fread( &loopAnimation, 1, sizeof( CBool ), filePtr );
				
				fread( &tempGeoSize, sizeof( CInt ), 1, filePtr );


				CBool sceneLoaded = CFalse;
				CScene * tempScene = new CScene();
				CChar * nameOnly = GetAfterPath( tempSceneName );

				//save functions. it should be copies in WIN32 Project as well
				CChar name[MAX_NAME_SIZE];

				sprintf( name, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/External Scenes/", nameOnly );

				g_useOriginalPathOfDAETextures = CFalse;
				if( tempScene->Load( name, CTrue, CTrue ) )
				{
					tempScene->SetClipIndexForStartup( clipIndex );
					tempScene->m_playAnimation = playAnimation;
					if( tempScene->m_playAnimation )
					{
						tempScene->m_animationStatus = eANIM_PLAY;
						tempScene->SetClipIndex( clipIndex, loopAnimation );
					}
					else
					{
						tempScene->m_animationStatus = eANIM_PAUSE;
					}
					tempScene->m_loopAnimationAtStartup = loopAnimation;
					//save functions/////////////////////////////////
					g_currentScene = tempScene; //mark the current scene. Save functions

					for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
					{
						if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
							tempScene->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
						else
							tempScene->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
					}
					//save functions/////////////////////////////////

					tempScene->Update();

					g_scene.push_back( tempScene );
					InsertItemToSceneList( tempScene->m_pureFileName );
					PrintInfo( "\nScene ' ");
					PrintInfo( tempScene->m_fileName, COLOR_RED_GREEN );
					PrintInfo( " ' loaded successfully" );
					
					sceneLoaded = CTrue;

					if( i == tempSceneSize - 1 ) //last scene is selected, so show its objects
					{
						//remove the current contents of the object list
						for (int nItem = m_listBoxObjects.GetItemCount()-1; nItem >= 0 ;nItem-- )
						{
							m_listBoxObjects.DeleteItem(nItem);
						}
					}
				}
				else
				{
					CChar tempErrorName[MAX_NAME_SIZE];
					sprintf( tempErrorName, "\nCouldn't load the scene '%s'", name );
					PrintInfo( tempErrorName, COLOR_RED );
					delete tempScene;
					tempScene = NULL;
					fclose( filePtr );
					ReleaseCapture();
					sceneLoaded = CFalse;
				}
				if( !sceneLoaded )
					return CFalse;
				if( CmpIn( tempScene->GetName(), "trigger" ) ) //triggers
				{
					if(!g_multipleView->m_nx->m_hasScene)
					{
						tempScene->m_isTrigger = CTrue;
						tempScene->Update();
						tempScene->CreateTrigger( g_multipleView->m_nx );
					}
					else
					{
						PrintInfo( "\nCouldn't create the triggers. In order to create triggers from COLLADA files, you should remove current external PhysX scene.", COLOR_RED );
					}

				}
				else if( CmpIn( tempScene->GetName(), "grass" ) )
				{
					tempScene->m_isGrass = CTrue;
					for( CInt j = 0; j < tempGeoSize; j++ )
					{
						tempScene->m_geometries[j]->SetAlphaMap( "grass_alpha" );
						tempScene->m_geometries[j]->SetDiffuse( "grass_color" );
					}
				}

				if( i == tempSceneSize - 1 ) //last scene is selected, so show its objects
				{
					for( CUInt j = 0; j < tempScene->m_images.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_images[j]->GetName(), eOBJECTLIST_IMAGE);
					}
					for( CUInt j = 0; j < tempScene->m_effects.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_effects[j]->GetName(), eOBJECTLIST_EFFECT);
					}
					for( CUInt j = 0; j < tempScene->m_materials.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_materials[j]->GetName(), eOBJECTLIST_MATERIAL);
					}
					//animations
					for( CUInt j = 0; j < tempScene->m_animations.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_animations[j]->GetName(), eOBJECTLIST_ANIMATION);
					}
					for( CUInt j = 0; j < tempScene->m_animationClips.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_animationClips[j]->GetName(), eOBJECTLIST_ANIMATIONCLIP);
					}
					//lights
					for( CUInt j = 0; j < tempScene->m_lights.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_lights[j]->GetName(), eOBJECTLIST_LIGHT);
					}
					//for( CUInt j = 0; j < tempScene->m_lightInstances.size(); j++ )
					//{
					//	InserItemToObjectList( tempScene->m_lightInstances[j]->GetName());
					//}
					//camera
					for( CUInt j = 0; j < tempScene->m_cameras.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_cameras[j]->GetName(), eOBJECTLIST_CAMERA);
					}
					//for( CUInt j = 0; j < tempScene->m_cameraInstances.size(); j++ )
					//{
					//	InserItemToObjectList( tempScene->m_cameraInstances[j]->GetName());
					//}
					//geometry
					for( CUInt j = 0; j < tempScene->m_geometries.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_geometries[j]->GetName(), eOBJECTLIST_MESH);
					}
					//for( CUInt j = 0; j < tempScene->m_instanceGeometries.size(); j++ )
					//{
					//	InserItemToObjectList( tempScene->m_instanceGeometries[j]->GetName());
					//}
					//controller
					for( CUInt j = 0; j < tempScene->m_controllers.size(); j++ )
					{
						InserItemToObjectList( tempScene->m_controllers[j]->GetName(), eOBJECTLIST_SKINCONTROLLER);
					}
				}

				for( CInt j = 0; j < tempGeoSize; j++ )
				{
					CChar m_strNormalMap[MAX_NAME_SIZE];
					CChar m_strShadowMap[MAX_NAME_SIZE];
					CChar m_strDirtMap[MAX_NAME_SIZE];
					CChar m_strAlphaMap[MAX_NAME_SIZE];
					CChar m_strGlossMap[MAX_NAME_SIZE];
					CChar m_strHeightMap[MAX_NAME_SIZE];
					CChar m_strDuDvMap[MAX_NAME_SIZE];
					CChar m_strDiffuse[MAX_NAME_SIZE];
					CBool m_hasAlphaMap, m_hasNormalMap, m_hasGlossMap, m_hasHeightMap, m_hasDuDvMap, m_hasShadowMap, m_hasDirtMap, m_hasDiffuse;
					CBool m_cullFaces;
					CUInt m_groupSize;
					CFloat m_bias, m_scale;

					fread( &m_hasShadowMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strShadowMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasDirtMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strDirtMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasAlphaMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strAlphaMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasNormalMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
					fread( &m_bias, sizeof(CFloat), 1, filePtr  );
					fread( &m_scale, sizeof(CFloat), 1, filePtr  );

					fread( &m_hasGlossMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strGlossMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasHeightMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strHeightMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasDuDvMap, sizeof( CBool ), 1, filePtr  );
					fread( m_strDuDvMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					fread( &m_hasDiffuse, sizeof( CBool ), 1, filePtr  );
					fread( m_strDiffuse, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

					//load cull face( enabled or disabled data) for the current geo
					fread( &m_cullFaces, sizeof( CBool ), 1, filePtr );

					//read PhysX data
					CUInt physx_point_size;
					fread( &physx_point_size, sizeof( CUInt ), 1, filePtr );
					for( CUInt m = 0; m < physx_point_size; m++ )
					{
						CVec3f* point = CNew(CVec3f);
						fread( point , sizeof( CVec3f ), 1, filePtr );
						tempScene->m_geometries[j]->m_physx_points.push_back( point );
					}
					CUInt physx_triangles_size;
					fread( &physx_triangles_size, sizeof( CUInt ), 1, filePtr );

					for( CUInt m = 0; m < physx_triangles_size; m++ )
					{
						CTriangles* tri = CNew( CTriangles );
						CUInt count;
						fread( &count, sizeof(CUInt), 1, filePtr );
						tri->m_count = count;
						tri->m_indexes = CNewData(CUInt, tri->m_count * 3);

						for( CUInt n = 0; n < tri->m_count * 3; n++ )
						{
							CUInt index;
							fread( &index, sizeof( CUInt ), 1, filePtr );
							tri->m_indexes[n] = index;
						}

						tempScene->m_geometries[j]->m_physx_triangles.push_back(tri);
					}
					///////////////////

					fread( &m_groupSize, sizeof( CUInt ), 1, filePtr );
					//store group info

					if( sceneLoaded && !tempScene->m_isGrass)
					{
						if( m_hasShadowMap )
						{
							tempScene->m_geometries[j]->m_hasShadowMap = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strShadowMap, m_strShadowMap );
						}
						if( m_hasDirtMap )
						{
							tempScene->m_geometries[j]->m_hasDirtMap = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strDirtMap, m_strDirtMap );
						}
						if( m_hasAlphaMap )
						{
							tempScene->m_geometries[j]->m_hasAlphaMap = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strAlphaMap, m_strAlphaMap );
						}
						if( m_hasNormalMap )
						{
							tempScene->m_geometries[j]->m_hasNormalMap = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strNormalMap, m_strNormalMap );
						}
						tempScene->m_geometries[j]->m_parallaxMapBias = m_bias;
						tempScene->m_geometries[j]->m_parallaxMapScale = m_scale;
						if( m_hasGlossMap )
						{
							tempScene->m_geometries[j]->m_hasGlossMap = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strGlossMap, m_strGlossMap );
						}
						if( m_hasDiffuse )
						{
							tempScene->m_geometries[j]->m_hasDiffuse = CTrue;
							Cpy( tempScene->m_geometries[j]->m_strDiffuse, m_strDiffuse );
						}
						//if( m_hasHeightMap )
						//	tempScene->m_geometries[j]->SetHeightMap( m_strHeightMap );
						//if( m_hasDuDvMap )
						//	tempScene->m_geometries[j]->SetDuDvMap(); //under construction!
						if( m_cullFaces )
							tempScene->m_geometries[j]->SetCullFace( CTrue );
						else
							tempScene->m_geometries[j]->SetCullFace( CFalse );

						for( CUInt k = 0; k < m_groupSize; k++ )
						{
							CChar m_strGroupShadowMap[MAX_NAME_SIZE];
							CBool m_hasGroupShadowMap;
							fread( &m_hasGroupShadowMap, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupShadowMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							if( m_hasGroupShadowMap )
								tempScene->m_geometries[j]->m_groups[k]->SetShadowMap( m_strGroupShadowMap );

							CChar m_strGroupGlossMap[MAX_NAME_SIZE];
							CBool m_hasGroupGlossMap;
							fread( &m_hasGroupGlossMap, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupGlossMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							if( m_hasGroupGlossMap )
								tempScene->m_geometries[j]->m_groups[k]->SetGlossMap( m_strGroupGlossMap );

							CChar m_strGroupAlphaMap[MAX_NAME_SIZE];
							CBool m_hasGroupAlphaMap;
							fread( &m_hasGroupAlphaMap, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupAlphaMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							if( m_hasGroupAlphaMap )
								tempScene->m_geometries[j]->m_groups[k]->SetAlphaMap( m_strGroupAlphaMap );

							CChar m_strGroupDirtMap[MAX_NAME_SIZE];
							CBool m_hasGroupDirtMap;
							fread( &m_hasGroupDirtMap, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupDirtMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							if( m_hasGroupDirtMap )
								tempScene->m_geometries[j]->m_groups[k]->SetDirtMap( m_strGroupDirtMap );

							CChar m_strGroupNormalMap[MAX_NAME_SIZE];
							CFloat m_bias, m_scale;
							CBool m_hasGroupNormalMap;
							fread( &m_hasGroupNormalMap, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							fread( &m_bias, sizeof(CFloat), 1, filePtr  );
							fread( &m_scale, sizeof(CFloat), 1, filePtr  );
							if( m_hasGroupNormalMap )
								tempScene->m_geometries[j]->m_groups[k]->SetNormalMap( m_strGroupNormalMap, m_bias, m_scale );

							CChar m_strGroupDiffuseMap[MAX_NAME_SIZE];
							CBool m_hasGroupDiffuse;
							fread( &m_hasGroupDiffuse, sizeof( CBool ), 1, filePtr  );
							fread( m_strGroupDiffuseMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
							if( m_hasGroupDiffuse )
								tempScene->m_geometries[j]->m_groups[k]->SetDiffuse( m_strGroupDiffuseMap );
						}
					}
				} //for all of the geos

				CUInt instanceGeoSize;
				fread( &instanceGeoSize, 1, sizeof( CUInt ), filePtr );
				for( CUInt j = 0; j < instanceGeoSize; j++ )
				{
					CChar geoName[MAX_NAME_SIZE];
					CMatrix instanceLocalToWorldMatrix;
					CBool hasPhysX;
					CChar PhysXName[MAX_NAME_SIZE];
					CPhysXAlgorithm physXAlgorithm;
					CFloat physXDensity;
					CInt physXPercentage;
					CBool isTrigger;
					CBool hasScriptEnter;
					CBool hasScriptExit;
					CChar scriptEnter[MAX_NAME_SIZE];
					CChar scriptExit[MAX_NAME_SIZE];
					fread( geoName, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
					fread( instanceLocalToWorldMatrix, 1, sizeof( CMatrix ), filePtr );
					fread( &hasPhysX, 1, sizeof( CBool ), filePtr );
					fread( PhysXName, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr ); 
					fread( &physXAlgorithm, 1, sizeof( CPhysXAlgorithm), filePtr );
					fread( &physXDensity, 1, sizeof( CFloat ), filePtr );
					fread( &physXPercentage, 1, sizeof( CInt ), filePtr );
					fread( &isTrigger, 1, sizeof( CBool ), filePtr );
					fread( scriptEnter, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
					fread( &hasScriptEnter, 1, sizeof( CBool ), filePtr );
					fread( scriptExit, 1, sizeof (CChar ) * MAX_NAME_SIZE, filePtr );
					fread( &hasScriptExit, 1, sizeof( CBool ), filePtr );
					if( hasPhysX )
					{
						for(CUInt k = 0; k < tempScene->m_instanceGeometries.size(); k++ )
						{
							if( Cmp( geoName, tempScene->m_instanceGeometries[k]->m_abstractGeometry->GetName() ) )
							{
								CBool equal = CTrue;
								for( CUInt l = 0; l < 16; l++ )
								{
									if( instanceLocalToWorldMatrix[l] !=  tempScene->m_instanceGeometries[k]->m_localToWorldMatrix[l] )
									{
										equal = CFalse;
										break;
									}
								}
								if( equal )
								{
									if( tempScene->GeneratePhysX( physXAlgorithm, physXDensity, physXPercentage, isTrigger, tempScene->m_instanceGeometries[k] ) )
									{
										CChar scriptEnterPath[MAX_NAME_SIZE];
										CChar scriptExitPath[MAX_NAME_SIZE];

										CChar* tempEnterPath = GetAfterPath( scriptEnter );
										CChar* tempExitPath = GetAfterPath( scriptExit );
										//Copy this to Win32 Project as well
										sprintf( scriptEnterPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Scripts/", tempEnterPath );
										sprintf( scriptExitPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Scripts/", tempExitPath );

										Cpy( tempScene->m_instanceGeometries[k]->m_enterScript, scriptEnterPath );
										Cpy( tempScene->m_instanceGeometries[k]->m_exitScript, scriptExitPath );
										tempScene->m_instanceGeometries[k]->m_hasEnterScript = hasScriptEnter;
										tempScene->m_instanceGeometries[k]->m_hasExitScript = hasScriptExit;

										if( physXDensity > 0 || tempScene->m_instanceGeometries[k]->m_abstractGeometry->m_hasAnimation)
											InsertItemToPhysXList( tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_DYNAMIC_RIGIDBODY );
										else if( isTrigger )
											InsertItemToPhysXList( tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_TRIGGER );
										else
											InsertItemToPhysXList( tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_STATIC_RIGIDBODY );
									}
								}
							}
						} //for
					} //if has PhysX
				}
			} // for all of the scenes

			CBool showSky;
			fread( &showSky, sizeof( CBool ), 1, filePtr );

			if( showSky )
			{
				CChar name[MAX_NAME_SIZE];
				CChar path[MAX_NAME_SIZE];
				CInt slices, sides;
				CFloat dampening, radius, position[3];
				CBool exponential;
				fread( name, sizeof( CChar ), MAX_NAME_SIZE, filePtr );
				fread( path, sizeof( CChar ), MAX_NAME_SIZE, filePtr );
				fread( &slices, sizeof( CInt ), 1, filePtr );
				fread( &sides, sizeof( CInt ), 1, filePtr );
				fread( &radius, sizeof( CFloat ), 1, filePtr );
				fread( position, sizeof( CFloat ), 3, filePtr );
				fread( &dampening, sizeof( CFloat ), 1, filePtr );
				fread( &exponential, sizeof( CBool ), 1, filePtr );
				
				CChar skyPath[MAX_NAME_SIZE];
				CChar* tempPath = GetAfterPath( path );
				//Copy this to Win32 Project as well
				sprintf( skyPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sky/", tempPath );
				g_skyDome = CNew( CSkyDome );
				g_skyDome->SetName( name );
				g_skyDome->SetPath( skyPath );
				g_skyDome->SetSlices( slices );
				g_skyDome->SetSides( sides );
				g_skyDome->SetRadius( radius );
				g_skyDome->SetPosition( position );
				g_skyDome->SetDampening( dampening );
				g_skyDome->SetExponential( exponential );
				g_skyDome->Initialize();
				g_menu.m_insertAndShowSky = CTrue;
				GetMenu()->EnableMenuItem( ID_INSERT_SKYDOME, MF_DISABLED | MF_GRAYED );
				m_mainBtnSky.EnableWindow( FALSE );

				InserItemToEngineObjectList( g_skyDome->GetName() , eENGINEOBJECTLIST_SKY);

				//save functions/////////////////////////////////
				for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
				{
					if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
						g_skyDome->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
					else
						g_skyDome->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
				}
				//save functions/////////////////////////////////

			}
			else
				g_menu.m_insertAndShowSky = CFalse;

			CInt tempWaterCount, tempGeoWaterCount;
			CChar strNormalMap[ MAX_NAME_SIZE];
			CChar strDuDvMap[ MAX_NAME_SIZE ];
			CChar strWaterName[MAX_NAME_SIZE];
			CFloat waterPos[3];
			CFloat waterLightPos[3];
			CFloat waterHeight, waterSpeed, waterScale, waterUV;

			fread( &tempWaterCount, sizeof( CInt ), 1, filePtr );
			for( CInt i = 0 ; i < tempWaterCount; i++ )
			{
				CWater* water = new CWater;

				fread( strWaterName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( strNormalMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( strDuDvMap, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );

				fread( waterPos, sizeof( CFloat ), 3, filePtr );
				fread( waterLightPos, sizeof( CFloat ), 3, filePtr );
				fread( &waterHeight, sizeof( CFloat ), 1, filePtr );
				fread( &waterSpeed, sizeof( CFloat ), 1, filePtr );
				fread( &waterScale, sizeof( CFloat ), 1, filePtr );
				fread( &waterUV, sizeof( CFloat ), 1, filePtr );

				fread( &tempGeoWaterCount, sizeof( CInt ), 1, filePtr );
				for( CInt j = 0; j < tempGeoWaterCount; j++ )
				{
					CChar geoName[MAX_NAME_SIZE];
					fread( geoName, sizeof( CChar ), MAX_NAME_SIZE, filePtr );

					for( CUInt k = 0 ; k < g_scene.size(); k++ )
					{
						for( CUInt l = 0; l < g_scene[k]->m_geometries.size(); l++ )
						{
							if( Cmp( g_scene[k]->m_geometries[l]->GetName(), geoName ) )
								water->m_geometry.push_back( g_scene[k]->m_geometries[l] );
						}
					}
				}

				CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
				Cpy( g_currentVSceneNameWithoutDot, g_currentVSceneName );
				GetWithoutDot( g_currentVSceneNameWithoutDot );

				CChar dudvPath[MAX_NAME_SIZE];
				CChar* DuDvAfterPath = GetAfterPath( strDuDvMap );
				//Copy this to Win32 Project as well
				sprintf( dudvPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Waters/", DuDvAfterPath );

				CChar normalPath[MAX_NAME_SIZE];
				CChar* normalAfterPath = GetAfterPath( strNormalMap );
				//Copy this to Win32 Project as well
				sprintf( normalPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Waters/", normalAfterPath );

				water->SetName( strWaterName );
				water->SetDuDvMap( dudvPath );
				water->SetNormalMap( normalPath );
				water->SetHeight( waterHeight );
				water->SetSpeed( waterSpeed );
				water->SetScale( waterScale );
				water->SetUV( waterUV );
				water->SetPos( waterPos );
				water->SetLightPos( waterLightPos );
				water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFLECTION_ID );
				water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFRACTION_ID );
				water->CreateRenderTexture(g_waterTextureSize, 1, GL_DEPTH_COMPONENT, WATER_DEPTH_ID );
				water->SetSideVertexPositions();

				//save functions/////////////////////////////////
				for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
				{
					if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
						water->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
					else
						water->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
				}
				//save functions/////////////////////////////////

				g_engineWaters.push_back( water );
				InserItemToEngineObjectList( water->GetName() , eENGINEOBJECTLIST_WATER);

			}
			//Engine Lights
			CInt tempLightCount;
			fread( &tempLightCount, sizeof( CInt ), 1, filePtr );
			CFloat m_fLightPos[4]; 	CFloat m_fConstantAttenuation; CFloat m_fLinearAttenuation; CFloat m_fQuadAttenuation; CFloat m_fSpotCuttoff;
			CFloat m_fSpotDirection[4]; CFloat m_fSpotExponent; CFloat m_fShininess; CFloat m_fAmbientColor[4]; CFloat m_fDiffuseColor[4];
			CFloat m_fSpecularColor[4]; CLightType m_lightType;
			CChar lightName[ MAX_NAME_SIZE ];
			for( CInt i = 0 ; i < tempLightCount; i++ )
			{
				fread( lightName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( m_fLightPos, sizeof( CFloat ), 4, filePtr  );
				fread( &m_fConstantAttenuation, sizeof( CFloat ), 1, filePtr  );
				fread( &m_fLinearAttenuation, sizeof( CFloat ), 1, filePtr  );
				fread( &m_fQuadAttenuation, sizeof( CFloat ), 1, filePtr  );
				fread( &m_fShininess, sizeof( CFloat ), 1, filePtr  );
				fread( m_fAmbientColor, sizeof( CFloat ), 4, filePtr  );
				fread( m_fDiffuseColor, sizeof( CFloat ), 4, filePtr  );
				fread( m_fSpecularColor, sizeof( CFloat ), 4, filePtr  );
				fread( &m_lightType, sizeof( CLightType ), 1, filePtr  );
				if( m_lightType == eLIGHTTYPE_SPOT)
				{
					fread( &m_fSpotCuttoff, sizeof( CFloat ), 1, filePtr  );
					fread( m_fSpotDirection, sizeof( CFloat ), 4, filePtr  );
					fread( &m_fSpotExponent, sizeof( CFloat ), 1, filePtr  );
				}
				CInstanceLight* instance_light = new CInstanceLight();
				CLight* abstract_light = new CLight();

				instance_light->m_parent = NULL; //Not using COLLADA parent
				instance_light->m_abstractLight = abstract_light;
				abstract_light->SetName( lightName );
				abstract_light->SetType( m_lightType );
				switch( m_lightType )
				{
				case eLIGHTTYPE_SPOT:
					instance_light->m_abstractLight->SetSpotCutoff( m_fSpotCuttoff );
					instance_light->m_abstractLight->SetSpotExponent( m_fSpotExponent );
					instance_light->m_abstractLight->SetSpotDirection( m_fSpotDirection );
					break;
				}

				abstract_light->SetAmbient ( m_fAmbientColor );
				abstract_light->SetDiffuse( m_fDiffuseColor );
				abstract_light->SetSpecular( m_fSpecularColor );
				abstract_light->SetPosition( m_fLightPos );
				abstract_light->SetShininess( m_fShininess );
				abstract_light->SetConstantAttenuation( m_fConstantAttenuation );
				abstract_light->SetLinearAttenuation( m_fLinearAttenuation );
				abstract_light->SetQuadraticAttenuation( m_fQuadAttenuation );

				instance_light->SetIndex();

				g_engineLights.push_back( instance_light );

				InserItemToEngineObjectList( lightName, eENGINEOBJECTLIST_LIGHT );
				g_engineObjectNames.push_back( lightName );

			}

			//static sounds
			CInt tempStaticSoundCount;
			fread( &tempStaticSoundCount, sizeof( CInt ), 1, filePtr );
			CFloat staticSoundMaxDistance, staticSoundPitch, staticSoundReferenceDistance, staticSoundRolloff, staticSoundVolume;
			CFloat staticSoundPos[3];
			CBool play, loop;

			CChar name[ MAX_NAME_SIZE ], path[ MAX_NAME_SIZE ];
			CBool notLoaded = CFalse;

			for( CInt i = 0 ; i < tempStaticSoundCount; i++ )
			{
				fread( name, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( path, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( &loop, sizeof( CBool ), 1, filePtr  );
				fread( &staticSoundMaxDistance, sizeof( CFloat ), 1, filePtr  );
				fread( &staticSoundPitch, sizeof( CFloat ), 1, filePtr  );
				fread( &play, sizeof( CBool ), 1, filePtr  );
				fread( staticSoundPos, sizeof( CFloat ), 3, filePtr  );
				fread( &staticSoundReferenceDistance, sizeof( CFloat ), 1, filePtr  );
				fread( &staticSoundRolloff, sizeof( CFloat ), 1, filePtr  );
				fread( &staticSoundVolume, sizeof( CFloat ), 1, filePtr  );

				CChar StaticSoundPath[MAX_NAME_SIZE];
				CChar* StaticSoundName = GetAfterPath( path );
				//Copy this to Win32 Project as well
				sprintf( StaticSoundPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sounds/Static/", StaticSoundName );

				COpenALSoundBuffer* m_staticSoundBuffer = GetSoundBuffer( GetAfterPath(StaticSoundPath) );
				
				if( m_staticSoundBuffer == NULL || (m_staticSoundBuffer && !m_staticSoundBuffer->m_loaded ) )
				{
					if( m_staticSoundBuffer == NULL )
					{
						m_staticSoundBuffer = CNew( COpenALSoundBuffer );
						g_soundBuffers.push_back( m_staticSoundBuffer );
					}
					else 
					{
						CChar tempBuffer[MAX_NAME_SIZE];
						sprintf( tempBuffer, "\nTrying to reload '%s%s", GetAfterPath(m_staticSoundBuffer->GetName() ), "'" );
						PrintInfo( tempBuffer, COLOR_YELLOW );
					}

					if( !m_staticSoundBuffer->LoadOggVorbisFromFile( StaticSoundPath ) )
					{
						CChar buffer[MAX_NAME_SIZE];
						sprintf( buffer, "\n%s%s%s", "Couldn't load the file '", StaticSoundPath, "'" );
						PrintInfo( buffer, COLOR_RED );
						m_staticSoundBuffer->m_loaded = CFalse;

					}
					else
					{
						CChar buffer[MAX_NAME_SIZE];
						sprintf( buffer, "\n%s%s%s", "ogg file '", StaticSoundPath, "' loaded successfully." );
						PrintInfo( buffer );
						m_staticSoundBuffer->m_loaded = CTrue;
					}

					m_staticSoundBuffer->SetName( StaticSoundPath );	
				}
				else
				{
						CChar temp[MAX_NAME_SIZE]; 
						sprintf( temp, "\n%s%s%s", "sound buffer '", GetAfterPath(m_staticSoundBuffer->GetName()), "' already exists." );
						PrintInfo( temp, COLOR_YELLOW );
				}
				COpenALSoundSource* m_staticSoundSource = CNew( COpenALSoundSource );
				CStaticSound* m_staticSound = CNew( CStaticSound );

				m_staticSoundSource->BindSoundBuffer (*m_staticSoundBuffer);
				m_staticSoundSource->SetLooping( loop );
				m_staticSoundSource->SetPitch( staticSoundPitch );

				//m_staticSoundSource->SetVolume( staticSoundVolume );
				m_staticSoundSource->SetReferenceDistance( staticSoundReferenceDistance );
				m_staticSoundSource->SetMaxDistance( staticSoundMaxDistance );
				m_staticSoundSource->SetRolloff( staticSoundRolloff );
				m_staticSoundSource->SetSoundPosition( staticSoundPos );

				m_staticSound->SetName( name );
				m_staticSound->SetPath( StaticSoundPath );
				m_staticSound->SetPosition( staticSoundPos );
				m_staticSound->SetLoop( loop );
				m_staticSound->SetMaxDistance( staticSoundMaxDistance );
				m_staticSound->SetPitch( staticSoundPitch );
				m_staticSound->SetPlay( play );
				m_staticSound->SetRefrenceDistance( staticSoundReferenceDistance );
				m_staticSound->SetRolloff( staticSoundRolloff );
				m_staticSound->SetSoundSource( m_staticSoundSource );
				m_staticSound->SetSoundBuffer( m_staticSoundBuffer );

				//save functions/////////////////////////////////
				for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
				{
					if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
						m_staticSound->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
					else
						m_staticSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
				}
				//save functions/////////////////////////////////

				g_engineStaticSounds.push_back( m_staticSound );
				if( play && !notLoaded)
					g_multipleView->m_soundSystem->PlayALSound( *m_staticSoundSource );

				if( !notLoaded )
				{
					InserItemToEngineObjectList( m_staticSound->GetName() , eENGINEOBJECTLIST_STATICSOUND);
					PrintInfo( "\nStatic sound '", COLOR_GREEN );
					PrintInfo( StaticSoundPath, COLOR_RED_GREEN );
					PrintInfo( "' initialized successfully", COLOR_GREEN );
				}
				else
				{
					PrintInfo( "\nCouldn't load the static sound '", COLOR_RED );
					PrintInfo( StaticSoundPath, COLOR_RED_GREEN );
					PrintInfo( "'", COLOR_RED );
				}


			}

			//Ambient Sound
			CBool insertAmbientSound = CFalse;
			CChar strAmbientSoundName[MAX_NAME_SIZE];
			CChar strAmbientSoundPath[MAX_NAME_SIZE];
			CFloat volume, pitch;
			fread( &insertAmbientSound, sizeof( CBool ), 1, filePtr  );
			if( insertAmbientSound )
			{
				fread( strAmbientSoundName, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( strAmbientSoundPath, sizeof( CChar ), MAX_NAME_SIZE, filePtr  );
				fread( &volume, sizeof( CFloat ), 1, filePtr  );
				fread( &pitch, sizeof( CFloat ), 1, filePtr  );
			}

			CChar AmbientSoundPath[MAX_NAME_SIZE];
			CChar* AmbientSoundName = GetAfterPath( strAmbientSoundPath );
			//Copy this to Win32 Project as well
			sprintf( AmbientSoundPath, "%s%s%s%s", g_VScenePath, g_currentVSceneNameWithoutDot, "/Sounds/Ambient/", AmbientSoundName );
			//load the ambient sound if it exists
			if( insertAmbientSound )
			{
				CDelete( g_multipleView->m_ambientSound );
				COpenALSoundSource* m_ambientSoundSource = CNew( COpenALSoundSource );
				COpenALSoundBuffer* m_ambientSoundBuffer = CNew( COpenALSoundBuffer );

				//Initialize ambient sound here
				// Velocity of the source sound.
				if( !m_ambientSoundBuffer->LoadOggVorbisFromFile( AmbientSoundPath ) )
				{
					g_menu.m_insertAmbientSound = CTrue;
					g_multipleView->m_ambientSound = CNew( CAmbientSound );
					m_ambientSoundSource->BindSoundBuffer ( *m_ambientSoundBuffer );
					g_multipleView->m_ambientSound->SetName( strAmbientSoundName );
					g_multipleView->m_ambientSound->SetPath( strAmbientSoundPath );
					g_multipleView->m_ambientSound->SetSoundSource( m_ambientSoundSource );
					g_multipleView->m_ambientSound->SetSoundBuffer( m_ambientSoundBuffer );
					g_multipleView->m_ambientSound->SetVolume( volume );
					g_multipleView->m_ambientSound->SetPitch( pitch );
	
					PrintInfo("\nCouldn't load ambient sound '", COLOR_RED);
					PrintInfo( AmbientSoundPath, COLOR_RED_GREEN );
					PrintInfo( "'", COLOR_RED );
				}
				else
				{
					m_ambientSoundSource->BindSoundBuffer ( *m_ambientSoundBuffer );

					m_ambientSoundSource->SetLooping( true );
					m_ambientSoundSource->SetPitch( pitch );
					m_ambientSoundSource->SetVolume( volume );
					g_multipleView->m_soundSystem->PlayALSound( *m_ambientSoundSource );

					g_multipleView->m_ambientSound = CNew( CAmbientSound );
					g_multipleView->m_ambientSound->SetSoundSource( m_ambientSoundSource );
					g_multipleView->m_ambientSound->SetSoundBuffer( m_ambientSoundBuffer );
					g_multipleView->m_ambientSound->SetName( strAmbientSoundName );
					g_multipleView->m_ambientSound->SetPath( AmbientSoundPath );
					g_multipleView->m_ambientSound->SetVolume( volume );
					g_multipleView->m_ambientSound->SetPitch( pitch );
					//save functions/////////////////////////////////
					for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
					{
						if( Cmp( g_currentVSceneName, g_allVSceneNames[index].c_str() ) ) //current scene name found
							g_multipleView->m_ambientSound->AddVSceneToList( g_allVSceneNames[index], CFalse ); //Do not write to zip file
						else
							g_multipleView->m_ambientSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file
					}
					//save functions/////////////////////////////////

					PrintInfo("\nAmbient sound '");
					PrintInfo( AmbientSoundPath, COLOR_RED_GREEN );
					PrintInfo( "' loaded successfully" );
					
					g_menu.m_playAmbientSound = CTrue;
					g_menu.m_insertAmbientSound = CTrue;
					GetMenu()->EnableMenuItem( ID_INSERT_SOUND_AMBIENT, MF_DISABLED | MF_GRAYED );
					m_mainBtnAmbientSound.EnableWindow( FALSE );

					InserItemToEngineObjectList( g_multipleView->m_ambientSound->GetName(), eENGINEOBJECTLIST_AMBIENTSOUND );
				}

			}
			fclose( filePtr );
			ReleaseCapture();
			m_savePathName = m_strpathName;

			CChar temp[MAX_NAME_SIZE];
			sprintf( temp, "total errors: %i, total warnings: %i", totalErrors, totalWarnings );
			PrintInfo2( temp, COLOR_RED_GREEN );
			totalErrors = totalWarnings = 0;

		}
	}
	else if( iResponse == IDCANCEL )
		return CFalse;
	return CTrue;

}

CVoid CVandaEngine1Dlg::OnMenuClickedPhysicsGravity()
{
	if( g_menu.m_physicsGravity )
	{
		g_multipleView->m_nx->gDefaultGravity = NxVec3( 0.0f, -7.8f, 0.0f );
	}
	else
	{
		g_multipleView->m_nx->gDefaultGravity = NxVec3(0.0f);
	}
}

CVoid CVandaEngine1Dlg::OnMenuClickedEditMaterial()
{
	m_dlgEditMaterial->m_firstBias = m_dlgEditMaterial->m_firstScale = CTrue;
	if( g_selectedName != -1 )
	{
		CBool foundTarget = CFalse;
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				CGeometry* m_geometry = g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry;
				if( g_menu.m_geometryBasedSelection )
				{
					if(  g_scene[i]->m_instanceGeometries[j]->m_nameIndex == g_selectedName )
					{
						foundTarget = CTrue;
						g_render.SetSelectedScene( g_scene[i] ); 
						SetDialogData( CTrue, g_scene[i]->m_instanceGeometries[j], m_geometry );
					}
				}
				else //material based selection
				{
					for( CUInt k = 0; k < m_geometry->m_groups.size(); k++ )
					{
						CPolyGroup* m_group = m_geometry->m_groups[k];
						if( m_group->m_nameIndex == g_selectedName )
						{
							foundTarget = CTrue;
							g_render.SetSelectedScene( g_scene[i] ); 
							SetDialogData2( CTrue, g_scene[i]->m_geometries[j]->m_groups[k] );
						}
					}
				}
			}

		}
	}
	INT_PTR result = m_dlgEditMaterial->DoModal();
}

BOOL CVandaEngine1Dlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    CRect rect;
    GetClientRect(&rect);
    CBrush myBrush( RGB(40,40,40) );    // dialog background color
    CBrush *pOld = pDC->SelectObject(&myBrush);
    BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);


	CRect windowRect;
	GetClientRect( &windowRect );
	CFloat aspectRatio = fabs( CFloat( windowRect.bottom - windowRect.top ) / CFloat( windowRect.right - windowRect.left ) );
	//Standard size of left buttons = 6
	CInt fivePercent = ( (5 * ( windowRect.right - windowRect.left ) / 100) * aspectRatio);
	//horizontal gap between buttons
	m_horizontalPointFivePercent = CInt( 0.5 * ( windowRect.right - windowRect.left ) / 100);
	m_horizontalSizeOfLeftCulomn = 2 * m_horizontalPointFivePercent + 1 * fivePercent; //2 gaps+1 Buttons
	m_horizontalSizeOfRightCulomn = 5 * m_horizontalPointFivePercent + 5.5 * fivePercent; //5 gaps+3 Buttons

	//Vertical gap between areas
	CInt verticalOnePercent = ( windowRect.bottom - windowRect.top ) / 100;

	CRect rcRect;
	rcRect.left = CInt( 100 * ( windowRect.right - windowRect.left ) / 100 ) - m_horizontalSizeOfRightCulomn + m_horizontalPointFivePercent;
	rcRect.right = CInt( 99.5 * ( windowRect.right - windowRect.left ) / 100 );
	//ScreenToClient( rcRect );
	CPen * myPen = new CPen(PS_SOLID,1,RGB(127,127,127));
	previousColumn1 = rcRect.bottom;
	pDC->SelectObject(myPen);  
	//COLLADA scenes area
	previousColumn1 = m_startLeftButtons * ( windowRect.bottom - windowRect.top ) / 100;

	rcRect.top = previousColumn1 + 4 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 20 * verticalOnePercent;

	pDC->RoundRect( rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, 10, 10 );
	/////
	//COLLADA objects area
	previousColumn2 = rcRect.bottom;

	rcRect.top = previousColumn2 + 4 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 20 * verticalOnePercent;
	pDC->RoundRect( rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, 10, 10 );

	/////
	//Engine Objects area
	previousColumn3 = rcRect.bottom;

	rcRect.top = previousColumn3 + 4 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 20 * verticalOnePercent;
	pDC->RoundRect( rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, 10, 10 );
	/////

	/////
	//PhysX Elements area
	previousColumn4 = rcRect.bottom;

	rcRect.top = previousColumn4 + 4 * verticalOnePercent;
	rcRect.bottom = rcRect.top + 20 * verticalOnePercent;
	pDC->RoundRect( rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, 10, 10 );
	/////

	delete myPen; 

    pDC->SelectObject(pOld);    // restore old brush

    return bRes;                       // CDialog::OnEraseBkgnd(pDC);

	return CDialog::OnEraseBkgnd(pDC);
}

CVoid CVandaEngine1Dlg::OnBnClickedCullFace()
{
	//reverse cull face for the current selected items
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
		{
			if(  g_scene[i]->m_instanceGeometries[j]->m_nameIndex == g_selectedName )
			{
				if( g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->m_cullFaces )
					CheckCullFace( eCULLFACETYPEFORMENU_UNCHECKED );
				else
					CheckCullFace( eCULLFACETYPEFORMENU_CHECKED );

				g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->m_cullFaces = !g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->m_cullFaces;

			}
		}
	}
}


void CVandaEngine1Dlg::OnBnClickedBtnEngineObjectProperties()
{
	int nSelected = -1; 
	POSITION p = m_listBoxEngineObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxEngineObjects.GetNextSelectedItem(p);
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
		m_listBoxEngineObjects.GetItem(&lvi);

		if( g_skyDome && Cmp( g_skyDome->GetName(), szBuffer )  )
		{
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CFalse );
		
			ChangeSkyDomeProperties();

			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CTrue );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();
			return;
		}
		for( CUInt i = 0; i < g_engineLights.size(); i++ )
		{
			if( Cmp( g_engineLights[i]->m_abstractLight->GetName(), szBuffer ) )
			{
				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CFalse );

				ChangeLightProperties( g_engineLights[i]);

				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CTrue );

				g_multipleView->SetElapsedTimeFromBeginning();
				g_multipleView->RenderWindow();
				return;
			}
		}
		for( CUInt i = 0; i < g_engineWaters.size(); i++ )
		{
			if( Cmp( g_engineWaters[i]->GetName(), szBuffer ) )
			{
				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CFalse );

				ChangeWaterProperties(g_engineWaters[i]);

				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CTrue );

				g_multipleView->SetElapsedTimeFromBeginning();
				g_multipleView->RenderWindow();
				return;
			}
		}

		for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
		{
			if( Cmp( g_engineStaticSounds[i]->GetName(), szBuffer ) )
			{
				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CFalse );

				ChangeStaticSoundProperties(g_engineStaticSounds[i]);

				if( g_multipleView->m_enableTimer )
					g_multipleView->EnableTimer( CTrue );

				g_multipleView->SetElapsedTimeFromBeginning();
				return;
			}
		}
		if( g_menu.m_insertAmbientSound && Cmp( g_multipleView->m_ambientSound->GetName(), szBuffer )  )
		{
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CFalse );

			ChangeAmbientSoundProperties();

			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CTrue );

			g_multipleView->SetElapsedTimeFromBeginning();
			return;
		}
	}
}
CVoid CVandaEngine1Dlg::SortEngineObjectList(CInt selectedIndex)
{
	m_engineObjectListIndex--;
}
void CVandaEngine1Dlg::OnBnClickedBtnRemoveEngineObject()
{
	int nSelected = -1; 
	POSITION p = m_listBoxEngineObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxEngineObjects.GetNextSelectedItem(p);
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
		m_listBoxEngineObjects.GetItem(&lvi);

		CInt result = IDYES;
		if( m_askRemoveEngineObject )
		{
			CChar tempString[MAX_NAME_SIZE];
			sprintf( tempString, "Delete object '%s' ?", szBuffer );
			result = MessageBox( tempString, "Warning", MB_YESNO | MB_ICONERROR );
		}
		if( result == IDYES )
		{
			for( CUInt i = 0; i < g_engineObjectNames.size(); i++ )
			{
				if( Cmp( g_engineObjectNames[i].c_str(), szBuffer ) )
					g_engineObjectNames[i] = "";
			}

			if( g_skyDome && Cmp( g_skyDome->GetName(), szBuffer )  )
			{
				if( g_skyDome->GetIndex() == g_selectedName || g_skyDome->GetIndex()  == g_lastEngineObjectSelectedName )
				{
					g_showArrow = CFalse;
				}
				CDelete( g_skyDome );
				g_menu.m_insertAndShowSky = CFalse;
				GetMenu()->EnableMenuItem( ID_INSERT_SKYDOME, MF_ENABLED );
				m_mainBtnSky.EnableWindow( TRUE );

				m_listBoxEngineObjects.DeleteItem(nSelected);
				g_multipleView->SetElapsedTimeFromBeginning();
				g_multipleView->RenderWindow();
				if( m_listBoxEngineObjects.GetItemCount() == 0 )
				{
					m_btnRemoveEngineObject.EnableWindow( FALSE );
					m_btnEngineObjectProperties.EnableWindow( FALSE );
				}
				SortEngineObjectList(nSelected);
				return;
			}
			for( CUInt i = 0; i < g_engineLights.size(); i++ )
			{
				if( Cmp( g_engineLights[i]->m_abstractLight->GetName(), szBuffer ) )
				{
					if( g_engineLights[i]->GetIndex() == g_selectedName || g_engineLights[i]->GetIndex()  == g_lastEngineObjectSelectedName )
					{
						g_showArrow = CFalse;
					}

					if( g_engineLights[i]->m_abstractLight->GetType() == eLIGHTTYPE_DIRECTIONAL )
						if( Cmp( g_shadowProperties.m_directionalLightName, g_engineLights[i]->m_abstractLight->GetName() ) )
							Cpy( g_shadowProperties.m_directionalLightName, "\n" );

					CDelete( g_engineLights[i] );
					g_engineLights.erase( g_engineLights.begin() + i );

					m_listBoxEngineObjects.DeleteItem(nSelected);
					g_multipleView->SetElapsedTimeFromBeginning();
					g_multipleView->RenderWindow();
					if( m_listBoxEngineObjects.GetItemCount() == 0 )
					{
						m_btnRemoveEngineObject.EnableWindow( FALSE );
						m_btnEngineObjectProperties.EnableWindow( FALSE );
					}
					SortEngineObjectList(nSelected);
					return;

				}
			}
			for( CUInt i = 0; i < g_engineWaters.size(); i++ )
			{
				if( Cmp( g_engineWaters[i]->GetName(), szBuffer ) )
				{
					if( g_engineWaters[i]->GetIndex() == g_selectedName || g_engineWaters[i]->GetIndex()  == g_lastEngineObjectSelectedName )
					{
						g_showArrow = CFalse;
					}

					//delete the scene
					CDelete(  g_engineWaters[i] );
					//delete the vector that holds the scene
					g_engineWaters.erase( g_engineWaters.begin( ) + i );

					m_listBoxEngineObjects.DeleteItem(nSelected);
					g_multipleView->SetElapsedTimeFromBeginning();
					g_multipleView->RenderWindow();
					if( m_listBoxEngineObjects.GetItemCount() == 0 )
					{
						m_btnRemoveEngineObject.EnableWindow( FALSE );
						m_btnEngineObjectProperties.EnableWindow( FALSE );
					}
					SortEngineObjectList(nSelected);
					return;
				}
			}

			for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
			{
				if( Cmp( g_engineStaticSounds[i]->GetName(), szBuffer ) )
				{
					if( g_engineStaticSounds[i]->GetIndex() == g_selectedName || g_engineStaticSounds[i]->GetIndex()  == g_lastEngineObjectSelectedName )
					{
						g_showArrow = CFalse;
					}

					//delete the scene
					CDelete(  g_engineStaticSounds[i] );
					//delete the vector that holds the scene
					g_engineStaticSounds.erase( g_engineStaticSounds.begin( ) + i );

					m_listBoxEngineObjects.DeleteItem(nSelected);
					g_multipleView->SetElapsedTimeFromBeginning();
					g_multipleView->RenderWindow();
					if( m_listBoxEngineObjects.GetItemCount() == 0 )
					{
						m_btnRemoveEngineObject.EnableWindow( FALSE );
						m_btnEngineObjectProperties.EnableWindow( FALSE );
					}
					SortEngineObjectList(nSelected);
					return;
				}
			}
			if( g_menu.m_insertAmbientSound && Cmp( g_multipleView->m_ambientSound->GetName(), szBuffer )  )
			{
				g_multipleView->m_soundSystem->StopALSound( *(g_multipleView->m_ambientSound->GetSoundSource()) );
				alSourcei(g_multipleView->m_ambientSound->GetSoundSource()->GetSource(), AL_BUFFER, AL_NONE); 
				CDelete( g_multipleView->m_ambientSound );
				g_menu.m_insertAmbientSound = CFalse;
				m_listBoxEngineObjects.DeleteItem(nSelected);
				GetMenu()->EnableMenuItem( ID_INSERT_SOUND_AMBIENT, MF_ENABLED );
				m_mainBtnAmbientSound.EnableWindow( TRUE );
				g_multipleView->SetElapsedTimeFromBeginning();
				if( m_listBoxEngineObjects.GetItemCount() == 0 )
				{
					m_btnRemoveEngineObject.EnableWindow( FALSE );
					m_btnEngineObjectProperties.EnableWindow( FALSE );
				}
				SortEngineObjectList(nSelected);
				return;
			}
		} //end of if( result == IDYES )
	} // end of if( nSelected >= 0 )
}

CVoid CVandaEngine1Dlg::SortPhysXList()
{
	m_physXElementListIndex--;
}

CVoid CVandaEngine1Dlg::SortSceneList(CInt selectedIndex)
{
	m_sceneListIndex--;
}

CVoid CVandaEngine1Dlg::OnBnClickedBtnRemoveScene()
{
	int nSelected = -1; 
	POSITION p = m_listBoxScenes.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxScenes.GetNextSelectedItem(p);
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
		m_listBoxScenes.GetItem(&lvi);
		
		CChar tempString[MAX_NAME_SIZE];
		CInt result;
		if( m_askRemoveScene )
		{
			sprintf( tempString, "Delete Scene '%s' ?", szBuffer );
		    result = MessageBox( tempString, "Warning", MB_YESNO | MB_ICONERROR );
		}
		else
		{
			result = IDYES;
		}

		if( result == IDYES )
		{
			SetCapture();
			SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );

			g_octree->ResetState();
			Cpy(g_shadowProperties.m_directionalLightName, "\n" );

			for( CUInt i = 0; i < g_scene.size(); i++ )
			{
				if( Cmp( g_scene[i]->m_pureFileName, szBuffer ) )
				{
					for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
					{
						for( CUInt k = 0; k < g_engineWaters.size(); k++ )
						{
							for( CUInt l = 0; l < g_engineWaters[k]->m_geometry.size(); l++ )
							{
								if( Cmp( g_engineWaters[k]->m_geometry[l]->GetName(), g_scene[i]->m_geometries[j]->GetName() ) )
									g_engineWaters[k]->m_geometry.erase( g_engineWaters[k]->m_geometry.begin() + l );
							}
						}
					}

					//save functions
					for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
					{
						CGeometry* m_geo = g_scene[i]->m_geometries[j];
						for( CUInt k = 0; k < m_geo->m_groups.size(); k++ )
						{
							for( CUInt l = 0; l < 6; l++ )
							{
								CImage* currentImage = NULL;
								if( l == 0 && m_geo->m_groups[k]->m_hasDiffuse )
								{
									currentImage = m_geo->m_groups[k]->m_diffuseImg;
								}
								else if ( l == 1 && m_geo->m_groups[k]->m_hasNormalMap )
								{
									currentImage = m_geo->m_groups[k]->m_normalMapImg;
								}
								else if( l == 2 && m_geo->m_groups[k]->m_hasAlphaMap )
								{
									currentImage = m_geo->m_groups[k]->m_alphaMapImg;
								}
								else if( l == 3 && m_geo->m_groups[k]->m_hasGlossMap )
								{
									currentImage = m_geo->m_groups[k]->m_glossMapImg;
								}
								else if( l == 4 && m_geo->m_groups[k]->m_hasDirtMap )
								{
									currentImage = m_geo->m_groups[k]->m_dirtMapImg;
								}
								else if( l == 5 && m_geo->m_groups[k]->m_hasShadowMap )
								{
									currentImage = m_geo->m_groups[k]->m_shadowMapImg;
								}
								if( currentImage )
								{
									//see if other scenes use this texture
									CBool foundTarget = CFalse;
									for( CUInt m = 0; m < g_scene.size(); m++ )
									{
										if( !Cmp( g_scene[m]->GetName(), g_scene[i]->GetName() ) )
										{
											for( CUInt n = 0; n < g_scene[m]->m_geometries.size(); n++ )
											{
												CGeometry* m_geo = g_scene[m]->m_geometries[n];
												for( CUInt o = 0; o < m_geo->m_groups.size(); o++ )
												{
													if( m_geo->m_groups[o]->m_hasDiffuse && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_diffuseImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
													else if( m_geo->m_groups[o]->m_hasNormalMap && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_normalMapImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
													else if( m_geo->m_groups[o]->m_hasAlphaMap && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_alphaMapImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
													else if( m_geo->m_groups[o]->m_hasGlossMap && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_glossMapImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
													else if( m_geo->m_groups[o]->m_hasShadowMap && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_shadowMapImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
													else if( m_geo->m_groups[o]->m_hasDirtMap && Cmp(GetAfterPath(currentImage->m_fileName),GetAfterPath(m_geo->m_groups[o]->m_dirtMapImg->m_fileName) ) )
													{
														foundTarget = CTrue;
														break;
													}
												}
												if( foundTarget )
													break;
											}
										}
										if( foundTarget )
											break;
									}
									if( !foundTarget )
									{
										for( CUInt p = 0; p < g_images.size(); p++ )
										{
											if( Cmp( GetAfterPath(g_images[p]->m_fileName), GetAfterPath(currentImage->m_fileName ) ) )
											{
												CChar* nameAfterPath = GetAfterPath( currentImage->m_fileName );
												CChar temp[MAX_NAME_SIZE];
												sprintf( temp, "\n%s%s%s", "Image '", nameAfterPath, "' removed from memory" );
												PrintInfo( temp );
												CDelete( currentImage );
												g_images.erase( g_images.begin() + p );
												break;
											}
										}
									} //if (!foundTarget )
								} //if( currentImage )
							} //for( CUInt l = 0; l < 6; l++ )
						} //for groups
					} //for other scenes
					//save functions///////////////////////////////////////////////////

					//delete the scene
					CDelete(  g_scene[i] );
					//delete the vector that holds the scene
					g_scene.erase( g_scene.begin() + i );
					//delete item from the list
					m_listBoxScenes.DeleteItem(nSelected);

					//remove the current contents of the object list
					for (int nItem = m_listBoxObjects.GetItemCount()-1; nItem >= 0 ;nItem-- )
					{
						m_listBoxObjects.DeleteItem(nItem);
					}
				} //if
			} //for
			if( m_listBoxScenes.GetItemCount() == 0)
			{
				m_btnRemoveScene.EnableWindow( FALSE );
				m_btnSceneProperties.EnableWindow( FALSE );
			}

			SortSceneList( nSelected );

			g_multipleView->SetElapsedTimeFromBeginning();
			g_multipleView->RenderWindow();

			ReleaseCapture();
		} //if
	} //if
}

CVoid CVandaEngine1Dlg::OnBnClickedBtnRemoveObject()
{
	int nSelected = -1; 
	POSITION p = m_listBoxObjects.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxObjects.GetNextSelectedItem(p);
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
		m_listBoxObjects.GetItem(&lvi);

		CChar tempString[MAX_NAME_SIZE];
		sprintf( tempString, "Delete Object '%s' ?", szBuffer );
		CInt result = MessageBox( tempString, "Warning", MB_YESNO | MB_ICONERROR );
	} //if
}

HBRUSH CVandaEngine1Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = (HBRUSH)m_brush;// = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (pWnd->GetDlgCtrlID())
	{
	  case IDC_STATIC_SCENES:
	  case IDC_STATIC_OBJECTS:
	  case IDC_STATIC_ENGINE_OBJECTS:
	  case IDC_STATIC_PHYSX_ELEMENTS:
		   pDC->SetTextColor (RGB(180, 150, 000));
		   pDC->SetBkColor (RGB(40,40,40)); 
		   hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		   break;

	  default:
		   hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
		   break;
	}
	return hbr;

}

//To get rid of the Esc key
void CVandaEngine1Dlg::OnCancel()
{
	//CDialog::OnCancel();
}

CVoid CVandaEngine1Dlg::InsertItemToSceneList( char * sceneName )
{
	m_sceneListIndex++;
	int index = m_sceneListIndex;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = index;
	lvItem.iSubItem = 0;
	lvItem.pszText = sceneName;
	m_listBoxScenes.InsertItem(&lvItem);

	m_listBoxScenes.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP);
	m_listBoxScenes.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED | LVIS_FOCUSED  | LVS_SHOWSELALWAYS);
	m_listBoxScenes.SetSelectionMark(index);
	m_listBoxScenes.EnsureVisible(index, FALSE);
	m_listBoxScenes.UpdateWindow();
	if( m_listBoxScenes.GetItemCount() )
	{
		m_btnRemoveScene.EnableWindow( TRUE );
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( Cmp( g_scene[i]->m_pureFileName, sceneName ) )
			{
				if( g_scene[i]->m_hasAnimation )
					m_btnSceneProperties.EnableWindow( TRUE );
			}
		}

	}
	m_objectListIndex = -1;
}

CVoid CVandaEngine1Dlg::InserItemToObjectList( char * objectName, int imageIndex)
{
	m_objectListIndex++;
	int objectIndex = m_objectListIndex;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvItem.iItem = objectIndex;
	lvItem.iSubItem = 0;
	lvItem.iImage = imageIndex;
	lvItem.pszText = objectName;
	m_listBoxObjects.InsertItem(&lvItem);
	m_listBoxObjects.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP );
	m_listBoxObjects.SetItemState(objectIndex, LVIS_SELECTED, LVIS_ACTIVATING);
	m_listBoxObjects.SetSelectionMark(objectIndex);
	m_listBoxObjects.EnsureVisible(objectIndex, TRUE);
	m_listBoxObjects.UpdateWindow();
}

CVoid CVandaEngine1Dlg::InserItemToEngineObjectList( char * objectName, int imageIndex )
{
	m_engineObjectListIndex++;
	int engineObjectIndex = m_engineObjectListIndex;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvItem.iItem = engineObjectIndex;
	lvItem.iSubItem = 0;
	lvItem.iImage = imageIndex;
	lvItem.pszText = objectName;
	m_listBoxEngineObjects.InsertItem(&lvItem);
	m_listBoxEngineObjects.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP );
	m_listBoxEngineObjects.SetItemState(engineObjectIndex, LVIS_SELECTED,LVIS_ACTIVATING);
	m_listBoxEngineObjects.SetSelectionMark(engineObjectIndex);
	m_listBoxEngineObjects.EnsureVisible(engineObjectIndex, FALSE);
	m_listBoxEngineObjects.UpdateWindow();

	//if( m_listBoxEngineObjects.GetItemCount() )
	//{
	//	m_btnRemoveEngineObject.EnableWindow( TRUE );
	//	m_btnEngineObjectProperties.EnableWindow( TRUE );
	//}
}

CVoid CVandaEngine1Dlg::InsertItemToPhysXList( char * objectName, int imageIndex )
{
	m_physXElementListIndex++;
	int physXElementIndex = m_physXElementListIndex;
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvItem.iItem = physXElementIndex;
	lvItem.iSubItem = 0;
	lvItem.iImage = imageIndex;
	lvItem.pszText = objectName;
	m_listBoxPhysXElements.InsertItem(&lvItem);
	m_listBoxPhysXElements.SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP );
	m_listBoxPhysXElements.SetItemState(physXElementIndex, LVIS_SELECTED,LVIS_ACTIVATING);
	m_listBoxPhysXElements.SetSelectionMark(physXElementIndex);
	m_listBoxPhysXElements.EnsureVisible(physXElementIndex, FALSE);
	m_listBoxPhysXElements.UpdateWindow();
}

COpenALSoundBuffer *CVandaEngine1Dlg::GetSoundBuffer( const CChar * name )
{
	if (name == NULL) return NULL;
	for(CUInt i=0; i<g_soundBuffers.size(); i++)
	{
		if ( ICmp(GetAfterPath(g_soundBuffers[i]->GetName()), name ) )
			return g_soundBuffers[i];
	}
	return NULL;
}



CVoid CVandaEngine1Dlg::ChangeLightProperties(CInstanceLight* light )
{
	m_dlgAddLight = CNew( CAddLight );
	m_dlgAddLight->SetName( light->m_abstractLight->GetName() );
	m_dlgAddLight->SetPos( light->m_abstractLight->GetPosition() );
	m_dlgAddLight->SetAmbient( light->m_abstractLight->GetAmbient() );
	m_dlgAddLight->SetDiffuse( light->m_abstractLight->GetDiffuse() );
	m_dlgAddLight->SetSpecular( light->m_abstractLight->GetSpecular() );
	m_dlgAddLight->SetType( light->m_abstractLight->GetType() );
	m_dlgAddLight->SetShininess( light->m_abstractLight->GetShininess() );
	m_dlgAddLight->SetConstantAttenuation( light->m_abstractLight->GetConstantAttenuation() );
	m_dlgAddLight->SetLinearAttenuation( light->m_abstractLight->GetLinearAttenuation() );
	m_dlgAddLight->SetQuadAttenuation( light->m_abstractLight->GetQuadraticAttenuation() );
	if( m_dlgAddLight->GetType() == eLIGHTTYPE_SPOT )
	{
		m_dlgAddLight->SetSpotDirection( light->m_abstractLight->GetSpotDirection() );
		m_dlgAddLight->SetSpotExponent( light->m_abstractLight->GetSpotExponent() );
		m_dlgAddLight->SetSpotCuttoff( light->m_abstractLight->GetSpotCutoff() );
	}
	m_dlgAddLight->SetEditMode( CTrue );

	INT_PTR result = m_dlgAddLight->DoModal();
	if ( result  == IDOK )
	{
		if( light->GetIndex() == g_selectedName || light->GetIndex()  == g_lastEngineObjectSelectedName )
		{
			g_showArrow = CFalse;
		}
		m_askRemoveEngineObject = CFalse;
		m_askRemoveScene = CTrue;
		OnBnClickedBtnRemoveEngineObject();
		m_askRemoveEngineObject = CTrue;

		CInstanceLight* instance_light = new CInstanceLight();
		CLight* abstract_light = new CLight();

		instance_light->m_parent = NULL; //Not using COLLADA parent
		instance_light->m_abstractLight = abstract_light;
		abstract_light->SetName( m_dlgAddLight->GetName() );
		abstract_light->SetType( m_dlgAddLight->GetType() );
		switch( m_dlgAddLight->GetType() )
		{
		case eLIGHTTYPE_SPOT:
			instance_light->m_abstractLight->SetSpotCutoff( m_dlgAddLight->GetSpotCuttoff() );
			instance_light->m_abstractLight->SetSpotExponent( m_dlgAddLight->GetSpotExponent() );
			instance_light->m_abstractLight->SetSpotDirection( m_dlgAddLight->GetSpotDirection() );
			break;
		}

		abstract_light->SetAmbient ( m_dlgAddLight->GetAmbientColor() );
		abstract_light->SetDiffuse( m_dlgAddLight->GetDiffuseColor() );
		abstract_light->SetSpecular( m_dlgAddLight->GetSpecularColor() );
		abstract_light->SetPosition( m_dlgAddLight->GetLightPos() );
		abstract_light->SetShininess( m_dlgAddLight->GetShininess() );
		abstract_light->SetConstantAttenuation( m_dlgAddLight->GetConstantAttenuation() );
		abstract_light->SetLinearAttenuation( m_dlgAddLight->GetLinearAttenuation() );
		abstract_light->SetQuadraticAttenuation( m_dlgAddLight->GetQuadAttenuation() );

		instance_light->SetIndex();

		g_engineLights.push_back( instance_light );
		InserItemToEngineObjectList( instance_light->m_abstractLight->GetName() , eENGINEOBJECTLIST_LIGHT);
		if( !Cmp( m_dlgAddLight->GetName(), m_dlgAddLight->GetTempName() ) )
			for( std::vector<std::string>::iterator it = g_engineObjectNames.begin(); it != g_engineObjectNames.end();it++ )
				if( Cmp( m_dlgAddLight->GetTempName(), (*it).c_str() ) )
					(*it) = m_dlgAddLight->GetName();
		CDelete( m_dlgAddLight );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddLight );


}

CVoid CVandaEngine1Dlg::ChangeWaterProperties(CWater* water)
{
	m_dlgAddWater = CNew( CAddWater );
	m_dlgAddWater->SetName( water->GetName() );

	CChar* DuDvName =  water->GetDuDvMapName(); //GetAfterPath( water->GetDuDvMapName() );
	CChar* NormalName = water->GetNormalMapName(); //GetAfterPath( water->GetNormalMapName() );
	//CChar DuDvName[MAX_NAME_SIZE];
	//CChar NormalName[MAX_NAME_SIZE];
	//sprintf( DuDvName, "%s%s", g_pathProperties.m_waterPath, DuDv );
	//sprintf( NormalName, "%s%s", g_pathProperties.m_waterPath, Normal );

	m_dlgAddWater->SetDuDvMapName( DuDvName );
	m_dlgAddWater->SetNormalMapName( NormalName );
	m_dlgAddWater->SetHeight( water->GetHeight() );
	m_dlgAddWater->SetScale( water->GetScale() );
	m_dlgAddWater->SetUV( water->GetUV() );
	m_dlgAddWater->SetLightPos( water->GetLightPos() );
	m_dlgAddWater->SetPos( water->GetPos() );
	m_dlgAddWater->SetSpeed( water->GetSpeed() );
	m_dlgAddWater->SetEditMode( CTrue );

	INT_PTR result = m_dlgAddWater->DoModal();
	if ( result  == IDOK )
	{
		if( water->GetIndex() == g_selectedName || water->GetIndex()  == g_lastEngineObjectSelectedName )
		{
			g_showArrow = CFalse;
		}
		m_askRemoveEngineObject = CFalse;
		OnBnClickedBtnRemoveEngineObject();
		m_askRemoveEngineObject = CTrue;

		CWater* water = new CWater;
		water->SetName( m_dlgAddWater->GetName() );
		water->SetDuDvMap( m_dlgAddWater->GetDuDvMap(), CTrue );
		water->SetNormalMap( m_dlgAddWater->GetNormalMap(), CTrue );
		water->SetPos( m_dlgAddWater->GetPos() );
		water->SetLightPos( m_dlgAddWater->GetLightPos() );
		water->SetHeight( m_dlgAddWater->GetHeight() );
		water->SetScale( m_dlgAddWater->GetScale() );
		water->SetUV( m_dlgAddWater->GetUV() );
		water->SetSpeed( m_dlgAddWater->GetSpeed() );
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFLECTION_ID );
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFRACTION_ID );
		water->CreateRenderTexture(g_waterTextureSize, 1, GL_DEPTH_COMPONENT, WATER_DEPTH_ID );
		water->SetSideVertexPositions();
		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			water->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		g_engineWaters.push_back( water );
		InserItemToEngineObjectList( water->GetName() , eENGINEOBJECTLIST_WATER);
		if( !Cmp( m_dlgAddWater->GetName(), m_dlgAddWater->GetTempName() ) )
			for( std::vector<std::string>::iterator it = g_engineObjectNames.begin(); it != g_engineObjectNames.end();it++ )
				if( Cmp( m_dlgAddWater->GetTempName(), (*it).c_str() ) )
					(*it) = m_dlgAddWater->GetName();
		CDelete( m_dlgAddWater );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddWater );
}

CVoid CVandaEngine1Dlg::ChangeStaticSoundProperties(CStaticSound* sound)
{
	m_dlgAddStaticSound = CNew( CAddStaticSound );
	m_dlgAddStaticSound->SetSoundPos( sound->GetPosition() );
	m_dlgAddStaticSound->SetName( sound->GetName() );

	CChar StaticSoundPath[MAX_NAME_SIZE];
	CChar* StaticSoundName = GetAfterPath( sound->GetPath() );
	sprintf( StaticSoundPath, "%s", sound->GetPath() );

	m_dlgAddStaticSound->SetPath( StaticSoundPath );
	m_dlgAddStaticSound->SetPitch( sound->GetPitch() );
	m_dlgAddStaticSound->SetVolume( sound->GetVolume() );
	m_dlgAddStaticSound->SetRolloff( sound->GetRolloff() );
	m_dlgAddStaticSound->SetMaxDistance( sound->GetMaxDistance() );
	m_dlgAddStaticSound->SetReferenceDistance( sound->GetRefrenceDistance() );
	m_dlgAddStaticSound->SetPlay( sound->GetPlay() );
	m_dlgAddStaticSound->SetLoop( sound->GetLoop() );
	m_dlgAddStaticSound->SetEditMode( CTrue );
	//m_dlgAddStaticSound->SetIndex( sound->GetIndex() );

	INT_PTR result = m_dlgAddStaticSound->DoModal();
	if ( result  == IDOK )
	{
		if( sound->GetIndex() == g_selectedName || sound->GetIndex()  == g_lastEngineObjectSelectedName )
		{
			g_showArrow = CFalse;
		}
		m_askRemoveEngineObject = CFalse;
		OnBnClickedBtnRemoveEngineObject();
		m_askRemoveEngineObject = CTrue;

		COpenALSoundSource* m_staticSoundSource = CNew( COpenALSoundSource );
		CStaticSound* m_staticSound = CNew( CStaticSound );

		CChar temp[ MAX_NAME_SIZE];
		//if( m_dlgAddStaticSound->m_strStaticSoundPureDataPath.IsEmpty() )
			Cpy( temp, m_dlgAddStaticSound->GetPath() );
		//else
		//	sprintf( temp, "%s%s.ogg", g_pathProperties.m_soundPath, m_dlgAddStaticSound->m_strStaticSoundPureDataPath );

		COpenALSoundBuffer* m_staticSoundBuffer = GetSoundBuffer( GetAfterPath(temp) );
		if( m_staticSoundBuffer == NULL || (m_staticSoundBuffer && !m_staticSoundBuffer->m_loaded ) )
		{
			if( m_staticSoundBuffer == NULL )
			{
				m_staticSoundBuffer = CNew( COpenALSoundBuffer );
				g_soundBuffers.push_back( m_staticSoundBuffer );
			}
			else 
			{
				CChar tempBuffer[MAX_NAME_SIZE];
				sprintf( tempBuffer, "\nTrying to reload '%s%s", GetAfterPath(m_staticSoundBuffer->GetName() ), "'" );
				PrintInfo( tempBuffer, COLOR_YELLOW );
			}
			if( !m_staticSoundBuffer->LoadOggVorbisFromFile( temp ) )
			{
				CChar buffer[MAX_NAME_SIZE];
				sprintf( buffer, "\n%s%s%s", "Couldn't load the file '", temp, "'" );
				PrintInfo( buffer, COLOR_RED );
				m_staticSoundBuffer->m_loaded = CFalse;

			}
			else
			{
				CChar buffer[MAX_NAME_SIZE];
				sprintf( buffer, "\n%s%s%s", "ogg file '", temp, "' loaded successfully." );
				PrintInfo( buffer );
				m_staticSoundBuffer->m_loaded = CTrue;
			}
			m_staticSoundBuffer->SetName( temp );	
		}
		else
		{
				CChar temp[MAX_NAME_SIZE]; 
				sprintf( temp, "\n%s%s%s", "sound buffer '", GetAfterPath(m_staticSoundBuffer->GetName()), "' already exists." );
				PrintInfo( temp, COLOR_YELLOW );
		}

		m_staticSoundSource->BindSoundBuffer (*m_staticSoundBuffer);
		m_staticSoundSource->SetLooping( m_dlgAddStaticSound->GetLoopCondition() );
		m_staticSoundSource->SetPitch( m_dlgAddStaticSound->GetPitch() );
		m_staticSoundSource->SetReferenceDistance( m_dlgAddStaticSound->GetReferenceDistance() );
		m_staticSoundSource->SetMaxDistance( m_dlgAddStaticSound->GetMaxDistance() );
		m_staticSoundSource->SetRolloff( m_dlgAddStaticSound->GetRolloff() );
		m_staticSoundSource->SetSoundPosition( m_dlgAddStaticSound->GetSoundPos() );

		m_staticSound->SetName( m_dlgAddStaticSound->GetName() );
		m_staticSound->SetPath( temp );
		m_staticSound->SetPosition( m_dlgAddStaticSound->GetSoundPos() );
		m_staticSound->SetLoop( m_dlgAddStaticSound->GetLoopCondition() );
		m_staticSound->SetMaxDistance( m_dlgAddStaticSound->GetMaxDistance() );
		m_staticSound->SetPitch( m_dlgAddStaticSound->GetPitch() );
		m_staticSound->SetPlay( m_dlgAddStaticSound->GetPlayCondition() );
		m_staticSound->SetRefrenceDistance( m_dlgAddStaticSound->GetReferenceDistance() );
		m_staticSound->SetRolloff( m_dlgAddStaticSound->GetRolloff() );
		//m_staticSound->SetIndex( m_dlgAddStaticSound->GetIndex() );

		m_staticSound->SetSoundSource( m_staticSoundSource );
		m_staticSound->SetSoundBuffer( m_staticSoundBuffer );

		if( m_dlgAddStaticSound->GetPlayCondition() )
		{
			g_multipleView->m_soundSystem->PlayALSound( *m_staticSoundSource );
		}
		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			m_staticSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		g_engineStaticSounds.push_back( m_staticSound );
		InserItemToEngineObjectList( m_staticSound->GetName() , eENGINEOBJECTLIST_STATICSOUND);

		if( !Cmp( m_dlgAddStaticSound->GetName(), m_dlgAddStaticSound->GetTempName() ) )
			for( std::vector<std::string>::iterator it = g_engineObjectNames.begin(); it != g_engineObjectNames.end();it++ )
				if( Cmp( m_dlgAddStaticSound->GetTempName(), (*it).c_str() ) )
					(*it) = m_dlgAddStaticSound->GetName();

		CDelete( m_dlgAddStaticSound );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddStaticSound );
}

CVoid CVandaEngine1Dlg::ChangeAmbientSoundProperties()
{
	m_dlgAddAmbientSound = CNew( CAddAmbientSound );
	m_dlgAddAmbientSound->SetName( g_multipleView->m_ambientSound->GetName() );

	CChar AmbientSoundPath[MAX_NAME_SIZE];
	CChar* AmbientSoundName = GetAfterPath( g_multipleView->m_ambientSound->GetPath() );
	sprintf( AmbientSoundPath, "%s", g_multipleView->m_ambientSound->GetPath() );

	m_dlgAddAmbientSound->SetPath( AmbientSoundPath );
	m_dlgAddAmbientSound->SetVolume( g_multipleView->m_ambientSound->GetVolume() );
	m_dlgAddAmbientSound->SetPitch( g_multipleView->m_ambientSound->GetPitch() );
	m_dlgAddAmbientSound->SetEditMode( CTrue );

	INT_PTR result = m_dlgAddAmbientSound->DoModal();
	if ( result  == IDOK )
	{
		m_askRemoveEngineObject = CFalse;
		OnBnClickedBtnRemoveEngineObject();
		m_askRemoveEngineObject = CTrue;

		COpenALSoundSource* m_ambientSoundSource = CNew( COpenALSoundSource );
		COpenALSoundBuffer* m_ambientSoundBuffer = CNew( COpenALSoundBuffer );

		CChar temp[ MAX_NAME_SIZE];
		//if( m_dlgAddAmbientSound->m_strPureAmbientSoundBuffer.IsEmpty() )
			Cpy( temp, m_dlgAddAmbientSound->GetPath() );
		//else
		//	sprintf( temp, "%s%s.ogg", g_pathProperties.m_soundPath, m_dlgAddAmbientSound->m_strPureAmbientSoundBuffer );

		if( !m_ambientSoundBuffer->LoadOggVorbisFromFile( temp ) )
			return;
		m_ambientSoundSource->BindSoundBuffer( *(m_ambientSoundBuffer) );

		m_ambientSoundSource->SetLooping( true );
		m_ambientSoundSource->SetPitch( m_dlgAddAmbientSound->GetPitch() );
		m_ambientSoundSource->SetVolume( m_dlgAddAmbientSound->GetVolume() );

		g_multipleView->m_ambientSound = CNew( CAmbientSound );
		g_multipleView->m_ambientSound->SetSoundSource( m_ambientSoundSource );
		g_multipleView->m_ambientSound->SetSoundBuffer( m_ambientSoundBuffer );
		g_multipleView->m_ambientSound->SetName( m_dlgAddAmbientSound->GetName() );
		g_multipleView->m_ambientSound->SetPath( temp );
		g_multipleView->m_ambientSound->SetVolume( m_dlgAddAmbientSound->GetVolume() );
		g_multipleView->m_ambientSound->SetPitch( m_dlgAddAmbientSound->GetPitch() );


		g_multipleView->m_soundSystem->PlayALSound( *m_ambientSoundSource );
		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			g_multipleView->m_ambientSound->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////

		PrintInfo( "\nAmbient sound '" );
		sprintf( temp, "%s", m_dlgAddAmbientSound->m_strAmbientSoundBuffer );
		PrintInfo( temp, COLOR_RED_GREEN );
		PrintInfo( "' loaded successfully" );
		
		g_menu.m_playAmbientSound = CTrue;
		GetMenu()->EnableMenuItem( ID_INSERT_SOUND_AMBIENT, MF_DISABLED | MF_GRAYED );
		m_mainBtnAmbientSound.EnableWindow( FALSE );

		g_menu.m_insertAmbientSound = CTrue;
		InserItemToEngineObjectList( g_multipleView->m_ambientSound->GetName() , eENGINEOBJECTLIST_AMBIENTSOUND);

		if( !Cmp( m_dlgAddAmbientSound->GetName(), m_dlgAddAmbientSound->GetTempName() ) )
			for( std::vector<std::string>::iterator it = g_engineObjectNames.begin(); it != g_engineObjectNames.end();it++ )
				if( Cmp( m_dlgAddAmbientSound->GetTempName(), (*it).c_str() ) )
					(*it) = m_dlgAddAmbientSound->GetName();

		CDelete( m_dlgAddAmbientSound );
	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddAmbientSound );
}

CVoid CVandaEngine1Dlg::ChangeSkyDomeProperties()
{
	m_dlgAddSkyDome = CNew( CAddSkyDome );

	m_dlgAddSkyDome->SetPos( g_skyDome->GetPosition() );
	m_dlgAddSkyDome->SetName( g_skyDome->GetName() );
	m_dlgAddSkyDome->SetPath( g_skyDome->GetPath() );
	m_dlgAddSkyDome->SetRadius( g_skyDome->GetRadius() );
	m_dlgAddSkyDome->SetDampening( g_skyDome->GetDampening() );
	m_dlgAddSkyDome->SetSides( g_skyDome->GetSides() );
	m_dlgAddSkyDome->SetSlices( g_skyDome->GetSlices() );
	m_dlgAddSkyDome->SetExponential( g_skyDome->GetExponential() );
	m_dlgAddSkyDome->SetEditMode( CTrue );

	INT_PTR result = m_dlgAddSkyDome->DoModal();
	if ( result == IDOK )
	{
		if( g_skyDome->GetIndex() == g_selectedName || g_skyDome->GetIndex()  == g_lastEngineObjectSelectedName )
		{
			g_showArrow = CFalse;
		}
		m_askRemoveEngineObject = CFalse;
		OnBnClickedBtnRemoveEngineObject();
		m_askRemoveEngineObject = CTrue;
				
		CDelete( g_skyDome );
		g_skyDome = CNew( CSkyDome );
		CChar temp[ MAX_NAME_SIZE];
		//if( m_dlgAddSkyDome->m_strSkyDomePurePath.IsEmpty() )
			Cpy( temp, m_dlgAddSkyDome->GetPath() );
		//else
		//	sprintf( temp, "%s%s.dds", g_pathProperties.m_skyPath, m_dlgAddSkyDome->GetPurePath() );
		g_skyDome = CNew( CSkyDome );
		g_skyDome->SetName( m_dlgAddSkyDome->GetName() );
		g_skyDome->SetPath( temp );
		g_skyDome->SetRadius( m_dlgAddSkyDome->GetRadius() );
		g_skyDome->SetPosition( m_dlgAddSkyDome->GetPos() );
		g_skyDome->SetDampening( m_dlgAddSkyDome->GetDampening() );
		g_skyDome->SetSides( m_dlgAddSkyDome->GetSides() );
		g_skyDome->SetSlices( m_dlgAddSkyDome->GetSlices() );
		g_skyDome->SetExponential( m_dlgAddSkyDome->GetExponential() );

		g_skyDome->Initialize();
		g_menu.m_insertAndShowSky = CTrue;
		InserItemToEngineObjectList( g_skyDome->GetName(), eENGINEOBJECTLIST_SKY );

		if( !Cmp( m_dlgAddSkyDome->GetName(), m_dlgAddSkyDome->GetTempName() ) )
			for( std::vector<std::string>::iterator it = g_engineObjectNames.begin(); it != g_engineObjectNames.end();it++ )
				if( Cmp( m_dlgAddSkyDome->GetTempName(), (*it).c_str() ) )
					(*it) = m_dlgAddSkyDome->GetName();

		GetMenu()->EnableMenuItem( ID_INSERT_SKYDOME, MF_DISABLED | MF_GRAYED );
		CDelete( m_dlgAddSkyDome );
		m_mainBtnSky.EnableWindow( FALSE );

		//save functions/////////////////////////////////
		for( CUInt index = 0; index < g_allVSceneNames.size(); index++ )
		{
			g_skyDome->AddVSceneToList( g_allVSceneNames[index], CTrue ); //Write to zip file and copy the textures
		}
		//save functions/////////////////////////////////


	}
	else if( result == IDCANCEL )
		CDelete( m_dlgAddSkyDome );

}

void CVandaEngine1Dlg::OnClose()
{
	//configuration
	CChar ConfigPath[MAX_NAME_SIZE];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, ConfigPath);
	if (result != S_OK)
	{
        PrintInfo( "\nCouldn't get the documents folder to write data", COLOR_RED );
	}
	else
	{
		Append( ConfigPath, "\\VandaRTI\\conf_editor.dat" );
	}

	FILE *filePtr;
	filePtr = fopen( ConfigPath, "wb" );
	fwrite( &g_options, sizeof( COptions ), 1, filePtr  );
	fclose(filePtr);
	////////////////

	if( g_scene.size() > 0 || g_engineLights.size() > 0 || g_engineWaters.size() > 0 || g_menu.m_insertAndShowSky || g_menu.m_insertAmbientSound || g_engineStaticSounds.size() > 0 )
	{
		CInt iResponse;
		iResponse = MessageBox( "Save scene?", "Warning" , MB_YESNOCANCEL |MB_ICONSTOP);
		if( iResponse == IDYES )
		{
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CFalse );
			OnMenuClickedSaveAs();
			if( g_multipleView->m_enableTimer )
				g_multipleView->EnableTimer( CTrue );

			PostQuitMessage( 0 );
		}
		else if( iResponse == IDNO )
			PostQuitMessage( 0 );
	}
	else
		PostQuitMessage( 0 );
	CDialog::OnClose();
}

//void CVandaEngine1Dlg::OnNMReturnListScenes(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

BOOL CVandaEngine1Dlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
            pMsg->wParam = VK_TAB;
    }
	if (NULL != m_pToolTip)
            m_pToolTip->RelayEvent(pMsg);
    return CDialog::PreTranslateMessage(pMsg);
}
void CVandaEngine1Dlg::OnBnClickedBtnNew()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedNew( CTrue ); //ask to see if we should proceed?
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnSave()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	OnMenuClickedSaveAs( CFalse );

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}


void CVandaEngine1Dlg::OnBnClickedBtnSaveas()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedSaveAs();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnOpen()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedOpen();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnCollada()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedImportCollada();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnPhysx()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedImportPhysX();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnAnimPrev()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	g_render.GetSelectedScene()->SetPrevAnimation();

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnAnimNext()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	g_render.GetSelectedScene()->SetNextAnimation();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnAnimPlay()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	g_render.GetSelectedScene()->m_animationStatus = eANIM_PLAY; 
	if( !g_render.GetSelectedScene()->UpdateAnimationLists() && g_render.GetSelectedScene()->GetNumClips() == 1)
	{
		g_render.GetSelectedScene()->SetClipIndex(0);
	}
	PrintInfo3( "\nAnimation '" + (CString) g_render.GetSelectedScene()->m_animationClips[g_render.GetSelectedScene()->GetCurrentClipIndex()]->GetName() + "'activated" );
	ex_pBtnPlayAnim->EnableWindow( FALSE );
	ex_pBtnPlayAnim->ShowWindow( SW_HIDE );
	ex_pBtnPauseAnim->EnableWindow( TRUE );
	ex_pBtnPauseAnim->ShowWindow( SW_SHOW );

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

}

BOOL CVandaEngine1Dlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor( LoadCursor( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDC_DEFAULT_ARROW ) ) );
    return TRUE;
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CVandaEngine1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
}

void CVandaEngine1Dlg::OnBnClickedBtnTimer()
{
	g_multipleView->m_enableTimer = !g_multipleView->m_enableTimer;
	g_multipleView->EnableTimer( g_multipleView->m_enableTimer );
	if( g_multipleView->m_enableTimer )
		PrintInfo3( "\nTimer Enabled" );
	else
		PrintInfo3( "\nTimer Disabled" );

	g_multipleView->SetElapsedTimeFromBeginning();

}

void CVandaEngine1Dlg::OnBnClickedBtnAnimPause()
{
	g_render.GetSelectedScene()->m_animationStatus = eANIM_PAUSE;
	ex_pBtnPlayAnim->EnableWindow( TRUE );
	ex_pBtnPlayAnim->ShowWindow( SW_SHOW );
	ex_pBtnPauseAnim->EnableWindow( FALSE );
	ex_pBtnPauseAnim->ShowWindow( SW_HIDE );
}

void CVandaEngine1Dlg::OnBnClickedBtnWeb()
{
	ShellExecute(NULL, "open", "http://www.vandaengine.com", NULL, NULL, SW_SHOWNORMAL);
	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnFacebook()
{
	ShellExecute(NULL, "open", "https://www.facebook.com/vandaengine", NULL, NULL, SW_SHOWNORMAL);
	g_multipleView->SetElapsedTimeFromBeginning();
}


void CVandaEngine1Dlg::OnBnClickedBtnMaterial()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedEditMaterial();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();

}

void CVandaEngine1Dlg::OnBnClickedBtnLight()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedInsertLight();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnWater()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedInsertWater();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnAmbientsound()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedInsertAmbientSound();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnStaticsound()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedInsertStaticSound();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnSky()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedInsertSkyDome();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnPlayer()
{
	g_characterPos = g_multipleView->m_nx->gCharacterPos;
	g_cameraPitchYawTilt.x = g_camera->m_perspectiveCameraPitch;
	g_cameraPitchYawTilt.y = g_camera->m_perspectiveCameraYaw;
	g_cameraPitchYawTilt.z = g_camera->m_perspectiveCameraTilt;
	PrintInfo( "\nPlayer set to the current position" );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnConsole()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	m_dlgConsole = CNew( CConsole );
	m_dlgConsole->DoModal();
	CDelete( m_dlgConsole );

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();

}

void CVandaEngine1Dlg::OnBnClickedBtnSummary()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	m_dlgWarnings = CNew( CWarnings );
	m_dlgWarnings->DoModal();
	CDelete( m_dlgWarnings);

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnLvnItemchangedListScenes(NMHDR *pNMHDR, LRESULT *pResult)
{
	//remove the current contents of the object list
	for (int nItem = m_listBoxObjects.GetItemCount()-1; nItem >= 0 ;nItem-- )
	{
		m_listBoxObjects.DeleteItem(nItem);
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nSelected = -1; 
	POSITION p = m_listBoxScenes.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxScenes.GetNextSelectedItem(p);
	}
	if( nSelected >= 0 )
	{
		m_btnRemoveScene.EnableWindow( TRUE );
		TCHAR szBuffer[1024];
		DWORD cchBuf(1024);
		LVITEM lvi;
		lvi.iItem = nSelected;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_listBoxScenes.GetItem(&lvi);
		
		m_objectListIndex = -1;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( Cmp( g_scene[i]->m_pureFileName, szBuffer ) )
			{
				if( g_scene[i]->m_hasAnimation )
					m_btnSceneProperties.EnableWindow( TRUE );

				for( CUInt j = 0; j < g_scene[i]->m_images.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_images[j]->GetName(), eOBJECTLIST_IMAGE);
				}
				for( CUInt j = 0; j < g_scene[i]->m_effects.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_effects[j]->GetName(), eOBJECTLIST_EFFECT);
				}
				for( CUInt j = 0; j < g_scene[i]->m_materials.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_materials[j]->GetName(), eOBJECTLIST_MATERIAL);
				}
				//animations
				for( CUInt j = 0; j < g_scene[i]->m_animations.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_animations[j]->GetName(), eOBJECTLIST_ANIMATION);
				}
				for( CUInt j = 0; j < g_scene[i]->m_animationClips.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_animationClips[j]->GetName(), eOBJECTLIST_ANIMATIONCLIP);
				}
				//lights
				for( CUInt j = 0; j < g_scene[i]->m_lights.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_lights[j]->GetName(), eOBJECTLIST_LIGHT);
				}
				//for( CUInt j = 0; j < g_scene[i]->m_lightInstances.size(); j++ )
				//{
				//	InserItemToObjectList( g_scene[i]->m_lightInstances[j]->GetName());
				//}
				//camera
				for( CUInt j = 0; j < g_scene[i]->m_cameras.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_cameras[j]->GetName(), eOBJECTLIST_CAMERA);
				}
				//for( CUInt j = 0; j < g_scene[i]->m_cameraInstances.size(); j++ )
				//{
				//	InserItemToObjectList( g_scene[i]->m_cameraInstances[j]->GetName());
				//}
				//geometry
				for( CUInt j = 0; j < g_scene[i]->m_geometries.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_geometries[j]->GetName(), eOBJECTLIST_MESH);
				}
				//for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
				//{
				//	InserItemToObjectList( g_scene[i]->m_instanceGeometries[j]->GetName());
				//}
				//controller
				for( CUInt j = 0; j < g_scene[i]->m_controllers.size(); j++ )
				{
					InserItemToObjectList( g_scene[i]->m_controllers[j]->GetName(), eOBJECTLIST_SKINCONTROLLER);
				}
			}
		}
	}
	else
	{
		m_btnRemoveScene.EnableWindow( FALSE );
		m_btnSceneProperties.EnableWindow( FALSE );
	}
}

void CVandaEngine1Dlg::OnLvnItemchangedListEngineObjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION p = m_listBoxEngineObjects.GetFirstSelectedItemPosition();
	CInt nSelected = -1;
	while(p)
	{
		nSelected = m_listBoxEngineObjects.GetNextSelectedItem(p);
	}
	if( nSelected >= 0 )
	{
		m_btnRemoveEngineObject.EnableWindow( TRUE );
		m_btnEngineObjectProperties.EnableWindow( TRUE );
	}
	else
	{
		m_btnRemoveEngineObject.EnableWindow( FALSE );
		m_btnEngineObjectProperties.EnableWindow( FALSE );
	}
}

void CVandaEngine1Dlg::ResetPhysX()
{
	g_multipleView->m_nx->m_nxActorNames.clear();
	g_multipleView->m_nx->m_nxActorTypes.clear();

	g_multipleView->m_nx->ReleaseCharacterControllers();

	for( CUInt i = 0; i < gPhysXscene->getNbActors(); i++ )
	{
		gPhysXscene->releaseActor( *gPhysXscene->getActors()[i] );
	}
	gPhysicsSDK->releaseScene( *gPhysXscene );
	gPhysXscene = NULL;

	g_multipleView->m_nx->gDefaultGravity.y = -7.8f;
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = g_multipleView->m_nx->gDefaultGravity;
	sceneDesc.simType = NX_SIMULATION_HW;

	gPhysXscene = gPhysicsSDK->createScene( sceneDesc );
	if( !gPhysXscene )
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		gPhysXscene = gPhysicsSDK->createScene(sceneDesc);
	}

	gPhysXscene->setGroupCollisionFlag( GROUP_COLLIDABLE_NON_PUSHABLE, GROUP_COLLIDABLE_PUSHABLE, CTrue );
	gPhysXscene->setGroupCollisionFlag( GROUP_COLLIDABLE_PUSHABLE, GROUP_COLLIDABLE_PUSHABLE, CTrue );
	gPhysXscene->setUserTriggerReport(&g_multipleView->m_nx->gTriggerReport);

	//NxMaterial* defaultMaterial = gPhysXscene->getMaterialFromIndex(0);
	//defaultMaterial->setRestitution(0.6f);
	//defaultMaterial->setStaticFriction(0.4f);
	//defaultMaterial->setDynamicFriction(0.4f);

	CFloat TimeStep = 1.0f / 60.0f;
	if (g_multipleView->m_nx->bFixedStep)	
		gPhysXscene->setTiming(TimeStep, 1, NX_TIMESTEP_FIXED);
	else
		gPhysXscene->setTiming(TimeStep, 1, NX_TIMESTEP_VARIABLE);

	g_multipleView->m_nx->InitCharacterControllers(g_multipleView->m_nx->gCharacterPos.x, g_multipleView->m_nx->gCharacterPos.y, g_multipleView->m_nx->gCharacterPos.z, 0.3f, 1.0f );
	g_multipleView->m_nx->SetScene( CFalse );

	OnMenuClickedPhysicsGravity();
}
void CVandaEngine1Dlg::OnBnClickedBtnRemovePhysx()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	if( !g_multipleView->m_nx->m_hasScene )
	{
		int nSelected = -1; 
		POSITION p = m_listBoxPhysXElements.GetFirstSelectedItemPosition();
		while(p)
		{
			nSelected = m_listBoxPhysXElements.GetNextSelectedItem(p);
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
			m_listBoxPhysXElements.GetItem(&lvi);

			CInt result = IDYES;
			CChar tempString[MAX_NAME_SIZE];
			sprintf( tempString, "Delete PhysX actor '%s' ?", szBuffer );
			result = MessageBox( tempString, "Warning", MB_YESNO | MB_ICONERROR );
			if( result == IDYES )
			{
				for( CUInt i = 0 ; i < g_scene.size(); i++ )
				{
					CBool foundTarget = CFalse;
					for( CUInt j = 0 ; j < g_scene[i]->m_instanceGeometries.size(); j++ )
					{
						if( g_scene[i]->m_instanceGeometries[j]->m_hasPhysX && Cmp( g_scene[i]->m_instanceGeometries[j]->m_physXName, szBuffer ) )
						{
							foundTarget = CTrue;
							if( gPhysXscene )
							{
								for( CUInt k = 0; k < gPhysXscene->getNbActors(); k++ )
								{
									CChar actorName[MAX_NAME_SIZE];

									if( !gPhysXscene->getActors()[k]->getName() ) continue;
									Cpy( actorName, gPhysXscene->getActors()[k]->getName() );

									if( Cmp( actorName, g_scene[i]->m_instanceGeometries[j]->m_physXName ) )
									{
										gPhysXscene->releaseActor( *gPhysXscene->getActors()[k] );
										g_scene[i]->m_instanceGeometries[j]->m_hasPhysX = CFalse;
										Cpy( g_scene[i]->m_instanceGeometries[j]->m_physXName, "\n" );
										PrintInfo( "\nActor removed successfully" );
										m_listBoxPhysXElements.DeleteItem(nSelected);
										SortPhysXList();
										if( g_scene[i]->m_instanceGeometries[j]->m_physXDensity > 0 )
										g_updateOctree = CTrue;

										break;
									}
								}

							}
							break;
						}
					}
					if( foundTarget )
						break;
				}
			}
		}
	}
	else
	{

		if( MessageBox( "Remove Current PhysX Scene?", "Vanda Engine 1 Warning", MB_YESNO | MB_ICONWARNING ) == IDYES )
		{
			CDelete(g_externalPhysX);
			ResetPhysX();
			m_physXElementListIndex = -1;
			for (int nItem = m_listBoxPhysXElements.GetItemCount()-1; nItem >= 0 ;nItem-- )
			{
				m_listBoxPhysXElements.DeleteItem(nItem);
			}
			m_btnRemovePhysX.EnableWindow( FALSE );
		}
	}

	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();

}

CVoid CVandaEngine1Dlg::OnMenuClickedGeneralAmbientColor()
{
	m_dlgGeneralAmbientColor = CNew( CGeneralAmbientColor );
	m_dlgGeneralAmbientColor->DoModal();
	CDelete(m_dlgGeneralAmbientColor );
}
void CVandaEngine1Dlg::OnBnClickedBtnTwitter()
{
	ShellExecute(NULL, "open", "https://www.twitter.com/vanda_engine", NULL, NULL, SW_SHOWNORMAL);
	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnYoutube()
{
	ShellExecute(NULL, "open", "https://www.youtube.com/channel/UCLTmJ3HgZuIoTSxyhD2UnkA", NULL, NULL, SW_SHOWNORMAL);
	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnLvnItemchangedListPhysxElements(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !g_multipleView->m_nx->m_hasScene )
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		POSITION p = m_listBoxPhysXElements.GetFirstSelectedItemPosition();
		CInt nSelected = -1;
		while(p)
		{
			nSelected = m_listBoxPhysXElements.GetNextSelectedItem(p);
		}
		if( nSelected >= 0 )
			m_btnRemovePhysX.EnableWindow( TRUE );
		else
			m_btnRemovePhysX.EnableWindow( FALSE );
	}
}

void CVandaEngine1Dlg::OnBnClickedBtnPublishSolution()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	m_dlgPublishProject = CNew(CPublishProject);
	CInt result = m_dlgPublishProject->DoModal();
	if( result == IDOK )
	{
		if( !Cmp(g_currentVSceneName, "\n") && m_dlgPublishProject->m_saveCurrentScene )
		{
			for( CUInt index = 0; index < g_allVSceneNamesToBePublished.size(); index++ )
			{
				CChar temp[MAX_NAME_SIZE];
				Cpy( temp, g_allVSceneNamesToBePublished[index].c_str() );
				if( Cmp( g_currentVSceneName, temp ) )
				{
					//found the target
					OnMenuClickedSaveAs( CFalse );
					break;
				}
			}
		}

		SetCapture();
		SetCursor( LoadCursorFromFile( "Assets/Engine/Icons/progress.ani") );

		CChar rootPath[MAX_NAME_SIZE];
		sprintf( rootPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\" );
		CreateWindowsDirectory( rootPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/DLLs/", rootPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Publish/", rootPath );
		CChar oldFileName[MAX_NAME_SIZE];
		sprintf( oldFileName, "%s%s", rootPath, "publish.exe" );
		CChar newFileName[MAX_NAME_SIZE];
		sprintf( newFileName, "%s%s%s", rootPath, m_dlgPublishProject->m_strName, ".exe" );

		rename(oldFileName, newFileName);

		CChar driversPath[MAX_NAME_SIZE];
		sprintf( driversPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Drivers\\" );
		CreateWindowsDirectory( driversPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Drivers/", driversPath );

		CChar assetPath[MAX_NAME_SIZE];
		sprintf( assetPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\" );
		CreateWindowsDirectory( assetPath );

		CChar logoPath[MAX_NAME_SIZE];
		sprintf( logoPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Logo\\" );
		CreateWindowsDirectory( logoPath );
		CChar tempFileName[MAX_NAME_SIZE];
		Cpy( tempFileName, m_dlgPublishProject->m_strSplash.GetBuffer(m_dlgPublishProject->m_strSplash.GetLength()) );
		m_dlgPublishProject->m_strSplash.ReleaseBuffer();
		CopyOneFileToDstDirectory( tempFileName, logoPath );
		CChar* afterPath = GetAfterPath( tempFileName );
		CChar fileToRename[MAX_NAME_SIZE];
		sprintf( fileToRename, "%s%s", logoPath, afterPath );
		CChar renamedFileName[MAX_NAME_SIZE];
		sprintf( renamedFileName, "%s%s", logoPath, "Logo.bmp" );
		rename( fileToRename, renamedFileName );

		CChar vScenesPath[MAX_NAME_SIZE];
		sprintf( vScenesPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\VScenes\\" );
		CreateWindowsDirectory( vScenesPath );

		CChar configPath[MAX_NAME_SIZE];
		sprintf( configPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Config\\" );
		CreateWindowsDirectory( configPath );
		CopyOneFileToDstDirectory( "Assets/Config/conf_win32.dat", configPath);

		CChar savePath[MAX_NAME_SIZE];
		sprintf( savePath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Save\\" );
		CreateWindowsDirectory( savePath );

		CChar savePathFile[MAX_NAME_SIZE];
		sprintf( savePathFile, "%s%s", savePath, "publish.txt" );
		FILE* VScenesFilePtr =  fopen( savePathFile, "wt" );

		if( !VScenesFilePtr )
		{
			MessageBox( "Couldn't open 'assets/save/publish.txt' to save data!", "Vanda Engine Error", MB_OK | MB_ICONERROR);
			return;
		}
		
		CChar nameWithoutDot[MAX_NAME_SIZE];
		Cpy( nameWithoutDot, g_firstSceneNameToBeLoaded.c_str() );
		GetWithoutDot( nameWithoutDot );

		CChar save[MAX_NAME_SIZE];
		sprintf( save, "%s%s%s", "file = Assets/VScenes/", nameWithoutDot, "\n" );
		std::string finalSave(save);
		fwrite( save, finalSave.length(), 1, VScenesFilePtr );
		fclose(VScenesFilePtr);


		CChar enginePath[MAX_NAME_SIZE];
		sprintf( enginePath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Engine\\" );
		CreateWindowsDirectory( enginePath );

		CChar shaderRootPath[MAX_NAME_SIZE];
		sprintf( shaderRootPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Engine\\Shaders\\" );
		CreateWindowsDirectory( shaderRootPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Shaders/", shaderRootPath );

		CChar dofShaderPath[MAX_NAME_SIZE];
		sprintf( dofShaderPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Engine\\Shaders\\DOF\\" );
		CreateWindowsDirectory( dofShaderPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Shaders/DOF/", dofShaderPath );

		CChar shadowShaderPath[MAX_NAME_SIZE];
		sprintf( shadowShaderPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Engine\\Shaders\\Shadow\\" );
		CreateWindowsDirectory( shadowShaderPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Shaders/Shadow/", shadowShaderPath );

		CChar spotShadowShaderPath[MAX_NAME_SIZE];
		sprintf( spotShadowShaderPath, "%s%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName, "\\Assets\\Engine\\Shaders\\Shadow\\Spot\\" );
		CreateWindowsDirectory( spotShadowShaderPath );
		CopyAllFilesFromSrcToDstDirectory( "Assets/Engine/Shaders/Shadow/Spot/", spotShadowShaderPath );

		//create all of scene the directories
		for( CUInt i = 0; i < g_allVSceneNamesToBePublished.size(); i++ )
		{
			CChar currentSceneNameWithoutDot[MAX_NAME_SIZE];
			Cpy( currentSceneNameWithoutDot, g_allVSceneNamesToBePublished[i].c_str() );
			GetWithoutDot( currentSceneNameWithoutDot );

			//original paths
			CChar originalVScenePath[MAX_NAME_SIZE];
			sprintf( originalVScenePath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/" );

			CChar originalExternalScenePath[MAX_NAME_SIZE];
			sprintf( originalExternalScenePath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/External Scenes/" );

			CChar originalExternalSceneTexturesPath[MAX_NAME_SIZE];
			sprintf( originalExternalSceneTexturesPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Textures/" );

			CChar originalWaterTexturesPath[MAX_NAME_SIZE];
			sprintf( originalWaterTexturesPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Waters/" );

			CChar originalSoundPath[MAX_NAME_SIZE];
			sprintf( originalSoundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/" );

			CChar originalAmbientSoundPath[MAX_NAME_SIZE];
			sprintf( originalAmbientSoundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/Ambient/" );

			CChar originalStaticSoundPath[MAX_NAME_SIZE];
			sprintf( originalStaticSoundPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sounds/Static/" );

			CChar originalExternalPhysicsPath[MAX_NAME_SIZE];
			sprintf( originalExternalPhysicsPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/External Physics/" );

			CChar originalSkyPath[MAX_NAME_SIZE];
			sprintf( originalSkyPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Sky/" );

			CChar originalBannerPath[MAX_NAME_SIZE];
			sprintf( originalBannerPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Banner/" );

			CChar originalScriptPath[MAX_NAME_SIZE];
			sprintf( originalScriptPath, "%s%s%s", g_VScenePath, currentSceneNameWithoutDot, "/Scripts/" );

			////////

			CChar VScenePath[MAX_NAME_SIZE];
			sprintf( VScenePath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\" );
			CreateWindowsDirectory( VScenePath );
			CopyAllFilesFromSrcToDstDirectory(originalVScenePath, VScenePath);

			CChar externalScenePath[MAX_NAME_SIZE];
			sprintf( externalScenePath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\External Scenes\\" );
			CreateWindowsDirectory( externalScenePath );
			CopyAllFilesFromSrcToDstDirectory(originalExternalScenePath, externalScenePath);

			CChar externalSceneTexturesPath[MAX_NAME_SIZE];
			sprintf( externalSceneTexturesPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Textures\\" );
			CreateWindowsDirectory( externalSceneTexturesPath );
			CopyAllFilesFromSrcToDstDirectory(originalExternalSceneTexturesPath, externalSceneTexturesPath);

			CChar waterTexturesPath[MAX_NAME_SIZE];
			sprintf( waterTexturesPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Waters\\" );
			CreateWindowsDirectory( waterTexturesPath );
			CopyAllFilesFromSrcToDstDirectory(originalWaterTexturesPath, waterTexturesPath);

			CChar soundPath[MAX_NAME_SIZE];
			sprintf( soundPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Sounds\\" );
			CreateWindowsDirectory( soundPath );

			CChar ambientSoundPath[MAX_NAME_SIZE];
			sprintf( ambientSoundPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Sounds\\Ambient\\" );
			CreateWindowsDirectory( ambientSoundPath );
			CopyAllFilesFromSrcToDstDirectory(originalAmbientSoundPath, ambientSoundPath);

			CChar staticSoundPath[MAX_NAME_SIZE];
			sprintf( staticSoundPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Sounds\\Static\\" );
			CreateWindowsDirectory( staticSoundPath );
			CopyAllFilesFromSrcToDstDirectory(originalStaticSoundPath, staticSoundPath);

			CChar externalPhysicsPath[MAX_NAME_SIZE];
			sprintf( externalPhysicsPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\External Physics\\" );
			CreateWindowsDirectory( externalPhysicsPath );
			CopyAllFilesFromSrcToDstDirectory(originalExternalPhysicsPath, externalPhysicsPath);

			CChar skyPath[MAX_NAME_SIZE];
			sprintf( skyPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Sky\\" );
			CreateWindowsDirectory( skyPath );
			CopyAllFilesFromSrcToDstDirectory(originalSkyPath, skyPath);

			CChar bannerPath[MAX_NAME_SIZE];
			sprintf( bannerPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Banner\\" );
			CreateWindowsDirectory( bannerPath );
			CopyAllFilesFromSrcToDstDirectory(originalBannerPath, bannerPath);

			CChar scriptPath[MAX_NAME_SIZE];
			sprintf( scriptPath, "%s%s%s%s", rootPath, "\\assets\\vscenes\\", currentSceneNameWithoutDot, "\\Scripts\\" );
			CreateWindowsDirectory( scriptPath );
			CopyAllFilesFromSrcToDstDirectory(originalScriptPath, scriptPath);

		}
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s%s%s", "Project published to '", m_dlgPublishProject->m_strDestination,"\\", m_dlgPublishProject->m_strName, "' successfully");
		PrintInfo(temp);

		CChar openShell[MAX_NAME_SIZE];
		sprintf( openShell, "%s%s%s", m_dlgPublishProject->m_strDestination, "\\", m_dlgPublishProject->m_strName );
		ShellExecute( NULL, "open", openShell, NULL, NULL, SW_SHOWNORMAL );
		CDelete(m_dlgPublishProject);
		ReleaseCapture();
	}
	g_allVSceneNamesToBePublished.clear();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();
}

void CVandaEngine1Dlg::OnBnClickedBtnScriptManager()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );

	if( g_vandaDemo )
	{
		MessageBox( "This button opens the script manager that lets you load another scene or activate an animation while colliding the appropriate triggers.\nThis feature is not available in RTI demo Edition Version 1.x", "Vanda Engine Error", MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		m_dlgAddScript->DoModal();
	}
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );

	g_multipleView->SetElapsedTimeFromBeginning();

}

void CVandaEngine1Dlg::OnBnClickedBtnCameraRenderingManager()
{
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CFalse );
	OnMenuClickedSelectCamera();
	if( g_multipleView->m_enableTimer )
		g_multipleView->EnableTimer( CTrue );
	g_multipleView->SetElapsedTimeFromBeginning();
	g_multipleView->RenderWindow();
}

void CVandaEngine1Dlg::OnBnClickedBtnSceneProperties()
{
	int nSelected = -1; 
	POSITION p = m_listBoxScenes.GetFirstSelectedItemPosition();
	while(p)
	{
		nSelected = m_listBoxScenes.GetNextSelectedItem(p);
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
		m_listBoxScenes.GetItem(&lvi);
		CScene* scene = NULL;
		for( CUInt i = 0; i < g_scene.size(); i++ )
		{
			if( Cmp( szBuffer, g_scene[i]->GetName() ) )
			{
				scene = g_scene[i];
			}
		}
		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CFalse );

		CSceneProperties* m_dlgSceneProperties = CNew (CSceneProperties);
		m_dlgSceneProperties->SetScene( scene );
		m_dlgSceneProperties->DoModal();
		CDelete( m_dlgSceneProperties );

		if( g_multipleView->m_enableTimer )
			g_multipleView->EnableTimer( CTrue );
		g_multipleView->SetElapsedTimeFromBeginning();
		g_multipleView->RenderWindow();
	}
}
