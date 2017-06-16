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

#include "cObstacleManager.h"

//=======================================================================================
//
ObstacleManager::ObstacleManager( GlslManager*	_glsl_manager,
								  VboManager*	_vbo_manager,
								  LogManager*	_log_manager,
								  float			_scene_width,
								  float			_scene_depth	)
{
	cursor_row				= 0;
	cursor_col				= 0;
	cursor_index			= 0;
	R						= 0.0f;
	S						= 0.0f;
	W						= 0.0f;
	H						= 0.0f;
	glsl_manager			= _glsl_manager;
	vbo_manager				= _vbo_manager;
	log_manager				= _log_manager;
	scene_width				= _scene_width;
	scene_depth				= _scene_depth;
	scene_width_in_tiles	= 0;
	scene_depth_in_tiles	= 0;
	tile_width				= 0.0f;
	tile_depth				= 0.0f;
	mdp_tc_index			= 0;
	mdp_tc_frame			= 0;
	mdp_tc_size				= 0;
	layer0_tex				= 0;
	layer1_tex				= 0;
	mdpTexCoords			= 0;
	policy_tex				= 0;
	density_tex				= 0;
	arrows_tex				= 0;

#ifdef MDP_SQUARE
	mdp_manager				= new MDPSquareManager( log_manager );
	NQ						= 8;
#elif defined MDP_HEXAGON
	mdp_manager				= new MDPHexagonManager( log_manager );
	NQ						= 6;
#endif
	status					= MDPS_IDLE;

	cursor_tint				= new float[3];
	cursor_tint[0]			= 1.0f;
	cursor_tint[1]			= 0.0f;
	cursor_tint[2]			= 0.0f;

	cursor_obj				= NULL;
	obstacle_obj			= NULL;
	obstacle_tex			= 0;

	str_amb					= string( "amb" );
	str_textured			= string( "textured" );
	str_tint				= string( "tint" );
}
//
//=======================================================================================
//
ObstacleManager::~ObstacleManager( void )
{

}
//
//=======================================================================================
//
void ObstacleManager::init( string&			_mdp_csv_file,
							FboManager*		fbo_manager		)
{
	mdp_csv_file = _mdp_csv_file;
#ifdef CUDA_PATHS
	initMDP();
	initMdpTexCoords( fbo_manager );
#elif defined OSC_PATHS

#elif defined MPI_PATHS || defined MPI_PATHS_ON_NODES

#endif
	initCursor();
	initObstacles();
}
//
//=======================================================================================
//
void ObstacleManager::initMDP( void )
{
	if( mdp_manager->solve_from_csv( mdp_csv_file, policy ) )
	{
		mdp_manager->getRewards( mdp_topology );
		vector<float> policy2;
		vector<float> density;
		scene_depth_in_tiles	= mdp_manager->getRows();
		scene_width_in_tiles	= mdp_manager->getColumns();
		tile_width				= scene_width/(float)scene_width_in_tiles;
		tile_depth				= scene_depth/(float)scene_depth_in_tiles;
#ifdef MDP_SQUARE
		W						= tile_width;
		H						= tile_depth;
#elif defined MDP_HEXAGON
		H						= tile_depth;
		R						= H / sqrtf( 3.0f );
		W						= 2.0f * R;
		S						= 3.0f * R / 2.0f;
#endif
		for( int r = scene_depth_in_tiles-1; r >= 0; r-- )
		{
			for( unsigned int c = 0; c < scene_width_in_tiles; c++ )
			{
				policy2.push_back( policy[r*scene_width_in_tiles+c] );
				policy2.push_back( 0.0f );
				policy2.push_back( 0.0f );
				policy2.push_back( 0.0f );
				density.push_back( 0.0f );
				density.push_back( 0.0f );
				density.push_back( 0.0f );
				density.push_back( 0.0f );
			}
		}
		string str_policy		= string( "policy.rgba" );
		policy_tex				= TextureManager::getInstance()->loadRawTexture(	str_policy,
																					policy2,
																					GL_NEAREST,
																					scene_width_in_tiles,
																					GL_RGBA32F,
																					GL_RGBA,
																					GL_TEXTURE_RECTANGLE );
		string str_density		= string( "density.rgba" );
		density_tex				= TextureManager::getInstance()->loadRawTexture(	str_density,
																					density,
																					GL_NEAREST,
																					scene_width_in_tiles,
																					GL_RGBA32F,
																					GL_RGBA,
																					GL_TEXTURE_RECTANGLE );
		status					= MDPS_READY;
		log_manager->file_log( LogManager::OBSTACLE_MANAGER, "MDP_INITED_OK::ROWS=%d::COLS=%d::ITERATIONS=%i", scene_depth_in_tiles, scene_width_in_tiles, mdp_manager->getIterations() );
	}
	else
	{
		log_manager->file_log( LogManager::LERROR, "ERROR_SOLVING_FROM_CSV::\"%s\"", mdp_csv_file.c_str() );
		status					= MDPS_ERROR;
	}
}
//
//=======================================================================================
//
void ObstacleManager::initObstacles( void )
{
	string str_obstacle = string( "obstacle" );
	string path			= string( "assets/box" );
	string filename		= string( "box.obj" );
	string texname		= string( "assets/box/box.tga" );
#ifdef MDP_SQUARE
	path 			= string( "assets/bush_and_statue" );
	filename 		= string( "bush_and_statue.obj" );
	texname 		= string( "assets/bush_and_statue/bush_and_statue.tga" );
	obstacle_obj 	= new Model3D( str_obstacle, path, vbo_manager, glsl_manager, filename, 1.0f );
	obstacle_tex 	= TextureManager::getInstance()->loadTexture( texname, false, GL_TEXTURE_2D, GL_MODULATE );
#elif defined MDP_HEXAGON
	path 			= string( "assets/bush_and_statue_circle" );
	filename 		= string( "bush_and_statue_circle.obj" );
	texname 		= string( "assets/bush_and_statue_circle/bush_and_statue_circle.tga" );
	obstacle_obj 	= new Model3D( str_obstacle, path, vbo_manager, glsl_manager, filename, 1.0f );
	obstacle_tex 	= TextureManager::getInstance()->loadTexture( texname, false, GL_TEXTURE_2D, GL_MODULATE );
#else
	obstacle_obj 	= new Model3D( str_obstacle, path, vbo_manager, glsl_manager, filename, 1.0f );
	obstacle_tex 	= TextureManager::getInstance()->loadTexture( texname, false, GL_TEXTURE_2D, GL_MODULATE );
#endif
	obstacle_obj->init( true );

	for( int r = (int)scene_depth_in_tiles-1; r >= 0; r-- )
	{
		for( int c = 0; c < (int)scene_width_in_tiles; c++ )
		{
			float zDisp		= 0.0f;
			float xDisp		= tile_width / 2.0f;
			float col_width	= tile_width;
#ifdef MDP_HEXAGON
			if( c%2 == 1 )
			{
				zDisp	= tile_depth / 2.0f;
			}
			xDisp		= W / 2.0f;
			col_width	= S;
#endif
			if( policy[r*(int)scene_width_in_tiles+c] == (float)NQ ) //IS_WALL
			{
				obstacle_pos.push_back( ((c * col_width) - scene_width/2.0f) + xDisp );
				obstacle_pos.push_back( 0.0f );
				obstacle_pos.push_back( ((r * tile_depth) - scene_depth/2.0f) + tile_depth/2.0f + zDisp );
				obstacle_pos.push_back( 1.0f );

				float oScale = col_width + ((float)rand() / RAND_MAX) * (col_width / 10.0f);
				obstacle_scale.push_back( oScale );
				obstacle_scale.push_back( oScale );
				obstacle_scale.push_back( oScale );
				obstacle_scale.push_back( 1.0f );

				obstacle_rot.push_back( 0.0f );
				obstacle_rot.push_back( 0.0f );
				obstacle_rot.push_back( 0.0f );
				obstacle_rot.push_back( 1.0f );
			}
		}
	}
}
//
//=======================================================================================
//
void ObstacleManager::initCursor( void )
{
	string str_cursor 		= string( "cursor" );
	string str_path			= string( "assets/mdp/cursors" );
	string str_obj;
#ifdef MDP_SQUARE
	str_obj					= string( "cursor_square.obj" );
	cursor_obj				= new Model3D( str_cursor, str_path, vbo_manager, glsl_manager, str_obj, 1.0f );
	cursor_scale.x			= W;
	cursor_scale.y			= 800.0f;
	cursor_scale.z			= W;
	cursor_position.x		= ((float)cursor_col * W) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#elif defined MDP_HEXAGON
	str_obj					= string( "cursor_hexagonal.obj" );
	cursor_obj				= new Model3D( str_cursor, str_path, vbo_manager, glsl_manager, str_obj, 1.0f );
	cursor_scale.x			= W / 2.0f;
	cursor_scale.y			= 800.0f;
	cursor_scale.z			= W / 2.0f;
	cursor_position.x		= ((float)cursor_col * S) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#endif
	cursor_obj->init( true );
}
//
//=======================================================================================
//
void ObstacleManager::draw( float* view_mat )
{
	vbo_manager->render_instanced_vbo(
		obstacle_obj->getIds()[0],
		obstacle_tex,
		obstacle_obj->getSizes()[0],
		obstacle_pos.size() / 4,
		obstacle_pos.size()   * sizeof( float ),
		&obstacle_pos[0],
		obstacle_rot.size()   * sizeof( float ),
		&obstacle_rot[0],
		obstacle_scale.size() * sizeof( float ),
		&obstacle_scale[0],
		view_mat								);
	drawCursor();
}
//
//=======================================================================================
//
void ObstacleManager::drawCursor( void )
{
	glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
	{
		//glDisable( GL_LIGHTING );
		//glDisable( GL_TEXTURE_2D );
		glPushMatrix();
		{
			glTranslatef( cursor_position.x, cursor_position.y, cursor_position.z );
			glPushMatrix();
			{
				glScalef( cursor_scale.x, cursor_scale.y, cursor_scale.z );
				glsl_manager->activate( "lambert" );
				{
					glsl_manager->setUniformf( "lambert", (char*)str_amb.c_str(), 0.8f );
					glsl_manager->setUniformi( "lambert", (char*)str_textured.c_str(), 0 );
					glsl_manager->setUniformfv( "lambert", (char*)str_tint.c_str(), cursor_tint, 3 );
					vbo_manager->render_vbo( cursor_obj->getIds()[0], cursor_obj->getSizes()[0] );
				}
				glsl_manager->deactivate( "lambert" );
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopAttrib();
}
//
//=======================================================================================
//
void ObstacleManager::moveCursorUp( void )
{
	if( cursor_row > 0 )
	{
		cursor_row--;
		cursor_index = cursor_row * scene_width_in_tiles + cursor_col;
	}
#ifdef MDP_SQUARE
	cursor_position.x		= ((float)cursor_col * W) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#elif defined MDP_HEXAGON
	cursor_position.x		= ((float)cursor_col * S) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	if( cursor_col % 2 == 0 )
	{
		cursor_position.z	= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
	}
	else
	{
		cursor_position.z	= ((float)cursor_row * H) + H - scene_depth / 2.0f;
	}
#endif
}
//
//=======================================================================================
//
void ObstacleManager::moveCursorDown( void )
{
	if( (cursor_row+1) < scene_depth_in_tiles )
	{
		cursor_row++;
		cursor_index = cursor_row * scene_width_in_tiles + cursor_col;
	}
#ifdef MDP_SQUARE
	cursor_position.x		= ((float)cursor_col * W) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#elif defined MDP_HEXAGON
	cursor_position.x		= ((float)cursor_col * S) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	if( cursor_col % 2 == 0 )
	{
		cursor_position.z	= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
	}
	else
	{
		cursor_position.z	= ((float)cursor_row * H) + H - scene_depth / 2.0f;
	}
#endif
}
//
//=======================================================================================
//
void ObstacleManager::moveCursorLeft( void )
{
	if( cursor_col > 0 )
	{
		cursor_col--;
		cursor_index = cursor_row * scene_width_in_tiles + cursor_col;
	}
#ifdef MDP_SQUARE
	cursor_position.x		= ((float)cursor_col * W) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#elif defined MDP_HEXAGON
	cursor_position.x		= ((float)cursor_col * S) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	if( cursor_col % 2 == 0 )
	{
		cursor_position.z	= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
	}
	else
	{
		cursor_position.z	= ((float)cursor_row * H) + H - scene_depth / 2.0f;
	}
#endif
}
//
//=======================================================================================
//
void ObstacleManager::moveCursorRight( void )
{
	if( (cursor_col+1) < scene_width_in_tiles )
	{
		cursor_col++;
		cursor_index = cursor_row * scene_width_in_tiles + cursor_col;
	}
#ifdef MDP_SQUARE
	cursor_position.x		= ((float)cursor_col * W) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	cursor_position.z		= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
#elif defined MDP_HEXAGON
	cursor_position.x		= ((float)cursor_col * S) + (W / 2.0f) - scene_width / 2.0f;
	cursor_position.y		= 0.0f;
	if( cursor_col % 2 == 0 )
	{
		cursor_position.z	= ((float)cursor_row * H) + (H / 2.0f) - scene_depth / 2.0f;
	}
	else
	{
		cursor_position.z	= ((float)cursor_row * H) + H - scene_depth / 2.0f;
	}
#endif
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getSceneWidthInTiles( void )
{
	return scene_width_in_tiles;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getSceneDepthInTiles( void )
{
	return scene_depth_in_tiles;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getPolicyTextureId( void )
{
	return policy_tex;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getDensityTextureId( void )
{
	return density_tex;
}
//
//=======================================================================================
//
vector<float>& ObstacleManager::getPolicy( void )
{
	return policy;
}
//
//=======================================================================================
//
void ObstacleManager::initMdpTexCoords( FboManager* fbo_manager )
{
	vector<string> arrows;
	vector<GLint> params;
#ifdef MDP_SQUARE
	arrows.push_back( "assets/mdp/arrows/square/0s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/1s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/2s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/3s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/4s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/5s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/6s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/7s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/8s.tga" );
	arrows.push_back( "assets/mdp/arrows/square/9s.tga" );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
#elif defined MDP_HEXAGON
	arrows.push_back( "assets/mdp/arrows/hexagon/0h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/1h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/2h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/3h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/4h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/5h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/6h.tga" );
	arrows.push_back( "assets/mdp/arrows/hexagon/7h.tga" );

	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
	params.push_back( GL_LINEAR );
#endif
	arrows_tex = TextureManager::getInstance()->loadTexture3D( arrows, params, false, GL_TEXTURE_2D_ARRAY );

	string l0_fname = "assets/mdp/pavement2.tga";
	layer0_tex = TextureManager::getInstance()->loadTexture( l0_fname, false, GL_TEXTURE_2D, GL_REPLACE );
	string l1_fname = "assets/mdp/grass2.tga";
	layer1_tex = TextureManager::getInstance()->loadTexture( l1_fname, false, GL_TEXTURE_2D, GL_MODULATE );

	mdp_tc_index = 0;
	mdp_tc_frame = 0;
	string vbo_name = "MDP_TEX_COORDS";
	mdp_tc_index = vbo_manager->createVBOContainer( vbo_name, mdp_tc_frame );

	unsigned int i, j;
	for( i = 0; i < fbo_manager->fbos["mdp_fbo"].fbo_width; i++ )
	{
		for( j = 0; j < fbo_manager->fbos["mdp_fbo"].fbo_height; j++ )
		{
			Vertex v;
			INITVERTEX( v );
			v.location[0] = (float)i;
			v.location[1] = (float)j;
			v.location[2] = 0.0f;
			v.location[3] = 1.0f;
			v.texture[0]  = (float)i;
			v.texture[1]  = (float)j;
			vbo_manager->vbos[mdp_tc_index][mdp_tc_frame].vertices.push_back( v );
		}
	}
	vbo_manager->gen_vbo( mdpTexCoords, mdp_tc_index, mdp_tc_frame );
	mdp_tc_size = vbo_manager->vbos[mdp_tc_index][mdp_tc_frame].vertices.size();
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getArrowsTextureId( void )
{
	return arrows_tex;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getLayer0TextureId( void )
{
	return layer0_tex;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getLayer1TextureId( void )
{
	return layer1_tex;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getMdpTexCoordsId( void )
{
	return mdpTexCoords;
}
//
//=======================================================================================
//
unsigned int ObstacleManager::getMdpTexCoordsSize( void )
{
	return mdp_tc_size;
}
//
//=======================================================================================
//
void ObstacleManager::toggleObstacle( void )
{
	if( status == MDPS_READY )
	{
		if( !mdp_manager->mdp_is_exit( scene_depth_in_tiles - cursor_row - 1, cursor_col ) )
		{
			if( mdp_topology[cursor_index] != -10.0f )
			{
				mdp_topology[cursor_index] = -10.0f;
			}
			else
			{
				mdp_topology[cursor_index] = -0.04f;
			}
			obstacle_pos.clear();
			obstacle_scale.clear();
			obstacle_rot.clear();
			for( int r = (int)scene_depth_in_tiles-1; r >= 0; r-- )
			{
				for( int c = 0; c < (int)scene_width_in_tiles; c++ )
				{
					float zDisp		= 0.0f;
					float xDisp		= tile_width / 2.0f;
					float col_width	= tile_width;
#ifdef MDP_HEXAGON
					if( c%2 == 1 )
					{
						zDisp	= tile_depth / 2.0f;
					}
					xDisp		= W / 2.0f;
					col_width	= S;
#endif
					if( mdp_topology[r*(int)scene_width_in_tiles+c] == -10.0f ) //IS_WALL
					{
						obstacle_pos.push_back( ((c * col_width) - scene_width/2.0f) + xDisp );
						obstacle_pos.push_back( 0.0f );
						obstacle_pos.push_back( ((r * tile_depth) - scene_depth/2.0f) + tile_depth/2.0f + zDisp );
						obstacle_pos.push_back( 1.0f );

						float oScale = col_width + ((float)rand() / RAND_MAX) * (col_width / 10.0f);
						obstacle_scale.push_back( oScale );
						obstacle_scale.push_back( oScale );
						obstacle_scale.push_back( oScale );
						obstacle_scale.push_back( 1.0f );

						obstacle_rot.push_back( 0.0f );
						obstacle_rot.push_back( 0.0f );
						obstacle_rot.push_back( 0.0f );
						obstacle_rot.push_back( 1.0f );
					}
				}
			}
			status = MDPS_INIT_STRUCTURES_ON_HOST;
			log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_INIT_STRUCTURES_ON_HOST" );
		}
	}
}
//
//=======================================================================================
//
void ObstacleManager::toggleExit( void )
{
	if( status == MDPS_READY )
	{
		if( !mdp_manager->mdp_is_exit( scene_depth_in_tiles - cursor_row - 1, cursor_col ) )
		{
			mdp_topology[cursor_index] = 1.0f;
		}
		else
		{
			mdp_topology[cursor_index] = -0.04f;
		}
		obstacle_pos.clear();
		obstacle_scale.clear();
		obstacle_rot.clear();
		for( int r = (int)scene_depth_in_tiles-1; r >= 0; r-- )
		{
			for( int c = 0; c < (int)scene_width_in_tiles; c++ )
			{
				float zDisp		= 0.0f;
				float xDisp		= tile_width / 2.0f;
				float col_width	= tile_width;
#ifdef MDP_HEXAGON
				if( c%2 == 1 )
				{
					zDisp	= tile_depth / 2.0f;
				}
				xDisp		= W / 2.0f;
				col_width	= S;
#endif
				if( mdp_topology[r*(int)scene_width_in_tiles+c] == -10.0f ) //IS_WALL
				{
					obstacle_pos.push_back( ((c * col_width) - scene_width/2.0f) + xDisp );
					obstacle_pos.push_back( 0.0f );
					obstacle_pos.push_back( ((r * tile_depth) - scene_depth/2.0f) + tile_depth/2.0f + zDisp );
					obstacle_pos.push_back( 1.0f );

					float oScale = col_width + ((float)rand() / RAND_MAX) * (col_width / 10.0f);
					obstacle_scale.push_back( oScale );
					obstacle_scale.push_back( oScale );
					obstacle_scale.push_back( oScale );
					obstacle_scale.push_back( 1.0f );

					obstacle_rot.push_back( 0.0f );
					obstacle_rot.push_back( 0.0f );
					obstacle_rot.push_back( 0.0f );
					obstacle_rot.push_back( 1.0f );
				}
			}
		}
		status = MDPS_INIT_STRUCTURES_ON_HOST;
		log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_INIT_STRUCTURES_ON_HOST" );
	}
}
//
//=======================================================================================
//
MDP_MACHINE_STATE& ObstacleManager::getState( void )
{
	return status;
}
//
//=======================================================================================
//
void ObstacleManager::initStructuresOnHost( void )
{
	mdp_manager->init_structures_on_host( mdp_topology );
	status = MDPS_INIT_PERMS_ON_DEVICE;
	log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_INIT_PERMS_ON_DEVICE" );
}
//
//=======================================================================================
//
void ObstacleManager::initPermsOnDevice( void )
{
	mdp_manager->init_perms_on_device();
	status = MDPS_UPLOADING_TO_DEVICE;
	log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_UPLOADING_TO_DEVICE" );
}
//
//=======================================================================================
//
void ObstacleManager::uploadToDevice( void )
{
	mdp_manager->upload_to_device();
	status = MDPS_ITERATING_ON_DEVICE;
	log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_ITERATING_ON_DEVICE" );
}
//
//=======================================================================================
//
void ObstacleManager::iterateOnDevice( void )
{
	mdp_manager->iterate_on_device();
	if( mdp_manager->getConvergence() )
	{
		status = MDPS_DOWNLOADING_TO_HOST;
		log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_DOWNLOADING_TO_HOST" );
	}
	else
	{
		log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_ITERATING_ON_DEVICE" );
	}
}
//
//=======================================================================================
//
void ObstacleManager::downloadToHost( void )
{
	mdp_manager->download_to_host();
	status = MDPS_UPDATING_POLICY;
	log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_UPDATING_POLICY" );
}
//
//=======================================================================================
//
void ObstacleManager::updatePolicy( void )
{
	mdp_manager->get_policy( policy );
	vector<float> policy2;
	for( int r = scene_depth_in_tiles-1; r >= 0; r-- )
	{
		for( unsigned int c = 0; c < scene_width_in_tiles; c++ )
		{
			policy2.push_back( policy[r*scene_width_in_tiles+c] );
			policy2.push_back( 0.0f );
			policy2.push_back( 0.0f );
			policy2.push_back( 0.0f );
		}
	}
	string str_policy = string( "policy.rgba" );
	policy_tex = TextureManager::getInstance()->reloadRawTexture(	str_policy,
																	policy2,
																	GL_NEAREST,
																	scene_width_in_tiles,
																	GL_RGBA32F,
																	GL_RGBA,
																	GL_TEXTURE_RECTANGLE );

	status = MDPS_READY;
	log_manager->file_log( LogManager::OBSTACLE_MANAGER, "STATUS::MDPS_READY" );
}
//
//=======================================================================================
//
void ObstacleManager::updateDensity( vector<float>& density )
{
	vector<float> density2;
	for( int r = scene_depth_in_tiles-1; r >= 0; r-- )
	{
		for( unsigned int c = 0; c < scene_width_in_tiles; c++ )
		{
			density2.push_back( density[r*scene_width_in_tiles+c] );
			density2.push_back( 0.0f );
			density2.push_back( 0.0f );
			density2.push_back( 0.0f );
		}
	}
	string str_density = string( "density.rgba" );
	density_tex = TextureManager::getInstance()->reloadRawTexture(	str_density,
																	density2,
																	GL_NEAREST,
																	scene_width_in_tiles,
																	GL_RGBA32F,
																	GL_RGBA,
																	GL_TEXTURE_RECTANGLE );
}
//
//=======================================================================================
