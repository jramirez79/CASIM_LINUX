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

#include "cMDPCudaPathManager.h"

//=======================================================================================
//
//"including" mdp_square_paths_kernel.cu
extern "C" void launch_mdp_square_kernel(	float*			current_positions,
											float*			original_positions,
											float*			policy,
											float*			density,
											float			scene_width_in_tiles,
											float			scene_height_in_tiles,
											float			tile_width,
											float			tile_height,
											unsigned int	mesh_width,
											unsigned int	mesh_height,
											float			time,
											bool			policy_reset		);

//"including" mdp_hexagon_paths_kernel.cu
extern "C" void launch_mdp_hexagon_kernel(	float*			current_positions,
											float*			original_positions,
											float*			policy,
											float*			density,
											float			scene_width,
											float			scene_height,
											float			scene_width_in_tiles,
											float			scene_height_in_tiles,
											float			tile_side,
											float			tile_height,
											unsigned int	mesh_width,
											unsigned int	mesh_height,
											float			time,
											bool			policy_reset		);

//=======================================================================================
//
MDPCudaPathManager::MDPCudaPathManager( LogManager*		log_manager,
										VboManager*		vbo_manager,
										FboManager*		fbo_manager,
										string			_fbo_pos_name,
										string			_fbo_pos_tex_name	)
{
	this->log_manager	= log_manager;
	this->vbo_manager	= vbo_manager;
	this->fbo_manager	= fbo_manager;
	proj_manager		= new ProjectionManager();
	fbo_pos_name		= string( _fbo_pos_name );
	fbo_pos_tex_name	= string( _fbo_pos_tex_name );
	string vbo_name;

	vbo_name = string( "cuda_init_positions" );
	cuda_ipos_vbo_frame	= 0;
	cuda_ipos_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_ipos_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA init positions container."	);
	cuda_ipos_vbo_id	= 0;
	cuda_ipos_vbo_size	= 0;
	cuda_ipos_vbo_res	= NULL;

	vbo_name = string( "cuda_curr_positions" );
	cuda_cpos_vbo_frame	= 0;
	cuda_cpos_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_cpos_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA curr positions container." );
	cuda_cpos_vbo_id	= 0;
	cuda_cpos_vbo_size	= 0;
	cuda_cpos_vbo_res	= NULL;

	vbo_name = string( "cuda_policy" );
	cuda_poli_vbo_frame	= 0;
	cuda_poli_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_poli_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA policy container."			);
	cuda_poli_vbo_id	= 0;
	cuda_poli_vbo_size	= 0;
	cuda_poli_vbo_res	= NULL;

	vbo_name = string( "cuda_density" );
	cuda_dens_vbo_frame	= 0;
	cuda_dens_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_dens_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA density container."		);
	cuda_dens_vbo_id	= 0;
	cuda_dens_vbo_size	= 0;
	cuda_dens_vbo_res	= NULL;

	policy_reset		= false;

	texture_buffer_id	= 0;

	//cudaSetDevice( getMaxGflopsDeviceId() );
	//cudaGLSetGLDevice( getMaxGflopsDeviceId() );

	cudaSetDevice( 0 );
	cudaGLSetGLDevice( 0 );
}
//
//=======================================================================================
//
MDPCudaPathManager::~MDPCudaPathManager( void )
{
	render_textures.clear();
	init_positions.clear();
	FREE_INSTANCE( proj_manager );
}
//
//=======================================================================================
//
bool MDPCudaPathManager::init(	vector<float>&	_init_pos,
								vector<float>&	_policy,
								vector<float>&	_density	)
{
	init_positions	= _init_pos;
	policy			= _policy;
	density			= _density;
	return init();
}
//
//=======================================================================================
//
bool MDPCudaPathManager::init( void )
{
	if( init_positions.size() % 4 == 0 )
	{
		for( unsigned int i = 0; i < init_positions.size(); i += 4 )
		{
			Vertex vi;
			INITVERTEX( vi );
			vi.location[0]		= init_positions[i+0];
			vi.location[1]		= init_positions[i+1];
			vi.location[2]		= init_positions[i+2];
			vi.location[3]		= init_positions[i+3];
			Vertex vc;
			INITVERTEX( vc );
			vc.location[0]		= init_positions[i+0];
			vc.location[1]		= -1.0f;
			vc.location[2]		= init_positions[i+2];
			vc.location[3]		= init_positions[i+3];
			vbo_manager->gl_cuda_vbos[cuda_ipos_vbo_index][cuda_ipos_vbo_frame].vertices.push_back( vi );
			vbo_manager->gl_cuda_vbos[cuda_cpos_vbo_index][cuda_cpos_vbo_frame].vertices.push_back( vc );
		}

		cuda_ipos_vbo_size  = vbo_manager->gen_gl_cuda_vbo2(	cuda_ipos_vbo_id,
																cuda_ipos_vbo_res,
																cuda_ipos_vbo_index,
																cuda_ipos_vbo_frame	);
		log_manager->log( LogManager::CUDA, "Generated GL-CUDA init positions VBO. Vertices: %u (%uKB).",
										    cuda_ipos_vbo_size/4,
										    cuda_ipos_vbo_size * sizeof( float ) / 1024					);


		cuda_cpos_vbo_size  = vbo_manager->gen_gl_cuda_vbo2(	cuda_cpos_vbo_id,
																cuda_cpos_vbo_res,
																cuda_cpos_vbo_index,
																cuda_cpos_vbo_frame	);
		log_manager->log( LogManager::CUDA, "Generated GL-CUDA curr positions VBO. Vertices: %u (%uKB).",
										    cuda_cpos_vbo_size/4,
										    cuda_cpos_vbo_size * sizeof( float ) / 1024					);


		cuda_poli_vbo_size  = vbo_manager->gen_gl_cuda_vbo3(	cuda_poli_vbo_id,
																policy,
																cuda_poli_vbo_res,
																cuda_poli_vbo_index,
																cuda_poli_vbo_frame	);
		log_manager->log( LogManager::CUDA, "Generated GL-CUDA policy VBO. Floats: %u (%uKB).",
										    cuda_poli_vbo_size,
										    cuda_poli_vbo_size * sizeof( float ) / 1024					);

		cuda_dens_vbo_size  = vbo_manager->gen_gl_cuda_vbo3(	cuda_dens_vbo_id,
																density,
																cuda_dens_vbo_res,
																cuda_dens_vbo_index,
																cuda_dens_vbo_frame	);
		log_manager->log( LogManager::CUDA, "Generated GL-CUDA density VBO. Floats: %u (%uKB).",
										    cuda_dens_vbo_size,
										    cuda_dens_vbo_size * sizeof( float ) / 1024					);


		// ALSO_ATTACH_A_TEXTURE_BUFFER_OBJECT:
		glGenTextures	( 1, &texture_buffer_id								);
		glActiveTexture ( GL_TEXTURE7										);
		glBindTexture	( GL_TEXTURE_BUFFER, texture_buffer_id				);
		glTexBuffer		( GL_TEXTURE_BUFFER, GL_RGBA32F, cuda_cpos_vbo_id	);
		glBindTexture	( GL_TEXTURE_BUFFER, (GLuint)0						);

		VboRenderTexture texture_buffer;
		texture_buffer.id		= texture_buffer_id;
		texture_buffer.target	= GL_TEXTURE_BUFFER;
		texture_buffer.offset	= GL_TEXTURE7;
		render_textures.push_back( texture_buffer );

		return true;
	}
	else
	{
		log_manager->log( LogManager::CUDA, "ERROR::Wrong CUDA paths inputs size." );
		return false;
	}
}
//
//=======================================================================================
//
void MDPCudaPathManager::updatePolicy( vector<float>& _policy )
{
//->UPDATE_POLICY
	policy			= _policy;
	glBindBuffer					(	GL_ARRAY_BUFFER,
										cuda_poli_vbo_id					);
	glBufferSubData					(	GL_ARRAY_BUFFER,
										0,
										sizeof(float) * policy.size(),
										&policy[0]							);
	glBindBuffer					(	GL_ARRAY_BUFFER,
										0									);
	policy_reset	= true;
//<-UPDATE_POLICY
/*
//->COPY_CURRENT_TO_INITIAL_POSITION_(RESET_POSITIONS)
    glBindBuffer					(	GL_ARRAY_BUFFER,
										cuda_cpos_vbo_id					);
	GLfloat* data;
	data = (GLfloat*)glMapBuffer	(	GL_ARRAY_BUFFER,
										GL_READ_WRITE						);
	if( data != (GLfloat*)NULL )
	{
		for( unsigned int i = 0; i < cuda_cpos_vbo_size; i += 4 )
		{
			data[i+1] = 0.0f;  // Set Y values (prev_tile in Kernel) to 0
		}
		glUnmapBuffer				(	GL_ARRAY_BUFFER						);

		glBindBuffer				(	GL_COPY_WRITE_BUFFER,
										cuda_ipos_vbo_id					);
		glCopyBufferSubData			(	GL_ARRAY_BUFFER,
										GL_COPY_WRITE_BUFFER,
										0,
										0,
										sizeof(float) * cuda_cpos_vbo_size	);
		glBindBuffer				(	GL_COPY_WRITE_BUFFER,
										0									);
		glBindBuffer				(	GL_ARRAY_BUFFER,
										0									);
	}
	else
	{
		glBindBuffer				(	GL_ARRAY_BUFFER,
										0									);
	}
//<-COPY_CURRENT_TO_INITIAL_POSITION_(RESET_POSITIONS)
*/
}
//
//=======================================================================================
//
void MDPCudaPathManager::runCuda(	float			scene_width,
									float			scene_height,
									float			scene_width_in_tiles,
									float			scene_height_in_tiles,
									float			tile_width_or_side,
									float			tile_height,
									unsigned int	texture_width,
									unsigned int	texture_height,
									float			parameter			)
{
    // Map OpenGL buffer object for writing from CUDA:
    float*	dptr1;
	float*	dptr2;
	float*	dptr3;
	float*	dptr4;
	struct cudaGraphicsResource* res[4] = {
		vbo_manager->gl_cuda_vbos[cuda_cpos_vbo_index][cuda_cpos_vbo_frame].cuda_vbo_res,
		vbo_manager->gl_cuda_vbos[cuda_ipos_vbo_index][cuda_ipos_vbo_frame].cuda_vbo_res,
		vbo_manager->gl_cuda_vbos[cuda_poli_vbo_index][cuda_poli_vbo_frame].cuda_vbo_res,
		vbo_manager->gl_cuda_vbos[cuda_dens_vbo_index][cuda_dens_vbo_frame].cuda_vbo_res
	};
	cudaGraphicsMapResources( 4, res , 0 );
    size_t num_bytes1;
	size_t num_bytes2;
	size_t num_bytes3;
	size_t num_bytes4;
    cudaGraphicsResourceGetMappedPointer( (void **)&dptr1, &num_bytes1, res[0] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "CUDA mapped VBO1: May access %ld bytes\n", num_bytes1 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr2, &num_bytes2, res[1] );
#ifdef CASIM_CUDA_PATH_DEBUG
    printf( "CUDA mapped VBO2: May access %ld bytes\n", num_bytes2 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr3, &num_bytes3, res[2] );
#ifdef CASIM_CUDA_PATH_DEBUG
    printf( "CUDA mapped VBO3: May access %ld bytes\n", num_bytes3);
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr4, &num_bytes4, res[3] );
#ifdef CASIM_CUDA_PATH_DEBUG
    printf( "CUDA mapped VBO4: May access %ld bytes\n", num_bytes4 );
