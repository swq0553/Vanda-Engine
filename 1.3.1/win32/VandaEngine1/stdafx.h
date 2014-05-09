//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define NOMINMAX
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <stdio.h>
#include <atlstr.h>
#include <string>
#include "common\\defines.h"
using namespace std;

extern CUInt g_nameIndex;  
extern CUInt g_nodeIndex;
extern CInt g_selectedName; //holds the name of the selected object
extern CInt g_numVerts;
// TODO: reference additional headers your program requires here
struct lua_State;
CInt PlaySound(lua_State *L);
CInt StopSound(lua_State *L);
CInt PauseSound(lua_State *L);
CInt BlendCycle(lua_State *L);
CInt ClearCycle(lua_State *L);
CInt ExecuteAction(lua_State *L);
CInt ReverseExecuteAction(lua_State *L);
CInt LoadScene( lua_State *L);
CInt ActivateCamera(lua_State *L);
void GenerateLoadingTexture( char* fileName );
void DeleteLoadingTexture();
CVoid ShowLoadingScene();
