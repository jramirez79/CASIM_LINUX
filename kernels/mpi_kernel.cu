
#ifndef __MPI_BEHAVIOR_
#define __MPI_BEHAVIOR_

#include <stdio.h>
#include "cMacros.h"


#ifdef MPI_PATHS

__device__ int d_behavior(unsigned int x, unsigned int y, float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int agent_width)
{
	//HPV movimiento de los agentes
	// 1ra opcion seguir en la misma direccion, se revisa el mundo para ver si esta disponible las sig coordenadas
	// en caso contrario se mueve 45 grados en sentido antihorario

	float PI = 3.14159265358979323846;
	int rebote = 0;
	int limit_width = world_width/40;
	int limit_height = world_height/40;

	//Verificando si estas cerca de la orilla en la siguiente coordenada
	//next position = actual position + (cos(teta)*velocity)
	int ccx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * 2 * agent[y*agent_width+x].w) );
	int ccy = round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) * 2 * agent[y*agent_width+x].w) );
	if( ccx < limit_width || ccx > world_width - limit_width || ccy < limit_height || ccy > world_height - limit_height )
	{
		//si la siguiente coordenada sale del mundo entonces el angulo cambia 90 grados "rebote"
		agent[y*agent_width+x].z += PI/2;
		rebote = 1;
	}

	//calculando las coordenadas originales
	int cx_old = round( agent[y*agent_width+x].x ) ;
	int cy_old = ( round( agent[y*agent_width+x].y ) - 1 ) * world_width ;
	if ( cy_old < 0 )
		cy_old = 0 ;
	int coord_old =  cy_old + cx_old ;

	//Aqui revisas que la nueva posicion no este ocupada, si se trata de un rebote haces una excepcion y permites la colision
	// si esta ocupada la sig posicion te vas moviendo 45 grados en sentido antihorario
	// se utiliza world_width porque se hace la conversion a un arreglo dimensional de uno bidimensional
	int cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) );
	int cy = (  round( agent[y*agent_width+x].y +  (sin(agent[y*agent_width+x].z)  * agent[y*agent_width+x].w) ) - 1  )* world_width ;
	int coord =  cx + cy ;
	if( d_world[coord] == 0 || rebote )
	{
		//cambias el valor a las siguientes posiciones
		agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ;
		agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ;
		//pones el id del agente en la casilla del mundo que esta ocupando
		d_world[coord] = ids[y*agent_width+x].x ;
		//liberas la casilla ocupada anteriormente
		d_world[coord_old] = 0;
	}
	else{
		cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI/4)  *  agent[y*agent_width+x].w ) ) ;
		cy = (  round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI/4)  * agent[y*agent_width+x].w) ) -1  ) * world_width ;
		coord =  cy + cx ;
		if( d_world[coord] == 0)
		{
			agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI/4) *agent[y*agent_width+x].w) ;
			agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI/4) *agent[y*agent_width+x].w) ;
			d_world[coord] = ids[y*agent_width+x].x ;
			d_world[coord_old] = 0;
		}
		else{
			cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 2*PI/4)  * agent[y*agent_width+x].w) ) ;
			cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 2*PI/4)  * agent[y*agent_width+x].w) ) - 1) * world_width;
			coord =  cy + cx ;
			if( d_world[coord] == 0)
			{
				agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 2*PI/4) *agent[y*agent_width+x].w) ;
				agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 2*PI/4) *agent[y*agent_width+x].w) ;
				d_world[coord] = ids[y*agent_width+x].x ;
				d_world[coord_old] = 0;
			}
			else{
				cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 3*PI/4)  * agent[y*agent_width+x].w) ) ;
				cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 3*PI/4) * agent[y*agent_width+x].w) ) -1 ) * world_width ;
				coord =  cy + cx ;
				if( d_world[coord] == 0)
				{
					agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 3*PI/4) *agent[y*agent_width+x].w) ;
					agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 3*PI/4) *agent[y*agent_width+x].w) ;
					d_world[coord] = ids[y*agent_width+x].x ;
					d_world[coord_old] = 0;
				}
				else{
					cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI)  * agent[y*agent_width+x].w) ) ;
					cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI) * agent[y*agent_width+x].w) ) -1 ) * world_width;
					coord =  cy + cx ;
					if( d_world[coord] == 0)
					{
						agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI) *agent[y*agent_width+x].w) ;
						agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI) *agent[y*agent_width+x].w) ;
						d_world[coord] = ids[y*agent_width+x].x ;
						d_world[coord_old] = 0;
					}
					else{
						cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 5*PI/4)  * agent[y*agent_width+x].w) ) ;
						cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 5*PI/4)  * agent[y*agent_width+x].w) ) - 1 ) * world_width;
						coord =  cy + cx ;
						if( d_world[coord] == 0)
						{
							agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 5*PI/4) *agent[y*agent_width+x].w) ;
							agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 5*PI/4) *agent[y*agent_width+x].w) ;
							d_world[coord] = ids[y*agent_width+x].x ;
							d_world[coord_old] = 0;
						}
						else{
							cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 6*PI/4) * agent[y*agent_width+x].w) ) ;
							cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 6*PI/4) * agent[y*agent_width+x].w) ) -1 ) * world_width;
							coord =  cy + cx ;
							if( d_world[coord] == 0)
							{
								agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 6*PI/4) *agent[y*agent_width+x].w) ;
								agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 6*PI/4) *agent[y*agent_width+x].w) ;
								d_world[coord] = ids[y*agent_width+x].x ;
								d_world[coord_old] = 0;
							}
							else{
								cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 7*PI/4) * agent[y*agent_width+x].w) ) ;
								cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 7*PI/4) * agent[y*agent_width+x].w) ) - 1 ) * world_width;
								coord =  cy + cx ;
								if( d_world[coord] == 0)
								{
									agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 7*PI/4) *agent[y*agent_width+x].w) ;
									agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 7*PI/4) *agent[y*agent_width+x].w) ;
									d_world[coord] = ids[y*agent_width+x].x ;
									d_world[coord_old] = 0;
								}
								else{
									//si todas las posiciones a su alrededor estan ocupadas se queda donde esta y marcas
									//ocupada de nuevo esa posicion
									//d_world[coord_old] = 1;
	//si todas las posiciones a su alrededor estan ocupadas avanzas en la direccion original aunque se colisione
	cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ) ;
	cy = ( round( agent[y*agent_width+x].y +  ( sin(agent[y*agent_width+x].z)  * agent[y*agent_width+x].w) ) - 1 )* world_width ;
	coord =  cx + cy ;

	agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) *agent[y*agent_width+x].w)*32.0 ;
	agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) *agent[y*agent_width+x].w)*32.0 ;
	d_world[coord] = ids[y*agent_width+x].x ;
	d_world[coord_old] = 0;
								}
							} //7*PI/4
						} //6*PI/4
					}//5*PI/4
				}//PI
			}//3*PI/4
		 }//PI/2
	}//PI/4

	return coord;

}

