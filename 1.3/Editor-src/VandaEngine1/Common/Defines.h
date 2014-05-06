//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once

#define CChar   char 
#define CByte	char
#define CInt32	int
#define CInt16	short
#define CLong	short
#define CInt    CInt32 

#define CUChar	unsigned CByte
#define CUByte	unsigned CByte
#define CUInt32	unsigned CInt
#define CUInt16	unsigned CInt16
#define CULong	unsigned CLong
#define CUInt	CUInt32

#define CFloat     float
#define CDouble    double 

#define CVoid	void 
#define CError	CInt

#define CBool bool
#define	CFalse false
#define	CTrue true

typedef CFloat CMatrix[16];
typedef CFloat CMatrix3x4[12];

CFloat CPI = 3.14159f; //PI number

#ifndef M00
#define M00	0
enum
{
    M01=1, M02, M03,
	M10, M11, M12, M13,
	M20, M21, M22, M23,
	M30, M31, M32, M33
};
#endif 

enum CPhysXAlgorithm
{
	eLOD_CONVEX_HULL = 0,
	eLOD_LENGTH,
	eLOD_LENGTH_CURVATURE,
	eLOD_BOX,
	eLOD_SPHERE,
	eLOD_CAPSULE_METHOD1,
	eLOD_CAPSULE_METHOD2,
	eLOD_CAPSULE_METHOD3,
	eLOD_TRIGGER,
	eLOD_NONE,
};

enum CPolygonMode
{
	ePOLYGON_POINT,
	ePOLYGON_LINE,
	ePOLYGON_FILL,
};

enum CMaterialChannels
{
	eCHANNELS_ALL = 0,
	eCHANNELS_DIFFUSE,
	eCHANNELS_NORMALMAP,
	eCHANNELS_GLOSSMAP,
	eCHANNELS_SHADOWMAP,
	eCHANNELS_DIRTMAP,
	eCHANNELS_ALPHAMAP,
};

enum CGBufferTextureType {
	eGBUFFER_TEXTURE_TYPE_DEFAULT = 0,
    eGBUFFER_TEXTURE_TYPE_POSITION,
    eGBUFFER_TEXTURE_TYPE_NORMAL,
    eGBUFFER_TEXTURE_TYPE_DIFFUSE,
	eGBUFFER_TEXTURE_TYPE_NORMALMAP,
	eGBUFFER_TEXTURE_TYPE_GLOSSMAP,
	eGBUFFER_TEXTURE_TYPE_SHADOWMAP,
	eGBUFFER_TEXTURE_TYPE_DIRTMAP,
	//eGBUFFER_TEXTURE_TYPE_ALPHAMAP,

    eGBUFFER_NUM_TEXTURES = 1
};

enum CObjectListImage
{
	eOBJECTLIST_CAMERA = 0,
	eOBJECTLIST_IMAGE,
	eOBJECTLIST_MATERIAL,
	eOBJECTLIST_MESH,
	eOBJECTLIST_ANIMATION,
	eOBJECTLIST_LIGHT,
	eOBJECTLIST_EFFECT,
	eOBJECTLIST_ANIMATIONCLIP,
	eOBJECTLIST_SKINCONTROLLER,
};

enum CEngineObjectListImage
{
	eENGINEOBJECTLIST_WATER = 0,
	eENGINEOBJECTLIST_STATICSOUND,
	eENGINEOBJECTLIST_AMBIENTSOUND,
	eENGINEOBJECTLIST_SKY,
	eENGINEOBJECTLIST_LIGHT,
};
enum CUpAxis
{
	eCXUp,
	eCYUp,
	eCZUp,
};

enum CPhysXElementListImage
{
	ePHYSXELEMENTLIST_DYNAMIC_RIGIDBODY = 0,
	ePHYSXELEMENTLIST_STATIC_RIGIDBODY,
	ePHYSXELEMENTLIST_TRIGGER,
};

enum COpenGLShape
{
	eOPENGLSHAPE_POLYGON = 0,
	eOPENGLSHAPE_QUAD,
};

