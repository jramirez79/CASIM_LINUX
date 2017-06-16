

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#include <new>
#include "cMacros.h"

#ifdef MPI_PATHS_ON_NODES

	#include "behaviorMPIpathsOnNodes.h"


	//#define MINOTAURO
	//#define DEBUG
	//Number of iteration to measure time
	unsigned int iterations = 10;

	// OpenGL Graphics includes
//	#include <GL/glew.h>
//	#include <GL/freeglut.h>

	int 	count_agents_total = 0;
	int 	*d_world = 0;
	float4 	*d_agents_pos=0, *d_agents_ids=0;



	////////////////////////////////////////////////////////////////////////////////
	//!	GL Global Variables
	////////////////////////////////////////////////////////////////////////////////
	// vbo variables
	GLuint vbo;
	struct cudaGraphicsResource *cuda_vbo_resource;
	void *d_vbo_buffer = NULL;

	// mouse controls
//	int mouse_old_x, mouse_old_y;
//	int mouse_buttons = 0;
//	float rotate_x = 0.0, rotate_y = 0.0;
//	float translate_z = 0.0;
//	StopWatchInterface *timer = NULL;

	// Auto-Verification Code
//	int fpsCount = 0;        // FPS count for averaging
//	int fpsLimit = 1;        // FPS limit for sampling
//	float avgFPS = 0.0f;
//	unsigned int frameCount = 0;
	unsigned int g_TotalErrors = 0;

//	const char *sSDKsample = "Crowd (MPI+Cuda+OpenGL)";

	////////////////////////////////////////////////////////////////////////////////
	//!	Functions
	////////////////////////////////////////////////////////////////////////////////
	// GL functionality

	bool initGL(int *argc, char **argv);
	void createVBO(GLuint *vbo, struct cudaGraphicsResource **vbo_res, unsigned int vbo_res_flags);
	void deleteVBO(GLuint *vbo, struct cudaGraphicsResource *vbo_res);
	void computeFPS();

	// rendering callbacks
//	void display();
//	void keyboard(unsigned char key, int x, int y);
//	void mouse(int button, int state, int x, int y);
//	void motion(int x, int y);
//	void timerEvent(int value);
//	void idle(void);

	//Simulation functions
	void data_server(int argc, char *argv[]);
	void init_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids, int world_width_node, int world_height_node, int num_comp_nodes);
	bool runSimulation(int argc, char **argv);
	void recv_data();
	void compute_process(struct cudaGraphicsResource **vbo_resource);
	void display_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids);
	float Ranf( float, float );
	void cleanup();

	extern "C" void launch_kernel_border(	float4 *d_agents_pos,
											float4 *d_agents_ids,
											int *d_world,
											int world_width_a,
											int world_height_a,
											int world_height_node,
											int world_width_node,
											int pid,
											int num_comp_nodes,
											int count_agents_total,
											float4 *d_agents_pos_fr,
											float4 *d_agents_ids_fr,
											cudaStream_t stream
										);

	extern "C" void launch_kernel_internal(	float4 *d_agents_pos,
											float4 *d_agents_ids,
											int *d_world,
											int world_width_a,
											int world_height_a,
											int world_height_node,
											int world_width_node,
											int pid,
											int num_comp_nodes,
											int count_agents_total
										);

