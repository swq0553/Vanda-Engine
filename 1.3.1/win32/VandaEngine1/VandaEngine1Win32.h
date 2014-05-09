//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once

#include "GraphicsEngine/Window.h"
#include "GraphicsEngine/Render.h"
#include "graphicsEngine/render.h"
#include "GraphicsEngine/image.h"
#include "GraphicsEngine/shadowMap.h"
#include "GraphicsEngine/simplefont.h"
#include "inputEngine/input.h"
#include "timerEngine/timer.h"
#include "physXEngine/nx.h"
#include "GraphicsEngine/camera.h"
#include "scenemanagerEngine/octree.h"
#include "common/luaForCpp.h"
#include "AudioEngine/openAL.h"
#include "AudioEngine/OpenALSystem.h"
#include "AudioEngine/OpenALSoundBuffer.h"
#include "AudioEngine/OpenALSoundSource.h"
#include "AudioEngine/StaticSound.h"
#include "resource.h"
#include <vector>

class CInstanceLight;
class CWater;
class CStaticSound;
class CScene;
class CImage;
class CBloom;
class CSkyDome;
class COctree;
class CMain;
struct CUpdateCamera;
class COpenALSoundBuffer;
class CInstanceCamera;
//Edition.MaxVersion.MinVersion.BugFixes;
//For example: RTI.1.0.0
extern CInt g_maxVersion;
extern CInt g_minVersion;
extern CInt g_bugFixesVersion;
extern CChar g_edition[MAX_NAME_SIZE];
extern CScene* g_currentScene; //current scene that we are working on it.
extern CChar g_currentVSceneName[MAX_NAME_SIZE]; //save functions
extern CChar g_currentVSceneNameViaScript[MAX_NAME_SIZE];
extern CBool g_loadSceneViaScript;
extern std::vector<COpenALSoundBuffer*>g_soundBuffers;
extern CChar g_currentPassword[MAX_NAME_SIZE];
extern std::vector<CScene*> g_scene;
extern std::vector<CInstanceLight*> g_engineLights; //these aren't collada lights, they are specified via the editor
extern std::vector<CWater*> g_engineWaters; //We may use multiple water surfaces in our engine
extern std::vector<CStaticSound*>g_engineStaticSounds;
extern CUpdateCamera *g_camera;
extern COctree* g_octree;
extern CShadowMap* g_dynamicShadowMap;
extern GLuint g_shaderType;
extern CSkyDome *g_skyDome;   //Currently each map has just one sky, But I may decide to add extra layers later
extern CBloom*	g_bloom;
extern std::vector<CImage*>g_images; // This variable holds the information of all the images of Engine
extern std::vector<CImage*>g_waterImages; // This variable holds the information of water images of Engine
extern CInt g_numLights;
extern CInt g_totalLights;
extern CWindow g_window; 
extern HINSTANCE g_instance;
extern CInputSystem g_input;
extern CTimer* g_timer;
extern CNovodex* g_nx;
extern CFont* g_font;
extern CInt g_width;                   //default g_width of our screen
extern CInt g_height;                   //default Height of our screen
extern CBool g_useOldRenderingStyle;
extern CFloat g_elapsedTime;
extern CBool g_updateOctree;
extern CMain* g_main;
extern CBool g_useGlobalAmbientColor;
extern CColor4f g_globalAmbientColor;
extern CLuaState g_lua;
extern COpenALSystem* g_soundSystem;
extern std::vector<CInstanceCamera*> g_cameraInstances;
struct COptions
{
	CInt m_width;
	CInt m_height;
	CInt m_numSamples;
	CInt m_anisotropy;
	CBool m_showStartupDialog;
	CBool m_disableVSync;
	CBool m_enableWaterReflection;
	CBool m_enableShader;
	CBool m_enableFBO;
	CBool m_enableVBO;

	COptions()
	{
		m_width = 640;
		m_height = 480;
		m_numSamples = 4;
		m_anisotropy = 4;
		m_showStartupDialog = CTrue;
		m_disableVSync = CTrue;
		m_enableWaterReflection = CTrue;
		m_enableShader = CTrue;
		m_enableFBO = CTrue;
		m_enableVBO = CTrue;
	}
};

struct CExtraTexturesNamingConventions
{
	CChar m_diffuseNameExcept[MAX_NAME_SIZE];
	CChar m_alphaMapName[MAX_NAME_SIZE];
	CChar m_glossMapName[MAX_NAME_SIZE];
	CChar m_normalMapName[MAX_NAME_SIZE];
	CChar m_shadowMapName[MAX_NAME_SIZE];
	CChar m_dirtMapName[MAX_NAME_SIZE];

