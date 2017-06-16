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

#include "cScenario.h"

//=======================================================================================

unsigned int sppsteps =
		aiProcess_CalcTangentSpace					|	// calculate tangents and bitangents if possible
		aiProcess_Triangulate						|	// use only triangles
		aiProcess_SortByPType						|	// use only triangles
		aiProcess_JoinIdenticalVertices				|	// join identical vertices / optimize indexing
		aiProcess_ValidateDataStructure				|	// perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality				| 	// improve the cache locality of the output vertices
		aiProcess_FixInfacingNormals				|	// This step tries to determine which meshes have normal vectors that are facing inwards.
		aiProcess_RemoveRedundantMaterials			|	// remove redundant materials
		aiProcess_FindDegenerates					|	// remove degenerated polygons from the import
		aiProcess_FindInvalidData					|	// detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords						|	// convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords					|	// preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances						|	// search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights					|	// limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes					|	// join small meshes, if possible;
		0;

//=======================================================================================

Scenario::Scenario( string&			name_,
					string&			rel_path_,
					VboManager*		vbo_manager_,
					GlslManager*	glsl_manager_,
					string&			fname_,
					float			scale_			)
{
	name				= name_;
	rel_path			= rel_path_;
	scale				= scale_;
	vbo_manager			= vbo_manager_;
	glsl_manager		= glsl_manager_;
	fname				= rel_path + fname_;
	numTextures			= 0;

	vbo_frame = 0;
	importer.SetPropertyInteger( AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS | aiComponent_COLORS | aiComponent_TEXTURES );
	scene = importer.ReadFile( Scenario::fname, sppsteps );
	if( !scene )
	{
		printf( "\n\n%s\n\n", importer.GetErrorString() );
	}
	assimp_recursive_gather_data( scene, scene->mRootNode );
	printf( "\n\nScenario \"%s\" has %lu meshes.\n\n", name.c_str(), meshes.size() );

	chr_amb				= (char*)string( "amb" ).c_str();
	chr_textured		= (char*)string( "textured" ).c_str();
	chr_tint			= (char*)string( "tint" ).c_str();
}
//
//=======================================================================================
//
void Scenario::draw( void )
{
	for( unsigned int m = 0; m < meshes.size(); m++ )
	{
		if( meshes[m].tex_id > 0 )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, meshes[m].tex_id );
			glsl_manager->activate( "lambert" );
			glsl_manager->setUniformf( "lambert", chr_amb, 0.12f );
			glsl_manager->setUniformi( "lambert", chr_textured, 1 );
			glsl_manager->setUniformfv( "lambert", chr_tint, meshes[m].tint, 3 );
			vbo_manager->render_vbo( meshes[m].vbo_id, meshes[m].vbo_size );
			glsl_manager->deactivate( "lambert" );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		else
		{
			glsl_manager->activate( "lambert" );
			glsl_manager->setUniformf( "lambert", chr_amb, 0.12f );
			glsl_manager->setUniformi( "lambert", chr_textured, 0 );
			glsl_manager->setUniformfv( "lambert", chr_tint, meshes[m].tint, 3 );
			vbo_manager->render_vbo( meshes[m].vbo_id, meshes[m].vbo_size );
			glsl_manager->deactivate( "lambert" );
		}
	}
}
//
//=======================================================================================
//
Scenario::~Scenario( void )
{
	textureIdMap.clear();
}
//
//=======================================================================================
//
void Scenario::assimp_recursive_gather_data(	const struct aiScene* sc,
												const struct aiNode*  nd	)
{
	for( unsigned int n = 0; n < nd->mNumMeshes; ++n )
	{
		scenario_mesh curr_mesh;
		curr_mesh.vbo_index = vbo_manager->createVBOContainer( fname, vbo_frame );
		curr_mesh.vbo_frame = vbo_frame;
		vbo_frame++;
		curr_mesh.tex_id	= 0;
		curr_mesh.vbo_id	= 0;
		curr_mesh.tint[0]	= 0.0f;
		curr_mesh.tint[1]	= 0.0f;
		curr_mesh.tint[2]	= 0.0f;
		curr_mesh.tint_set  = false;

		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
		for( unsigned int f = 0; f < mesh->mNumFaces; ++f )
		{
			const struct aiFace* face = &mesh->mFaces[f];
			for( unsigned int i = 0; i < face->mNumIndices; i++ )
			{
				unsigned int index = face->mIndices[i];
				Vertex vert;
				INITVERTEX( vert );
				if( mesh->HasTextureCoords( 0 ) )
				{
					vert.texture[0] = mesh->mTextureCoords[0][index].x;
					vert.texture[1] = mesh->mTextureCoords[0][index].y;
				}
				if( mesh->HasTangentsAndBitangents() )
				{
					vert.tangent[0] = mesh->mTangents[index].x;
					vert.tangent[1] = mesh->mTangents[index].y;
					vert.tangent[2] = mesh->mTangents[index].z;
				}
				if( mesh->HasNormals() )
				{
					vert.normal[0] = mesh->mNormals[index].x;
					vert.normal[1] = mesh->mNormals[index].y;
					vert.normal[2] = mesh->mNormals[index].z;
				}
				if( mesh->HasVertexColors( 0 ) )
				{
					vert.color[0] = mesh->mColors[0][index].r;
					vert.color[1] = mesh->mColors[0][index].g;
					vert.color[2] = mesh->mColors[0][index].b;
					vert.color[3] = mesh->mColors[0][index].a;
				}


				struct aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

				aiString texPath;
				if( AI_SUCCESS == mtl->GetTexture( aiTextureType_DIFFUSE, 0, &texPath ) )
				{
					if( curr_mesh.tex_id == 0 )
					{
						curr_mesh.tex_file = rel_path + string(texPath.data);  // get filename
						curr_mesh.tex_id = TextureManager::getInstance()->loadTexture( curr_mesh.tex_file, false, GL_TEXTURE_2D, GL_REPLACE );
					}
				}

				aiColor4D diffuse;
				if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				{
					vert.color[0] = diffuse.r;
					vert.color[1] = diffuse.g;
					vert.color[2] = diffuse.b;
					vert.color[3] = diffuse.a;
					if( !curr_mesh.tint_set )
					{
						curr_mesh.tint[0] = diffuse.r;
						curr_mesh.tint[1] = diffuse.g;
						curr_mesh.tint[2] = diffuse.b;
						curr_mesh.tint_set = true;
					}
				}

				/*
				aiColor4D ambient;
				if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
				{

				}

				aiColor4D specular;
				if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
				{

				}

				aiColor4D emission;
				if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
				{

				}

				float shininess = 0.0;
				unsigned int max;
				aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
				{

				}
				*/

				vert.location[0] = mesh->mVertices[index].x * scale;
				vert.location[1] = mesh->mVertices[index].y * scale;
				vert.location[2] = mesh->mVertices[index].z * scale;
				vert.location[3] = 1.0f;
				vbo_manager->vbos[curr_mesh.vbo_index][curr_mesh.vbo_frame].vertices.push_back( vert );
			}
		}
		// Proceed to fill the VBO (current mesh) with data:
		curr_mesh.vbo_size = vbo_manager->gen_vbo( curr_mesh.vbo_id, curr_mesh.vbo_index, curr_mesh.vbo_frame );
		meshes.push_back( curr_mesh );
	}
	for( unsigned int n = 0; n < nd->mNumChildren; ++n )
	{
		assimp_recursive_gather_data( sc, nd->mChildren[n] );
	}
}
//
//=======================================================================================
