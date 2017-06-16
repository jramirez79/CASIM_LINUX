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

#include <iostream>
#include <fstream>

#include <unordered_map>
#include <vector>
#include <string>

#include "cMacros.h"

#include "cVertex.h"
#include "cModel3D.h"
#include "cCamera.h"

using namespace std;

//=======================================================================================

#ifndef __CHARACTER_MODEL
#define __CHARACTER_MODEL

class CharacterModel
{
public:
											CharacterModel				(	void										);
											CharacterModel				(	LOD_TYPE				_LOD,
																			string					_name,
																			VboManager*				_vbo_manager,
																			GlslManager*			_glsl_manager,
																			Model3D*				_head,
																			Model3D*				_hair,
																			Model3D*				_torso,
																			Model3D*				_legs				);
											CharacterModel				(	LOD_TYPE				_LOD,
																			string					_name,
																			VboManager*				_vbo_manager,
																			GlslManager*			_glsl_manager,
																			Model3D*				_model				);

											~CharacterModel				(	void										);

	bool									stitch_parts				(	void										);
	bool									save_obj					(	string&					filename			);

#if defined CUDA_PATHS || defined OSC_PATHS || defined MPI_PATHS || defined MPI_PATHS_ON_NODES
#ifdef DEMO_SHADER
	void									draw_instanced_culled_rigged(	Camera*					cam,
																			unsigned int			frame,
																			unsigned int			instances,
																			unsigned int			_AGENTS_NPOT,
																			unsigned int			_ANIMATION_LENGTH,
																			unsigned int			_STEP,
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
																			float*					viewMat,
																			bool					wireframe,
																			float					doHandD,
																			float					doPatterns,
																			float					doColor,
																			float					doFacial					);
#else
	void									draw_instanced_culled_rigged(	Camera*					cam,
																			unsigned int			frame,
																			unsigned int			instances,
																			unsigned int			_AGENTS_NPOT,
																			unsigned int			_ANIMATION_LENGTH,
																			unsigned int			_STEP,
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
																			float*					viewMat,
																			bool					wireframe			);
#endif
#else
#ifdef DEMO_SHADER
	void									draw_instanced_culled_rigged(	Camera*					cam,
																			unsigned int			frame,
																			unsigned int			instances,
																			unsigned int			_AGENTS_NPOT,
																			unsigned int			_ANIMATION_LENGTH,
																			unsigned int			_STEP,
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
																			float*					viewMat,
																			bool					wireframe,
																			float					doHandD,
																			float					doPatterns,
																			float					doColor,
																			float					doFacial					);
#else
	void									draw_instanced_culled_rigged(	Camera*					cam,
																			unsigned int			frame,
																			unsigned int			instances,
																			unsigned int			_AGENTS_NPOT,
																			unsigned int			_ANIMATION_LENGTH,
																			unsigned int			_STEP,
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
																			float*					viewMat,
																			bool					wireframe			);
#endif
#endif

#if defined CUDA_PATHS || defined OSC_PATHS || defined MPI_PATHS || defined MPI_PATHS_ON_NODES
	void									draw_instanced_culled_rigged_shadow(	Camera*					cam,
																					unsigned int			frame,
																					unsigned int			instances,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					float					lod,
																					float					gender,
																					float*					viewMat,
																					float*					projMat,
																					float*					shadowMat,
																					bool					wireframe			);
#else
	void									draw_instanced_culled_rigged_shadow(	Camera*					cam,
																					unsigned int			frame,
																					unsigned int			instances,
																					unsigned int			_AGENTS_NPOT,
																					unsigned int			_ANIMATION_LENGTH,
																					unsigned int			_STEP,
																					GLuint&					posTextureTarget,
																					GLuint&					posTextureId,
																					GLuint&					pos_vbo_id,
																					GLuint&					rigging_mt_tex_id,
																					GLuint&					animation_mt_tex_id,
																					float					lod,
																					float					gender,
																					float*					viewMat,
																					float*					projMat,
																					float*					shadowMat,
																					bool					wireframe			);
#endif

private:

	LOD_TYPE								LOD;
	Model3D*								head;
	Model3D*								hair;
	Model3D*								torso;
	Model3D*								legs;
	Model3D*								model;
	VboManager*								vbo_manager;
	GlslManager*							glsl_manager;

	vector<Location4>						unique_locations;
	vector<Normal>							unique_normals;
	vector<Uv>								unique_uvs;
	vector<Face3>							faces;

	string									name;
	vector<GLuint>							sizes;
	vector<GLuint>							ids;
};

#endif
