// @REV_TAG SRL_09_2013
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


#include "cAxes.h"

//=======================================================================================

Axes::Axes( float scale )
{
	axesList = glGenLists( 1 );
	glNewList( axesList, GL_COMPILE );
	{
		int x = 0, y = 0, z = 0;
		// X Axis:
		glColor3f( 1.0f, 0.0f, 0.0f );
		glBegin( GL_LINES );
		{
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( scale, 0.0f, 0.0f );
		}
		glEnd();
		// Markers every 10 pixels:
		while( x <= scale )
		{
			glPushMatrix();
			{
				glTranslatef( (float)x, 0.0f, 0.0f );
				glutSolidSphere( 0.5f, 10, 10 );
				x += 10;
			}
			glPopMatrix();
		}
		// Y Axis:
		glColor3f( 0.0f, 1.0f, 0.0f );
		glBegin( GL_LINES );
		{
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, scale, 0.0f );
		}
		glEnd();
		// Markers every 10 pixels:
		while( y <= scale )
		{
			glPushMatrix();
			{
				glTranslatef( 0.0f, (float)y, 0.0f );
				glutSolidSphere( 0.5f, 10, 10 );
				y += 10;
			}
			glPopMatrix();
		}
		// Z Axis:
		glColor3f( 0.0f, 0.0f, 1.0f );
		glBegin( GL_LINES );
		{
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, 0.0f, scale );
		}
		glEnd();
		// Markers every 10 pixels:
		while( z <= scale )
		{
			glPushMatrix();
			{
				glTranslatef( 0.0f, 0.0f, (float)z );
				glutSolidSphere( 0.5f, 10, 10 );
				z += 10;
			}
			glPopMatrix();
		}
		// Negative axes:
		glColor3f( 0.5f, 0.5f, 0.5f );
		glBegin( GL_LINES );
		{
			glVertex3f(  0.0f,  0.0f, 0.0f );
			glVertex3f( -scale, 0.0f, 0.0f );
		}
		glEnd();
		glBegin( GL_LINES );
		{
			glVertex3f( 0.0f,  0.0f,  0.0f );
			glVertex3f( 0.0f, -scale, 0.0f );
		}
		glEnd();
		glBegin( GL_LINES );
		{
			glVertex3f( 0.0f, 0.0f,  0.0f  );
			glVertex3f( 0.0f, 0.0f, -scale );
		}
		glEnd();
	}
	glEndList();
}

//=======================================================================================

Axes::~Axes( void )
{
	glDeleteLists( axesList, 1 );
}

//=======================================================================================

void Axes::draw( void )
{
	glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
	{
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glCallList( axesList );
	}
	glPopAttrib();
}

//=======================================================================================
