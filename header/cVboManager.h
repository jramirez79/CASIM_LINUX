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
#include "cMacros.h"
#include "cVertex.h"
#include "cGlslManager.h"
#include "cLogManager.h"
#include "cGlErrorManager.h"
#include "cCamera.h"

#include <cuda_gl_interop.h>

#include <map>
#include <vector>
#include <string>

using namespace std;

//=======================================================================================

#ifndef __VBO_MANAGER
#define __VBO_MANAGER

//=======================================================================================

typedef struct
{
	vector<Vertex>					vertices;
	unsigned int					id;
	unsigned int					attached_tbo_id;
}									VBO;

#define INITVBO( vbo )				\
	vbo.id				= 0;		\
	vbo.attached_tbo_id = 0

//=======================================================================================

typedef struct
{
	vector<Vertex>					vertices;
	unsigned int					gl_id;
	unsigned int					attached_tbo_gl_id;
	struct cudaGraphicsResource*	cuda_vbo_res;
}									GL_CUDA_VBO;

#define INITGL_CUDA_VBO( vbo )		\
	vbo.gl_id				= 0;	\
	vbo.attached_tbo_gl_id	= 0;	\
	vbo.cuda_vbo_res		= 0

//=======================================================================================

typedef struct
{
	GLenum							offset;
	GLenum							target;
	GLuint							id;
}									VboRenderTexture;

//=======================================================================================

class VboManager
{
public:
									VboManager									(	LogManager*				log_manager_,
																					GlErrorManager*			err_manager_,
																					GlslManager*			glsl_manager_										);
									~VboManager									(	void																		);

	unsigned int					getReusedVBOs								(	void																		);
	bool							isFrameRegistered							(	string&					filename,
																					unsigned int			frame												);
	VBO								getVBO										(	string&					filename,
																					unsigned int			frame												);
	unsigned int					reuseFrame									(	GLuint&					vboId,
																					string&					filename,
																					unsigned int			frame												);
	unsigned int					createVBOContainer							(	string&					filename,
																					unsigned int			frame												);
	unsigned int					createGlCudaVboContainer					(	string&					filename,
																					unsigned int			frame												);
	unsigned int					gen_vbo										(	GLuint&					vboId,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame											);
	void							attach_tbo2vbo								(	GLuint&					tboId,
																					unsigned int			gl_tex,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame											);
	void							delete_vbo									(	unsigned int			vbo_index,
																					unsigned int			vbo_frame											);
	unsigned int					gen_gl_cuda_vbo								(	GLuint&					vboId,
																					cudaGraphicsResource*	cuda_vbo_res,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame,
																					unsigned int			vbo_res_flags = cudaGraphicsMapFlagsWriteDiscard	);
	unsigned int					gen_gl_cuda_vbo2							(	GLuint&					vboId,
																					cudaGraphicsResource*	cuda_vbo_res,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame,
																					unsigned int			vbo_res_flags = cudaGraphicsMapFlagsWriteDiscard	);
	unsigned int					gen_gl_cuda_vbo3							(	GLuint&					vboId,
																					vector<float>&			data,
																					cudaGraphicsResource*	cuda_vbo_res,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame,
																					unsigned int			vbo_res_flags = cudaGraphicsMapFlagsWriteDiscard	);
	void							delete_gl_cuda_vbo							(	unsigned int			vbo_index,
																					unsigned int			vbo_frame											);
	void							gen_empty_vbo								(	GLuint&					vboId,
																					unsigned int			vbo_index,
																					unsigned int			vbo_frame,
																					unsigned int			size												);
	void							update_vbo									(	unsigned int			vbo_index,
																					unsigned int			vbo_frame											);
	void							update_gl_cuda_vbo							(	unsigned int			gl_cuda_vbo_index,
																					unsigned int			gl_cuda_vbo_frame									);
	void							render_vbo									(	GLuint&					vboId,
																					unsigned int			size												);
	void							render_vbo									(	GLuint&					vboId,
																					unsigned int			size,
																					unsigned int			mode												);
	void							render_vbo									(	GLuint&					vboId,
																					GLint					data_size,
																					GLenum					data_type,
																					GLenum					draw_mode,
																					GLint					draw_size											);
	void							render_bumped_vbo							(	GLuint&					vboId,
																					unsigned int			size												);
	void							render_instanced_vbo						(	GLuint&					vboId,
																					GLuint&					texture,
																					unsigned int			size,
																					unsigned int			count,
																					unsigned int			positions_sizeInBytes,
																					float*					positions,
																					unsigned int			rotations_sizeInBytes,
																					float*					rotations,
																					float*					viewMat												);
	void							render_instanced_vbo						(	GLuint&					vboId,
																					GLuint&					texture,
																					unsigned int			size,
																					unsigned int			count,
																					unsigned int			positions_sizeInBytes,
																					float*					positions,
																					unsigned int			rotations_sizeInBytes,
																					float*					rotations,
																					unsigned int			scales_sizeInBytes,
																					float*					scales,
																					float*					viewMat												);
	void							render_instanced_culled_vbo					(	GLuint&					vboId,
																					GLuint&					texture,
																					GLuint&					pos_vbo_id,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat												);
	void							render_instanced_culled_rigged_vbo			(	GLuint&					vboId,
																					GLuint&					texture,
																					GLuint&					zonesTexture,
																					GLuint&					weightsTexture,
																					GLuint&					displacementTexture,
																					GLuint&					pos_vbo_id,
																					float					dt,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);