enum CTextureType 
{
    eTEXTURETYPE_DDS,
    eTEXTURETYPE_TGA,
    eTEXTURETYPE_UNKNOWN,
};

enum CCullFaceTypeForMenu
{
	eCULLFACETYPEFORMENU_CHECKED,
	eCULLFACETYPEFORMENU_UNCHECKED,
	eCULLFACETYPEFORMENU_GRAYED,

};

enum CLightType 
{
	eLIGHTTYPE_UNKNOWN,
	eLIGHTTYPE_AMBIENT = 1,
	eLIGHTTYPE_DIRECTIONAL,
	eLIGHTTYPE_POINT,
	eLIGHTTYPE_SPOT	
};
enum CAnimationStatus
{
	eANIM_NONE = 0,
	eANIM_PAUSE,
	eANIM_PLAY,
	eANIM_STOP,
	eANIM_BLEND_CYCLE,
	eANIM_CLEAR_CYCLE,
	eANIM_EXECUTE_ACTION,
	eANIM_REMOVE_ACTION,
};

enum CAnimTarget
{
	eAnimTargetX,
	eAnimTargetY,
	eAnimTargetZ,
	eAnimTargetXYZ, 
	eAnimTargetAngle,
	eMatrix,

	eTargetUnknown, 

	eRotXAxis,
	eRotYAxis,
	eRotZAxis,
	eRotate, 

	eScaleXAxis,
	eScaleYAxis,
	eScaleZAxis,
	eScale,

	eTransXAxis,
	eTransYAxis,
	eTransZAxis, 
	eTranslate, 


/*	eJointOrientX,
	eJointOrientY,
	eJointOrientZ,

	eRotateAxisX,
	eRotateAxisY,
	eRotateAxisZ,
*/
};

enum CTFormType
{
	eCRotate,
	eCTranslate,
	eCLookAt,
	eCMatrix, 
	eCScale,
	eCSkew,
	eCTFormUnknown, 
};

enum CLookAt
{
	eCLook,
	eCEye,
	eCUp,
};

enum CGeoDataType 
{
	eGeoPoints,
	eGeoNormals,
	eGeoBiNormals,
	eGeoTangents,
	eGeoSkinBindPoints,
	eGeoSkinBindNormals,
	eGeoSkinIndices,
	eGeoSkinWeights,
    eGeoIndices,
	eGeoTexCoord0,
	eGeoTexCoord1,
	eGeoTexCoord2,
	eGeoTexCoord3,
	eGeoTexCoord4,
	eGeoTexCoord5,
	eGeoTexCoord6,
	eGeoTexCoord7,
	eGeoTexCoord8,
	eGeoTexCoord9,
	eGeoTexCoord10,
	eGeoTexCoord11,
	eGeoTexCoord12,
	eGeoTexCoord13,
	eGeoTexCoord14,
	eGeoTexCoord15,
	eGeoTexCoord16,
	eGeoTexCoord17,
	eGeoTexCoord18,
	eGeoTexCoord19,
	eGeoDataTypeMax
};

enum CInputType 
{
	eUNKNOWN, 
	ePOINT_SOURCE = 1,
	eNORMAL_SOURCE,
	eTANGENT_SOURCE, 
	eBINORMAL_SOURCE, //Added this variable, So I load Binormal and don't need to compute it inside the shader
	eTEXCOORD0_SOURCE,
	eTEXCOORD1_SOURCE,
	eTEXCOORD2_SOURCE,
	eTEXCOORD3_SOURCE,
	eTEXCOORD4_SOURCE,
	eTEXCOORD5_SOURCE,
	eTEXCOORD6_SOURCE,
	eTEXCOORD7_SOURCE,
	eTEXCOORD8_SOURCE,
	eTEXCOORD9_SOURCE,
	eTEXCOORD10_SOURCE,
	eTEXCOORD11_SOURCE,
	eTEXCOORD12_SOURCE,
	eTEXCOORD13_SOURCE,
	eTEXCOORD14_SOURCE,
	eTEXCOORD15_SOURCE,
	eTEXCOORD16_SOURCE,
	eTEXCOORD17_SOURCE,
	eTEXCOORD18_SOURCE,
	eTEXCOORD19_SOURCE,
};

