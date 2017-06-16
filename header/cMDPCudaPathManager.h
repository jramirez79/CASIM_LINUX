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

#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <cuda_runtime.h>
#include <math.h>

#include "cMacros.h"
#include "cVertex.h"
#include "cLogManager.h"
#include "cVboManager.h"
#include "cFboManager.h"
//#include "cStaticLod.h"
#include "cProjectionManager.h"
#include "cGlErrorManager.h"

//=======================================================================================

#ifndef __MDP_CUDA_PATH_MANAGER
#define __MDP_CUDA_PATH_MANAGER

class MDPCudaPathManager
{
public:
								MDPCudaPathManager		( LogManager*		log_manager,
														  VboManager*		vbo_manager,
														  FboManager*		fbo_manager,
														  string			_fbo_pos_name,
														  string			_fbo_pos_tex_name	);
								~MDPCudaPathManager		( void									);

	bool						init					( vector<float>&	_init_pos,
														  vector<float>&	_policy,
														  vector<float>&	_density			);
	bool						init					( void									);
	void						runCuda					( float				scene_width,
														  float				scene_height,
														  float				scene_width_in_tiles,
														  float				scene_height_in_tiles,
														  float				tile_width_or_side,
														  float				tile_height,
														  unsigned int		texture_width,
														  unsigned int		texture_height,
														  float				parameter			);
	void						updatePolicy			( vector<float>&	_policy				);
	void						getDensity				( vector<float>&	_density			);

	unsigned int				cuda_ipos_vbo_id;
	unsigned int				cuda_ipos_vbo_size;
	unsigned int				cuda_ipos_vbo_index;
	unsigned int				cuda_ipos_vbo_frame;

	unsigned int				cuda_cpos_vbo_id;
	unsigned int				cuda_cpos_vbo_size;
	unsigned int				cuda_cpos_vbo_index;
	unsigned int				cuda_cpos_vbo_frame;

	unsigned int				cuda_poli_vbo_id;
	unsigned int				cuda_poli_vbo_size;
	unsigned int				cuda_poli_vbo_index;
	unsigned int				cuda_poli_vbo_frame;

	unsigned int				cuda_dens_vbo_id;
	unsigned int				cuda_dens_vbo_size;
	unsigned int				cuda_dens_vbo_index;
	unsigned int				cuda_dens_vbo_frame;

	GLuint						texture_buffer_id;
	vector<float>				init_positions;
	vector<float>				policy;
	vector<float>				density;
	vector<VboRenderTexture>	render_textures;

private:
	int							getMaxGflopsDeviceId	( void								);
	int							_ConvertSMVer2Cores		( int				major,
														  int				minor			);

	LogManager*					log_manager;
	VboManager*					vbo_manager;
	FboManager*					fbo_manager;
	cudaGraphicsResource*		cuda_ipos_vbo_res;
	cudaGraphicsResource*		cuda_cpos_vbo_res;
	cudaGraphicsResource*		cuda_poli_vbo_res;
	cudaGraphicsResource*		cuda_dens_vbo_res;
	ProjectionManager*			proj_manager;

	string						fbo_pos_name;
	string						fbo_pos_tex_name;

	bool						policy_reset;
};

#endif

//=======================================================================================
