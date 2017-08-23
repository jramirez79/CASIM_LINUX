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
//->FLOAT
float               PLANE_SCALE             = 10000.0f;
float			    fps					    = 0.0f;
float			    delta_time			    = 0.0f;
float				time_counter			= 0.0f;
float			    prev_time			    = 0.0f;
float 				camNear					= 0.0f;
float				camAccel				= 40.0f;
float				one_sixtieth			= 1.0f / 60.0f;
float				one_thirtieth			= 1.0f / 30.0f;
float* 	 			imgCmpPtr;
//<-FLOAT
//=======================================================================================
//->INT
int				    update_frame		    = 0;
int					glut_mod				= 0;
//<-INT
//=======================================================================================
//->BOOL
bool				lMouseDown				= false;
bool				rMouseDown				= false;
bool				mMouseDown				= false;
bool			    ctrlDown		        = false;

bool				runpaths				= true;
bool				showstats				= false;
bool				animating				= true;
bool				wireframe				= false;
bool				hideCharacters			= false;
bool				drawShadows				= true;
bool				policy_floor			= true;
bool				density_floor			= true;

#ifdef DEMO_SHADER
bool				doHeightAndDisplacement	= true;
bool				doPatterns				= true;
bool				doColor					= true;
bool				doFacial				= true;
bool				webScktsFlg 			= true;

#endif
//<-BOOL
//=======================================================================================
//->UNSIGNED
unsigned long	    frame_counter		    = 0;

unsigned int		predef_cam_index		= 0;
unsigned int 		d_lod1 					= 0;
unsigned int 		d_lod2 					= 0;
unsigned int 		d_lod3 					= 0;
unsigned int 		d_total					= 0;
unsigned int		d_culled				= 0;
//->UNSIGNED
//=======================================================================================
//->POINTER
float 				view_mat[16];
float				proj_mat[16];
//<-POINTER
//=======================================================================================
//->STD
//-->VECTOR
vector<glm::vec3>	predef_cam_pos;
vector<glm::vec3>	predef_cam_dir;
vector<glm::vec3>	predef_cam_up;
//<--VECTOR
//=======================================================================================
//-->STRING
string				str_title							= string( "CASIM LINUX v1.0"				);
string				str_fps								= string( "FPS:        "					);
string				str_delta_time						= string( "DELTA TIME: "					);
string				str_culled							= string( "CULLED:     "					);
string				str_lod1							= string( "LOD1:       "					);
string				str_lod2							= string( "LOD2:       "					);
string				str_lod3							= string( "LOD3:       "					);
string				str_display_fbo						= string( "display_fbo" 					);
string				str_gl_Position						= string( "gl_Position"						);
string				str_instancing						= string( "instancing"						);
string				str_positions						= string( "positions"						);
string				str_rotations						= string( "rotations"						);
string				str_scales							= string( "scales"							);
string				str_normal							= string( "normal"							);
string				str_texCoord0						= string( "texCoord0"						);
string				str_ViewMat4x4						= string( "ViewMat4x4"						);
string				str_instancing_culled				= string( "instancing_culled"				);
string				str_instancing_culled_rigged		= string( "instancing_culled_rigged"		);
string				str_normalV							= string( "normalV"							);
string				str_dt								= string( "dt"								);
string				str_instancing_culled_rigged_shadow = string( "instancing_culled_rigged_shadow"	);
string				str_ProjMat4x4						= string( "ProjMat4x4"						);
string				str_ShadowMat4x4					= string( "ShadowMat4x4"					);
string				str_mdp_tex							= string( "mdp_tex"							);
string				str_mdp_fbo							= string( "mdp_fbo"							);
string				str_display_tex						= string( "display_tex"						);
string				str_display_depth_tex				= string( "display_depth_tex"				);
string				str_mdp_csv							= string( "" 								);
string				str_crowd_xml						= string( "Crowd.xml" 						);
string				str_pass_rect						= string( "pass_rect" 						);
string				str_depth_rect						= string( "depth_rect"						);
string				str_screen_composition_rect			= string( "composition_rect"				);
string				str_policy_on						= string( "policy_on"						);
string				str_density_on						= string( "density_on" 						);
string				str_tang							= string( "tang" 							);
string				str_nearPlane						= string( "nearPlane" 						);
string				str_farPlane						= string( "farPlane" 						);
string				str_ratio							= string( "ratio" 							);
string				str_tile_side						= string( "tile_side" 						);
string				str_tile_height						= string( "tile_height" 					);
string				str_policy_width					= string( "policy_width" 					);
string				str_policy_height					= string( "policy_height" 					);
//<--STRING
//<-STD
//=======================================================================================
//->CASIM
VboManager*         vbo_manager             = NULL;
GlslManager*        glsl_manager            = NULL;
FboManager*			fbo_manager				= NULL;
GlErrorManager*     err_manager             = NULL;
LogManager*         log_manager             = NULL;
Axes*               axes                    = NULL;
Camera*             camera1                 = NULL;
Camera*             camera2                 = NULL;
Camera*             camera                  = NULL;
SkyboxManager*		skybox_manager			= NULL;
CrowdManager*		crowd_manager			= NULL;
ObstacleManager*	obstacle_manager		= NULL;
cimg_library::CImg<float>* imageBG			= NULL;
cimg_library::CImg<float>* imageDM			= NULL;
//<-CASIM
//=======================================================================================
//->OSC
pthread_t		threads[NUM_THREADS];
pthread_t		threads2;
//<-OSC
//=======================================================================================
//
/////////////////////////////////WebSockets related variables//////////////////////////////////////////
long			frames, time_,timebase=0;
long			time_2,timebase2=0;
unsigned long	frameCount;
unsigned int*	screenTexIds 	= 0;		//store screen textures (color and depth)
#define			WIN_WIDTH		512
#define			WIN_HEIGHT		512
typedef 		float			myScreenType;
unsigned int 	pixelSizeMsg 	= 0;
bool				running			= true;		// controls the glLoop function used by workers
glm::mat4			transformMat[3];		 // tranformation matrices: [0] - Model; [1] - View; [2] - Projection
unsigned int		MVPmatUBO			= 0; // Modelview Pojection Matrix
unsigned int	passthruInstancingShader;
int				colorLoc;
RenderContext 	rcx;
//
//=======================================================================================
//
