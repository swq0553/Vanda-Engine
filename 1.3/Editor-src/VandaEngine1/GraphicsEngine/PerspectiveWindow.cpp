//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// PerspectiveWindow.cpp : implementation file
//

#include "stdafx.h"
#include "PerspectiveWindow.h"
#include "opengl.h"
#include "render.h"
#include "scene.h"
#include "fog.h"
#include "..\VandaEngine1.h"
#include "..\VandaEngine1Dlg.h"
#include "Animation.h"
// CMultipleWindows
CInt g_numLights = 0;
CInt g_totalLights = 0;
CBool g_fogBlurPass = CFalse;
IMPLEMENT_DYNAMIC(CMultipleWindows, CWnd)

//Vanda C Functions to be used in Lua scripts
CInt PlaySound(lua_State *L)
{
	if( g_testScript )
		return 0;
	int argc = lua_gettop(L);

	CBool setLoop = CFalse;
	for ( int n=1; n<=argc; ++n )
	{
		if( Cmp( "VANDA_LOOP",  lua_tostring(L, n) ) )
		{
			setLoop = CTrue;
			break;
		}
	}

	for ( int n=1; n<=argc; ++n )
	{
		for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
		{
			if( Cmp( g_engineStaticSounds[i]->GetName(),  lua_tostring(L, n) ) )
			{
				g_engineStaticSounds[i]->m_source->SetLooping( setLoop );
				g_engineStaticSounds[i]->SetLoop( setLoop );
				g_engineStaticSounds[i]->SetPlay(CTrue);
				g_soundSystem->PlayALSound( *(g_engineStaticSounds[i]->m_source) );
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt PauseSound(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);

	for ( int n=1; n<=argc; ++n )
	{
		for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
		{
			if( Cmp( g_engineStaticSounds[i]->GetName(),  lua_tostring(L, n) ) )
			{
				g_engineStaticSounds[i]->SetPlay(CFalse);
				g_soundSystem->PauseALSound( *(g_engineStaticSounds[i]->m_source) );
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt StopSound(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);

	for ( int n=1; n<=argc; ++n )
	{
		for( CUInt i = 0 ; i < g_engineStaticSounds.size(); i++ )
		{
			if( Cmp( g_engineStaticSounds[i]->GetName(),  lua_tostring(L, n) ) )
			{
				g_engineStaticSounds[i]->SetPlay(CFalse);
				g_soundSystem->StopALSound( *(g_engineStaticSounds[i]->m_source) );
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt BlendCycle(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 4 )
	{
		PrintInfo( "\nPlease specify 4 arguments for BlendCycle()", COLOR_RED );
		return 0;
	}
	CScene* scene = NULL;

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		CChar sceneName[MAX_NAME_SIZE];
		Cpy( sceneName, g_scene[i]->GetName() );
		StringToUpper( sceneName );
		if( Cmp(sceneName ,  luaToString ) )
		{
			g_scene[i]->m_animationStatus = eANIM_PLAY;
			scene = g_scene[i];
			break;
		}
	}

	if( !scene )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the scene name '", luaToString, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy( luaToString2, lua_tostring(L, 2) );
	StringToUpper( luaToString2 );
	CBool foundTarget = CFalse;
	CInt index;
	for( CInt i = 0; i < scene->GetNumClips(); i++ )
	{
		CChar animationName[MAX_NAME_SIZE];
		Cpy(animationName, scene->m_animationClips[i]->GetName() );
		StringToUpper( animationName );
		if( Cmp( luaToString2, animationName ) )
		{
			index = i;
			foundTarget = CTrue;
			break;
		}
	}
	if( !foundTarget )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}
	scene->BlendCycle( index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber( L, 4 ) );
	return 0;
}

CInt ClearCycle(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 3 )
	{
		PrintInfo( "\nPlease specify 3 arguments for ClearCycle()", COLOR_RED );
		return 0;
	}
	CScene* scene = NULL;

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		CChar sceneName[MAX_NAME_SIZE];
		Cpy( sceneName, g_scene[i]->GetName() );
		StringToUpper( sceneName );
		if( Cmp(sceneName ,  luaToString ) )
		{
			g_scene[i]->m_animationStatus = eANIM_PLAY;
			scene = g_scene[i];
			break;
		}
	}

	if( !scene )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the scene name '", luaToString, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy( luaToString2, lua_tostring(L, 2) );
	StringToUpper( luaToString2 );
	CBool foundTarget = CFalse;
	CInt index;
	for( CInt i = 0; i < scene->GetNumClips(); i++ )
	{
		CChar animationName[MAX_NAME_SIZE];
		Cpy(animationName, scene->m_animationClips[i]->GetName() );
		StringToUpper( animationName );
		if( Cmp( luaToString2, animationName ) )
		{
			index = i;
			g_scene[i]->m_animationStatus = eANIM_PLAY;
			foundTarget = CTrue;
			break;
		}
	}
	if( !foundTarget )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}
	scene->ClearCycle( index, (CFloat)lua_tonumber(L, 3) );
	return 0;
}

CInt ExecuteAction(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 4 )
	{
		PrintInfo( "\nPlease specify at least 4 arguments for ExecuteAction()", COLOR_RED );
		return 0;
	}
	CScene* scene = NULL;

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		CChar sceneName[MAX_NAME_SIZE];
		Cpy( sceneName, g_scene[i]->GetName() );
		StringToUpper( sceneName );
		if( Cmp(sceneName ,  luaToString ) )
		{
			g_scene[i]->m_animationStatus = eANIM_PLAY;
			scene = g_scene[i];
			break;
		}
	}

	if( !scene )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the scene name '", luaToString, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy( luaToString2, lua_tostring(L, 2) );
	StringToUpper( luaToString2 );
	CBool foundTarget = CFalse;
	CInt index;
	for( CInt i = 0; i < scene->GetNumClips(); i++ )
	{
		CChar animationName[MAX_NAME_SIZE];
		Cpy(animationName, scene->m_animationClips[i]->GetName() );
		StringToUpper( animationName );
		if( Cmp( luaToString2, animationName ) )
		{
			index = i;
			foundTarget = CTrue;
			break;
		}
	}
	if( !foundTarget )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}
	CFloat weight = 1.0f;
	CBool lock = CFalse;
	if( argc > 4 )
		weight = lua_tonumber(L,5);
	if( argc > 5 )
		lock = (CBool)lua_toboolean(L, 6);
	scene->ExecuteAction( index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L,4), weight, lock);
	return 0;
}

CInt ReverseExecuteAction(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 2 )
	{
		PrintInfo( "\nPlease specify 2 arguments for ReverseExecuteAction()", COLOR_RED );
		return 0;
	}
	CScene* scene = NULL;

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );
	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		CChar sceneName[MAX_NAME_SIZE];
		Cpy( sceneName, g_scene[i]->GetName() );
		StringToUpper( sceneName );
		if( Cmp(sceneName ,  luaToString ) )
		{
			g_scene[i]->m_animationStatus = eANIM_PLAY;
			scene = g_scene[i];
			break;
		}
	}

	if( !scene )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the scene name '", luaToString, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy( luaToString2, lua_tostring(L, 2) );
	StringToUpper( luaToString2 );
	CBool foundTarget = CFalse;
	CInt index;
	for( CInt i = 0; i < scene->GetNumClips(); i++ )
	{
		CChar animationName[MAX_NAME_SIZE];
		Cpy(animationName, scene->m_animationClips[i]->GetName() );
		StringToUpper( animationName );
		if( Cmp( luaToString2, animationName ) )
		{
			index = i;
			foundTarget = CTrue;
			break;
		}
	}
	if( !foundTarget )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'" );
		PrintInfo( temp, COLOR_RED );
		return 0;
	}
	scene->ReverseExecuteAction( index );
	return 0;
}

CInt LoadScene(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 1 )
	{
		PrintInfo( "\nPlease specify 1 argument for LoadScene()", COLOR_RED );
		return 0;
	}

	g_multipleView->m_loadScene = CTrue;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	ex_pVandaEngine1Dlg->OnMenuClickedNew(CFalse);

	GetWithoutDot(luaToString);
	CChar RTIPath[MAX_NAME_SIZE];
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, RTIPath);
	sprintf( g_fileNameInCommandLine, "%s%s%s%s%s%s", RTIPath, "\\VandaRTI\\VScenes\\", luaToString, "\\", luaToString, ".vin" );
	ex_pVandaEngine1Dlg->OnMenuClickedOpen(CFalse);
	g_multipleView->m_loadScene = CFalse;

	return 0;
}

CInt ActivateCamera( lua_State* L )
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );

	if( Cmp( "DEFAULT", luaToString ) )
		g_render.SetActiveInstanceCamera( NULL );
	else
	{
		for( CUInt i = 0; i < g_cameraInstances.size(); i++ )
		{
			CChar camera[MAX_NAME_SIZE];
			Cpy( camera, g_cameraInstances[i]->m_abstractCamera->GetName() );
			StringToUpper( camera );

			if( Cmp( camera, luaToString ) )
			{
				g_render.SetActiveInstanceCamera(  g_cameraInstances[i] );
				break;
			}
		}
	}
	return 0;
}

CMultipleWindows::CMultipleWindows()
: m_multiSample(CFalse)
, m_grid( CTrue )
, m_numSamples( 8 )
, m_lMouseDown(CFalse)
,m_rMouseDown(CFalse)
,m_mMouseDown(CFalse)
,m_selectObject(CTrue)
,m_tempMovement(CFalse)
,m_tempSelectedName( -1 )
,m_lastSelectedName( -1 )
,m_loadData( CTrue )
,m_dynamicShadowMap(NULL)
,m_timerCounter(0)
,m_lockInput(CFalse)
,m_loadScene(CFalse)
{
}

CMultipleWindows::~CMultipleWindows()
{
	glDeleteTextures(eGBUFFER_NUM_TEXTURES, &m_textureTarget[0] );								
	glDeleteTextures(1, &m_textureTargetSwapLights);								
	glDeleteTextures(1, &m_textureTargetSumLighting);								
	glDeleteTextures(1, &m_textureFogDof);								

	glDeleteFramebuffersEXT( 1, &m_mFboID );
	glDeleteFramebuffersEXT( 1, &m_mFboID2 );
	glDeleteFramebuffersEXT( 1, &m_fboID );
	glDeleteFramebuffersEXT( 1, &m_fboID2 );
	glDeleteFramebuffersEXT( 1, &m_fboIDSum );
	glDeleteFramebuffersEXT( 1, &m_fboIDFogDof );
	glDeleteFramebuffersEXT( 1, &m_mFboIDFogDof );
	
	CDelete( g_arrowScene );
	CDelete( g_negativeZArrow );
	//CDelete (g_zArrow);
	CDelete( g_centerArrowScene );
	CDelete( g_soundImg );
	CDelete( g_pointLightImg );
	CDelete( g_directionalLightImg );
	CDelete( g_spotLightImg );
	CDelete( g_pointLightCOLLADAImg );
	CDelete( g_directionalLightCOLLADAImg );
	CDelete( g_spotLightCOLLADAImg );
	CDelete( g_waterImg );
	CDelete( g_skyImg );
	CDelete( g_cameraImg );

	CDelete( m_translationController );
	//Release physX here
	if( m_nx )
	{
		m_nx->releaseNx();
		delete m_nx;
		m_nx = NULL;
	}
	//Release Audio
	CDelete( m_ambientSound );
	CDelete(m_soundSystem );

	//Release input here
	m_inputSystem->UnacquireAll();
	m_inputSystem->Shutdown();
	CDelete( m_inputSystem );

	CDelete( g_camera );
	CDelete( m_dynamicShadowMap );
	g_octree->ResetState();
	m_dof.DeleteFBOs();
	m_simpleFont.Release();
	KillTimer( 1 );
}


BEGIN_MESSAGE_MAP(CMultipleWindows, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()

END_MESSAGE_MAP()

// CMultipleWindows message handlers

CInt CMultipleWindows::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	// Can we put this in the constructor?
  
	CInt PixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	memset (&pfd, 0, sizeof(pfd));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits	= 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits	= 32;
	pfd.cStencilBits = 0;

	//Get a DC for the Client Area
    m_pDC = new CClientDC(this);

    //Failure to Get DC
    if(m_pDC == NULL) {
        MessageBox(_T("Failure to Get DC"));
        return -1;
	}
	if( m_multiSample )
	{
		switch( g_options.m_numSamples )
		{
			case 2:
				m_numSamples = 2;
				break;
			case 4:
				m_numSamples = 4;
				break;
			case 8:
				m_numSamples = 8;
				break;
			case 16:
				m_numSamples = 16;
				break;
			default:
				m_numSamples = 0;
		}
		GLint samples;
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
		if( m_numSamples > (CInt)samples )
			m_numSamples = samples;
		if( WGLEW_ARB_pixel_format )
		{
			CFloat fAttributes[] = {0,0};
			UINT numFormats;
			CInt attributes[] = {
				WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
				WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,24,
				WGL_ALPHA_BITS_ARB,8,
				WGL_DEPTH_BITS_ARB,24,
				WGL_STENCIL_BITS_ARB,0,
				WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
				WGL_SAMPLE_BUFFERS_ARB,1,
				WGL_SAMPLES_ARB, m_numSamples ,						// Check For m_numSamplesX Multisampling
				0,0
			};

			wglChoosePixelFormatARB( m_pDC->GetSafeHdc(), attributes, fAttributes, 1, &PixelFormat, &numFormats );
			if( numFormats == 0 )
			{
				AfxMessageBox( "Couldn't initialize your window with your desired samples");
				CInt temp = m_numSamples / 2;
				//Test for 2 samples
				while( numFormats == 0 )
				{
					if( temp < 2 )
					{
						m_multiSample = CFalse;
						break;
					}
					attributes[19] = temp;
					wglChoosePixelFormatARB( m_pDC->GetSafeHdc(), attributes, fAttributes, 1, &PixelFormat, &numFormats );
					temp = m_numSamples / 2 ;
				}
				if( numFormats == 0 )
					AfxMessageBox( "Couldn't support multisampling" );
				else
					AfxMessageBox( "I could enable multisampling with lower samples" );
			}
		}
		else
		{
			AfxMessageBox( "Couldn't support multisampling" );
			m_multiSample = CFalse;
		}

	}//if( multisampling )

	if( !m_multiSample )
	{
		PixelFormat = ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd);
		if (PixelFormat == 0)
		{	
			AfxMessageBox("Couldn't choose the pixel format.");
			delete m_pDC;
			m_pDC = NULL;
			return (-1);							
		}
	}
	if (SetPixelFormat(m_pDC->GetSafeHdc(), PixelFormat, &pfd ) == FALSE)	
	{
		AfxMessageBox("Couldn't set the pixel format.");
		delete m_pDC;
		m_pDC = NULL;
		return (-1);							
	}
	m_hRC = wglCreateContext( m_pDC->GetSafeHdc() );
	if ( m_hRC == NULL)
	{
		AfxMessageBox("Couldn't create the OpenGL context.");
		delete m_pDC;
		m_pDC = NULL;
		return (-1);								
	}

	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))					
	{
		AfxMessageBox("Couldn't make a stream between HDC and HRC.");
		wglDeleteContext(m_hRC);
		delete m_pDC;
		m_pDC = NULL;
		m_hRC = NULL;
		return (-1);								
	}
	//if( !InitAll() )
	//	return (-1);
	return (1);
}

