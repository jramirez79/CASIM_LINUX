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

#pragma once

#include "cMacros.h"
#include "cVertex.h"
#include "cVboManager.h"
#include "cGlErrorManager.h"
#include "cCamera.h"

#include <string>

using namespace std;

//=======================================================================================

#ifndef __STATIC_LOD
#define __STATIC_LOD

class StaticLod
{
public:
					StaticLod					(	GlslManager*		glsl_manager_,
													VboManager*			vbo_manager_,
													GlErrorManager*		err_manager_,
													string				owner_name_				);
					~StaticLod					(	void										);

	void			init						(	unsigned int		numAgentsWidth_,
													unsigned int		numAgentsHeight_		);
	void			runAssignment				(	unsigned int		target,
													unsigned int		posID,
													unsigned int		agentsIdsTexture,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);
	void			runAssignmentCuda			(	unsigned int		target,
													unsigned int		posBufferId,
													unsigned int		agentsIdsTexture,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);

	unsigned int	runSelection				(	unsigned int		target,
													float				groupID,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);
	unsigned int	runSelectionCuda			(	unsigned int		target,
													float				groupID,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);

	unsigned int	runAssignmentAndSelection	(	unsigned int		target,
													unsigned int		posID,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);
	unsigned int	runAssignmentAndSelection	(	unsigned int		target,
													unsigned int		posID,
													unsigned int		agentsIdsTexture,
													struct sVBOLod*		vboLOD,
													Camera*				cam						);

	enum			{ VBO_CULLED };
	unsigned int	numAgentsWidth;
	unsigned int	numAgentsHeight;

	unsigned int	primitivesWritten[5];

	unsigned int	texCoords;

private:
	void			initTexCoords				(	unsigned int		target					);
	void			initTexCoordsCuda			(	unsigned int		target					);

	GlslManager*	glsl_manager;
	VboManager*		vbo_manager;
	GlErrorManager*	err_manager;

	string			owner_name;

	unsigned int	numElements;
	unsigned int	numLODs;
	unsigned int*	posVBOLODs;
	unsigned int	query_generated;
	unsigned int	query_written;
	unsigned int*	posTexBufferId;

	unsigned int	primitivesGenerated[5];
	unsigned int	lodAid;
	unsigned int	lodSid;
	int				locAid[1];
	int				locSid[1];
	unsigned int	tc_index;
	unsigned int	tc_frame;
	unsigned int	tc_size;

	string			str_tang;
	string			str_AGENTS_NPOT;
	string			str_nearPlane;
	string			str_farPlane;
	string			str_ratio;
	string			str_X;
	string			str_Y;
	string			str_Z;
	string			str_camPos;
	string			str_lod;
	string			str_groupId;
};

#endif

//=======================================================================================
