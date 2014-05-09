//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

/******************************************************
***************VandaEngine MAIN FILE***************
******Engine programmer: Ehsan Kamrani.****************
******************************************************/

/*******************************************************
*******************MAIN INCLUDES************************
*******************************************************/
#define NOMINMAX
#include <windows.h>
#include "stdafx.h"
#include "VandaEngine1Win32.h"
#include "main.h"
#include "inputEngine/input.h"
#include "timerEngine/timer.h"
#include "graphicsEngine/render.h"
#include "graphicsEngine/image.h"
#include "graphicsEngine/texture.h"
#include "main.h"

class COpenALSoundBuffer;
//Edition.MaxVersion.MinVersion.BugFixes;
//RTI.1.0.0
CInt g_maxVersion;
CInt g_minVersion;
CInt g_bugFixesVersion;
CChar g_edition[MAX_NAME_SIZE];
CScene* g_currentScene = NULL;
CChar g_currentVSceneName[MAX_NAME_SIZE]; //save functions
CChar g_currentVSceneNameViaScript[MAX_NAME_SIZE];
CBool g_loadSceneViaScript = CFalse;
std::vector<COpenALSoundBuffer*>g_soundBuffers;
CChar g_currentPassword[MAX_NAME_SIZE];
CWindow g_window; 
CRender g_render; //extern
std::vector<CScene*> g_scene;
std::vector<CInstanceLight*> g_engineLights;
std::vector<CWater*> g_engineWaters;
std::vector<CImage*> g_images;
std::vector<CImage*>g_waterImages;
std::vector<CStaticSound*>g_engineStaticSounds;
CBool g_useOldRenderingStyle = CFalse;
CUpdateCamera *g_camera;
CSkyDome *g_skyDome = NULL;
CBloom* g_bloom = NULL;
COctree* g_octree;
CBool g_updateOctree = CTrue;
CMain* g_main = NULL;
CBool g_useGlobalAmbientColor;
CColor4f g_globalAmbientColor;
CLuaState g_lua;
COpenALSystem* g_soundSystem = NULL;
std::vector<CInstanceCamera*> g_cameraInstances;
GLuint g_shaderType;
//shadow
CShadowMap* g_dynamicShadowMap;
/////////
CInt g_numLights = 0;
CInt g_totalLights = 0;
CFloat g_elapsedTime;

bool g_fullScreen = true;             //Use Fullscreen
int g_width = 1024;                   //default g_width of our screen
int g_height = 768;                   //default Height of our screen
int g_bits = 32;                      //Color Bits
bool g_renderScene = false;
bool g_loading = CTrue;
CInputSystem g_input;
CTimer* g_timer = NULL;
CNovodex* g_nx = NULL;
CFont* g_font = NULL;

CImage* m_loadingImg;
CChar m_strLoadingImg[MAX_NAME_SIZE];

CDatabaseVariables g_databaseVariables;
CShadowProperties g_shadowProperties;
CDOFProperties g_dofProperties;
CLightProperties g_lightProperties;
CFogProperties g_fogProperties;
CBloomProperties g_bloomProperties;
CPathProperties g_pathProperties;
CExtraTexturesNamingConventions g_extraTexturesNamingConventions;
COptions g_options;
CBool g_fogBlurPass = CFalse;
CVec4f g_defaultDirectionalLight;
CInstanceLight* g_currentInstanceLight = NULL;
CBool g_firstPass;
CInt g_sceneManagerObjectsPerSplit = 15;

HWND g_hwnd = NULL;
HINSTANCE g_instance = NULL;
bool g_done;       // flag saying when our app is complete 
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

void GenerateLoadingTexture( char* fileName )
{
	m_loadingImg = CNew( CImage );

	Cpy( m_strLoadingImg, fileName );

	if(!CTexture::LoadDDSTexture( m_loadingImg, m_strLoadingImg, NULL ) )
		MessageBox( NULL, _T("GenerateLoadingTexture>Couldn't load the texture"), _T("VandaEngine Error"), MB_OK );

	m_loadingImg->SetFileName( GetAfterPath(m_strLoadingImg) );
}
 