CBool CMultipleWindows::InitAll()
{
	for( CInt i = 0; i < 8; i++ )
	{
		glEnable(GL_LIGHT0 + i);	 //just for per vertex lighting 	

		//This is the properties of the camera light
		GLfloat light_pos0[4] = {0.0f, 0.0f, 0.0f, 0.0f };
		GLfloat light_ambient0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		GLfloat light_diffuse0[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat light_specular0[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glLightfv( GL_LIGHT0 + i, GL_POSITION, light_pos0  );
		glLightfv( GL_LIGHT0 + i, GL_AMBIENT , light_ambient0 );
		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE , light_diffuse0 );
		glLightfv( GL_LIGHT0 + i, GL_SPECULAR, light_specular0 );
		glLightf ( GL_LIGHT0 + i, GL_SPOT_CUTOFF,(GLfloat)180.0f );
		glLightf ( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION , (GLfloat)0.0f );
		glLightf ( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION,	(GLfloat)0.0f );
		glLightf ( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, (GLfloat)0.0f );
	}
	glShadeModel(GL_SMOOTH);										// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// Black Background
	glClearDepth( 1.0 );

	//glEnable(GL_TEXTURE_2D);							
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK ); 
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
	GLfloat globalAmbientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbientLight );
	glEnable( GL_NORMALIZE );

	CFog fog;
	fog.SetColor( g_fogProperties.m_fogColor );
	fog.SetDensity( g_fogProperties.m_fogDensity );

	if( m_multiSample )
		glDisable( GL_MULTISAMPLE );
	//glDrawBuffer( GL_BACK );

	if( !m_dynamicShadowMap )
	{
		m_dynamicShadowMap = CNew( CShadowMap );
		g_dynamicShadowMap = m_dynamicShadowMap;
		if( m_dynamicShadowMap->Init() )
			g_render.m_useDynamicShadowMap = CTrue;
		else
		{
			g_render.m_useDynamicShadowMap = CFalse;
		}
	}

	// I use this variable to load the data once. We may destroy and create the window multiple times,
	// But we don't need to load and unload some data for multiple times
	if( m_loadData ) 
	{
		m_dof.InitFBOs( g_width, g_height );
		g_octree = CNew( COctree );
		g_updateOctree = CTrue;
		//initialize audio
		// Position of the listener.
		m_listenerPos[0] = m_listenerPos[1] = m_listenerPos[2] = 0.0f;
		// Velocity of the listener.
		m_listenerVel[0] = m_listenerVel[1] = m_listenerVel[2] =  0.0f;
		// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
		m_listenerOri[0] = 0.0f;
		m_listenerOri[1] = 0.0f;
		m_listenerOri[2] = 1.0f;
		m_listenerOri[3] = 0.0f;
		m_listenerOri[4] = 1.0f;
		m_listenerOri[5] = 0.0f;

		g_soundSystem = m_soundSystem = new COpenALSystem();
		m_soundSystem->Init();
		m_ambientSound = NULL;
		alDistanceModel( AL_INVERSE_DISTANCE );

		//initialize input
		m_inputSystem = new CInputSystem();
		m_inputSystem->AcquireAll();

		if( !m_inputSystem->Initialize( CWnd::m_hWnd, AfxGetInstanceHandle(), true, IS_USEKEYBOARD | IS_USEMOUSE ) )
		{
			MessageBox( "Couldn't initialize Input Engine", "Vanda Engine Error", MB_OK );
			return false;
		}
		else
			PrintInfo( "\nKeyboard and Mouse manager initialized successfully", COLOR_WHITE );

		//Initialize physX here

		m_nx = new CNovodex();

		if( !m_nx->initNx(10.0, 5.0, 10.0,-9.8f, 0.3f, 1.0 ) )
		{
			MessageBox( "Couldn't initialize physX", "Vanda Engine Error", MB_OK );
			return false;
		}
		else
		{
			char temp[MAX_NAME_SIZE];
			sprintf( temp, "\nPhysX version %i.%i.%i initialized successfully", NX_SDK_VERSION_MAJOR, NX_SDK_VERSION_MINOR, NX_SDK_VERSION_BUGFIX );
			PrintInfo( temp, COLOR_WHITE );
		}

		if( !timer.Init() )
		{
			MessageBox( "Couldn't initialize timer.", "Vanda Engine Error", MB_OK | MB_ICONINFORMATION );
			return false; 
		}
		else
			PrintInfo( "\nTimer initialized successfully\n", COLOR_WHITE );

		if( !m_simpleFont.Init( "Arial", 14 ) )
		{
			MessageBox( "Couldn't initialize the font", "VandaEngine Error", MB_OK | MB_ICONINFORMATION );
			return false;
		}
		g_camera = new CUpdateCamera();
		m_loadData = CFalse;
	}
	SetTimer( 1, 1, 0 );
	m_enableTimer = CTrue;
	SetElapsedTimeFromBeginning(); //reset the timer
	g_arrowScene = CNew( CScene );
	//Load Default Arrows
	g_reportInfo = CFalse;

	if( !g_arrowScene->Load( "Assets/engine/Arrows/Arrows.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		//return false;
	}
	g_render.SetScene( g_arrowScene );
	g_arrowScene->Update();

	//g_zArrow = CNew( CScene );
	//if( !g_zArrow->Load( "Assets/engine/Arrows/ZArrow.dae", CFalse ) )
	//{
	//	PrintInfo( "\nCouldn't load the Z arrow", COLOR_RED );
	//	return false;
	//}
	//g_zArrow->Update();

	g_negativeZArrow = CNew( CScene );
	if( !g_negativeZArrow->Load( "Assets/engine/Arrows/NegativeZArrow.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the negative Z arrow", COLOR_RED );
		return false;
	}
	g_render.SetScene( g_negativeZArrow );
	g_negativeZArrow->Update();

	g_centerArrowScene = CNew( CScene );
	//Load Default Arrows
	if( !g_centerArrowScene->Load( "Assets/engine/Arrows/Arrows.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		return false;
	}
	g_render.SetScene( g_centerArrowScene );
	g_reportInfo = CTrue;

	g_centerArrowScene->Update();
	m_translationController = CNew( CMouseTranslationController );
	//Load Default Textures
	GenerateDefaultTextures();
	return true;

}

CBool CMultipleWindows::GenerateDefaultTextures()
{
	g_soundImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_soundImg, "Assets/Engine/Icons/sound.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the sound texture");
		return CFalse;
	}
	g_soundImg->SetFileName( "sound.dds" );

	//point light
	g_pointLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_pointLightImg, "Assets/Engine/Icons/point_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the point light texture");
		return CFalse;
	}
	g_pointLightImg->SetFileName( "point_light.dds" );

	//directional light
	g_directionalLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_directionalLightImg, "Assets/Engine/Icons/directional_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the directional light texture");
		return CFalse;
	}
	g_directionalLightImg->SetFileName( "directional_light.dds" );

	//spot light
	g_spotLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_spotLightImg, "Assets/Engine/Icons/spot_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the spot light texture");
		return CFalse;
	}
	g_spotLightImg->SetFileName( "spot_light.dds" );

	//COLLADA icons
	//point light
	g_pointLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_pointLightCOLLADAImg, "Assets/Engine/Icons/point_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the point_light_collada texture");
		return CFalse;
	}
	g_pointLightCOLLADAImg->SetFileName( "point_light_collada.dds" );

	//directional light
	g_directionalLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_directionalLightCOLLADAImg, "Assets/Engine/Icons/directional_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the directional_light_collada texture");
		return CFalse;
	}
	g_directionalLightCOLLADAImg->SetFileName( "directional_light_collada.dds" );

	//spot light
	g_spotLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_spotLightCOLLADAImg, "Assets/Engine/Icons/spot_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the spot_light_collada texture");
		return CFalse;
	}
	g_spotLightCOLLADAImg->SetFileName( "spot_light_collada.dds" );
	//////////////////
	g_skyImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_skyImg, "Assets/Engine/Icons/sky.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the sky texture");
		return CFalse;
	}
	g_skyImg->SetFileName( "sky.dds" );

	g_waterImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_waterImg, "Assets/Engine/Icons/water.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the water texture");
		return CFalse;
	}
	g_waterImg->SetFileName( "water.dds" );
	//camera
	g_cameraImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_cameraImg, "Assets/Engine/Icons/camera.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the camera texture");
		return CFalse;
	}
	g_cameraImg->SetFileName( "camera.dds" );

	return CTrue;
}

CVoid CMultipleWindows::OnSize(UINT nType, CInt cx, CInt cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_width = cx;
	m_height = cy;
}

CVoid CMultipleWindows::OnDestroy()
{

	wglMakeCurrent( NULL, NULL );
	if( m_hRC )
		wglDeleteContext( m_hRC );
	if( m_pDC )
		delete m_pDC;
	CWnd::OnDestroy();

}

CVoid CMultipleWindows::OnPaint()
{
	CPaintDC dc(this); // device context for paCInting
	// TODO: Add your message handler code here
	RenderWindow();
}

CVoid CMultipleWindows::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnTimer(nIDEvent);
	OnPaint();
}

CVoid CMultipleWindows::OnLButtonDown(UINT nFlags, CPoint point )
{
	ActivateCamera( point );

	SetCapture();
	SetFocus();
	if( !m_enableTimer )
	{
		SetElapsedTimeFromBeginning();
		SetTimer( 1, 1, 0 );
	}
	if( !g_menu.m_justPerspective )
	{
		m_translateMousePosition.x = point.x * 2;
		m_translateMousePosition.y =   ( 2 * point.y )  -  g_height;
	}
	else
		m_translateMousePosition = point;

	if( g_camera->m_activatePerspectiveCamera && g_translateObject && !m_translationController->Initialized() ) 
	{
		CUInt selected = -1;
		selected = GetSelectedObject();
		CBool foundTarget = CFalse;
		//check the icon
		if( selected == g_lastEngineObjectSelectedName )
			foundTarget = CTrue;
		else
		{
			//chack the arrows
			for( CUInt j = 0; j < g_arrowScene->m_geometries.size(); j++ )
			{
				CGeometry* m_geometry = g_arrowScene->m_geometries[j];
				CBool selectedInstance = CFalse;
				for( CUInt k = 0; k < m_geometry->m_instanceGeometries.size(); k++ )
				{
					if( m_geometry->m_instanceGeometries[k]->m_nameIndex == selected )
					{
						selectedInstance = CTrue;
						break;
					}
				}
				if( g_menu.m_geometryBasedSelection )
				{
					if(  selectedInstance )
						foundTarget = CTrue;
				}
				else //material based selection
				{
					for( CUInt k = 0; k < m_geometry->m_groups.size(); k++ )
					{
						CPolyGroup* m_group = m_geometry->m_groups[k];
						if( m_group->m_nameIndex == selected )
							foundTarget = CTrue;
					}
				}
			}
		}
		if( foundTarget  )
			m_translationController->Initialize((int)m_translateMousePosition.x,(int)m_translateMousePosition.y, g_translateDirection, g_arrowPosition);					
	}

	m_lMouseDown = CTrue;
	m_mousePosition = point;  //It's used for selection
	m_selectObject = CTrue;
	m_tempMovement = CFalse; //It's used for selection
	if( g_camera->m_activatePerspectiveCamera )
	{
		//if( g_camera->m_activatePerspectiveCamera && g_translateObject && !m_translationController->Initialized() )
		//	m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_UNAVAILABLE));
		//else
			m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_MOVE));
	}
	else
		m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUSTOM_HAND));

	SetCursor( m_currentCursor );

	CWnd::OnLButtonDown(nFlags, point );
}

