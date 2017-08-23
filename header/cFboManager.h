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
#include "cMacros.h"
#include "cGlErrorManager.h"
#include "cProjectionManager.h"
#include "cLogManager.h"
#include "cGlslManager.h"
#include "cScreenText.h"
#include "cCamera.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

//=======================================================================================

#ifndef UCDAVIS_FRAMEBUFFER_OBJECT_H
#define UCDAVIS_FRAMEBUFFER_OBJECT_H

class FramebufferObject
{
public:
  FramebufferObject();
  virtual ~FramebufferObject();
  void Bind();
  virtual void AttachTexture( GLenum texTarget,
                              GLuint texId,
                              GLenum attachment = GL_COLOR_ATTACHMENT0_EXT,
                              int mipLevel      = 0,
                              int zSlice        = 0 );
  virtual void AttachTextures( int numTextures,
                               GLenum texTarget[],
                               GLuint texId[],
                               GLenum attachment[] = NULL,
                               int mipLevel[]      = NULL,
                               int zSlice[]        = NULL );
  virtual void AttachRenderBuffer( GLuint buffId,
                                   GLenum attachment = GL_COLOR_ATTACHMENT0_EXT );
  virtual void AttachRenderBuffers( int numBuffers, GLuint buffId[],
                                    GLenum attachment[] = NULL );
  void Unattach( GLenum attachment );
  void UnattachAll();
#ifndef NDEBUG
  bool IsValid( std::ostream& ostr = std::cerr );
#else
  bool IsValid( std::ostream& ostr = std::cerr ) {
    return true;
  }
#endif
    GLuint GetID( ) { return m_fboId; }
    GLenum GetAttachedType( GLenum attachment );
    GLuint GetAttachedId( GLenum attachment );
    GLint  GetAttachedMipLevel( GLenum attachment );
    GLint  GetAttachedCubeFace( GLenum attachment );
    GLint  GetAttachedZSlice( GLenum attachment );
    static int GetMaxColorAttachments();
    static void Disable();
protected:
  void  _GuardedBind();
  void  _GuardedUnbind();
  void  _FramebufferTextureND( GLenum attachment, GLenum texTarget,
                               GLuint texId, int mipLevel, int zSlice );
  static GLuint _GenerateFboId();
private:
  GLuint m_fboId;
  GLint  m_savedFboId;
};

#endif

//=======================================================================================

#ifndef UCDAVIS_RENDER_BUFFER_H
#define UCDAVIS_RENDER_BUFFER_H

class Renderbuffer
{
public:
  Renderbuffer();
  Renderbuffer(GLenum internalFormat, int width, int height);
  ~Renderbuffer();
  void   Bind();
  void   Unbind();
  void   Set(GLenum internalFormat, int width, int height);
  GLuint GetId() const;
  static GLint GetMaxSize();
private:
  GLuint m_bufId;
  static GLuint _CreateBufferId();
};

#endif

//=======================================================================================

#ifndef __INPUT_FBO_TEXTURE
#define __INPUT_FBO_TEXTURE

class InputFboTexture
{
public:
	InputFboTexture( string& name, int type )
	{
		fbo_texture_name	= string( name );
		fbo_texture_type	= type;
	};
	~InputFboTexture( void )
	{

	};
	string fbo_texture_name;
	int fbo_texture_type;
};

#endif

//=======================================================================================

#ifndef __INPUT_FBO
#define __INPUT_FBO

class InputFbo
{
public:
								InputFbo(	string&						name,
											GLenum						tex_target,
											vector<InputFboTexture*>&	textures,
											unsigned int				width,
											unsigned int				height,
											float						wfactor = 1.0f,
											float						hfactor = 1.0f
										)
								{
									fbo_name				= string( name );
									fbo_textures			= textures;
									fbo_width				= width;
									fbo_height				= height;
									fbo_tex_target			= tex_target;
									wFactor					= wfactor;
									hFactor					= hfactor;
									display_width_flag		= false;
									display_height_flag		= false;
									agents_npot_width_flag	= false;
									agents_npot_height_flag	= false;
								};
								~InputFbo( void )
								{
									fbo_textures.clear();
								};
	enum						FBO_INIT_TYPE{	TYPICAL,
												GPGPU,
												DEPTH_NO_COMPARE,
												DEPTH_R_COMPARE		};
	string						fbo_name;
	vector<InputFboTexture*>	fbo_textures;
	unsigned int				fbo_width;
	unsigned int				fbo_height;
	GLenum						fbo_tex_target;
	float						wFactor;
	float						hFactor;
	bool						display_width_flag;
	bool						display_height_flag;
	bool						agents_npot_width_flag;
	bool						agents_npot_height_flag;
};

#endif

//=======================================================================================

#ifndef __LOCAL_FBO
#define __LOCAL_FBO

typedef struct
{
	FramebufferObject*	fbo;
	unsigned int		fbo_tex_target;
	unsigned int		fbo_width;
	unsigned int		fbo_height;
	string				fbo_name;
	bool				dynamic_size;
}						LocalFbo;

