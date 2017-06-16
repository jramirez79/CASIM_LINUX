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

#include "cTextureManager.h"
#include "cGlslManager.h"
#include "cVboManager.h"
#include "cFrustum.h"
#include "cGlErrorManager.h"
#include "cLogManager.h"

#include <vector>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

//=======================================================================================

#ifndef	__SKYBOX_MANAGER
#define __SKYBOX_MANAGER

class SkyboxManager
{
public:
									SkyboxManager			( unsigned int		id_,
															  GlslManager*		glsl_manager_,
															  VboManager*		vbo_manager_,
															  LogManager*		log_manager_,
															  GlErrorManager*	err_manager_,
															  vec3&				center,
															  vec3&				extents,
															  vector<bool>&		bump,
															  vector<float>&	tile,
															  bool				instancing_,
															  bool				lighting_		);
									~SkyboxManager			( void								);

	bool							LoadSkyboxTextures		( string&			FRONT_Filename,
															  unsigned int		front_env,
															  string&			BACK_Filename,
															  unsigned int		back_env,
															  string&			LEFT_Filename,
															  unsigned int		left_env,
															  string&			RIGHT_Filename,
															  unsigned int		right_env,
															  string&			TOP_Filename,
															  unsigned int		top_env,
															  string&			BOTTOM_Filename,
															  unsigned int		bottom_env		);
	bool							LoadSkyboxBumpTextures	( string&			FRONT_Filename,
															  unsigned int		front_env,
															  string&			BACK_Filename,
															  unsigned int		back_env,
															  string&			LEFT_Filename,
															  unsigned int		left_env,
															  string&			RIGHT_Filename,
															  unsigned int		right_env,
															  string&			TOP_Filename,
															  unsigned int		top_env,
															  string&			BOTTOM_Filename,
															  unsigned int		bottom_env		);
	bool							LoadSkyboxSpecTextures	( string&			FRONT_Filename,
															  unsigned int		front_env,
															  string&			BACK_Filename,
															  unsigned int		back_env,
															  string&			LEFT_Filename,
															  unsigned int		left_env,
															  string&			RIGHT_Filename,
															  unsigned int		right_env,
															  string&			TOP_Filename,
															  unsigned int		top_env,
															  string&			BOTTOM_Filename,
															  unsigned int		bottom_env		);
	void							draw					( Frustum*			frustum,
															  bool				bump_enabled,
															  bool				draw_bv			);
	void							draw					( bool				bump_enabled,
															  bool				draw_bv			);
	unsigned int					getVertexCount			( int				face			);
	unsigned int					getTextureWeight		( int				face			);
public:
	enum							SKYBOX_PLANE{
										FRONT,
										BACK,
										LEFT,
										RIGHT,
										TOP,
										BOTTOM
									};
private:
	typedef struct
	{
		vec3						center;
		vec3						halfdiag;
		float						bR;
		float						bG;
		float						bB;
	}								WallBox;
	GLuint							DIFFUSE_IDS[6];
	GLuint							BUMPMAP_IDS[6];
	GLuint							SPECMAP_IDS[6];
	vector<bool>					bumped;
	vector<float>					tiling;
	bool							instancing;
	bool							lighting;
	vector<float>					front_positions;
	vector<float>					front_rotations;
	vector<float>					back_positions;
	vector<float>					back_rotations;
	vector<float>					left_positions;
	vector<float>					left_rotations;
	vector<float>					right_positions;
	vector<float>					right_rotations;
	vector<float>					top_positions;
	vector<float>					top_rotations;
	vector<float>					bottom_positions;
	vector<float>					bottom_rotations;
	vector<GLuint>					vboIds;
	vector<GLuint>					ivboIds;
	vector<unsigned int>			vbo_indices;
	vector<Vertex>					v_front;
	vector<Vertex>					v_back;
	vector<Vertex>					v_left;
	vector<Vertex>					v_right;
	vector<Vertex>					v_top;
	vector<Vertex>					v_bottom;
	unsigned int					id;
	unsigned int					vertexCounts[6];
	unsigned int					texture_weights[6];
	unsigned int					texture_bump_weights[6];
	unsigned int					texture_spec_weights[6];
	GlslManager*					glsl_manager;
	GlErrorManager*					err_manager;
	VboManager*						vbo_manager;
	LogManager*						log_manager;
	vector<WallBox>					front_boxes;
	vector<WallBox>					back_boxes;
	vector<WallBox>					left_boxes;
	vector<WallBox>					right_boxes;
	vector<WallBox>					top_boxes;
	vector<WallBox>					bottom_boxes;
	GLuint							skyboxList;
	float							WIDTH;
	float							HWIDTH;
	float							HEIGHT;
	float							HHEIGHT;
	float							LENGTH;
	float							HLENGTH;
	float							front_tile_width;
	float							front_tile_height;
	float							back_tile_width;
	float							back_tile_height;
	float							left_tile_width;
	float							left_tile_height;
	float							right_tile_width;
	float							right_tile_height;
	float							top_tile_width;
	float							top_tile_height;
	float							bottom_tile_width;
	float							bottom_tile_height;
	vec3							center;
private:
	void							init					( void								);
	void							gen_vbos				( void								);
	void							fill_vertex				( Vertex&			v,
															  float				loc0,
															  float				loc1,
															  float				loc2,
															  float				nor0,
															  float				nor1,
															  float				nor2,
															  float				tex0,
															  float				tex1			);
	void							fill_tangents			( Vertex&			vert1,
															  Vertex&			vert2,
															  Vertex&			vert3,
															  Vertex&			vert4			);
	vec3							findTangent				( Vertex&			v1,
															  Vertex&			v2,
															  Vertex&			v3				);
	void							renderSkyboxBVs			( void								);
	void							drawBVs					( void								);
	bool							tilesFrustumCheck		( Frustum*			frustum,
															  vector<WallBox>&	boxes			);
};

#endif

//=======================================================================================