__device__ void d_interchange(unsigned int x, unsigned int y, float4 *agent, float4 *ids, int *d_world, int agent_width, int world_height_node,
		int world_width_node, int num_comp_nodes, int coord)
{
   int lookup_node4[2][2];
   lookup_node4[0][0] = 1;
   lookup_node4[0][1] = 2;
   lookup_node4[1][0] = 3;
   lookup_node4[1][1] = 4;

   int lookup_node16[4][4];
   lookup_node16[0][0] = 1;
   lookup_node16[0][1] = 2;
   lookup_node16[0][2] = 3;
   lookup_node16[0][3] = 4;
   lookup_node16[1][0] = 5;
   lookup_node16[1][1] = 6;
   lookup_node16[1][2] = 7;
   lookup_node16[1][3] = 8;
   lookup_node16[2][0] = 9;
   lookup_node16[2][1] = 10;
   lookup_node16[2][2] = 11;
   lookup_node16[2][3] = 12;
   lookup_node16[3][0] = 13;
   lookup_node16[3][1] = 14;
   lookup_node16[3][2] = 15;
   lookup_node16[3][3] = 16;

   int lookup_node9[3][3];
   lookup_node9[0][0] = 1;
   lookup_node9[0][1] = 2;
   lookup_node9[0][2] = 3;
   lookup_node9[1][0] = 4;
   lookup_node9[1][1] = 5;
   lookup_node9[1][2] = 6;
   lookup_node9[2][0] = 7;
   lookup_node9[2][1] = 8;
   lookup_node9[2][2] = 9;

	if (num_comp_nodes == 4)
	{
		ids[y*agent_width+x].z = lookup_node4[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}
	else if (num_comp_nodes == 16)
	{
		ids[y*agent_width+x].z = lookup_node16[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}
	else if (num_comp_nodes == 9)
	{
		ids[y*agent_width+x].z = lookup_node9[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}

}

__device__ bool d_border(int x, int y, float4 *agent, int world_width, int agent_width, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes)
{
	int border_width = 5;
	int nodes_per_row = sqrtf(num_comp_nodes);
	int x_limit_right = (( ( (pid - 1) % nodes_per_row ) + 1 ) * world_width_node);
	int x_limit_left = (( (pid - 1) % nodes_per_row )  * world_width_node);
	int num_row = (pid - 1) / nodes_per_row;
	int y_limit_up = (( num_row + 1 ) * world_height_node);
	int y_limit_down = (num_row  * world_height_node);

	//getting coords
	int cx = round( agent[y*agent_width+x].x ) ;
	int cy = round( agent[y*agent_width+x].y ) ;


	if( cx < x_limit_left + border_width )
	{
		return true;
	}

	if( cx > x_limit_right - border_width )
	{
		return true;
	}

	if( cy < y_limit_down + border_width )
	{
		return true;
	}

	if( cy > y_limit_up - border_width )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
//! Kernel to modify internal agents positions
//! @param data  data in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel_internal(float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, int grid_width)
{
   unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
   unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
   int coord;
   int agent_width = grid_width;

   if ( y * agent_width + x < count_agents_total ) //verify bounds
   {
	   if(ids[y*agent_width+x].z == pid) //check if agent is active
	   {
		   if(!d_border(x, y, agent, world_width, agent_width, world_height_node, world_width_node, pid, num_comp_nodes)) //check is not in the border
		   {
			   ids[y*agent_width+x].w += 1; //iterations counter

			   coord = d_behavior(x, y, agent, ids, d_world, world_width, world_height, agent_width);
			   d_interchange(x, y, agent, ids, d_world, agent_width, world_height_node, world_width_node, num_comp_nodes, coord);
		   }
	   }
	   else
		   ids[y*agent_width+x].z = -1;
   }
}

///////////////////////////////////////////////////////////////////////////////
//! Kernel to modify border agents positions
//! @param data  data in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel_border(float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, float4 *agent_fr, float4 *ids_fr, int grid_width)
{
   unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
   unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
   int agent_width = grid_width;
   int coord;


   if ( y * agent_width + x < count_agents_total ) //verify bounds of the array
   {
	   if(ids[y*agent_width+x].z == pid) //check if agent is active
	   {
		   if(d_border(x, y, agent, world_width, agent_width, world_height_node, world_width_node, pid, num_comp_nodes)) //check is in the border
		   {

			   ids[y*agent_width+x].w += 1; //iterations counter

			   coord = d_behavior(x, y, agent, ids, d_world, world_width, world_height, agent_width);
			   d_interchange(x, y, agent, ids, d_world, agent_width, world_height_node, world_width_node, num_comp_nodes, coord);
			   agent_fr[y*agent_width+x] = agent[y*agent_width+x];
			   ids_fr[y*agent_width+x] = ids[y*agent_width+x];
		   }
	   }
	   else
		   ids[y*agent_width+x].z = -1;
   }
}

// CUDA computation on each node
// No MPI here, only CUDA
extern "C" void launch_kernel_border(float4 *d_agents_in, float4 *d_agents_ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, float4 *d_agents_in_fr, float4 *d_agents_ids_fr,	cudaStream_t stream0)
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

    kernel_border<<< grid, block, 0, stream0>>>(d_agents_in, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
    		pid, num_comp_nodes, count_agents_total, d_agents_in_fr, d_agents_ids_fr, agent_width);
}

extern "C" void launch_kernel_internal(float4 *d_agents_in, float4 *d_agents_ids, int *d_world, int world_width, int world_height,
		int world_height_node, int world_width_node, int pid, int num_comp_nodes, int count_agents_total, cudaStream_t stream1 )
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

//printf ("\n\n BEGIN: launch_kernel_internal() \n");

    kernel_internal<<< grid, block>>>(d_agents_in, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
    		pid, num_comp_nodes, count_agents_total, agent_width);

//printf ("\n\n END: launch_kernel_internal() \n");
}


#endif
#endif

#ifdef MPI_PATHS_ON_NODES

#define KERNEL

#ifdef KERNEL
//Kernel 2
__device__ int d_behavior(unsigned int x, unsigned int y, float4 *d_agents, float4 *d_ids, int *d_world, int world_width, int world_height, int agent_width)
{
	float PI = 3.14159265358979323846;
	int limit_width = world_width/40;
	int limit_height = world_height/40;
	int radio = 5;
	int number_directions = 8;
    //int points[number_directions]
	int points[8];
	//int nl[number_directions*radio]
	int nl[ 40 ];
	int i, j, direction, k=0;
	int coord;
	float cx, cy, ai;
	float cx_new, cy_new;
	int coord_new, p=0, mx;

    float angle = d_agents[y*agent_width+x].z;
    int ax = d_agents[y*agent_width+x].x;
    int ay = d_agents[y*agent_width+x].y;
    int aw = d_agents[y*agent_width+x].w;
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);



    //if next cell is out of the world the angle change 90 degrees
	int ccx = ax + ( cos_angle * 2 * aw) ;
	int ccy = ay + ( sin_angle * 2 * aw) ;
	if( (ccx < limit_width || ccx > world_width - limit_width) || (ccy < limit_height || ccy > world_height - limit_height) )
	{
		angle += PI/2;
		//d_agents[y*agent_width+x].z *= -1;
	}

	//get the original coord to make available
	float cx_old = ax;
	float cy_old = ( ay - 1 ) * world_width ;
	if ( cy_old < 0 )
		cy_old = 0 ;
	int coord_old =  cy_old + cx_old;


	//Listing cells in the neighborhood (raytrace over a radio)
	ai = angle;
	#pragma unroll
	for(k=0; k < radio*number_directions;  )
	{
		ai += PI/4;
		for(j = 1; j <= radio; j++)
		{
			cx = cx_old + ( cosf(ai) * j);
			cy = cy_old + ( sinf(ai) * j);
			coord = round(cx + cy);
			nl[k] = d_world[coord];
			k++;
		}
	}


	//Evaluating the path
	for(direction = 0; direction < number_directions; direction++)
	{
		p = 0;
		for(i=0; i < radio; i++)
		{
			if(nl[i + (direction*radio)] == 0)
				p++;
			else if( round(d_agents[ nl[i + (direction*radio)] - 1 ].z) == round(angle) )
				p++;
			else
				break;
		}
		points[direction] = p;
	}

	//Choosing the best path
	mx = points[0];
	direction = 0;
	for(i=1;i<number_directions;i++)
	{
		if(mx < points[i])
		{
			mx = points[i];
			direction = i;
		}
	}

	//Updating values
	angle += direction * PI/4;
	//Asign new coords
	d_agents[y*agent_width+x].x = round( ax + ( cosf(angle) * aw ) ) ;
	d_agents[y*agent_width+x].y = round( ay + ( sinf(angle) * aw ) ) ;
	d_agents[y*agent_width+x].z = angle;
	cx_new = d_agents[y*agent_width+x].x;
	cy_new = ( d_agents[y*agent_width+x].y - 1 ) * world_width ;
	if ( cy_new < 0 )
		cy_new = 0 ;
	coord_new =  cx_new + cy_new ;
	//set as busy cell

	d_world[coord_new] = d_ids[y*agent_width+x].x ;
	//release old cell
	d_world[coord_old] = 0;

	return coord_new;
}
#else


__device__ int d_behavior(unsigned int x, unsigned int y, float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int agent_width)
{
	//HPV movimiento de los agentes
	// 1ra opcion seguir en la misma direccion, se revisa el mundo para ver si esta disponible las sig coordenadas
	// en caso contrario se mueve 45 grados en sentido antihorario

	float PI = 3.14159265358979323846;
	int rebote = 0;
	int limit_width = world_width/40;
	int limit_height = world_height/40;

	//Verificando si estas cerca de la orilla en la siguiente coordenada
	//next position = actual position + (cos(teta)*velocity)
	int ccx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * 2 * agent[y*agent_width+x].w) );
	int ccy = round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) * 2 * agent[y*agent_width+x].w) );
	if( ccx < limit_width || ccx > world_width - limit_width || ccy < limit_height || ccy > world_height - limit_height )
	{
		//si la siguiente coordenada sale del mundo entonces el angulo cambia 90 grados "rebote"
		agent[y*agent_width+x].z += PI/2;
		rebote = 1;
	}

	//calculando las coordenadas originales
	int cx_old = round( agent[y*agent_width+x].x ) ;
	int cy_old = ( round( agent[y*agent_width+x].y ) - 1 ) * world_width ;
	if ( cy_old < 0 )
		cy_old = 0 ;
	int coord_old =  cy_old + cx_old ;

	//Aqui revisas que la nueva posicion no este ocupada, si se trata de un rebote haces una excepcion y permites la colision
	// si esta ocupada la sig posicion te vas moviendo 45 grados en sentido antihorario
	// se utiliza world_width porque se hace la conversion a un arreglo dimensional de uno bidimensional
	int cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) );
	int cy = (  round( agent[y*agent_width+x].y +  (sin(agent[y*agent_width+x].z)  * agent[y*agent_width+x].w) ) - 1  )* world_width ;
	int coord =  cx + cy ;
	if( d_world[coord] == 0 || rebote )
	{
		//cambias el valor a las siguientes posiciones
		agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ;
		agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ;
		//pones el id del agente en la casilla del mundo que esta ocupando
		d_world[coord] = ids[y*agent_width+x].x ;
		//liberas la casilla ocupada anteriormente
		d_world[coord_old] = 0;
	}
	else{
		cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI/4)  *  agent[y*agent_width+x].w ) ) ;
		cy = (  round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI/4)  * agent[y*agent_width+x].w) ) -1  ) * world_width ;
		coord =  cy + cx ;
		if( d_world[coord] == 0)
		{
			agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI/4) *agent[y*agent_width+x].w) ;
			agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI/4) *agent[y*agent_width+x].w) ;
			d_world[coord] = ids[y*agent_width+x].x ;
			d_world[coord_old] = 0;
		}
		else{
			cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 2*PI/4)  * agent[y*agent_width+x].w) ) ;
			cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 2*PI/4)  * agent[y*agent_width+x].w) ) - 1) * world_width;
			coord =  cy + cx ;
			if( d_world[coord] == 0)
			{
				agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 2*PI/4) *agent[y*agent_width+x].w) ;
				agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 2*PI/4) *agent[y*agent_width+x].w) ;
				d_world[coord] = ids[y*agent_width+x].x ;
				d_world[coord_old] = 0;
			}
			else{
				cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 3*PI/4)  * agent[y*agent_width+x].w) ) ;
				cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 3*PI/4) * agent[y*agent_width+x].w) ) -1 ) * world_width ;
				coord =  cy + cx ;
				if( d_world[coord] == 0)
				{
					agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 3*PI/4) *agent[y*agent_width+x].w) ;
					agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 3*PI/4) *agent[y*agent_width+x].w) ;
					d_world[coord] = ids[y*agent_width+x].x ;
					d_world[coord_old] = 0;
				}
				else{
					cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI)  * agent[y*agent_width+x].w) ) ;
					cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI) * agent[y*agent_width+x].w) ) -1 ) * world_width;
					coord =  cy + cx ;
					if( d_world[coord] == 0)
					{
						agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + PI) *agent[y*agent_width+x].w) ;
						agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + PI) *agent[y*agent_width+x].w) ;
						d_world[coord] = ids[y*agent_width+x].x ;
						d_world[coord_old] = 0;
					}
					else{
						cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 5*PI/4)  * agent[y*agent_width+x].w) ) ;
						cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 5*PI/4)  * agent[y*agent_width+x].w) ) - 1 ) * world_width;
						coord =  cy + cx ;
						if( d_world[coord] == 0)
						{
							agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 5*PI/4) *agent[y*agent_width+x].w) ;
							agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 5*PI/4) *agent[y*agent_width+x].w) ;
							d_world[coord] = ids[y*agent_width+x].x ;
							d_world[coord_old] = 0;
						}
						else{
							cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 6*PI/4) * agent[y*agent_width+x].w) ) ;
							cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 6*PI/4) * agent[y*agent_width+x].w) ) -1 ) * world_width;
							coord =  cy + cx ;
							if( d_world[coord] == 0)
							{
								agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 6*PI/4) *agent[y*agent_width+x].w) ;
								agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 6*PI/4) *agent[y*agent_width+x].w) ;
								d_world[coord] = ids[y*agent_width+x].x ;
								d_world[coord_old] = 0;
							}
							else{
								cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 7*PI/4) * agent[y*agent_width+x].w) ) ;
								cy = ( round( agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 7*PI/4) * agent[y*agent_width+x].w) ) - 1 ) * world_width;
								coord =  cy + cx ;
								if( d_world[coord] == 0)
								{
									agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z + 7*PI/4) *agent[y*agent_width+x].w) ;
									agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z + 7*PI/4) *agent[y*agent_width+x].w) ;
									d_world[coord] = ids[y*agent_width+x].x ;
									d_world[coord_old] = 0;
								}
								else{
									//si todas las posiciones a su alrededor estan ocupadas se queda donde esta y marcas
									//ocupada de nuevo esa posicion
									//d_world[coord_old] = 1;
	//si todas las posiciones a su alrededor estan ocupadas avanzas en la direccion original aunque se colisione
	cx = round( agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) * agent[y*agent_width+x].w) ) ;
	cy = ( round( agent[y*agent_width+x].y +  ( sin(agent[y*agent_width+x].z)  * agent[y*agent_width+x].w) ) - 1 )* world_width ;
	coord =  cx + cy ;

	agent[y*agent_width+x].x = agent[y*agent_width+x].x + ( cos(agent[y*agent_width+x].z) *agent[y*agent_width+x].w) ;
	agent[y*agent_width+x].y = agent[y*agent_width+x].y + ( sin(agent[y*agent_width+x].z) *agent[y*agent_width+x].w) ;
	d_world[coord] = ids[y*agent_width+x].x ;
	d_world[coord_old] = 0;
								}
							} //7*PI/4
						} //6*PI/4
					}//5*PI/4
				}//PI
			}//3*PI/4
		 }//PI/2
	}//PI/4

	return coord;

}

