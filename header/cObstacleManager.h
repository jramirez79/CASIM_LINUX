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
#include "cModel3D.h"
#include "cGlslManager.h"
#include "cVboManager.h"
#include "cFboManager.h"
#include "cMDPSquareManager.h"
#include "cMDPHexagonManager.h"
#include "cLogManager.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

using namespace std;

//=======================================================================================

#ifndef __OBSTACLE_MANAGER
#define __OBSTACLE_MANAGER

class ObstacleManager
{
public:
							ObstacleManager			(	GlslManager*	_glsl_manager,
														VboManager*		_vbo_manager,
														LogManager*		_log_manager,
														float			_scene_width,
														float			_scene_depth			);
							~ObstacleManager		(	void									);

	void					init					(	string&			_mdp_csv_file,
														FboManager*		fbo_manager				);
	void					draw					(	float*			view_mat				);
	void					moveCursorUp			(	void									);
	void					moveCursorDown			(	void									);
	void					moveCursorLeft			(	void									);
	void					moveCursorRight			(	void									);
	void					toggleObstacle			(	void									);
	void					toggleExit				(	void									);
	void					initStructuresOnHost	(	void									);
	void					initPermsOnDevice		(	void									);
	void					uploadToDevice			(	void									);
	void					iterateOnDevice			(	void									);
	void					downloadToHost			(	void									);
	void					updatePolicy			(	void									);
	void					updateDensity			(	vector<float>&	density					);

	unsigned int			getSceneWidthInTiles	(	void									);
	unsigned int			getSceneDepthInTiles	(	void									);
	unsigned int			getPolicyTextureId		(	void									);
	unsigned int			getDensityTextureId		(	void									);
	unsigned int			getArrowsTextureId		(	void									);
	unsigned int			getLayer0TextureId		(	void									);
	unsigned int			getLayer1TextureId		(	void									);
	unsigned int			getMdpTexCoordsId		(	void									);
	unsigned int			getMdpTexCoordsSize		(	void									);
	vector<float>&			getPolicy				(	void									);
	MDP_MACHINE_STATE&		getState				(	void									);

private:
	void					initObstacles			(	void									);
	void					initCursor				(	void									);
	void					initMDP					(	void									);
	void					initMdpTexCoords		(	FboManager*		fbo_manager				);
	void					drawCursor				(	void									);

	GlslManager*			glsl_manager;
	VboManager*				vbo_manager;
	LogManager*				log_manager;
//->CURSOR
	Model3D*				cursor_obj;
	glm::vec3				cursor_scale;
	glm::vec3				cursor_position;
	unsigned int			scene_width_in_tiles;
	unsigned int			scene_depth_in_tiles;
	float					scene_width;
	float					scene_depth;
	float					tile_width;
	float					tile_depth;
	float*					cursor_tint;
	float					R;
	float					S;
	float					W;
	float					H;
	unsigned int			cursor_row;
	unsigned int			cursor_col;
	unsigned int			cursor_index;
//<-CURSOR
//->OBSTACLE
	Model3D*				obstacle_obj;
	unsigned int			obstacle_tex;
	vector<float>			obstacle_pos;
	vector<float>			obstacle_rot;
	vector<float>			obstacle_scale;
//<-OBSTACLE
//->MDP
	unsigned int			NQ;
#ifdef MDP_SQUARE
	MDPSquareManager*		mdp_manager;
#elif defined MDP_HEXAGON
	MDPHexagonManager*		mdp_manager;
#endif
	string					mdp_csv_file;
	vector<float>			policy;
	unsigned int			policy_tex;
	unsigned int			density_tex;
	unsigned int			layer0_tex;
	unsigned int			layer1_tex;
	unsigned int			mdp_tc_index;
	unsigned int			mdp_tc_frame;
	unsigned int			mdp_tc_size;
	unsigned int			mdpTexCoords;
	unsigned int			arrows_tex;
	MDP_MACHINE_STATE		status;
	vector<float>			mdp_topology;
//<-MDP
	string					str_amb;
	string					str_textured;
	string					str_tint;
};

#endif

//=======================================================================================
