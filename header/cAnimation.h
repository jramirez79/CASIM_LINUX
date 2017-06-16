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
#include <string>
#include <vector>

#include "cMacros.h"
#include "cTextureManager.h"

using namespace std;

//=======================================================================================

#ifndef __ANIMATION
#define __ANIMATION

class Animation
{
public:
				Animation			(	void							);
				~Animation			(	void							);

	void		loadMultitexture	(	string&			dir,
										string&			ext,
										string&			rest_name,
										string&			frame_name,
										GLuint			start_frame,
										GLuint			end_frame		);
	void		nextFrame			(	void							);
	GLuint		getLength			(	void							);
	GLuint		multitextureID;
	GLuint		step;

private:
	GLuint		num_frames;

};

#endif

//=======================================================================================
