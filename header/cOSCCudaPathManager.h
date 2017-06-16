#pragma once

#include <cuda_runtime.h>

#include "cMacros.h"
#include "cVertex.h"
#include "cLogManager.h"
#include "cVboManager.h"
#include "cFboManager.h"
#include "cGlErrorManager.h"

class cOSCCudaPathManager
{
public:
					cOSCCudaPathManager		( LogManager*		log_manager,
											  VboManager*		vbo_manager			);

					~cOSCCudaPathManager	( void									);
	bool			init					( unsigned int numCharacters			);

	void			runCuda					( unsigned int 		crowd_width,
											  unsigned int 		crowd_height,
											  float				parameter			);
	void			updateOSCPositions		( void									);
	int				getNumCharacters		( ) 			{ return numCharacters; };

	unsigned int				cuda_ppos_vbo_id;   // previous position
	unsigned int				cuda_ppos_vbo_size;
	unsigned int				cuda_ppos_vbo_index;
	unsigned int				cuda_ppos_vbo_frame;

	unsigned int				cuda_cpos_vbo_id;	// current position
	unsigned int				cuda_cpos_vbo_size;
	unsigned int				cuda_cpos_vbo_index;
	unsigned int				cuda_cpos_vbo_frame;

	unsigned int				cuda_npos_vbo_id;	// next position
	unsigned int				cuda_npos_vbo_size;
	unsigned int				cuda_npos_vbo_index;
	unsigned int				cuda_npos_vbo_frame;

	unsigned int 				cpos_tbo_id;

private:

	int							getMaxGflopsDeviceId	( void								);
	int							_ConvertSMVer2Cores		( int				major,
														  int				minor			);
	int							numCharacters;

protected:
	LogManager*					log_manager;
	VboManager*					vbo_manager;
	cudaGraphicsResource*		cuda_ppos_vbo_res;
	cudaGraphicsResource*		cuda_cpos_vbo_res;
	cudaGraphicsResource*		cuda_npos_vbo_res;

};
