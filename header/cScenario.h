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

#include "cVertex.h"
#include "cMacros.h"
#include "cVboManager.h"
#include "cGlslManager.h"

#include "cTextureManager.h"
//#ifdef ASSIMP_PURE_C
	#include <assimp/cimport.h>
	#include <assimp/scene.h>
	#include <assimp/postprocess.h>
	#include <assimp/material.h>
	#include <assimp/Importer.hpp>
/*
#else
	#include <assimp/assimp.hpp>
	#include <assimp/aiScene.h>
	#include <assimp/aiPostProcess.h>
	#include <assimp/aiMaterial.h>
#endif
*/
using namespace std;

//=======================================================================================

typedef struct
{
	unsigned int vbo_index;
	unsigned int vbo_frame;
	unsigned int vbo_size;
	unsigned int vbo_id;
	unsigned int tex_id;
	float tint[3];
	bool tint_set;
	string tex_file;
} scenario_mesh;

//=======================================================================================

#ifndef __SCENARIO
#define __SCENARIO

class Scenario
{
public:
							Scenario						(	string&					name_,
																string&					rel_path_,
																VboManager*				vbo_manager_,
																GlslManager*			shader_manager_,
																string&					fname_,
																float					scale_			);
							~Scenario						(	void									);

	void					draw							(	void									);

	string					name;
private:
	void					assimp_recursive_gather_data	(	const struct aiScene*	sc,
																const struct aiNode*	nd				);

	float					scale;
	VboManager*				vbo_manager;
	Assimp::Importer		importer;
	const struct aiScene*	scene;
	vector<GLuint>			sizes;
	vector<GLuint>			ids;
	int						numTextures;
	map<string, GLuint>		textureIdMap;
	vector<scenario_mesh>	meshes;
	string					rel_path;
	string					fname;
	unsigned int			vbo_frame;

	GlslManager*			glsl_manager;

	char*					chr_amb;
	char*					chr_textured;
	char*					chr_tint;
};

#endif

//=======================================================================================