CVoid CMultipleWindows::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( GetCapture() == this )
		ReleaseCapture();

	if( !m_tempMovement && g_camera->m_activatePerspectiveCamera )
	{
		g_translateObject = CFalse;

		g_lastEngineObjectSelectedName = -1;
		g_selectedName = m_tempSelectedName;
		if( g_selectedName == m_lastSelectedName )
		{
			m_lastSelectedName = g_selectedName = -1;
		}
		else
		{
			m_lastSelectedName = g_selectedName;
		}
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
			//check arrows
			for( CUInt j = 0; j < g_arrowScene->m_geometries.size(); j++ )
			{
				CGeometry* m_geometry = g_arrowScene->m_geometries[j];
				if( g_menu.m_geometryBasedSelection )
				{
					CBool selected = CFalse;
					for( CUInt k = 0; k < g_arrowScene->m_geometries[j]->m_instanceGeometries.size(); k++ )
					{
						if( g_arrowScene->m_geometries[j]->m_instanceGeometries[k]->m_nameIndex == g_selectedName )
						{
							selected = CTrue;
							break;
						}
					}
					if( selected )
					{
						foundTarget = CTrue;
						g_render.SetSelectedScene( g_arrowScene ); 
						g_lastEngineObjectSelectedName = g_tempLastEngineObjectSelectedName;

						g_translateObject = CTrue; //we are in translation mode ( not in selection mode )
						if( Cmp(g_arrowScene->m_geometries[j]->GetName(), "XAxis" ) )
						{
							g_translateDirection = XTRANS/*XYTRANS*/;
						}
						else if( Cmp(g_arrowScene->m_geometries[j]->GetName(), "YAxis" ) )
						{
							g_translateDirection = ZTRANS/*ZXTRANS*/;
						}
						else if( Cmp(g_arrowScene->m_geometries[j]->GetName(), "ZAxis" ) )
						{
							g_translateDirection = YTRANS/*YZTRANS*/;
						}

 						SetDialogData( CTrue, NULL, g_arrowScene->m_geometries[j], CTrue, CFalse );
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
							g_render.SetSelectedScene( g_arrowScene ); 
							g_lastEngineObjectSelectedName = g_tempLastEngineObjectSelectedName;

							g_translateObject = CTrue; //we are in translation mode ( not in selection mode )
							if( Cmp(m_group->m_materialName, "XAxis" ) )
							{
								g_translateDirection = XTRANS;
							}
							else if( Cmp(m_group->m_materialName, "YAxis" ) )
							{
								g_translateDirection = ZTRANS;
							}
							else if( Cmp(m_group->m_materialName, "ZAxis" ) )
							{
								g_translateDirection = YTRANS;
							}

							SetDialogData2( CTrue, g_arrowScene->m_geometries[j]->m_groups[k], CTrue, CFalse );
						}
					}
				}
			}

			if( !foundTarget ) //something like light, sound, sky or water has been selected
				SetDialogData( CFalse, NULL, NULL, CTrue );
		}
		else
		{
			g_render.SetSelectedScene( NULL ); 

			if( g_menu.m_geometryBasedSelection )
			{
				SetDialogData( CFalse );
			}
			else
			{
				SetDialogData2( CFalse );
			}
		}

	}
	m_translationController->Finish();
	m_lMouseDown = CFalse;
	RenderWindow();
	if( !m_enableTimer )
		KillTimer(1);
	CWnd::OnLButtonUp(nFlags, point);
}

CVoid CMultipleWindows::EnableTimer( CBool enable )
{
	if( enable )
		SetTimer( 1, 1, 0 );
	else
		KillTimer( 1 );
}

CVoid CMultipleWindows::OnRButtonDown(UINT nFlags, CPoint point)
{
	ActivateCamera( point );

	if( !m_enableTimer )
	{
		SetElapsedTimeFromBeginning();
		SetTimer( 1, 1, 0 );
	}

	SetCapture();
	SetFocus();

	m_rMouseDown = CTrue;
	m_selectObject = CTrue;
	m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ZOOMIN));

	SetCursor( m_currentCursor );
	CWnd::OnRButtonDown(nFlags, point);
}

CVoid CMultipleWindows::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( GetCapture() == this )
		ReleaseCapture();
	m_rMouseDown = CFalse;

	if( !m_enableTimer )
		KillTimer(1);
	CWnd::OnRButtonUp(nFlags, point);
}

CVoid CMultipleWindows::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	OnLButtonDown( nFlags, point );
	CWnd::OnMButtonDown(nFlags, point);
}

CVoid CMultipleWindows::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//OnLButtonUp( nFlags, point );
	CWnd::OnMButtonUp(nFlags, point);
}

CVoid CMultipleWindows::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( g_translateObject )
		m_tempMovement = CTrue;
	if( !g_menu.m_justPerspective )
	{
		m_translateMousePosition.x = point.x * 2;
		m_translateMousePosition.y =   ( 2 * point.y )  -  g_height;
	}
	else
		m_translateMousePosition = point;

	m_mousePosition = point;
	CWnd::OnMouseMove(nFlags, point);

}

CVoid CMultipleWindows::ActivateCamera( CPoint point )
{
	if( g_menu.m_justPerspective || ( point.x < m_width / 2 && point.y > m_height / 2 ) ) //perspective window
	{
		g_camera->m_activatePerspectiveCamera = CTrue;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
	else if( ( point.x < m_width / 2 && point.y < m_height / 2 ) ) //upper left
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CTrue;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
	else if( ( point.x > m_width / 2 && point.y < m_height / 2 ) ) //upper right
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CTrue;
	}
	else if( ( point.x > m_width / 2 && point.y > m_height / 2 ) ) //lower right
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CTrue;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
}

CVoid CMultipleWindows::DrawGrid(CVoid)
{
	glUseProgram(0);
	glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT | GL_VIEWPORT_BIT);
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	//Draw the grid here
	glLineWidth( 1.0f );
	glColor3f( 0.5f, 0.7f, 0.5f );
	glBegin( GL_LINES );
	for( CInt index = -50; index <= 50; index += 5 )
	{
		glVertex3f( -50.0f, 0.0f, (CFloat)index );
		glVertex3f(  50.0f, 0.0f, (CFloat)index );

		glVertex3f( (CFloat)index, 0.0f, -50.0f );
		glVertex3f( (CFloat)index, 0.0f, 50.0f );
	}
	glEnd();
	glLineWidth( 3.0f );
	//Draw the axes here

	glBegin( GL_LINES );

	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 1.0f, 0.0f, 0.0f );
	
	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 1.0f, 0.0f );

	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 1.0f );

	glEnd();

	glPopAttrib();
}

