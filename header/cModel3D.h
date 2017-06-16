// @REV_TAG SRL_09_2013
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

#include <unordered_map>
#include <vector>
#include <string>

#include "cMacros.h"
#include "cVertex.h"
#include "cVboManager.h"
#include "cGlslManager.h"
#include "cTextureManager.h"

//#ifdef ASSIMP_PURE_C
	#include <assimp/cimport.h>
	#include <assimp/scene.h>
	#include <assimp/postprocess.h>
	#include <assimp/material.h>
	#include <assimp/Importer.hpp>
/*#else
	#include <assimp/assimp.hpp>
	#include <assimp/aiScene.h>
	#include <assimp/aiPostProcess.h>
	#include <assimp/aiMaterial.h>
#endif*/

using namespace std;

typedef unordered_multimap<float,unsigned int> RefMap;

//=======================================================================================

#ifndef __MODEL3D
#define __MODEL3D

class Model3D
{
public:
											Model3D							(	void										);
											Model3D							(	string&					mname,
																				string&					rel_path,
																				VboManager*				vbo_manager,
																				GlslManager*			glsl_manager,
																				string&					fname,
																				float					scale				);
											Model3D							(	Model3D*				other				);
											~Model3D						(	void										);

	bool									init							(	bool					gen_vbos			);
	vector<GLuint>&							getSizes						(	void										);
	vector<GLuint>&							getIds							(	void										);

	vector<unsigned int>&					getIndicesUvs					(	void										);
	vector<unsigned int>&					getIndicesNormals				(	void										);
	vector<unsigned int>&					getIndicesLocations				(	void										);
	vector<Location4>&						getUniqueLocations				(	void										);
	vector<Normal>&							getUniqueNormals				(	void										);
	vector<Uv>&								getUniqueUvs					(	void										);
	vector<Face3>&							getFaces						(	void										);

	string									name;
#ifndef ASSIMP_PURE_C
	Assimp::Importer						importer;
#endif
	const struct aiScene*					scene;
	unsigned int							ppsteps;
	string									rel_path;
	string									fileName;
	string									currFile;
	float									scale;
	bool									inited;

private:

	void									recursive_gather_data			(	const struct aiScene*	sc,
																				const struct aiNode*	nd,
																				unsigned int			vbo_index,
																				unsigned int			vbo_frame			);
	void									recursive_gather_data			(	const struct aiScene*	sc,
																				const struct aiNode*	nd					);

	VboManager*								vbo_manager;
	GlslManager*							glsl_manager;

	vector<GLuint>							sizes;
	vector<GLuint>							ids;

	vector<unsigned int>					indices_uvs;
	vector<unsigned int>					indices_normals;
	vector<unsigned int>					indices_locations;

	vector<Location4>						unique_locations;
	vector<Normal>							unique_normals;
	vector<Uv>								unique_uvs;
	vector<Face3>							faces;

	RefMap									l_map;
	RefMap									n_map;
	RefMap									u_map;
};

#endif

//=======================================================================================
