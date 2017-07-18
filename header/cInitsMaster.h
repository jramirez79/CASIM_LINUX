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

//=======================================================================================
//
void init_glsl_master( void )
{
	glsl_manager = new GlslManager( err_manager, log_manager );
	vector<InputShader*> input_shaders;

	string sName;
	string sVert;
	string sFrag;
	string sGeom;

	// TODO: init shaders
	// Example:
	/*
		sName = string( "vfc_lod_assignment" );
		sVert = string( "shaders/VFCLodAssigmentTexVertexCuda.glsl" );
		sGeom = string( "shaders/VFCLodAssigmentTexGeometryCuda.glsl" );
		InputShader* input_vfc_lod_ass = new InputShader( sName );
		input_vfc_lod_ass->is_transform_feedback = true;
		input_vfc_lod_ass->s_vert = sVert;
		input_vfc_lod_ass->s_geom = sGeom;
		input_vfc_lod_ass->transform_feedback_vars.push_back( str_gl_Position );
		input_vfc_lod_ass->s_uni_i["idTex"]		= 2;
		input_vfc_lod_ass->s_uni_i["positions"] = 7;
		input_vfc_lod_ass->s_ipri = GL_POINTS;
		input_vfc_lod_ass->s_opri = GL_POINTS;
		input_shaders.push_back( input_vfc_lod_ass );

	 */

	sName = string( "pass_rect" );
	sVert = string( "shaders/passthru_rect_vertex.glsl" );
	sFrag = string( "shaders/passthru_rect_fragment.glsl" );
	InputShader* input_pass_rect = new InputShader( sName );
	input_pass_rect->s_vert = sVert;
	input_pass_rect->s_frag = sFrag;
	input_pass_rect->s_uni_i["tex"] = 0;
	input_shaders.push_back( input_pass_rect );

	sName = str_depth_rect;
	sVert = string ( "shaders/passthru_rect_vertex.glsl" 	);
	sFrag = string ( "shaders/depth_rect_fragment.glsl" 	);
	InputShader* input_depth_rect = new InputShader ( sName );
	input_depth_rect->s_vert = sVert;
	input_depth_rect->s_frag = sFrag;
	input_depth_rect->s_uni_i["screen"] = 0;
	input_shaders.push_back( input_depth_rect );

	sName = str_screen_composition_rect;
	sVert = string ( "shaders/passthru_rect_vertex.glsl" 	);
	sFrag = string ( "shaders/screen_composition.frag" 	);
	InputShader* input_screen_composition_rect = new InputShader ( sName );
	input_screen_composition_rect->s_vert = sVert;
	input_screen_composition_rect->s_frag = sFrag;
	input_screen_composition_rect->s_uni_i["tex"] = 0;
	input_screen_composition_rect->s_uni_i["cmp_flg"] = 0;
	input_screen_composition_rect->s_uni_f["z_Far"] = 0.0;
	input_shaders.push_back( input_screen_composition_rect );



	err_manager->getError( "Before init shaders:" );
	if( !glsl_manager->init( input_shaders ) )
	{
		log_manager->log( LogManager::LERROR, "While initializing GLSL shaders!" );
		cleanup();
		exit( 1 );
	}



	log_manager->log( LogManager::INFORMATION, "GLSL SHADERS initialization complete." );
}
//
//=======================================================================================
//
void init_vbos_master ( void )
{
    vbo_manager = new VboManager							( 	log_manager,
    															err_manager,
    															glsl_manager 						);
/*
	vbo_manager->setInstancingLocations						(	str_instancing,
																str_positions,
																str_rotations,
																str_scales,
																str_normal,
																str_texCoord0,
																str_ViewMat4x4 						);
	vbo_manager->setInstancingCulledLocations				(	str_instancing_culled,
																str_normal,
																str_texCoord0,
																str_ViewMat4x4 						);
	vbo_manager->setInstancingCulledRiggedLocations			(	str_instancing_culled_rigged,
																str_normalV,
																str_dt,
																str_texCoord0,
																str_ViewMat4x4 						);
	vbo_manager->setInstancingCulledRiggedShadowLocations	(	str_instancing_culled_rigged_shadow,
																str_normalV,
																str_texCoord0,
																str_dt,
																str_ViewMat4x4,
																str_ProjMat4x4,
																str_ShadowMat4x4 					);
*/
	if( err_manager->getError( "After setting instancing locations:" ) != GL_NO_ERROR )
	{
		log_manager->log( LogManager::LERROR, "While initializing VBOs!" );
		cleanup();
		exit( 1 );
	}
	log_manager->log( LogManager::INFORMATION, "VBOs initialization complete." );
}
//
//=======================================================================================
//
void init_fbos_master( void )
{
	vector<InputFbo*> fbo_inputs;

	vector<InputFboTexture*> input_fbo_textures1;
	InputFboTexture* tex1 = new InputFboTexture( str_display_tex,       InputFbo::TYPICAL			);
	InputFboTexture* tex2 = new InputFboTexture( str_display_depth_tex, InputFbo::DEPTH_NO_COMPARE	);
	input_fbo_textures1.push_back( tex1 );
	input_fbo_textures1.push_back( tex2 );
	//InputFbo* fbo1 = new InputFbo( str_display_fbo, GL_TEXTURE_RECTANGLE, input_fbo_textures1, 700, 700 );
	InputFbo* fbo1 = new InputFbo( str_display_fbo, GL_TEXTURE_RECTANGLE, input_fbo_textures1, 512, 512 );
	fbo_inputs.push_back( fbo1 );


	fbo_manager = new FboManager( log_manager, glsl_manager, fbo_inputs );
	if( !fbo_manager->init() )
	{
		log_manager->log( LogManager::LERROR, "While initializing FBOs!" );
		cleanup();
		exit( 1 );
	}
	fbo_manager->report();
	log_manager->log( LogManager::INFORMATION, "FBOs initialization complete." );
}
//
//=======================================================================================
//
void init_textures_master( void )
{
    TextureManager::getInstance()->init( err_manager, log_manager );
    log_manager->log( LogManager::INFORMATION, "TEXTURES initialization complete." );
}
//
//=======================================================================================
//
void init_master( int argc, char *argv[] )
{
	Timer::getInstance()->init();

	//init_gl( argc, argv );
	init_glMaster( argc, argv );
    err_manager	= new GlErrorManager( log_manager );

	float init_cpu;
#if defined _WIN32
	DWORD start;
	Timer::getInstance()->start( start );
#elif defined __unix
	timeval start;
	Timer::getInstance()->start( start );
#endif

	log_manager->log( LogManager::INFORMATION, "Initializing LIGHTS..." );
    init_lights();
    log_manager->separator();
    log_manager->log( LogManager::INFORMATION, "Initializing TEXTURES..." );
	init_textures();
	log_manager->separator();

	log_manager->log( LogManager::INFORMATION, "Initializing GLSL SHADERS..." );
    init_glsl_master();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing VBOs..." );
    init_vbos_master();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing FBOs..." );
    init_fbos_master();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing CAMERAS..." );
    init_cameras();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing MODELS..." );
    init_models();
    log_manager->separator();


#if defined _WIN32
	init_cpu = Timer::getInstance()->stop( start );
#elif defined __unix
	init_cpu = Timer::getInstance()->stop( start );
#endif
    log_manager->file_log( LogManager::INFORMATION, "<B>CASIM</B> Initialized in <B>%08.5f</B> seconds", init_cpu/1000.0f );
    log_manager->console_log( LogManager::INFORMATION, "CASIM Initialized in %08.5f seconds", init_cpu/1000.0f );
    log_manager->separator();
}