CVoid CMultipleWindows::DrawUpperLeft()
{
	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	////Draw grids of the upper left window 
	glViewport( 0, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_upperLeftZoom , m_width * g_camera->m_upperLeftZoom, -m_height * g_camera->m_upperLeftZoom, m_height * g_camera->m_upperLeftZoom ); 
	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( g_camera->m_upperLeftMoveHorizantal , g_camera->m_upperLeftMoveVertical, 1.0f,
		g_camera->m_upperLeftMoveHorizantal, g_camera->m_upperLeftMoveVertical,
		0.0f, 0.0f, 1.0f, 0.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 1.0f );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f( 0.5f, 0.5f, 0.5f );
			layer = -49001;
		}

		if( g_camera->m_upperLeftZoom > 0.05 )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( 0.0f + (CFloat)index, -100.0f, layer );
				glVertex3f( 0.0f + (CFloat)index, 100.0f, layer );

				glVertex3f( -100.0f ,(CFloat)index, layer );
				glVertex3f( 100.0f,(CFloat)index, layer );
			}
		}
		else
		{
			glVertex3f( 0.0f + (CFloat)index, -100.0f, layer );
			glVertex3f( 0.0f + (CFloat)index, 100.0f, layer );

			glVertex3f( -100.0f ,(CFloat)index, layer );
			glVertex3f( 100.0f,(CFloat)index, layer );
		}
	}
	glEnd();
	glPopAttrib();

	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		//g_render.GetScene()->Update();

		if( !g_scene[i]->m_isTrigger )
		{
			g_render.GetScene()->Render(CFalse, NULL, CTrue); 
			g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
		}
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print( "Front", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::DrawUpperRight()
{
	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	//Draw grids of the upper right window 
	glViewport( m_width / 2, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_upperRightZoom, m_width * g_camera->m_upperRightZoom, -m_height * g_camera->m_upperRightZoom, m_height * g_camera->m_upperRightZoom ); 

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( 1.0, g_camera->m_upperRightMoveVertical, g_camera->m_upperRightMoveHorizantal,
		0.0f, g_camera->m_upperRightMoveVertical, g_camera->m_upperRightMoveHorizantal,
		0.0f, 1.0f, 0.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glLineWidth( 1.0f );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f( 0.5f, 0.5f, 0.5f );
			layer = -49001;
		}

		if( g_camera->m_upperRightZoom > 0.05f )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( layer, -100.f, (CFloat)index );
				glVertex3f( layer, 100.f, (CFloat)index );

				glVertex3f( layer,(CFloat)index, -100.f );
				glVertex3f( layer,(CFloat)index, 100.f );
			}
		}
		else
		{
			glVertex3f( layer, -100.f, (CFloat)index );
			glVertex3f( layer, 100.f, (CFloat)index);

			glVertex3f(  layer,(CFloat)index, -100.f );
			glVertex3f( layer,(CFloat)index, 100.f );
		}
	}
	glEnd();
	glPopAttrib();

	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		//g_render.GetScene()->Update();
		if( !g_scene[i]->m_isTrigger )
		{
			g_render.GetScene()->Render(CFalse, NULL, CTrue); 
			g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
		}
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print( "Side", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::DrawLowerRight()
{
	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	//Draw grids of the lower right window 
	glViewport( m_width / 2, 0 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_lowerRightZoom, m_width * g_camera->m_lowerRightZoom, -m_height * g_camera->m_lowerRightZoom, m_height * g_camera->m_lowerRightZoom ); 

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( g_camera->m_lowerRightMoveHorizantal, 1.0, g_camera->m_lowerRightMoveVertical,
		g_camera->m_lowerRightMoveHorizantal, 0.0f, g_camera->m_lowerRightMoveVertical,
		0.0f, 0.0f , 1.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glLineWidth( 1.0f );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f( 0.5f, 0.5f, 0.5f );
			layer = -49001;
		}

		if( g_camera->m_lowerRightZoom > 0.05 )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( -100.0f, layer, (CFloat)index );
				glVertex3f(  100.0f, layer, (CFloat)index );

				glVertex3f( (CFloat)index, layer, -100.0f );
				glVertex3f( (CFloat)index, layer, 100.0f );
			}
		}
		else
		{
			glVertex3f( -100.0f, layer, (CFloat)index );
			glVertex3f(  100.0f, layer, (CFloat)index );

			glVertex3f( (CFloat)index, layer, -100.0f );
			glVertex3f( (CFloat)index, layer, 100.0f );
		}
	}
	glEnd();
	glPopAttrib();

	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		//g_render.GetScene()->Update();

		if( !g_scene[i]->m_isTrigger )
		{
			g_render.GetScene()->Render(CFalse, NULL, CTrue); 
			g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
		}

	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print( "Top", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::SetElapsedTimeFromBeginning()
{
	elapsedTime = timer.GetElapsedSeconds( CTrue ); //start from the beginning
	timer.GetFps( 1, CTrue );
	g_elapsedTime = elapsedTime;
	m_timerCounter  = 0;
	m_totalElapsedTime = 0;
}

CVoid CMultipleWindows::DrawPerspective()
{

	g_numVerts = 0; //debug info
	g_multipleView->UpdateAnimations();

	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting(m_mFboID);
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );
	//if( !g_useOldRenderingStyle )
	//{
	//	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
	//	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT/*, GL_COLOR_ATTACHMENT8_EXT*/};
	//	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);
	//}
	//else
		glDrawBuffer( GL_BACK );

	CInstanceCamera *instanceCamera = g_render.GetActiveInstanceCamera();
	if( instanceCamera )		
	{
		m_lockInput = CTrue;
		SetInstanceCamera(instanceCamera, m_width / 2., m_height / 2.);
	}
	else
	{
		m_lockInput = CFalse;
		g_camera->m_cameraManager->SetPerspective( g_camera->m_cameraManager->GetAngle(), m_width / 2. , m_height / 2., 0.1, 10000.0 );

		if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
		{
			//shadow
			glGetFloatv(GL_PROJECTION_MATRIX, cam_proj);
		}

		m_nx->GetCameraAndCharacterPositions( g_camera->m_perspectiveCameraPitch,
			g_camera->m_perspectiveCameraYaw,
			g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir );

		//Update camera position and orientation
		g_camera->m_cameraManager->UpdateCameraPosDir( g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir,
			g_camera->m_perspectiveCameraTilt );

		//if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
		//{
			// store the inverse of the resulting modelview matrix for the shadow computation
			glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
			g_camera->m_cameraManager->GetInverseMatrix( cam_inverse_modelview );
		//}
	}

	const float bias[16] = {	0.5f, 0.0f, 0.0f, 0.0f, 
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f	};
	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		//shadow
		float light_dir[4] = {g_defaultDirectionalLight.x, g_defaultDirectionalLight.y, g_defaultDirectionalLight.z , 1.0f };

		if( !instanceCamera )
		{
			cam_pos[0] = g_camera->m_perspectiveCameraPos.x;
			cam_pos[1] = g_camera->m_perspectiveCameraPos.y + 1.0;
			cam_pos[2] = g_camera->m_perspectiveCameraPos.z;

			cam_dir[0] = g_camera->m_perspectiveCharacterPos.x - cam_pos[0];
			cam_dir[1] = g_camera->m_perspectiveCharacterPos.y + g_camera->m_perspectiveCameraTilt - cam_pos[1];
			cam_dir[2] = g_camera->m_perspectiveCharacterPos.z - cam_pos[2];
		}
		if( !Cmp( g_shadowProperties.m_directionalLightName, "\n" ) && g_shadowProperties.m_enable && g_render.UsingShadowShader())
			m_dynamicShadowMap->MakeShadowMap( cam_pos, cam_dir, light_dir ); 
	}

	g_camera->m_cameraManager->UpdateFrustum();

	//audio
	m_ambientSourcePos[0] = m_listenerPos[0] = g_camera->m_perspectiveCameraPos.x;
	m_ambientSourcePos[1] = m_listenerPos[1] = g_camera->m_perspectiveCameraPos.y;
	m_ambientSourcePos[2] = m_listenerPos[2] = g_camera->m_perspectiveCameraPos.z;

	m_listenerOri[0] = -g_camera->m_perspectiveCameraDir.x;
	m_listenerOri[1] = g_camera->m_perspectiveCameraDir.y;
	m_listenerOri[2] = -g_camera->m_perspectiveCameraDir.z;

	m_soundSystem->SetListenerPosition( m_listenerPos );
	//m_soundSystem->SetListenerVelocity( m_listenerVel );
	m_soundSystem->SetListenerOrientation( m_listenerOri );
	//set ambient sound
	if(	g_menu.m_insertAmbientSound )
		m_ambientSound->GetSoundSource()->SetSoundPosition( m_ambientSourcePos );

	if( g_polygonMode == ePOLYGON_LINE )
	{
		g_render.m_useShader = CFalse;
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDisable(GL_LIGHTING );
		glDisable(GL_CULL_FACE );
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_MULTISAMPLE );
		glLineWidth(0.5f);
	}
	else if(  g_polygonMode == ePOLYGON_POINT )
	{
		g_render.m_useShader = CFalse;
		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
		glDisable(GL_LIGHTING );
		glDisable(GL_CULL_FACE );
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_MULTISAMPLE );
		glPointSize(1.0);
	}
	else if( g_polygonMode == ePOLYGON_FILL )
	{
		g_render.m_useShader = CTrue;
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	glPushAttrib(GL_LIGHTING_BIT);

	//create all the water textures here
	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			CVec3f waterPoints[4];
			waterPoints[0].x = g_engineWaters[i]->m_sidePoint[0].x;waterPoints[0].y = g_engineWaters[i]->m_sidePoint[0].y;waterPoints[0].z = g_engineWaters[i]->m_sidePoint[0].z;
			waterPoints[1].x = g_engineWaters[i]->m_sidePoint[1].x;waterPoints[1].y = g_engineWaters[i]->m_sidePoint[1].y;waterPoints[1].z = g_engineWaters[i]->m_sidePoint[1].z;
			waterPoints[2].x = g_engineWaters[i]->m_sidePoint[2].x;waterPoints[2].y = g_engineWaters[i]->m_sidePoint[2].y;waterPoints[2].z = g_engineWaters[i]->m_sidePoint[2].z;
			waterPoints[3].x = g_engineWaters[i]->m_sidePoint[3].x;waterPoints[3].y = g_engineWaters[i]->m_sidePoint[3].y;waterPoints[3].z = g_engineWaters[i]->m_sidePoint[3].z;

			if( g_camera->m_cameraManager->IsBoxInFrustum( waterPoints, 4 ) )
			{
				g_engineWaters[i]->CreateReflectionTexture(g_waterTextureSize );
				//g_engineWaters[i]->CreateRefractionDepthTexture(g_waterTextureSize );
			}
		}
	}
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting(m_mFboID);
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );

	//glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	if( g_useOldRenderingStyle || !g_options.m_enableFBO)
		glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
	else
		glClearColor( 0.0f, 0.0f, 0.f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	//Draw grids of the lower left window (perspective)
	if( g_menu.m_justPerspective )
	{
		glViewport( 0, 0 , m_width , m_height );// resets the viewport to new dimensions.
		if( m_multiSample && g_options.m_numSamples ) glEnable( GL_MULTISAMPLE );
		else glDisable( GL_MULTISAMPLE );
	}
	else
	{
		if( g_useOldRenderingStyle || !g_options.m_enableFBO)
		{
			if( m_multiSample ) glDisable( GL_MULTISAMPLE );
			glViewport( 0, 0 , m_width /2 , m_height /2 );// resets the viewport to new dimensions.
		}
		else
		{
			glViewport( 0, 0 , m_width, m_height );// resets the viewport to new dimensions.
			if( m_multiSample && g_options.m_numSamples ) glEnable( GL_MULTISAMPLE );
			else glDisable( GL_MULTISAMPLE );
		}
	}
	if( g_camera->m_activatePerspectiveCamera && g_translateObject && m_translationController->Initialized() )
	{
		m_translationController->Update((int)m_translateMousePosition.x,(int)m_translateMousePosition.y );		
	}
	//Update timer	
	if( !m_selectObject )
		elapsedTime = timer.GetElapsedSeconds();
	else
		elapsedTime = timer.GetElapsedSeconds( CTrue ); //start from the beginning
	g_elapsedTime = elapsedTime;
	m_totalElapsedTime += elapsedTime;
	//Process  inputs
 	if( !m_translationController->Initialized()  )
		ProcessInputs();

	if( m_selectObject  )
	{
		if( g_camera->m_activatePerspectiveCamera )
		{
			CUInt selected = -1;
			selected = GetSelectedObject();

			//CChar temp[ MAX_NAME_SIZE];
			//sprintf( temp, "%d\n", selected );
			//PrintInfo2( temp );
		}
		m_selectObject = CFalse;
	}
	//render waters here
	CVec3f cameraPos( g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z );

	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
 		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			CVec3f waterPoints[4];
			waterPoints[0].x = g_engineWaters[i]->m_sidePoint[0].x;waterPoints[0].y = g_engineWaters[i]->m_sidePoint[0].y;waterPoints[0].z = g_engineWaters[i]->m_sidePoint[0].z;
			waterPoints[1].x = g_engineWaters[i]->m_sidePoint[1].x;waterPoints[1].y = g_engineWaters[i]->m_sidePoint[1].y;waterPoints[1].z = g_engineWaters[i]->m_sidePoint[1].z;
			waterPoints[2].x = g_engineWaters[i]->m_sidePoint[2].x;waterPoints[2].y = g_engineWaters[i]->m_sidePoint[2].y;waterPoints[2].z = g_engineWaters[i]->m_sidePoint[2].z;
			waterPoints[3].x = g_engineWaters[i]->m_sidePoint[3].x;waterPoints[3].y = g_engineWaters[i]->m_sidePoint[3].y;waterPoints[3].z = g_engineWaters[i]->m_sidePoint[3].z;

			if( g_camera->m_cameraManager->IsBoxInFrustum( waterPoints, 4 ) )
			{
				glUseProgram( g_render.m_waterProgram );
				g_engineWaters[i]->RenderWater(cameraPos, elapsedTime );
				glUseProgram( 0 );
			}
		}
	}

	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		glActiveTexture(GL_TEXTURE6 );
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, m_dynamicShadowMap->depth_tex_ar);
		if( g_shadowProperties.m_shadowType > 3 || g_shadowProperties.m_shadowType == 0 /* 0 is debug state*/)
			glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		else 
			glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		for(int i=m_dynamicShadowMap->cur_num_splits; i<MAX_SPLITS; i++)
			far_bound[i] = 0;

		// for every active split
		for(int i=0; i<m_dynamicShadowMap->cur_num_splits; i++)
		{
			float light_m[16];

			// f[i].fard is originally in eye space - tell's us how far we can see.
			// Here we compute it in camera homogeneous coordinates. Basically, we calculate
			// cam_proj * (0, 0, f[i].fard, 1)^t and then normalize to [0; 1]
			far_bound[i] = 0.5f*(-m_dynamicShadowMap->f[i].fard*cam_proj[10]+cam_proj[14])/m_dynamicShadowMap->f[i].fard + 0.5f;

			// compute a matrix that transforms from camera eye space to light clip space
			// and pass it to the shader through the OpenGL texture matrices, since we
			// don't use them now
			glActiveTexture(GL_TEXTURE0 + (GLenum)i);
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf(bias);
			glMultMatrixf(m_dynamicShadowMap->shad_cpm[i]);
			// multiply the light's (bias*crop*proj*modelview) by the inverse camera modelview
			// so that we can transform a pixel as seen from the camera
			glMultMatrixf(cam_inverse_modelview);
			
			// compute a normal matrix for the same thing (to transform the normals)
			// Basically, N = ((L)^-1)^-t
			glGetFloatv(GL_TEXTURE_MATRIX, light_m);
			matrix4<float> nm;
			nm.set_value(light_m);
			nm = inverse(nm);
			nm = transpose(nm);

			glActiveTexture(GL_TEXTURE0 + (GLenum)(i+4));
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf(nm._array);
		}
		glMatrixMode( GL_MODELVIEW );
	}

	//render the scene
	g_numLights = 0;

	if( g_polygonMode == ePOLYGON_FILL )
		glEnable(GL_LIGHTING);   //just for per vertex lighting

	EngineLightPass();
	COLLADALightPass();
	FixedFunctionLightPass();
	DefaultLightPass();

	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting( m_mFboID );
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );

	g_currentInstanceLight = NULL;
	g_totalLights = g_numLights;

	CBool useFog;
	if( ( g_dofProperties.m_enable && g_dofProperties.m_debug ) || ( g_shadowProperties.m_shadowType == eSHADOW_SINGLE_HL && g_shadowProperties.m_enable && g_render.UsingShadowShader()))
		useFog = CFalse;
	else 
		useFog = CTrue;

	if( ( g_fogProperties.m_enable && useFog ) || g_dofProperties.m_enable )
	{
		if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
			g_render.BindForWriting(m_mFboIDFogDof);
		else if(!g_useOldRenderingStyle && g_options.m_enableFBO )
			g_render.BindForWriting( m_fboIDFogDof );
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		g_fogBlurPass = CTrue;
		g_octree->Render();
		g_multipleView->Render3DAnimatedModels( CTrue);
		g_multipleView->Render3DModelsControlledByPhysX();
		g_fogBlurPass = CFalse;
		BlendFogWithScene();
	}

	if( g_menu.m_showPerspectiveGrids )
		g_centerArrowScene->Render( CTrue );

	if( g_updateOctree && g_scene.size() > 0 )
	{
		PrintInfo( "\nUpdating Octree...", COLOR_GREEN );
		Render3DModels( CTrue, NULL );
		g_octree->ResetState();
		//g_octree->Init();
		g_octree->GetWorldDimensions();
		g_octree->SetName( "octree_root" );
		g_octree->SetLevel(0);
		g_octree->AttachGeometriesToNode();
		//g_octree->AttachLightsToGeometries();
		g_updateOctree = CFalse;
		PrintInfo( "\nOctree updated successfully", COLOR_RED_GREEN );
		PrintInfo( "\nReady", COLOR_GREEN );

	}
	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		//reset matrice
		for( CUInt i = 0; i < 8; i++ )
		{
			glActiveTexture(GL_TEXTURE0 + i );
			glMatrixMode( GL_TEXTURE );
			glLoadIdentity();
		}
		glActiveTexture(GL_TEXTURE0);
		glMatrixMode( GL_MODELVIEW );
	}
	
	if( g_menu.m_insertAndShowSky )
	{
		g_skyDome->RenderDome();
		if( g_menu.m_showSkyIcon && !g_dofProperties.m_debug )
			g_skyDome->RenderIcon();
	}

	//render water icons after the scene rendering
	if ( g_menu.m_showWaterIcons && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader && !g_dofProperties.m_debug)
	{
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			g_engineWaters[i]->RenderIcon();
	}
	//Render light icons after the scene rendering
	//Engine lights
	if( !g_dofProperties.m_debug )
	{
		for( CUInt i = 0; i < g_engineLights.size(); i++ )
		{
			g_engineLights[i]->RenderIcon();
			g_engineLights[i]->RenderExtents();
		}
	}
	if( !g_dofProperties.m_debug )
	{
		//COLLADA Lights. Not recommended
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			
			//set all the lights here
			for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
			{
				CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
				if( instanceLight )
				{
					instanceLight->RenderIcon();
					instanceLight->RenderExtents();
				}
			}
		}
	}
	if( !g_dofProperties.m_debug )
	{
		for( CUInt i = 0; i < g_cameraInstances.size(); i++ )
		{
			CInstanceCamera * instanceCamera = g_cameraInstances[i];
			if( instanceCamera )
				instanceCamera->RenderIcon();
		}
	}
	if( g_camera->m_activatePerspectiveCamera && g_translateObject && m_translationController->Initialized() )
	{
		g_arrowPosition += m_translationController->GetDisplacement() ;
		m_translationController->DrawPlane();
	}

	if( g_menu.m_physicsDebugMode )
		m_nx->debugMode = CTrue;
	else
		m_nx->debugMode = CFalse;

	if( m_nx->debugMode )
		m_nx->debugRenderer();

	if( g_menu.m_showPerspectiveGrids && !g_dofProperties.m_debug)
		DrawGrid();
	if( g_menu.m_showSoundIcons && !g_dofProperties.m_debug)
	{
		for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
		{
			 g_engineStaticSounds[i]->RenderIcon();
		}
	}
	if( g_showArrow )
	{
		CFloat distance = sqrt( pow( g_camera->m_perspectiveCameraPos.x - g_arrowPosition.x, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.y - g_arrowPosition.y, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.z - g_arrowPosition.z, 2 ) );
		if( distance < 10.0f )
			distance = 10.0f;

		glPushMatrix();
		glTranslatef( g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z );
		glScalef( distance * 0.2f , distance * 0.2f, distance * 0.2f);
		g_arrowScene->Render( CFalse );
		glPopMatrix();
	}

	glDisable( GL_LIGHT0 );

	for( CInt i = 0; i <g_numLights; i++ )
	{
		glDisable( GL_LIGHT0 + i );
	}
	glDisable( GL_LIGHTING ); //just for per vertex lighting'
	glMatrixMode( GL_MODELVIEW );

	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				g_scene[i]->m_instanceGeometries[j]->m_renderCount = 0;
			}
		}
		if( g_render.GetScene()->m_hasAnimation )
		{
			g_render.GetScene()->m_updateAnimation = CTrue;
		}
	}

	glPopAttrib(); //lighting

	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading( m_mFboID );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_DEFAULT );

		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_POSITION );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_NORMAL );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT3_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_DIFFUSE );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT4_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT4_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_NORMALMAP );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT5_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT5_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_GLOSSMAP );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT6_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT6_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_SHADOWMAP );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT7_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT7_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_DIRTMAP );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		if( g_dofProperties.m_enable )
		{
			glUseProgram(g_render.m_dofProgram[1]);
			if( g_dofProperties.m_debug )
				glUniform1i(glGetUniformLocation( g_render.m_dofProgram[1] , "debugMode"), CTrue);
			else
				glUniform1i(glGetUniformLocation( g_render.m_dofProgram[1] , "debugMode"), CFalse);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			/* Second pass: downsampling */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[0]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
			glUseProgram(g_render.m_dofProgram[0]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[0], "Tex0"), 0 );
			glViewport(0, 0, g_width/2, g_height/2);
			m_dof.DrawQuad();

			/* Third pass: Gaussian filtering along the X axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[1]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[0]);
			glUseProgram(g_render.m_dofProgram[1]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[1], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[1], "Width"), g_width );
			m_dof.DrawQuad();

			/* Fourth pass: Gaussian filtering along the Y axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[2]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(g_render.m_dofProgram[2]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[2], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[2], "Height"), g_height );
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[1]);
			m_dof.DrawQuad();

			/* Fifth pass: final compositing */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[3]);
			////glDrawBuffer(GL_BACK);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[2]);
			glUseProgram(g_render.m_dofProgram[3]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[3], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[3], "Tex1"), 1 );

			glViewport(0, 0, g_width, g_height);

			glBegin(GL_QUADS);
				glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);
				glVertex2d(-1, -1);
				glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 0.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 0.0f);
				glVertex2d(1, -1);
				glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 1.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 1.0f);
				glVertex2d(1, 1);
				glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 1.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 1.0f);
				glVertex2d(-1, 1);
			glEnd();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

		if( !g_menu.m_justPerspective )
			glViewport( 0, 0 , m_width / 2, m_height / 2 );// resets the viewport to new dimensions.

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glUseProgram(0);
		glDrawBuffer( GL_BACK );
		glClear( GL_COLOR_BUFFER_BIT );
		glPushAttrib( GL_ENABLE_BIT );
		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		gluOrtho2D(0, g_width, 0, g_height );
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
		glDisable( GL_LIGHTING );
		glDisable(GL_DEPTH_TEST);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		CBool found = CFalse;
		if( g_menu.m_showVertexPositions )
		{
			glBindTexture( GL_TEXTURE_2D, m_textureTarget[1] );
			found = CTrue;
		}
		else if( g_menu.m_showNormalVectors )
		{
			glBindTexture( GL_TEXTURE_2D, m_textureTarget[2] );
			found = CTrue;
		}
		else
		{
			switch( g_materialChannels )
			{
			case eCHANNELS_ALL:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[0] );
				break;
			case eCHANNELS_DIFFUSE:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[3] );
				found = CTrue;
				break;
			case eCHANNELS_NORMALMAP:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[4] );
				found = CTrue;
				break;
			case eCHANNELS_GLOSSMAP:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[5] );
				found = CTrue;
				break;
			case eCHANNELS_SHADOWMAP:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[6] );
				found = CTrue;
				break;
			case eCHANNELS_DIRTMAP:
				glBindTexture( GL_TEXTURE_2D, m_textureTarget[7] );
				found = CTrue;
				break;
			//case eCHANNELS_ALPHAMAP:
			//	glBindTexture( GL_TEXTURE_2D, m_textureTarget[8] );
			//	break;
			}
		}

		if( !found )
			if( g_dofProperties.m_enable )
			{
				glBindTexture( GL_TEXTURE_2D, m_dof.m_texid[3] );
				found = CTrue;
			}

		glEnable( GL_TEXTURE_2D);
		glClearColor( 0.4235f, 0.4235f, 0.4235f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//if( !found ) //render default texture with deferred shader
		//{
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[1]);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[2]);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[3]);
		//glUseProgram(g_render.m_deferredProgram);
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tPosition"), 0 );
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tNormals"), 1 );
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tDiffuse"), 2 );
		//CFloat cameraPos[3] = { g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z };
		//glUniform3fv(glGetUniformLocation(g_render.m_deferredProgram, "cameraPosition"), 1, cameraPos);
		//}
		glActiveTexture(GL_TEXTURE0);
		glBegin(GL_QUADS); 
		glTexCoord2d(0,	0);	glVertex2d(0, 0); 
		glTexCoord2d(1, 0);	glVertex2d(g_width, 0);
		glTexCoord2d(1, 1);	glVertex2d(g_width, g_height );
		glTexCoord2d(0, 1);	glVertex2d(0, g_height);
		glEnd();
		glUseProgram(0);

		CBool useBloom;
		if( (g_dofProperties.m_enable && g_dofProperties.m_debug ) || g_materialChannels != eCHANNELS_ALL || g_menu.m_showNormalVectors || g_menu.m_showVertexPositions || g_polygonMode != ePOLYGON_FILL)
			useBloom = CFalse;
		else
			useBloom = CTrue;

		if( g_render.m_useBloom && useBloom && g_bloomProperties.m_enable)
		{
			g_bloom->CreateRuntimeTexture( g_width, g_height, m_textureTarget[0]  );

			glPushAttrib( GL_CURRENT_BIT );

			glEnable( GL_TEXTURE_2D );

			glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glColor4f( g_bloomProperties.m_bloomColor[0], g_bloomProperties.m_bloomColor[1], g_bloomProperties.m_bloomColor[2], g_bloomProperties.m_bloomIntensity );
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, g_bloom->m_bloomTexture );
			glBegin(GL_QUADS); 
			glTexCoord2d(0,	0);	glVertex2d(0, 0); 
			glTexCoord2d(1, 0);	glVertex2d(g_width, 0);
			glTexCoord2d(1, 1);	glVertex2d(g_width, g_height );
			glTexCoord2d(0, 1);	glVertex2d(0, g_height);
			glEnd();
			glDisable( GL_BLEND );
			glPopAttrib();
		}

		if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_menu.m_showDynamicShadowDepthTexture && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
 			m_dynamicShadowMap->ShowDepthTex();
	
		DrawGUI();
		glPopAttrib();
		glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
	}
	else
	{
		DrawGUI();
	}
	glFlush();
}

