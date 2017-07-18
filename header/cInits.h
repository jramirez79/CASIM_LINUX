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
void init_glsl( void )
{
	glsl_manager = new GlslManager( err_manager, log_manager );
	vector<InputShader*> input_shaders;

	string sName;
	string sVert;
	string sFrag;
	string sGeom;

	sName = string( "lambert" );
	sVert = string( "shaders/lambert.vert" );
	sFrag = string( "shaders/lambert.frag" );
	InputShader* input_lambert = new InputShader( sName );
	input_lambert->s_vert = sVert;
	input_lambert->s_frag = sFrag;
	input_lambert->s_uni_i["diffuseTexture"] = 0;
    input_lambert->s_uni_f["amb"] = 0.12f;
    input_lambert->s_uni_i["textured"] = 1;
	input_shaders.push_back( input_lambert );

	sName = string( "gouraud" );
	sVert = string( "shaders/gouraud.vert" );
	sFrag = string( "shaders/gouraud.frag" );
	InputShader* input_gouraud = new InputShader( sName );
	input_gouraud->s_vert = sVert;
	input_gouraud->s_frag = sFrag;
	input_gouraud->s_uni_i["diffuseTexture"] = 0;
	input_shaders.push_back( input_gouraud );

#ifdef MDP_SQUARE
	sName = string( "mdp_floor" );
	sVert = string( "shaders/mdp_floor_vertex.glsl" );
	sFrag = string( "shaders/mdp_floor_fragment.glsl" );
	InputShader* input_mdp_floor = new InputShader( sName );
	input_mdp_floor->s_vert = sVert;
	input_mdp_floor->s_frag = sFrag;
	input_mdp_floor->s_uni_i["policy"]			= 0;
	input_mdp_floor->s_uni_i["density"]			= 1;
	input_mdp_floor->s_uni_i["arrowsTA"]		= 2;
	input_mdp_floor->s_uni_i["layer0"]			= 3;
	input_mdp_floor->s_uni_i["layer1"]			= 4;
	input_mdp_floor->s_uni_f["tile_width"]		= 100.0f;
	input_mdp_floor->s_uni_f["tile_height"]		= 100.0f;
	input_mdp_floor->s_uni_f["fbo_width"]		= 1000.0f;
	input_mdp_floor->s_uni_f["fbo_height"]		= 1000.0f;
	input_mdp_floor->s_uni_f["policy_width"]	= 10.0f;
	input_mdp_floor->s_uni_f["policy_height"]	= 10.0f;
	input_shaders.push_back( input_mdp_floor );
#elif defined MDP_HEXAGON
	sName = string( "mdp_floor" );
	sVert = string( "shaders/mdp_floor_hex_vertex.glsl" );
	sFrag = string( "shaders/mdp_floor_hex_fragment.glsl" );
	InputShader* input_mdp_floor				= new InputShader( sName );
	input_mdp_floor->s_vert						= sVert;
	input_mdp_floor->s_frag						= sFrag;
	input_mdp_floor->s_uni_i["policy"]			= 0;
	input_mdp_floor->s_uni_i["density"]			= 1;
	input_mdp_floor->s_uni_i["arrowsTA"]		= 2;
	input_mdp_floor->s_uni_i["layer0"]			= 3;
	input_mdp_floor->s_uni_i["layer1"]			= 4;
	//input_mdp_floor->s_uni_f["tile_width"]	= 115.470053838f;	// W = 2*R			R =57.7350269f
	input_mdp_floor->s_uni_f["tile_side"]		= 86.6025404f;		// S = (3/2)*R
	input_mdp_floor->s_uni_f["tile_height"]		= 100.0f;			// H = SQRT(3)*R;	H = 2*R*sin(60)
	input_mdp_floor->s_uni_f["fbo_width"]		= 1000.0f;
	input_mdp_floor->s_uni_f["fbo_height"]		= 1000.0f;
	input_mdp_floor->s_uni_f["policy_width"]	= 10.0f;
	input_mdp_floor->s_uni_f["policy_height"]	= 10.0f;
	input_shaders.push_back( input_mdp_floor );
#endif

	sName = string( "instancing" );
	sVert = string( "shaders/instancing.vert" );
	sFrag = string( "shaders/instancing.frag" );
	InputShader* input_instancing = new InputShader( sName );
	input_instancing->s_vert = sVert;
	input_instancing->s_frag = sFrag;
	input_instancing->s_uni_i["diffuseTexture"] = 0;
	input_shaders.push_back( input_instancing );

	sName = string( "tbo" );
	sVert = string( "shaders/drawTextureBufferVertex.glsl" );
	sFrag = string( "shaders/drawTextureBufferFragment.glsl" );
	InputShader* input_tbo = new InputShader( sName );
	input_tbo->s_vert = sVert;
	input_tbo->s_frag = sFrag;
	input_tbo->s_uni_i["posTextureBuffer"]	= 0;
	input_tbo->s_uni_f["PLANE_SCALE"]		= PLANE_SCALE;
	input_shaders.push_back( input_tbo );

	sName = string( "instancing_culled" );
	sVert = string( "shaders/instancing_culled.vert" );
	sFrag = string( "shaders/instancing_culled.frag" );
	InputShader* input_instancing_culled = new InputShader( sName );
	input_instancing_culled->s_vert = sVert;
	input_instancing_culled->s_frag = sFrag;
	input_instancing_culled->s_uni_i["diffuseTexture"] = 0;
	input_instancing_culled->s_uni_i["posTextureBuffer"] = 1;
	input_shaders.push_back( input_instancing_culled );

#if defined CUDA_PATHS || defined OSC_PATHS || defined MPI_PATHS || defined MPI_PATHS_ON_NODES
#ifdef DEMO_SHADER
	err_manager->getError ("BEGIN: instancing_culled_rigged");
	sName = string( "instancing_culled_rigged" );
	sVert = string( "shaders/instancing_culled_rigged_demo_cuda.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_demo_cuda.frag" );
	InputShader* input_instancing_culled_rigged = new InputShader( sName );
	input_instancing_culled_rigged->s_vert = sVert;
	input_instancing_culled_rigged->s_frag = sFrag;

	input_instancing_culled_rigged->s_uni_i["globalMT"]					= 0;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["torsoMT"]					= 1;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["legsMT"]					= 2;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["riggingMT"]				= 3;		// rigging multi texture.
	input_instancing_culled_rigged->s_uni_i["animationMT"]				= 4;		// animation multi texture.
	input_instancing_culled_rigged->s_uni_i["facialMT"]					= 5;		// facial multi texture.
	input_instancing_culled_rigged->s_uni_i["idsTextureBuffer"]			= 6;
	input_instancing_culled_rigged->s_uni_i["posTextureBuffer"]			= 7;
	input_instancing_culled_rigged->s_uni_i["color_table"]				= 10;
	input_instancing_culled_rigged->s_uni_i["pattern_table"]			= 11;

	input_instancing_culled_rigged->s_uni_i["AGENTS_NPOT"]				= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged->s_uni_i["ANIMATION_LENGTH"]			= 0;
	input_instancing_culled_rigged->s_uni_i["STEP"]						= 0;
	input_instancing_culled_rigged->s_uni_f["lod"]						= 0.0f;

	input_instancing_culled_rigged->s_uni_f["doHeightAndDisplacement"]	= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doColor"]					= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doPatterns"]				= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doFacial"]					= 0.0f;
	input_instancing_culled_rigged->s_uni_f["zNear"]					= 100.0f;
	input_instancing_culled_rigged->s_uni_f["zFar"]						= 10000.0f;

	input_shaders.push_back( input_instancing_culled_rigged );
	err_manager->getError ("END: instancing_culled_rigged");