	CExtraTexturesNamingConventions()
	{
		Cpy( m_diffuseNameExcept, "" );
		Cpy( m_alphaMapName, "_a" );
		Cpy( m_glossMapName, "_g" );
		Cpy( m_normalMapName, "_n" );
		Cpy( m_shadowMapName, "_s" );
		Cpy( m_dirtMapName, "_d" );
	}

	CVoid Reset()
	{
		Cpy( m_diffuseNameExcept, "" );
		Cpy( m_alphaMapName, "_a" );
		Cpy( m_glossMapName, "_g" );
		Cpy( m_normalMapName, "_n" );
		Cpy( m_shadowMapName, "_s" );
		Cpy( m_dirtMapName, "_d" );
	}
};

struct CPathProperties
{
	CChar m_meshPath[MAX_NAME_SIZE];
	CChar m_meshDiffuseTexturePath[MAX_NAME_SIZE];
	CChar m_meshOtherTexturesPath[MAX_NAME_SIZE];
	CChar m_waterPath[MAX_NAME_SIZE];
	CChar m_skyPath[MAX_NAME_SIZE];
	CChar m_soundPath[MAX_NAME_SIZE];
	CChar m_physXPath[MAX_NAME_SIZE];

	CPathProperties()
	{
		Cpy( m_meshOtherTexturesPath, "Assets/Textures/Extra/" );
		Cpy( m_meshDiffuseTexturePath, "Assets/Textures/Diffuse/" );
		Cpy( m_skyPath, "Assets/Textures/sky/" );
		Cpy( m_waterPath, "Assets/Textures/water/" );
		Cpy( m_meshPath, "Assets/meshes/" );
		Cpy( m_soundPath, "Assets/sounds/" );
		Cpy( m_physXPath, "Assets/physx/" );
	}

	CVoid Reset()
	{
		Cpy( m_meshOtherTexturesPath, "Assets/Textures/Extra/" );
		Cpy( m_meshDiffuseTexturePath, "Assets/Textures/Diffuse/" );
		Cpy( m_skyPath, "Assets/Engine/Textures/Sky/" );
		Cpy( m_waterPath, "Assets/Engine/Textures/Water/" );
		Cpy( m_meshPath, "Assets/Meshes/" );
		Cpy( m_soundPath, "Assets/Engine/Sounds/" );
		Cpy( m_physXPath, "Assets/Engine/PhysX/" );
	}
};
struct CLightProperties
{
	CFloat m_ambientColor[4];
	CFloat m_specularColor[4];
	CFloat m_shininess;
	CFloat m_tolerance;

	CLightProperties()
	{
		for(CInt i = 0; i < 3; i++ )
		{
			m_ambientColor[i] = 1.0f;
			m_specularColor[i] = 0.6f;
		}
		m_ambientColor[3] = m_specularColor[3] = 1.0;
		m_tolerance = 0.2f;
		m_shininess = 50.0f;
	}

	CVoid Reset()
	{
		for(CInt i = 0; i < 3; i++ )
		{
			m_ambientColor[i] = 1.0f;
			m_specularColor[i] = 0.6f;
		}
		m_ambientColor[3] = m_specularColor[3] = 1.0;
		m_tolerance = 0.2f;
		m_shininess = 50.0f;
	}

};

struct CShadowProperties
{
	CFloat m_shadowSplitWeight;
	CFloat m_shadowNearClipPlane;
	CFloat m_shadowFarClipPlane;
	CShadowType m_shadowType;
	CShadowSplits m_shadowSplits;
	CShadowResolution m_shadowResolution;
	CFloat m_intensity;
	CBool m_enable;
	CChar m_directionalLightName[MAX_NAME_SIZE];

	CShadowProperties()
	{
		m_shadowSplitWeight = 0.3f;
		m_shadowNearClipPlane = 0.1f;
		m_shadowFarClipPlane = 30.f;
		m_shadowType = eSHADOW_PCF_GAUSSIAN;
		m_shadowSplits = eSHADOW_3_SPLITS; //3 splits
		m_shadowResolution = eSHADOW_1024;
		m_intensity = 0.5f;
		m_enable = CFalse;
		Cpy( m_directionalLightName, "\n" );
	}

	CVoid Reset()
	{
		m_shadowSplitWeight = 0.3f;
		m_shadowNearClipPlane = 0.1f;
		m_shadowFarClipPlane = 200.f;
		m_shadowType = eSHADOW_PCF_GAUSSIAN;
		m_shadowSplits = eSHADOW_3_SPLITS; //3 splits
		m_shadowResolution = eSHADOW_1024;
		m_intensity = 0.5f;
		m_enable = CFalse;
		Cpy( m_directionalLightName, "\n" );
	}
};