CVoid CMultipleWindows::DrawJustPerspectiveBorders()
{
	//Draw black borders here
    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

    // calculate the aspect ratio of the window.
	glOrtho( 0, 1024, 0, 768, -50000, 50000 );
	

	glPushAttrib( GL_VIEWPORT_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );

	//main borders
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

	glColor4f( 0.75f, 0.75f, 0.75f, 0.0f );
	glLineWidth( 3.0f );

	glBegin( GL_LINE_LOOP );
	glVertex3f( 0 , 0, 49999.0 ); 
	glVertex3f( 1024 , 0, 49999.0 ); 
	glVertex3f( 1024 , 768, 49999.0 ); 
	glVertex3f( 0, 768, 49999.0 ); 
	glEnd();
	glPopAttrib();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

CVoid CMultipleWindows::DrawBordersAndUI()
{
	//Draw black borders here
    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

    // calculate the aspect ratio of the window.
	glOrtho( 0, 1024, 0, 768, -50000, 50000 );
	
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

	glPushAttrib( GL_VIEWPORT_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );

	//main borders

	glViewport( 0, 0, m_width, m_height );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0, 1024.0, 0.0, 768.0, -50000.0, 50000.0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glColor4f( 0.157f, 0.157f, 0.157f, 0.0f );
	glLineWidth( 5.0f );

	glBegin( GL_LINES );
	glVertex3f( 1024 / 2 , 0, 49999.0 ); 
	glVertex3f( 1024 / 2 , 768, 49999.0 ); 
	glVertex3f( 0 , 768 / 2, 49999.0 ); 
	glVertex3f( 1024, 768 / 2, 49999.0 ); 
	glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glLineWidth( 2.0f );
	//perspective borders
	glViewport(0, 0, m_width / 2, m_height / 2);// resets the viewport to new dimensions.
	if( g_camera->m_activatePerspectiveCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f( 0.75f, 0.75f, 0.75f, 0.0f );
	glBegin( GL_LINE_LOOP );
	glVertex3f( 2, 2, 49999.0 ); 
	glVertex3f( 1024 - 6, 2, 49999.0 ); 
	glVertex3f( 1024 - 6, 768 - 8, 49999.0 ); 
	glVertex3f( 2, 768 - 8, 49999.0 ); 
	glEnd();

	//Lower right borders
	glLineWidth( 2.0f );

	glViewport(m_width / 2, 0, m_width / 2 , m_height / 2);// resets the viewport to new dimensions.
	if( g_camera->m_activateLowerRightCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f( 0.75f, 0.75f, 0.75f, 0.0f );
	glBegin( GL_LINE_LOOP );
	glVertex3f( 5, 2, 49999.0 ); 
	glVertex3f( 1024 - 2, 2, 49999.0 ); 
	glVertex3f( 1024 - 2, 768 - 8, 49999.0 ); 
	glVertex3f( 5, 768 - 8, 49999.0 ); 
	glEnd();

	//Axes of lower right window
	glLineWidth( 1.0f );
	glColor4f( 0.7f, 0.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.0f, 1.0f, 0.0f );
	glVertex2i( 77 , 39 ); 
	glVertex2i( 77 , 100 ); 
	glEnd();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "y", 60.0f, 40.0f, 0.0f, 0.00f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 55.0f, 140.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.Print( "x", 15.0f, 35.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.EndRendering();

	//Upper left borders
	glLineWidth( 2.0f );

	glViewport(0, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	if( g_camera->m_activateUpperLeftCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f( 0.75f, 0.75f, 0.75f, 0.0f );
	glBegin( GL_LINE_LOOP );
	glVertex3f( 2, 6, 49999.0 ); 
	glVertex3f( 1024 - 6, 6, 49999.0 ); 
	glVertex3f( 1024 - 6, 768 - 2, 49999.0 ); 
	glVertex3f( 2, 768 - 2, 49999.0 ); 
	glEnd();

	//Axes of upper left window
	glLineWidth( 1.0f );
	glColor4f( 0.7f, 0.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.7f, 0.0f, 0.0f );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 31 , 100 ); 
	glEnd();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "x", 60.0f, 40.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.Print( "y", 20.0f, 140.0f, 0.0f, 0.0f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 15.0f, 35.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.EndRendering();

	//Upper right borders
	glLineWidth( 2.0f );

	glViewport(m_width / 2, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	if( g_camera->m_activateUpperRightCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f( 0.75f, 0.75f, 0.75f, 0.0f );
	glBegin( GL_LINE_LOOP );
	glVertex3f( 5, 6, 49999.0 ); 
	glVertex3f( 1024 - 2, 6, 49999.0 ); 
	glVertex3f( 1024 - 2, 768 - 2, 49999.0 ); 
	glVertex3f( 5, 768 - 2, 49999.0 ); 
	glEnd();
	//Axes of upper right window
	glLineWidth( 1.0f );
	glColor4f( 0.0f, 0.0f, 1.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.7f, 0.0f, 0.0f );
	glVertex2i( 77 , 39 ); 
	glVertex2i( 77 , 100 ); 
	glEnd();

    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPopMatrix();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "x", 60.0f, 40.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.Print( "y", 55.0f, 140.0f, 0.0f, 0.0f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 15.0f, 35.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.EndRendering();


	glPopAttrib();
}

CVoid CMultipleWindows::RenderWindow()
{
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	DrawPerspective();
	if( !g_menu.m_justPerspective )
	{
		CPolygonMode tempMode = g_polygonMode;
		g_polygonMode = ePOLYGON_FILL; //to enable texturing. It's not a good way though!
		glDisable( GL_MULTISAMPLE );
		DrawUpperLeft();
		DrawUpperRight();
		DrawLowerRight();
		DrawBordersAndUI();	
		g_polygonMode = tempMode;
	}
	else
	{
		DrawJustPerspectiveBorders();
	}
	ResetPhysXCounts();
	SwapBuffers(m_pDC->m_hDC);
}

CVoid CMultipleWindows::ResetPhysXCounts()
{
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			g_scene[i]->m_instanceGeometries[j]->m_physXCount = 0;
	}
}

BOOL CMultipleWindows::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

		if( pMsg->message == WM_KEYDOWN )
		{
			return TRUE;
		}

	return CWnd::PreTranslateMessage(pMsg);
}

//void CMultipleWindows::ApplyForce( NxVec3 forceDirection, CInt moveDirection, CFloat elapsedTime )
//{
//	if( m_physXVariables.m_applyDirectionalForce )
//	{
//		if( m_physXVariables.m_forceSpeedValue >= 0.0f )
//		{
//			m_nx->bPushCharacter = CTrue;
//			m_nx->runPhysics( forceDirection, m_physXVariables.m_forceSpeedValue, moveDirection, elapsedTime );
//		}
//		else 
//		{
//			m_nx->bPushCharacter = CFalse;
//			m_physXVariables.m_applyDirectionalForce = CFalse;
//		}
//		m_physXVariables.m_forceSpeedValue -= elapsedTime * m_physXVariables.m_forceDecreaseValue;
//	}
//	else
//		m_nx->runPhysics( NxVec3( 0.0f, 0.0f, 0.0f ), 0.0f, moveDirection, elapsedTime );
//}

CVoid CMultipleWindows::ApplyForce( CInt direction, CFloat elapsedTime )
{
	m_nx->runPhysics( NxVec3( 0.0f, 0.0f, 0.0f ), 0.0f, direction, elapsedTime );
}

CVoid CMultipleWindows::ProcessInputs()
{
	if( m_lockInput || m_loadScene ) return;
	m_inputSystem->Update();

	CBool forceApplied = false;
	if( m_lMouseDown )
	{
		if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_W ) )
		{
			g_multipleView->m_nx->gCharacterSpeed += 1.0f;
		}
		else 	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_S ) )
 		{
			g_multipleView->m_nx->gCharacterSpeed -= 1.0f;
			if(	g_multipleView->m_nx->gCharacterSpeed <= 0.0 )
				g_multipleView->m_nx->gCharacterSpeed += 1.0f;

		}
		else	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_LSHIFT ) )
		{
			g_multipleView->m_nx->gCharacterSpeed = DEFAULT_CHARACTER_SPEED;
		}
		else	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_Z ) )
		{
			g_camera->m_cameraManager->SetAngle( DEFAULT_CAMERA_ANGLE );
		}
		else	if( m_inputSystem->KeyDown( DIK_W ) || m_inputSystem->KeyDown( DIK_UP ) )
		{
			ApplyForce( MOVE_FRONT, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
		else if( m_inputSystem->KeyDown( DIK_S ) || m_inputSystem->KeyDown( DIK_DOWN ) )
		{
			ApplyForce( MOVE_BACK, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
		else if( m_inputSystem->KeyDown( DIK_A ) || m_inputSystem->KeyDown( DIK_LEFT ) )
		{
			ApplyForce( MOVE_LEFT, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
		else if( m_inputSystem->KeyDown( DIK_D ) || m_inputSystem->KeyDown( DIK_RIGHT ) )
		{
			ApplyForce( MOVE_RIGHT, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
		else if( m_inputSystem->KeyDown( DIK_Q ) )
		{
			ApplyForce( MOVE_UP, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
		else if( m_inputSystem->KeyDown( DIK_E ) )
		{
			ApplyForce( MOVE_DOWN, elapsedTime );
			forceApplied = true;
			m_tempMovement = CTrue;
		}
	}

	if( !forceApplied )
		ApplyForce( IDLE, elapsedTime );
	if( m_lMouseDown || m_rMouseDown )
	{
		CInt dx, dy;
		m_inputSystem->GetMouseMovement( dx, dy );

		if( !m_selectObject )
		{
			//perspective viewport
			if( g_camera->m_activatePerspectiveCamera )
			{
				if( m_lMouseDown )
				{
					if( dx > 0 ) 
					{
						g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
						m_tempMovement = CTrue;
					}
					else if( dx < 0 )
					{
						g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
						m_tempMovement = CTrue;
					}
					if( dy > 0 ) 
					{
						g_camera->m_perspectiveCameraTilt -= (CFloat)dy * 0.01f;
						m_tempMovement = CTrue;
					}
					else if( dy < 0 )
					{
						g_camera->m_perspectiveCameraTilt -= (CFloat)dy * 0.01f;
						m_tempMovement = CTrue;
					}
				}
				else if( m_rMouseDown )
				{  
					if( dy > 0 ) 
						g_camera->m_cameraManager->SetZoomOut( elapsedTime * 50.0f );
					else if( dy < 0 )
						g_camera->m_cameraManager->SetZoomIn( elapsedTime * 50.0f );
				}
			}
			//lower left viewport
			else if( g_camera->m_activateLowerRightCamera )
			{
				if( m_lMouseDown )
				{
					if( dx > 0 ) 
						g_camera->m_lowerRightMoveHorizantal += (CFloat)dx * g_camera->m_lowerRightZoom * 2.0f;
					else if( dx < 0 )
						g_camera->m_lowerRightMoveHorizantal += (CFloat)dx * g_camera->m_lowerRightZoom * 2.0f;

					if( dy > 0 ) 
						g_camera->m_lowerRightMoveVertical += (CFloat)dy * g_camera->m_lowerRightZoom * 2.0f;
					else if( dy < 0 )
						g_camera->m_lowerRightMoveVertical += (CFloat)dy * g_camera->m_lowerRightZoom * 2.0f;
				}
				else if( m_rMouseDown ) //zoom in and out
				{
					if( dy > 0 && g_camera->m_lowerRightZoom <  0.2f ) 
						g_camera->m_lowerRightZoom += (CFloat)dy * g_camera->m_lowerRightZoom * 0.004f;
					else if( dy < 0 && g_camera->m_lowerRightZoom > 0.001f )
						g_camera->m_lowerRightZoom += (CFloat)dy * g_camera->m_lowerRightZoom * 0.004f;
				}
			}
			//upper left viewport
			else if( g_camera->m_activateUpperLeftCamera )
			{
				if( m_lMouseDown ) //move horizontal or vertical
				{
					if( dx > 0 ) 
						g_camera->m_upperLeftMoveHorizantal -= (CFloat)dx * g_camera->m_upperLeftZoom * 2.0f;
					else if( dx < 0 )
						g_camera->m_upperLeftMoveHorizantal -= (CFloat)dx * g_camera->m_upperLeftZoom * 2.0f;

					if( dy > 0 ) 
						g_camera->m_upperLeftMoveVertical += (CFloat)dy * g_camera->m_upperLeftZoom * 2.0f;
					else if( dy < 0 )
						g_camera->m_upperLeftMoveVertical += (CFloat)dy * g_camera->m_upperLeftZoom * 2.0f;
				}
				else if( m_rMouseDown ) //zoom in and out
				{
					if( dy > 0 && g_camera->m_upperLeftZoom < 0.2f ) 
						g_camera->m_upperLeftZoom += (CFloat)dy * g_camera->m_upperLeftZoom * 0.004f;
					else if( dy < 0 && g_camera->m_upperLeftZoom > 0.001f )
						g_camera->m_upperLeftZoom += (CFloat)dy * g_camera->m_upperLeftZoom * 0.004f;
				}
			}
			//upper right viewport
			else if( g_camera->m_activateUpperRightCamera )
			{
				if( m_lMouseDown )
				{
					if( dx > 0 ) 
						g_camera->m_upperRightMoveHorizantal += (CFloat)dx * g_camera->m_upperRightZoom * 2.0f;
					else if( dx < 0 )
						g_camera->m_upperRightMoveHorizantal += (CFloat)dx * g_camera->m_upperRightZoom * 2.0f;

					if( dy > 0 ) 
						g_camera->m_upperRightMoveVertical += (CFloat)dy * g_camera->m_upperRightZoom * 2.0f;
					else if( dy < 0 )
						g_camera->m_upperRightMoveVertical += (CFloat)dy * g_camera->m_upperRightZoom * 2.0f;
				}
				else if( m_rMouseDown ) //zoom in and out
				{
					if( dy > 0 && g_camera->m_upperRightZoom < 0.2f ) 
						g_camera->m_upperRightZoom += (CFloat)dy * g_camera->m_upperRightZoom * 0.004f;
					else if( dy < 0 && g_camera->m_upperRightZoom > 0.001f )
						g_camera->m_upperRightZoom += (CFloat)dy * g_camera->m_upperRightZoom * 0.004f;
				}
			}
		}
	}
}

CVoid CMultipleWindows::InitObjectSelection()
{
	GLint viewport[4];
	glSelectBuffer( MAX_NAME_SIZE , Buffer );
	(void)glRenderMode( GL_SELECT );
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT,viewport);

	if( g_menu.m_justPerspective )
	{
		gluPickMatrix( (CDouble)m_mousePosition.x - 1.0,  (CDouble)m_height - (CDouble)m_mousePosition.y - 2.0 , 2.0, 1.0, viewport );
	}
	else
	{
		gluPickMatrix( (CInt)m_mousePosition.x - 2,  m_height - (CInt)m_mousePosition.y - 2 , 4.0, 4.0, viewport );
	}
	gluPerspective( g_camera->m_cameraManager->GetAngle(), CDouble( m_width ) / CDouble (m_height ) , 0.1, 2000.0 );
	g_render.ModelViewMatrix();
	glInitNames();
}

CVoid CMultipleWindows::FinishObjectSelection()
{
	g_render.ProjectionMatrix();
	g_render.PopMatrix();
	g_render.ModelViewMatrix();
	glFlush();
	GLint hits = glRenderMode( GL_RENDER );

	if( hits > 0 )
	{
		ptr = Buffer;
		for (CInt j = 0; j < hits; j++)
		{	
			ptr++; //skip the first info ( number of names which is one in my engine )and move to the min depth info

			if (*ptr < minZ )
			{
				minZ = *ptr;

				selectedName = *(ptr + 2); //name of the object
			}
			ptr += 3; //move to the next hit
		}
	}
}
//Returns -1 if no selection occurs
CUInt CMultipleWindows::GetSelectedObject()
{
	minZ = 0xffffffff;
	selectedName = -1;
	glPushAttrib( GL_VIEWPORT_BIT );
	if( g_menu.m_justPerspective )
	{
		glViewport( 0, 0 , m_width , m_height );// resets the viewport to new dimensions.
	}
	else
	{
		glViewport( 0, 0 , m_width /2 , m_height /2 );// resets the viewport to new dimensions.
	}

	if( g_showArrow )
	{
		CFloat distance = sqrt( pow( g_camera->m_perspectiveCameraPos.x - g_arrowPosition.x, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.y - g_arrowPosition.y, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.z - g_arrowPosition.z, 2 ) );
		if( distance < 10.0f )
			distance = 10.0f;
		InitObjectSelection();
		glPushMatrix();
		glTranslatef( g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z );
		glScalef( distance * 0.2f , distance * 0.2f, distance * 0.2f);
		g_arrowScene->RenderSelectionMode();
		glPopMatrix();
		FinishObjectSelection();
	}

	if( g_menu.m_insertAndShowSky )
	{
		InitObjectSelection();
		g_skyDome->RenderIcon( CTrue );
		FinishObjectSelection();
	}

	InitObjectSelection();
	//Render light icons after the scene rendering
	//Engine lights
	for( CUInt i = 0; i < g_engineLights.size(); i++ )
	{
		g_engineLights[i]->RenderIcon(CTrue);
	}
	//COLLADA Lights. Not recommended
	//for( CUInt i = 0 ; i < g_scene.size(); i++ )
	//{
	//	g_render.SetScene( g_scene[i] );
	//	
	//	//set all the lights here
	//	for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
	//	{
	//		CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
	//		if( instanceLight )
	//			instanceLight->RenderIcon(CTrue);
	//	}
	//}
	FinishObjectSelection();

	InitObjectSelection();
	if( g_menu.m_showSoundIcons )
	{
		for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
		{
			 g_engineStaticSounds[i]->RenderIcon(CTrue);
		}
	}
	FinishObjectSelection();

	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
 		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			InitObjectSelection();
			g_engineWaters[i]->RenderIcon( CTrue ); //render in selection mode
			FinishObjectSelection();
		}
	}

	for( CUInt i = 0; i < g_scene.size(); i++ )
	{
		InitObjectSelection();
		//g_scene[i]->RenderAABBWithQuads(); //Here's the code to "draw" the objects in selection mode
		g_render.SetScene( g_scene[i] );
		//g_render.GetScene()->Update();
		g_render.GetScene()->RenderSelectionMode(); 
		FinishObjectSelection();
	}
	glPopAttrib();


	m_tempSelectedName = (CUInt)selectedName;
	return selectedName;
}

CBool CMultipleWindows::InitFBOs( CInt channels, CInt type )
{
	CUInt numSamples;
	GLenum status;
	if( m_multiSample ) //initialize 2 separate FBOs
	{
		switch( g_options.m_numSamples  )
		{
		case 2:
			numSamples = 2;
			break;
		case 4:
			numSamples = 4;
			break;
		case 8:
			numSamples = 8;
			break;
		case 16:
			numSamples = 16;
			break;
		default:
			numSamples = 0;
		}
		GLint samples;
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
		if( numSamples > (CUInt)samples )
			numSamples = samples;
		m_mFboID = g_render.GenerateFBO();
		g_render.BindFBO( m_mFboID );
		//g_render.BindForWriting( m_mFboID );
		//g_render.Attach2DTextureToFBOColor( m_bloomTexture );
		m_rbDepthID = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbDepthID );
		g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
		g_render.AttachRenderBufferToFBODepth( m_rbDepthID );

		for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
		{
			m_rbColorID[i] = g_render.GenerateRenderBuffer();
			g_render.BindRenderBuffer( m_rbColorID[i] );
			g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
			g_render.AttachRenderBufferToFBOColor( m_rbColorID[i], i );
		}
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
		GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT/*, GL_COLOR_ATTACHMENT8_EXT*/};
		glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			default:
				PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
				break;
		}

		g_render.BindFBO(0);
		g_render.BindRenderBuffer(0);

		//second multisample FBO for multi light passes
		m_mFboID2 = g_render.GenerateFBO();
		g_render.BindFBO( m_mFboID2 );
		m_rbDepthMID2 = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbDepthMID2 );
		g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
		g_render.AttachRenderBufferToFBODepth( m_rbDepthMID2 );

		m_rbColorID2 = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbColorID2 );
		g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
		g_render.AttachRenderBufferToFBOColor( m_rbColorID2, 0 );

		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			default:
				PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
				break;
		}

		g_render.BindFBO(0);
		g_render.BindRenderBuffer(0);

	}

	for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
	{
		glGenTextures(1, &m_textureTarget[i] );								
		glBindTexture(GL_TEXTURE_2D, m_textureTarget[i] );					
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if( i == eGBUFFER_TEXTURE_TYPE_POSITION || i == eGBUFFER_TEXTURE_TYPE_NORMAL ) //vertex or normal
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
		else
    		glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );
	}

	m_fboID = g_render.GenerateFBO();
	g_render.BindFBO( m_fboID );
	//g_render.BindForWriting( m_fboID );


	for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
	{
		g_render.Attach2DTextureToFBOColor( m_textureTarget[i], i );
	}

	m_rbDepthID2 = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthID2 );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachRenderBufferToFBODepth( m_rbDepthID2 );

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT/*, GL_COLOR_ATTACHMENT8_EXT*/};
	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	//second FBO for multi light passes
	glGenTextures(1, &m_textureTargetSwapLights );								
	glBindTexture(GL_TEXTURE_2D, m_textureTargetSwapLights );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
  	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboID2 = g_render.GenerateFBO();
	g_render.BindFBO( m_fboID2 );

	g_render.Attach2DTextureToFBOColor( m_textureTargetSwapLights, 0 );

	m_rbDepthIDSwapLights = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthIDSwapLights );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachRenderBufferToFBODepth( m_rbDepthIDSwapLights );

	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	//Final FBO for multi light passes
	glGenTextures(1, &m_textureTargetSumLighting );								
	glBindTexture(GL_TEXTURE_2D, m_textureTargetSumLighting );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
   	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboIDSum = g_render.GenerateFBO();
	g_render.BindFBO( m_fboIDSum );
	g_render.Attach2DTextureToFBOColor( m_textureTargetSumLighting, 0 );

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	g_render.BindFBO( 0 );

	glBindTexture(GL_TEXTURE_2D, 0);



	// FBO for fog and depth of field
	glGenTextures(1, &m_textureFogDof );								
	glBindTexture(GL_TEXTURE_2D, m_textureFogDof );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
   	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboIDFogDof = g_render.GenerateFBO();
	g_render.BindFBO( m_fboIDFogDof );
	g_render.Attach2DTextureToFBOColor( m_textureFogDof, 0 );
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	m_rbDepthIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthIDFogDof );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachRenderBufferToFBODepth( m_rbDepthIDFogDof );
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	//Multisample FBO for depth and DOF
	m_mFboIDFogDof = g_render.GenerateFBO();
	g_render.BindFBO( m_mFboIDFogDof );

	m_rbColorIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbColorIDFogDof );
	g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
	g_render.AttachRenderBufferToFBOColor( m_rbColorIDFogDof, 0 );

	m_rbMDepthIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbMDepthIDFogDof );
	g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
	g_render.AttachRenderBufferToFBODepth( m_rbMDepthIDFogDof );

	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	//g_render.BindFBO(0);
	g_render.BindFBO( 0 );
	g_render.BindRenderBuffer(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	return CTrue;
}

