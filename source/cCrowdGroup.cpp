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

#include "cCrowdGroup.h"

//=======================================================================================
//
CrowdGroup::CrowdGroup( CharacterGroup*	_cgroup,
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
						GLuint			_height			)
{
	id							= 0;
	cgroup						= _cgroup;
	static_lod					= _static_lod;
	animation					= _animation;
	fbo_lod_name				= _fbo_lod_name;
	fbo_pos_tex_name			= _fbo_pos_tex_name;
	fbo_ids_tex_name			= _fbo_ids_tex_name;
	percentage					= _percentage;
	frames						= _frames;
	duration					= _duration;
	frame						= _frame;
	width						= _width;
	height						= _height;
	for( unsigned int i = 0; i < NUM_LOD; i++ )
	{
		vbo_lod[i].id					= 0;
		vbo_lod[i].primitivesGenerated	= 0;
		vbo_lod[i].primitivesWritten	= 0;
	}
#ifdef LOCAL_POS_TEXTURE
	dt							= _dt;
	cuda_path_manager			= NULL;
	path_param					= 0.0f;
#endif
	cgroup->addAnimation( animation, frames, duration );
}
//
//=======================================================================================
//
CrowdGroup::CrowdGroup( CharacterGroup*	_cgroup,
						string			_animation,
						float			_percentage,
						float			_dt,
						GLuint			_frames,
						GLuint			_duration,
						GLuint			_frame		)
{
	id							= 0;
	cgroup						= _cgroup;
	static_lod					= NULL;
	animation					= _animation;
	fbo_lod_name				= "";
	fbo_pos_tex_name			= "";
	fbo_ids_tex_name			= "";
	percentage					= _percentage;
	frames						= _frames;
	duration					= _duration;
	frame						= _frame;
	width						= 0;
	height						= 0;
	for( unsigned int i = 0; i < NUM_LOD; i++ )
	{
		vbo_lod[i].id					= 0;
		vbo_lod[i].primitivesGenerated	= 0;
		vbo_lod[i].primitivesWritten	= 0;
	}
#ifdef LOCAL_POS_TEXTURE
	dt							= _dt;
	cuda_path_manager			= NULL;
	path_param					= 0.0f;
#endif
	cgroup->addAnimation( animation, frames, duration );
}
//
//=======================================================================================
//
CrowdGroup::~CrowdGroup( void )
{

}
//
//=======================================================================================
//
GLuint CrowdGroup::getWidth( void )
{
	return width;
}
//
//=======================================================================================
//
GLuint CrowdGroup::getHeight( void )
{
	return height;
}
//
//=======================================================================================
//
#ifdef LOCAL_POS_TEXTURE
bool CrowdGroup::init_paths(	LogManager*	log_manager,
								FboManager*	fbo_manager,
								VboManager*	vbo_manager,
								float		plane_scale	)
{
	for( unsigned int s = 0; s < width * height; s++ )
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
#ifndef DEMO_ALONE
		x = (float)(rand() % (int)(plane_scale*2.0f));
		z = (float)(rand() % (int)(plane_scale*2.0f));
		int sign = rand() % 100;
		if( sign < 50 )
		{
			x = -x;
		}
		sign = rand() % 100;
		if( sign < 50 )
		{
			z = -z;
		}
#endif
		instance_positions_flat.push_back( x     );
		instance_positions_flat.push_back( y     );
		instance_positions_flat.push_back( z     );
		instance_positions_flat.push_back( 0.0f  );
		instance_rotations_flat.push_back( 90.0f );		// CENITAL
		instance_rotations_flat.push_back( 0.0f  );		// AZIMUTH
		instance_rotations_flat.push_back( 0.0f  );
		instance_rotations_flat.push_back( 0.0f  );
		instance_control_points.push_back( x	 );
		instance_control_points.push_back( 0.0f	 );
		instance_control_points.push_back( z     );
		instance_control_points.push_back( 0.0f  );
	}
	float* data = new float[width*height*4];
	for( unsigned int i = 0; i < width*height*4; i++ )
	{
		data[i] = 1.0f;
	}
	for( unsigned int i = 0; i < width*height*4; i++ )
	{
		data[i] = instance_positions_flat[i];
	}

	glBindTexture( fbo_manager->fbos[fbo_lod_name].fbo_tex_target, fbo_manager->texture_ids[fbo_pos_tex_name] );
	{
		glTexSubImage2D( fbo_manager->fbos[fbo_lod_name].fbo_tex_target,
						 0,
						 0,
						 0,
						 fbo_manager->fbos[fbo_lod_name].fbo_width,
						 fbo_manager->fbos[fbo_lod_name].fbo_height,
						 GL_RGBA,
						 GL_FLOAT,
						 data										);
	}
	glBindTexture( fbo_manager->fbos[fbo_lod_name].fbo_tex_target, NULL );
	delete data;

	cuda_path_manager = new CudaPathManager( log_manager, vbo_manager, fbo_manager, fbo_lod_name, fbo_pos_tex_name );
	return cuda_path_manager->init( instance_positions_flat, instance_control_points );
}
//
//=======================================================================================
//
GLuint CrowdGroup::init_ids(	FboManager*	fbo_manager,
								GLuint&		_groupId,
								GLuint		offset		)
{
	id	= _groupId;
	GLuint curr_id = 0;
	for( unsigned int s = 0; s < width * height; s++ )
	{
		curr_id = offset + s;
		float id = (float)curr_id;
		instance_ids_flat.push_back( (float)id	);
		instance_ids_flat.push_back( 0.0f		);
		instance_ids_flat.push_back( id			);
		instance_ids_flat.push_back( 0.0f		);
	}
	float* data = new float[width*height*4];
	for( unsigned int i = 0; i < width*height*4; i++ )
	{
		data[i] = 1.0f;
	}
	for( unsigned int i = 0; i < width*height*4; i++ )
	{
		data[i] = instance_ids_flat[i];
	}

	glBindTexture( fbo_manager->fbos[fbo_lod_name].fbo_tex_target, fbo_manager->texture_ids[fbo_ids_tex_name] );
	{
		glTexSubImage2D( fbo_manager->fbos[fbo_lod_name].fbo_tex_target,
						 0,
						 0,
						 0,
						 fbo_manager->fbos[fbo_lod_name].fbo_width,
						 fbo_manager->fbos[fbo_lod_name].fbo_height,
						 GL_RGBA,
						 GL_FLOAT,
						 data										);
	}
	glBindTexture( fbo_manager->fbos[fbo_lod_name].fbo_tex_target, NULL );
	delete data;
	return curr_id;
}
#endif
//
//=======================================================================================
//
void CrowdGroup::nextFrame( void )
{
	cgroup->nextFrame();
}
//
//=======================================================================================
//
#ifdef LOCAL_POS_TEXTURE
void CrowdGroup::run_paths(	void )
{
	cuda_path_manager->runCuda( width, height, path_param );
	path_param += 0.001f;
	if( path_param > 1.0f )
	{
		// Reset param:
		path_param = 0.0f;
	}
}
#endif
//
//=======================================================================================
//
#ifdef CUDA_PATHS
#ifdef DEMO_SHADER
GLuint CrowdGroup::draw(	FboManager*			fbo_manager,
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
							bool				doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelectionCuda(	fbo_target,
																(float)id,
																vbo_lod,
																camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											cuda_path_manager->texture_buffer_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
	return not_culled;
}
//
//=======================================================================================
//
void CrowdGroup::draw(	FboManager*			fbo_manager,
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
						bool				doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[fbo_pos_tex_name];
	GLuint ids_texture		= fbo_manager->texture_ids[fbo_ids_tex_name];
	static_lod->runAssignmentAndSelection(	fbo_target,
											fbo_texture,
											ids_texture,
											vbo_lod,
											camera		);

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											width,
											vbo_lod,
											cuda_path_manager->texture_buffer_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
}
//
//=======================================================================================
//
#else
GLuint CrowdGroup::draw(	FboManager*			fbo_manager,
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
							bool				shadows				)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[_fbo_pos_tex_name];
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelection(	fbo_target,
															(float)id,
															vbo_lod,
															camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											cuda_path_manager->texture_buffer_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows		);
	return not_culled;
}
//
//=======================================================================================
//
void CrowdGroup::draw(	FboManager*			fbo_manager,
						VboManager*			vbo_manager,
						GlslManager*		glsl_manager,
						CudaPathManager*	cuda_path_manager,
						Camera*				camera,
						float*				viewMat,
						float*				projMat,
						float*				shadowMat,
						bool				wireframe,
						bool				shadows		)
{
	GLuint fbo_target		= fbo_manager->fbos[fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[fbo_pos_tex_name];
	GLuint ids_texture		= fbo_manager->texture_ids[fbo_ids_tex_name];
	static_lod->runAssignmentAndSelection(	fbo_target,
											fbo_texture,
											ids_texture,
											vbo_lod,
											camera		);

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											width,
											vbo_lod,
											cuda_path_manager->texture_buffer_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows		);
}
#endif
#elif defined OSC_PATHS
#ifdef DEMO_SHADER
GLuint CrowdGroup::draw(	FboManager*				fbo_manager,
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
							bool					doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelectionCuda(	fbo_target,
																(float)id,
																vbo_lod,
																camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											osc_cuda_path_manager->cpos_tbo_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
	return not_culled;
}
#endif
//
//=======================================================================================
//
#elif defined MPI_PATHS || defined MPI_PATHS_ON_NODES
#ifdef DEMO_SHADER
GLuint CrowdGroup::draw(	FboManager*				fbo_manager,
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
							bool					doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelectionCuda(	fbo_target,
																(float)id,
																vbo_lod,
																camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											mpi_cuda_path_manager->cpos_tbo_id,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
	return not_culled;
}
#endif
#else
#ifdef DEMO_SHADER
//
//=======================================================================================
//
GLuint CrowdGroup::draw(	FboManager*		fbo_manager,
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
							bool			doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[_fbo_pos_tex_name];
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelection(	fbo_target,
															(float)id,
															vbo_lod,
															camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											fbo_target,
											fbo_texture,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
	return not_culled;
}
//
//=======================================================================================
//
void CrowdGroup::draw(	FboManager*		fbo_manager,
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
						bool			doFacial		)
{
	GLuint fbo_target		= fbo_manager->fbos[fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[fbo_pos_tex_name];
	GLuint ids_texture		= fbo_manager->texture_ids[fbo_ids_tex_name];
	static_lod->runAssignmentAndSelection(	fbo_target,
											fbo_texture,
											ids_texture,
											vbo_lod,
											camera		);

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											width,
											vbo_lod,
											fbo_target,
											fbo_texture,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows,
											doHandD,
											doPatterns,
											doColor,
											doFacial	);
}
#else
//
//=======================================================================================
//
GLuint CrowdGroup::draw(	FboManager*		fbo_manager,
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
							bool			shadows				)
{
	GLuint fbo_target		= fbo_manager->fbos[_fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[_fbo_pos_tex_name];
	GLuint not_culled		= 0;
	not_culled				= _static_lod->runSelection(	fbo_target,
															(float)id,
															vbo_lod,
															camera		);

	//for(unsigned int i = 0; i < 3; i++)
	//{
	//	printf( "Group primitivesWritten=%d\t", vbo_lod[i].primitivesWritten );
	//}
	//printf("\n\n");

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											_AGENTS_NPOT,
											vbo_lod,
											fbo_target,
											fbo_texture,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows		);
	return not_culled;
}
//
//=======================================================================================
//
void CrowdGroup::draw(	FboManager*		fbo_manager,
						VboManager*		vbo_manager,
						GlslManager*	glsl_manager,
						Camera*			camera,
						float*			viewMat,
						float*			projMat,
						float*			shadowMat,
						bool			wireframe,
						bool			shadows		)
{
	GLuint fbo_target		= fbo_manager->fbos[fbo_lod_name].fbo_tex_target;
	GLuint fbo_texture		= fbo_manager->texture_ids[fbo_pos_tex_name];
	GLuint ids_texture		= fbo_manager->texture_ids[fbo_ids_tex_name];
	static_lod->runAssignmentAndSelection(	fbo_target,
											fbo_texture,
											ids_texture,
											vbo_lod,
											camera		);

	cgroup->draw_instanced_culled_rigged(	camera,
											frame,
											width,
											vbo_lod,
											fbo_target,
											fbo_texture,
											0.0f,
											viewMat,
											projMat,
											shadowMat,
											wireframe,
											shadows		);
}
#endif
#endif
//
//=======================================================================================
