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

#include "cCrowdManager.h"

//=======================================================================================
//
CrowdManager::CrowdManager( LogManager*		log_manager_,
							GlErrorManager*	err_manager_,
							FboManager*		fbo_manager_,
						    VboManager*		vbo_manager_,
							GlslManager*	glsl_manager_,
							string&			xml_file_		)
{
	log_manager		= log_manager_;
	err_manager		= err_manager_;
	fbo_manager		= fbo_manager_;
	vbo_manager		= vbo_manager_;
	glsl_manager	= glsl_manager_;
	xml_file		= xml_file_;

	crowd_parser = new CrowdParser( log_manager, (char*)xml_file.c_str() );

	vector<ParsedModelProps*> pMProps = crowd_parser->getParsed_ModelProps();
	for( unsigned int pmp = 0; pmp < pMProps.size(); pmp++ )
	{
		ParsedModelProps* pMP  = pMProps[pmp];
		ModelProps* mProps = new ModelProps( pMP->type, pMP->name, pMP->sex );
		string mpType = "";

		switch( mProps->getType() )
		{
		case MPT_CLOTHING:
			mpType = "CLOTHING";
			log_manager->file_log( LogManager::CROWD_MANAGER, "MODEL_PROPS::TYPE=\"%s\"::NAME=\"%s\"", mpType.c_str(), pMP->name.c_str() );

			clothing_props_names_map[pMP->name] = clothing_model_props.size();
			clothing_model_props.push_back( mProps );

			for( unsigned int pp = 0; pp < pMP->parsed_props.size(); pp++ )
			{
				ParsedProp* pP			= pMP->parsed_props[pp];
				ModelProp* mP			= new ModelProp();
				mP->type				= pP->type;
				mP->subtype				= pP->subtype;
				mP->name				= pP->name;
				mP->file				= pP->file;
				mP->ref_wrinkles		= NULL;
				if( pP->ref_wrinkles.length() > 0 )
				{
					mP->ref_wrinkles	= mProps->getProp( pP->ref_wrinkles );
				}
				for( unsigned int p = 0; p < pP->ref_pattern.size(); p++ )
				{
					string pRef = pP->ref_pattern[p];
					if( pRef.length() > 0 )
					{
						mP->ref_pattern.push_back( mProps->getProp( pRef ) );
					}
				}
				for( unsigned int a = 0; a < pP->ref_atlas.size(); a++ )
				{
					string aRef = pP->ref_atlas[a];
					if( aRef.length() > 0 )
					{
						mP->ref_atlas.push_back( mProps->getProp( aRef ) );
					}
				}
				mP->model3D				= NULL;
				mP->textureID			= 0;
				mP->loaded				= false;
				mProps->addProp( mP );

				string sType = "";
				switch( mP->type )
				{
				case PP_HEAD:
					sType = "HEAD";
					break;
				case PP_HAIR:
					sType = "HAIR";
					break;
				case PP_TORSO:
					sType = "TORSO";
					break;
				case PP_LEGS:
					sType = "LEGS";
					break;
				case PP_ATLAS:
					sType = "ATLAS";
					break;
				case PP_WRINKLES:
					sType = "WRINKLES";
					break;
				case PP_PATTERN:
					sType = "PATTERN";
					break;
				}
				log_manager->file_log( LogManager::CROWD_MANAGER, "PROP::TYPE=\"%s\"::NAME=\"%s\"::FILE=\"%s\"", sType.c_str(), mP->name.c_str(), mP->file.c_str() );
			}
			break;
		case MPT_FACIAL:
			mpType = "FACIAL";
			log_manager->file_log( LogManager::CROWD_MANAGER, "MODEL_PROPS::TYPE=\"%s\"::NAME=\"%s\"", mpType.c_str(), pMP->name.c_str() );

			facial_props_names_map[pMP->name] = facial_model_props.size();
			facial_model_props.push_back( mProps );

			for( unsigned int pp = 0; pp < pMP->parsed_props.size(); pp++ )
			{
				ParsedProp* pP			= pMP->parsed_props[pp];
				ModelProp* mP			= new ModelProp();
				mP->type				= pP->type;
				mP->subtype				= pP->subtype;
				mP->name				= pP->name;
				mP->file				= pP->file;
				mP->ref_wrinkles		= NULL;
				mP->model3D				= NULL;
				mP->textureID			= 0;
				mP->loaded				= false;
				mProps->addProp( mP );

				string sType = "";
				switch( mP->type )
				{
				case PP_FACIAL_WRINKLES:
					sType = "FACIAL_WRINKLES";
					break;
				case PP_FACIAL_EYE_SOCKETS:
					sType = "FACIAL_EYE_SOCKETS";
					break;
				case PP_FACIAL_SPOTS:
					sType = "FACIAL_SPOTS";
					break;
				case PP_FACIAL_BEARD:
					sType = "FACIAL_BEARD";
					break;
				case PP_FACIAL_MOUSTACHE:
					sType = "FACIAL_MOUSTACHE";
					break;
				case PP_FACIAL_MAKEUP:
					sType = "FACIAL_MAKEUP";
					break;
				}
				log_manager->file_log( LogManager::CROWD_MANAGER, "PROP::TYPE=\"%s\"::NAME=\"%s\"::FILE=\"%s\"", sType.c_str(), mP->name.c_str(), mP->file.c_str() );
			}
			break;
		case MPT_RIGGING:
			mpType = "RIGGING";
			log_manager->file_log( LogManager::CROWD_MANAGER, "MODEL_PROPS::TYPE=\"%s\"::NAME=\"%s\"", mpType.c_str(), pMP->name.c_str() );

			rigging_props_names_map[pMP->name] = rigging_model_props.size();
			rigging_model_props.push_back( mProps );

			for( unsigned int pp = 0; pp < pMP->parsed_props.size(); pp++ )
			{
				ParsedProp* pP			= pMP->parsed_props[pp];
				ModelProp* mP			= new ModelProp();
				mP->type				= pP->type;
				mP->subtype				= pP->subtype;
				mP->name				= pP->name;
				mP->file				= pP->file;
				mP->ref_wrinkles		= NULL;
				mP->model3D				= NULL;
				mP->textureID			= 0;
				mP->loaded				= false;
				mProps->addProp( mP );

				string sType = "";
				switch( mP->type )
				{
				case PP_RIGGING_ZONES:
					sType = "RIGGING_ZONES";
					break;
				case PP_RIGGING_WEIGHTS:
					sType = "RIGGING_WEIGHTS";
					break;
				case PP_RIGGING_ANIMATION:
					sType = "RIGGING_ANIMATION";
					break;
				}
				log_manager->file_log( LogManager::CROWD_MANAGER, "PROP::TYPE=\"%s\"::NAME=\"%s\"::FILE=\"%s\"", sType.c_str(), mP->name.c_str(), mP->file.c_str() );
			}
			break;
		}
		log_manager->file_log( LogManager::CROWD_MANAGER, "MODEL_PROPS::END" );
	}

	vector<ParsedGroup*> pMGroups = crowd_parser->getParsed_Groups();
	for( unsigned int pmg = 0; pmg < pMGroups.size(); pmg++ )
	{
		ParsedGroup* pMG = pMGroups[pmg];
		CharacterGroup* cGroup = new CharacterGroup(	log_manager,
														err_manager,
														pMG->name,
														pMG->sex,
														pMG->type,
														clothing_model_props[clothing_props_names_map[pMG->ref_clothing_props]],
														facial_model_props[facial_props_names_map[pMG->ref_facial_props]],
														rigging_model_props[rigging_props_names_map[pMG->ref_rigging_props]]	);
		cGroup->setOutfit	(	pMG->ref_head,
								pMG->ref_hair,
								pMG->ref_torso,
								pMG->ref_legs,
								pMG->ref_palette		);
		cGroup->setFace		(	pMG->ref_wrinkles,
								pMG->ref_eye_sockets,
								pMG->ref_spots,
								pMG->ref_beard,
								pMG->ref_moustache,
								pMG->ref_makeup			);
		cGroup->setRig		(	pMG->ref_zones,
								pMG->ref_weights,
								pMG->ref_displacement,
								pMG->ref_animation		);
		cGroup->setWeight( pMG->fat, pMG->average, pMG->thin, pMG->strong );
		cGroup->setHeight( pMG->min, pMG->max );
		group_names_map[pMG->name] = char_groups.size();
		char_groups.push_back( cGroup );
		log_manager->file_log( LogManager::CROWD_MANAGER, "GROUP::NAME=\"%s\"::CLOTHING_PROPS=\"%s\"", pMG->name.c_str(), pMG->ref_clothing_props.c_str() );
	}

	vector<ParsedCrowd*> pCrowds = crowd_parser->getParsed_Crowds();
	for( unsigned int pc = 0; pc < pCrowds.size(); pc++ )
	{
		ParsedCrowd* pC = pCrowds[pc];
		Crowd* crowd = new Crowd(	log_manager,
									err_manager,
									fbo_manager,
									vbo_manager,
									glsl_manager,
									pC->name,
									pC->width,
									pC->height	);
		log_manager->file_log( LogManager::CROWD_MANAGER, "CROWD::\"%s\"::WIDTH=%d::HEIGHT=%d", pC->name.c_str(), pC->width, pC->height );
		for( unsigned int pcg = 0; pcg < pC->groups.size(); pcg++ )
		{
			ParsedCrowdGroup* pCG = pC->groups[pcg];
#ifdef GLOBAL_POS_TEXTURE
			crowd->addGroup		(	char_groups[group_names_map[pCG->ref]],
									pCG->animation,
									pCG->percentage,
									pCG->animation_frames,
									pCG->animation_duration				);
			log_manager->file_log( LogManager::CROWD_MANAGER, "CROWD_GROUP::REF=\"%s\"::ANIMATION=\"%s\"::PERCENTAGE=%.3f::DURATION=%d", pCG->ref.c_str(), pCG->animation.c_str(), pCG->percentage, pCG->animation_duration );
#elif defined LOCAL_POS_TEXTURE
			StaticLod* static_lod = new StaticLod( glsl_manager, vbo_manager, char_groups[group_names_map[pCG->ref] );
			string lod_pos_tex	= string( "LOD_POS_TEX_" );
			string lod_fbo		= string( "LOD_FBO_" );
			string fbo_ids_tex	= string( "FBO_IDS_TEX_" );
			lod_pos_tex.append	(	pCG->ref );
			lod_fbo.append		(	pCG->ref );
			fbo_ids_tex.append	(	pCG->ref );
			crowd->addGroup		(	char_groups[group_names_map[pCG->ref]],
									static_lod,
									pCG->animation,
									lod_fbo,
									lod_pos_tex,
									fbo_ids_tex,
									pCG->percentage,
									pCG->animation_frames,
									pCG->animation_duration				);
			vector<CrowdGroup*> cGroups = crowd->getGroups();
			static_lod->init( cGroups[pcg]->getWidth(), cGroups[pcg]->getHeight() );
			log_manager->file_log( LogManager::CROWD_MANAGER, "CROWD_GROUP::REF=\"%s\"::ANIMATION=\"%s\"::PERCENTAGE=%.3f::DURATION=%d", pCG->ref.c_str(), pCG->animation.c_str(), pCG->percentage, pCG->animation_duration );
#endif
		}
		crowds.push_back( crowd );
		log_manager->file_log( LogManager::CROWD_MANAGER, "CROWD::\"%s\"::END", (char*)pC->name.c_str() );
	}
}
//
//=======================================================================================
//
CrowdManager::~CrowdManager( void )
{

}
//
//=======================================================================================
//
bool CrowdManager::loadAssets( void )
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		Crowd* crowd = crowds[c];
		vector<CrowdGroup*> cGroups = crowd->getGroups();
		for( unsigned int cg = 0; cg < cGroups.size(); cg++ )
		{
			CharacterGroup* cG = cGroups[cg]->cgroup;
			ModelProps* mProps = cG->getModelProps();
			vector<ModelProp*> props = mProps->getProps();
			for( unsigned int p = 0; p < props.size(); p++ )
			{
				if( props[p]->loaded == false )
				{
					string base_name = "";
					string rel_path = "";
					vector<string> sep = StringUtils::split( props[p]->file, '/' );
					switch( props[p]->type )
					{
					case PP_HEAD:
					case PP_HAIR:
					case PP_TORSO:
					case PP_LEGS:
						rel_path.append( sep[0] );
						for( unsigned int rp = 1; rp < sep.size() - 1; rp++ )
						{
							rel_path.append( "/" );
							rel_path.append( sep[rp] );
						}
						base_name			= sep[sep.size() -1];

						std::transform( props[p]->file.begin(), props[p]->file.end(), props[p]->file.begin(), ::tolower);
						std::transform( base_name.begin(), base_name.end(), base_name.begin(), ::tolower);

						log_manager->file_log( LogManager::CROWD_MANAGER, "LOADING::FILE=\"%s\"", base_name.c_str() );

						props[p]->model3D	= new Model3D(	props[p]->name,
															rel_path,
															vbo_manager,
															glsl_manager,
															base_name,
															1.0f			);
						if( props[p]->model3D->init( false ) == false )
						{
							log_manager->file_log( LogManager::LERROR, "LOADING::FILE=\"%s\"", base_name.c_str() );
							return false;
						}
						props[p]->loaded = true;
						break;
					case PP_ATLAS:
					case PP_WRINKLES:
					case PP_PATTERN:
						break;
					}
				}
			}
			if( cG->genCharacterModel( vbo_manager, glsl_manager ) == false )
			{
				log_manager->file_log( LogManager::LERROR, "GENERATING_STITCHED_OBJ::GROUP=\"%s\"", cG->getName().c_str() );
				return false;
			}
		}
	}
	return true;
}
//
//=======================================================================================
//
#ifdef DEMO_SHADER
void CrowdManager::draw(	Camera* camera,
							float*	viewMat,
							float*	projMat,
							float*	shadowMat,
							bool	wireframe,
							bool	shadows,
							bool	doHandD,
							bool	doPatterns,
							bool	doColor,
							bool	doFacial	)
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->draw(	camera,
							viewMat,
							projMat,
							shadowMat,
							wireframe,
							shadows,
							doHandD,
							doPatterns,
							doColor,
							doFacial	);
	}
}
#else
//
//=======================================================================================
//
void CrowdManager::draw(	Camera* camera,
							float*	viewMat,
							float*	projMat,
							float*	shadowMat,
							bool	wireframe,
							bool	shadows		)
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->draw(	camera,
							viewMat,
							projMat,
							shadowMat,
							wireframe,
							shadows		);
	}
}
#endif
//
//=======================================================================================
//
vector<Crowd*>& CrowdManager::getCrowds( void )
{
	return crowds;
}
//
//=======================================================================================
//
void CrowdManager::setFboManager( FboManager* _fbo_manager )
{
	fbo_manager = _fbo_manager;
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->setFboManager( fbo_manager );
	}
}
//
//=======================================================================================
//
void CrowdManager::nextFrame( void )
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->nextFrame();
	}
}
//
//=======================================================================================
//
void CrowdManager::initPaths(	float								plane_scale,
								vector<float>&						policy,
								unsigned int						grid_width,
								unsigned int						grid_height,
								vector<glm::vec2>&					occupation,
								vector<vector<GROUP_FORMATION>>&	formations	)
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->init_paths( plane_scale, policy, grid_width, grid_height, occupation, formations[c] );
	}
}
//
//=======================================================================================
//
void CrowdManager::updatePolicy( vector<float>&	policy )
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->updatePolicy( policy );
	}
}
//
//=======================================================================================
//
void CrowdManager::getDensity( vector<float>&	_density,
							   unsigned int		index	)
{
	crowds[index]->getDensity( _density );
}
//
//=======================================================================================
//
void CrowdManager::runPaths( void )
{
	for( unsigned int c = 0; c < crowds.size(); c++ )
	{
		crowds[c]->run_paths();
	}
}
//
//=======================================================================================
