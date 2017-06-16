

//#define MINOTAURO
//#define DEBUG
//Number of iteration to measure time
//unsigned int iterations = 1000;


////////////////////////////////////////////////////////////////////////////////
//!	Global Variables
////////////////////////////////////////////////////////////////////////////////
#define MAX_GPU_COUNT	2

#define DATA_COLLECT 3
#define MAX(a,b) ((a > b) ? a : b)

#define agent_width 64
#define agent_height 64
#define world_width 4096
#define world_height 4096

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



struct sAgents{
	float4 pos[agents_total];
	float4 ids[agents_total];
};

////////////////////////////////////////////////////////////////////////////////
//!	Functions
////////////////////////////////////////////////////////////////////////////////
void data_server( int pid );
bool runSimulation();
void refreshData();
void init_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids, int world_width_node, int world_height_node, int num_comp_nodes);
void compute_process( int pid);
void display_data(sAgents h_agents_in);
void display_data2(float4 *h_agents_in_pos, float4 *h_agents_in_ids);
float Ranf( float, float );


extern float4 *h_agents_in_pos; // variable that connects positions to CASIM
extern bool unlocked_mpi;

