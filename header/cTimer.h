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

#pragma once

#ifdef __unix
	#include <sys/time.h>
#endif
#include "cMacros.h"

//=======================================================================================

#ifndef		__TIMER
#define		__TIMER

class Timer
{
protected:
					Timer			( void 				);
					~Timer			( void 				);

public:
	static Timer* 	getInstance		( void 				);
	static void		freeInstance	( void 				);
	void 			init			( void 				);
	void 			update			( void 				);
	unsigned long 	getTimeMSec		( void 				);
	unsigned long 	getTime			( void 				);
	float 			getAnimTime		( void 				);
	float 			getFps			( void 				);
#ifdef _WIN32
	void			start			( DWORD&	time1 	);
	float			stop			( DWORD&	time1	);
#elif defined __unix
	void			start			( timeval& 	time1 	);
	float			stop			( timeval&	time1	);
#endif

private:
	unsigned long	currentTime;
	unsigned long	lastTime;
	static Timer*	singleton;
};

#endif

//=======================================================================================
