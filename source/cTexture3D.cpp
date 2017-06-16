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
//
#include "cTextureManager.h"
//
//=======================================================================================
//
Texture3D::Texture3D
(
	unsigned int&		_id,
	string&				_name,
	vector<string>&		_file_names,
	unsigned int		_width,
	unsigned int		_height,
	unsigned int		_depth,
	unsigned int		_weight,
	unsigned int		_bpp,
	unsigned int		_texTarget,
	bool				_vert_flipped,
	bool				_horiz_flipped,
	bool				_mipmapped
)
{
	id					= _id;
	name				= string( _name );
	for( unsigned int n = 0; n < _file_names.size(); n++ )
	{
		file_names.push_back( _file_names[n] );
	}
	width				= _width;
	height				= _height;
	depth				= _depth;
	weight				= _weight;
	weight_kb			= BYTE2KB( weight );
	weight_mb			= BYTE2MB( weight );
	weight_gb			= BYTE2GB( weight );
	bpp					= _bpp;
	if( bpp == 4 )
	{
		transparent = true;
	}
	else
	{
		transparent = false;
	}
	texTarget			= _texTarget;
	vert_flipped		= _vert_flipped;
	horiz_flipped		= _horiz_flipped;
	mipmapped			= _mipmapped;
}
//
//=======================================================================================
//
Texture3D::Texture3D
(
	string&				_name,
	vector<string>&		_file_names,
	unsigned int		_width,
	unsigned int		_height,
	unsigned int		_depth,
	unsigned int		_weight,
	unsigned int		_bpp,
	unsigned int		_texTarget,
	bool				_vert_flipped,
	bool				_horiz_flipped,
	bool				_mipmapped
)
{
	id					= 0;
	name				= string( _name );
	for( unsigned int n = 0; n < _file_names.size(); n++ )
	{
		file_names.push_back( _file_names[n] );
	}
	width				= _width;
	height				= _height;
	depth				= _depth;
	weight				= _weight;
	weight_kb			= BYTE2KB( weight );
	weight_mb			= BYTE2MB( weight );
	weight_gb			= BYTE2GB( weight );
	bpp					= _bpp;
	texTarget			= _texTarget;
	vert_flipped		= _vert_flipped;
	horiz_flipped		= _horiz_flipped;
	mipmapped			= _mipmapped;
}
//
//=======================================================================================
//
Texture3D::~Texture3D( void )
{
	FREE_TEXTURE( id );
	name.erase();
}
//
//=======================================================================================
//
unsigned int Texture3D::getId( void )
{
	return id;
}
//
//=======================================================================================
//
string& Texture3D::getName( void )
{
	return name;
}
//
//=======================================================================================
//
unsigned int Texture3D::getWidth( void )
{
	return width;
}
//
//=======================================================================================
//
unsigned int Texture3D::getHeight( void )
{
	return height;
}
//
//=======================================================================================
//
unsigned int Texture3D::getDepth( void )
{
	return depth;
}
//
//=======================================================================================
//
unsigned int Texture3D::getWeightB( void )
{
	return weight;
}
//
//=======================================================================================
//
unsigned int Texture3D::getWeightKb( void )
{
	return weight_kb;
}
//
//=======================================================================================
//
unsigned int Texture3D::getWeightMb( void )
{
	return weight_mb;
}
//
//=======================================================================================
//
unsigned int Texture3D::getWeightGb( void )
{
	return weight_gb;
}
//
//=======================================================================================
//
unsigned int Texture3D::getBpp( void )
{
	return bpp;
}
//
//=======================================================================================
//
bool Texture3D::isHorizFlipped( void )
{
	return horiz_flipped;
}
//
//=======================================================================================
//
bool Texture3D::isVertFlipped( void )
{
	return vert_flipped;
}
//
//=======================================================================================
//
bool Texture3D::hasMipmaps( void )
{
	return mipmapped;
}
//
//=======================================================================================
//
bool Texture3D::isTransparent( void )
{
	return transparent;
}
//
//=======================================================================================
//
vector<string>& Texture3D::getFileNames( void )
{
	return file_names;
}
//
//=======================================================================================