#endif

//=======================================================================================

#ifndef __FBO_MANAGER
#define __FBO_MANAGER

class FboManager
{
public:
								FboManager					(	LogManager*			log_manager_,
																GlslManager*		glsl_manager_,
																vector<InputFbo*>&	inputs				);
								~FboManager					(	void									);

	void						setTarget					(	string			tname,
																bool			clear_color_bit,
																bool			clear_depth_bit			);
	void						setTargets					(	string			tname1,
																string			tname2,
																bool			clear_color_bit,
																bool			clear_depth_bit			);
	void 						renderQuad					( 	string			fbo_name,
																string			shader_name,
																unsigned int	width,
																unsigned int	height					);
	void						renderQuad					(	string			shader_name,
																unsigned int	quad_width,
																unsigned int	quad_height,
																unsigned int	texture_width,
																unsigned int	texture_height			);
	void						renderQuadMA				(	string			shader_name,
																	unsigned int	quad_width,
																	unsigned int	quad_height,
																	unsigned int	texture_width,
																	unsigned int	texture_height			);
	void						displayTexture				(	string			shader_name,
																string			tex_name,
																unsigned int	quad_width,
																unsigned int	quad_height,
																unsigned int	texture_width,
																unsigned int	texture_height,
																unsigned char*  pixels=0);
	void 						displayTexture				( 	string 			shader_name,
																string 			tex_name,
																unsigned int 	width,
																unsigned int 	height,
																unsigned char*  pixels=0);
	void						renderTiled					(	string			shader_name,
																string			tex_name,
																unsigned int	tile_number,
																unsigned int	quad_width,
																unsigned int	quad_height,
																unsigned int	texture_width,
																unsigned int	texture_height			);
	bool						init						(	unsigned int	display_width,
																unsigned int	display_height,
																unsigned int	agents_npot				);
	bool						init						(	void									);
	void						pushActiveBind				(	string			tex_name,
																unsigned int	opengl_texture_offset	);
	void						popActiveBind				(	void									);
	bool						updateFboDims				(	string			fbo_name,
																unsigned int	new_width,
																unsigned int	new_height				);
	bool						updateDisplayDims			(	unsigned int	display_width,
																unsigned int	display_height			);
	void						bind_fbo					(	string			fbo_name				);
	void						unbind_fbo					(	string			fbo_name				);
	void						pushMatrices				(	Camera*			light_cam				);
	void						popMatrices					(	void									);
	unsigned int				getWidth					(	string			fbo_name				);
	unsigned int				getHeight					(	string			fbo_name				);
	void						report						(	void									);
	unsigned char*				getswpBuffin				(	string fbo_name							);
	unsigned char*				getswpBuffout				(	string fbo_name							);
	unsigned int				getswpBuffinLen				(	void									);
	unsigned int				getswpBuffOutLen			(	void									);
#include "cPostProcFx.h"
public:
	map<string, LocalFbo>		fbos;
	map<string, GLuint>			texture_ids;
	map<string, LocalFbo>		tex_fbo_map;
	ProjectionManager*			proj_manager;
	map<string, GLenum>			targets_map;


private:
	vector<InputFbo*>			inputs;
	vector<LocalFbo>			local_fbos;
	ScreenText*					screen_text;
	GlslManager*				glsl_manager;
	LogManager*					log_manager;
	GlErrorManager*				err_manager;
	GLint						maxAttachments;
	bool						fboOK;
	vector<GLuint>				pushed_tex_offsets;
	vector<GLuint>				pushed_tex_targets;
	unsigned char*				swpPixelsIn;
	unsigned int				swpPixelsInLen;
	unsigned char*				swpPixelsOut;
	unsigned int				swpPixelsOutLen;

private:
	void						typicalTextureSettings		 (	char*			msg,
																unsigned int	tex_target,
																unsigned int&	texture,
																unsigned int	w,
																unsigned int	h						);
	void						gpgpuTextureSettings		 (  char*			msg,
																unsigned int	tex_target,
																unsigned int&	texture,
																unsigned int	w,
																unsigned int	h						);
	void						depthTextureNoCompareSettings(	char*			msg,
																unsigned int	tex_target,
																unsigned int&	depthTexture,
																unsigned int	w,
																unsigned int	h						);
	void						depthTextureRCompareSettings (	char*			msg,
																unsigned int	tex_target,
																unsigned int&	depthTexture,
																unsigned int	w,
																unsigned int	h						);
	void						drawQuad					 (	unsigned int	w,
																unsigned int	h,
																unsigned int	tw,
																unsigned int	th						);
	void						drawQuadArray				 (	unsigned int	w,
																unsigned int	h,
																unsigned int	tw,
																unsigned int	th						);
	void						registerColorTarget			 (	string			target_name,
																unsigned int	tex_target,
																unsigned int	target_color,
																unsigned int&	target_texture			);
	void						registerDepthTarget			 (	unsigned int	tex_target,
																unsigned int&	target_texture			);
	const char*					isFboValid					 (	unsigned int	fb						);
};

#endif

//=======================================================================================
