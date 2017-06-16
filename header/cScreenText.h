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

#pragma once
#include "cMacros.h"
#include "cProjectionManager.h"
#include <stdio.h>
#include <stdarg.h>

//=======================================================================================

#ifndef __SCREEN_TEXT
#define __SCREEN_TEXT

class ScreenText
{
public:
						ScreenText	(	void				);
						~ScreenText	(	void				);

	void				setfont		(	char*	name,
										int		size		);
	void				drawstr3D	(	GLuint	x,
										GLuint	y,
										GLuint	z,
										char*	format, ... );
	void				drawstr2D	(	GLuint	x,
										GLuint	y,
										char*	format, ... );
	void				begin		(	int		w,
										int		h			);
	void				end			(	void				);

private:
	ProjectionManager*	projMan;
	GLvoid*				font_style;
	bool				reset_l;
	bool				reset_t;
};

#endif

//=======================================================================================