CVoid CMultipleWindows::SetInstanceCamera( CInstanceCamera * inst, CFloat sWidth, CFloat sHeight )
{
	// Get the camera from the instance and set the projection matrix from it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(inst->m_abstractCamera->GetYFov(),
					( sWidth / sHeight ),
					/*inst->m_abstractCamera->GetZNear()*/0.1,
					/*inst->m_abstractCamera->GetZFar()*/10000);

	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		//shadow
		glGetFloatv(GL_PROJECTION_MATRIX, cam_proj);
	}

	// Now get the node from the instance, and set the modelview matrix to the inverse of 
	// the node's matrix.
	glMatrixMode(GL_MODELVIEW);
	CMatrix mat;
	const CMatrix *ltow;
	ltow = inst->m_parent->GetLocalToWorldMatrix();
	CMatrixCopy(*ltow, mat); 
	//CMatrix3x4Invert( mat, mat);
	//LoadMatrix( mat );
	CVec3f at(0,0,-1),pos( 0,0,0), tAt, tPos;
	CMatrixTransform( mat, at, tAt );
	CMatrixTransform( mat, pos, tPos );
	g_camera->m_perspectiveCameraPos.x = tPos.x;
	g_camera->m_perspectiveCameraPos.y = tPos.y;
	g_camera->m_perspectiveCameraPos.z = tPos.z;

	//it's used for OpenAL
	g_camera->m_perspectiveCameraDir.x = -(tAt.x - tPos.x);
	g_camera->m_perspectiveCameraDir.y = tAt.y - tPos.y;
	g_camera->m_perspectiveCameraDir.z = -(tAt.z - tPos.z);

	//shadow 
	cam_pos[0] = g_camera->m_perspectiveCameraPos.x;
	cam_pos[1] = g_camera->m_perspectiveCameraPos.y;
	cam_pos[2] = g_camera->m_perspectiveCameraPos.z;

	cam_dir[0] = -g_camera->m_perspectiveCameraDir.x;
	cam_dir[1] = g_camera->m_perspectiveCameraDir.y;
	cam_dir[2] = -g_camera->m_perspectiveCameraDir.z;

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( tPos.x , tPos.y, tPos.z, tAt.x, tAt.y, tAt.z, 0.0, 1.0, 0.0 );
	//if( g_shadowProperties.m_enable && g_render.UsingShadowShader()  && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	//{
		// store the inverse of the resulting modelview matrix for the shadow computation
		glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
		g_camera->m_cameraManager->GetInverseMatrix( cam_inverse_modelview );
	//}

}
CVoid CMultipleWindows::UpdateAnimations()
{
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger && !g_render.GetScene()->m_isGrass )
		{
			if( g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation  )
			{
				if( m_enableTimer && g_render.GetScene()->m_animationStatus == eANIM_PLAY && g_render.GetScene()->UpdateAnimationLists() )
				{
					g_render.GetScene()->Update( elapsedTime );
					g_render.GetScene()->m_updateAnimation = CFalse;
					g_render.GetScene()->m_update = CFalse;
				} //if
				else if( g_render.GetScene()->m_update )
				{
					g_render.GetScene()->Update();
					g_render.GetScene()->m_update = CFalse;
				} //else

			} //if
			else if( g_render.GetScene()->m_update )
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			} //else
		} //if
	} //for
}

