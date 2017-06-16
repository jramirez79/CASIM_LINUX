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
#include <IL/il.h>
#include <IL/ilu.h>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "cMacros.h"
#include "cStringUtils.h"
#include "cGlErrorManager.h"
#include "cLogManager.h"

using namespace std;

//=======================================================================================

#ifndef __TEXTURE
#define __TEXTURE

class Texture
{
public:
					Texture			( unsigned int&	id,
									  string&		name,
									  unsigned int	width,
									  unsigned int	height,
									  unsigned int	weight,
									  unsigned int	bpp,
									  unsigned int	texTarget,
									  bool			vert_flipped,
									  bool			horiz_flipped,
									  bool			mipmapped		);
					Texture			( string&		name,
									  unsigned int	width,
									  unsigned int	height,
									  unsigned int	weight,
									  unsigned int	bpp,
									  unsigned int	texTarget,
									  bool			vert_flipped,
									  bool			horiz_flipped,
									  bool			mipmapped		);
					~Texture		( void							);

	unsigned int	getId			( void							);
	string&			getName			( void							);
	unsigned int	getWidth		( void							);
	unsigned int	getHeight		( void							);
	unsigned int	getWeightB		( void							);
	unsigned int	getWeightKb		( void							);
	unsigned int	getWeightMb		( void							);
	unsigned int	getWeightGb		( void							);
	unsigned int	getBpp			( void							);
	bool			isHorizFlipped	( void							);
	bool			isVertFlipped	( void							);
	bool			hasMipmaps		( void							);
	bool			isTransparent   ( void							);
public:
	unsigned int	id;
private:
	string			name;
	unsigned int	width;
	unsigned int	height;
	unsigned int	weight;
	unsigned int	weight_kb;
	unsigned int	weight_mb;
	unsigned int	weight_gb;
	unsigned int	bpp;
	unsigned int	texTarget;
	bool			vert_flipped;
	bool			horiz_flipped;
	bool			mipmapped;
	bool			transparent;
};

#endif

//=======================================================================================

#ifndef __TEXTURE3D
#define __TEXTURE3D

class Texture3D
{
public:
					Texture3D		( unsigned int&		_id,
									  string&			_name,
									  vector<string>&	_file_names,
									  unsigned int		_width,
									  unsigned int		_height,
									  unsigned int		_depth,
									  unsigned int		_weight,
									  unsigned int		_bpp,
									  unsigned int		_texTarget,
									  bool				_vert_flipped,
									  bool				_horiz_flipped,
									  bool				_mipmapped		);
					Texture3D		( string&			_name,
									  vector<string>&	_file_names,
									  unsigned int		_width,
									  unsigned int		_height,
									  unsigned int		_depth,
									  unsigned int		_weight,
									  unsigned int		_bpp,
									  unsigned int		_texTarget,
									  bool				_vert_flipped,
									  bool				_horiz_flipped,
									  bool				_mipmapped		);
					~Texture3D		( void							);

	unsigned int	getId			( void							);
	string&			getName			( void							);
	unsigned int	getWidth		( void							);
	unsigned int	getHeight		( void							);
	unsigned int	getDepth		( void							);
	unsigned int	getWeightB		( void							);
	unsigned int	getWeightKb		( void							);
	unsigned int	getWeightMb		( void							);
	unsigned int	getWeightGb		( void							);
	unsigned int	getBpp			( void							);
	vector<string>&	getFileNames	( void							);
	bool			isHorizFlipped	( void							);
	bool			isVertFlipped	( void							);
	bool			hasMipmaps		( void							);
	bool			isTransparent   ( void							);
public:
	unsigned int	id;
private:
	string			name;
	vector<string>	file_names;
	unsigned int	width;
	unsigned int	height;
	unsigned int	depth;
	unsigned int	weight;
	unsigned int	weight_kb;
	unsigned int	weight_mb;
	unsigned int	weight_gb;
	unsigned int	bpp;
	unsigned int	texTarget;
	bool			vert_flipped;
	bool			horiz_flipped;
	bool			mipmapped;
	bool			transparent;
};

#endif

//=======================================================================================

#ifndef	__TEXTURE_MANAGER
#define	__TEXTURE_MANAGER

typedef	list<Texture*>			textureList;
typedef	list<Texture3D*>		texture3DList;
typedef	textureList::iterator	tListItor;
typedef	texture3DList::iterator	t3DListItor;

class TextureManager
{
protected:
									TextureManager		( void								)
									{
										curr_tex_weight		= 0;
										curr_tex_depth		= 0;
										curr_tex_width		= 0;
										curr_tex_height		= 0;
										err_manager			= NULL;
										log_manager			= NULL;
									}
	virtual							~TextureManager		( void								)
									{
										cleanAllTextures();
									}
public:
	static TextureManager*			getInstance			( void								);
	static void						freeInstance		( void								);

