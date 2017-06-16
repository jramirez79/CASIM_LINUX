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
Texture::Texture
(
	unsigned int&		id,
	string&				name,
	unsigned int		width,
	unsigned int		height,
	unsigned int		weight,
	unsigned int		bpp,
	unsigned int		texTarget,
	bool				vert_flipped,
	bool				horiz_flipped,
	bool				mipmapped
)
{
	this->id			= id;
	this->name			= string( name );
	this->width			= width;
	this->height		= height;
	this->weight		= weight;
	weight_kb			= BYTE2KB( weight );
	weight_mb			= BYTE2MB( weight );
	weight_gb			= BYTE2GB( weight );
	this->bpp			= bpp;
	if( bpp == 4 )
	{
		transparent = true;
	}
	else
	{
		transparent = false;
	}
	this->texTarget		= texTarget;
	this->vert_flipped	= vert_flipped;
	this->horiz_flipped	= horiz_flipped;
	this->mipmapped		= mipmapped;
}
//
//=======================================================================================
//
Texture::Texture
(
	string&			name,
	unsigned int	width,
	unsigned int	height,
	unsigned int	weight,
	unsigned int	bpp,
	unsigned int	texTarget,
	bool			vert_flipped,
	bool			horiz_flipped,
	bool			mipmapped
)
{
	id					= 0;
	this->name			= string( name );
	this->width			= width;
	this->height		= height;
	this->weight		= weight;
	weight_kb			= BYTE2KB( weight );
	weight_mb			= BYTE2MB( weight );
	weight_gb			= BYTE2GB( weight );
	this->bpp			= bpp;
	this->texTarget		= texTarget;
	this->vert_flipped	= vert_flipped;
	this->horiz_flipped	= horiz_flipped;
	this->mipmapped		= mipmapped;
}
//
//=======================================================================================
//
Texture::~Texture( void )
{
	FREE_TEXTURE( id );
	name.erase();
}
//
//=======================================================================================
//
unsigned int Texture::getId( void )
{
	return id;
}
//
//=======================================================================================
//
string& Texture::getName( void )
{
	return name;
}
//
//=======================================================================================
//
unsigned int Texture::getWidth( void )
{
	return width;
}
//
//=======================================================================================
//
unsigned int Texture::getHeight( void )
{
	return height;
}
//
//=======================================================================================
//
unsigned int Texture::getWeightB( void )
{
	return weight;
}
//
//=======================================================================================
//
unsigned int Texture::getWeightKb( void )
{
	return weight_kb;
}
//
//=======================================================================================
//
unsigned int Texture::getWeightMb( void )
{
	return weight_mb;
}
//
//=======================================================================================
//
unsigned int Texture::getWeightGb( void )
{
	return weight_gb;
}
//
//=======================================================================================
//
unsigned int Texture::getBpp( void )
{
	return bpp;
}
//
//=======================================================================================
//
bool Texture::isHorizFlipped( void )
{
	return horiz_flipped;
}
//
//=======================================================================================
//
bool Texture::isVertFlipped( void )
{
	return vert_flipped;
}
//
//=======================================================================================
//
bool Texture::hasMipmaps( void )
{
	return mipmapped;
}
//
//=======================================================================================
//
bool Texture::isTransparent( void )
{
	return transparent;
}
//
//=======================================================================================