void DeleteLoadingTexture()
{
	CDelete(m_loadingImg);
}

void ShowLoadingScene()
{
	glPushAttrib( GL_ENABLE_BIT );
	glUseProgram(0);
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
	glViewport( 0, 0, g_width, g_height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, 1, 0, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_loadingImg->GetId() );
	glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex2f( 0.0f, 0.0f );
	glTexCoord2f( 1.0f, 0.0f );
	glVertex2f( 1.0f, 0.0f );
	glTexCoord2f( 1.0f, 1.0f );
	glVertex2f( 1.0f, 1.0f );
	glTexCoord2f( 0.0f, 1.0f );
	glVertex2f( 0.0f, 1.0f );
	glEnd();
	glPopAttrib();
}

GLvoid CleanUp()
{
	g_main->Release();
	CDelete( g_main );
	DeleteLoadingTexture();
	//configuration
	FILE *filePtr;
	filePtr = fopen( "Assets/config/conf_win32.dat", "wb" );
	fwrite( &g_options, sizeof( COptions ), 1, filePtr  );
	fclose(filePtr);
	////////////////

}

int Initialize()
{
	g_main = CNew( CMain );
	g_main->Init();
	return 1;
}

bool Render()
{
	if( g_loading ) 
	{
		CChar temp[MAX_NAME_SIZE];
		CChar afterPath[MAX_NAME_SIZE];
		CChar finalPath[MAX_NAME_SIZE];

		if( !g_render.ParsePublishFile( "Assets/Save/publish.txt" ) )
		{
			MessageBox( NULL, _T("Couldn't load the publish file\nLoading 'Assets/save/VandaEngineDemo.vin'..."), _T("VandaEngineError"), MB_OK | MB_ICONERROR );
			Cpy( temp, "Assets/Save/VandaEngineDemo.vin" );
		}
		else
		{
			Cpy( finalPath, g_render.m_savedFile.c_str() );
			CChar* afterPathTemp = GetAfterPath(finalPath);
			Cpy( afterPath, afterPathTemp );
			Append( afterPath, ".vin" );
			Append( finalPath, "/" );
			Append( finalPath, afterPath );
			Cpy( temp, finalPath);
		}
		//Find the name of loading banner
		CChar currentSceneNameWithoutDot[MAX_NAME_SIZE];
		Cpy( currentSceneNameWithoutDot, afterPath );
		GetWithoutDot( currentSceneNameWithoutDot );

		CChar bannerPath[MAX_NAME_SIZE];
		sprintf( bannerPath, "%s%s%s", "assets/vscenes/", currentSceneNameWithoutDot, "/Banner/" );

		HANDLE hFind;
		WIN32_FIND_DATAA data;
		CChar bannerTempPath[MAX_NAME_SIZE];
		sprintf( bannerTempPath, "%s%s", bannerPath, "*.*" );
		hFind = FindFirstFileA( bannerTempPath, &data );
		CChar bannerNameAndPath[MAX_NAME_SIZE];
		do
		{
		sprintf( bannerNameAndPath, "%s%s", bannerPath, data.cFileName );
		if( !Cmp(data.cFileName, "." ) && !Cmp(data.cFileName, ".." ) )
			break;
		}while (FindNextFileA( hFind, &data));
		FindClose(hFind);
		GenerateLoadingTexture( bannerNameAndPath );

		ShowLoadingScene();
		SwapBuffers(g_window.m_windowGL.hDC);
		DeleteLoadingTexture();

		g_main->Load( temp );
		g_loading = CFalse;
	}
	else if( g_loadSceneViaScript )
	{
		g_main->m_loadScene = CTrue;

		//find the scene
		GetWithoutDot(g_currentVSceneNameViaScript);

		CChar bannerPath[MAX_NAME_SIZE];
		sprintf( bannerPath, "%s%s%s", "assets/vscenes/", g_currentVSceneNameViaScript, "/Banner/" );

		HANDLE hFind;
		WIN32_FIND_DATAA data;
		CChar bannerTempPath[MAX_NAME_SIZE];
		sprintf( bannerTempPath, "%s%s", bannerPath, "*.*" );
		hFind = FindFirstFileA( bannerTempPath, &data );
		CChar bannerNameAndPath[MAX_NAME_SIZE];
		do
		{
		sprintf( bannerNameAndPath, "%s%s", bannerPath, data.cFileName );
		if( !Cmp(data.cFileName, "." ) && !Cmp(data.cFileName, ".." ) )
			break;
		}while (FindNextFileA( hFind, &data));
		FindClose(hFind);
		GenerateLoadingTexture( bannerNameAndPath );

		glUseProgram(0);
		glDrawBuffer( GL_BACK );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		ShowLoadingScene();
		glFlush();
		SwapBuffers( g_window.m_windowGL.hDC );
		DeleteLoadingTexture();
		g_main->Reset();

		CChar RTIPath[MAX_NAME_SIZE];
		sprintf( RTIPath, "%s%s%s%s%s", "Assets\\VScenes\\", g_currentVSceneNameViaScript, "\\", g_currentVSceneNameViaScript, ".vin" );
		g_main->Load(RTIPath);
		g_main->m_loadScene = CFalse;
		g_loadSceneViaScript = CFalse;
	}
	else
	{
		if( !g_main->Render() ) return CFalse;
	}
	g_main->ResetPhysXCounts();
	SwapBuffers( g_window.m_windowGL.hDC );

	return CTrue;
}

