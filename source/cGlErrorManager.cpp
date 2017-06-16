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

// Source: http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml

#include "cGlErrorManager.h"

//=======================================================================================
//
GlErrorManager::GlErrorManager( LogManager* log_manager_ )
{
    log_manager = log_manager_;
}
//
//=======================================================================================
//
GlErrorManager::~GlErrorManager( void )
{

}
//
//=======================================================================================
//
GLenum GlErrorManager::getError( void )
{
	GLenum err = glGetError();
	GLenum first = err;
	do
	{
		if( err != 0 )
		{
			const char *err_str = getString( err );
			if( log_manager )
                log_manager->log( LogManager::LERROR, "(%i)%s", err, err_str );
            else
                printf( "(%i)%s\n", err, err_str );
		}
		err = glGetError();
	}
	while( err != GL_NO_ERROR );
	return first;
}
//
//=======================================================================================
//
GLenum GlErrorManager::getError( const char* note )
{
	GLenum err = glGetError();
	GLenum first = err;
	do
	{
		if( err != 0 )
		{
			const char *err_str = getString( err );
			if( log_manager )
                log_manager->log( LogManager::LERROR, "%s::(%i)%s", note, err, err_str );
            else
                printf( "%s\n(%i)%s\n", note, err, err_str );
		}
		err = glGetError();
	}
	while( err != GL_NO_ERROR );
	return first;
}
//
//=======================================================================================
//
const char* GlErrorManager::getString( unsigned int err )
{
	string text;
	switch( err )
	{
		case GL_NO_ERROR:
			text = string(  "\t\t\tGL_NO_ERROR:\n\t\t\tNo error has been recorded.\n\t\t\tThe "
                            "value of this symbolic constant is guaranteed to be 0." );
			break;
		case GL_INVALID_ENUM:
			text = string(  "\t\t\tGL_INVALID_ENUM:\n\t\t\tAn unacceptable value is specified "
                            "for an enumerated argument.\n\t\t\tThe offending command is ignored"
                            "\n\t\t\tand has no other side effect than to set the error flag." );
			break;
		case GL_INVALID_VALUE:
			text = string(  "\t\t\tGL_INVALID_VALUE:\n\t\t\tA numeric argument is out of range."
                            "\n\t\t\tThe offending command is ignored\n\t\t\tand has no other "
                            "side effect than to set the error flag." );
			break;
		case GL_INVALID_OPERATION:
			text = string(  "\t\t\tGL_INVALID_OPERATION:\n\t\t\tThe specified operation is not "
                            "allowed in the current state.\n\t\t\tThe offending command is "
                            "ignored\n\t\t\tand has no other side effect than to set the "
                            "error flag." );
			break;
		case GL_STACK_OVERFLOW:
			text = string(  "\t\t\tGL_STACK_OVERFLOW:\n\t\t\tThis command would cause a stack "
                            "overflow.\n\t\t\tThe offending command is ignored\n\t\t\tand has "
                            "no other side effect than to set the error flag." );
			break;
		case GL_STACK_UNDERFLOW:
			text = string(  "\t\t\tGL_STACK_UNDERFLOW:\n\t\t\tThis command would cause a stack "
                            "underflow.\n\t\t\tThe offending command is ignored\n\t\t\tand has "
                            "no other side effect than to set the error flag." );
			break;
		case GL_OUT_OF_MEMORY:
			text = string(  "\t\t\tGL_OUT_OF_MEMORY:\n\t\t\tThere is not enough memory left to "
                            "execute the command.\n\t\t\tThe state of the GL is undefined,"
                            "\n\t\t\texcept for the state of the error flags,\n\t\t\tafter this "
                            "error is recorded." );
			break;
		case GL_TABLE_TOO_LARGE:
			text = string(  "\t\t\tGL_TABLE_TOO_LARGE:\n\t\t\tThe specified table exceeds the "
                            "implementation's maximum supported table\n\t\t\tsize.  The offending"
                            " command is ignored and has no other side effect\n\t\t\tthan to set "
                            "the error flag." );
			break;
		default:
			text = string(  "\t\t\tUNKNOWN_ERROR!" );
			break;
	}
	return text.c_str();
}
//
//=======================================================================================