struct CDOFProperties
{
	CBool m_enable;
	CFloat m_dofFocalDistance;
	CFloat m_dofFocalRange;
	CBool m_debug;

	CDOFProperties()
	{
		m_dofFocalDistance = 5.0f;
		m_dofFocalRange = 3.0f;
		m_enable = CFalse;
		m_debug = CFalse;
	}

	CVoid Reset()
	{
		m_dofFocalDistance = 5.0f;
		m_dofFocalRange = 3.0f;
		m_enable = CFalse;
		m_debug = CFalse;
	}

};

struct CFogProperties
{
	CBool m_enable;
	CFloat m_fogDensity;
	CFloat m_fogColor[4];

	CFogProperties()
	{
		m_enable = CFalse;
		m_fogDensity = 0.1f;
		for( CInt i = 0; i < 4; i++ )
		{
			if( i < 3 )
				m_fogColor[i] = 0.5f;
			else
				m_fogColor[i] = 1.0f;
		}
	}

	CVoid Reset()
	{
		m_enable = CFalse;
		m_fogDensity = 0.1f;
		for( CInt i = 0; i < 4; i++ )
		{
			if( i < 3 )
				m_fogColor[i] = 0.5f;
			else
				m_fogColor[i] = 1.0f;
		}
	}

};

struct CBloomProperties
{
	CBool m_enable;
	CFloat m_bloomIntensity;
	CFloat m_bloomColor[3];

	CBloomProperties()
	{
		m_enable = CTrue;
		m_bloomIntensity = 1.0f;
		for( CInt i = 0; i < 3; i++ )
		{
			m_bloomColor[i] = 1.0f;
		}
	}

	CVoid Reset()
	{
		m_enable = CTrue;
		m_bloomIntensity = 1.0f;
		for( CInt i = 0; i < 3; i++ )
		{
			m_bloomColor[i] = 1.0f;
		}
	}

};

struct CDatabaseVariables
{
	CBool m_useShader; //deprecate
	CBool m_multisampling; //deprecate
	CBool m_fog; //deprecate	
	CBool m_dof; //deprecate
	CBool m_showPerspectiveGrids;
	CBool m_showLightPositions;
	CBool m_showSoundPositions;
	CBool m_showBoundingBox;
	CBool m_showStatistics;
	CBool m_enableDynamicShadow; //deprecate
	CBool m_insertAndShowSky;
	CBool m_insertAmbientSound;
	CBool m_physicsDebugMode;
	CBool m_physicsGravity;
	CBool m_playAmbientSound;

	CDatabaseVariables()
	{
		m_useShader = CTrue;
		m_multisampling = CTrue;
		m_fog = CTrue;

	   m_showPerspectiveGrids = CFalse;
	   m_showLightPositions = CFalse;
	   m_showSoundPositions = CFalse;
	   m_showBoundingBox = CFalse;
	   m_showStatistics = CFalse;
	   m_enableDynamicShadow = CTrue;
	   m_insertAndShowSky = CFalse;
	   m_insertAmbientSound = CFalse;
	   m_physicsDebugMode = CFalse;
	   m_physicsGravity = CFalse;
	   m_playAmbientSound = CFalse;
	   m_dof = CFalse;
	}

	CVoid Reset()
	{
		m_useShader = CTrue;
		m_multisampling = CTrue;
		m_fog = CTrue;

	   m_showPerspectiveGrids = CFalse;
	   m_showLightPositions = CFalse;
	   m_showSoundPositions = CFalse;
	   m_showBoundingBox = CFalse;
	   m_showStatistics = CFalse;
	   m_enableDynamicShadow = CTrue;
	   m_insertAndShowSky = CFalse;
	   m_insertAmbientSound = CFalse;
	   m_physicsDebugMode = CFalse;
	   m_physicsGravity = CFalse;
	   m_playAmbientSound = CFalse;
	   m_dof = CFalse;
	}

};
extern CDatabaseVariables g_databaseVariables;
extern CShadowProperties g_shadowProperties;
extern CDOFProperties g_dofProperties;
extern CFogProperties g_fogProperties;
extern CBloomProperties g_bloomProperties;
extern CLightProperties g_lightProperties;
extern CPathProperties g_pathProperties;
extern CExtraTexturesNamingConventions g_extraTexturesNamingConventions;
extern COptions g_options;
extern CVec4f g_defaultDirectionalLight;
extern CInstanceLight* g_currentInstanceLight;
extern CBool g_firstPass;
extern CBool g_fogBlurPass;
extern CInt g_sceneManagerObjectsPerSplit;