enum CShadowType 
{
	eSHADOW_SINGLE_HL = 0,
	eSHADOW_SINGLE,
	eSHADOW_MULTI_LEAK,
	eSHADOW_MULTI_NOLEAK,
	eSHADOW_PCF,
	eSHADOW_PCF_TRILIN,
	eSHADOW_PCF_4TAP,
	eSHADOW_PCF_8TAP,
	eSHADOW_PCF_GAUSSIAN,
};

enum CShadowSplits 
{
	eSHADOW_1_SPLIT = 0,
	eSHADOW_2_SPLITS,
	eSHADOW_3_SPLITS,
	eSHADOW_4_SPLITS,
};

enum CShadowResolution
{
	eSHADOW_1024 = 0,
	eSHADOW_2048,
	eSHADOW_4096,
};

// axes/plane to transform scene contents in
enum TRANSFORMAXES
{
	XTRANS,
	YTRANS,
	ZTRANS,
	XYTRANS,
	ZXTRANS,
	YZTRANS,
	FREETRANS,
};
enum CTextureMapType
{
	eDIFFUSE,
	eSHADOW_MAP,
	eGLOSS_MAP,
	eALPHA_MAP,
	eDIRT_MAP,
	eNORMAL_MAP,
};

const int MAX_STRING_SIZE = 256; 

const CUInt MAX_KEY_SETS = 10; //used for animation
const CInt ANIM_SAMPLE_RATE = 30; //used for animation

#define OPENAL_BUFFER_SIZE   32768     // 32 KB buffer

#define TGA_TYPE_MAPPED 1
#define TGA_TYPE_COLOR 2
#define TGA_TYPE_GRAY 3
#define TGA_TYPE_MAPPED_RLE 9
#define TGA_TYPE_COLOR_RLE 10
#define TGA_TYPE_GRAY_RLE 11

#define TGA_DESC_ABITS 0x0f
#define TGA_DESC_HORIZONTAL 0x10
#define TGA_DESC_VERTICAL 0x20

#define TGA_TRUECOLOR_32      (4)
#define TGA_TRUECOLOR_24      (3)

const CInt32 MAX_NAME_SIZE = 1024; 
const CInt32 MAX_URI_SIZE = 2048; 
const CInt32 MAX_TEX_COORDS = 20;
#define WRITEBUFFERSIZE 16384 //It's used to unzip password-protected zipped files.
// Memory Defines
#define CNew( c )	new c
#define CDelete( p ) if ( p )	{delete p ; p = 0;}
#define CNewData( c, n ) new c[n]
#define CDeleteData( p ) if ( p )	{delete [] p ; p = 0;}

#define COLOR_BLACK			RGB( 000, 000, 000 )
#define COLOR_WHITE			RGB( 200, 200, 200 )
#define COLOR_RED_GREEN		RGB( 180, 140, 000 )
#define COLOR_RED		    RGB( 200, 030, 030 )
#define COLOR_YELLOW	    RGB( 230, 230, 030 )
#define COLOR_GREEN			RGB( 040, 150, 030 )
#define COLOR_BLUE			RGB( 150, 150, 250 )


#define DEGREES_TO_RADIANS 0.017453292f

#define PI (3.1415926535897932384626433832795f)
#define DEG_TO_RAD(a)	(PI/180*(a))
#define RAD_TO_DEG(a)	(180/PI*(a))

#define DEFAULT_CHARACTER_SPEED 5
#define DEFAULT_CAMERA_ANGLE 55
#define MAX_CAMERA_ANGLE 70
#define MIN_CAMERA_ANGLE 20

#define MAX_GEO_SIZE 100

#define DEFAULT_DOF_DISTANCE 9
#define DEFAULT_DOF_RANGE 6

#define MAX_ANIMATION_CLIPS 20

