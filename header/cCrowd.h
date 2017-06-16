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
#include "cCrowdGroup.h"
#include "cCharacterGroup.h"
#include "cLogManager.h"
#include "cGlErrorManager.h"
#include "cFboManager.h"
#include "cCamera.h"
#include "cMDPCudaPathManager.h"
#include "cOSCCudaPathManager.h"
#include "cMPICudaPathManager.h"

#include <string>
#include <glm/glm.hpp>

using namespace std;

//=======================================================================================

#ifndef __CROWD
#define __CROWD

class Crowd
{
public:
							Crowd			(	LogManager*					log_manager_,
												GlErrorManager*				err_manager_,
												FboManager*					fbo_manager_,
												VboManager*					vbo_manager_,
												GlslManager*				glsl_manager_,
												string&						name_,
												GLuint						width_,
												GLuint						height_			);
							~Crowd			(	void										);

	void					addGroup		(	CharacterGroup*				_group,
												StaticLod*					_static_lod,
												string&						_animation,
												string&						_fbo_lod_name,
												string&						_fbo_pos_tex_name,
												string&						_fbo_ids_tex_name,
												float						_percentage,
												GLuint						_frames,
												GLuint						_duration		);
	void					addGroup		(	CharacterGroup*				_group,
												string&						_animation,
												float						_percentage,
												GLuint						_frames,
												GLuint						_duration		);
	string&					getName			(	void										);
	string&					getFboLodName	(	void										);
	string&					getFboPosTexName(	void										);
	GLuint					getWidth		(	void										);
	GLuint					getHeight		(	void										);
	vector<CrowdGroup*>&	getGroups		(	void										);
	void					initFboInputs	(	vector<InputFbo*>&  		fbo_inputs		);
	void					setFboManager	(	FboManager*					_fbo_manager	);
	bool					init_paths		(	float						plane_scale,
												vector<float>&				_policy,
												unsigned int				grid_width,
												unsigned int				grid_height,
												vector<glm::vec2>&			occupation,
												vector<GROUP_FORMATION>&	formations		);
	void					updatePolicy	(	vector<float>&				_policy			);
	void					getDensity		(	vector<float>&				_density		);
	void					init_ids		(	GLuint&						groupOffset,
												GLuint&						agentOffset		);
	void					run_paths		(	void										);
	void					nextFrame		(	void										);
	GLuint&					getCudaTBO		(	void										);
	void					initTexCoords	(	void										);
	GLuint&					getPosTexCoords	(	void										);
	GLuint					getPosTexSize	(	void										);
#ifdef DEMO_SHADER
	void					draw			(	Camera*				camera,
												float*				viewMat,
												float*				projMat,
												float*				shadowMat,
												bool				wireframe,
												bool				shadows,
												bool				doHandD,
												bool				doPatterns,
												bool				doColor,
												bool				doFacial				);
#else
	void					draw			(	Camera*				camera,
												float*				viewMat,
												float*				projMat,
												float*				shadowMat,
												bool				wireframe,
												bool				shadows					);
#endif

	vector<GLuint>			models_drawn;

private:
	bool					is_pos_occupied	(	vector<glm::vec2>&	occupation,
												glm::vec2			position,
												float				radius					);
	float					rand_between	(	float				min,
												float				max						);

	LogManager*				log_manager;
	GlErrorManager*			err_manager;
	FboManager*				fbo_manager;
	VboManager*				vbo_manager;
	GlslManager*			glsl_manager;

	MDPCudaPathManager*		cuda_path_manager;
	cOSCCudaPathManager*	osc_cuda_path_manager;
	cMPICudaPathManager*	mpi_cuda_path_manager;

	string					fbo_lod_name;
	string					fbo_pos_tex_name;
	string					fbo_ids_tex_name;
	float					path_param;
	vector<float>			instance_positions_flat;
	vector<float>			instance_rotations_flat;
	vector<float>			instance_control_points;
	vector<float>			instance_ids_flat;
	StaticLod*				static_lod;
	sVBOLod					vbo_lod[NUM_LOD];

	string					name;
	GLuint					width;
	GLuint					height;
	vector<CrowdGroup*>		groups;

	float					scene_width;
	float					scene_height;
	float					scene_width_in_tiles;
	float					scene_height_in_tiles;
	float					tile_width;
	float					tile_height;
	float					tile_side;
	float					personal_space;
	vector<float>			policy;
	vector<float>			density;

	unsigned int			pos_tc_index;
	unsigned int			pos_tc_frame;
	unsigned int			pos_tc_size;
	unsigned int			posTexCoords;

	unsigned int			pos_crashes;
};

#endif

//=======================================================================================
