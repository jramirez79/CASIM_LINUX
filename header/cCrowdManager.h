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

#include <string>
#include <vector>
#include <map>

#include "cMacros.h"
#include "cLogManager.h"
#include "cCrowdParser.h"
#include "cModelProps.h"
#include "cCharacterGroup.h"
#include "cCrowd.h"
#include "cFboManager.h"
#include "cVboManager.h"
#include "cGlslManager.h"
#include "cGlErrorManager.h"
#include "cCamera.h"
#include "cTextureManager.h"
#include "cModel3D.h"

using namespace std;

//=======================================================================================

#ifndef __CROWD_MANAGER
#define __CROWD_MANAGER

class CrowdManager
{
public:
								CrowdManager					(	LogManager*							log_manager_,
																	GlErrorManager*						err_manager_,
																	FboManager*							fbo_manager_,
																	VboManager*							vbo_manager_,
																	GlslManager*						glsl_manager_,
																	string&								xml_file_			);
								~CrowdManager					(	void													);

	bool						loadAssets						(	void													);
	void						setFboManager					(	FboManager*							fbo_manager_		);
	void						initPaths						(	float								plane_scale,
																	vector<float>&						policy,
																	unsigned int						grid_width,
																	unsigned int						grid_height,
																	vector<glm::vec2>&					occupation,
																	vector<vector<GROUP_FORMATION>>&	formations			);
	void						updatePolicy					(	vector<float>&						policy				);
	void						getDensity						(	vector<float>&						_density,
																	unsigned int						index				);
	void						runPaths						(	void													);
#ifdef DEMO_SHADER
	void						draw							(	Camera*								camera,
																	float*								viewMat,
																	float*								projMat,
																	float*								shadowMat,
																	bool								wireframe,
																	bool								shadows,
																	bool								doHandD,
																	bool								doPatterns,
																	bool								doColor,
																	bool								doFacial			);
#else
	void						draw							(	Camera*								camera,
																	float*								viewMat,
																	float*								projMat,
																	float*								shadowMat,
																	bool								wireframe,
																	bool								shadows				);
#endif
	void						nextFrame						(	void													);
	vector<Crowd*>&				getCrowds						(	void													);

private:

	vector<Crowd*>				crowds;
	vector<ModelProps*>			clothing_model_props;
	vector<ModelProps*>			facial_model_props;
	vector<ModelProps*>			rigging_model_props;
	vector<CharacterGroup*>		char_groups;
	LogManager*					log_manager;
	FboManager*					fbo_manager;
	VboManager*					vbo_manager;
	GlslManager*				glsl_manager;
	GlErrorManager*				err_manager;
	CrowdParser*				crowd_parser;
	string						xml_file;

	map<string, unsigned int>	clothing_props_names_map;
	map<string, unsigned int>	facial_props_names_map;
	map<string, unsigned int>	rigging_props_names_map;
	map<string, unsigned int>	group_names_map;
};

#endif

//=======================================================================================
