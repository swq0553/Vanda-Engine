//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "VandaEngine1Win32.h"
#include "inputEngine/input.h"
#include "timerEngine/timer.h"
#include "graphicsEngine/render.h"
#include "graphicsEngine/OpenGL.h"
#include "graphicsEngine/Fog.h"
#include "graphicsEngine/Water.h"
#include "graphicsEngine/Scene.h"
#include "graphicsEngine/Bloom.h"
#include "graphicsEngine/Sky.h"
//#include "graphicsEngine/imagelib.h"
#include "graphicsEngine/shadowMap.h"
#include "graphicsEngine/particleSystem.h"
#include "graphicsEngine/dof.h"
#include "graphicsEngine/simpleFont.h"
#include "PhysXEngine/Nx.h"
#include "AudioEngine/openal.h"
#include "AudioEngine/OpenALSystem.h"
#include "AudioEngine/OpenALSoundBuffer.h"
#include "AudioEngine/OpenALSoundSource.h"
#include "AudioEngine/StaticSound.h"
#include "AudioEngine/AmbientSound.h"
struct CPhysXVariables
{
	CBool m_applyDirectionalForce;
	NxVec3 m_forceDirectionVector;
	CFloat m_forceSpeedValue;
	CFloat m_forceDecreaseValue;

	CPhysXVariables()
	{
		m_applyDirectionalForce = CFalse;
		m_forceDecreaseValue = 0.0f;
		m_forceSpeedValue = 0.0f;
		m_forceDirectionVector = NxVec3( 0.0f, 0.0f, 0.0f );
	}
	~CPhysXVariables()
	{
	}
};

class CMain
{
public:
	CMain();
	~CMain();
	CBool Init();
	CBool Render();
	CVoid Release();
	CVoid DrawGrid(CVoid);
	CBool Load( CChar* pathName );
	CBool Reset();
	CVoid ResetTimer();
	CParticleSystem* m_particleSystem;

	//shadow
	float cam_proj[16];
	float cam_modelview[16];
	float cam_inverse_modelview[16];
	float cam_pos[3];
	float cam_dir[3];
	float far_bound[MAX_SPLITS];
	CVoid Render3DModels(CBool sceneManager, CChar* parentTreeNameOfGeometries );
	CVoid Render3DAnimatedModels(CBool sceneManager );
	CVoid Render3DModelsControlledByPhysX(CBool sceneManager = CTrue);

	CVoid UpdateAnimations();
	CVoid DrawGUI();
	void ResetPhysX();

	CVoid EngineLightPass();
	CVoid COLLADALightPass();
	CVoid FixedFunctionLightPass();
	CVoid DefaultLightPass();

	CVoid BlendLights(CUInt lightIndex);
	CVoid BlendFogWithScene();
	CVoid ResetPhysXCounts();
private:
	CPhysXVariables m_physXVariables;

	//Audio variables 
	ALfloat m_ambientSourcePos[3];
	// Velocity of the source sound.
	ALfloat m_ambientSourceVel[3];
	// Position of the listener.
	ALfloat m_listenerPos[3];
	// Velocity of the listener.
	ALfloat m_listenerVel[3];
	// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	ALfloat m_listenerOri[6];
	CAmbientSound * m_ambientSound;
	COpenALSystem* m_soundSystem;
	CFloat elapsedTime;
	__int64 m_timerCounter; //To get FPS
	CFloat m_totalElapsedTime; //To get FPS
	CDOF m_dof;
	CFloat m_cameraInverse[16];

	CUInt m_mFboID; //multisample FBO 
	CUInt m_rbDepthID; //attach a render buffer to the depth buffer of multisample FBO
	CUInt m_rbColorID[eGBUFFER_NUM_TEXTURES]; //attach a render buffer to the color buffers of multisample FBO

	CUInt m_mFboID2; //multisample FBO 
	CUInt m_rbDepthMID2; //attach a render buffer to the depth buffer of multisample FBO
	CUInt m_rbColorID2; //attach a render buffer to the color buffers of multisample FBO

	CUInt m_fboID; //each texture has its own FBO ID
	CUInt m_textureTarget[eGBUFFER_NUM_TEXTURES];
	CUInt m_rbDepthID2; //It's used when multisampling is disabled

	CUInt m_fboID2; //each texture has its own FBO ID
	CUInt m_textureTargetSwapLights;
	CUInt m_rbDepthIDSwapLights; //It's used when multisampling is disabled

	CUInt m_fboIDSum; //each texture has its own FBO ID
	CUInt m_textureTargetSumLighting;

	CUInt m_fboIDFogDof;
	CUInt m_textureFogDof;
	CUInt m_rbDepthIDFogDof;

	CUInt m_rbColorIDFogDof;
	CUInt m_mFboIDFogDof;
	CUInt m_rbMDepthIDFogDof;

	CBool InitFBOs( CInt channels, CInt type );
	CVoid InitDynamicShadowMap(CVec3f lightPos, CVec3f atPos );
	CVoid SetInstanceCamera( CInstanceCamera * inst, CFloat sWidth, CFloat sHeight );
private:
	CBool ProcessInputs();
	CVoid	ApplyForce( /*NxVec3 forceDirection, */CInt moveDirection, CFloat elapsedTime );
	COpenALSoundBuffer* GetSoundBuffer( const CChar * name );
	CShadowMap* m_dynamicShadowMap;

private:
	CBool m_showHelpInfo;

public:
	CBool m_lockInput;
	CBool m_loadScene;

};
