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
#include <string>

#include "cMacros.h"
#include "cCharacterGroup.h"
#include "cStaticLod.h"
#include "cLogManager.h"
#include "cFboManager.h"
#include "cVboManager.h"
#include "cCamera.h"
#include "cMDPCudaPathManager.h"
#include "cOSCCudaPathManager.h"
#include "cMPICudaPathManager.h"

using namespace std;

//=======================================================================================

#ifndef __CROWD_GROUP
#define __CROWD_GROUP

class CrowdGroup
{
public:
						CrowdGroup	( 	CharacterGroup*	_cgroup,
										StaticLod*		_static_lod,
										string			_animation,
										string			_fbo_lod_name,
										string			_fbo_pos_tex_name,
										string			_fbo_ids_tex_name,
										float			_percentage,
										float			_dt,
										GLuint			_frames,
										GLuint			_duration,
										GLuint			_frame,
										GLuint			_width,
										GLuint			_height			);
						CrowdGroup	( 	CharacterGroup*	_cgroup,
										string			_animation,
										float			_percentage,
										float			_dt,
										GLuint			_frames,
										GLuint			_duration,
										GLuint			_frame			);
						~CrowdGroup	( 	void 							);

	GLuint				getWidth	(	void							);
	GLuint				getHeight	(	void							);
#ifdef LOCAL_POS_TEXTURE
	bool				init_paths	(	LogManager*		log_manager,
										FboManager*		fbo_manager,
										VboManager*		vbo_manager,
										float			plane_scale		);
	GLuint				init_ids	(	FboManager*		fbo_manager,
										GLuint&			_groupId,
										GLuint			offset			);
	void				run_paths	(	void							);
#endif
	void				nextFrame	(	void							);

#ifdef CUDA_PATHS
#ifdef DEMO_SHADER
	GLuint				draw		(	FboManager*			fbo_manager,
										VboManager*			vbo_manager,
										GlslManager*		glsl_manager,
										MDPCudaPathManager*	cuda_path_manager,
										StaticLod*			_static_lod,
										struct sVBOLod*		vboCulledLOD,
										string				_fbo_lod_name,
										string				_fbo_pos_tex_name,
										unsigned int		_AGENTS_NPOT,
										Camera*				camera,
										float*				viewMat,
										float*				projMat,
										float*				shadowMat,
										bool				wireframe,
										bool				shadows,
										bool				doHandD,
										bool				doPatterns,
										bool				doColor,
										bool				doFacial		);
	void				draw		(	FboManager*			fbo_manager,
										VboManager*			vbo_manager,
										GlslManager*		glsl_manager,
										MDPCudaPathManager*	cuda_path_manager,
										Camera*				camera,
										float*				viewMat,
										float*				projMat,
										float*				shadowMat,
										bool				wireframe,
										bool				shadows,
										bool				doHandD,
										bool				doPatterns,
										bool				doColor,
										bool				doFacial		);
#else
	GLuint				draw		(	FboManager*			fbo_manager,
										VboManager*			vbo_manager,
										GlslManager*		glsl_manager,
										CudaPathManager*	cuda_path_manager,
										StaticLod*			_static_lod,
										struct sVBOLod*		vboCulledLOD,
										string				_fbo_lod_name,
										string				_fbo_pos_tex_name,
										unsigned int		_AGENTS_NPOT,
										Camera*				camera,
										float*				viewMat,
										float*				projMat,
										float*				shadowMat,
										bool				wireframe,
										bool				shadows			);
	void				draw		(	FboManager*			fbo_manager,
										VboManager*			vbo_manager,
										GlslManager*		glsl_manager,
										CudaPathManager*	cuda_path_manager,
										Camera*				camera,
										float*				viewMat,
										float*				projMat,
										float*				shadowMat,
										bool				wireframe,
										bool				shadows			);
#endif
#elif defined OSC_PATHS
	GLuint				draw		(	FboManager*				fbo_manager,
										VboManager*				vbo_manager,
										GlslManager*			glsl_manager,
										cOSCCudaPathManager*	osc_cuda_path_manager,
										StaticLod*				_static_lod,
										struct sVBOLod*			vboCulledLOD,
										string					_fbo_lod_name,
										string					_fbo_pos_tex_name,
										unsigned int			_AGENTS_NPOT,
										Camera*					camera,
										float*					viewMat,
										float*					projMat,
										float*					shadowMat,
										bool					wireframe,
										bool					shadows,
										bool					doHandD,
										bool					doPatterns,
										bool					doColor,
										bool					doFacial		);
#elif defined MPI_PATHS || defined MPI_PATHS_ON_NODES
	GLuint				draw		(	FboManager*				fbo_manager,
										VboManager*				vbo_manager,
										GlslManager*			glsl_manager,
										cMPICudaPathManager*	mpi_cuda_path_manager,
										StaticLod*				_static_lod,
										struct sVBOLod*			vboCulledLOD,
										string					_fbo_lod_name,
										string					_fbo_pos_tex_name,
										unsigned int			_AGENTS_NPOT,
										Camera*					camera,
										float*					viewMat,
										float*					projMat,
										float*					shadowMat,
										bool					wireframe,
										bool					shadows,
										bool					doHandD,
										bool					doPatterns,
										bool					doColor,
										bool					doFacial		);

#else
#ifdef DEMO_SHADER
	GLuint				draw		(	FboManager*		fbo_manager,
										VboManager*		vbo_manager,
										GlslManager*	glsl_manager,
										StaticLod*		_static_lod,
										struct sVBOLod*	vboCulledLOD,
										string			_fbo_lod_name,
										string			_fbo_pos_tex_name,
										unsigned int	_AGENTS_NPOT,
										Camera*			camera,
										float*			viewMat,
										float*			projMat,
										float*			shadowMat,
										bool			wireframe,
										bool			shadows,
										bool			doHandD,
										bool			doPatterns,
										bool			doColor,
										bool			doFacial		);
	void				draw		(	FboManager*		fbo_manager,
										VboManager*		vbo_manager,
										GlslManager*	glsl_manager,
										Camera*			camera,
										float*			viewMat,
										float*			projMat,
										float*			shadowMat,
										bool			wireframe,
										bool			shadows,
										bool			doHandD,
										bool			doPatterns,
										bool			doColor,
										bool			doFacial		);
#else
	GLuint				draw		(	FboManager*		fbo_manager,
										VboManager*		vbo_manager,
										GlslManager*	glsl_manager,
										StaticLod*		_static_lod,
										struct sVBOLod*	vboCulledLOD,
										string			_fbo_lod_name,
										string			_fbo_pos_tex_name,
										unsigned int	_AGENTS_NPOT,
										Camera*			camera,
										float*			viewMat,
										float*			projMat,
										float*			shadowMat,
										bool			wireframe,
										bool			shadows			);
	void				draw		(	FboManager*		fbo_manager,
										VboManager*		vbo_manager,
										GlslManager*	glsl_manager,
										Camera*			camera,
										float*			viewMat,
										float*			projMat,
										float*			shadowMat,
										bool			wireframe,
										bool			shadows			);
#endif
#endif

	StaticLod*			static_lod;
#ifdef LOCAL_POS_TEXTURE
	CudaPathManager*	cuda_path_manager;
	float				dt;
	float				path_param;
	vector<float>		instance_positions_flat;
	vector<float>		instance_rotations_flat;
	vector<float>		instance_control_points;
	vector<float>		instance_ids_flat;
#endif
	sVBOLod				vbo_lod[NUM_LOD];
	CharacterGroup*		cgroup;
	string				animation;
	string				fbo_lod_name;
	string				fbo_pos_tex_name;
	string				fbo_ids_tex_name;
	float				percentage;
	GLuint				frames;
	GLuint				duration;
	GLuint				frame;
	GLuint				width;
	GLuint				height;
	GLuint				id;
};
#endif