#endif

#ifdef MDP_SQUARE
	launch_mdp_square_kernel(	dptr1,
								dptr2,
								dptr3,
								dptr4,
								scene_width_in_tiles,
								scene_height_in_tiles,
								tile_width_or_side,
								tile_height,
								texture_width,
								texture_height,
								parameter,
								policy_reset		);
#elif defined MDP_HEXAGON
	launch_mdp_hexagon_kernel(	dptr1,
								dptr2,
								dptr3,
								dptr4,
								scene_width,
								scene_height,
								scene_width_in_tiles,
								scene_height_in_tiles,
								tile_width_or_side,
								tile_height,
								texture_width,
								texture_height,
								parameter,
								policy_reset		);
#endif
    // Unmap buffer object:
    cudaGraphicsUnmapResources( 4, res, 0 );
	if( policy_reset )
	{
		policy_reset = false;
	}
}
//
//=======================================================================================
//
void MDPCudaPathManager::getDensity( vector<float>& _density )
{
	_density.clear();
    glBindBuffer					(	GL_ARRAY_BUFFER,
										cuda_dens_vbo_id					);
	GLfloat* data;
	data = (GLfloat*)glMapBuffer	(	GL_ARRAY_BUFFER,
										GL_READ_ONLY						);
	if( data != (GLfloat*)NULL )
	{
		for( unsigned int i = 0; i < cuda_dens_vbo_size; i++ )
		{
			_density.push_back( data[i] );
		}
		density = _density;
	}
	glUnmapBuffer					(	GL_ARRAY_BUFFER						);
    glBindBuffer					(	GL_ARRAY_BUFFER,
										0									);
}
//
//=======================================================================================
//
inline int MDPCudaPathManager::getMaxGflopsDeviceId( void )
{
	// This function returns the best GPU (with maximum GFLOPS)
	int current_device   = 0;
	int sm_per_multiproc = 0;
	int max_compute_perf = 0;
	int max_perf_device  = 0;
	int device_count     = 0;
	int best_SM_arch     = 0;
	cudaDeviceProp deviceProp;

	cudaGetDeviceCount( &device_count );
	// Find the best major SM Architecture GPU device
	while( current_device < device_count )
	{
		cudaGetDeviceProperties( &deviceProp, current_device );

        printf("=====================================\n");
        printf("CUDA information:\n");
        printf("CUDA device %d [%s] has: \n", current_device, deviceProp.name);
        printf("\tCompute capability: %d.%d\n", deviceProp.major, deviceProp.minor);
        printf("\t%d Multi-Processors\n", deviceProp.multiProcessorCount);
        printf("\t%.0f MHz (%0.2f GHz) GPU Clock rate.\n", deviceProp.clockRate * 1e-3f, deviceProp.clockRate * 1e-6f);
        printf("\t%d Maximum resident threads per multiprocessor\n", deviceProp.maxThreadsPerMultiProcessor);
        printf("\t%d Bytes of shared memory available per block\n", deviceProp.sharedMemPerBlock);
        printf("\t%d 32-bit registers available per block\n", deviceProp.regsPerBlock);
        printf("\t%d Maximum number of threads per block\n", deviceProp.maxThreadsPerBlock);
        printf("\t%d Threads per warp\n", deviceProp.warpSize);
        printf("=====================================\n");

		if( deviceProp.major > 0 && deviceProp.major < 9999 )
		{
			best_SM_arch = std::max( best_SM_arch, deviceProp.major );
		}
		current_device++;
	}



    // Find the best CUDA capable GPU device
	current_device = 0;
	while( current_device < device_count )
	{
		cudaGetDeviceProperties( &deviceProp, current_device );
		if( deviceProp.major == 9999 && deviceProp.minor == 9999 )
		{
		    sm_per_multiproc = 1;
		}
		else
		{
			sm_per_multiproc = _ConvertSMVer2Cores( deviceProp.major, deviceProp.minor );
		}

		int compute_perf  =
			deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;
		if( compute_perf  > max_compute_perf )
		{
            // If we find GPU with SM major > 2, search only these
			if( best_SM_arch > 2 )
			{
				// If our device==dest_SM_arch, choose this, or else pass
				if( deviceProp.major == best_SM_arch )
				{
					max_compute_perf  = compute_perf;
					max_perf_device   = current_device;
				}
			}
			else
			{
				max_compute_perf  = compute_perf;
				max_perf_device   = current_device;
			}
		}
		++current_device;
	}

	printf("CUDA selected device: %d \n", max_perf_device);
	return max_perf_device;
}
//
//=======================================================================================
//
inline int MDPCudaPathManager::_ConvertSMVer2Cores( int major, int minor )
{
	// Defines for GPU Architecture types
	// (using the SM version to determine the # of cores per SM)
	typedef struct {
		int SM; // 0xMm (hexidecimal notation),
				// M = SM Major version, and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] =
	{ { 0x10,  8 },
	  { 0x11,  8 },
	  { 0x12,  8 },
	  { 0x13,  8 },
	  { 0x20, 32 },
	  { 0x21, 48 },
	  {   -1, -1 }
	};

	int index = 0;
	while( nGpuArchCoresPerSM[index].SM != -1 )
	{
		if( nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) )
		{
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}
	log_manager->log( LogManager::CUDA,
					  "MapSMtoCores undefined SMversion: %d.%d!",
					  major,
					  minor										);
	return -1;
}
//
//=======================================================================================
