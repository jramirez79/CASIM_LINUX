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

#include "cAnimation.h"

//=======================================================================================
//
Animation::Animation( void )
{
	multitextureID	= 0;
	num_frames		= 0;
	step			= 0;
}
//
//=======================================================================================
//
Animation::~Animation( void )
{

}
//
//=======================================================================================
//
void Animation::loadMultitexture(	string&		dir,
									string&		ext,
									string&		rest_name,
									string&		frame_name,
								    GLuint		start_frame,
									GLuint		end_frame	)
{
	vector<string> filenames;
	vector<GLint> parameters;

	string rest = dir;
	rest.append( "/" );
	rest.append( rest_name );
	rest.append( "." );
	rest.append( ext );
	std::transform( rest.begin(), rest.end(), rest.begin(), ::tolower );

	num_frames	= 0;
	step		= 0;
	for( unsigned int f = start_frame; f <= end_frame; ++f )
	{
		string fName = dir;
		fName.append( "/" );
		fName.append( frame_name );
		string frame_str = static_cast<ostringstream*>( &(ostringstream() << f) )->str();
		fName.append( frame_str );
		fName.append( "." );
		fName.append( ext );
		std::transform( fName.begin(), fName.end(), fName.begin(), ::tolower );
		filenames.push_back( fName );
		parameters.push_back( GL_NEAREST );
		num_frames++;
	}
	multitextureID = TextureManager::getInstance()->loadRawTexture3D(	filenames,
																		parameters,
																		4,
																		GL_RGBA32F,
																		GL_RGBA,
																		GL_TEXTURE_2D_ARRAY	);
}
//
//=======================================================================================
//
GLuint Animation::getLength( void )
{
	return num_frames;
}
//
//=======================================================================================
//
void Animation::nextFrame( void )
{
	step++;
	if( step >= num_frames )
	{
		step = 0;
	}
}
//
//=======================================================================================