// the Windows Procedure event handlers
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_SYSCOMMAND:						// Intercept System Commands
		{
			switch (wParam)						// Check System Calls
			{
				case SC_SCREENSAVE:				// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;
			}
			break;							// Exit
		}

		case WM_CLOSE:					// windows is closing

			// send WM_QUIT to message queue
			PostQuitMessage(0);
			return 0;
			break;

		case WM_DESTROY:
			g_window.DestroyWindowGL( &g_window.m_windowGL );
			PostQuitMessage(0);
			return 0;
			break;

		case WM_SIZE:
			g_window.ReshapeGL(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;
			break;

		default:
			break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}

LRESULT CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hBitmap;
	hBitmap = LoadImage(NULL, _T("Assets/Logo/Logo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HWND hPic = GetDlgItem(hwnd,IDC_STATIC_LOGO);

	switch( message )
	{
	case WM_INITDIALOG:
		SendMessage(hPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap)); 
		switch( g_options.m_width )
		{
		case 800:
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_800 );
			break;
		case 1024:
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_1024 );
			break;
		case 1280:
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_1280 );
			break;
		case 1440:
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_1440 );
			break;
		case 1680:
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_1680 );
			break;
		default:
			DEVMODE dmScreenSettings;												// device mode
			EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);	
			g_options.m_width = dmScreenSettings.dmPelsWidth;		// screen g_width
			g_options.m_height = dmScreenSettings.dmPelsHeight;		// screen g_height
            CheckRadioButton( hwnd, IDC_RES_CURRENT, IDC_RES_1680, IDC_RES_CURRENT );

			break;
		}
   		//load  multisampling from the config file
     
		switch( g_options.m_numSamples )
		{
		case 0:
			CheckRadioButton( hwnd, IDC_MSAMPLE_NO, IDC_MSAMPLE_16, IDC_MSAMPLE_NO );
			break;
		case 2:
			CheckRadioButton( hwnd, IDC_MSAMPLE_NO, IDC_MSAMPLE_16, IDC_MSAMPLE_2 );
			break;
		case 4:
			CheckRadioButton( hwnd, IDC_MSAMPLE_NO, IDC_MSAMPLE_16, IDC_MSAMPLE_4);
			break;
		case 8:
			CheckRadioButton( hwnd, IDC_MSAMPLE_NO, IDC_MSAMPLE_16, IDC_MSAMPLE_8 );
			break;
		case 16:
			CheckRadioButton( hwnd, IDC_MSAMPLE_NO, IDC_MSAMPLE_16, IDC_MSAMPLE_16 );
			break;
		}

   		//load  anisotropic filtering from the config file
		switch( g_options.m_anisotropy )
		{
		case 0:
			CheckRadioButton( hwnd, IDC_ISOTROP_NO, IDC_ISOTROP_16, IDC_ISOTROP_NO );
			break;
		case 2:
			CheckRadioButton( hwnd, IDC_ISOTROP_NO, IDC_ISOTROP_16, IDC_ISOTROP_2 );
			break;
		case 4:
			CheckRadioButton( hwnd, IDC_ISOTROP_NO, IDC_ISOTROP_16, IDC_ISOTROP_4 );
			break;
		case 8:
			CheckRadioButton( hwnd, IDC_ISOTROP_NO, IDC_ISOTROP_16, IDC_ISOTROP_8 );
			break;
		case 16:
			CheckRadioButton( hwnd, IDC_ISOTROP_NO, IDC_ISOTROP_16, IDC_ISOTROP_16 );
			break;
		}

   		//load  VSync data from the config file
		if( g_options.m_disableVSync == CTrue )
		{
			SendDlgItemMessage (hwnd, IDC_DVSYNC, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{
			SendDlgItemMessage (hwnd, IDC_DVSYNC, BM_SETCHECK, BST_UNCHECKED, 0);
		}

   		//load  FBO data from the config file
		if( g_options.m_enableFBO == CTrue )
		{
			SendDlgItemMessage (hwnd, IDC_FBO, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{
			SendDlgItemMessage (hwnd, IDC_FBO, BM_SETCHECK, BST_UNCHECKED, 0);
		}

   		//load  VBO data from the config file
		if( g_options.m_enableVBO == CTrue )
		{
			SendDlgItemMessage (hwnd, IDC_VBO, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{
			SendDlgItemMessage (hwnd, IDC_VBO, BM_SETCHECK, BST_UNCHECKED, 0);
		}

		break; //case WM_INIT

	case WM_COMMAND:
		{
		if( wParam == IDCANCEL )
			EndDialog( hwnd, 0 );
		else if( wParam == IDOK )
			EndDialog( hwnd, 1 );
		else if( wParam == IDC_RES_CURRENT )
		{
			DEVMODE dmScreenSettings;					// device mode
			EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);	
			g_options.m_width = dmScreenSettings.dmPelsWidth;		// screen g_width
			g_options.m_height = dmScreenSettings.dmPelsHeight;		// screen g_height
		}
		else if( wParam == IDC_RES_800 )
		{
			g_options.m_width = 800;
			g_options.m_height = 600;
		}
		else if( wParam == IDC_RES_1024 )
		{
			g_options.m_width = 1024;
			g_options.m_height = 768;
		}
		else if( wParam == IDC_RES_1280 )
		{
			g_options.m_width = 1280;
			g_options.m_height = 720;
		}
		else if( wParam == IDC_RES_1440 )
		{
			g_options.m_width = 1440;
			g_options.m_height = 900;
		}
		else if( wParam == IDC_RES_1680 )
		{
			g_options.m_width = 1680;
			g_options.m_height = 1050;
		}

		//multisampling
		else if( wParam == IDC_MSAMPLE_NO )
			g_options.m_numSamples = 0;
		else if( wParam == IDC_MSAMPLE_2 )
			g_options.m_numSamples = 2;
		else if( wParam == IDC_MSAMPLE_4 )
			g_options.m_numSamples = 4;
		else if( wParam == IDC_MSAMPLE_8 )
			g_options.m_numSamples = 8;
		else if( wParam == IDC_MSAMPLE_16 )
			g_options.m_numSamples = 16;

		//Anisotropic filtering
		else if( wParam == IDC_ISOTROP_NO )
			g_options.m_anisotropy = 0;
		else if( wParam == IDC_ISOTROP_2 )
			g_options.m_anisotropy = 2;
		else if( wParam == IDC_ISOTROP_4 )
			g_options.m_anisotropy = 4;
		else if( wParam == IDC_ISOTROP_8 )
			g_options.m_anisotropy = 8;
		else if( wParam == IDC_ISOTROP_16 )
			g_options.m_anisotropy = 16;

		//VSync
		if (SendDlgItemMessage (hwnd, IDC_DVSYNC, BM_GETCHECK, 0, 0) == BST_CHECKED)
			g_options.m_disableVSync = CTrue;
		else if( SendDlgItemMessage (hwnd, IDC_DVSYNC, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			g_options.m_disableVSync = CFalse;

		//FBO
		if (SendDlgItemMessage (hwnd, IDC_FBO, BM_GETCHECK, 0, 0) == BST_CHECKED)
			g_options.m_enableFBO = CTrue;
		else if( SendDlgItemMessage (hwnd, IDC_FBO, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			g_options.m_enableFBO = CFalse;

		//VBO
		if (SendDlgItemMessage (hwnd, IDC_VBO, BM_GETCHECK, 0, 0) == BST_CHECKED)
			g_options.m_enableVBO = CTrue;
		else if( SendDlgItemMessage (hwnd, IDC_VBO, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			g_options.m_enableVBO = CFalse;
		//if (SendDlgItemMessage (hwnd, IDC_SHOWDLG, BM_GETCHECK, 0, 0) == BST_CHECKED)
		//	g_options.m_showStartupDialog = CTrue;
		//else if (SendDlgItemMessage (hwnd, IDC_SHOWDLG, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
		//	g_options.m_showStartupDialog = CFalse;


		}break;
	}
	return 0;
}


// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_instance = GetModuleHandle( NULL );
	MSG	msg;											// Window Message Structure

	//configuration
	FILE *filePtr;
	filePtr = fopen( "Assets/config/conf_win32.dat", "rb" );
	if( !filePtr )
	{
		filePtr =  fopen( "Assets/config/conf_win32.dat", "wb" );
		fwrite( &g_options, sizeof( COptions ), 1, filePtr  );
	}
	else
	{
		fread( &g_options , sizeof( COptions ), 1, filePtr  );
	}
	fclose(filePtr);
	////////////////

	if( !DialogBox( hInstance, MAKEINTRESOURCE( IDD_INIT_DIALOG ), NULL, ( DLGPROC )DlgProc ) )
		return 0;

	g_width = g_options.m_width;
	g_height = g_options.m_height;
	// Fill Out Window
	ZeroMemory (&g_window.m_windowGL, sizeof (CWindowGL));		// Make Sure Memory Is Zeroed
	g_window.m_windowGL.init.className		= _T("ZZGame");
	g_window.m_windowGL.init.hInstance		= g_instance;
	g_window.m_windowGL.init.title			= _T("VandaEngine ");						// Window Title
	g_window.m_windowGL.init.width			= g_width;								// Window Width
	g_window.m_windowGL.init.height			= g_height;								// Window Height
	g_window.m_windowGL.init.bitsPerPixel	= g_bits;									// Bits Per Pixel
	g_window.m_windowGL.init.isFullScreen	= TRUE;									// Fullscreen? (Set To TRUE)
	g_window.m_windowGL.init.testWindow     = FALSE;
	g_window.m_windowGL.init.windowProc		= WndProc;
	g_window.m_windowGL.multiSampling		= false;
    
	if(!g_window.CreateWindowGL( &g_window.m_windowGL ) )
	{
		MessageBox( NULL, _T("Couldn't create the OpenGL window"), _T("VandaEngine Error"),  MB_OK | MB_ICONINFORMATION );
		return false;
	}

	GLenum err = glewInit();
	if( err != GLEW_OK )
		MessageBox( NULL, _T("Couldn't initialize GLEW"), _T("VandaEngine Error"), MB_OK | MB_ICONINFORMATION );
	if( !GLEW_EXT_framebuffer_object || !GLEW_ARB_texture_non_power_of_two || !g_options.m_enableFBO)
	{
		MessageBox( NULL, _T("Switching to old rendering style."), _T("VandaEngine Warning"), MB_OK | MB_ICONINFORMATION );
		g_useOldRenderingStyle = CTrue;
	}

	switch( g_options.m_numSamples )
	{
	case 2:
		g_window.m_numSamples = 2;
		break;
	case 4:
		g_window.m_numSamples = 4;
		break;
	case 8:
		g_window.m_numSamples = 8;
		break;
	case 16:
		g_window.m_numSamples = 16;
		break;
	default:
		g_window.m_numSamples = 0;
	}
	if( g_window.m_numSamples > 1 && !g_useOldRenderingStyle )
	{
		if( !GLEW_EXT_framebuffer_multisample )
		{
			MessageBox( NULL, _T("GL_EXT_framebuffer_multisample is not supported\nSwitching to old rendering style"), _T("VandaEngine Error"), MB_OK | MB_ICONINFORMATION );
			g_useOldRenderingStyle = CTrue; //See if we can we use multisampling with old rendering style?
		}
		else
			g_window.m_windowGL.multiSampling = true; //use new rendering style with multisampling
	}

	if( g_window.m_numSamples > 1 && g_useOldRenderingStyle && !WGLEW_ARB_multisample  )
			MessageBox( NULL, _T("Your implementation doesn't support multisampling"), _T("VandaEngine Error"),  MB_OK | MB_ICONINFORMATION );
	if( g_window.m_numSamples > 1 && WGLEW_ARB_multisample && g_useOldRenderingStyle )
	{
		if( ! g_window.DestroyWindowGL( &g_window.m_windowGL ) )
		{
			MessageBox( NULL, _T("Couldn't destroy the OpenGL window to use multisampling"), _T("VandaEngine Error"),  MB_OK | MB_ICONINFORMATION );
			return false;
		}
		g_window.m_windowGL.multiSampling = true;
		if(!g_window.CreateWindowGL( &g_window.m_windowGL ) )
		{
			MessageBox( NULL, _T("Couldn't create the OpenGL window with multisampling\n please disable multisampling"), _T("VandaEngine Error"),  MB_OK | MB_ICONINFORMATION );
			return false;
		}
		if( g_window.m_windowGL.multiSampling && GLEW_NV_multisample_filter_hint )
		{
			glHint(GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST);
			glEnable( GL_MULTISAMPLE );
		}
	}

	if( WGLEW_EXT_swap_control && g_options.m_disableVSync )
		wglSwapIntervalEXT(0);
	else
		wglSwapIntervalEXT(1);

	//HANDLE hIcon = LoadImage(g_window.m_windowGL.init.hInstance, _T("pass of ico"), IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
	//if (hIcon) {
	//	//Change both icons to the same icon handle.
	//	SendMessage(g_window.m_windowGL.hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	//	SendMessage(g_window.m_windowGL.hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	//	//This will ensure that the application icon gets changed too.
	//	SendMessage(GetWindow(g_window.m_windowGL.hWnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	//	SendMessage(GetWindow(g_window.m_windowGL.hWnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	//}	

	if( !Initialize() )
		return 0;
    
	// update the window
	// main message loop
	while (!g_done)
	{
		PeekMessage(&msg, g_hwnd, NULL, NULL, PM_REMOVE);

		TranslateMessage(&msg);		// translate and dispatch to event queue
		DispatchMessage(&msg);

		if( !Render() ) g_done = true;

	}
	CleanUp();
	ShowCursor (TRUE);
	return (int)msg.wParam;
}