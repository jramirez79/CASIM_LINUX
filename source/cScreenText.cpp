// @REV_TAG SRL_10_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjamín Hernández A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include <string.h>
#include "cScreenText.h"

//=======================================================================================
//
ScreenText::ScreenText( void )
{
/*
#   define  GLUT_STROKE_ROMAN               ((void *)0x0000)
#   define  GLUT_STROKE_MONO_ROMAN          ((void *)0x0001)
#   define  GLUT_BITMAP_9_BY_15             ((void *)0x0002)
#   define  GLUT_BITMAP_8_BY_13             ((void *)0x0003)
#   define  GLUT_BITMAP_TIMES_ROMAN_10      ((void *)0x0004)
#   define  GLUT_BITMAP_TIMES_ROMAN_24      ((void *)0x0005)
#   define  GLUT_BITMAP_HELVETICA_10        ((void *)0x0006)
#   define  GLUT_BITMAP_HELVETICA_12        ((void *)0x0007)
#   define  GLUT_BITMAP_HELVETICA_18        ((void *)0x0008)
*/
	projMan		= new ProjectionManager();
	font_style	= GLUT_BITMAP_9_BY_15;
	reset_l		= false;
	reset_t		= false;
}
//
//=======================================================================================
//
ScreenText::~ScreenText( void )
{
	FREE_INSTANCE( projMan );
}
//
//=======================================================================================
//
void ScreenText::setfont( char* name, int size )
{
    font_style = GLUT_BITMAP_HELVETICA_10;
    if( strcmp( name, "helvetica" ) == 0 )
	{
        if( size == 12 )
		{
            font_style = GLUT_BITMAP_HELVETICA_12;
		}
        else if( size == 18 )
		{
            font_style = GLUT_BITMAP_HELVETICA_18;
		}
    }
	else if( strcmp(name, "times roman" ) == 0 )
	{
        font_style = GLUT_BITMAP_TIMES_ROMAN_10;
        if( size == 24 )
		{
            font_style = GLUT_BITMAP_TIMES_ROMAN_24;
		}
    }
	else if( strcmp( name, "8x13" ) == 0 )
	{
        font_style = GLUT_BITMAP_8_BY_13;
    }
	else if( strcmp( name, "9x15" ) == 0 )
	{
        font_style = GLUT_BITMAP_9_BY_15;
    }
}
//
//=======================================================================================
//
void ScreenText::drawstr3D( GLuint x, GLuint y, GLuint z, char* format, ... )
{
    va_list args;
    char buffer[SCREEN_TEXT_BUFFER_SIZE], *s;

    va_start( args, format );
#if defined (_WIN32)
	vsprintf_s( buffer, SCREEN_TEXT_BUFFER_SIZE, format, args );
#elif defined (__unix)
	vsprintf( buffer, format, args );
#endif
    va_end( args );

    glRasterPos3i( x, y, z );
    for( s = buffer; *s; s++ )
	{
        glutBitmapCharacter( font_style, *s );
	}
}
//
//=======================================================================================
//
void ScreenText::begin( int w, int h )
{
	if( glIsEnabled( GL_LIGHTING ) )
	{
		reset_l = true;
		glDisable( GL_LIGHTING );
	}
	if( glIsEnabled( GL_TEXTURE_2D ) )
	{
		reset_t = true;
		glDisable( GL_TEXTURE_2D );
	}
	projMan->setTextProjection( w, h );
	glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
}
//
//=======================================================================================
//
void ScreenText::end( void )
{
	projMan->restoreProjection();
	if( reset_l )
	{
		glEnable( GL_LIGHTING );
	}
	reset_l = false;
	if( reset_t )
	{
		glEnable( GL_TEXTURE_2D );
	}
	reset_t = false;
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}
//
//=======================================================================================
//
void ScreenText::drawstr2D( GLuint x, GLuint y, char* format, ... )
{
	glPushMatrix();
	{
		glLoadIdentity();
		va_list args;
		char buffer[SCREEN_TEXT_BUFFER_SIZE], *s;

		va_start( args, format );

#if defined (_WIN32)
		vsprintf_s( buffer, SCREEN_TEXT_BUFFER_SIZE, format, args );
#elif defined (__unix)
		vsprintf( buffer,format, args );
#endif
		va_end( args );

		glRasterPos2i( x, y );
		for( s = buffer; *s; s++ )
		{
			glutBitmapCharacter( font_style, *s );
		}
	}
	glPopMatrix();
}
//
//=======================================================================================
