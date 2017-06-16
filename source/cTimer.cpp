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

#include "cTimer.h"

//=======================================================================================
//
Timer* Timer::singleton = 0;
//
//=======================================================================================
//
Timer::Timer( void )
{
	init();
}
//
//=======================================================================================
//
Timer::~Timer( void )
{

}
//
//=======================================================================================
//
Timer* Timer::getInstance( void )
{
	if( singleton == 0 )
	{
		singleton = new Timer;
	}

	return ((Timer *)singleton);
}
//
//=======================================================================================
//
void Timer::freeInstance( void )
{
	if( singleton != 0 )
	{
		FREE_INSTANCE( singleton );
	}
}
//
//=======================================================================================
//
void Timer::init( void )
{
	currentTime = getTimeMSec();
	lastTime	= 0;
}
//
//=======================================================================================
//
void Timer::update( void )
{
	lastTime	= currentTime;
	currentTime = getTimeMSec();
}
//
//=======================================================================================
//
unsigned long Timer::getTimeMSec( void )
{
	return glutGet( GLUT_ELAPSED_TIME );
}
//
//=======================================================================================
//
unsigned long Timer::getTime( void )
{
	return currentTime;
}
//
//=======================================================================================
//
float Timer::getAnimTime( void )
{
	return ((float)glutGet( GLUT_ELAPSED_TIME ) * 0.001f);
}
//
//=======================================================================================
//
float Timer::getFps( void )
{
	return (1000.0f / (float)(currentTime - lastTime));
}
//
//=======================================================================================
//
#ifdef _WIN32
void Timer::start( DWORD& time1 )
{
	time1 = GetTickCount();
}
//
//=======================================================================================
//
float Timer::stop( DWORD& time1 )
{
	DWORD init_elapsedTicks = GetTickCount() - time1;
	return (float)init_elapsedTicks;
}
#elif defined __unix
//
//=======================================================================================
//
void Timer::start( timeval& time1 )
{
	gettimeofday( &time1, NULL );
}
//
//=======================================================================================
//
float Timer::stop( timeval&	time1 )
{
	timeval stop, result;
	gettimeofday( &stop, NULL );
	timersub( &stop, &time1, &result );
	return (float)result.tv_sec*1000.0f+(float)result.tv_usec/1000.0f;
}
#endif
//
//=======================================================================================