	void							init				( GlErrorManager*	err_manager_,
														  LogManager*		log_manager_	);
	void							bindTexture			( unsigned int		texTarget,
														  unsigned int		id				);
	unsigned int					loadTexture			( string&			filename,
														  bool				flip,
														  unsigned int		texTarget,
														  unsigned int		envMode			);
	unsigned int					loadTexture			( string&			filename,
														  bool				flip,
														  bool				mipmaps,
														  unsigned int		texTarget,
														  unsigned int		envMode,
														  unsigned int		filterMode  	);
	unsigned int					loadTexture3D		( vector<string>&	filenames,
														  vector<GLint>&	parameters,
														  bool				flip,
														  unsigned int		texTarget		);
	unsigned int					loadRawTexture3D	( vector<string>&	filenames,
														  vector<GLint>&	parameters,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		);
	unsigned int					loadRawTexture		( string&			filename,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		);

	unsigned int					loadRawTexture		( string&			filename,
														  vector<float>&	data,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget,
														  bool				do_log = true	);

	unsigned int					loadRawTexture		( string&			filename,
														  float*			data,
														  GLint				parameter,
														  GLuint			height,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget,
														  bool				do_log = true	);

	unsigned int					reloadRawTexture	( string&			filename,
														  vector<float>&	data,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		);

	void							updateTexture		( unsigned int		tex_id,
														  float*			data,
														  GLint				parameter,
														  GLuint			height,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		);

	unsigned int					getTexture			( string&			filename		);
	unsigned int					getTexture3D		( string&			filename		);
	void							deleteTexture		( unsigned int		id				);
	void							deleteTexture3D		( unsigned int		id				);
	void							cleanAllTextures	( void								);
	unsigned int					getCurrWidth		( void								);
	unsigned int					getCurrHeight		( void								);
	unsigned int					getCurrDepth		( void								);
	unsigned int					getCurrWeight		( void								);
	textureList						getTextures			( void								);
	texture3DList					getTextures3D		( void								);
private:
	textureList						texture_list;
	texture3DList					texture3D_list;
	static TextureManager*			singleton;
	unsigned int					curr_tex_width;
	unsigned int					curr_tex_height;
	unsigned int					curr_tex_depth;
	unsigned int					curr_tex_weight;
	GlErrorManager*					err_manager;
	LogManager*						log_manager;
};

//=======================================================================================

inline unsigned int					loadTexture			( string&			filename,
														  bool				flip,
														  unsigned int		texTarget,
														  unsigned int		envMode			)
{
	return TextureManager::getInstance()->loadTexture	( filename,
														  flip,
														  texTarget,
														  envMode							);
}

inline unsigned int					getTexture			( string&			filename 		)
{
	return TextureManager::getInstance()->getTexture	( filename							);
}

inline textureList					getTextures			( void						 		)
{
	return TextureManager::getInstance()->getTextures();
}

inline texture3DList				getTextures3D		( void						 		)
{
	return TextureManager::getInstance()->getTextures3D();
}

inline unsigned int					loadTexture3D		( vector<string>&	filenames,
														  vector<GLint>&	parameters,
														  bool				flip,
														  unsigned int		texTarget,
														  unsigned int		envMode			)
{
	return TextureManager::getInstance()->loadTexture3D	( filenames,
														  parameters,
														  flip,
														  texTarget							);
}

inline unsigned int					loadRawTexture3D	( vector<string>&	filenames,
														  vector<GLint>&	parameters,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		)
{
	return TextureManager::getInstance()->loadRawTexture3D	(	filenames,
																parameters,
																width,
																internalFormat,
																format,
																texTarget					);
}

inline unsigned int					loadRawTexture		( string&			filename,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		)
{
	return TextureManager::getInstance()->loadRawTexture	(	filename,
																parameter,
																width,
																internalFormat,
																format,
																texTarget					);
}

inline unsigned int					loadRawTexture		( string&			filename,
														  vector<float>&	data,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget,
														  bool				do_log = true	)
{
	return TextureManager::getInstance()->loadRawTexture	(	filename,
																data,
																parameter,
																width,
																internalFormat,
																format,
																texTarget,
																do_log						);
}

inline unsigned int					reloadRawTexture	( string&			filename,
														  vector<float>&	data,
														  GLint				parameter,
														  GLuint			width,
														  GLint				internalFormat,
														  GLenum			format,
														  unsigned int		texTarget		)
{
	return TextureManager::getInstance()->reloadRawTexture	(	filename,
																data,
																parameter,
																width,
																internalFormat,
																format,
																texTarget					);
}

inline unsigned int					getTexture3D		( string&			filename 		)
{
	return TextureManager::getInstance()->getTexture3D	( filename							);
}

#endif

//=======================================================================================