#else
	err_manager->getError ("BEGIN: instancing_culled_rigged");
	sName = string( "instancing_culled_rigged" );
	sVert = string( "shaders/instancing_culled_rigged_demo.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_demo.frag" );
	InputShader* input_instancing_culled_rigged = new InputShader( sName );
	input_instancing_culled_rigged->s_vert = sVert;
	input_instancing_culled_rigged->s_frag = sFrag;

	input_instancing_culled_rigged->s_uni_i["globalMT"]				= 0;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["torsoMT"]				= 1;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["legsMT"]				= 2;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["riggingMT"]			= 3;		// rigging multi texture.
	input_instancing_culled_rigged->s_uni_i["animationMT"]			= 4;		// animation multi texture.
	input_instancing_culled_rigged->s_uni_i["facialMT"]				= 5;		// facial multi texture.
	input_instancing_culled_rigged->s_uni_i["posTextureBuffer"]		= 6;
	input_instancing_culled_rigged->s_uni_i["posTexture"]			= 7;
	input_instancing_culled_rigged->s_uni_i["color_table"]			= 10;
	input_instancing_culled_rigged->s_uni_i["pattern_table"]		= 11;

	input_instancing_culled_rigged->s_uni_i["AGENTS_NPOT"]			= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged->s_uni_i["ANIMATION_LENGTH"]		= 0;
	input_instancing_culled_rigged->s_uni_i["STEP"]					= 0;
	input_instancing_culled_rigged->s_uni_f["lod"]					= 0.0f;
	input_shaders.push_back( input_instancing_culled_rigged );
	err_manager->getError ("END: instancing_culled_rigged");
#endif
#else
#ifdef DEMO_SHADER
	err_manager->getError ("BEGIN: instancing_culled_rigged");
	sName = string( "instancing_culled_rigged" );
	sVert = string( "shaders/instancing_culled_rigged_demo.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_demo.frag" );
	InputShader* input_instancing_culled_rigged = new InputShader( sName );
	input_instancing_culled_rigged->s_vert = sVert;
	input_instancing_culled_rigged->s_frag = sFrag;

	input_instancing_culled_rigged->s_uni_i["globalMT"]					= 0;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["torsoMT"]					= 1;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["legsMT"]					= 2;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["riggingMT"]				= 3;		// rigging multi texture.
	input_instancing_culled_rigged->s_uni_i["animationMT"]				= 4;		// animation multi texture.
	input_instancing_culled_rigged->s_uni_i["facialMT"]					= 5;		// facial multi texture.
	input_instancing_culled_rigged->s_uni_i["posTextureBuffer"]			= 6;
	input_instancing_culled_rigged->s_uni_i["posTexture"]				= 7;
	input_instancing_culled_rigged->s_uni_i["color_table"]				= 10;
	input_instancing_culled_rigged->s_uni_i["pattern_table"]			= 11;

	input_instancing_culled_rigged->s_uni_i["AGENTS_NPOT"]				= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged->s_uni_i["ANIMATION_LENGTH"]			= 0;
	input_instancing_culled_rigged->s_uni_i["STEP"]						= 0;
	input_instancing_culled_rigged->s_uni_f["lod"]						= 0.0f;

	input_instancing_culled_rigged->s_uni_f["doHeightAndDisplacement"]	= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doColor"]					= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doPatterns"]				= 0.0f;
	input_instancing_culled_rigged->s_uni_f["doFacial"]					= 0.0f;

	input_shaders.push_back( input_instancing_culled_rigged );
	err_manager->getError ("END: instancing_culled_rigged");
