
#pragma once

#include <cuda_runtime.h>

#include "cMacros.h"
#include "cVertex.h"
#include "cLogManager.h"
#include "cVboManager.h"
#include "cFboManager.h"
#include "cGlErrorManager.h"
#include "cOSCCudaPathManager.h"

class cMPICudaPathManager : public cOSCCudaPathManager
{
public:
					cMPICudaPathManager		( 	LogManager*		log_manager,
												VboManager*		vbo_manager			);
					~cMPICudaPathManager	( 	void								);

#ifdef MPI_PATHS_ON_NODES
	void			runCuda					(	unsigned int crowd_width,
												unsigned int crowd_height,
												float parameter
											);
#endif

	void 			updateMPIPositions 		( );


private:

};
