//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#include "stdafx.h"
#include "SimpleFont.h"

bool CFont::Init( LPCWSTR fontName, int fontSize )
{
	hdc = CreateCompatibleDC(0);
	if( !hdc )
	{
		MessageBoxW( NULL, _T("CFont Class:Couldn't create compatible DC"), _T("VandaEngine Error"), MB_OK | MB_ICONERROR );
		return false;
	}
	base = glGenLists( 96 );
	if( !base )
	{
		MessageBoxW( NULL, _T("CFont Class:Couldn't create the list"), _T("VandaEngine Error"), MB_OK | MB_ICONERROR );
		return false;

	}

	hFont = CreateFont( fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
		fontName );
	if( !hFont )
	{
		MessageBoxW( NULL, _T("CFont Class:Couldn't create the requested font"), _T("VandaEngine Error"), MB_OK | MB_ICONERROR );
		return false;
	}
	SelectObject( hdc, hFont );
	wglUseFontBitmaps( hdc, 32, 96, base );
	return true;
}

void CFont::Release()
{
	if( hFont )
        DeleteObject( hFont );
	if( base != 0 )
	glDeleteLists( base, 96 );
}

void CFont::Print( char* string, GLfloat xPos, GLfloat yPos, GLfloat zPos, float r, float g, float b )
{
	glPushAttrib( GL_CURRENT_BIT );
	glPushAttrib( GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
	glColor3f( r, g, b );
    glRasterPos3f( xPos, yPos, zPos );
	glPushAttrib( GL_LIST_BASE );
	glListBase( base - 32 );
	glCallLists( (GLsizei)strlen( string ), GL_UNSIGNED_BYTE, string );
	glPopAttrib();//GL_LIST_BASE
	glPopAttrib();//GL_ENABLE_BIT
	glPopAttrib();//GL_CURRENT_BIT
}

void CFont::StartRendering()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0, 768.0, 0.0, 1024.0, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
}

void CFont::EndRendering()
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}