//	extern "C" void launch_kernel_draw(float4 *d_agents_pos, float4 *d_tmp, float4 *d_ids, int count_agents_total, int pid);


	#ifdef MINOTAURO
	////////////////////////////////////////////////////////////////////////////////
	// These are CUDA Helper functions
	// This will output the proper CUDA error strings in the event that a CUDA host call returns an error
	#define checkCudaErrors(err)           __checkCudaErrors (err, __FILE__, __LINE__)

	inline void __checkCudaErrors( cudaError err, const char *file, const int line )
	{
		if( cudaSuccess != err) {
			fprintf(stderr, "%s(%i) : CUDA Runtime API error %d: %s.\n",
			file, line, (int)err, cudaGetErrorString( err ) );
			exit(-1);
		}
	}

	// This function returns the best GPU (with maximum GFLOPS)
	int gpuGetMaxGflopsDeviceId()
	{
		int current_device     = 0, sm_per_multiproc  = 0;
		int max_compute_perf   = 0, max_perf_device   = 0;
		int device_count       = 0, best_SM_arch      = 0;
		cudaDeviceProp deviceProp;
		cudaGetDeviceCount( &device_count );

		// Find the best major SM Architecture GPU device
		while (current_device < device_count)
		{
			cudaGetDeviceProperties( &deviceProp, current_device );
			if (deviceProp.major > 0 && deviceProp.major < 9999)
			{
				best_SM_arch = MAX(best_SM_arch, deviceProp.major);
			}
			current_device++;
		}

		// Find the best CUDA capable GPU device
		current_device = 0;
		while( current_device < device_count )
		{
			cudaGetDeviceProperties( &deviceProp, current_device );
			if (deviceProp.major == 9999 && deviceProp.minor == 9999)
			{
				sm_per_multiproc = 1;
			}
			else
			{
				sm_per_multiproc = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
			}

			int compute_perf  = deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;

			if( compute_perf  > max_compute_perf )
			{
				// If we find GPU with SM major > 2, search only these
				if ( best_SM_arch > 2 )
				{
					// If our device==dest_SM_arch, choose this, or else pass
					if (deviceProp.major == best_SM_arch)
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
	#endif //MINOTAURO


	////////////////////////////////////////////////////////////////////////////
	//! Check for OpenGL error
	//! @return CUTTrue if no GL error has been encountered, otherwise 0
	//! @param file  __FILE__ macro
	//! @param line  __LINE__ macro
	//! @note The GL error is listed on stderr
	//! @note This function should be used via the CHECK_ERROR_GL() macro
	////////////////////////////////////////////////////////////////////////////
	inline bool sdkCheckErrorGL( const char* file, const int line)
	{
		bool ret_val = true;

		// check for error
		GLenum gl_error = glGetError();
		if (gl_error != GL_NO_ERROR)
		{
			#ifdef _WIN32
				char tmpStr[512];
				// NOTE: "%s(%i) : " allows Visual Studio to directly jump to the file at the right line
				// when the user double clicks on the error line in the Output pane. Like any compile error.
				sprintf_s(tmpStr, 255, "\n%s(%i) : GL Error : %s\n\n", file, line, gluErrorString(gl_error));
				OutputDebugString(tmpStr);
			#endif
			fprintf(stderr, "GL Error in file '%s' in line %d :\n", file, line);
			fprintf(stderr, "%s\n", gluErrorString(gl_error));
			ret_val = false;
		}
		return ret_val;
	}

	#define SDK_CHECK_ERROR_GL()                                              \
		if( false == sdkCheckErrorGL( __FILE__, __LINE__)) {                  \
			exit(EXIT_FAILURE);                                               \
		}

	////////////////////////////////////////////////////////////////////////////////
	//! Initialize GL
	////////////////////////////////////////////////////////////////////////////////
/*
	bool initGL(int *argc, char **argv)
	{
		int np, pid;
		MPI_Comm_size(MPI_COMM_WORLD, &np);
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);

		int num_comp_nodes = np -1;
		int num_rows, num_cols;
		int world_height_node, world_width_node;

		num_rows = num_cols = sqrt(num_comp_nodes);

		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		std::string pids = static_cast<std::ostringstream*>( &(std::ostringstream() << pid ) )->str();

		int window_width = 400;
		int window_height = 400;
		glutInit(argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
		glutInitWindowSize(window_width, window_height);
		glutCreateWindow(pids.c_str());
		glutDisplayFunc(display);
		glutKeyboardFunc(keyboard);
		glutMotionFunc(motion);
		glutTimerFunc(REFRESH_DELAY, timerEvent,0);

		glutSetWindowTitle(pids.c_str());

		switch(pid)
		{
		case 1:
			glutInitWindowPosition(100, 500);
			break;
		case 2:
			glutInitWindowPosition(500, 500);
			break;
		case 3:
			glutInitWindowPosition(100, 100);
			break;
		case 4:
			glutInitWindowPosition(500, 100);
			break;
		}


		// initialize necessary OpenGL extensions
		glewInit();

		if (! glewIsSupported("GL_VERSION_2_0 "))
		{
			fprintf(stderr, "ERROR: Support for necessary OpenGL extensions missing.");
			fflush(stderr);
			return false;
		}

		// default initialization
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glDisable(GL_DEPTH_TEST);

		// projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0, 0, world_width_node, world_height_node);


		//gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 10.0);
		gluPerspective(60.0, (GLfloat)world_width_node / (GLfloat) world_height_node, 0.1, world_height_node*2);

		glMatrixMode(GL_MODELVIEW);

		SDK_CHECK_ERROR_GL();

		return true;
	}
	*/

	////////////////////////////////////////////////////////////////////////////////
	//! Display callback
	////////////////////////////////////////////////////////////////////////////////
	/*
	void display()
	{
		sdkStartTimer(&timer);

		int np, pid;
		MPI_Comm_size(MPI_COMM_WORLD, &np);
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);

		int num_comp_nodes = np -1;
		int num_rows, num_cols;
		int world_height_node, world_width_node;

		num_rows = num_cols = sqrt(num_comp_nodes);

		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		//glutSetWindowTitle("hola");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set view matrix

		glLoadIdentity();


		switch(pid)
		{
		case 1:
			gluLookAt(world_width_node/2, world_height_node/2, world_height_node * 0.9,  world_width_node/2, world_height_node/2, 0, 0,1,0);
			glColor3f(0.0,1.0,0.0) ;
			// ----- Box
			//X Axis
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(world_width_node, 0.0f, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(0.0f, world_height_node, 0.0f);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glEnd();

			//Y Axis
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, world_height_node, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node, 0.0f, 0.0f);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glEnd();

			break;
		case 2:
			gluLookAt(world_width_node*3/2, world_height_node/2,world_height_node * 0.9, world_width_node*3/2, world_height_node/2,0, 0,1,0);
			glColor3f(0.0,1.0,0.0) ;
			// ----- Box
			//X Axis
			glBegin(GL_LINES);
			glVertex3f(world_width_node, 0.0f, 0.0f);
			glVertex3f(world_width_node*2, 0.0f, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glVertex3f(world_width_node*2, world_height_node, 0.0f);
			glEnd();

			//Y Axis
			glBegin(GL_LINES);
			glVertex3f(world_width_node, 0.0f, 0.0f);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node*2, 0.0f, 0.0f);
			glVertex3f(world_width_node*2, world_height_node, 0.0f);
			glEnd();

			break;
		case 3:
			gluLookAt(world_width_node/2, world_height_node*3/2,world_height_node * 0.9, world_width_node/2, world_height_node*3/2,0, 0,1,0);
			glColor3f(0.0,1.0,0.0) ;
			// ----- Box
			//X Axis
			glBegin(GL_LINES);
			glVertex3f(0.0f, world_height_node, 0.0f);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(0.0f, world_height_node*2, 0.0f);
			glVertex3f(world_width_node, world_height_node*2, 0.0f);
			glEnd();

			//Y Axis
			glBegin(GL_LINES);
			glVertex3f(0.0f, world_height_node, 0.0f);
			glVertex3f(0.0f, world_height_node*2, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glVertex3f(world_width_node, world_height_node*2, 0.0f);
			glEnd();

			break;
		case 4:
			gluLookAt(world_width_node*3/2, world_height_node*3/2,world_height_node * 0.9, world_width_node*3/2, world_height_node*3/2,0, 0,1,0);
			glColor3f(0.0,1.0,0.0) ;
			// ----- Box
			//X Axis
			glBegin(GL_LINES);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glVertex3f(world_width_node*2, world_height_node, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node, world_height_node*2, 0.0f);
			glVertex3f(world_width_node*2, world_height_node*2, 0.0f);
			glEnd();

			//Y Axis
			glBegin(GL_LINES);
			glVertex3f(world_width_node, world_height_node, 0.0f);
			glVertex3f(world_width_node, world_height_node*2, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(world_width_node*2, world_height_node, 0.0f);
			glVertex3f(world_width_node*2, world_height_node*2, 0.0f);
			glEnd();

			break;
		}

		// render from the vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexPointer(4, GL_FLOAT, 0, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(3);
		//glDrawArrays(GL_POINTS, 0, agent_width * agent_height);
		glDrawArrays(GL_POINTS, 0, count_agents_total);
		glDisableClientState(GL_VERTEX_ARRAY);

*/

/*
		//Z axis
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, world_width);
		glEnd();

		glBegin(GL_TRIANGLES);
		  glColor3f(1.0f, 0.0f, 0.0f);
		  glVertex3f(0.0f, 0.0f, 0.0f);
		  glVertex3f(256.0f, 512.0f, 0.0f);
		  glVertex3f(512.0f, 0.0f, 0.0f);
		glEnd();
*/
/*
		glutSwapBuffers();

//		sdkStopTimer(&timer);
		computeFPS();
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Run OpenGL
	////////////////////////////////////////////////////////////////////////////////
	bool runSimulation(int argc, char **argv)
	{
		// Create the CUTIL timer
		//sdkCreateTimer(&timer);

		// First initialize OpenGL context, so we can properly set the GL for CUDA.
		// This is necessary in order to achieve optimal performance with OpenGL/CUDA interop.
/*
		if (false == initGL(&argc, argv))
		{
			return false;
		}
*/
		//cudaGLSetGLDevice( gpuGetMaxGflopsDeviceId() );

		// create VBO
		createVBO(&vbo, &cuda_vbo_resource, cudaGraphicsMapFlagsWriteDiscard);

		// register callbacks
/*
		glutDisplayFunc(display);
		glutKeyboardFunc(keyboard);
		glutMouseFunc(mouse);
		glutMotionFunc(motion);
		glutIdleFunc(idle);
*/
		recv_data();
		// start rendering mainloop
		//glutMainLoop();

		atexit(cleanup);

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Throught idle function defines loop to receive data
	////////////////////////////////////////////////////////////////////////////////
/*
	void idle(void)
	{
		compute_process(&cuda_vbo_resource);
		//refreshData(&cuda_vbo_resource);

		glutPostRedisplay();
	}
*/

	////////////////////////////////////////////////////////////////////////////////
	//! Init and distribute data between the slaves
	////////////////////////////////////////////////////////////////////////////////
	void data_server(int argc, char *argv[])
	{
		int np, pid;
		MPI_Comm_size(MPI_COMM_WORLD, &np);
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		MPI_Status status;

		int num_comp_nodes = np - 1;
		int i, count_agents_out, pos;

		float4 *h_agents_in_pos = new float4[agents_total];
		float4 *h_agents_in_ids = new float4[agents_total];
		float4 *h_agents_out_pos = new float4[agents_total];
		float4 *h_agents_out_ids = new float4[agents_total];

		/* Mapping world to nodes */
		int num_rows, num_cols;
		int world_height_node, world_width_node;

		char *buffer_out = NULL;

		num_rows = num_cols = sqrt(num_comp_nodes);
		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		//std::cout << "********************************************-data_server after inits pid "<<pid<<"-********************************************"<<std::endl;

		try{

			/* initialize input data */
			init_data(h_agents_in_pos, h_agents_in_ids, world_width_node, world_height_node, num_comp_nodes);

			#ifdef DEBUG
				printf("Init data\n");
				display_data(h_agents_in_pos, h_agents_in_ids);
			#endif

			//Kernel/Pragma
			//Distribute the data
			for ( int process = 1; process <= num_comp_nodes; process++ )
			{
				count_agents_out = 0;
				for( i = 0; i < agents_total; i++)
				{
					if(h_agents_in_ids[i].z == process)
					{
						h_agents_out_pos[count_agents_out] = h_agents_in_pos[i];
						h_agents_out_ids[count_agents_out] = h_agents_in_ids[i];
						count_agents_out++;
					}
				}

				printf ( "pid: %d, count_agents_out: %d \n", pid,  count_agents_out);
				unsigned int num_bytes_agents_out = count_agents_out * sizeof(float4);
				int tam_buffer_out = num_bytes_agents_out * 2 + sizeof(unsigned int);
				buffer_out = new char[tam_buffer_out];

				pos = 0;
				MPI_Pack(&count_agents_out, sizeof(unsigned int), MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
				MPI_Pack(h_agents_out_pos, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
				MPI_Pack(h_agents_out_ids, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);

				MPI_Send(buffer_out, tam_buffer_out, MPI_BYTE, process, 0, MPI_COMM_WORLD);
				delete[] buffer_out;
			}

			//runSimulation(argc, argv);
		}
		catch(...){
			printf("Exception Fail\n");
			delete h_agents_in_pos;
			delete h_agents_in_ids;
			delete h_agents_out_pos;
			delete h_agents_out_ids;
		}

		//release resources
		delete h_agents_in_pos;
		delete h_agents_in_ids;
		delete h_agents_out_pos;
		delete h_agents_out_ids;
	}

	////////////////////////////////////////////////////////////////////////////////////
	//! Slaves receive data from master
	////////////////////////////////////////////////////////////////////////////////////
	void recv_data()
	{
		int pid;
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		int server_process = 0;
		MPI_Status status;

		size_t size_world = world_width * world_height * sizeof(int);
		int *h_world = new int[world_width * world_height];

		float4 *h_agents_in_pos = new float4[agents_total];
		float4 *h_agents_in_ids = new float4[agents_total];

		unsigned int num_bytes_agents_max = agents_total * sizeof(float4);
		unsigned int num_bytes_agents_curr= 0;

		int tam_buffer_in = (agents_total * sizeof(float4) * 2) + sizeof(unsigned int);
		char *buffer_in = new char[tam_buffer_in];
		int pos, i;

		// Error code to check return values for CUDA calls
		cudaError_t err = cudaSuccess;

		////////////////////////////////////////////////////////////////////////////////
		// Allocate the device pointers
		////////////////////////////////////////////////////////////////////////////////
		err = cudaMalloc((void **)&d_world, size_world);
		if (err != cudaSuccess)
		{
			printf("Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_pos, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			printf("Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_ids, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			printf("Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		//Initialize host and device pointers
		cudaMemset(d_agents_pos, 0, agents_total * sizeof(float4));
		cudaMemset(d_agents_ids, 0, agents_total * sizeof(float4));


		try {

			////////////////////////////////////////////////////////////////////////////////
			//! Agents initial reception
			////////////////////////////////////////////////////////////////////////////////
			MPI_Recv(buffer_in, tam_buffer_in, MPI_BYTE, server_process, 0, MPI_COMM_WORLD, &status);

			MPI_Unpack(buffer_in, tam_buffer_in, &pos, &count_agents_total, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

			if(count_agents_total > 0 )
			{
				MPI_Unpack(buffer_in, tam_buffer_in, &pos, h_agents_in_pos, ( count_agents_total * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
				MPI_Unpack(buffer_in, tam_buffer_in, &pos, h_agents_in_ids, ( count_agents_total * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);

		#ifdef DEBUG
		//if(pid == 1){
			printf("Recv data pid: %d count_agents_total: %d\n", pid, count_agents_total);
			display_data(h_agents_in_pos, h_agents_in_ids);
			//}
		#endif

				//Set busy cells in the world
				for( i = 0; i < count_agents_total; i++)
				{
					h_world[(int) (   ( round(h_agents_in_pos[i].y) - 1) * world_width + round(h_agents_in_pos[i].x) )] = h_agents_in_ids[i].x;
				}

				num_bytes_agents_curr = count_agents_total * sizeof(float4);
			}

			////////////////////////////////////////////////////////////////////////////////
			//! Transfer data to GPU
			////////////////////////////////////////////////////////////////////////////////
			err = cudaMemcpy(d_world, h_world, size_world, cudaMemcpyHostToDevice);
			if (err != cudaSuccess)
			{
				printf("Failed to copy pointer from host to device 2 (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}

			err = cudaMemcpy(d_agents_pos, h_agents_in_pos, num_bytes_agents_curr, cudaMemcpyHostToDevice);
			if (err != cudaSuccess)
			{
				printf("Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}

			err = cudaMemcpy(d_agents_ids, h_agents_in_ids, num_bytes_agents_curr, cudaMemcpyHostToDevice);
			if (err != cudaSuccess)
			{
				printf("Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}

		}
		catch(...){
			printf("Exception Fail\n");
			// Release resources
			delete[] h_world;
			delete[] buffer_in;
			free(h_agents_in_pos);
			free(h_agents_in_ids);
		}


		// Release resources
		delete[] h_world;
		delete[] buffer_in;
		free(h_agents_in_pos);
		free(h_agents_in_ids);

		printf ("\n\n pid: %d, count_agents_total: %d \n\n", pid, count_agents_total);

	}

	////////////////////////////////////////////////////////////////////////////////////
	//! Slaves compute and return data to the Master, also interchange data between them
	////////////////////////////////////////////////////////////////////////////////////
	void compute_process(struct cudaGraphicsResource **vbo_resource)
	{

		int np, pid;
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		MPI_Comm_size(MPI_COMM_WORLD, &np);
		int server_process = 0;
		MPI_Status status;
		MPI_Request	send_request;

		int num_comp_nodes = np -1;

		size_t size_world = world_width * world_height * sizeof(int);

		float4 *d_agents_pos_fr, *d_agents_ids_fr;


		cudaStream_t stream0;
		cudaStreamCreate(&stream0);

		float4 *h_agents_int_pos = new float4[agents_total];
		float4 *h_agents_int_ids = new float4[agents_total];
		int count_agents_int = 0;

		float4 *h_agents_in_pos;
		float4 *h_agents_in_pos_fr, *h_agents_in_ids_fr;

		float4 *h_agents_in_ids = new float4[agents_total];

		float4 *h_agents_out_pos = new float4[agents_total];
		float4 *h_agents_out_ids = new float4[agents_total];
		float4 *h_agents_left_out_pos = new float4[agents_total];
		float4 *h_agents_left_out_ids = new float4[agents_total];
		float4 *h_agents_right_out_pos = new float4[agents_total];
		float4 *h_agents_right_out_ids = new float4[agents_total];
		float4 *h_agents_low_out_pos = new float4[agents_total];
		float4 *h_agents_low_out_ids = new float4[agents_total];
		float4 *h_agents_up_out_pos = new float4[agents_total];
		float4 *h_agents_up_out_ids = new float4[agents_total];

		unsigned int num_bytes_agents_max = agents_total * sizeof(float4);
		unsigned int num_bytes_agents_curr= count_agents_total * sizeof(float4);
		unsigned int count_agents_right_out = 0, count_agents_left_out = 0;
		unsigned int count_agents_up_out = 0, count_agents_low_out = 0;
		unsigned int count_agents_right_in = 0, count_agents_left_in = 0;
		unsigned int count_agents_up_in = 0, count_agents_low_in = 0;
		int pos = 0, i = 0, count_agents_out = 0;

		int tam_buffer_in = (agents_total * sizeof(float4) * 2) + sizeof(unsigned int);

		int num_rows, num_cols;
		int world_height_node, world_width_node;

		num_rows = num_cols = sqrt(num_comp_nodes);

		int left_neighbor = ( pid % num_cols != 1) ? (pid - 1) : MPI_PROC_NULL;
		int right_neighbor = (pid % num_cols) ? (pid + 1) : MPI_PROC_NULL;
		int up_neighbor = (pid <= (num_cols * num_cols) - num_cols) ? (pid + num_cols) : MPI_PROC_NULL;
		int low_neighbor = (pid > num_cols) ? (pid - num_cols) : MPI_PROC_NULL;

		char *buffer_left_out = NULL;
		char *buffer_right_in = NULL;
		char *buffer_right_out = NULL;
		char *buffer_left_in = NULL;
		char *buffer_up_out = NULL;
		char *buffer_low_in = NULL;
		char *buffer_low_out = NULL;
		char *buffer_up_in = NULL;
		char *buffer_out = NULL;

		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		// Error code to check return values for CUDA calls
		cudaError_t err = cudaSuccess;

		////////////////////////////////////////////////////////////////////////////////
		// Allocate the device pointers
		////////////////////////////////////////////////////////////////////////////////
		err = cudaMalloc((void **)&d_agents_pos_fr, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_ids_fr, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_pos, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_pos_fr, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_ids_fr, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}



		//Initialize host and device pointers
		cudaMemset(d_agents_pos_fr, 0, agents_total * sizeof(float4));
		cudaMemset(d_agents_ids_fr, 0, agents_total * sizeof(float4));
		memset(h_agents_in_pos, 0, agents_total * sizeof(float4));
		memset(h_agents_in_pos_fr, 0, agents_total * sizeof(float4));
		memset(h_agents_in_ids_fr, 0, agents_total * sizeof(float4));

//printf ("pid: %d count_agents_total: %d \n", pid, count_agents_total);

		try {


			//////////////////////////////////////////////////////////////////////////////////////////////
			//Loop throught GL idle function to process agents, interchange data with the nodes and return the results to the master
			//////////////////////////////////////////////////////////////////////////////////////////////
			if (count_agents_total)
			{


				////////////////////////////////////////////////////////////////////////////////
				//! Kernels Compute behavior and position
				////////////////////////////////////////////////////////////////////////////////
				launch_kernel_border(d_agents_pos, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
							pid, num_comp_nodes, count_agents_total, d_agents_pos_fr, d_agents_ids_fr, stream0);



				err = cudaMemcpyAsync(h_agents_in_pos_fr, d_agents_pos_fr, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream0);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				err = cudaMemcpyAsync(h_agents_in_ids_fr, d_agents_ids_fr, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream0);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				cudaStreamSynchronize(stream0);
//				cudaDeviceSynchronize();


				launch_kernel_internal(d_agents_pos, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
							pid, num_comp_nodes, count_agents_total);



				#ifdef DEBUG
//					if (pid == 4) // || pid == 1)
	//				{
						//printf("After kernel pid: %d\n", pid);
		//				display_data(h_agents_in_pos_fr, h_agents_in_ids_fr);
			//		}
				#endif

				////////////////////////////////////////////////////////////////////////////////
				//Identify agents by zone
				////////////////////////////////////////////////////////////////////////////////
				//Kernel/Pragma
				for( i = 0; i < count_agents_total; i++)
				{
					//left neighbor
					if(h_agents_in_ids_fr[i].z == left_neighbor)
					{
						h_agents_left_out_pos[count_agents_left_out] = h_agents_in_pos_fr[i];
						h_agents_left_out_ids[count_agents_left_out] = h_agents_in_ids_fr[i];
						count_agents_left_out++;
					} //right neighbor
					else if(h_agents_in_ids_fr[i].z == right_neighbor)
					{
						h_agents_right_out_pos[count_agents_right_out] = h_agents_in_pos_fr[i];
						h_agents_right_out_ids[count_agents_right_out] = h_agents_in_ids_fr[i];
						count_agents_right_out++;
					}//lower neighbor
					else if(h_agents_in_ids_fr[i].z == low_neighbor)
					{
						h_agents_low_out_pos[count_agents_low_out] = h_agents_in_pos_fr[i];
						h_agents_low_out_ids[count_agents_low_out] = h_agents_in_ids_fr[i];
						count_agents_low_out++;
					}//upper neighbor
					else if(h_agents_in_ids_fr[i].z == up_neighbor )
					{
						h_agents_up_out_pos[count_agents_up_out] = h_agents_in_pos_fr[i];
						h_agents_up_out_ids[count_agents_up_out] = h_agents_in_ids_fr[i];
						count_agents_up_out++;
					}
				}
			}
			//Fin if(count_agents_total)


			////////////////////////////////////////////////////////////////////////////////
			//! Send and receive data from the neighbors
			////////////////////////////////////////////////////////////////////////////////
			// send data to left, get data from right
			unsigned int num_bytes_agents_left = count_agents_left_out * sizeof(float4);

			int tam_buffer_left_out = num_bytes_agents_left * 2 + sizeof(unsigned int);
			buffer_left_out = new char[tam_buffer_left_out];
			buffer_right_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_left_out, sizeof(unsigned int), MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_left_out_pos, num_bytes_agents_left, MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_left_out_ids, num_bytes_agents_left, MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);

			#ifdef DEBUG
//				printf("before sendrecv pid: %d, left_neighbor: %d right_neighbor: %d count_agents_left_out: %d \n", pid, left_neighbor, right_neighbor, count_agents_left_out);
//				display_data(h_agents_left_out_pos, h_agents_left_out_ids);
			#endif

			MPI_Sendrecv(buffer_left_out, tam_buffer_left_out, MPI_BYTE, left_neighbor, DATA_COLLECT, buffer_right_in, tam_buffer_in, MPI_BYTE, right_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The last column does not get data from right
			if(right_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, &count_agents_right_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_right_in > 0 )
				{
					MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, h_agents_int_pos, ( count_agents_right_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, h_agents_int_ids, ( count_agents_right_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_right_in;

					#ifdef DEBUG
						printf("Intercambio pid: %d h_agents_right_in: \n", pid);
						printf("count_agents_out: %d, count_agents_int: %d, count_agents_right_in: %d\n", count_agents_out, count_agents_int, count_agents_right_in);
					#endif
				}
			}

			// send data to right, get data from left
			unsigned int num_bytes_agents_right = count_agents_right_out * sizeof(float4);
			int tam_buffer_right_out = num_bytes_agents_right * 2 + sizeof(unsigned int);
			buffer_right_out = new char[tam_buffer_right_out];
			buffer_left_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_right_out, sizeof(unsigned int), MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_right_out_pos, num_bytes_agents_right, MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_right_out_ids, num_bytes_agents_right, MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_right_out, tam_buffer_right_out, MPI_BYTE, right_neighbor, DATA_COLLECT, buffer_left_in, tam_buffer_in, MPI_BYTE, left_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The first column does not get data from left
			if(left_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, &count_agents_left_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_left_in > 0 )
				{
					MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_left_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_left_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_left_in;

					#ifdef DEBUG
						printf("Intercambio pid: %d h_agents_left_in: \n", pid);
						printf("count_agents_total: %d, count_agents_out: %d, count_agents_int: %d, count_agents_left_in: %d, pos-> x: %f y: %f z: %f w: %f ids-> x: %f y: %f z: %f w: %f\n",
									count_agents_total, count_agents_out, count_agents_int, count_agents_left_in, h_agents_int_pos->x, h_agents_int_pos->y, h_agents_int_pos->z,
									h_agents_int_pos->w, h_agents_int_ids->x, h_agents_int_ids->y, h_agents_int_ids->z, h_agents_int_ids->w);
					#endif
				}
			}

			//send data to up, get data from low
			unsigned int num_bytes_agents_up = count_agents_up_out * sizeof(float4);
			int tam_buffer_up_out = num_bytes_agents_up * 2 + sizeof(unsigned int);
			buffer_up_out = new char[tam_buffer_up_out];
			buffer_low_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_up_out, sizeof(unsigned int), MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_up_out_pos, num_bytes_agents_up, MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_up_out_ids, num_bytes_agents_up, MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_up_out, tam_buffer_up_out, MPI_BYTE, up_neighbor, DATA_COLLECT, buffer_low_in, tam_buffer_in, MPI_BYTE, low_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The first row does not get data from down
			if(low_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, &count_agents_low_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_low_in > 0 )
				{
					MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_low_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_low_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_low_in;

					#ifdef DEBUG
						printf("Intercambio pid: %d h_agents_low_in: \n", pid);
						printf("count_agents_out: %d, count_agents_int: %d, count_agents_low_in: %d\n", count_agents_out, count_agents_int, count_agents_low_in);
					#endif
				}
			}

			//send data to low, get data from up
			unsigned int num_bytes_agents_low = count_agents_low_out * sizeof(float4);
			int tam_buffer_low_out = num_bytes_agents_low * 2 + sizeof(unsigned int);
			buffer_low_out = new char[tam_buffer_low_out];
			buffer_up_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_low_out, sizeof(unsigned int), MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_low_out_pos, num_bytes_agents_low, MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_low_out_ids, num_bytes_agents_low, MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_low_out, tam_buffer_low_out, MPI_BYTE, low_neighbor, DATA_COLLECT, buffer_up_in, tam_buffer_in, MPI_BYTE, up_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The last row does not get data from up
			if(up_neighbor != MPI_PROC_NULL )
			{
				pos = 0;
				MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, &count_agents_up_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_up_in > 0 )
				{
					MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_up_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_up_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_up_in;

					#ifdef DEBUG
						printf("Intercambio pid: %d h_agents_up_in: \n", pid);
						printf("count_agents_out: %d, count_agents_int: %d, count_agents_up_in: %d, pos-> x: %f y: %f z: %f w: %f ids-> x: %f y: %f z: %f w: %f\n",
								count_agents_out, count_agents_int, count_agents_up_in, h_agents_int_pos->x, h_agents_int_pos->y, h_agents_int_pos->z,
								h_agents_int_pos->w, h_agents_int_ids->x, h_agents_int_ids->y, h_agents_int_ids->z, h_agents_int_ids->w);
							//display_data(*h_agents_up_in);
					#endif
				}
			}

			//MPI_Barrier(MPI_COMM_WORLD);

			//Transfer to GPU the new agents received during interchange
			if(count_agents_int)
			{
				//Restart the cuda buffers when is over 80% of its capacity
				if ( count_agents_total >= (agents_total * 0.8) )
				{
					err = cudaMemcpy(h_agents_in_pos, d_agents_pos, num_bytes_agents_curr, cudaMemcpyDeviceToHost);
					if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}

					err = cudaMemcpy(h_agents_in_ids, d_agents_ids, num_bytes_agents_curr, cudaMemcpyDeviceToHost);
					if (err != cudaSuccess)
					{
							fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
							exit(EXIT_FAILURE);
					}

					//Selection of internal agents
					for( i = 0; i < count_agents_total; i++)
					{
						if(h_agents_in_ids[i].z == pid )
						{
							h_agents_out_pos[count_agents_out] = h_agents_in_pos[i];
							h_agents_out_ids[count_agents_out] = h_agents_in_ids[i];
							count_agents_out++;
						}
					}

					unsigned int num_bytes_agents_out = count_agents_out  * sizeof(float4);

					cudaMemset(d_agents_pos, 0, agents_total * sizeof(float4));
					cudaMemset(d_agents_ids, 0, agents_total * sizeof(float4));

					err = cudaMemcpy(d_agents_pos, h_agents_out_pos, num_bytes_agents_out, cudaMemcpyHostToDevice);
					if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
							exit(EXIT_FAILURE);
					}

					err = cudaMemcpy(d_agents_ids, h_agents_out_ids, num_bytes_agents_out, cudaMemcpyHostToDevice);
					if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}
					count_agents_total = count_agents_out;
				}

				num_bytes_agents_curr = count_agents_int * sizeof(float4);

				err = cudaMemcpy(d_agents_pos + count_agents_total, h_agents_int_pos, num_bytes_agents_curr, cudaMemcpyHostToDevice);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				err = cudaMemcpy(d_agents_ids + count_agents_total, h_agents_int_ids, num_bytes_agents_curr, cudaMemcpyHostToDevice);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				count_agents_total += count_agents_int;

				memset(h_agents_int_pos, 0, count_agents_int * sizeof(float4));
				memset(h_agents_int_ids, 0, count_agents_int * sizeof(float4));
			}


/*
			////////////////////////////////////////////////////////////////////////////////
			//! Kernels transfer data to draw
			//  This section sends CUDA memory  to OpenGL memory (VBO)
			////////////////////////////////////////////////////////////////////////////////
			// map OpenGL buffer object for writing from CUDA

			float4 *d_tmp = new float4[count_agents_total];

			//checkCudaErrors(cudaGraphicsMapResources(1, vbo_resource, 0));
			cudaGraphicsMapResources(1, vbo_resource, 0);

			size_t num_agents_bytes;
			//checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&d_tmp, &num_agents_bytes, *vbo_resource));
			cudaGraphicsResourceGetMappedPointer((void **)&d_tmp, &num_agents_bytes, *vbo_resource);
			if (err != cudaSuccess)
			{
				fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}

			//cudaMemset(d_tmp, 0, count_agents_total * sizeof(float4));

//			launch_kernel_draw(d_agents_pos, d_tmp, d_agents_ids, count_agents_total, pid);

			// unmap buffer object
			//checkCudaErrors(cudaGraphicsUnmapResources(1, vbo_resource, 0));
			cudaGraphicsUnmapResources(1, vbo_resource, 0);
*/
			#ifdef DEBUG
			/*
				float4 *d_tmp = new float4[count_agents_total];
				err = cudaMemcpy(h_agents_out_pos, d_tmp, count_agents_total * sizeof(float4), cudaMemcpyDeviceToHost);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				printf("============== pid: %d =================\n", pid);
				for( int i = 0; i < count_agents_total; i++ )
				printf("x: %f y: %f z: %f, w: %f \n", h_agents_out_pos[i].x, h_agents_out_pos[i].y, h_agents_out_pos[i].z, h_agents_out_pos[i].w);
*/
			//    MPI_Barrier(MPI_COMM_WORLD);
			#endif

			// Release resources
			delete[] h_agents_int_pos;
			delete[] h_agents_int_ids;

			delete[] h_agents_out_pos;
			delete[] h_agents_out_ids;
			delete[] h_agents_left_out_pos;
			delete[] h_agents_left_out_ids;
			delete[] h_agents_right_out_pos;
			delete[] h_agents_right_out_ids;
			delete[] h_agents_up_out_pos;
			delete[] h_agents_up_out_ids;
			delete[] h_agents_low_out_pos;
			delete[] h_agents_low_out_ids;


			cudaFreeHost(h_agents_in_pos);
			cudaFreeHost(h_agents_in_pos_fr);
			cudaFreeHost(h_agents_in_ids_fr);

			delete[] h_agents_in_ids;

			cudaFree(d_agents_pos_fr);
			cudaFree(d_agents_ids_fr);

			cudaStreamDestroy(stream0);

		}
		catch(...){
			printf("Exception Fail\n");
			// Release resources
			delete[] h_agents_int_pos;
			delete[] h_agents_int_ids;

			delete[] h_agents_out_pos;
			delete[] h_agents_out_ids;
			delete[] h_agents_left_out_pos;
			delete[] h_agents_left_out_ids;
			delete[] h_agents_right_out_pos;
			delete[] h_agents_right_out_ids;
			delete[] h_agents_up_out_pos;
			delete[] h_agents_up_out_ids;
			delete[] h_agents_low_out_pos;
			delete[] h_agents_low_out_ids;


			cudaFreeHost(h_agents_in_pos);
			cudaFreeHost(h_agents_in_pos_fr);
			cudaFreeHost(h_agents_in_ids_fr);

			delete[] h_agents_in_ids;

			cudaFree(d_agents_pos_fr);
			cudaFree(d_agents_ids_fr);

			cudaStreamDestroy(stream0);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Create VBO
	////////////////////////////////////////////////////////////////////////////////
	void createVBO(GLuint *vbo, struct cudaGraphicsResource **vbo_res,
				   unsigned int vbo_res_flags)
	{
		//assert(vbo);

		// create buffer object
		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);

		// initialize buffer object
		unsigned int size = agent_width * agent_height * 4 * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// register this buffer object with CUDA
		//checkCudaErrors(cudaGraphicsGLRegisterBuffer(vbo_res, *vbo, vbo_res_flags));
		cudaGraphicsGLRegisterBuffer(vbo_res, *vbo, vbo_res_flags);

		SDK_CHECK_ERROR_GL();
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Delete VBO
	////////////////////////////////////////////////////////////////////////////////
	void deleteVBO(GLuint *vbo, struct cudaGraphicsResource *vbo_res)
	{
		// unregister this buffer object with CUDA
		cudaGraphicsUnregisterResource(vbo_res);

		glBindBuffer(1, *vbo);
		glDeleteBuffers(1, vbo);

		*vbo = 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Timer Event
	////////////////////////////////////////////////////////////////////////////////
/*
	void timerEvent(int value)
	{
		glutPostRedisplay();
		glutTimerFunc(REFRESH_DELAY, timerEvent,0);
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Cleanup function
	////////////////////////////////////////////////////////////////////////////////
/*
	void cleanup()
	{
//		sdkDeleteTimer(&timer);

		if (vbo)
		{
			deleteVBO(&vbo, cuda_vbo_resource);
		}
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Keyboard events handler
	////////////////////////////////////////////////////////////////////////////////
/*
	void keyboard(unsigned char key, int x, int y)
	{
		switch (key)
		{
			case (27) :
				exit(EXIT_SUCCESS);
				break;
		}
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Mouse event handlers
	////////////////////////////////////////////////////////////////////////////////
/*
	void mouse(int button, int state, int x, int y)
	{
		if (state == GLUT_DOWN)
		{
			mouse_buttons |= 1<<button;
		}
		else if (state == GLUT_UP)
		{
			mouse_buttons = 0;
		}

		mouse_old_x = x;
		mouse_old_y = y;
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Motion
	////////////////////////////////////////////////////////////////////////////////
/*
	void motion(int x, int y)
	{
		float dx, dy;
		dx = (float)(x - mouse_old_x);
		dy = (float)(y - mouse_old_y);

		if (mouse_buttons & 1)
		{
			rotate_x += dy * 0.2f;
			rotate_y += dx * 0.2f;
		}
		else if (mouse_buttons & 4)
		{
			translate_z += dy * 0.01f;
		}

		mouse_old_x = x;
		mouse_old_y = y;
	}
*/
	/*************************************************************************/
	/** Main Function							**/
	/*************************************************************************/
	/*
	int main(int argc, char *argv[])
	{
		int pid = -1, np = -1;

		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		MPI_Comm_size(MPI_COMM_WORLD, &np);

		if(np<5)
		{
			if(0 == pid) printf("Needed 5 or more processes.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
			return 1;
		}
		//pid_pid
		if(pid == 0)
			data_server(argc, argv);
		else //if (pid == 1)
			runSimulation(argc, argv);
			//compute_process(argc, argv);

		MPI_Finalize();
		cudaFree(d_world);
		cudaFree(d_agents_pos);
		cudaFree(d_agents_ids);

		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////
	//! compute FPS
	////////////////////////////////////////////////////////////////////////////////
	void computeFPS()
	{
		frameCount++;
		fpsCount++;

		if (fpsCount == fpsLimit)
		{
			avgFPS = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
			fpsCount = 0;
			fpsLimit = (int)MAX(avgFPS, 1.f);

			sdkResetTimer(&timer);
		}

		char fps[256];
		sprintf(fps, "MPI Cuda GL Interop (VBO): %3.1f fps (Max 100Hz)", avgFPS);
		//glutSetWindowTitle(fps);
	}
*/
	////////////////////////////////////////////////////////////////////////////////
	//! Random number generator
	////////////////////////////////////////////////////////////////////////////////
	#define TOP	2147483647.		// 2^31 - 1
	float Ranf( float low, float high )
	{
		float r = (float)rand( );
		return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Initialize agents with random positions
	////////////////////////////////////////////////////////////////////////////////
	void init_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids, int world_width_node, int world_height_node, int num_comp_nodes)
	{
		const float PI = 3.1415926535;
		const float XMIN = world_width_node/10;
		const float XMAX = world_width_node - world_width_node / 10;
		const float YMIN = world_height_node /10;
		const float YMAX = world_height_node - world_height_node / 10 ;
		const float THMIN =	0.0;
		const float THMAX =	2*PI;
		const float VMIN = 2.0;
		const float VMAX = 2.0;

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

		for(int i = 0; i< agents_total;i++)
		{
			h_agents_in_ids[i].x = i + 1;
			h_agents_in_ids[i].y = -1;
			h_agents_in_ids[i].w = -1;
			h_agents_in_pos[i].x = Ranf( XMIN, XMAX );
			h_agents_in_pos[i].y = Ranf( YMIN, YMAX );
			h_agents_in_pos[i].z = Ranf( THMIN, THMAX );
			h_agents_in_pos[i].w = Ranf( VMIN, VMAX );
			if (num_comp_nodes == 4)
				h_agents_in_ids[i].z = lookup_node4[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
			else if (num_comp_nodes == 16)
				h_agents_in_ids[i].z = lookup_node16[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
			else if (num_comp_nodes == 9)
						h_agents_in_ids[i].z = lookup_node9[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
		}
	}


	////////////////////////////////////////////////////////////////////////////////
	//! Display Data (auxiliar for debug)
	////////////////////////////////////////////////////////////////////////////////
	void display_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids)
	{
		for( int i = 0; i < agents_total; i++ )
		printf("id: %f pid: %2.0f status: %2.3f flagMisc: %2.3f x: %2.3f y: %2.3f z: %2.3f, w: %2.3f \n", h_agents_in_ids[i].x, h_agents_in_ids[i].z, h_agents_in_ids[i].y,
				h_agents_in_ids[i].w, h_agents_in_pos[i].x, h_agents_in_pos[i].y, h_agents_in_pos[i].z, h_agents_in_pos[i].w);
	}



#endif

//
//=======================================================================================
//
//
//=======================================================================================
//
//
//=======================================================================================
//


#ifdef MPI_PATHS


	#include "behaviorMPI.h"

	//#define DEBUG

	extern "C" void launch_kernel_border	(	float4 *d_agents_pos,
												float4 *d_agents_ids,
												int *d_world,
												int world_width_a,
												int world_height_a,
												int world_height_node,
												int world_width_node,
												int pid,
												int num_comp_nodes,
												int count_agents_total,
												float4 *d_agents_pos_fr,
												float4 *d_agents_ids_fr,
												cudaStream_t stream0
											);

	extern "C" void launch_kernel_internal(		float4 *d_agents_pos,
												float4 *d_agents_ids,
												int *d_world,
												int world_width_a,
												int world_height_a,
												int world_height_node,
												int world_width_node,
												int pid,
												int num_comp_nodes,
												int count_agents_total,
												cudaStream_t stream1
											);

	//=======================================================================================
	//
	//

	float4 *h_agents_in_pos = 0; // variable that connects positions to CASIM
	bool unlocked_mpi = true;


	#ifdef MINOTAURO
	////////////////////////////////////////////////////////////////////////////////
	// These are CUDA Helper functions
	// This will output the proper CUDA error strings in the event that a CUDA host call returns an error
	#define checkCudaErrors(err)           __checkCudaErrors (err, __FILE__, __LINE__)

	inline void __checkCudaErrors( cudaError err, const char *file, const int line )
	{
		if( cudaSuccess != err) {
			fprintf(stderr, "%s(%i) : CUDA Runtime API error %d: %s.\n",
			file, line, (int)err, cudaGetErrorString( err ) );
			exit(-1);
		}
	}

	// This function returns the best GPU (with maximum GFLOPS)
	int gpuGetMaxGflopsDeviceId()
	{
		int current_device     = 0, sm_per_multiproc  = 0;
		int max_compute_perf   = 0, max_perf_device   = 0;
		int device_count       = 0, best_SM_arch      = 0;
		cudaDeviceProp deviceProp;
		cudaGetDeviceCount( &device_count );

		// Find the best major SM Architecture GPU device
		while (current_device < device_count)
		{
			cudaGetDeviceProperties( &deviceProp, current_device );
			if (deviceProp.major > 0 && deviceProp.major < 9999)
			{
				best_SM_arch = MAX(best_SM_arch, deviceProp.major);
			}
			current_device++;
		}

		// Find the best CUDA capable GPU device
		current_device = 0;
		while( current_device < device_count )
		{
			cudaGetDeviceProperties( &deviceProp, current_device );
			if (deviceProp.major == 9999 && deviceProp.minor == 9999)
			{
				sm_per_multiproc = 1;
			}
			else
			{
				sm_per_multiproc = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
			}

			int compute_perf  = deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;

			if( compute_perf  > max_compute_perf )
			{
				// If we find GPU with SM major > 2, search only these
				if ( best_SM_arch > 2 )
				{
					// If our device==dest_SM_arch, choose this, or else pass
					if (deviceProp.major == best_SM_arch)
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
	#endif //MINOTAURO


	////////////////////////////////////////////////////////////////////////////////
	//! Loop to receive data from the slaves
	////////////////////////////////////////////////////////////////////////////////
	bool runSimulation()
	{
		struct timeval start;
		struct timeval stop;


		//while(true)
		{
			gettimeofday(&start,NULL);

	//	   	for(int it= 0; it < iterations; it++)
			{
				refreshData();
	//   			printf("==================iteracion: %d=======================\n", it);
				//if(it == 870)
				//{
					//printf("==================iteracion: %d=======================\n", it);
					//exit(EXIT_FAILURE);
				//}
			}

			gettimeofday(&stop,NULL);
			double begin = (double) start.tv_sec + (double) (start.tv_usec / 1.E6);
			double end = (double) stop.tv_sec + (double) (stop.tv_usec / 1.E6);
			double elapsed =  end - begin;
	//	    printf("Time %d iterations: %0.5f [sec] \n", iterations, elapsed);
		}

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Receive data from the slaves
	////////////////////////////////////////////////////////////////////////////////

	void refreshData()
	{
		int np;
		MPI_Comm_size(MPI_COMM_WORLD, &np);


		MPI_Status status;

		int num_comp_nodes = np -1;
		unsigned int tam_buffer = (agents_total * sizeof(float4) * 2) + sizeof(unsigned int);



		if (h_agents_in_pos == 0)
		h_agents_in_pos 	= new float4[agents_total];
		float4 *h_agents_in_ids 	= new float4[agents_total];
		float4 *h_agents_out_pos 	= new float4[agents_total];
		float4 *h_agents_out_ids 	= new float4[agents_total];

		char *buffer_out = new char[tam_buffer];

		unsigned int count_agents_out = 0, i=0;

		try {
			/* Wait for nodes to compute */
			MPI_Barrier(MPI_COMM_WORLD);

			for(int process = 1; process <= num_comp_nodes; process++)
			{

				if (MPI_Recv(buffer_out, tam_buffer, MPI_BYTE, process, DATA_COLLECT, MPI_COMM_WORLD, &status) == MPI_SUCCESS )
				{

					count_agents_out = 0;
					int pos = 0;
					MPI_Unpack(buffer_out, tam_buffer, &pos, &count_agents_out, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

					if(count_agents_out > 0 )
					{
						MPI_Unpack(buffer_out, tam_buffer, &pos, h_agents_out_pos, ( count_agents_out * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
						MPI_Unpack(buffer_out, tam_buffer, &pos, h_agents_out_ids, ( count_agents_out * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);

						for( i = 0; i < count_agents_out; i++)
						{

							h_agents_in_pos[(int)(h_agents_out_ids[i].x - 1)] = h_agents_out_pos[i];
							h_agents_in_ids[(int)(h_agents_out_ids[i].x - 1)] = h_agents_out_ids[i];
							h_agents_in_pos[(int)(h_agents_out_ids[i].x - 1)].z = 0;
							h_agents_in_pos[(int)(h_agents_out_ids[i].x - 1)].w = 1;
						}
					}
				}
				else
				{
					printf ("Communication error. Process Id %d\n", process);
				}


			}

			#ifdef DEBUG
				printf("Final Data:\n");
				display_data2(h_agents_in_pos, h_agents_in_ids);
			#endif

				//unlocked_mpi = false;
		}
		catch(...){
			printf("Exception Fail\n");
			delete [] h_agents_in_pos;
			delete [] h_agents_in_ids;
			delete [] h_agents_out_pos;
			delete [] h_agents_out_ids;
			delete [] buffer_out;
		}


		// release resources
		//delete [] h_agents_in_pos;
		delete [] h_agents_in_ids;
		delete [] h_agents_out_pos;
		delete [] h_agents_out_ids;
		delete [] buffer_out;

	}


	////////////////////////////////////////////////////////////////////////////////
	//! Init and distribute data between the slaves
	////////////////////////////////////////////////////////////////////////////////
	void data_server(int pid)
	{
		int np;
		int device;

		MPI_Comm_size(MPI_COMM_WORLD, &np);
		MPI_Status status;

		int num_comp_nodes = np - 1;
		int i, count_agents_out, pos;

		float4 *h_agents_in_pos = new float4[agents_total];
		float4 *h_agents_in_ids = new float4[agents_total];
		float4 *h_agents_out_pos = new float4[agents_total];
		float4 *h_agents_out_ids = new float4[agents_total];

		/* Mapping world to nodes */

		int num_rows, num_cols;
		int world_height_node, world_width_node;

		num_rows = num_cols = sqrt(num_comp_nodes);
		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		char *buffer_out = 0;

		try{
			// initialize input data
			init_data(h_agents_in_pos, h_agents_in_ids, world_width_node, world_height_node, num_comp_nodes);

			#ifdef DEBUG
				printf("Init data\n");
				display_data2(h_agents_in_pos, h_agents_in_ids);
				//exit(EXIT_FAILURE);
			#endif




			//Kernel/Pragma
			for ( int process = 1; process <= num_comp_nodes; process++ )
			{
				count_agents_out = 0;
				for( i = 0; i < agents_total; i++)
				{
					if(h_agents_in_ids[i].z == process)
					{
						h_agents_out_pos[count_agents_out] = h_agents_in_pos[i];
						h_agents_out_ids[count_agents_out] = h_agents_in_ids[i];
						count_agents_out++;
					}
				}




				unsigned int num_bytes_agents_out = count_agents_out * sizeof(float4);
				int tam_buffer_out = num_bytes_agents_out * 2 + sizeof(unsigned int);



				buffer_out=new char[tam_buffer_out];



				pos = 0;
				MPI_Pack(&count_agents_out, sizeof(unsigned int), MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
				MPI_Pack(h_agents_out_pos, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
				MPI_Pack(h_agents_out_ids, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);




				MPI_Send(buffer_out, tam_buffer_out, MPI_BYTE, process, 0, MPI_COMM_WORLD);
			}

			//checkCudaErrors( cudaGetDevice(&device));
			//printf ("Process Id %d is using GPU %d\n", pid, device);

			runSimulation();



		}
		catch(...){
			printf("Exception Fail\n");
			delete [] h_agents_in_pos;
			delete [] h_agents_in_ids;
			delete [] h_agents_out_pos;
			delete [] h_agents_out_ids;
		}

		//release resources
		delete [] h_agents_in_pos;
		delete [] h_agents_in_ids;
		delete [] h_agents_out_pos;
		delete [] h_agents_out_ids;
		delete [] buffer_out;

	}

	////////////////////////////////////////////////////////////////////////////////////
	//! Slaves compute and return data to the Master, also interchange data between them
	////////////////////////////////////////////////////////////////////////////////////
	void compute_process(int pid)
	{
		int np;
		//MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		MPI_Comm_size(MPI_COMM_WORLD, &np);
		int server_process = 0;
		MPI_Status status;
		MPI_Request	send_request;



		int num_comp_nodes = np -1;

		size_t size_world = world_width * world_height * sizeof(int);

		int *h_world = new int[world_width * world_height];
		int *d_world;

		float4 *d_agents_pos 	= 0, *d_agents_ids		= 0;
		float4 *d_agents_pos_fr = 0, *d_agents_ids_fr 	= 0;

		cudaStream_t stream0, stream1;
		cudaStreamCreate(&stream0);
		cudaStreamCreate(&stream1);

		float4 *h_agents_int_pos = new float4[agents_total];
		float4 *h_agents_int_ids = new float4[agents_total];
		int count_agents_int = 0;
		int count_agents_total = 0;

		float4 *h_agents_in_pos, *h_agents_in_ids;
		float4 *h_agents_in_pos_fr, *h_agents_in_ids_fr;

		sAgents *h_agents_out 		= new sAgents();
		sAgents *h_agents_left_out 	= new sAgents();
		sAgents *h_agents_right_out = new sAgents();
		sAgents *h_agents_low_out 	= new sAgents();
		sAgents *h_agents_up_out 	= new sAgents();

		unsigned int num_bytes_agents_max = agents_total * sizeof(float4);
		unsigned int num_bytes_agents_curr= 0;
		unsigned int count_agents_right_out = 0, count_agents_left_out = 0;
		unsigned int count_agents_up_out = 0, count_agents_low_out = 0;
		unsigned int count_agents_right_in = 0, count_agents_left_in = 0;
		unsigned int count_agents_up_in = 0, count_agents_low_in = 0;
		int pos = 0, i = 0, count_agents_out = 0;


		int tam_buffer_in = (agents_total * sizeof(float4) * 2) + sizeof(unsigned int);

		char *buffer_in = new char[tam_buffer_in];



		char *buffer_left_out 	= 0;
		char *buffer_right_in 	= 0;
		char *buffer_right_out 	= 0;
		char *buffer_left_in	= 0;
		char *buffer_out 		= 0;
		char *buffer_low_out 	= 0;
		char *buffer_up_in 		= 0;
		char *buffer_up_out 	= 0;
		char *buffer_low_in 	= 0;


		int num_rows, num_cols;
		int world_height_node, world_width_node;

		num_rows = num_cols = sqrt(num_comp_nodes);

		int left_neighbor = ( pid % num_cols != 1) ? (pid - 1) : MPI_PROC_NULL;
		int right_neighbor = (pid % num_cols) ? (pid + 1) : MPI_PROC_NULL;
		int up_neighbor = (pid <= (num_cols * num_cols) - num_cols) ? (pid + num_cols) : MPI_PROC_NULL;
		int low_neighbor = (pid > num_cols) ? (pid - num_cols) : MPI_PROC_NULL;

		world_width_node = world_width / num_cols;
		world_height_node = world_height / num_rows;

		// Error code to check return values for CUDA calls
		cudaError_t err = cudaSuccess;



		////////////////////////////////////////////////////////////////////////////////
		// Allocate the device pointers
		////////////////////////////////////////////////////////////////////////////////
		err = cudaMalloc((void **)&d_world, size_world);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_pos, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_ids, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_pos_fr, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMalloc((void **)&d_agents_ids_fr, num_bytes_agents_max);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_pos, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_ids, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_pos_fr, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaHostAlloc((void **)&h_agents_in_ids_fr, num_bytes_agents_max, cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to allocate device pointer (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}


		//Initialize host and device pointers
		cudaMemset(d_agents_pos, 0, agents_total * sizeof(float4));
		cudaMemset(d_agents_ids, 0, agents_total * sizeof(float4));
		cudaMemset(d_agents_pos_fr, 0, agents_total * sizeof(float4));
		cudaMemset(d_agents_ids_fr, 0, agents_total * sizeof(float4));
		memset(h_agents_in_pos_fr, 0, count_agents_total * sizeof(float4));
		memset(h_agents_in_ids_fr, 0, count_agents_total * sizeof(float4));


		////////////////////////////////////////////////////////////////////////////////
		//! Agents initial reception
		////////////////////////////////////////////////////////////////////////////////

		MPI_Recv(buffer_in, tam_buffer_in, MPI_BYTE, server_process, 0, MPI_COMM_WORLD, &status);



		MPI_Unpack(buffer_in, tam_buffer_in, &pos, &count_agents_total, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

		if(count_agents_total > 0 )
		{
			MPI_Unpack(buffer_in, tam_buffer_in, &pos, h_agents_in_pos, ( count_agents_total * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
			MPI_Unpack(buffer_in, tam_buffer_in, &pos, h_agents_in_ids, ( count_agents_total * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);

			//Set busy cells in the world
			for( i = 0; i < count_agents_total; i++)
			{
				h_world[(int) (   ( round(h_agents_in_pos[i].y) - 1) * world_width + round(h_agents_in_pos[i].x) )] = h_agents_in_ids[i].x;
			}

			num_bytes_agents_curr = count_agents_total * sizeof(float4);
		}


		////////////////////////////////////////////////////////////////////////////////
		//! Transfer data to GPU
		////////////////////////////////////////////////////////////////////////////////
		err = cudaMemcpy(d_world, h_world, size_world, cudaMemcpyHostToDevice);
		if (err != cudaSuccess)
		{
			fprintf(stderr, "Failed to copy pointer from host to device 2 (error code %s)!\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		err = cudaMemcpy(d_agents_pos, h_agents_in_pos, num_bytes_agents_curr, cudaMemcpyHostToDevice);
		if (err != cudaSuccess)
			{
				fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}

		err = cudaMemcpy(d_agents_ids, h_agents_in_ids, num_bytes_agents_curr, cudaMemcpyHostToDevice);
		if (err != cudaSuccess)
			{
				fprintf(stderr, "Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
				exit(EXIT_FAILURE);
			}


		//////////////////////////////////////////////////////////////////////////////////////////////
		//Loop to process agents, interchange data with the nodes and return the results to the master
		//////////////////////////////////////////////////////////////////////////////////////////////
		while(true)
		{
			//printf ("compute_process() by process %d BEGIN: \n", pid);
			if (count_agents_total)
			{
				#ifdef DEBUG
					//if (pid == 3){
						//printf("Before kernel pid: %d\n", pid);
						//display_data2(h_agents_in_pos_fr, h_agents_in_ids_fr);
						//display_data(*h_agents_in);
						//exit(EXIT_FAILURE);
					//}
				#endif

				////////////////////////////////////////////////////////////////////////////////
				//! Kernels Compute behavior and position
				////////////////////////////////////////////////////////////////////////////////
				launch_kernel_border(d_agents_pos, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
						pid, num_comp_nodes, count_agents_total, d_agents_pos_fr, d_agents_ids_fr, stream0);

				launch_kernel_internal(d_agents_pos, d_agents_ids, d_world, world_width, world_height, world_height_node, world_width_node,
						pid, num_comp_nodes, count_agents_total, stream1);

				err = cudaMemcpyAsync(h_agents_in_pos_fr, d_agents_pos_fr, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream0);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				err = cudaMemcpyAsync(h_agents_in_ids_fr, d_agents_ids_fr, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream0);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				cudaStreamSynchronize(stream0);

				err = cudaMemcpyAsync(h_agents_in_pos, d_agents_pos, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream1);
				if (err != cudaSuccess)
				{
						fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
				}

				err = cudaMemcpyAsync(h_agents_in_ids, d_agents_ids, num_bytes_agents_curr, cudaMemcpyDeviceToHost, stream1);
				if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}

				/*** Sacar trazas con cudaMemcpyAsync y cudaMemcpy normal
				err = cudaMemcpy(h_agents_in_pos, d_agents_pos, num_bytes_agents_curr, cudaMemcpyDeviceToHost);
				if (err != cudaSuccess)
				{
						fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
				}

				err = cudaMemcpy(h_agents_in_ids, d_agents_ids, num_bytes_agents_curr, cudaMemcpyDeviceToHost);
				if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from device to host (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}
				 ***/

				#ifdef DEBUG
				//	if (pid == 3 || pid == 1)
					//{
						//printf("After kernel pid: %d\n", pid);
						//display_data2(h_agents_in_pos_fr, h_agents_in_ids_fr);
						//exit(EXIT_FAILURE);
					//}
				#endif

				////////////////////////////////////////////////////////////////////////////////
				//Identify agents by zone
				////////////////////////////////////////////////////////////////////////////////
				//Kernel/Pragma
				for( i = 0; i < count_agents_total; i++)
				{
					//left neighbor
					if(h_agents_in_ids_fr[i].z == left_neighbor)
					{
						h_agents_left_out->pos[count_agents_left_out] = h_agents_in_pos_fr[i];
						h_agents_left_out->ids[count_agents_left_out] = h_agents_in_ids_fr[i];
						count_agents_left_out++;
					} //right neighbor
					else if(h_agents_in_ids_fr[i].z == right_neighbor)
					{
						h_agents_right_out->pos[count_agents_right_out] = h_agents_in_pos_fr[i];
						h_agents_right_out->ids[count_agents_right_out] = h_agents_in_ids_fr[i];
						count_agents_right_out++;
					}//lower neighbor
					else if(h_agents_in_ids_fr[i].z == low_neighbor)
					{
						h_agents_low_out->pos[count_agents_low_out] = h_agents_in_pos_fr[i];
						h_agents_low_out->ids[count_agents_low_out] = h_agents_in_ids_fr[i];
						count_agents_low_out++;
					}//upper neighbor
					else if(h_agents_in_ids_fr[i].z == up_neighbor )
					{
						h_agents_up_out->pos[count_agents_up_out] = h_agents_in_pos_fr[i];
						h_agents_up_out->ids[count_agents_up_out] = h_agents_in_ids_fr[i];
						count_agents_up_out++;
					}
				}
			}//Fin if(count_agents_total)


	#ifdef DEBUG
	//		if (count_agents_left_out || count_agents_right_out || count_agents_low_out || count_agents_up_out)
	//		{
	//			printf("pid: %d, count_agents_left_out: %d, count_agents_right_out: %d, count_agents_up_out: %d, count_agents_low_out: %d\n",
	//					pid, count_agents_left_out, count_agents_right_out, count_agents_up_out, count_agents_low_out);
	//			exit(EXIT_FAILURE);
	//		}
	#endif

			//=================fin if(count_agents_in)=================================

			MPI_Barrier(MPI_COMM_WORLD);

			////////////////////////////////////////////////////////////////////////////////
			//! Send and receive data from the neighbors
			////////////////////////////////////////////////////////////////////////////////
			// send data to left, get data from right
			unsigned int num_bytes_agents_left = count_agents_left_out * sizeof(float4);

			int tam_buffer_left_out = num_bytes_agents_left * 2 + sizeof(unsigned int);
			buffer_left_out = new char[tam_buffer_left_out];
			buffer_right_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_left_out, sizeof(unsigned int), MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_left_out->pos, num_bytes_agents_left, MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_left_out->ids, num_bytes_agents_left, MPI_BYTE, buffer_left_out, tam_buffer_left_out, &pos, MPI_COMM_WORLD);

			#ifdef DEBUG
				//printf("before sendrecv pid: %d, left_neighbor: %d right_neighbor: %d count_agents_left_out: %d \n", pid, left_neighbor, right_neighbor, count_agents_left_out);
				//display_data(h_agents_left_out);
			#endif


			MPI_Sendrecv(buffer_left_out, tam_buffer_left_out, MPI_BYTE, left_neighbor, DATA_COLLECT, buffer_right_in, tam_buffer_in, MPI_BYTE, right_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);



			//The last column does not get data from right
			if(right_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, &count_agents_right_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_right_in > 0 )
				{
					MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, h_agents_int_pos, ( count_agents_right_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_right_in, tam_buffer_in, &pos, h_agents_int_ids, ( count_agents_right_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_right_in;

					#ifdef DEBUG
						//printf("WWWpid: %d h_agents_down_in: \n", pid);
						//display_data(h_agents_down_in);
					#endif
				}
			}

			// send data to right, get data from left
			unsigned int num_bytes_agents_right = count_agents_right_out * sizeof(float4);
			int tam_buffer_right_out = num_bytes_agents_right * 2 + sizeof(unsigned int);
			buffer_right_out = new char[tam_buffer_right_out];
			buffer_left_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_right_out, sizeof(unsigned int), MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_right_out->pos, num_bytes_agents_right, MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_right_out->ids, num_bytes_agents_right, MPI_BYTE, buffer_right_out, tam_buffer_right_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_right_out, tam_buffer_right_out, MPI_BYTE, right_neighbor, DATA_COLLECT, buffer_left_in, tam_buffer_in, MPI_BYTE, left_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);



			//The first column does not get data from left
			if(left_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, &count_agents_left_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_left_in > 0 )
				{
					MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_left_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_left_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_left_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_left_in;

					#ifdef DEBUG
						printf("WWWpid: %d h_agents_left_in: \n", pid);
						printf("count_agents_total: %d, count_agents_out: %d, count_agents_int: %d, count_agents_left_in: %d, pos-> x: %f y: %f z: %f w: %f ids-> x: %f y: %f z: %f w: %f\n",
									count_agents_total, count_agents_out, count_agents_int, count_agents_left_in, h_agents_int_pos->x, h_agents_int_pos->y, h_agents_int_pos->z,
									h_agents_int_pos->w, h_agents_int_ids->x, h_agents_int_ids->y, h_agents_int_ids->z, h_agents_int_ids->w);
					#endif
				}
			}

			//send data to up, get data from low
			unsigned int num_bytes_agents_up = count_agents_up_out * sizeof(float4);
			int tam_buffer_up_out = num_bytes_agents_up * 2 + sizeof(unsigned int);
			char *buffer_up_out = new char[tam_buffer_up_out];
			char *buffer_low_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_up_out, sizeof(unsigned int), MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_up_out->pos, num_bytes_agents_up, MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_up_out->ids, num_bytes_agents_up, MPI_BYTE, buffer_up_out, tam_buffer_up_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_up_out, tam_buffer_up_out, MPI_BYTE, up_neighbor, DATA_COLLECT, buffer_low_in, tam_buffer_in, MPI_BYTE, low_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The first row does not get data from down
			if(low_neighbor != MPI_PROC_NULL)
			{
				pos = 0;
				MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, &count_agents_low_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);


				if(count_agents_low_in > 0 )
				{
					MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_low_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_low_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_low_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_low_in;

					#ifdef DEBUG
						//printf("WWWpid: %d h_agents_down_in: \n", pid);
					//display_data(h_agents_down_in);
					#endif
				}
			}

			//send data to low, get data from up
			unsigned int num_bytes_agents_low = count_agents_low_out * sizeof(float4);
			int tam_buffer_low_out = num_bytes_agents_low * 2 + sizeof(unsigned int);
			buffer_low_out = new char[tam_buffer_low_out];
			buffer_up_in = new char[tam_buffer_in];

			pos = 0;
			MPI_Pack(&count_agents_low_out, sizeof(unsigned int), MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_low_out->pos, num_bytes_agents_low, MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_low_out->ids, num_bytes_agents_low, MPI_BYTE, buffer_low_out, tam_buffer_low_out, &pos, MPI_COMM_WORLD);

			MPI_Sendrecv(buffer_low_out, tam_buffer_low_out, MPI_BYTE, low_neighbor, DATA_COLLECT, buffer_up_in, tam_buffer_in, MPI_BYTE, up_neighbor, DATA_COLLECT, MPI_COMM_WORLD, &status);

			//The last row does not get data from up
			if(up_neighbor != MPI_PROC_NULL )
			{
				pos = 0;
				MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, &count_agents_up_in, sizeof(unsigned int), MPI_BYTE, MPI_COMM_WORLD);

				if(count_agents_up_in > 0 )
				{
					MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, h_agents_int_pos + count_agents_int, ( count_agents_up_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					MPI_Unpack(buffer_up_in, tam_buffer_in, &pos, h_agents_int_ids + count_agents_int, ( count_agents_up_in * sizeof(float4) ), MPI_BYTE, MPI_COMM_WORLD);
					count_agents_int += count_agents_up_in;

					#ifdef DEBUG
						printf("WWWpid: %d h_agents_up_in: \n", pid);
						printf("count_agents_out: %d, count_agents_int: %d, count_agents_up_in: %d, pos-> x: %f y: %f z: %f w: %f ids-> x: %f y: %f z: %f w: %f\n",
								count_agents_out, count_agents_int, count_agents_up_in, h_agents_int_pos->x, h_agents_int_pos->y, h_agents_int_pos->z,
								h_agents_int_pos->w, h_agents_int_ids->x, h_agents_int_ids->y, h_agents_int_ids->z, h_agents_int_ids->w);
						//display_data(*h_agents_up_in);
						//exit(EXIT_FAILURE);
					#endif
				}
			}



			#ifdef DEBUG
			if (pid == 3)
			{
				//printf("After sendrcv pid: %d\n", pid);
				//display_data(h_agents_right_node);
				//display_data(h_agents_in);
				//printf("pid: %d count_agents_left: %d\n", pid, count_agents_left);
				//printf("pid: %d count_agents_right: %d\n", pid, count_agents_right);


				//display_data(h_agents_in);
				//display_data(h_agents_up_in);
				//display_data(h_agents_down_in);

				//if (pid == 2)
				//{
					//printf("After sendrcv pid: %d count_agents_out: %d\n", pid, count_agents_out);
					//display_data(*h_agents_in);
					//if(tmp_it == 3)
						//	exit(EXIT_FAILURE);
				//}

			}
			#endif


			////////////////////////////////////////////////////////////////////////////////
			//Adding internal agents to interchange agents
			////////////////////////////////////////////////////////////////////////////////
			//Make sure compute and copy of internal agents is completed
			cudaStreamSynchronize(stream1);



			//Selection of internal agents
			for( i = 0; i < count_agents_total; i++)
			{
				if(h_agents_in_ids[i].z == pid )
				{
					h_agents_out->pos[count_agents_out] = h_agents_in_pos[i];
					h_agents_out->ids[count_agents_out] = h_agents_in_ids[i];
					count_agents_out++;
				}
			}


			////////////////////////////////////////////////////////////////////////////////
			//! Send data to the master
			////////////////////////////////////////////////////////////////////////////////
			unsigned int count_agents_to_master = count_agents_out + count_agents_int;
			unsigned int num_bytes_agents_out = count_agents_out  * sizeof(float4);
			unsigned int num_bytes_agents_int = count_agents_int  * sizeof(float4);

			int tam_buffer_out = (num_bytes_agents_out + num_bytes_agents_int) * 2 + sizeof(unsigned int);
			buffer_out = new char[tam_buffer_out];

			pos = 0;



			MPI_Pack(&count_agents_to_master, sizeof(unsigned int), MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_out->pos, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_int_pos, num_bytes_agents_int, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_out->ids, num_bytes_agents_out, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);
			MPI_Pack(h_agents_int_ids, num_bytes_agents_int, MPI_BYTE, buffer_out, tam_buffer_out, &pos, MPI_COMM_WORLD);

			MPI_Send(buffer_out, tam_buffer_out, MPI_BYTE, server_process, DATA_COLLECT, MPI_COMM_WORLD);

			//Transfer to GPU the new agents received during interchange
			if(count_agents_int)
			{
				//Restart the cuda buffers when is over 80% of his capacity
				if ( count_agents_total >= (agents_total * 0.8) )
				{
					//exit(EXIT_FAILURE);
					cudaMemset(d_agents_pos, 0, agents_total * sizeof(float4));
					cudaMemset(d_agents_ids, 0, agents_total * sizeof(float4));

					err = cudaMemcpy(d_agents_pos, h_agents_out->pos, num_bytes_agents_out, cudaMemcpyHostToDevice);
					if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}

					err = cudaMemcpy(d_agents_ids, h_agents_out->ids, num_bytes_agents_out, cudaMemcpyHostToDevice);
					if (err != cudaSuccess)
					{
						fprintf(stderr, "Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
						exit(EXIT_FAILURE);
					}
					count_agents_total = count_agents_out;
				}

				num_bytes_agents_curr = count_agents_int * sizeof(float4);

				err = cudaMemcpy(d_agents_pos + count_agents_total, h_agents_int_pos, num_bytes_agents_curr, cudaMemcpyHostToDevice);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from host to device 3 (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				err = cudaMemcpy(d_agents_ids + count_agents_total, h_agents_int_ids, num_bytes_agents_curr, cudaMemcpyHostToDevice);
				if (err != cudaSuccess)
				{
					fprintf(stderr, "Failed to copy pointer from host to device 4 (error code %s)!\n", cudaGetErrorString(err));
					exit(EXIT_FAILURE);
				}

				count_agents_total += count_agents_int;

				memset(h_agents_int_pos, 0, count_agents_int * sizeof(float4));
				memset(h_agents_int_ids, 0, count_agents_int * sizeof(float4));
			}

			num_bytes_agents_curr = count_agents_total * sizeof(float4);

			//Reset initial conditions
			count_agents_left_out = 0;
			count_agents_right_out = 0;
			count_agents_low_out = 0;
			count_agents_up_out = 0;
			count_agents_out = 0;
			count_agents_int = 0;
			count_agents_right_in = 0;
			count_agents_left_in = 0;
			count_agents_low_in = 0;
			count_agents_up_in = 0;

			memset(h_agents_left_out, 0, sizeof(sAgents));
			memset(h_agents_right_out, 0, sizeof(sAgents));
			memset(h_agents_up_out, 0, sizeof(sAgents));
			memset(h_agents_low_out, 0, sizeof(sAgents));
			memset(h_agents_out, 0, sizeof(sAgents));
			cudaMemset(d_agents_pos_fr, 0, count_agents_total * sizeof(float4));
			cudaMemset(d_agents_ids_fr, 0, count_agents_total * sizeof(float4));
			memset(h_agents_in_pos_fr, 0, count_agents_total * sizeof(float4));
			memset(h_agents_in_ids_fr, 0, count_agents_total * sizeof(float4));



			//printf ("compute_process() END:\n");

			delete [] buffer_out;
			delete [] buffer_low_out;
			delete [] buffer_up_in;

			delete [] buffer_up_out;
			delete [] buffer_low_in;

			delete [] buffer_right_out;
			delete [] buffer_left_in;

			delete [] buffer_left_out;
			delete [] buffer_right_in;



		}//End loop while

		/* Release resources */

		delete h_agents_out;
		delete h_agents_left_out;
		delete h_agents_right_out;
		delete h_agents_up_out;
		delete h_agents_low_out;

		delete [] h_world;
		delete [] h_agents_int_pos;
		delete [] h_agents_int_ids;





		free(h_agents_in_pos);
		free(h_agents_in_ids);
		free(h_agents_in_pos_fr);
		free(h_agents_in_ids_fr);
		cudaFree(d_world);
		cudaFree(d_agents_pos);
		cudaFree(d_agents_pos_fr);
		cudaFree(d_agents_ids);
		cudaFree(d_agents_ids_fr);


		cudaStreamDestroy(stream0);
		cudaStreamDestroy(stream1);

	}

	/*************************************************************************/
	/** Main Function							**/
	/*************************************************************************/
	/*
	int main(int argc, char *argv[])
	{
	#ifdef	MINOTAURO
		setvbuf (stdout , NULL , _IONBF , NULL );
	#endif

		int pid = -1, np = -1;
		int numGPUs, device;

		checkCudaErrors(cudaGetDeviceCount(&numGPUs));
		printf("CUDA-capable device count: %i\n", numGPUs);
		if (numGPUs > MAX_GPU_COUNT)
			numGPUs = MAX_GPU_COUNT;


		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &pid);
		MPI_Comm_size(MPI_COMM_WORLD, &np);

		printf ("pid %d\n", pid);
		printf ("np %d\n", np);

		if(np<5)
		{
			if(0 == pid) printf("Needed 5 or more processes.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
			return 1;
		}


		if(pid == 0)
		{
			checkCudaErrors(cudaSetDevice(numGPUs - MAX_GPU_COUNT));
			data_server(pid);

		}
		else
		{
			checkCudaErrors(cudaSetDevice(numGPUs - MAX_GPU_COUNT + 1));
			checkCudaErrors( cudaGetDevice(&device));
			printf ("Process Id %d is using GPU %d\n", pid, device);

			compute_process(pid);
		}

		MPI_Finalize();
		return 0;
	}
	*/

	////////////////////////////////////////////////////////////////////////////////
	//! Random number generator
	////////////////////////////////////////////////////////////////////////////////
	#define TOP	2147483647.		// 2^31 - 1
	float Ranf( float low, float high )
	{
		float r = (float)rand( );
		return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Initialize agents with random positions
	////////////////////////////////////////////////////////////////////////////////
	void init_data(float4 *h_agents_in_pos, float4 *h_agents_in_ids, int world_width_node, int world_height_node, int num_comp_nodes)
	{


		const float PI = 3.1415926535;
		const float XMIN = world_width/3;
		const float XMAX = 2*world_width/3;
		const float YMIN = world_height/3;
		const float YMAX = 2*world_height/3 ;
		const float THMIN =	0.0;
		const float THMAX =	2*PI;
		const float VMIN = 1.0;
		const float VMAX = 3.0;

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



		for(int i = 0; i< agents_total;i++)
		{
			h_agents_in_ids[i].x = i + 1;
			h_agents_in_ids[i].y = -1;
			h_agents_in_ids[i].w = -1;
			h_agents_in_pos[i].x = Ranf( XMIN, XMAX );
			h_agents_in_pos[i].y = Ranf( YMIN, YMAX );
			h_agents_in_pos[i].z = Ranf( THMIN, THMAX );
			h_agents_in_pos[i].w = Ranf( VMIN, VMAX );
			if (num_comp_nodes == 4)
				h_agents_in_ids[i].z = lookup_node4[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
			else if (num_comp_nodes == 16)
				h_agents_in_ids[i].z = lookup_node16[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
			else if (num_comp_nodes == 9)
						h_agents_in_ids[i].z = lookup_node9[(int)h_agents_in_pos[i].y/world_height_node][(int)h_agents_in_pos[i].x/world_width_node];
		}


	}

	////////////////////////////////////////////////////////////////////////////////
	//! Display Data (auxiliar for debug)
	////////////////////////////////////////////////////////////////////////////////
	void display_data(sAgents h_agents_in)
	{
		for( int i = 0; i < agents_total; i++ )
		printf("id: %f pid: %f status: %f flagMisc: %f x: %f y: %f z: %f, w: %f \n", h_agents_in.ids[i].x, h_agents_in.ids[i].z, h_agents_in.ids[i].y, h_agents_in.ids[i].w, h_agents_in.pos[i].x, h_agents_in.pos[i].y, h_agents_in.pos[i].z, h_agents_in.pos[i].w);
	}

	////////////////////////////////////////////////////////////////////////////////
	//! Display Data (auxiliar for debug)
	////////////////////////////////////////////////////////////////////////////////
	void display_data2(float4 *h_agents_in_pos, float4 *h_agents_in_ids)
	{
		for( int i = 0; i < agents_total; i++ )
		printf("id: %f pid: %f status: %f flagMisc: %f x: %f y: %f z: %f, w: %f \n", h_agents_in_ids[i].x, h_agents_in_ids[i].z, h_agents_in_ids[i].y,
				h_agents_in_ids[i].w, h_agents_in_pos[i].x, h_agents_in_pos[i].y, h_agents_in_pos[i].z, h_agents_in_pos[i].w);
	}
#endif