#endif

__device__ void d_interchange(unsigned int x, unsigned int y, float4 *agent, float4 *ids, int *d_world, int agent_width, int world_height_node,
		int world_width_node, int num_comp_nodes, int coord)
{
   int lookup_node4[2][2];
   lookup_node4[0][0] = 1;
   lookup_node4[0][1] = 2;
   lookup_node4[1][0] = 3;
   lookup_node4[1][1] = 4;

   int lookup_node16[4][4];
   lookup_node16[0][0] = 1;
   lookup_node16[0][1] = 2;
   lookup_node16[0][2] = 3;
   lookup_node16[0][3] = 4;
   lookup_node16[1][0] = 5;
   lookup_node16[1][1] = 6;
   lookup_node16[1][2] = 7;
   lookup_node16[1][3] = 8;
   lookup_node16[2][0] = 9;
   lookup_node16[2][1] = 10;
   lookup_node16[2][2] = 11;
   lookup_node16[2][3] = 12;
   lookup_node16[3][0] = 13;
   lookup_node16[3][1] = 14;
   lookup_node16[3][2] = 15;
   lookup_node16[3][3] = 16;

   int lookup_node9[3][3];
   lookup_node9[0][0] = 1;
   lookup_node9[0][1] = 2;
   lookup_node9[0][2] = 3;
   lookup_node9[1][0] = 4;
   lookup_node9[1][1] = 5;
   lookup_node9[1][2] = 6;
   lookup_node9[2][0] = 7;
   lookup_node9[2][1] = 8;
   lookup_node9[2][2] = 9;

	if (num_comp_nodes == 4)
	{
		ids[y*agent_width+x].z = lookup_node4[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}
	else if (num_comp_nodes == 16)
	{
		ids[y*agent_width+x].z = lookup_node16[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}
	else if (num_comp_nodes == 9)
	{
		ids[y*agent_width+x].z = lookup_node9[(int)agent[y*agent_width+x].y/world_height_node][(int)agent[y*agent_width+x].x/world_width_node];
		d_world[coord] = 0 ;
	}

}

__device__ bool d_border(int x, int y, float4 *agent, int world_width, int agent_width, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes)
{
	int border_width = 5;
	int nodes_per_row = sqrtf(num_comp_nodes);
	int x_limit_right = (( ( (pid - 1) % nodes_per_row ) + 1 ) * world_width_node);
	int x_limit_left = (( (pid - 1) % nodes_per_row )  * world_width_node);
	int num_row = (pid - 1) / nodes_per_row;
	int y_limit_up = (( num_row + 1 ) * world_height_node);
	int y_limit_down = (num_row  * world_height_node);

	//getting coords
	int cx = round( agent[y*agent_width+x].x ) ;
	int cy = round( agent[y*agent_width+x].y ) ;


	if( cx < x_limit_left + border_width )
	{
		return true;
	}

	if( cx > x_limit_right - border_width )
	{
		return true;
	}

	if( cy < y_limit_down + border_width )
	{
		return true;
	}

	if( cy > y_limit_up - border_width )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
//! Kernel to modify internal agents positions
//! @param data  data in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel_internal(float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, int grid_width)
{
   unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
   unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
   int coord;
   int agent_width = grid_width;



   if ( y * agent_width + x < count_agents_total ) //verify bounds
   {
	   if(ids[y*agent_width+x].z == pid) //check if agent is active
	   {
		   if(!d_border(x, y, agent, world_width, agent_width, world_height_node, world_width_node, pid, num_comp_nodes)) //check is not in the border
		   {
			   coord = d_behavior(x, y, agent, ids, d_world, world_width, world_height, agent_width);
			   //d_interchange(x, y, agent, ids, d_world, agent_width, world_height_node, world_width_node, num_comp_nodes, coord);
		   }
	   }
	   else
		   ids[y*agent_width+x].z = -1;
   }
}

///////////////////////////////////////////////////////////////////////////////
//! Kernel to modify border agents positions
//! @param data  data in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel_border(float4 *agent, float4 *ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, float4 *agent_fr, float4 *ids_fr, int grid_width)
{
   unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
   unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
   int agent_width = grid_width;
   int coord;
   /*
   if ( x == 0 )
 	   printf("kernel_border \n");
*/
   if ( y * agent_width + x < count_agents_total ) //verify bounds of the array
   {
	   if(ids[y*agent_width+x].z == pid) //check if agent is active
	   {
		   ids[y*agent_width+x].w += 1; //iterations counter

		   if(d_border(x, y, agent, world_width, agent_width, world_height_node, world_width_node, pid, num_comp_nodes)) //check is in the border
		   {
			   coord = d_behavior(x, y, agent, ids, d_world, world_width, world_height, agent_width);
			   d_interchange(x, y, agent, ids, d_world, agent_width, world_height_node, world_width_node, num_comp_nodes, coord);
			   agent_fr[y*agent_width+x] = agent[y*agent_width+x];
			   ids_fr[y*agent_width+x] = ids[y*agent_width+x];
		   }
	   }
	   else
		   ids[y*agent_width+x].z = -1;
   }
}

///////////////////////////////////////////////////////////////////////////////
//! Kernel to load positions to draw throught OpenGL
//! @param data  data in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel_draw(float4 *d_agent, float4 *d_tmp, float4 *ids, int agent_width, int count_agents_total, int pid)
{
	unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	if ( y * agent_width + x < count_agents_total)  //verify bounds of the array
	{
		if (ids[y*agent_width+x].z == pid)
		{
		d_tmp[y*agent_width+x].x = d_agent[y*agent_width+x].x;
		d_tmp[y*agent_width+x].y = d_agent[y*agent_width+x].y;
		d_tmp[y*agent_width+x].z = 0;
		d_tmp[y*agent_width+x].w = 1;
		}
		else
		{
		d_tmp[y*agent_width+x].x = -1;
		d_tmp[y*agent_width+x].y = -1;
		d_tmp[y*agent_width+x].z = 0;
		d_tmp[y*agent_width+x].w = 1;
		}

	}
}

// CUDA computation on each node
// No MPI here, only CUDA
extern "C" void launch_kernel_border(float4 *d_agents_in, float4 *d_agents_ids, int *d_world, int world_width, int world_height, int world_height_node,
		int world_width_node, int pid, int num_comp_nodes, int count_agents_total, float4 *d_agents_in_fr, float4 *d_agents_ids_fr,	cudaStream_t stream0)
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

    kernel_border<<< grid, block>>>(d_agents_in, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
    		pid, num_comp_nodes, count_agents_total, d_agents_in_fr, d_agents_ids_fr, agent_width);
}

extern "C" void launch_kernel_internal(float4 *d_agents_in, float4 *d_agents_ids, int *d_world, int world_width, int world_height,
		int world_height_node, int world_width_node, int pid, int num_comp_nodes, int count_agents_total)
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

    kernel_internal<<< grid, block>>>(d_agents_in, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
    		pid, num_comp_nodes, count_agents_total, agent_width);
}

extern "C" void launch_kernel_draw(float4 *d_agents_in, float4 *d_tmp, float4 *d_ids, int count_agents_total, int pid)
{
    // execute the kernel
    int block_width = 8;
    int block_height = 8;
    dim3 block(block_width, block_height, 1);
    int agent_width = sqrt(count_agents_total);
    dim3 grid((agent_width / block.x) + 1, (agent_width / block.y) + 1, 1);

    kernel_draw<<< grid, block>>>(d_agents_in, d_tmp, d_ids, agent_width, count_agents_total, pid);
}

#endif




