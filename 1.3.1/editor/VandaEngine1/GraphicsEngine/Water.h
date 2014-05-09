//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once

#include "image.h"
#include "perspectiveWindow.h"
//std headers
#include <string>
#include <set>
#include <map>
#include <vector>

enum
{
	WATER_REFLECTION_ID,
	WATER_REFRACTION_ID,
	WATER_DEPTH_ID,
	WATER_NORMAL_ID,
	WATER_DUDVMAP_ID,
};

#define MAX_WATER_TEXTURES 3 //these textures include reflection, refraction and depth
#define g_waterTextureSize 512


class CWater
{

public:
	CWater();
	~CWater();
	CVoid RenderWater( CVec3f cameraPos, CFloat elapsedTime );

	CFloat m_fWaterCPos[3];
	CFloat m_fWaterLPos[3];
	CFloat m_fWaterHeight;
	CFloat m_fWaterSpeed;
	CFloat m_fWaterUV;
	CFloat m_fWaterScale;
	//####public interface####	
	//The user just needs to use these functions 
	CFloat* GetPos() { return m_fWaterCPos; }
	CFloat* GetLightPos() { return m_fWaterLPos; }
	CFloat GetHeight() { return m_fWaterHeight; }
	CFloat GetSpeed() { return m_fWaterSpeed; }
	CFloat GetUV(){ return m_fWaterUV; }
	CFloat GetScale() { return m_fWaterScale; }
	CChar* GetName();
	CChar* GetDuDvMapName();
	CChar* GetNormalMapName();
	CVoid SetDuDvMapName(CChar* name);
	CVoid SetNormalMapName(CChar* name);
	CInt GetIndex() { return m_nameIndex; }

	CVoid SetName( CString name  );
	CVoid SetDuDvMap( CString mapName, CBool updateSharedImages = CFalse );
	CVoid SetNormalMap( CString mapName, CBool updateSharedImages = CFalse );
	CVoid SetPos( CFloat* pos ) { m_fWaterCPos[0] = pos[0]; m_fWaterCPos[1] = pos[1]; m_fWaterCPos[2] = pos[2]; }
	CVoid SetLightPos( CFloat* pos ) { m_fWaterLPos[0] = pos[0]; m_fWaterLPos[1] = pos[1]; m_fWaterLPos[2] = pos[2]; }
	CVoid SetHeight( CFloat height ) { m_fWaterHeight = height; }
	CVoid SetSpeed( CFloat speed ) { m_fWaterSpeed = speed; }
	CVoid SetUV( CFloat UV ) { m_fWaterUV = UV; }
	CVoid SetScale( CFloat scale ) { m_fWaterScale = scale; }

	CVoid SetIndex() { m_nameIndex = g_nameIndex++; }
	// This renders the current screen to a texture map, given a specified size and type

	CVoid CreateRenderTexture(CInt size, CInt channels, CInt type, CInt textureID );

	// This renders our screen to a texture that is used for reflection
	CVoid CreateReflectionTexture(CInt textureSize );

	// This renders our screen to a texture for refraction and depth
	CVoid CreateRefractionDepthTexture(CInt textureSize);

	void SetSideVertexPositions();

	CVoid RenderIcon( CBool selectionMode = CFalse );

	std::vector<std::map<std::string,CBool>> m_VSceneList; //List of all VScenes created via the editor, save functions

	inline CVoid AddVSceneToList(std::string vSceneName, CBool save) //save functions
	{
		std::map<std::string, CBool> tempVScene;
		tempVScene[vSceneName] = save;
		m_VSceneList.push_back( tempVScene );
	}
	inline CBool IsInVSceneList( std::string vSceneName, CBool content, CBool reverseTheContent ) //save functions
	{
		for( CUInt i = 0; i < m_VSceneList.size(); i++ )
		{
			if( m_VSceneList[i].begin()->first == vSceneName )
			{
				if( m_VSceneList[i].begin()->second == content) //found the target
				{
					if( reverseTheContent )
						m_VSceneList[i].begin()->second = !content;
					return CTrue;
				}
				else
					return CFalse;
			}
		}
		if( reverseTheContent )
			AddVSceneToList( vSceneName, !content );
		else
			AddVSceneToList( vSceneName, content );
		return CTrue;
	}

	CChar m_strWaterName[MAX_NAME_SIZE];
	CChar m_strDuDvMap[MAX_NAME_SIZE];
	CChar m_strNormalMap[MAX_NAME_SIZE];
	CUInt m_waterTexture[ MAX_WATER_TEXTURES ];
	std::vector<CGeometry*> m_geometry;
	CVec3f m_sidePoint[4]; //each water has four vertexes

	CVoid SetGeometry( CGeometry* geo ) { m_geometry.push_back( geo );  }
	CImage *GetWaterImage( const CChar * name );

private:
	CImage *m_dudvMapImg;
	CImage *m_normalMapImg;
	CUInt m_fboID[ MAX_WATER_TEXTURES ]; //each texture has its own FBO ID
	CUInt m_rbID[ MAX_WATER_TEXTURES ]; //each texture has its own FBO ID

	CInt m_nameIndex; //selection

private:
	// This loads all of our animation textures and stores them in our texture array
	CVoid LoadAnimTextures(UINT textureArray[], LPSTR szFileName, CInt startIndex, CInt textureCount);

	// This cycles through our caustic bitmaps to produce the animation
	CVoid RenderCaustics(CFloat causticScale);


};