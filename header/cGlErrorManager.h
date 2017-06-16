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
#include "cLogManager.h"

#include <stdio.h>
#include <string>

using namespace std;

//=======================================================================================

#ifndef __GL_ERROR_MANAGER
#define __GL_ERROR_MANAGER

class GlErrorManager
{
public:
				GlErrorManager	( LogManager*   log_manager_    );
				~GlErrorManager	( void						    );

	const char*	getString		( unsigned int	err			    );
	GLenum		getError		( void						    );
	GLenum		getError		( const char*	note		    );

private:
    LogManager* log_manager;
};

#endif

//=======================================================================================
