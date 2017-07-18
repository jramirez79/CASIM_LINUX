

//#define MINOTAURO
//#define DEBUG
//Number of iteration to measure time
//unsigned int iterations = 1000;


////////////////////////////////////////////////////////////////////////////////
//!	Global Variables
////////////////////////////////////////////////////////////////////////////////
#define MAX_GPU_COUNT	2
#define REFRESH_DELAY     3 //ms
#define DATA_COLLECT 3
#define MAX(a,b) ((a > b) ? a : b)


#define agent_width 64
#define agent_height 64
#define world_width 4096
#define world_height 4096
/*
#define agent_width 32
#define agent_height 32
#define world_width 1024
#define world_height 1024
*/
//#define world_width 3072
//#define world_height 3072
#define agents_total   agent_width * agent_height

#ifdef MINOTAURO
	// includes, cuda
	#include <cuda_runtime_api.h>

	// CUDA utilities and system includes
	#include <sdkHelper.h>    // includes cuda.h and cuda_runtime_api.h
	#include <shrQATest.h>    // standard utility and system includes
#else
	//LOCAL
	// includes, cuda
	#include <cuda_runtime.h>

	// Utilities and timing functions
	//#include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h

	// CUDA helper functions
	//#include <helper_cuda.h>         // helper functions for CUDA error check
#endif

	#include <cuda_gl_interop.h>
	#include <vector_types.h>


struct sAgents{
	float4 pos[agents_total];
	float4 ids[agents_total];
};

////////////////////////////////////////////////////////////////////////////////
//!	Functions
////////////////////////////////////////////////////////////////////////////////

// crowdMpiCuda Nov 2013
void data_server(int argc, char *argv[]);
void init_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids, int world_width_node, int world_height_node, int num_comp_nodes);
bool runSimulation(int argc, char **argv);
void recv_data();
void compute_process(struct cudaGraphicsResource **vbo_resource);
void display_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids);
float Ranf( float, float );

/*
void 	runSimulation		();
void 	compute_process		();
void	refreshData			();
*/
//extern float4 *h_agents_in_pos; // variable that connects positions to CASIM
//extern bool unlocked_mpi;

extern int count_agents_total;
extern int *d_world;
extern float4 *d_agents_pos, *d_agents_ids; // variable that connects positions / ids to CASIM



