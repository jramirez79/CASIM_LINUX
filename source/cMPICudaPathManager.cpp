
#include "cMPICudaPathManager.h"
#ifdef MPI_PATHS
	#include "behaviorMPI.h"
#endif
#ifdef MPI_PATHS_ON_NODES
	#include <mpi.h>
	#include "behaviorMPIpathsOnNodes.h"
#endif


extern float PLANE_SCALE;

//=======================================================================================
//
//"including" osc_bezier_paths_kernel.cu
extern "C" void launch_osc_bezier_kernel		(	float*			npos,
													float*			ppos,
													float*			cpos,
													unsigned int	mesh_width,
													unsigned int	mesh_height,
													float			plane_scale,
													float			time				);

extern "C" void launch_mpi_on_nodes_kernel (		float*			npos,
													float*			ppos,
													float*			cpos,
													float4*			node_agents_pos, // position calculated on slaves nodes (compute_process, behaviorMPI.cpp)
													float4*			node_agents_ids,
													unsigned int 	crowd_width,
													unsigned int 	crowd_height,
													float 			plane_scale,
													int				pid,
													float			time
																						);

//
//=======================================================================================
//
cMPICudaPathManager::cMPICudaPathManager( LogManager*		log_manager,
											VboManager*		vbo_manager ) : cOSCCudaPathManager ( log_manager, vbo_manager )
{

}
//
//=======================================================================================
//
cMPICudaPathManager::~cMPICudaPathManager()
{

}
//
//=======================================================================================
//
#ifdef MPI_PATHS_ON_NODES
void cMPICudaPathManager::runCuda(unsigned int crowd_width, unsigned int crowd_height, float parameter)
{
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
	printf( "cMPICudaPathManager: CUDA mapped VBO1: May access %ld bytes\n", num_bytes1 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr2, &num_bytes2, res[1] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "cMPICudaPathManager: CUDA mapped VBO2: May access %ld bytes\n", num_bytes2 );
#endif
	cudaGraphicsResourceGetMappedPointer( (void **)&dptr3, &num_bytes3, res[2] );
#ifdef CASIM_CUDA_PATH_DEBUG
	printf( "cMPICudaPathManager: CUDA mapped VBO3: May access %ld bytes\n", num_bytes3 );
#endif

//	printf ("crowd_width %d, crowd_height %d \n", crowd_width, crowd_height);




	int pid;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

//	printf ("pid: %d, calling compute_process (NULL); \n", pid);
	compute_process (NULL); // We are using our own cudaGraphicsResource already initialized


//	d_agents_pos & d_agents_ids are calculated in compute_process method from behaviorMPI.cpp :
	launch_mpi_on_nodes_kernel( dptr1, dptr2, dptr3, d_agents_pos, d_agents_ids, crowd_width, crowd_height, PLANE_SCALE, pid, parameter );

//	launch_mpi_on_nodes_kernel_passtru ( d_agents_pos, dptr3, count_agents_total, pid );

    // Unmap buffer object:
    cudaGraphicsUnmapResources( 3, res, 0 );
}
#endif
//
//=======================================================================================
//

using namespace std;

vector<float> pos;

void cMPICudaPathManager::updateMPIPositions ()
{
#ifdef MPI_PATHS

	refreshData();
	pos.clear();
//	printf ("cMPICudaPathManager::updateMPIPositions () \n");
//	printf (" Num characters %d\n", getNumCharacters());

    for( int i = 0; i < getNumCharacters(); i++ )
    {
    	pos.push_back ( h_agents_in_pos[i].x / 2048.0f  * PLANE_SCALE - PLANE_SCALE/2.0f );
    	pos.push_back ( 0.0f );
    	pos.push_back ( h_agents_in_pos[i].y /2048.f * PLANE_SCALE - PLANE_SCALE/2.0f );
    	pos.push_back ( 1.0f );


    }
    		//printf("id: %d [%f,%f,%f,%f]\n",i, h_agents_in_pos[i].x, h_agents_in_pos[i].y, h_agents_in_pos[i].z, h_agents_in_pos[i].w);

// MINOTAURO MAY NOT SUPPORT OPENGL DIRECT STATE ACCESS

//	if (!unlocked_mpi)
	{

		glNamedCopyBufferSubDataEXT	( 	cuda_npos_vbo_id,						//READ_BUFFER
										cuda_ppos_vbo_id,						//WRITE_BUFFER
										0,										//READ_OFFSET
										0,										//WRITE_OFFSET
										sizeof(float)*pos.size() 	);
		glNamedBufferSubDataEXT 	( 	cuda_npos_vbo_id,
										0,
										sizeof(float)*pos.size(),
										&pos[0] 						);
		//unlocked_mpi = !unlocked_mpi;
	}
#endif

#ifdef MPI_PATHS_ON_NODES
	// do stuff
//	printf ("\n\n updateMPIPositions \n\n");
/*
	glNamedCopyBufferSubDataEXT	( 	cuda_npos_vbo_id,						//READ_BUFFER
									cuda_ppos_vbo_id,						//WRITE_BUFFER
									0,										//READ_OFFSET
									0,										//WRITE_OFFSET
									sizeof(float)*4096 	);

*/
#endif

}
