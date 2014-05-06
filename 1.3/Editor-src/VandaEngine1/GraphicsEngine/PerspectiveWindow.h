//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "..\PhysXEngine\Nx.h"
#include "..\InputEngine\input.h"
#include "..\timerEngine\timer.h"
#include "light.h"
#include "bloom.h"
#include "ShadowMap.h"
#include "camera.h"
#include "sky.h"
#include "dof.h"
#include "..\AudioEngine/openal.h"
#include "..\AudioEngine/OpenALSystem.h"
#include "..\AudioEngine/ambientSound.h"
#include "..\GraphicsEngine/SimpleFont.h"
#include "../MouseTranslationController.h"
// CMultipleWindows

struct CUpdateCamera
{
	CCamera *m_cameraManager; //Default camera of the scene, I don't use the external cameras of the COLLADA file

	NxVec3 m_perspectiveCameraPos;
	NxVec3 m_perspectiveCameraDir;
	NxVec3 m_perspectiveCharacterPos; //equals AtPos
	CFloat m_perspectiveCameraYaw;
	CFloat m_perspectiveCameraPitch;
	CFloat m_perspectiveCameraTilt; //Currently m_tilt must  be passed directly to UpdateCameraPosDir(), I should find a way to calculate the tilt inside the GetCameraAndCharaterPosition().
	CDouble m_perspectiveCameraZoom;

	//camera properties of the Lower right viewport 
	CFloat m_lowerRightMoveHorizantal; 
	CFloat m_lowerRightMoveVertical;
	CFloat m_lowerRightZoom;

	//camera properties of the upper right viewport 
	CFloat m_upperRightMoveHorizantal; 
	CFloat m_upperRightMoveVertical;
	CFloat m_upperRightZoom;

	//camera properties of the upper left viewport 
	CFloat m_upperLeftMoveHorizantal; 
	CFloat m_upperLeftMoveVertical;
	CFloat m_upperLeftZoom;

	//Appropriate camera is activated based on the position of the mouse with left or right mouse button clicks
	//If one camera is activated, others are deactivated
	CBool m_activatePerspectiveCamera; 
	CBool m_activateLowerRightCamera;
	CBool m_activateUpperLeftCamera;
	CBool m_activateUpperRightCamera;

	CUpdateCamera()
	{
		m_cameraManager = new CCamera();

		m_perspectiveCameraYaw = 0.8f;
		m_perspectiveCameraPitch = 0.0f;
		m_perspectiveCameraTilt = 0.5f;
		m_perspectiveCameraZoom = 0.0;

		m_lowerRightMoveHorizantal = 0.0f; 
		m_lowerRightMoveVertical = 0.0f;
		m_lowerRightZoom = 0.03f;

		m_upperRightMoveHorizantal = 0.0f; 
		m_upperRightMoveVertical = 0.0f;
		m_upperRightZoom = 0.03f;

		m_upperLeftMoveHorizantal = 0.0f; 
		m_upperLeftMoveVertical = 0.0f;
		m_upperLeftZoom = 0.03f;

		m_activatePerspectiveCamera = CFalse; 
		m_activateLowerRightCamera = CFalse;
		m_activateUpperLeftCamera = CFalse;
		m_activateUpperRightCamera = CFalse;
	}
	~CUpdateCamera()
	{
		CDelete( m_cameraManager );
	}
};

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


class CMultipleWindows : public CWnd
{
	DECLARE_DYNAMIC(CMultipleWindows)

public:
	CMultipleWindows();
	virtual ~CMultipleWindows();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg CInt OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg CVoid OnSize(UINT nType, CInt cx, CInt cy);
	afx_msg CVoid OnDestroy();
	afx_msg CVoid OnPaint();
	afx_msg CVoid OnMouseMove(UINT nFlags, CPoint point);
	afx_msg CVoid OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg CVoid OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg CVoid OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg CVoid OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg CVoid OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg CVoid OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg CVoid OnTimer(UINT_PTR nIDEvent);

	CVoid ActivateCamera( CPoint point );
	CVoid DrawGrid( CVoid );
	CVoid RenderWindow();
	CVoid DrawUpperLeft();
	CVoid DrawUpperRight();
	CVoid DrawLowerRight();
	CVoid DrawPerspective();
	CVoid DrawJustPerspectiveBorders();
	CVoid DrawBordersAndUI();
	CVoid Render3DModels(CBool sceneManager, CChar* parentTreeNameOfGeometries );
	CVoid Render3DAnimatedModels(CBool sceneManager );
	CVoid Render3DModelsControlledByPhysX(CBool sceneManager = CTrue);
	CVoid EngineLightPass();
	CVoid COLLADALightPass();
	CVoid DefaultLightPass();
	CVoid FixedFunctionLightPass();
	CVoid UpdateAnimations();
	CVoid CMultipleWindows::DrawGUI();
	CVoid BlendLights(CUInt lightIndex);
	CVoid BlendFogWithScene();
	CVoid ResetPhysXCounts();
	CBool InitAll();
	CBool GenerateDefaultTextures();
	CUInt GetSelectedObject();
	CVoid SetElapsedTimeFromBeginning();
	CVoid EnableTimer( CBool enable );
	CVoid ApplyForce( /*NxVec3 forceDirection, */CInt moveDirection, CFloat elapsedTime );
	CFloat elapsedTime;
	__int64 m_timerCounter;
	CFloat m_totalElapsedTime;

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
	CVoid SetInstanceCamera( CInstanceCamera * inst, CFloat sWidth, CFloat sHeight );
	CDOF m_dof;
	CBool m_lMouseDown, m_rMouseDown, m_mMouseDown;
	CBool m_selectObject;
	CMouseTranslationController* m_translationController;
	CPoint m_mousePosition; //Mouse position when the user clicks left mouse button
	CPoint m_translateMousePosition;
	CBool m_loadData; //load initial data?

	//shadow
	float cam_proj[16];
	float cam_modelview[16];
	float cam_inverse_modelview[16];
	float cam_pos[3];
	float cam_dir[3];
	float far_bound[MAX_SPLITS];

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

	CPhysXVariables m_physXVariables;
	CNovodex* m_nx;
	CTimer timer;
	CBool m_enableTimer;
	CSimpleFont m_simpleFont;
	CInputSystem* m_inputSystem;

	// Should we use multisampling?
	CBool m_multiSample;
	CInt m_numSamples;
	// To hold the pixel format of an OpenGL window in which doesn't use multisampling
	HGLRC	m_hRC;
	CDC*	m_pDC;
	HCURSOR m_currentCursor;
	
	CInt m_width, m_height;
	CInt m_grid;
	//After the user left clickes, we must check to see if there are any movements until he releases the mouse button
	//If there were no movement we send the selected name( If available )to the global selected name( defined  at stdafx.h )
	CBool m_tempMovement; 
	//It's used to hold the name until the user releases the button
	CUInt m_tempSelectedName;
	CUInt m_lastSelectedName;

	CBool m_lockInput;
	CBool m_loadScene;

	//this variable is used for bloom effect
	CBloom* m_bloom;
	CShadowMap* m_dynamicShadowMap;
	
	GLuint *ptr, minZ, selectedName, Buffer[MAX_NAME_SIZE];//selection
	CVoid InitObjectSelection();
	CVoid FinishObjectSelection();
public:
	CVoid ProcessInputs();
	CVoid InitDynamicShadowMap(CVec3f lightPos, CVec3f atPos );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CInt g_numLights;
extern CInt g_totalLights;
extern CBool g_fogBlurPass;