#else
	err_manager->getError ("BEGIN: instancing_culled_rigged");
	sName = string( "instancing_culled_rigged" );
	sVert = string( "shaders/instancing_culled_rigged.vert" );
	sFrag = string( "shaders/instancing_culled_rigged.frag" );
	InputShader* input_instancing_culled_rigged = new InputShader( sName );
	input_instancing_culled_rigged->s_vert = sVert;
	input_instancing_culled_rigged->s_frag = sFrag;

	input_instancing_culled_rigged->s_uni_i["globalMT"]				= 0;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["torsoMT"]				= 1;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["legsMT"]				= 2;		// clothing multi texture.
	input_instancing_culled_rigged->s_uni_i["riggingMT"]			= 3;		// rigging multi texture.
	input_instancing_culled_rigged->s_uni_i["animationMT"]			= 4;		// animation multi texture.
	input_instancing_culled_rigged->s_uni_i["facialMT"]				= 5;		// facial multi texture.
	input_instancing_culled_rigged->s_uni_i["posTextureBuffer"]		= 6;
	input_instancing_culled_rigged->s_uni_i["posTexture"]			= 7;
	input_instancing_culled_rigged->s_uni_i["color_table"]			= 10;
	input_instancing_culled_rigged->s_uni_i["pattern_table"]		= 11;

	input_instancing_culled_rigged->s_uni_i["AGENTS_NPOT"]			= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged->s_uni_i["ANIMATION_LENGTH"]		= 0;
	input_instancing_culled_rigged->s_uni_i["STEP"]					= 0;
	input_instancing_culled_rigged->s_uni_f["lod"]					= 0.0f;
	input_shaders.push_back( input_instancing_culled_rigged );
	err_manager->getError ("END: instancing_culled_rigged");
#endif
#endif

#if defined CUDA_PATHS || defined OSC_PATHS || defined MPI_PATHS || defined MPI_PATHS_ON_NODES
	err_manager->getError ("BEGIN: instancing_culled_rigged_shadow");
	sName = string( "instancing_culled_rigged_shadow" );
	sVert = string( "shaders/instancing_culled_rigged_shadow_cuda.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_shadow_cuda.frag" );
	InputShader* input_instancing_culled_rigged_shadow = new InputShader( sName );
	input_instancing_culled_rigged_shadow->s_vert = sVert;
	input_instancing_culled_rigged_shadow->s_frag = sFrag;

	input_instancing_culled_rigged_shadow->s_uni_i["riggingMT"]			= 3;		// rigging multi texture.
	input_instancing_culled_rigged_shadow->s_uni_i["animationMT"]		= 4;		// animation multi texture.
	input_instancing_culled_rigged_shadow->s_uni_i["idsTextureBuffer"]	= 6;
	input_instancing_culled_rigged_shadow->s_uni_i["posTextureBuffer"]	= 7;

	input_instancing_culled_rigged_shadow->s_uni_i["AGENTS_NPOT"]		= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged_shadow->s_uni_i["ANIMATION_LENGTH"]	= 0;
	input_instancing_culled_rigged_shadow->s_uni_i["STEP"]				= 0;
	input_instancing_culled_rigged_shadow->s_uni_f["lod"]				= 0.0f;
	input_instancing_culled_rigged_shadow->s_uni_f["lod"]				= 0.0f;
	input_instancing_culled_rigged->s_uni_f["zNear"]					= 100.0f;
	input_instancing_culled_rigged->s_uni_f["zFar"]						= 10000.0f;


	input_shaders.push_back( input_instancing_culled_rigged_shadow );
	err_manager->getError ("END: instancing_culled_rigged_shadow");
#else
	err_manager->getError ("BEGIN: instancing_culled_rigged_shadow");
	sName = string( "instancing_culled_rigged_shadow" );
	sVert = string( "shaders/instancing_culled_rigged_shadow.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_shadow.frag" );
	InputShader* input_instancing_culled_rigged_shadow = new InputShader( sName );
	input_instancing_culled_rigged_shadow->s_vert = sVert;
	input_instancing_culled_rigged_shadow->s_frag = sFrag;

	input_instancing_culled_rigged_shadow->s_uni_i["riggingMT"]			= 3;		// rigging multi texture.
	input_instancing_culled_rigged_shadow->s_uni_i["animationMT"]		= 4;		// animation multi texture.
	input_instancing_culled_rigged_shadow->s_uni_i["posTextureBuffer"]	= 6;
	input_instancing_culled_rigged_shadow->s_uni_i["posTexture"]		= 7;

	input_instancing_culled_rigged_shadow->s_uni_i["AGENTS_NPOT"]		= 0;		//AGENTS_NPOT;
	input_instancing_culled_rigged_shadow->s_uni_i["ANIMATION_LENGTH"]	= 0;
	input_instancing_culled_rigged_shadow->s_uni_i["STEP"]				= 0;
	input_instancing_culled_rigged_shadow->s_uni_f["lod"]				= 0.0f;
	input_shaders.push_back( input_instancing_culled_rigged_shadow );
	err_manager->getError ("END: instancing_culled_rigged_shadow");
#endif

	sName = string( "instancing_culled_rigged_low" );
	sVert = string( "shaders/instancing_culled_rigged_low.vert" );
	sFrag = string( "shaders/instancing_culled_rigged_low.frag" );
	InputShader* input_instancing_culled_rigged_low = new InputShader( sName );
	input_instancing_culled_rigged_low->s_vert = sVert;
	input_instancing_culled_rigged_low->s_frag = sFrag;
	input_instancing_culled_rigged_low->s_uni_i["diffuseTexture"]	= 0;
	input_instancing_culled_rigged_low->s_uni_i["zonesTexture"]		= 1;
	input_instancing_culled_rigged_low->s_uni_i["weightsTexture"]	= 2;
	input_instancing_culled_rigged_low->s_uni_i["posTextureBuffer"] = 3;
	input_shaders.push_back( input_instancing_culled_rigged_low );

#if defined CUDA_PATHS || defined OSC_PATHS || defined MPI_PATHS || defined MPI_PATHS_ON_NODES
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
#else
	sName = string( "vfc_lod_assignment" );
	sVert = string( "shaders/VFCLodAssigmentTexVertex.glsl" );
	sGeom = string( "shaders/VFCLodAssigmentTexGeometry.glsl" );
	InputShader* input_vfc_lod_ass = new InputShader( sName );
	input_vfc_lod_ass->is_transform_feedback = true;
	input_vfc_lod_ass->s_vert = sVert;
	input_vfc_lod_ass->s_geom = sGeom;
	input_vfc_lod_ass->transform_feedback_vars.push_back( str_gl_Position );
	input_vfc_lod_ass->s_uni_i["positions"] = 0;
	input_vfc_lod_ass->s_uni_i["idTex"]		= 2;
	input_vfc_lod_ass->s_ipri = GL_POINTS;
	input_vfc_lod_ass->s_opri = GL_POINTS;
	input_shaders.push_back( input_vfc_lod_ass );