	void							render_instanced_culled_rigged_vbo			(	GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					texture,
																					GLuint&					zonesTexture,
																					GLuint&					weightsTexture,
																					GLuint&					displacementTexture,
																					GLuint&					pos_vbo_id,
																					float					dt,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);

	void							render_instanced_culled_rigged_vbo			(	GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					skin_texture,
																					GLuint&					hair_texture,
																					GLuint&					cap_texture,
																					GLuint&					torso_texture,
																					GLuint&					legs_texture,
																					GLuint&					zonesTexture,
																					GLuint&					weightsTexture,
																					GLuint&					displacementTexture,
																					GLuint&					pos_vbo_id,
																					float					dt,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);

	void							render_instanced_culled_rigged_vbo			(	GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					skin_texture,
																					GLuint&					hair_texture,
																					GLuint&					cap_texture,
																					GLuint&					torso_texture,
																					GLuint&					legs_texture,
																					GLuint&					clothing_color_table_tex_id,
																					GLuint&					clothing_patterns_tex_id,
																					GLuint&					zonesTexture,
																					GLuint&					weightsTexture,
																					GLuint&					displacementTexture,
																					GLuint&					pos_vbo_id,
																					float					dt,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);

#ifdef DEMO_SHADER
	void							render_instanced_culled_rigged_vbo2			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,

																					GLuint&					clothing_color_table_tex_id,
																					GLuint&					pattern_color_table_tex_id,
																					GLuint&					global_mt_tex_id,
																					GLuint&					torso_mt_tex_id,
																					GLuint&					legs_mt_tex_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					GLuint&					facial_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe,

																					float					doHandD,
																					float					doPatterns,
																					float					doColor,
																					float					doFacial											);
#else
	void							render_instanced_culled_rigged_vbo2			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,

																					GLuint&					clothing_color_table_tex_id,
																					GLuint&					pattern_color_table_tex_id,
																					GLuint&					global_mt_tex_id,
																					GLuint&					torso_mt_tex_id,
																					GLuint&					legs_mt_tex_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					GLuint&					facial_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);
#endif

	void							render_instanced_culled_rigged_vbo3			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,

																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					float*					projMat,
																					float*					shadowMat,
																					bool					wireframe											);

#ifdef DEMO_SHADER
	void							render_instanced_culled_rigged_vbo4			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					ids_buffer,
																					GLuint&					pos_buffer,

																					GLuint&					clothing_color_table_tex_id,
																					GLuint&					pattern_color_table_tex_id,
																					GLuint&					global_mt_tex_id,
																					GLuint&					torso_mt_tex_id,
																					GLuint&					legs_mt_tex_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					GLuint&					facial_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe,

																					float					doHandD,
																					float					doPatterns,
																					float					doColor,
																					float					doFacial											);
#else
	void							render_instanced_culled_rigged_vbo4			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,

																					GLuint&					clothing_color_table_tex_id,
																					GLuint&					pattern_color_table_tex_id,
																					GLuint&					global_mt_tex_id,
																					GLuint&					torso_mt_tex_id,
																					GLuint&					legs_mt_tex_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					GLuint&					facial_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);
