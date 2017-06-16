#ifndef _PATHS_KERNEL_H_
#define _PATHS_KERNEL_H_


#define DEG2RAD	0.01745329251994329576f
#define RAD2DEG 57.29577951308232087679f

//#define HDF5Server


//
//=======================================================================================
//
//
//=======================================================================================
//
__global__ void kernel2(	float*			npos,
							float*			ppos,
							float*			cpos,
							unsigned int	width, 
							unsigned int	height,
							float			plane_scale,
							float			time 		)
{
    unsigned int	x				= blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int	y				= blockIdx.y * blockDim.y + threadIdx.y;
	//unsigned int	index			= y * width + x;
	unsigned int	index			= 4 * ( x + y * blockDim.x * gridDim.x );

#ifdef HDF5Server
	if( npos[index+0] == -plane_scale/2.0f && npos[index+2] == -plane_scale/2.0f )			//GOING TOWARDS 0,0
	{
		cpos[index+0]					= -plane_scale * 10.0f;
		cpos[index+2]					= -plane_scale * 10.0f;
	}
	else if( ppos[index+0] == -plane_scale/2.0f && ppos[index+2] == -plane_scale/2.0f )		//COMING FROM 0,0
	{
		cpos[index+0]					= npos[index+0];
		cpos[index+2]					= npos[index+1];
	}
	else
	{
#endif
		float3	tpos;
		tpos.x					= ppos[index+0] + time*(npos[index+0]-ppos[index+0]);
		tpos.y					= 0.0f;
		tpos.z					= ppos[index+2] + time*(npos[index+2]-ppos[index+2]);
#ifdef HDF5Server
		//if( tpos.x == cpos[index+0] && tpos.z == cpos[index+2] )							//NO MOTION
		//if( ppos[index+0] == npos[index+0] && ppos[index+2] == npos[index+2] )							//NO MOTION
//		{
//			cpos[index+0]				= -plane_scale * 10.0f;
//			cpos[index+2]				= -plane_scale * 10.0f;
//		}
//		else
//		{
#endif
			cpos[index+0]				= tpos.x;
			cpos[index+1]				= tpos.y;
			cpos[index+2]				= tpos.z;
			cpos[index+3]				= atan2( npos[index+0]-ppos[index+0], npos[index+2]-ppos[index+2] );
#ifdef HDF5Server
		}
	}
#endif
}
//
//=======================================================================================
//


__global__ void mpi_on_node_kernel(	float*			npos,
									float*			ppos,
									float*			cpos,
									float4*			node_agents_pos,
									float4*			node_agents_ids,
									float			plane_scale,
									int				pid,
									float			time					)
{
	unsigned int	x				= blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int	y				= blockIdx.y * blockDim.y + threadIdx.y;
	//unsigned int	index			= y * width + x;
	unsigned int	index			= 4 * ( x + y * blockDim.x * gridDim.x );

	ppos[index+0] = npos[index+0];
	ppos[index+1] = npos[index+1];
	ppos[index+2] = npos[index+2];
	ppos[index+3] = npos[index+3];



	if ( node_agents_ids[index].z == pid )
	{
		npos[index+0] = node_agents_pos[index].x / 4096.0f  * plane_scale - plane_scale/2.0f ;
		npos[index+1] = 0.0f;
		npos[index+2] = node_agents_pos[index].y / 4096.0f  * plane_scale - plane_scale/2.0f ;
		npos[index+3] = 0.0f;
	}

	else
	{
		npos[index+0] = -plane_scale * 10.0f;
		npos[index+1] = -plane_scale * 10.0f;
		npos[index+2] = -plane_scale * 10.0f;
		npos[index+3] = 1.0f;
	}

	float3	tpos;
	tpos.x					= ppos[index+0] + time*(npos[index+0]-ppos[index+0]);
	tpos.y					= 0.0f;
	tpos.z					= ppos[index+2] + time*(npos[index+2]-ppos[index+2]);

	cpos[index+0]			= tpos.x;
	cpos[index+1]			= tpos.y;
	cpos[index+2]			= tpos.z;
	cpos[index+3]			= atan2( npos[index+0]-ppos[index+0], npos[index+2]-ppos[index+2] );



}
//
//=======================================================================================
//
/*
__global__ void mpi_on_node_kernel_passthru (	float4*	d_agent,
												float*	cpos,
												int		agent_width,
												int		count_agents_total,
												int		)
												*/
//
//=======================================================================================
//
extern "C" void launch_osc_bezier_kernel (	float*			npos,
											float*			ppos,
											float*			cpos,
											unsigned int	mesh_width,
											unsigned int	mesh_height,
											float			plane_scale,
											float			time		)
{
    dim3 block( 4, 4, 1 );
    dim3 grid( mesh_width / block.x, mesh_height / block.y, 1 );
    kernel2<<<grid, block>>>( npos, ppos, cpos, mesh_width, mesh_height, plane_scale, time );
}
//
//=======================================================================================
//
extern "C" void launch_mpi_on_nodes_kernel (	float*			npos,
												float*			ppos,
												float*			cpos,
												float4*			node_agents_pos, // position calculated on slaves nodes (compute_process, behaviorMPI.cpp)
												float4*			node_agents_ids,
												unsigned int 	crowd_width,
												unsigned int 	crowd_height,
												float 			plane_scale,
												int				pid,
												float			time		)
{
    dim3 block( 4, 4, 1 );
    dim3 grid( crowd_width / block.x, crowd_height / block.y, 1 );
    mpi_on_node_kernel<<<grid, block>>>( npos, ppos, cpos, node_agents_pos, node_agents_ids, plane_scale, pid, time );
}
//
//=======================================================================================
//
/*
extern "C" void launch_mpi_on_nodes_kernel_passthru( float4* 		d_agents,
													float*  		cpos,
													int				count_agents_total,
													int 			pid 				)
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

    mpi_on_node_kernel_passthru<<< grid, block>>>(d_agents_in, cpos, agent_width, count_agents_total, pid);


}
*/
//
//=======================================================================================
//

#endif
