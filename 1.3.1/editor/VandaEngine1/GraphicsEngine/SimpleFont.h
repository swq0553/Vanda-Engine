//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once

#include <windows.h>
#include "OpenGL.h"

class CSimpleFont
{
public:
	bool Init( CChar* fontName, int fontSize );
	void Release();
	void Print( char* string, GLfloat xPos, GLfloat yPos, GLfloat zPos, float r, float g, float b );
	void StartRendering();
	void EndRendering();

private:
	HDC hdc;
	HFONT hFont;
	GLuint base;
};