#endif

	void							render_instanced_culled_rigged_vbo5			(	Camera*					cam,
																					GLuint&					vboId,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,

																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,

																					float					lod,
																					float					gender,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					float*					projMat,
																					float*					shadowMat,
																					bool					wireframe											);

	void							render_instanced_culled_rigged_vbo			(	GLuint&					vboId,
																					GLuint&					texture1,
																					GLuint&					texture2,
																					GLuint&					texture3,
																					GLuint&					texture4,
																					GLuint&					zonesTexture,
																					GLuint&					weightsTexture,
																					GLuint&					displacementTexture,
																					GLuint&					pos_vbo_id,
																					float					dt,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					bool					wireframe											);
	void							render_instanced_vbo						(	GLuint&					vboId,
																					GLuint&					texture,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					GLuint					uPosBuffer,
																					GLuint					uRotBuffer,
																					GLuint					uScaBuffer											);
	void							render_instanced_bumped_vbo					(	GLuint&					vboId,
																					GLuint&					texture,
																					GLuint&					normal_texture,
																					GLuint&					specular_texture,
																					unsigned int			size,
																					unsigned int			count,
																					unsigned int			positions_sizeInBytes,
																					float*					positions,
																					unsigned int			rotations_sizeInBytes,
																					float*					rotations,
																					float*					viewMat												);
	void							render_instanced_bumped_vbo					(	GLuint&					vboId,
																					GLuint&					texture,
																					GLuint&					normal_texture,
																					GLuint&					specular_texture,
																					unsigned int			size,
																					unsigned int			count,
																					float*					viewMat,
																					GLuint					uPosBuffer,
																					GLuint					uRotBuffer											);
	void							setInstancingLocations						(	string&					instancing_shader_name,
																					string&					instancing_positions_name,
																					string&					instancing_rotations_name,
																					string&					instancing_scales_name,
																					string&					instancing_normal_name,
																					string&					instancing_texCoord0_name,
																					string&					instancing_viewmat_name								);
	void							setInstancingCulledLocations				(	string&					instancing_culled_shader_name,
																					string&					instancing_culled_normal_name,
																					string&					instancing_culled_texCoord0_name,
																					string&					instancing_culled_viewmat_name						);
	void							setInstancingCulledRiggedLocations			(	string&					instancing_culled_rigged_shader_name,
																					string&					instancing_culled_rigged_normal_name,
																					string&					instancing_culled_rigged_dt_name,
																					string&					instancing_culled_rigged_texCoord0_name,
																					string&					instancing_culled_rigged_viewmat_name				);
	void							setInstancingCulledRiggedShadowLocations	(	string&					instancing_culled_rigged_shadow_shader_name,
																					string&					instancing_culled_rigged_shadow_normal_name,
																					string&					instancing_culled_rigged_shadow_texCoord0_name,
																					string&					instancing_culled_rigged_shadow_dt_name,
																					string&					instancing_culled_rigged_shadow_viewmat_name,
																					string&					instancing_culled_rigged_shadow_projmat_name,
																					string&					instancing_culled_rigged_shadow_shadowmat_name				);
	void							setBumpedInstancingLocations				(	string&					bump_shader_name,
																					string&					bump_positions_name,
																					string&					bump_rotations_name,
																					string&					bump_normal_name,
																					string&					bump_tangent_name,
																					string&					bump_texCoord0_name,
																					string&					bump_viewmat_name									);

public:
	vector<vector<VBO> >			vbos;
	vector<vector<GL_CUDA_VBO>>		gl_cuda_vbos;
	string							instancing_culled_rigged_shader_name;
	string							instancing_culled_rigged_shadow_shader_name;
private:
	LogManager*						log_manager;
	GlErrorManager*					err_manager;
	GlslManager*					glsl_manager;
	unsigned int					reused;
	map<string, unsigned int>		filename_index_map;

	string							instancing_shader_name;
	string							instancing_positions_name;
	string							instancing_rotations_name;
	string							instancing_scales_name;
	string							instancing_normal_name;
	string							instancing_texCoord0_name;
	string							instancing_viewmat_name;
	GLuint							instancing;
	unsigned int					instancing_normalLocation;
	unsigned int					instancing_texCoord0Location;
	GLint							instancing_uPosLocation;
	GLint							instancing_uRotLocation;
	GLint							instancing_uScaLocation;

	string							instancing_culled_shader_name;
	string							instancing_culled_normal_name;
	string							instancing_culled_texCoord0_name;
	string							instancing_culled_viewmat_name;
	GLuint							instancing_culled;
	unsigned int					instancing_culled_normalLocation;
	unsigned int					instancing_culled_texCoord0Location;

	string							instancing_culled_rigged_normal_name;
	string							instancing_culled_rigged_dt_name;
	string							instancing_culled_rigged_texCoord0_name;
	string							instancing_culled_rigged_viewmat_name;
	GLuint							instancing_culled_rigged;
	unsigned int					instancing_culled_rigged_normalLocation;
	unsigned int					instancing_culled_rigged_dtLocation;
	unsigned int					instancing_culled_rigged_texCoord0Location;

	string							instancing_culled_rigged_shadow_normal_name;
	string							instancing_culled_rigged_shadow_texCoord0_name;
	string							instancing_culled_rigged_shadow_dt_name;
	string							instancing_culled_rigged_shadow_viewmat_name;
	string							instancing_culled_rigged_shadow_projmat_name;
	string							instancing_culled_rigged_shadow_shadowmat_name;
	GLuint							instancing_culled_rigged_shadow;
	unsigned int					instancing_culled_rigged_shadow_normalLocation;
	unsigned int					instancing_culled_rigged_shadow_texCoord0Location;
	unsigned int					instancing_culled_rigged_shadow_dtLocation;

	string							bump_shader_name;
	string							bump_positions_name;
	string							bump_rotations_name;
	string							bump_normal_name;
	string							bump_tangent_name;
	string							bump_texCoord0_name;
	string							bump_viewmat_name;
	GLuint							bumped_instancing;
	unsigned int					bumped_instancing_tangentLocation;
	unsigned int					bumped_instancing_normalLocation;
	unsigned int					bumped_instancing_texCoord0Location;
	GLint							bumped_instancing_uPosLocation;
	GLint							bumped_instancing_uRotLocation;

	unsigned int*					indexPtr;
	GLuint							ibHandle;
};

#endif

//=======================================================================================
