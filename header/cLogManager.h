// @REV_TAG SRL_09_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjam�n Hern�ndez A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#pragma once
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <map>

#include "cMacros.h"

using namespace std;

//=======================================================================================

#ifndef __LOG_MANAGER
#define __LOG_MANAGER

class LogManager
{
public:
								LogManager			( string& log_filename			);
								~LogManager			( void							);
public:
	enum					    LOG_MESSAGE_TYPE
								{
									LERROR,
									GL_ERROR,
									WARNING,
									CONFIGURATION,
									XML,
									CONTEXT,
									GLSL_MANAGER,
									FBO_MANAGER,
									VBO_MANAGER,
									EXTENSION_MANAGER,
									TEXTURE_MANAGER,
									CROWD_MANAGER,
									OBSTACLE_MANAGER,
									BOUNDING_VOLUME,
									MODEL,
									GPU_RAM,
									FMOD,
									ALUT,
									OGG,
									MD2,
									SKYBOX,
									INFORMATION,
									STATUS,
									STATISTICS,
									CLEANUP,
									NET,
									EXIT,
									STATIC_LOD,
									CUDA,
									MDP,
									MPI
								};
public:
	void						log					( int			section,
													  string		data			);
	void						log					( int			section,
													  const char*	format, ...		);
	void						console_log			( int			section,
													  const char*	format, ...		);
	void						file_log			( int			section,
													  const char*	format, ...		);
	void						separator			( void							);
	void						console_separator	( void							);
	void						file_separator		( void							);
	void						file_prc			( int			section,
													  int			curr_val,
													  int			max_val			);
	void						console_prc			( int			curr_val,
													  int			max_val			);
	void						logStatistics		( unsigned int	tex_w,
													  unsigned int	vert_c,
													  unsigned int	frame_vert_c,
													  unsigned int	vert_size		);
	void						logPeakFps			( float			peakFps,
													  float			avgFps,
													  unsigned long frame_c,
													  float			spf,
													  unsigned int	culled,
													  unsigned int	total			);
	void						logLowFps			( float			lowFps,
													  float			avgFps,
													  unsigned long frame_c,
													  float			spf,
													  unsigned int	culled,
													  unsigned int	total			);
private:
	map<int, string>			sections_map;
	map<int, string>			html_colors_map;
	ofstream					logFile;
	char*						timeBuf;
	string						filename;
private:
	void						getTime				( void							);
	void						printTime			( const char*	format, ...		);
};

#endif

//=======================================================================================