#endif

	sName = string( "vfc_lod_selection" );
	sVert = string( "shaders/VFCLodAssigmentTexVertex.glsl" );
	sGeom = string( "shaders/lodSelectionGeometry.glsl" );
	InputShader* input_vfc_lod_sel = new InputShader( sName );
	input_vfc_lod_sel->is_transform_feedback = true;
	input_vfc_lod_sel->s_vert = sVert;
	input_vfc_lod_sel->s_geom = sGeom;
	input_vfc_lod_sel->transform_feedback_vars.push_back( str_gl_Position );
	input_vfc_lod_sel->s_ipri = GL_POINTS;
	input_vfc_lod_sel->s_opri = GL_POINTS;
	input_shaders.push_back( input_vfc_lod_sel );

	sName = str_pass_rect;
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
	input_depth_rect->s_uni_i["tex"] = 0;
	input_shaders.push_back( input_depth_rect );


	sName = string( "pass_2d" );
	sVert = string( "shaders/passthru_2d_vertex.glsl" );
	sFrag = string( "shaders/passthru_2d_fragment.glsl" );
	InputShader* input_pass_2d = new InputShader( sName );
	input_pass_2d->s_vert = sVert;
	input_pass_2d->s_frag = sFrag;
	input_pass_2d->s_uni_i["tex"] = 0;
	input_shaders.push_back( input_pass_2d );

	sName = string( "clothing_coordinate" );
	sVert = string( "shaders/passthru_2d_vertex.glsl" );
	sFrag = string( "shaders/cloth_coordinate.frag" );
	InputShader* input_cloth_coordinate = new InputShader( sName );
	input_cloth_coordinate->s_vert = sVert;
	input_cloth_coordinate->s_frag = sFrag;
	input_cloth_coordinate->s_uni_i["pattern_tex"]		= 0;
	input_cloth_coordinate->s_uni_i["coordinate_tex"]	= 1;
	input_cloth_coordinate->s_uni_i["wrinkle_tex"]		= 2;
	input_shaders.push_back( input_cloth_coordinate );

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
void init_vbos( void )
{
    vbo_manager = new VboManager							( 	log_manager,
    															err_manager,
    															glsl_manager 						);

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
void init_crowds( void )
{
	crowd_manager	= new CrowdManager( log_manager,
										err_manager,
										fbo_manager,
										vbo_manager,
										glsl_manager,
										str_crowd_xml );

	if( crowd_manager->loadAssets() == false )
	{
		log_manager->file_log( LogManager::LERROR, "While loading crowd assets!" );
		cleanup();
		exit( 1 );
	}
	log_manager->log( LogManager::INFORMATION, "CROWDS initialization complete." );
}
//
//=======================================================================================
//
void init_fbos( void )
{
	vector<InputFbo*> fbo_inputs;

	vector<InputFboTexture*> input_fbo_mdp;
	InputFboTexture* mdp_tex1 = new InputFboTexture( str_mdp_tex, InputFbo::GPGPU );
	input_fbo_mdp.push_back( mdp_tex1 );
	InputFbo* fbo_mdp = new InputFbo( str_mdp_fbo, GL_TEXTURE_RECTANGLE, input_fbo_mdp, 1000, 1000 );
	fbo_inputs.push_back( fbo_mdp );

	vector<InputFboTexture*> input_fbo_textures1;
	InputFboTexture* tex1 = new InputFboTexture( str_display_tex,       InputFbo::TYPICAL			);
	InputFboTexture* tex2 = new InputFboTexture( str_display_depth_tex, InputFbo::DEPTH_NO_COMPARE	);
	input_fbo_textures1.push_back( tex1 );
	input_fbo_textures1.push_back( tex2 );
	InputFbo* fbo1 = new InputFbo( str_display_fbo, GL_TEXTURE_RECTANGLE, input_fbo_textures1, 512, 512 );
	fbo_inputs.push_back( fbo1 );



	vector<Crowd*> crowds = crowd_manager->getCrowds();
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		vector<InputFbo*> crowdInputs;
		Crowd* crowd = crowds[c];
		crowd->initFboInputs( crowdInputs );
		for( unsigned int ci = 0; ci < crowdInputs.size(); ci++ )
		{
			fbo_inputs.push_back( crowdInputs[ci] );
		}
	}

	fbo_manager = new FboManager( log_manager, glsl_manager, fbo_inputs );
	if( !fbo_manager->init() )
	{
		log_manager->log( LogManager::LERROR, "While initializing FBOs!" );
		cleanup();
		exit( 1 );
	}
	crowd_manager->setFboManager( fbo_manager );

	GLuint  agents_offset	= 0;
	GLuint  group_id		= 0;
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		Crowd* crowd = crowds[c];
		//TODO: check crowd->init_ids( group_id, agents_offset );
		crowd->init_ids( group_id, agents_offset );
	}
	fbo_manager->report();
	log_manager->log( LogManager::INFORMATION, "FBOs initialization complete." );
}
//
//=======================================================================================
//
void init_textures( void )
{
    TextureManager::getInstance()->init( err_manager, log_manager );
    log_manager->log( LogManager::INFORMATION, "TEXTURES initialization complete." );
}
//
//=======================================================================================
//
void init_skybox( void )
{
	string skybox_front	( "assets/skybox/rays_north.dds");
	string skybox_back	( "assets/skybox/rays_south.dds");
	string skybox_left  ( "assets/skybox/rays_west.dds"	);
	string skybox_right ( "assets/skybox/rays_east.dds" );
	string skybox_top   ( "assets/skybox/rays_up.dds"	);
	//string skybox_bottom( "assets/skybox/rays_down.dds"	);
	string skybox_bottom( "assets/skybox/blank.dds"	);

	vector<bool> skb_bump;
	skb_bump.push_back( false );
	skb_bump.push_back( false );
	skb_bump.push_back( false );
	skb_bump.push_back( false );
	skb_bump.push_back( false );
	skb_bump.push_back( false );
	vector<float> skb_tile;
	skb_tile.push_back( 1.0f );		//FRONT
	skb_tile.push_back( 1.0f );		//BACK
	skb_tile.push_back( 1.0f );		//LEFT
	skb_tile.push_back( 1.0f );		//RIGHT
	skb_tile.push_back( 1.0f );		//TOP
	skb_tile.push_back( 1.0f );		//BOTTOM
	glm::vec3 vDisp( 0.0f, PLANE_SCALE / 6.0f, 0.0f );
	glm::vec3 vDims( 5.0f * PLANE_SCALE,
					 5.0f * PLANE_SCALE,
					 5.0f * PLANE_SCALE );
	skybox_manager = new SkyboxManager( 1,
										glsl_manager,
										vbo_manager,
										log_manager,
										err_manager,
										vDisp,
										vDims,
										skb_bump,
										skb_tile,
										false,
										false 		);
	bool skb_tex =
	skybox_manager->LoadSkyboxTextures( skybox_front,
										GL_REPLACE,
										skybox_back,
										GL_REPLACE,
										skybox_left,
										GL_REPLACE,
										skybox_right,
										GL_REPLACE,
										skybox_top,
										GL_REPLACE,
										skybox_bottom,
										GL_REPLACE		);
	if( skb_tex == false )
	{
		log_manager->log( LogManager::LERROR, "While loading Skybox textures!" );
		cleanup();
		exit( 1 );
	}
	log_manager->log( LogManager::INFORMATION, "MODEL::Skybox ready." );
}
//
//=======================================================================================
//
void init_models( void )
{
//->AXES
    axes = new Axes( 100.0f );
    log_manager->log( LogManager::INFORMATION, "MODEL::Axes ready." );
//<-AXES

//->SKYBOX
    init_skybox();
//<-SKYBOX
    log_manager->log( LogManager::INFORMATION, "MODELS initialization complete." );
}
//
//=======================================================================================
//
#include "cShadows.h"
void init_lights( void )
{
	float global_ambient[]	= {    0.2f,	  0.2f,		0.2f,	1.0f };

	float position0[]		= {    PLANE_SCALE,	PLANE_SCALE*2.0f, PLANE_SCALE,	1.0f };
	shadowMatrix( position0 );
	log_manager->log( LogManager::CONFIGURATION, 	"Shadow Matrix at position (%.3f, %.3f, %.3f) ready.",
													position0[0],
													position0[1],
													position0[2] 										);
	float diffuse0[]		= {    1.0f,	  1.0f,		1.0f,	1.0f };
	float specular0[]		= {    0.4f,	  0.4f,		0.4f,	1.0f };
	float ambient0[]		= {    0.2f,	  0.2f,		0.2f,	1.0f };

	float position1[]		= {   20.0f,    100.0f,	  500.0f,	0.0f };
	float diffuse1[]		= {    1.0f,	  1.0f,		1.0f,	1.0f };
	float specular1[]		= {    0.4f,	  0.4f,		0.4f,	1.0f };
	float ambient1[]		= {    0.2f,	  0.2f,		0.2f,	1.0f };

	float* position2;
	position2				= new float[4];
	position2[0]			=  PLANE_SCALE * 10.0f;
	position2[1]			=  PLANE_SCALE;
	position2[2]			=  PLANE_SCALE * 10.0f;
	position2[3]			=  1.0f;
	float diffuse2[]		= {    1.0f,	  1.0f,		1.0f,	1.0f };
	float specular2[]		= {    0.4f,	  0.4f,		0.4f,	1.0f };
	float ambient2[]		= {    0.2f,	  0.2f,		0.2f,	1.0f };

	float* position3;
	position3				= new float[4];
	position3[0]			= -PLANE_SCALE * 10.0f;
	position3[1]			=  PLANE_SCALE;
	position3[2]			=  PLANE_SCALE * 10.0f;
	position3[3]			=  1.0f;
	float diffuse3[]		= {    1.0f,	  1.0f,		1.0f,	1.0f };
	float specular3[]		= {    0.4f,	  0.4f,		0.4f,	1.0f };
	float ambient3[]		= {    0.2f,	  0.2f,		0.2f,	1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, global_ambient );
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
/*
	Source: http://www.csit.parkland.edu/~dbock/Portfolio/Content/Lights.html
	GL_LIGHT_MODEL_LOCAL_VIEWER:
	GL_FALSE: infinite viewpoint - viewing vector doesn't change across surface - faster,
			  less realistic
    GL_TRUE:  local viewpoint - viewing vector changes across surface - slower,
			  more realistic
*/
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );

	glLightfv(  GL_LIGHT0,  GL_POSITION,    position0	);
	glLightfv(  GL_LIGHT0,  GL_DIFFUSE,     diffuse0	);
	glLightfv(  GL_LIGHT0,  GL_SPECULAR,    specular0	);
	glLightfv(  GL_LIGHT0,  GL_AMBIENT,     ambient0	);
	glEnable (  GL_LIGHT0		                        );

	glLightfv(  GL_LIGHT1,  GL_POSITION,    position1	);
	glLightfv(  GL_LIGHT1,  GL_DIFFUSE,     diffuse1	);
	glLightfv(  GL_LIGHT1,  GL_SPECULAR,    specular1	);
	glLightfv(  GL_LIGHT1,  GL_AMBIENT,     ambient1	);
	glEnable (  GL_LIGHT1		                        );

	glLightfv(  GL_LIGHT2,  GL_POSITION,    position2	);
	glLightfv(  GL_LIGHT2,  GL_DIFFUSE,     diffuse2	);
	glLightfv(  GL_LIGHT2,  GL_SPECULAR,    specular2	);
	glLightfv(  GL_LIGHT2,  GL_AMBIENT,     ambient2	);
	glEnable (  GL_LIGHT2		                        );

	glLightfv(  GL_LIGHT3,  GL_POSITION,    position3	);
	glLightfv(  GL_LIGHT3,  GL_DIFFUSE,     diffuse3	);
	glLightfv(  GL_LIGHT3,  GL_SPECULAR,    specular3	);
	glLightfv(  GL_LIGHT3,  GL_AMBIENT,     ambient3	);
	glEnable (  GL_LIGHT3		                        );

	glEnable (  GL_LIGHTING	                            );
	log_manager->log( LogManager::INFORMATION, "LIGHTS initialization complete." );
}
//
//=======================================================================================
//
void init_cameras( void )
{
	glm::vec3 p0;   glm::vec3 p1;   glm::vec3 p2;   glm::vec3 p3;   glm::vec3 p4;
	glm::vec3 d0;   glm::vec3 d1;   glm::vec3 d2;   glm::vec3 d3;   glm::vec3 d4;
	glm::vec3 r0;   glm::vec3 r1;   glm::vec3 r2;   glm::vec3 r3;   glm::vec3 r4;
	glm::vec3 u0;   glm::vec3 u1;   glm::vec3 u2;   glm::vec3 u3;   glm::vec3 u4;

	p0.x = -PLANE_SCALE;    p0.y =  PLANE_SCALE/2.0f;   p0.z =  PLANE_SCALE;
	r0.x =  PLANE_SCALE;    r0.y =  0.0f;               r0.z =  PLANE_SCALE;
	p1.x =  PLANE_SCALE;    p1.y =  PLANE_SCALE/2.0f;   p1.z =  PLANE_SCALE;
	r1.x =  PLANE_SCALE;    r1.y =  0.0f;               r1.z = -PLANE_SCALE;
	p2.x =  PLANE_SCALE;    p2.y =  PLANE_SCALE/2.0f;   p2.z = -PLANE_SCALE;
	r2.x = -PLANE_SCALE;    r2.y =  0.0f;               r2.z = -PLANE_SCALE;
	p3.x = -PLANE_SCALE;    p3.y =  PLANE_SCALE/2.0f;   p3.z = -PLANE_SCALE;
	r3.x = -PLANE_SCALE;    r3.y =  0.0f;               r3.z =  PLANE_SCALE;
	p4.x =  0.0f;           p4.y =  PLANE_SCALE*2.0f;   p4.z =  0.0f;
	r4.x =  1.0f;           r4.y =  0.0f;               r4.z =  0.0f;

	d0 = -p0;   d1 = -p1;   d2 = -p2;   d3 = -p3;   d4 = -p4;

	d0 = glm::normalize( d0 );  d1 = glm::normalize( d1 );
	d2 = glm::normalize( d2 );  d3 = glm::normalize( d3 );
	d4 = glm::normalize( d4 );
	r0 = glm::normalize( r0 );  r1 = glm::normalize( r1 );
	r2 = glm::normalize( r2 );  r3 = glm::normalize( r3 );
	r4 = glm::normalize( r4 );
	u0 = glm::cross( r0, d0 );  u1 = glm::cross( r1, d1 );
	u2 = glm::cross( r2, d2 );  u3 = glm::cross( r3, d3 );
	u4 = glm::cross( r4, d4 );

	predef_cam_pos.push_back( p0 ); predef_cam_pos.push_back( p1 );
	predef_cam_pos.push_back( p2 ); predef_cam_pos.push_back( p3 );
	predef_cam_pos.push_back( p4 );
	predef_cam_dir.push_back( d0 ); predef_cam_dir.push_back( d1 );
	predef_cam_dir.push_back( d2 ); predef_cam_dir.push_back( d3 );
	predef_cam_dir.push_back( d4 );
	predef_cam_up.push_back( u0 );  predef_cam_up.push_back( u1 );
	predef_cam_up.push_back( u2 );  predef_cam_up.push_back( u3 );
	predef_cam_up.push_back( u4 );

    camera1 = new Camera( 0, Camera::FREE, Frustum::RADAR );
    camera2 = new Camera( 1, Camera::FREE, Frustum::RADAR );
    camera  = camera2;

    vec3 pos1( PLANE_SCALE / 20.0f,  PLANE_SCALE / 20.0f,  -PLANE_SCALE / 100.0f );
    vec3 dir1( 0.0f, 0.0f, -1.0f );
    vec3 up1( 0.0f, 1.0f, 0.0f );
    vec3 piv1 = pos1 + dir1;

    vec3 pos2( -PLANE_SCALE / 2.0f,  PLANE_SCALE / 2.0f,  PLANE_SCALE / 2.0f );
    vec3 dir2( -pos2.x, -pos2.y, -pos2.z );
    vec3 up2( 0.0f, 1.0f, 0.0f );
    vec3 piv2 = pos2 + dir2;

    camera1->getFrustum()->setFovY( 54.43f );
    camera1->getFrustum()->setNearD( 1.0f );
    camera1->getFrustum()->setFarD( 5.0f * PLANE_SCALE );
    camera1->setEyeSeparation( 35.0f );
    camera1->setPivot( piv1 );
    camera1->setPosition( pos1 );
    camera1->setUpVec( up1 );
    camera1->setDirection( dir1 );

    camera2->getFrustum()->setFovY( 54.43f );
    camera2->getFrustum()->setNearD( 1.0f );
    camera2->getFrustum()->setFarD( 5.0f * PLANE_SCALE );
    camera2->setEyeSeparation( 35.0f );
    camera2->setPivot( piv2 );
    camera2->setPosition( pos2 );
    camera2->setUpVec( up2 );
    camera2->setDirection( dir2 );

    camNear = camera2->getFrustum()->getNearD();
    log_manager->log( LogManager::INFORMATION, "CAMERAS initialization complete." );

    float camRange[2];
    camRange[0] = camera->getFrustum()->getNearD();
    camRange[1] = camera->getFrustum()->getFarD();

	glsl_manager->activate( str_depth_rect );
		glsl_manager->setUniformfv ( str_depth_rect, "camrange", camRange, 2 );
	glsl_manager->deactivate( str_depth_rect );

}
//
//=======================================================================================
//
void init_obstacles( void )
{
	//str_mdp_csv = string( "assets/mdp/mdp_001_256x256h.csv"	);
	//str_mdp_csv = string( "assets/mdp/mdp_001_5x5h.csv"	);
	str_mdp_csv = string( "assets/mdp/mdp_001_8x8h.csv"	);
	//str_mdp_csv = string( "assets/mdp/mdp_001_10x10h.csv"	);
	//str_mdp_csv = string( "assets/mdp/mdp_001_20x20h.csv"	);

	//str_mdp_csv = string( "assets/mdp/mdp_002_20x20h.csv"	);
	//str_mdp_csv = string( "assets/mdp/mdp_002_10x10h.csv"	);
	//str_mdp_csv = string( "assets/mdp/mdp_003_10x10h.csv"	);

	//str_mdp_csv = string( "assets/mdp/mdp10x10.csv";

	obstacle_manager = new ObstacleManager( glsl_manager,
											vbo_manager,
											log_manager,
											PLANE_SCALE * 2.0f,
											PLANE_SCALE * 2.0f	);
	obstacle_manager->init( str_mdp_csv, fbo_manager );

	vector<glm::vec2> occupation;
	vector<vector<GROUP_FORMATION>> formations;
	vector<GROUP_FORMATION> formation;

	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_CIRCLE );
	/*
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_SQUARE );
	*/
	/*
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_SQUARE );
	formation.push_back( GFRM_CIRCLE );
	formation.push_back( GFRM_SQUARE );
	*/
	formations.push_back( formation );
	crowd_manager->initPaths(	PLANE_SCALE,
								obstacle_manager->getPolicy(),
								obstacle_manager->getSceneWidthInTiles(),
								obstacle_manager->getSceneDepthInTiles(),
								occupation,
								formations								);
	glsl_manager->activate( "mdp_floor" );
	{
		float tH = 1000.0f / (float)obstacle_manager->getSceneDepthInTiles();
		float tR = tH / sqrtf( 3.0f );
		float tS = 3.0f*tR/2.0f;
		glsl_manager->setUniformf( "mdp_floor", (char*)str_tile_side.c_str(),     tS);
		glsl_manager->setUniformf( "mdp_floor", (char*)str_tile_height.c_str(),   tH);
		glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_width.c_str(),  (float)obstacle_manager->getSceneWidthInTiles() );
		glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_height.c_str(), (float)obstacle_manager->getSceneDepthInTiles() );
		if( policy_floor )
		{
			glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_on.c_str(), 1.0f );
		}
		else
		{
			glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_on.c_str(), 0.0f );
		}
		if( density_floor )
		{
			glsl_manager->setUniformf( "mdp_floor", (char*)str_density_on.c_str(), 1.0f );
		}
		else
		{
			glsl_manager->setUniformf( "mdp_floor", (char*)str_density_on.c_str(), 0.0f );
		}
	}
	glsl_manager->deactivate( "mdp_floor" );
	log_manager->log( LogManager::INFORMATION, "OBSTACLES initialization complete." );
}
//
//=======================================================================================
//
void init_glMaster( int argc, char *argv[] )
{

	glutInit					( 	&argc,	argv	);
	if(!webScktsFlg){

		glutInitDisplayMode			( 	GLUT_RGBA 	|
										GLUT_DEPTH 	|
										GLUT_DOUBLE 						);
		//glutInitWindowSize			( 	700,
										//700 								);
		glutInitWindowSize			( 	512,512	);
		glutInitWindowPosition		( 	1290,
										100									);
	/*
		glutInitContextVersion		(	3,
										1									);
	*/
		#if defined MPI_PATHS_ON_NODES
			ostringstream window_title;
			window_title << "CASIM LINUX v1.0 PID " << pid;
			str_title = window_title.str ();
			glutCreateWindow        	(   (char*)str_title.c_str() 	);
		#else
			glutCreateWindow        	(   (char*)str_title.c_str() 	);
		#endif
		//	glutInitContextFlags		(	GLUT_FORWARD_COMPATIBLE		);
		//	glutInitContextProfile		(	GLUT_COMPATIBILITY_PROFILE	);
	}
	else{
		rcx.winWidth 	= WIN_WIDTH;
		rcx.winHeight	= WIN_HEIGHT;
		rcx.winX		= 0;
		rcx.winY		= 0;

		initEarlyGLXfnPointers	(	);
		CreateWindowless( &rcx, ":0.0", 4, 2 );
	}


	glewExperimental 	= GL_TRUE;
	int glew_status 	= glewInit();
    if( glew_status != GLEW_OK )
    {
		log_manager->log( LogManager::LERROR, "GLEW initialization failed!" );
		cleanup();
		exit( 1 );
    }

	glClearColor    (   195.0f/255.0f,
                        195.0f/255.0f,
                        195.0f/255.0f,
                        0.0                             );
	glShadeModel    (   GL_SMOOTH                       );
	glEnable        (   GL_DEPTH_TEST                   );
	glEnable        (   GL_TEXTURE_2D                   );
	glHint          (   GL_PERSPECTIVE_CORRECTION_HINT,
                        GL_NICEST                       );
	glEnable        (   GL_MULTISAMPLE                  );
	glEnable        (   GL_BLEND                        );
	glBlendFunc     (   GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA          );

    string gl_vdr( (const char*)glGetString     ( GL_VENDOR                     ) );
    string gl_ren( (const char*)glGetString     ( GL_RENDERER                   ) );
    string gl_ver( (const char*)glGetString     ( GL_VERSION                    ) );
    string gw_ver( (const char*)glewGetString   ( GLEW_VERSION                  ) );
    string sl_ver( (const char*)glGetString     ( GL_SHADING_LANGUAGE_VERSION   ) );

	log_manager->log( LogManager::CONTEXT, "Vendor:   %s",	    gl_vdr.c_str() );
	log_manager->log( LogManager::CONTEXT, "Renderer: %s",	    gl_ren.c_str() );
	log_manager->log( LogManager::CONTEXT, "GL:       %s",	    gl_ver.c_str() );
	log_manager->log( LogManager::CONTEXT, "GLEW:     %s",      gw_ver.c_str() );
	log_manager->log( LogManager::CONTEXT, "GLSL:     %s",      sl_ver.c_str() );
	log_manager->separator();
}
//
//=======================================================================================
//
void init_gl( int argc, char *argv[] )
{
    glutInit					( 	&argc,	argv );
    if(!webScktsFlg){
    	glutInitDisplayMode			( 	GLUT_RGBA 	|
    	    								GLUT_DEPTH 	|
    	    								GLUT_DOUBLE 						);
		glutInitWindowSize			( 	512,
										512 								);
		glutInitWindowPosition		( 	1290,
										100									);
    	/*
    	    glutInitContextVersion		(	3,
    										1									);
    	*/
    	#if defined MPI_PATHS_ON_NODES
    		ostringstream window_title;
    		window_title << "CASIM LINUX v1.0 PID " << pid;
    		str_title = window_title.str ();
    		glutCreateWindow        	(   (char*)str_title.c_str() 	);
    	#else
    		glutCreateWindow        	(   (char*)str_title.c_str() 	);
    	#endif
    	//	glutInitContextFlags		(	GLUT_FORWARD_COMPATIBLE		);
    	//	glutInitContextProfile		(	GLUT_COMPATIBILITY_PROFILE	);
    }
    else{
    	int col, row;
		col = (pid) / 3;
		row = (pid) - col * 3;

    	rcx.winWidth 	= WIN_WIDTH;
		rcx.winHeight	= WIN_HEIGHT;
		rcx.winX		= WIN_WIDTH*row;
		rcx.winY		= WIN_HEIGHT*col+100;

		initEarlyGLXfnPointers();

		if (pid%2)
			CreateWindowless	(&rcx, ":0.1", 4, 2);
		else
			CreateWindowless	(&rcx, ":0.0", 4, 2);
    }

	glewExperimental 	= GL_TRUE;
	int glew_status 	= glewInit();
    if( glew_status != GLEW_OK )
    {
		log_manager->log( LogManager::LERROR, "GLEW initialization failed!" );
		cleanup();
		exit( 1 );
    }

	glClearColor    (   195.0f/255.0f,
                        195.0f/255.0f,
                        195.0f/255.0f,
                        0.0                             );
	glShadeModel    (   GL_SMOOTH                       );
	glEnable        (   GL_DEPTH_TEST                   );
	glEnable        (   GL_TEXTURE_2D                   );
	glHint          (   GL_PERSPECTIVE_CORRECTION_HINT,
                        GL_NICEST                       );
	glEnable        (   GL_MULTISAMPLE                  );
	glEnable        (   GL_BLEND                        );
	glBlendFunc     (   GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA          );

    string gl_vdr( (const char*)glGetString     ( GL_VENDOR                     ) );
    string gl_ren( (const char*)glGetString     ( GL_RENDERER                   ) );
    string gl_ver( (const char*)glGetString     ( GL_VERSION                    ) );
    string gw_ver( (const char*)glewGetString   ( GLEW_VERSION                  ) );
    string sl_ver( (const char*)glGetString     ( GL_SHADING_LANGUAGE_VERSION   ) );

	log_manager->log( LogManager::CONTEXT, "Vendor:   %s",	    gl_vdr.c_str() );
	log_manager->log( LogManager::CONTEXT, "Renderer: %s",	    gl_ren.c_str() );
	log_manager->log( LogManager::CONTEXT, "GL:       %s",	    gl_ver.c_str() );
	log_manager->log( LogManager::CONTEXT, "GLEW:     %s",      gw_ver.c_str() );
	log_manager->log( LogManager::CONTEXT, "GLSL:     %s",      sl_ver.c_str() );
	log_manager->separator();
}
//
//=======================================================================================
//
void init( int argc, char *argv[] )
{
	Timer::getInstance()->init();

	if(webScktsFlg){
		// Next line solve the bug in Ubuntu 3.10 and Nvidia Drivers
		//	https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
			int thread=pthread_getconcurrency();
	}


	init_gl( argc, argv );

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
    init_glsl();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing VBOs..." );
    init_vbos();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing CROWDS..." );
    init_crowds();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing FBOs..." );
    init_fbos();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing CAMERAS..." );
    init_cameras();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing MODELS..." );
    init_models();
    log_manager->separator();

    log_manager->log( LogManager::INFORMATION, "Initializing OBSTACLES..." );
    init_obstacles();
    log_manager->separator();

#if defined _WIN32
	init_cpu = Timer::getInstance()->stop( start );
#elif defined __unix
	init_cpu = Timer::getInstance()->stop( start );
#endif
    log_manager->file_log( LogManager::INFORMATION, "<B>CASIM</B> Initialized in <B>%08.5f</B> seconds", init_cpu/1000.0f );
    log_manager->console_log( LogManager::INFORMATION, "CASIM Initialized in %08.5f seconds", init_cpu/1000.0f );
    log_manager->separator();

    //oscPos = new float[crowd_manager->getCrowds()[0]->getHeight()*crowd_manager->getCrowds()[0]->getWidth()*3];
    //printf( "size: %ld\n", sizeof(oscPos) );
}
//
//=======================================================================================
//
void *OSCListener (void *threadArgs)
{
	cerr << "Starting listening thread!" << endl;
	cPacketListener *listener = new cPacketListener();
    UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
            listener );
    std::cout << "OSC Client is listening...\n";
    s.RunUntilSigInt();
	cerr << "Exiting thread!" << endl;
	pthread_exit(NULL);
}
//
//=======================================================================================
//
void launchOSC ()
{
	int rc = pthread_create( &threads[0], NULL, OSCListener, NULL );
	if( rc )
	{
		log_manager->log( LogManager::NET, "Thread %d not initialized...", rc );
		cleanup();
	    exit( 1 );
	}
}
//
//=======================================================================================
//

