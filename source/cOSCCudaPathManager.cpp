
#include "cOSCGlobals.h"
#include "cOSCCudaPathManager.h"

//=======================================================================================
//
//"including" osc_bezier_paths_kernel.cu
extern "C" void launch_osc_bezier_kernel(	float*			npos,
											float*			ppos,
											float*			cpos,
											unsigned int	mesh_width,
											unsigned int	mesh_height,
											float			plane_scale,
											float			time		);

//=======================================================================================
//
cOSCCudaPathManager::cOSCCudaPathManager ( LogManager*		log_manager,
											VboManager*		vbo_manager	)
{
	this->log_manager = log_manager;
	this->vbo_manager = vbo_manager;

	string vbo_name;

	vbo_name = string( "cuda_prev_positions" );
	cuda_ppos_vbo_frame	= 0;
	cuda_ppos_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_ppos_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA prev positions container."	);
	cuda_ppos_vbo_id	= 0;
	cuda_ppos_vbo_size	= 0;
	cuda_ppos_vbo_res	= NULL;

	vbo_name = string( "cuda_curr_positions" );
	cuda_cpos_vbo_frame	= 0;
	cuda_cpos_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_cpos_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA curr positions container." );
	cuda_cpos_vbo_id	= 0;
	cuda_cpos_vbo_size	= 0;
	cuda_cpos_vbo_res	= NULL;

	vbo_name = string( "cuda_next_positions" );
	cuda_npos_vbo_frame	= 0;
	cuda_npos_vbo_index	=
		vbo_manager->createGlCudaVboContainer( vbo_name,
											   cuda_npos_vbo_frame					);
	log_manager->log( LogManager::CUDA, "Created GL-CUDA next positions container." );
	cuda_npos_vbo_id	= 0;
	cuda_npos_vbo_size	= 0;
	cuda_npos_vbo_res	= NULL;

	cpos_tbo_id = 0;

	cudaSetDevice( getMaxGflopsDeviceId() );
	cudaGLSetGLDevice( getMaxGflopsDeviceId() );
}
//
//=======================================================================================
//
cOSCCudaPathManager::~cOSCCudaPathManager()
{
}
//
//=======================================================================================
//
bool cOSCCudaPathManager::init( unsigned int numCharacters )
{
	this->numCharacters = numCharacters;

	for( unsigned int i = 0; i < numCharacters*4; i += 4 )
			{
				Vertex vp;
				INITVERTEX( vp );
				vp.location[0]		= (float)i;
				vp.location[1]		= (float)i;
				vp.location[2]		= 0.0f;
				vp.location[3]		= 1.5f;
				Vertex vn;
				INITVERTEX( vn );
				vn.location[0]		= (float)i;
				vn.location[1]		= (float)i;
				vn.location[2]		= 0.0f;
				vn.location[3]		= 1.5f;
				Vertex vc;
				INITVERTEX( vc );
				vc.location[0]		= (float)i;
				vc.location[1]		= (float)i;
				vc.location[2]		= 0.0f;
				vc.location[3]		= 1.5f;
				vbo_manager->gl_cuda_vbos[cuda_npos_vbo_index][cuda_npos_vbo_frame].vertices.push_back( vn );
				vbo_manager->gl_cuda_vbos[cuda_ppos_vbo_index][cuda_ppos_vbo_frame].vertices.push_back( vp );
				vbo_manager->gl_cuda_vbos[cuda_cpos_vbo_index][cuda_cpos_vbo_frame].vertices.push_back( vc );
			}


	cuda_npos_vbo_size  = vbo_manager->gen_gl_cuda_vbo2(	cuda_npos_vbo_id,
															cuda_npos_vbo_res,
															cuda_npos_vbo_index,
															cuda_npos_vbo_frame	);
	log_manager->log( LogManager::CUDA, "Generated GL-CUDA next positions VBO. Vertices: %u (%uKB).",
									    cuda_npos_vbo_size/4,
									    cuda_npos_vbo_size * sizeof( float ) / 1024					);

	cuda_ppos_vbo_size  = vbo_manager->gen_gl_cuda_vbo2(	cuda_ppos_vbo_id,
															cuda_ppos_vbo_res,
															cuda_ppos_vbo_index,
															cuda_ppos_vbo_frame	);
	log_manager->log( LogManager::CUDA, "Generated GL-CUDA prev positions VBO. Vertices: %u (%uKB).",
									    cuda_ppos_vbo_size/4,
									    cuda_ppos_vbo_size * sizeof( float ) / 1024					);


	cuda_cpos_vbo_size  = vbo_manager->gen_gl_cuda_vbo2(	cuda_cpos_vbo_id,
															cuda_cpos_vbo_res,
															cuda_cpos_vbo_index,
															cuda_cpos_vbo_frame	);
	log_manager->log( LogManager::CUDA, "Generated GL-CUDA curr positions VBO. Vertices: %u (%uKB).",
									    cuda_cpos_vbo_size/4,
									    cuda_cpos_vbo_size * sizeof( float ) / 1024					);


	glGenTextures	( 1, &cpos_tbo_id									);
	glActiveTexture	( GL_TEXTURE7 										);
	glBindTexture	( GL_TEXTURE_BUFFER, cpos_tbo_id 					);
	glTexBuffer		( GL_TEXTURE_BUFFER, GL_RGBA32F, cuda_cpos_vbo_id	);
	glBindTexture	( GL_TEXTURE_BUFFER, 0								);

	return true;

}
//
//=======================================================================================
//
void cOSCCudaPathManager::runCuda(unsigned int crowd_width, unsigned int crowd_height, float parameter)
{
	//printf( "Width=%.3f, Height=%.3f, Param=%.3f\n", scene_width, scene_height, parameter );
	// Map OpenGL buffer object for writing from CUDA:
	float* dptr1;
	float* dptr2;
	float* dptr3;

	struct cudaGraphicsResource* res[3] = {
			vbo_manager->gl_cuda_vbos[cuda_npos_vbo_index][cuda_npos_vbo_frame].cuda_vbo_res,
			vbo_manager->gl_cuda_vbos[cuda_ppos_vbo_index][cuda_ppos_vbo_frame].cuda_vbo_res,
			vbo_manager->gl_cuda_vbos[cuda_cpos_vbo_index][cuda_cpos_vbo_frame].cuda_vbo_res
	};
	cudaGraphicsMapResources (3, res, 0);

	size_t num_bytes1;
	size_t num_bytes2;
	size_t num_bytes3;

	cudaGraphicsResourceGetMappedPointer( (void **)&dptr1, &num_bytes1, res[0] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "cOSCCudaPathManager: CUDA mapped VBO1: May access %ld bytes\n", num_bytes1 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr2, &num_bytes2, res[1] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "cOSCCudaPathManager: CUDA mapped VBO2: May access %ld bytes\n", num_bytes2 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr3, &num_bytes3, res[2] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "cOSCCudaPathManager: CUDA mapped VBO3: May access %ld bytes\n", num_bytes3 );
#endif

//	printf ("crowd_width %d, crowd_height %d \n", crowd_width, crowd_height);

	launch_osc_bezier_kernel( dptr1, dptr2, dptr3, crowd_width, crowd_height, PLANE_SCALE, parameter );

    // Unmap buffer object:
    cudaGraphicsUnmapResources( 3, res, 0 );

}
//
//=======================================================================================
//
void cOSCCudaPathManager::updateOSCPositions( )
{

	if (!unlocked)
	{
		glNamedCopyBufferSubDataEXT	( 	cuda_npos_vbo_id,						//READ_BUFFER
										cuda_ppos_vbo_id,						//WRITE_BUFFER
										0,										//READ_OFFSET
										0,										//WRITE_OFFSET
										sizeof(float)*oscPos.size() 	);
		glNamedBufferSubDataEXT 	( 	cuda_npos_vbo_id,
										0,
										sizeof(float)*oscPos.size(),
										&oscPos[0] 						);
		unlocked = !unlocked;
	}

}
//
//=======================================================================================
//
inline int cOSCCudaPathManager::getMaxGflopsDeviceId( void )
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
	return max_perf_device;
}
//
//=======================================================================================
//
inline int cOSCCudaPathManager::_ConvertSMVer2Cores( int major, int minor )
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
      { 0x30, 192}, // Kepler Generation (SM 3.0) GK10x class
      { 0x35, 192}, // Kepler Generation (SM 3.5) GK11x class
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
//