CVoid CMultipleWindows::Render3DModelsControlledByPhysX(CBool sceneManager)
{
	//3D Model data
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger && !g_render.GetScene()->m_isGrass )
		{
			if( g_render.GetScene()->m_update )
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				if( g_fogBlurPass )
					g_shaderType = g_render.m_fogBlurProgram;
				else
				if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && !Cmp(g_shadowProperties.m_directionalLightName, "\n" ))
				{
					if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_spot_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_spot_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_spot_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_spot_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_spot_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_spot_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_spot_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_spot_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_spot_prog;
							break;
						}
					}
					else
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_prog;
							break;
						}
					}
				}
				else 	if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
				{
					g_shaderType = g_render.m_spotShaderProgram;
				}
				else
					g_shaderType = g_render.m_shaderProgram;
		        glUseProgram(g_shaderType);
				glUniform1i(glGetUniformLocation(g_shaderType, "stex"), 6); // depth-maps
				glUniform4fv(glGetUniformLocation(g_shaderType, "far_d"), 1, far_bound);
				glUniform2f(glGetUniformLocation(g_shaderType, "texSize"), (float)m_dynamicShadowMap->depth_size, 1.0f/(float)m_dynamicShadowMap->depth_size);
				glUniformMatrix4fv( glGetUniformLocation( g_shaderType , "camera_inverse_matrix"), 1, CFalse, cam_inverse_modelview );

				glUniform1i(glGetUniformLocation(g_shaderType, "firstPass"), g_firstPass);
				glUniform1i(glGetUniformLocation(g_shaderType, "character_shadow"), CFalse);
				glUniform1f(glGetUniformLocation(g_shaderType, "shadow_intensity"), g_shadowProperties.m_intensity );
				if( g_currentInstanceLight )
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), g_currentInstanceLight->GetRadius());
					if( g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_POINT )
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
					else
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CFalse);
				}
				else
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), 1000000.0f);
					glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
				}

				if( g_fogBlurPass )
				{
					glUniform1f(glGetUniformLocation( g_shaderType , "focalDistance"), m_dof.m_focalDistance);
					glUniform1f(glGetUniformLocation( g_shaderType , "focalRange"), m_dof.m_focalRange);
				}


			}
			g_render.GetScene()->RenderModelsControlledByPhysX(sceneManager);

			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				glUseProgram( 0 );
			}
			
			if( g_menu.m_showBoundingBox )
			{
				g_render.GetScene()->RenderAABBWithLines();
			}
		}
	}

}

CVoid CMultipleWindows::Render3DAnimatedModels(CBool sceneManager )
{
	//3D Model data
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger && !g_render.GetScene()->m_isGrass )
		{
			if( g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation  )
			{
				if( m_enableTimer && g_render.GetScene()->m_animationStatus == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists() )
				{
					g_render.GetScene()->Update( elapsedTime );
					g_render.GetScene()->m_updateAnimation = CFalse;
					g_render.GetScene()->m_update = CFalse;
				}
				else if( g_render.GetScene()->m_update )
				{
					g_render.GetScene()->Update();
					g_render.GetScene()->m_update = CFalse;
				}

			}
			else if( g_render.GetScene()->m_update )
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				if( g_fogBlurPass )
					g_shaderType = g_render.m_fogBlurProgram;
				else
				if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && !Cmp(g_shadowProperties.m_directionalLightName, "\n" ))
				{
					if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_spot_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_spot_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_spot_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_spot_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_spot_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_spot_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_spot_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_spot_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_spot_prog;
							break;
						}
					}
					else
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_prog;
							break;
						}
					}
				}
				else 	if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
				{
					g_shaderType = g_render.m_spotShaderProgram;
				}
				else
					g_shaderType = g_render.m_shaderProgram;
		        glUseProgram(g_shaderType);
				glUniform1i(glGetUniformLocation(g_shaderType, "stex"), 6); // depth-maps
				glUniform4fv(glGetUniformLocation(g_shaderType, "far_d"), 1, far_bound);
				glUniform2f(glGetUniformLocation(g_shaderType, "texSize"), (float)m_dynamicShadowMap->depth_size, 1.0f/(float)m_dynamicShadowMap->depth_size);

				glUniformMatrix4fv( glGetUniformLocation( g_shaderType , "camera_inverse_matrix"), 1, CFalse, cam_inverse_modelview );
				glUniform1i( glGetUniformLocation( g_shaderType , "firstPass"), g_firstPass );
				glUniform1i(glGetUniformLocation(g_shaderType, "character_shadow"), CTrue);
				glUniform1f(glGetUniformLocation(g_shaderType, "shadow_intensity"), g_shadowProperties.m_intensity );

				if( g_currentInstanceLight )
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), g_currentInstanceLight->GetRadius());
					if( g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_POINT )
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
					else
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CFalse);
				}
				else
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), 1000000.0f);
					glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
				}
					
				if( g_fogBlurPass )
				{
					glUniform1f(glGetUniformLocation( g_shaderType , "focalDistance"), m_dof.m_focalDistance);
					glUniform1f(glGetUniformLocation( g_shaderType , "focalRange"), m_dof.m_focalRange);
				}
			}
			g_render.GetScene()->RenderAnimatedModels(sceneManager); 

			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				glUseProgram( 0 );
			}
			
			if( g_menu.m_showBoundingBox )
			{
				g_render.GetScene()->RenderAABBWithLines(CTrue); //Animated geometries
			}
		}
	}

}

