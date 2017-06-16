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
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

//=======================================================================================

#ifndef __STRING_UTILS
#define __STRING_UTILS

class StringUtils
{
public:
							StringUtils			( void						);
							~StringUtils		( void						);

	static vector<string>	split				( const string&		s,
											  	  char				delim	);
	static void				toUpper				( char*				in		);
	static const char*		getNameFromPath		( char*				name	);
	static string			getStrNameFromPath 	( char*				name	);
private:
	static vector<string>&	split				( const string&		s,
											  	  char				delim,
											  	  vector<string>&	elems	);
};

#endif

//=======================================================================================