CVoid CMultipleWindows::Render3DModels(CBool sceneManager, CChar* parentTreeNameOfGeometries )
{
	//3D Model data
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger && !g_render.GetScene()->m_isGrass )
		{
			if( g_render.GetScene()->m_update )
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				if( g_fogBlurPass )
					g_shaderType = g_render.m_fogBlurProgram;
				else
				if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && !Cmp(g_shadowProperties.m_directionalLightName, "\n" ))
				{
					if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_spot_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_spot_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_spot_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_spot_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_spot_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_spot_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_spot_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_spot_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_spot_prog;
							break;
						}
					}
					else
					{
						switch( g_shadowProperties.m_shadowType )
						{
						case eSHADOW_SINGLE_HL:
							g_shaderType = g_render.m_shad_single_hl_prog;
							break;
						case eSHADOW_SINGLE:
							g_shaderType = g_render.m_shad_single_prog;
							break;
						case eSHADOW_MULTI_LEAK:
							g_shaderType = g_render.m_shad_multi_prog;
							break;
						case eSHADOW_MULTI_NOLEAK:
							g_shaderType = g_render.m_shad_multi_noleak_prog;
							break;
						case eSHADOW_PCF:
							g_shaderType = g_render.m_shad_pcf_prog;
							break;
						case eSHADOW_PCF_TRILIN:
							g_shaderType = g_render.m_shad_pcf_trilin_prog;
							break;
						case eSHADOW_PCF_4TAP:
							g_shaderType = g_render.m_shad_pcf_4tap_prog;
							break;
						case eSHADOW_PCF_8TAP:
							g_shaderType = g_render.m_shad_pcf_8tap_prog;
							break;
						case eSHADOW_PCF_GAUSSIAN:
							g_shaderType = g_render.m_shad_pcf_gaussian_prog;
							break;
						}
					}
				}
				else 	if( g_currentInstanceLight && g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_SPOT)
				{
					g_shaderType = g_render.m_spotShaderProgram;
				}
				else
					g_shaderType = g_render.m_shaderProgram;
		        glUseProgram(g_shaderType);
				glUniform1i(glGetUniformLocation(g_shaderType, "stex"), 6); // depth-maps
				glUniform4fv(glGetUniformLocation(g_shaderType, "far_d"), 1, far_bound);
				glUniform2f(glGetUniformLocation(g_shaderType, "texSize"), (float)m_dynamicShadowMap->depth_size, 1.0f/(float)m_dynamicShadowMap->depth_size);
				glUniformMatrix4fv( glGetUniformLocation( g_shaderType , "camera_inverse_matrix"), 1, CFalse, cam_inverse_modelview );

				glUniform1i(glGetUniformLocation(g_shaderType, "firstPass"), g_firstPass);
				glUniform1i(glGetUniformLocation(g_shaderType, "character_shadow"), CFalse);
				glUniform1f(glGetUniformLocation(g_shaderType, "shadow_intensity"), g_shadowProperties.m_intensity );
				if( g_currentInstanceLight )
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), g_currentInstanceLight->GetRadius());
					if( g_currentInstanceLight->m_abstractLight->GetType() == eLIGHTTYPE_POINT )
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
					else
						glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CFalse);
				}
				else
				{
					glUniform1f(glGetUniformLocation(g_shaderType, "light_radius"), 1000000.0f);
					glUniform1f(glGetUniformLocation(g_shaderType, "pointLight"), CTrue);
				}

				if( g_fogBlurPass )
				{
					glUniform1f(glGetUniformLocation( g_shaderType , "focalDistance"), m_dof.m_focalDistance);
					glUniform1f(glGetUniformLocation( g_shaderType , "focalRange"), m_dof.m_focalRange);
				}


			}
			g_render.GetScene()->Render(sceneManager,parentTreeNameOfGeometries); 

			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
			{
				glUseProgram( 0 );
			}
			
			if( g_menu.m_showBoundingBox )
			{
				g_render.GetScene()->RenderAABBWithLines();
			}
		}
	}

	//render grass
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );

		if( g_render.GetScene()->m_isGrass )
		{
			if( g_render.GetScene()->m_update )
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
				glUseProgram( g_render.m_grassProgram );
			g_render.GetScene()->RenderGrass(sceneManager,parentTreeNameOfGeometries); 
			if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
				glUseProgram(0);
			//if( g_menu.m_showBoundingBox )
			//	g_render.GetScene()->RenderAABBWithLines();
		}
	}

}
CVoid CMultipleWindows::DrawGUI()
{
	m_timerCounter++;
	m_simpleFont.StartRendering();
	if( g_menu.m_justPerspective )
	{
		m_simpleFont.Print( "Perspective", 5.0f, 1000.0f, 0.0f, 0.85f, 0.67f, 0.0f );
		if( g_menu.m_showStatistics )
		{
			CChar temp[MAX_NAME_SIZE];
			CInt fps = (CInt)timer.GetFps(m_timerCounter);
			sprintf( temp, "FPS : %i", fps );
			m_simpleFont.Print( temp, 5.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Vertexes : %i", g_numVerts );
			m_simpleFont.Print( temp, 5.0f, 940.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Triangles : %i", g_numVerts / 3 );
			m_simpleFont.Print( temp, 5.0f, 910.0f, 0.0f, 0.85f, 0.67f, 0.0f );
		}
		if( g_translateObject )
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf( temp, "Translation Mode : X = %f  Y = %f  Z = %f\n", g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z );
			m_simpleFont.Print( temp, 5.0f, 50.0f, 0.0f, 0.55f, 0.37f, 0.0f );
		}

	}
	else
	{
		m_simpleFont.Print( "Perspective", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );

		if( g_menu.m_showStatistics )
		{
			CChar temp[MAX_NAME_SIZE];
			CInt fps = (CInt)(timer.GetFps(m_timerCounter));
			sprintf( temp, "FPS : %i", fps );
			m_simpleFont.Print( temp, 10.0f, 920.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Vertexes : %i", g_numVerts );
			m_simpleFont.Print( temp, 10.0f, 870.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Triangles : %i", g_numVerts / 3 );
			m_simpleFont.Print( temp, 10.0f, 820.0f, 0.0f, 0.85f, 0.67f, 0.0f );
		}
		if( g_translateObject )
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf( temp, "Translation Mode : X = %f  Y = %f  Z = %f\n", g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z );
			m_simpleFont.Print( temp, 10.0f, 50.0f, 0.0f, 0.55f, 0.37f, 0.0f );
		}

	}
	m_simpleFont.EndRendering();

	if( m_totalElapsedTime >= 5 ) //Every 5 seconds update the FPS
	{
		SetElapsedTimeFromBeginning();
	}
}

CVoid CMultipleWindows::BlendFogWithScene()
{
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading( m_mFboID );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		g_render.BindForReading( m_mFboIDFogDof );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboIDFogDof );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureFogDof);

	g_render.BindForWriting( m_fboIDSum );

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(g_render.m_blendTexturesProgram);
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_0"), 0 );
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_1"), 1 );
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "blendLights"), CFalse );


	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	gluOrtho2D(0, m_width, 0, m_height);
	glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2i(0,	0);	glVertex2i(0, 0); 
	glTexCoord2i(1, 0);	glVertex2i(g_width, 0);
	glTexCoord2i(1, 1);	glVertex2i(g_width, g_height);
	glTexCoord2i(0, 1);	glVertex2i(0, g_height);
	glEnd();
	glFlush();

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTextureARB(GL_TEXTURE1_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	g_render.BindFBO(0);

	//copy  sum to the default FBO
	g_render.BindForReading( m_fboIDSum );
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_mFboID );
	}
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_fboID );
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	if ( g_render.UsingFBOs() && g_options.m_enableFBO )
		glBlitFramebufferEXT(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

CVoid CMultipleWindows::BlendLights(CUInt lightIndex)
{
	if( lightIndex == 0 ) return;
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading( m_mFboID );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		g_render.BindForReading( m_mFboID2 );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID2 );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureTargetSwapLights);

	g_render.BindForWriting( m_fboIDSum );

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(g_render.m_blendTexturesProgram);
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_0"), 0 );
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_1"), 1 );
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "blendLights"), CTrue );


	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	gluOrtho2D(0, m_width, 0, m_height);
	glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2i(0,	0);	glVertex2i(0, 0); 
	glTexCoord2i(1, 0);	glVertex2i(g_width, 0);
	glTexCoord2i(1, 1);	glVertex2i(g_width, g_height);
	glTexCoord2i(0, 1);	glVertex2i(0, g_height);
	glEnd();
	glFlush();

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTextureARB(GL_TEXTURE1_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	g_render.BindFBO(0);

	//copy light sum to the default FBO
	g_render.BindForReading( m_fboIDSum );
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		if( lightIndex % 2  == 0 )
		{
			g_render.BindForWriting( m_mFboID );
		}
		else
		{
			g_render.BindForWriting( m_mFboID2 );
		}
	}
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		if( lightIndex % 2  == 0 )
			g_render.BindForWriting( m_fboID );
		else
			g_render.BindForWriting( m_fboID2 );
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	if ( g_render.UsingFBOs() && g_options.m_enableFBO )
		glBlitFramebufferEXT(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

CVoid CMultipleWindows::EngineLightPass()
{
	if( !g_useOldRenderingStyle && g_options.m_enableFBO && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader && g_engineLights.size() > 0 )
	{
		//set all the lights here
		for( CUInt j = 0; j < g_engineLights.size(); j++ )
		{
			CInstanceLight *instanceLight = g_engineLights[j];
			g_currentInstanceLight = instanceLight;

			CVec3f  Position;
			if( instanceLight->m_parent )
			{
				float *matrix = (float *)instanceLight->m_parent->GetLocalToWorldMatrix();
				Position.x = matrix[12]; Position.y = matrix[13]; Position.z =  matrix[14]; 
			}
			else
			{
				Position.x = instanceLight->m_abstractLight->GetPosition()[0];
				Position.y = instanceLight->m_abstractLight->GetPosition()[1];
				Position.z = instanceLight->m_abstractLight->GetPosition()[2];
			}

			if( g_camera->m_cameraManager->IsSphereInFrustum( Position.x, Position.y, Position.z, (CFloat)instanceLight->GetRadius() ) )
			{
				if( Cmp(g_shadowProperties.m_directionalLightName, g_engineLights[j]->m_abstractLight->GetName() ) )
				{
					g_firstPass = CTrue;
				//	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
				//	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT/*, GL_COLOR_ATTACHMENT8_EXT*/};
				//	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);
				}
				else
				{
					g_firstPass = CFalse;
				}
				glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

				if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				{
					if( g_numLights % 2 == 0 )
						g_render.BindForWriting( m_mFboID );
					else
						g_render.BindForWriting( m_mFboID2 );
				}
				else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
				{
					if( g_numLights % 2 == 0 )
						g_render.BindForWriting( m_fboID );
					else
						g_render.BindForWriting( m_fboID2 );
				}
				if( g_numLights != 0 )
				{
					glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				}
				glViewport( 0, 0, m_width, m_height );

				if( g_numLights < 8 ) // 8 lights are supported
				{
					CBool defaultLight;
					if( Cmp(g_shadowProperties.m_directionalLightName, g_engineLights[j]->m_abstractLight->GetName() ) || ( Cmp(g_shadowProperties.m_directionalLightName, "\n" ) && g_engineLights[j]->m_abstractLight->GetType() == eLIGHTTYPE_DIRECTIONAL ))
						defaultLight = CTrue;
					else
						defaultLight = CFalse;
					if( g_render.SetInstanceLight(g_engineLights[j],0, defaultLight) )
					{
						if( defaultLight )
							Cpy( g_shadowProperties.m_directionalLightName, g_engineLights[j]->m_abstractLight->GetName() );
						g_octree->Render();
						g_multipleView->Render3DAnimatedModels( CTrue);
						g_multipleView->Render3DModelsControlledByPhysX();
						BlendLights(g_numLights);
						++g_numLights;
						g_octree->ResetOctreeGeoCount();
					}
				}
			}
		}
		if( g_numLights > 1 && (g_numLights % 2 == 0) )
		{
			if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				g_render.BindForReading( m_mFboID2 );
			else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
				g_render.BindForReading( m_fboID2 );

			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

			if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				g_render.BindForWriting( m_mFboID );
			else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
				g_render.BindForWriting( m_fboID );

			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			if ( g_render.UsingFBOs() && g_options.m_enableFBO )
			{
				glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
			}
		}
	} //if !g_useOldRenderingStyle
	else if( g_engineLights.size() > 0 && ( g_useOldRenderingStyle || !g_options.m_enableFBO ) && g_render.UsingShader() && g_render.m_useShader )//old rendering style
	{
		glDrawBuffer( GL_BACK );
		//set all the lights here
		for( CUInt j = 0; j < g_engineLights.size(); j++ )
		{
			if( j == 0 )
			if( g_numLights < 1 ) // 1 lights is supported
			{
				if( g_render.SetInstanceLight(g_engineLights[j],0, CTrue) )
				{
					g_octree->Render();
					g_multipleView->Render3DAnimatedModels( CTrue);
					g_multipleView->Render3DModelsControlledByPhysX();
					g_octree->ResetOctreeGeoCount();
					++g_numLights;
				}
			}
		}
	}
	else
	{
		for( CUInt j = 0; j < g_engineLights.size(); j++ )
		{
			if( g_numLights < 8 ) // 8 lights are supported
			{
				if( g_render.SetInstanceLight(g_engineLights[j],g_numLights, CTrue) )
				{
					++g_numLights;
				}
			}
		}
	}
}

CVoid CMultipleWindows::COLLADALightPass()
{
	if( !g_useOldRenderingStyle && g_options.m_enableFBO && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
		//COLLADA Lights. Not recommended
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			
			//set all the lights here
			for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
			{
				CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
				if( instanceLight->m_abstractLight->GetType() == eLIGHTTYPE_AMBIENT )
					continue;
				g_currentInstanceLight = instanceLight;

				CVec3f  Position;
				if( instanceLight->m_parent )
				{
					float *matrix = (float *)instanceLight->m_parent->GetLocalToWorldMatrix();
					Position.x = matrix[12]; Position.y = matrix[13]; Position.z =  matrix[14]; 
				}
				else
				{
					Position.x = instanceLight->m_abstractLight->GetPosition()[0];
					Position.y = instanceLight->m_abstractLight->GetPosition()[1];
					Position.z = instanceLight->m_abstractLight->GetPosition()[2];
				}

				if( g_camera->m_cameraManager->IsSphereInFrustum( Position.x, Position.y, Position.z, (CFloat)instanceLight->GetRadius() ) )
				{
					if( Cmp(g_shadowProperties.m_directionalLightName, instanceLight->m_abstractLight->GetName() ) )
					{
						g_firstPass = CTrue;
					//	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
					//	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT/*, GL_COLOR_ATTACHMENT8_EXT*/};
					//	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);
					}
					else
					{
						g_firstPass = CFalse;
					}
					glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

					if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
					{
						if( g_numLights % 2 == 0 )
							g_render.BindForWriting( m_mFboID );
						else
							g_render.BindForWriting( m_mFboID2 );
					}
					else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
					{
						if( g_numLights % 2 == 0 )
							g_render.BindForWriting( m_fboID );
						else
							g_render.BindForWriting( m_fboID2 );
					}
					if( g_numLights != 0 ) //clear m_fboID2 or m_mFboID2
					{
						glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
						glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					}
					glViewport( 0, 0, m_width, m_height );

					if( g_numLights < 8 ) // 8 lights are supported
					{
						if( instanceLight )
						{
							CBool defaultLight;
							if( Cmp(g_shadowProperties.m_directionalLightName, instanceLight->m_abstractLight->GetName() ) || ( Cmp(g_shadowProperties.m_directionalLightName, "\n" ) && instanceLight->m_abstractLight->GetType() == eLIGHTTYPE_DIRECTIONAL ))
								defaultLight = CTrue;
							else
								defaultLight = CFalse;
							if( g_render.SetInstanceLight(instanceLight,0, defaultLight) )
							{
								if( defaultLight )
									Cpy( g_shadowProperties.m_directionalLightName, instanceLight->m_abstractLight->GetName() );
								g_octree->Render();
								g_multipleView->Render3DAnimatedModels( CTrue);
								g_multipleView->Render3DModelsControlledByPhysX();
								BlendLights(g_numLights);
								++g_numLights;
								g_octree->ResetOctreeGeoCount();
								g_render.SetScene( g_scene[i] );
							}
						}
					}
				}
			}
		}
		if( g_numLights > 1 && (g_numLights % 2 == 0) )
		{
			if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				g_render.BindForReading( m_mFboID2 );
			else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
				g_render.BindForReading( m_fboID2 );

			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

			if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				g_render.BindForWriting( m_mFboID );
			else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
				g_render.BindForWriting( m_fboID );

			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

			if ( g_render.UsingFBOs() && g_options.m_enableFBO )
			{
				glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
			}
		}
	} //if !g_useOldRenderingStyle
	else if( ( g_useOldRenderingStyle || !g_options.m_enableFBO ) && g_render.UsingShader() && g_render.m_useShader )//old rendering style
	{
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			
			//set all the lights here
			for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
			{
				if( j == 0 )
				{
					if( g_numLights < 1 ) // 1 lights is supported
					{
						CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
						if( instanceLight )
						{
							if( g_render.SetInstanceLight(instanceLight,0, CTrue) )
							{
								g_octree->Render();
								g_multipleView->Render3DAnimatedModels( CTrue);
								g_multipleView->Render3DModelsControlledByPhysX();
								g_octree->ResetOctreeGeoCount();
								++g_numLights;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			
			//set all the lights here
			for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
			{
				if( g_numLights < 8 ) // 8 lights are supported
				{
					CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
					if( instanceLight )
					{
						if( g_render.SetInstanceLight(instanceLight,g_numLights, CTrue) )
						{
							++g_numLights;
						}
					}
				}
			}
		}
	}
}

CVoid CMultipleWindows::FixedFunctionLightPass()
{
	if( !g_render.UsingShader() || !g_render.m_useShader || !g_options.m_enableShader)
	{
		g_octree->Render();
		g_multipleView->Render3DAnimatedModels( CTrue);
		g_multipleView->Render3DModelsControlledByPhysX();

	}
}

CVoid CMultipleWindows::DefaultLightPass()
{
	if( g_numLights == 0 )
	{
		//glViewport( 0, 0, m_width, m_height );

		glEnable(GL_LIGHT0);	 //just for per vertex lighting 	

		//This is the properties of the camera light
		GLfloat light_pos0[4] = {g_camera->m_perspectiveCameraPos.x,g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z, 1.0f };
		//g_defaultDirectionalLight.x = light_pos0[0]; g_defaultDirectionalLight.y = light_pos0[1]; g_defaultDirectionalLight.z = light_pos0[2]; g_defaultDirectionalLight.w = 0.0f;

		GLfloat light_ambient0[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
		GLfloat light_diffuse0[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		GLfloat light_specular0[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
		GLfloat light_shininess0 = 5.0f;

		glLightfv( GL_LIGHT0, GL_POSITION, light_pos0  );
		glLightfv( GL_LIGHT0, GL_AMBIENT , light_ambient0 );
		glLightfv( GL_LIGHT0, GL_DIFFUSE , light_diffuse0 );
		glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular0 );
		glLightf ( GL_LIGHT0, GL_SHININESS, light_shininess0 );
		//glLightf ( GL_LIGHT0, GL_SPOT_CUTOFF,(GLfloat)180.0f );
		glLightf ( GL_LIGHT0, GL_CONSTANT_ATTENUATION , (GLfloat)1.0f );
		glLightf ( GL_LIGHT0, GL_LINEAR_ATTENUATION,	(GLfloat)0.0f );
		glLightf ( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, (GLfloat)0.0f );

		g_octree->Render();
		g_multipleView->Render3DAnimatedModels( CTrue);
		g_multipleView->Render3DModelsControlledByPhysX();

		if( g_options.m_enableShader  && g_render.UsingShader()  && g_render.m_useShader )
			g_octree->ResetOctreeGeoCount();
		g_numLights++;
	}
}