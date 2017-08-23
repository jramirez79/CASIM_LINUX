// @REV_TAG SRL_09_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjam�n Hern�ndez A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include "cVboManager.h"

//=======================================================================================
//
VboManager::VboManager( LogManager*		log_manager_,
						GlErrorManager*	err_manager_,
						GlslManager* 	glsl_manager_ )
{
	log_manager											= log_manager_;
	err_manager											= err_manager_;
	glsl_manager	            						= glsl_manager_;

	instancing											= 0;
	instancing_normalLocation							= 0;
	instancing_texCoord0Location						= 0;
	instancing_uPosLocation								= 0;
	instancing_uRotLocation								= 0;
	instancing_uScaLocation								= 0;
	instancing_culled									= 0;
	instancing_culled_normalLocation					= 0;
	instancing_culled_texCoord0Location					= 0;
	instancing_culled_rigged							= 0;
	instancing_culled_rigged_normalLocation				= 0;
	instancing_culled_rigged_dtLocation					= 0;
	instancing_culled_rigged_texCoord0Location			= 0;
	instancing_culled_rigged_shadow						= 0;
	instancing_culled_rigged_shadow_normalLocation		= 0;
	instancing_culled_rigged_shadow_texCoord0Location	= 0;
	instancing_culled_rigged_shadow_dtLocation			= 0;
	bumped_instancing									= 0;
	bumped_instancing_tangentLocation					= 0;
	bumped_instancing_normalLocation					= 0;
	bumped_instancing_texCoord0Location					= 0;
	bumped_instancing_uPosLocation						= 0;
	bumped_instancing_uRotLocation						= 0;
	ibHandle											= 0;
	reused												= 0;

	indexPtr											= new unsigned int[MAX_INSTANCES];
	glGenBuffers( 1, &ibHandle 							);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle 	);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
				  MAX_INSTANCES * sizeof( unsigned int ),
				  indexPtr,
				  GL_STATIC_DRAW 						);
}
//
//=======================================================================================
//
VboManager::~VboManager( void )
{
	vbos.clear();
	filename_index_map.clear();
	glDeleteBuffers( 1, &ibHandle );
	delete [] indexPtr;
	for( unsigned int i = 0; i < vbos.size(); i++ )
	{
		for( unsigned int j = 0; j < vbos[i].size(); j++ )
		{
			delete_vbo( i, j );
		}
	}

	for( unsigned int i = 0; i < gl_cuda_vbos.size(); i++ )
	{
		for( unsigned int j = 0; j < gl_cuda_vbos[i].size(); j++ )
		{
			delete_gl_cuda_vbo( i, j );
		}
	}

	vbos.clear();
	gl_cuda_vbos.clear();
}
//
//=======================================================================================
//
bool VboManager::isFrameRegistered( string& filename, unsigned int frame )
{
	bool found = false;
	if( filename_index_map.find( filename ) != filename_index_map.end() )
	{
		unsigned int index = filename_index_map[filename];
		if( vbos[index].size() > frame )
		{
			found = true;
		}
	}
	return found;
}
//
//=======================================================================================
//
unsigned int VboManager::reuseFrame( GLuint&		vboId,
									 string&		filename,
									 unsigned int	frame   )
{
	unsigned int index = filename_index_map[filename];
	vboId = vbos[index][frame].id;
	unsigned int size = vbos[index][frame].vertices.size();
	reused++;
	return size;
}
//
//=======================================================================================
//
unsigned int VboManager::createVBOContainer( string& filename, unsigned int frame )
{
	unsigned int index = 0;
	VBO vbo;
	INITVBO( vbo );
	if( filename_index_map.find( filename ) != filename_index_map.end() )
	{
		index = filename_index_map[filename];
		vbos[index].push_back( vbo );
	}
	else
	{
		vector<VBO> new_filename;
		new_filename.push_back( vbo );
		index = vbos.size();
		vbos.push_back( new_filename );
		filename_index_map[filename] = index;
	}
	return index;
}
//
//=======================================================================================
//
unsigned int VboManager::createGlCudaVboContainer( string& filename, unsigned int frame )
{
	unsigned int index = 0;
	GL_CUDA_VBO gl_cuda_vbo;
	INITGL_CUDA_VBO( gl_cuda_vbo );
	if( filename_index_map.find( filename ) != filename_index_map.end() )
	{
		index = filename_index_map[filename];
		gl_cuda_vbos[index].push_back( gl_cuda_vbo );
	}
	else
	{
		vector<GL_CUDA_VBO> new_filename;
		new_filename.push_back( gl_cuda_vbo );
		index = gl_cuda_vbos.size();
		gl_cuda_vbos.push_back( new_filename );
		filename_index_map[filename] = index;
	}
	return index;
}
//
//=======================================================================================
//
unsigned int VboManager::gen_vbo( GLuint&		vboId,
								  unsigned int	vbo_index,
								  unsigned int	vbo_frame   )
{
	glGenBuffers	(	1, &vbos[vbo_index][vbo_frame].id								);
	glBindBuffer	(	GL_ARRAY_BUFFER, vbos[vbo_index][vbo_frame].id					);
	glBufferData	(	GL_ARRAY_BUFFER,
						sizeof(Vertex) * vbos[vbo_index][vbo_frame].vertices.size(),
						NULL,
						GL_STATIC_DRAW													);
	glBufferSubData	(	GL_ARRAY_BUFFER,
						0,
						sizeof(Vertex) * vbos[vbo_index][vbo_frame].vertices.size(),
						&vbos[vbo_index][vbo_frame].vertices[0]							);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0											    );
	vboId = vbos[vbo_index][vbo_frame].id;
	return vbos[vbo_index][vbo_frame].vertices.size();
}
//
//=======================================================================================
//
void VboManager::attach_tbo2vbo( GLuint&		tboId,
								 unsigned int	gl_tex,
								 unsigned int	vbo_index,
								 unsigned int	vbo_frame
							   )
{
	glGenTextures	( 1, &vbos[vbo_index][vbo_frame].attached_tbo_id				);
	glActiveTexture ( gl_tex );
	glBindTexture	( GL_TEXTURE_BUFFER, vbos[vbo_index][vbo_frame].attached_tbo_id );
	glTexBuffer		( GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[vbo_index][vbo_frame].id	);
	glBindTexture	( GL_TEXTURE_BUFFER, 0  										);
	tboId = vbos[vbo_index][vbo_frame].attached_tbo_id;
}
//
//=======================================================================================
//
void VboManager::delete_vbo( unsigned int vbo_index, unsigned int vbo_frame )
{
	glBindBuffer	( GL_ARRAY_BUFFER, vbos[vbo_index][vbo_frame].id	);
	glDeleteBuffers	( 1, &vbos[vbo_index][vbo_frame].id					);
	glBindBuffer	( GL_ARRAY_BUFFER, 0								);
	FREE_TEXTURE	( vbos[vbo_index][vbo_frame].attached_tbo_id		);

	vbos[vbo_index][vbo_frame].id = 0;
	vbos[vbo_index][vbo_frame].vertices.clear();
}
//
//=======================================================================================
//
unsigned int VboManager::gen_gl_cuda_vbo( GLuint&						vboId,
										  struct cudaGraphicsResource*	cuda_vbo_res,
										  unsigned int					vbo_index,
										  unsigned int					vbo_frame,
										  unsigned int					vbo_res_flags
										)
{
	glGenBuffers	(	1, &gl_cuda_vbos[vbo_index][vbo_frame].gl_id						);
	glBindBuffer	(	GL_ARRAY_BUFFER, gl_cuda_vbos[vbo_index][vbo_frame].gl_id			);
	glBufferData	(	GL_ARRAY_BUFFER,
						sizeof(Vertex) * gl_cuda_vbos[vbo_index][vbo_frame].vertices.size(),
						&gl_cuda_vbos[vbo_index][vbo_frame].vertices[0],
						GL_DYNAMIC_DRAW														);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0													);
	cudaGraphicsGLRegisterBuffer( &gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res,
								  gl_cuda_vbos[vbo_index][vbo_frame].gl_id,
								  vbo_res_flags												);

	vboId = gl_cuda_vbos[vbo_index][vbo_frame].gl_id;
	cuda_vbo_res = gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res;
	return gl_cuda_vbos[vbo_index][vbo_frame].vertices.size();
}
//
//====================================================================================
//
unsigned int VboManager::gen_gl_cuda_vbo2( GLuint&						vboId,
										   struct cudaGraphicsResource*	cuda_vbo_res,
										   unsigned int					vbo_index,
										   unsigned int					vbo_frame,
										   unsigned int					vbo_res_flags
										)
{
	vector<float> data;
	for( unsigned int v = 0; v < gl_cuda_vbos[vbo_index][vbo_frame].vertices.size(); v++ )
	{
		data.push_back( gl_cuda_vbos[vbo_index][vbo_frame].vertices[v].location[0] );
		data.push_back( gl_cuda_vbos[vbo_index][vbo_frame].vertices[v].location[1] );
		data.push_back( gl_cuda_vbos[vbo_index][vbo_frame].vertices[v].location[2] );
		data.push_back( gl_cuda_vbos[vbo_index][vbo_frame].vertices[v].location[3] );
	}
	glGenBuffers	(	1, &gl_cuda_vbos[vbo_index][vbo_frame].gl_id						);
	glBindBuffer	(	GL_ARRAY_BUFFER, gl_cuda_vbos[vbo_index][vbo_frame].gl_id			);
	glBufferData	(	GL_ARRAY_BUFFER,
						sizeof(float) * 4 * gl_cuda_vbos[vbo_index][vbo_frame].vertices.size(),
						&data[0],
						GL_DYNAMIC_DRAW														);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0													);
	cudaGraphicsGLRegisterBuffer( &gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res,
								  gl_cuda_vbos[vbo_index][vbo_frame].gl_id,
								  vbo_res_flags												);

	vboId = gl_cuda_vbos[vbo_index][vbo_frame].gl_id;
	cuda_vbo_res = gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res;
	return gl_cuda_vbos[vbo_index][vbo_frame].vertices.size()*4;
}
//
//=======================================================================================
//
unsigned int VboManager::gen_gl_cuda_vbo3( GLuint&						vboId,
										   vector<float>&				data,
										   struct cudaGraphicsResource*	cuda_vbo_res,
										   unsigned int					vbo_index,
										   unsigned int					vbo_frame,
										   unsigned int					vbo_res_flags
										)
{
	glGenBuffers	(	1, &gl_cuda_vbos[vbo_index][vbo_frame].gl_id						);
	glBindBuffer	(	GL_ARRAY_BUFFER, gl_cuda_vbos[vbo_index][vbo_frame].gl_id			);
	glBufferData	(	GL_ARRAY_BUFFER,
						sizeof(float) * data.size(),
						&data[0],
						GL_DYNAMIC_DRAW														);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0													);
	cudaGraphicsGLRegisterBuffer( &gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res,
								  gl_cuda_vbos[vbo_index][vbo_frame].gl_id,
								  vbo_res_flags												);

	vboId = gl_cuda_vbos[vbo_index][vbo_frame].gl_id;
	cuda_vbo_res = gl_cuda_vbos[vbo_index][vbo_frame].cuda_vbo_res;
	return data.size();
}
//
//=======================================================================================
//
void VboManager::delete_gl_cuda_vbo( unsigned int vbo_index, unsigned int vbo_frame )
{
    if( gl_cuda_vbos[vbo_index][vbo_frame].gl_id )
	{
		cudaGLUnregisterBufferObject( gl_cuda_vbos[vbo_index][vbo_frame].gl_id			);

		glBindBuffer	( GL_ARRAY_BUFFER, gl_cuda_vbos[vbo_index][vbo_frame].gl_id		);
		glDeleteBuffers	( 1, &gl_cuda_vbos[vbo_index][vbo_frame].gl_id					);
		glBindBuffer	( GL_ARRAY_BUFFER, 0											);
		gl_cuda_vbos[vbo_index][vbo_frame].gl_id = 0;
		gl_cuda_vbos[vbo_index][vbo_frame].vertices.clear();
	}
}
//
//=======================================================================================
//
void VboManager::gen_empty_vbo( GLuint&			vboId,
								unsigned int	vbo_index,
								unsigned int	vbo_frame,
								unsigned int	size
							  )
{
	for( unsigned int i = 0; i < size; i++ )
	{
		Vertex v;
		INITVERTEX( v );
		vbos[vbo_index][vbo_frame].vertices.push_back( v );
	}
	glGenBuffers	(	1, &vbos[vbo_index][vbo_frame].id								);
	glBindBuffer	(	GL_ARRAY_BUFFER, vbos[vbo_index][vbo_frame].id					);
	glBufferData	(	GL_ARRAY_BUFFER,
						sizeof(Vertex) * size,
						NULL,
						GL_STATIC_DRAW													);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0  											);
	vboId = vbos[vbo_index][vbo_frame].id;
}
//
//=======================================================================================
//
void VboManager::update_vbo( unsigned int vbo_index, unsigned int vbo_frame )
{
	glBindBuffer	(	GL_ARRAY_BUFFER, vbos[vbo_index][vbo_frame].id					);
	glBufferSubData	(	GL_ARRAY_BUFFER,
						0,
						sizeof(Vertex) * vbos[vbo_index][vbo_frame].vertices.size(),
						&vbos[vbo_index][vbo_frame].vertices[0]							);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0  											);
}
//
//=======================================================================================
//
void VboManager::update_gl_cuda_vbo( unsigned int gl_cuda_vbo_index, unsigned int gl_cuda_vbo_frame )
{
	glBindBuffer	(	GL_ARRAY_BUFFER, gl_cuda_vbos[gl_cuda_vbo_index][gl_cuda_vbo_frame].gl_id			);
	glBufferSubData	(	GL_ARRAY_BUFFER,
						0,
						sizeof(Vertex) * gl_cuda_vbos[gl_cuda_vbo_index][gl_cuda_vbo_frame].vertices.size(),
						&gl_cuda_vbos[gl_cuda_vbo_index][gl_cuda_vbo_frame].vertices[0]						);
	glBindBuffer	(	GL_ARRAY_BUFFER, 0																	);
}
//
//=======================================================================================
//
void VboManager::render_vbo( GLuint& vboId, unsigned int size )
{
	glBindBuffer( GL_ARRAY_BUFFER, vboId );
	{
		glEnableClientState			( GL_TEXTURE_COORD_ARRAY							);
		glEnableClientState			( GL_COLOR_ARRAY									);
		glEnableClientState			( GL_NORMAL_ARRAY									);
		glEnableClientState			( GL_VERTEX_ARRAY									);

		glVertexPointer				( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET		);
		glTexCoordPointer			( 2, GL_FLOAT, sizeof(Vertex), TEXTURE_OFFSET		);
		glNormalPointer				(    GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET		);
		glColorPointer				( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET			);

		glDrawArrays				( GL_TRIANGLES, 0, size								);

		glDisableClientState		( GL_VERTEX_ARRAY									);
		glDisableClientState		( GL_NORMAL_ARRAY									);
		glDisableClientState		( GL_COLOR_ARRAY									);
		glDisableClientState		( GL_TEXTURE_COORD_ARRAY							);
	}
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
//
//=======================================================================================
//
void VboManager::render_vbo( GLuint& vboId, unsigned int size, unsigned int mode )
{
	glBindBuffer( GL_ARRAY_BUFFER, vboId );
	{
		glEnableClientState			( GL_TEXTURE_COORD_ARRAY							);
		glEnableClientState			( GL_COLOR_ARRAY									);
		glEnableClientState			( GL_NORMAL_ARRAY									);
		glEnableClientState			( GL_VERTEX_ARRAY									);

		glVertexPointer				( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET		);
		glTexCoordPointer			( 2, GL_FLOAT, sizeof(Vertex), TEXTURE_OFFSET		);
		glNormalPointer				(    GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET		);
		glColorPointer				( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET			);

		glDrawArrays				( mode, 0, size										);

		glDisableClientState		( GL_VERTEX_ARRAY									);
		glDisableClientState		( GL_NORMAL_ARRAY									);
		glDisableClientState		( GL_COLOR_ARRAY									);
		glDisableClientState		( GL_TEXTURE_COORD_ARRAY							);
	}
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
//
//=======================================================================================
//
void VboManager::render_vbo( GLuint&	vboId,
							 GLint		data_size,
							 GLenum		data_type,
							 GLenum		draw_mode,
							 GLint		draw_size	)
{
	glBindBuffer( GL_ARRAY_BUFFER, vboId );
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		{
			glVertexPointer	( data_size,
							  data_type,
							  (GLsizei)sizeof( Vertex ),
							  LOCATION_OFFSET				);
			glDrawArrays	( draw_mode,
							  (GLint)0,
							  (GLsizei)draw_size			);
		}
		glDisableClientState( GL_VERTEX_ARRAY );
	}
	glBindBuffer( GL_ARRAY_BUFFER, (GLuint)0 );
}
//
//=======================================================================================
//
void VboManager::render_bumped_vbo( GLuint&			vboId,
									unsigned int	size
								  )
{
	unsigned int tangentLocation;
	glBindBuffer( GL_ARRAY_BUFFER, vboId );
	{
		tangentLocation =
			glGetAttribLocation		(	glsl_manager->getId( bump_shader_name ),
										bump_tangent_name.c_str()						);
		glEnableClientState			(	GL_TEXTURE_COORD_ARRAY							);
		glEnableClientState			(	GL_COLOR_ARRAY									);
		glEnableClientState			(	GL_NORMAL_ARRAY									);
		glEnableClientState			(	GL_VERTEX_ARRAY									);
		glEnableVertexAttribArray	(	tangentLocation									);

		glVertexPointer				(	4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET	);
		glTexCoordPointer			(	2, GL_FLOAT, sizeof(Vertex), TEXTURE_OFFSET		);
		glNormalPointer				(	   GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET		);
		glColorPointer				(	4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET		);
		glVertexAttribPointer		(	tangentLocation,
										3,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TANGENT_OFFSET									);

		glDrawArrays				(	GL_TRIANGLES, 0, size							);

		glDisableVertexAttribArray	(	tangentLocation									);
		glDisableClientState		(	GL_VERTEX_ARRAY									);
		glDisableClientState		(	GL_NORMAL_ARRAY									);
		glDisableClientState		(	GL_COLOR_ARRAY									);
		glDisableClientState		(	GL_TEXTURE_COORD_ARRAY							);
	}
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
//
//=======================================================================================
//
void VboManager::render_instanced_vbo( GLuint&			vboId,
									   GLuint&			texture,
									   unsigned int		size,
									   unsigned int		count,
									   unsigned int		positions_sizeInBytes,
									   float*			positions,
									   unsigned int		rotations_sizeInBytes,
									   float*			rotations,
									   float*			viewMat
									 )
{
	GLuint uPosBuffer;
	glGenBuffers( 1, &uPosBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uPosBuffer );
	glBufferData( GL_UNIFORM_BUFFER, positions_sizeInBytes, positions, GL_STATIC_READ );
	glUniformBufferEXT( instancing, instancing_uPosLocation, uPosBuffer );

	GLuint uRotBuffer;
	glGenBuffers( 1, &uRotBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uRotBuffer );
	glBufferData( GL_UNIFORM_BUFFER, rotations_sizeInBytes, rotations, GL_STATIC_READ );
	glUniformBufferEXT( instancing, instancing_uRotLocation, uRotBuffer );

	glsl_manager->activate( instancing_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_shader_name,
										  (char*)instancing_viewmat_name.c_str(),
										  viewMat,
										  4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_normalLocation    );
				glEnableVertexAttribArray( instancing_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer( instancing_texCoord0Location,
									   2,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TEXTURE_OFFSET );
				glVertexAttribPointer( instancing_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_texCoord0Location );
				glDisableVertexAttribArray( instancing_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		err_manager->getError( "END: render_instanced_vbo" );
	}
	glsl_manager->deactivate( instancing_shader_name );
	glDeleteBuffers( 1, &uPosBuffer );
	glDeleteBuffers( 1, &uRotBuffer );
}
//
//=======================================================================================
//
void VboManager::render_instanced_vbo( GLuint&			vboId,
									   GLuint&			texture,
									   unsigned int		size,
									   unsigned int		count,
									   unsigned int		positions_sizeInBytes,
									   float*			positions,
									   unsigned int		rotations_sizeInBytes,
									   float*			rotations,
									   unsigned int		scales_sizeInBytes,
									   float*			scales,
									   float*			viewMat
									 )
{
	GLuint uPosBuffer;
	glGenBuffers( 1, &uPosBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uPosBuffer );
	glBufferData( GL_UNIFORM_BUFFER, positions_sizeInBytes, positions, GL_STATIC_READ );
	glUniformBufferEXT( instancing, instancing_uPosLocation, uPosBuffer );

	GLuint uRotBuffer;
	glGenBuffers( 1, &uRotBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uRotBuffer );
	glBufferData( GL_UNIFORM_BUFFER, rotations_sizeInBytes, rotations, GL_STATIC_READ );
	glUniformBufferEXT( instancing, instancing_uRotLocation, uRotBuffer );

	GLuint uScaBuffer;
	glGenBuffers( 1, &uScaBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uScaBuffer );
	glBufferData( GL_UNIFORM_BUFFER, scales_sizeInBytes, scales, GL_STATIC_READ );
	glUniformBufferEXT( instancing, instancing_uScaLocation, uScaBuffer );


	glsl_manager->activate( instancing_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_shader_name,
										  (char*)instancing_viewmat_name.c_str(),
										  viewMat,
										  4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_normalLocation    );
				glEnableVertexAttribArray( instancing_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer( instancing_texCoord0Location,
									   2,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TEXTURE_OFFSET );
				glVertexAttribPointer( instancing_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_texCoord0Location );
				glDisableVertexAttribArray( instancing_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		err_manager->getError( "END: render_instanced_vbo" );
	}
	glsl_manager->deactivate( instancing_shader_name );
	glDeleteBuffers( 1, &uPosBuffer );
	glDeleteBuffers( 1, &uRotBuffer );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_vbo(  GLuint&		vboId,
											   GLuint&		texture,
											   GLuint&		pos_vbo_id,
											   unsigned int size,
											   unsigned int count,
											   float*		viewMat
											)
{
	glsl_manager->activate( instancing_culled_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_shader_name,
										  (char*)instancing_culled_viewmat_name.c_str(),
										  viewMat,
										  4 );
		if( texture )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, texture );
		}

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_normalLocation    );
				if( texture )
				{
					glEnableVertexAttribArray( instancing_culled_texCoord0Location );
				}

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( texture )
				{
					glVertexAttribPointer( instancing_culled_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
				}
				glVertexAttribPointer( instancing_culled_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				if( texture )
				{
					glDisableVertexAttribArray( instancing_culled_texCoord0Location );
				}
				glDisableVertexAttribArray( instancing_culled_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		if( texture )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}
	glsl_manager->deactivate( instancing_culled_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo	(	GLuint&					vboId,
														GLuint&					texture,
														GLuint&					zonesTexture,
														GLuint&					weightsTexture,
														GLuint&					displacementTexture,
														GLuint&					pos_vbo_id,
														float					dt,
														unsigned int			size,
														unsigned int			count,
														float*					viewMat,
														bool					wireframe			)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);

		GLint activeTex = 0;
		glActiveTexture( GL_TEXTURE1 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE1 )
		printf( "\nACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, zonesTexture );

		glActiveTexture( GL_TEXTURE3 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE3 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, weightsTexture );

		glActiveTexture( GL_TEXTURE0 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE0 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, texture );

		glActiveTexture( GL_TEXTURE5 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE5 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				//glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				//glEnableClientState( GL_NORMAL_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( texture )
				{
					glVertexAttribPointer( instancing_culled_rigged_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
				}
				glVertexAttribPointer( instancing_culled_rigged_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				//glNormalPointer( GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET );
				//glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				//glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				//glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE1 )
		printf( "\nACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE3 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE0 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE5 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo	(	GLuint&					vboId,
														GLuint&					posTextureTarget,
														GLuint&					posTextureId,
														GLuint&					texture,
														GLuint&					zonesTexture,
														GLuint&					weightsTexture,
														GLuint&					displacementTexture,
														GLuint&					pos_vbo_id,
														float					dt,
														unsigned int			size,
														unsigned int			count,
														float*					viewMat,
														bool					wireframe			)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);

		GLint activeTex = 0;
		glActiveTexture( GL_TEXTURE1 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE1 )
		printf( "\nACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, zonesTexture );

		glActiveTexture( GL_TEXTURE3 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE3 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, weightsTexture );

		glActiveTexture( GL_TEXTURE0 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE0 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, texture );

		glActiveTexture( GL_TEXTURE5 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE5 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE7 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( posTextureTarget, posTextureId );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				//glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				//glEnableClientState( GL_NORMAL_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( texture )
				{
					glVertexAttribPointer( instancing_culled_rigged_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
				}
				glVertexAttribPointer( instancing_culled_rigged_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				//glNormalPointer( GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET );
				//glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				//glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				//glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE1 )
		printf( "\nACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE3 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE0 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE5 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);
		if( activeTex != GL_TEXTURE7 )
		printf( "ACTIVATED_TEXTURE %i\n", activeTex );
		glBindTexture( posTextureTarget, 0 );

	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo	(	GLuint&					vboId,
														GLuint&					posTextureTarget,
														GLuint&					posTextureId,
														GLuint&					skin_texture,
														GLuint&					hair_texture,
														GLuint&					cap_texture,
														GLuint&					torso_texture,
														GLuint&					legs_texture,
														GLuint&					zonesTexture,
														GLuint&					weightsTexture,
														GLuint&					displacementTexture,
														GLuint&					pos_vbo_id,
														float					dt,
														unsigned int			size,
														unsigned int			count,
														float*					viewMat,
														bool					wireframe			)
{

	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, skin_texture );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, hair_texture);

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, cap_texture );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, torso_texture );

		glActiveTexture( GL_TEXTURE8 );
		glBindTexture( GL_TEXTURE_2D, legs_texture );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, weightsTexture );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, zonesTexture );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, posTextureId );

		glActiveTexture( GL_TEXTURE9 );
		glBindTexture( GL_TEXTURE_2D, displacementTexture );



		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				//glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				//glEnableClientState( GL_NORMAL_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( skin_texture )
				{
					glVertexAttribPointer( instancing_culled_rigged_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
					glVertexAttribPointer( instancing_culled_rigged_normalLocation,
											3,
											GL_FLOAT,
											true,
											sizeof( Vertex ),
											NORMAL_OFFSET );


				}
				glVertexAttribPointer( instancing_culled_rigged_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				//glNormalPointer( GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET );
				//glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				//glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				//glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE8 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, 0 );

		glActiveTexture( GL_TEXTURE9 );
		glBindTexture( GL_TEXTURE_2D, 0 );

	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
//
//=======================================================================================
//
#ifdef DEMO_SHADER
void VboManager::render_instanced_culled_rigged_vbo2(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			posTextureTarget,
														GLuint&			posTextureId,
														GLuint&			pos_vbo_id,

														GLuint&			clothing_color_table_tex_id,
														GLuint&			pattern_color_table_tex_id,
														GLuint&			global_mt_tex_id,
														GLuint&			torso_mt_tex_id,
														GLuint&			legs_mt_tex_id,
														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,
														GLuint&			facial_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														bool			wireframe,

														float			doHandD,
														float			doPatterns,
														float			doColor,
														float			doFacial					)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
	    string name;
	    name = string( "AGENTS_NPOT" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _AGENTS_NPOT						);
        name = string( "ANIMATION_LENGTH" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _ANIMATION_LENGTH					);
        name = string( "STEP" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _STEP								);
        name = string( "lod" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
									 lod								);
        name = string( "gender" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
                                    gender								);
        name = string( "doHeightAndDisplacement" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doHandD							    );
        name = string( "doPatterns" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doPatterns							);
        name = string( "doColor" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doColor							    );
        name = string( "doFacial" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doFacial							);
        name = string( "camPos" );
		glsl_manager->setUniformfv(	instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    &cam->getPosition()[0],
                                    3								    );
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		name = string( "color" );
		glsl_manager->setUniformfv( instancing_culled_rigged_shader_name, (char*)name.c_str(), color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo2" );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, global_mt_tex_id );
		//glBufferSubData	(	GL_ARRAY_BUFFER,
		//					0,
		//					sizeof(Vertex) * gl_cuda_vbos[vbo_index][vbo_frame].vertices.size(),
		//					&gl_cuda_vbos[vbo_index][vbo_frame].vertices[0]						);

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, torso_mt_tex_id );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, legs_mt_tex_id );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, facial_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, posTextureId );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_color_table_tex_id );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, pattern_color_table_tex_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, 0 );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
		//err_manager->getError( "END: render_instanced_culled_rigged_vbo2" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
#else
void VboManager::render_instanced_culled_rigged_vbo2(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			posTextureTarget,
														GLuint&			posTextureId,
														GLuint&			pos_vbo_id,

														GLuint&			clothing_color_table_tex_id,
														GLuint&			pattern_color_table_tex_id,
														GLuint&			global_mt_tex_id,
														GLuint&			torso_mt_tex_id,
														GLuint&			legs_mt_tex_id,
														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,
														GLuint&			facial_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														bool			wireframe					)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "AGENTS_NPOT",
									 _AGENTS_NPOT						);
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "ANIMATION_LENGTH",
									 _ANIMATION_LENGTH					);
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "STEP",
									 _STEP								);
		glsl_manager->setUniformf( instancing_culled_rigged_shader_name,
									 "lod",
									 lod								);
		glsl_manager->setUniformf( instancing_culled_rigged_shader_name,
									 "gender",
									 gender								);
		glsl_manager->setUniformfv(	instancing_culled_rigged_shader_name,
										"camPos",
										cam->getPosition().asArray(),
										3								);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		glsl_manager->setUniformfv( instancing_culled_rigged_shader_name, "color", color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo2" );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, global_mt_tex_id );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, torso_mt_tex_id );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, legs_mt_tex_id );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, facial_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, posTextureId );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_color_table_tex_id );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, pattern_color_table_tex_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, 0 );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
		//err_manager->getError( "END: render_instanced_culled_rigged_vbo2" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
#endif
//
//=======================================================================================
//
#ifdef DEMO_SHADER
void VboManager::render_instanced_culled_rigged_vbo4(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			ids_buffer,
														GLuint&			pos_buffer,

														GLuint&			clothing_color_table_tex_id,
														GLuint&			pattern_color_table_tex_id,
														GLuint&			global_mt_tex_id,
														GLuint&			torso_mt_tex_id,
														GLuint&			legs_mt_tex_id,
														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,
														GLuint&			facial_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														bool			wireframe,

														float			doHandD,
														float			doPatterns,
														float			doColor,
														float			doFacial					)
{

	//cout<<"**************************************************render_instanced_culled_rigged_vbo4******************************************"<<endl;
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
	    string name;
	    name = string( "AGENTS_NPOT" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _AGENTS_NPOT						);
        name = string( "ANIMATION_LENGTH" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _ANIMATION_LENGTH					);
        name = string( "STEP" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    _STEP								);
        name = string( "lod" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
									 lod								);
        name = string( "gender" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
                                    gender								);
        name = string( "doHeightAndDisplacement" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doHandD							    );
        name = string( "doPatterns" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doPatterns							);
        name = string( "doColor" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doColor							    );
        name = string( "doFacial" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    doFacial							);
        name = string( "camPos" );
		glsl_manager->setUniformfv(	instancing_culled_rigged_shader_name,
                                    (char*)name.c_str(),
                                    &cam->getPosition()[0],
                                    3								    );
		name = string( "zNear"	);
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
									glsl_manager->vZNear			);
		//cout<<"-------------glsl_manager->vZNear------------"<<glsl_manager->vZNear<<endl;
		name = string( "zFar"	);
		glsl_manager->setUniformf(  instancing_culled_rigged_shader_name,
									(char*)name.c_str(),
									glsl_manager->vZFar				);
		//cout<<"-------------glsl_manager->vZFar------------"<<glsl_manager->vZFar<<endl;
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		name = string( "color" );
		glsl_manager->setUniformfv( instancing_culled_rigged_shader_name, (char*)name.c_str(), color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo4" );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, global_mt_tex_id );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, torso_mt_tex_id );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, legs_mt_tex_id );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, facial_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, ids_buffer );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_buffer );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_color_table_tex_id );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, pattern_color_table_tex_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
		//err_manager->getError( "END: render_instanced_culled_rigged_vbo4" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
#else
void VboManager::render_instanced_culled_rigged_vbo4(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			posTextureId,
														GLuint&			pos_vbo_id,

														GLuint&			clothing_color_table_tex_id,
														GLuint&			pattern_color_table_tex_id,
														GLuint&			global_mt_tex_id,
														GLuint&			torso_mt_tex_id,
														GLuint&			legs_mt_tex_id,
														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,
														GLuint&			facial_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														bool			wireframe					)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "AGENTS_NPOT",
									 _AGENTS_NPOT						);
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "ANIMATION_LENGTH",
									 _ANIMATION_LENGTH					);
		glsl_manager->setUniformi( instancing_culled_rigged_shader_name,
									 "STEP",
									 _STEP								);
		glsl_manager->setUniformf( instancing_culled_rigged_shader_name,
									 "lod",
									 lod								);
		glsl_manager->setUniformf( instancing_culled_rigged_shader_name,
									 "gender",
									 gender								);
		glsl_manager->setUniformfv(	instancing_culled_rigged_shader_name,
										"camPos",
										cam->getPosition().asArray(),
										3								);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		glsl_manager->setUniformfv( instancing_culled_rigged_shader_name, "color", color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo2" );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, global_mt_tex_id );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, torso_mt_tex_id );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, legs_mt_tex_id );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, facial_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, posTextureId );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_color_table_tex_id );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, pattern_color_table_tex_id );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
		//err_manager->getError( "END: render_instanced_culled_rigged_vbo2" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
#endif
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo3(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			posTextureTarget,
														GLuint&			posTextureId,
														GLuint&			pos_vbo_id,

														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														float*			projMat,
														float*			shadowMat,
														bool			wireframe					)
{
	glsl_manager->activate( instancing_culled_rigged_shadow_shader_name );
	{
	    string name;
	    name = string( "AGENTS_NPOT" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _AGENTS_NPOT						);
        name = string( "ANIMATION_LENGTH" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _ANIMATION_LENGTH					);
        name = string( "STEP" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _STEP								);
        name = string( "lod" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
									(char*)name.c_str(),
									 lod								);
        name = string( "gender" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
									(char*)name.c_str(),
                                    gender								);
        name = string( "camPos" );
		glsl_manager->setUniformfv(	instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    &cam->getPosition()[0],
                                    3								    );
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_viewmat_name.c_str(),
										  viewMat,
										  4														);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_projmat_name.c_str(),
										  projMat,
										  4														);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_shadowmat_name.c_str(),
										  shadowMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		name = string( "color" );
		glsl_manager->setUniformfv( instancing_culled_rigged_shadow_shader_name, (char*)name.c_str(), color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo3" );
		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, posTextureId );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_shadow_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_shadow_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_shadow_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_shadow_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_shadow_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_shadow_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, 0 );

		//err_manager->getError( "END: render_instanced_culled_rigged_vbo3" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shadow_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo5(	Camera*			cam,
														GLuint&			vboId,
														GLuint&			posTextureId,
														GLuint&			pos_vbo_id,

														GLuint&			rigging_mt_tex_id,
														GLuint&			animation_mt_tex_id,

														float			lod,
														float			gender,
														unsigned int	_AGENTS_NPOT,
														unsigned int	_ANIMATION_LENGTH,
														unsigned int	_STEP,
														unsigned int	size,
														unsigned int	count,
														float*			viewMat,
														float*			projMat,
														float*			shadowMat,
														bool			wireframe					)
{

	//cout<<"**************************************************render_instanced_culled_rigged_vbo5******************************************"<<endl;
	glsl_manager->activate( instancing_culled_rigged_shadow_shader_name );
	{
	    string name;
	    name = string( "AGENTS_NPOT" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _AGENTS_NPOT						);
        name = string( "ANIMATION_LENGTH" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _ANIMATION_LENGTH					);
        name = string( "STEP" );
		glsl_manager->setUniformi(  instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    _STEP								);
        name = string( "lod" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
									(char*)name.c_str(),
									 lod								);
        name = string( "gender" );
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
									(char*)name.c_str(),
                                    gender								);
        name = string( "camPos" );
		glsl_manager->setUniformfv(	instancing_culled_rigged_shadow_shader_name,
                                    (char*)name.c_str(),
                                    &cam->getPosition()[0],
                                    3								    );
		name = string( "zNear"	);
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
		                            (char*)name.c_str(),
		                            glsl_manager->vZNear				);
		name = string( "zFar"	);
		glsl_manager->setUniformf(  instancing_culled_rigged_shadow_shader_name,
									(char*)name.c_str(),
									glsl_manager->vZFar				);

		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_viewmat_name.c_str(),
										  viewMat,
										  4														);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_projmat_name.c_str(),
										  projMat,
										  4														);
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shadow_shader_name,
										  (char*)instancing_culled_rigged_shadow_shadowmat_name.c_str(),
										  shadowMat,
										  4														);
		float color[] = { ((int)lod<<1)/4.0f, 0.5f, 0.5f, 1.0f };
		name = string( "color" );
		glsl_manager->setUniformfv( instancing_culled_rigged_shadow_shader_name, (char*)name.c_str(), color, 4 );
		//err_manager->getError( "BEGIN: render_instanced_culled_rigged_vbo5" );
		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, rigging_mt_tex_id );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, animation_mt_tex_id );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, posTextureId );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_shadow_texCoord0Location );
				glEnableVertexAttribArray( instancing_culled_rigged_shadow_normalLocation    );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_shadow_texCoord0Location,
										2,
										GL_FLOAT,
										false,
										sizeof( Vertex ),
										TEXTURE_OFFSET );
				glVertexAttribPointer(	instancing_culled_rigged_shadow_normalLocation,
										3,
										GL_FLOAT,
										true,
										sizeof( Vertex ),
										NORMAL_OFFSET );
				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!
				glDisableVertexAttribArray( instancing_culled_rigged_shadow_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_shadow_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		//err_manager->getError( "END: render_instanced_culled_rigged_vbo5" );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shadow_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo	(	GLuint&					vboId,
														GLuint&					posTextureTarget,
														GLuint&					posTextureId,
														GLuint&					skin_texture,
														GLuint&					hair_texture,
														GLuint&					cap_texture,
														GLuint&					torso_texture,
														GLuint&					legs_texture,
														GLuint&					clothing_color_table_tex_id,
														GLuint&					clothing_patterns_tex_id,
														GLuint&					zonesTexture,
														GLuint&					weightsTexture,
														GLuint&					displacementTexture,
														GLuint&					pos_vbo_id,
														float					dt,
														unsigned int			size,
														unsigned int			count,
														float*					viewMat,
														bool					wireframe			)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, skin_texture );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, hair_texture);

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, cap_texture );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, torso_texture );

		glActiveTexture( GL_TEXTURE8 );
		glBindTexture( GL_TEXTURE_2D, legs_texture );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, weightsTexture );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, zonesTexture );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, posTextureId );

		glActiveTexture( GL_TEXTURE9 );
		glBindTexture( GL_TEXTURE_2D, displacementTexture );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_color_table_tex_id );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_RECTANGLE, clothing_patterns_tex_id );



		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				//glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				//glEnableClientState( GL_NORMAL_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( skin_texture )
				{
					glVertexAttribPointer( instancing_culled_rigged_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
					glVertexAttribPointer( instancing_culled_rigged_normalLocation,
											3,
											GL_FLOAT,
											true,
											sizeof( Vertex ),
											NORMAL_OFFSET );


				}
				glVertexAttribPointer( instancing_culled_rigged_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				//glNormalPointer( GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET );
				//glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				//glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				//glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE8 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( posTextureTarget, 0 );

		glActiveTexture( GL_TEXTURE9 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE11 );
		glBindTexture( GL_TEXTURE_2D, 0 );

	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_culled_rigged_vbo	(	GLuint&					vboId,
														GLuint&					texture1,
														GLuint&					texture2,
														GLuint&					texture3,
														GLuint&					texture4,
														GLuint&					zonesTexture,
														GLuint&					weightsTexture,
														GLuint&					displacementTexture,
														GLuint&					pos_vbo_id,
														float					dt,
														unsigned int			size,
														unsigned int			count,
														float*					viewMat,
														bool					wireframe			)
{
	glsl_manager->activate( instancing_culled_rigged_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_culled_rigged_shader_name,
										  (char*)instancing_culled_rigged_viewmat_name.c_str(),
										  viewMat,
										  4														);

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, zonesTexture );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, weightsTexture );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture1 );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, texture2 );
		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, texture3 );
		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, texture4 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, pos_vbo_id );

		//glActiveTexture( GL_TEXTURE0 );

		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				//glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				//glEnableClientState( GL_NORMAL_ARRAY );
				glEnableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				glEnableVertexAttribArray( instancing_culled_rigged_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				if( texture1 )
				{
					glVertexAttribPointer( instancing_culled_rigged_texCoord0Location,
										   2,
										   GL_FLOAT,
										   false,
										   sizeof( Vertex ),
										   TEXTURE_OFFSET );
				}
				glVertexAttribPointer( instancing_culled_rigged_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				//glNormalPointer( GL_FLOAT, sizeof(Vertex), NORMAL_OFFSET );
				//glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				if( wireframe )
				{
					for( unsigned int i = 0; i < size; i += 3 )
					  glDrawArraysInstanced( GL_LINE_LOOP, i, 3, count );
				}
				else
				{
					glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				}
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_culled_rigged_texCoord0Location );
				glDisableVertexAttribArray( instancing_culled_rigged_normalLocation    );
				//glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				//glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
	}
	glsl_manager->deactivate( instancing_culled_rigged_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_vbo( GLuint&		vboId,
									   GLuint&		texture,
									   unsigned int size,
									   unsigned int count,
									   float*		viewMat,
									   GLuint		uPosBuffer,
									   GLuint		uRotBuffer,
									   GLuint		uScaBuffer
									 )
{
	glsl_manager->activate( instancing_shader_name );
	{
		glsl_manager->setUniformMatrix( instancing_shader_name,
										  (char*)instancing_viewmat_name.c_str(),
										  viewMat,
										  4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( instancing_normalLocation    );
				glEnableVertexAttribArray( instancing_texCoord0Location );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer( instancing_texCoord0Location,
									   2,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TEXTURE_OFFSET );
				glVertexAttribPointer( instancing_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( instancing_texCoord0Location );
				glDisableVertexAttribArray( instancing_normalLocation    );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	glsl_manager->deactivate( instancing_shader_name );
}
//
//=======================================================================================
//
void VboManager::render_instanced_bumped_vbo( GLuint&		vboId,
											  GLuint&		texture,
											  GLuint&		normal_texture,
											  GLuint&		specular_texture,
											  unsigned int	size,
											  unsigned int	count,
											  unsigned int	positions_sizeInBytes,
											  float*		positions,
											  unsigned int	rotations_sizeInBytes,
											  float*		rotations,
											  float*		viewMat
											)
{
	GLuint uPosBuffer;
	glGenBuffers( 1, &uPosBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uPosBuffer );
	glBufferData( GL_UNIFORM_BUFFER, positions_sizeInBytes, positions, GL_STATIC_READ );
	glUniformBufferEXT( bumped_instancing, bumped_instancing_uPosLocation, uPosBuffer );

	GLuint uRotBuffer;
	glGenBuffers( 1, &uRotBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, uRotBuffer );
	glBufferData( GL_UNIFORM_BUFFER, rotations_sizeInBytes, rotations, GL_STATIC_READ );
	glUniformBufferEXT( bumped_instancing, bumped_instancing_uRotLocation, uRotBuffer );

	glsl_manager->activate( bump_shader_name );
	{
		glsl_manager->setUniformMatrix( bump_shader_name,
										  (char*)bump_viewmat_name.c_str(),
										  viewMat,
										  4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, normal_texture );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, specular_texture );
		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( bumped_instancing_texCoord0Location );
				glEnableVertexAttribArray( bumped_instancing_normalLocation    );
				glEnableVertexAttribArray( bumped_instancing_tangentLocation   );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer( bumped_instancing_texCoord0Location,
									   2,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TEXTURE_OFFSET );
				glVertexAttribPointer( bumped_instancing_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );
				glVertexAttribPointer( bumped_instancing_tangentLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TANGENT_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( bumped_instancing_tangentLocation   );
				glDisableVertexAttribArray( bumped_instancing_normalLocation    );
				glDisableVertexAttribArray( bumped_instancing_texCoord0Location );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	glsl_manager->deactivate( bump_shader_name );
	glDeleteBuffers( 1, &uPosBuffer );
	glDeleteBuffers( 1, &uRotBuffer );
}
//
//=======================================================================================
//
void VboManager::render_instanced_bumped_vbo( GLuint&		vboId,
											  GLuint&		texture,
											  GLuint&		normal_texture,
											  GLuint&		specular_texture,
											  unsigned int	size,
											  unsigned int	count,
											  float*		viewMat,
											  GLuint		uPosBuffer,
											  GLuint		uRotBuffer
											)
{
	glsl_manager->activate( bump_shader_name );
	{
		glsl_manager->setUniformMatrix( bump_shader_name,
										  (char*)bump_viewmat_name.c_str(),
										  viewMat,
										  4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, normal_texture );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, specular_texture );
		glBindBuffer( GL_ARRAY_BUFFER, vboId );
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibHandle );
			{
				glEnableClientState( GL_COLOR_ARRAY );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableVertexAttribArray( bumped_instancing_texCoord0Location );
				glEnableVertexAttribArray( bumped_instancing_normalLocation    );
				glEnableVertexAttribArray( bumped_instancing_tangentLocation   );

				glVertexPointer( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET );
				glVertexAttribPointer( bumped_instancing_texCoord0Location,
									   2,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TEXTURE_OFFSET );
				glVertexAttribPointer( bumped_instancing_normalLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   NORMAL_OFFSET );
				glColorPointer( 4, GL_FLOAT, sizeof(Vertex), COLOR_OFFSET );
				glVertexAttribPointer( bumped_instancing_tangentLocation,
									   3,
									   GL_FLOAT,
									   false,
									   sizeof( Vertex ),
									   TANGENT_OFFSET );

				// MIGHTY INSTANCING!
				glDrawArraysInstanced( GL_TRIANGLES, 0, size, count );
				// MIGHTY INSTANCING!

				glDisableVertexAttribArray( bumped_instancing_tangentLocation   );
				glDisableVertexAttribArray( bumped_instancing_normalLocation    );
				glDisableVertexAttribArray( bumped_instancing_texCoord0Location );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	glsl_manager->deactivate( bump_shader_name );
}
//
//=======================================================================================
//
unsigned int VboManager::getReusedVBOs( void )
{
	return reused;
}
//
//=======================================================================================
//
VBO VboManager::getVBO( string& filename, unsigned int frame )
{
	unsigned int index = 0;
	if( filename_index_map.find( filename ) != filename_index_map.end() )
	{
		index = filename_index_map[filename];
	}
	return vbos[index][frame];
}
//
//=======================================================================================
//
void VboManager::setInstancingLocations(	string&		instancing_shader_name,
											string&		instancing_positions_name,
											string&		instancing_rotations_name,
											string&		instancing_scales_name,
											string&		instancing_normal_name,
											string&		instancing_texCoord0_name,
											string&		instancing_viewmat_name
									   )
{
	VboManager::instancing_shader_name		= string( instancing_shader_name    );
	VboManager::instancing_positions_name	= string( instancing_positions_name );
	VboManager::instancing_rotations_name	= string( instancing_rotations_name );
	VboManager::instancing_scales_name		= string( instancing_scales_name	);
	VboManager::instancing_normal_name		= string( instancing_normal_name    );
	VboManager::instancing_texCoord0_name	= string( instancing_texCoord0_name );
	VboManager::instancing_viewmat_name		= string( instancing_viewmat_name   );

	instancing								= glsl_manager->getId	( instancing_shader_name			);
	instancing_uPosLocation					= glGetUniformLocation	( instancing,
																	  instancing_positions_name.c_str() );
	instancing_uRotLocation					= glGetUniformLocation	( instancing,
																	  instancing_rotations_name.c_str() );
	instancing_uScaLocation					= glGetUniformLocation	( instancing,
																	  instancing_scales_name.c_str()	);
	instancing_normalLocation				= glGetAttribLocation	( instancing,
																	  instancing_normal_name.c_str()    );
	instancing_texCoord0Location			= glGetAttribLocation	( instancing,
																	  instancing_texCoord0_name.c_str() );
	log_manager->log( LogManager::VBO_MANAGER, "Instancing locations set." );
}
//
//=======================================================================================
//
void VboManager::setInstancingCulledLocations(	string&		instancing_culled_shader_name,
												string&		instancing_culled_normal_name,
												string&		instancing_culled_texCoord0_name,
												string&		instancing_culled_viewmat_name
											 )
{
	VboManager::instancing_culled_shader_name		= string( instancing_culled_shader_name    );
	VboManager::instancing_culled_normal_name		= string( instancing_culled_normal_name    );
	VboManager::instancing_culled_texCoord0_name	= string( instancing_culled_texCoord0_name );
	VboManager::instancing_culled_viewmat_name		= string( instancing_culled_viewmat_name   );

	instancing_culled								= glsl_manager->getId	( instancing_culled_shader_name		);
	instancing_culled_normalLocation				= glGetAttribLocation	( instancing_culled,
																	  instancing_culled_normal_name.c_str()		);
	instancing_culled_texCoord0Location				= glGetAttribLocation	( instancing_culled,
																	  instancing_culled_texCoord0_name.c_str()	);
	log_manager->log( LogManager::VBO_MANAGER, "InstancingCulled locations set." );
}
//
//=======================================================================================
//
void VboManager::setInstancingCulledRiggedLocations	(	string&		instancing_culled_rigged_shader_name,
														string&		instancing_culled_rigged_normal_name,
														string&		instancing_culled_rigged_dt_name,
														string&		instancing_culled_rigged_texCoord0_name,
														string&		instancing_culled_rigged_viewmat_name		)
{
	VboManager::instancing_culled_rigged_shader_name		= string( instancing_culled_rigged_shader_name    );
	VboManager::instancing_culled_rigged_normal_name		= string( instancing_culled_rigged_normal_name    );
	VboManager::instancing_culled_rigged_dt_name			= string( instancing_culled_rigged_dt_name		  );
	VboManager::instancing_culled_rigged_texCoord0_name		= string( instancing_culled_rigged_texCoord0_name );
	VboManager::instancing_culled_rigged_viewmat_name		= string( instancing_culled_rigged_viewmat_name   );

	instancing_culled_rigged								= glsl_manager->getId	( instancing_culled_rigged_shader_name	);
	instancing_culled_rigged_texCoord0Location				= glGetAttribLocation	( instancing_culled_rigged,
																	  instancing_culled_rigged_texCoord0_name.c_str()		);
	instancing_culled_rigged_normalLocation					= glGetAttribLocation	( instancing_culled_rigged,
																	  instancing_culled_rigged_normal_name.c_str()			);
	//instancing_culled_rigged_dtLocation						= glGetAttribLocation	( instancing_culled_rigged,
	//																  instancing_culled_rigged_dt_name.c_str()				);
	log_manager->log( LogManager::VBO_MANAGER, "Instancing Culled Rigged locations set." );
}
//
//=======================================================================================
//
void VboManager::setInstancingCulledRiggedShadowLocations(	string&	instancing_culled_rigged_shadow_shader_name,
															string&	instancing_culled_rigged_shadow_normal_name,
															string&	instancing_culled_rigged_shadow_texCoord0_name,
															string&	instancing_culled_rigged_shadow_dt_name,
															string&	instancing_culled_rigged_shadow_viewmat_name,
															string&	instancing_culled_rigged_shadow_projmat_name,
															string&	instancing_culled_rigged_shadow_shadowmat_name	)
{
	VboManager::instancing_culled_rigged_shadow_shader_name		= string( instancing_culled_rigged_shadow_shader_name		);
	VboManager::instancing_culled_rigged_shadow_normal_name		= string( instancing_culled_rigged_shadow_normal_name		);
	VboManager::instancing_culled_rigged_shadow_texCoord0_name	= string( instancing_culled_rigged_shadow_texCoord0_name	);
	VboManager::instancing_culled_rigged_shadow_dt_name			= string( instancing_culled_rigged_shadow_dt_name			);
	VboManager::instancing_culled_rigged_shadow_viewmat_name	= string( instancing_culled_rigged_shadow_viewmat_name		);
	VboManager::instancing_culled_rigged_shadow_projmat_name	= string( instancing_culled_rigged_shadow_projmat_name		);
	VboManager::instancing_culled_rigged_shadow_shadowmat_name	= string( instancing_culled_rigged_shadow_shadowmat_name	);

	instancing_culled_rigged_shadow								= glsl_manager->getId	( instancing_culled_rigged_shadow_shader_name	);
	instancing_culled_rigged_shadow_texCoord0Location			= glGetAttribLocation	( instancing_culled_rigged_shadow,
																	  instancing_culled_rigged_shadow_texCoord0_name.c_str()			);
	instancing_culled_rigged_shadow_normalLocation				= glGetAttribLocation	( instancing_culled_rigged_shadow,
																	  instancing_culled_rigged_shadow_normal_name.c_str()				);
	log_manager->log( LogManager::VBO_MANAGER, "Instancing Culled Rigged Shadow locations set." );
}
//
//=======================================================================================
//
void VboManager::setBumpedInstancingLocations(	string&		bump_shader_name,
												string&		bump_positions_name,
												string&		bump_rotations_name,
												string&		bump_normal_name,
												string&		bump_tangent_name,
												string&		bump_texCoord0_name,
												string&		bump_viewmat_name
											 )
{
	VboManager::bump_shader_name			= string( bump_shader_name    );
	VboManager::bump_positions_name			= string( bump_positions_name );
	VboManager::bump_rotations_name			= string( bump_rotations_name );
	VboManager::bump_normal_name			= string( bump_normal_name    );
	VboManager::bump_tangent_name			= string( bump_tangent_name   );
	VboManager::bump_texCoord0_name			= string( bump_texCoord0_name );
	VboManager::bump_viewmat_name			= string( bump_viewmat_name   );

	bumped_instancing						= glsl_manager->getId	( bump_shader_name    );
	bumped_instancing_uPosLocation			= glGetUniformLocation	( bumped_instancing,
																	  bump_positions_name.c_str() );
	bumped_instancing_uRotLocation			= glGetUniformLocation	( bumped_instancing,
																	  bump_rotations_name.c_str() );
	bumped_instancing_normalLocation		= glGetAttribLocation	( bumped_instancing,
																	  bump_normal_name.c_str()    );
	bumped_instancing_tangentLocation		= glGetAttribLocation	( bumped_instancing,
																	  bump_tangent_name.c_str()   );
	bumped_instancing_texCoord0Location		= glGetAttribLocation	( bumped_instancing,
																	  bump_texCoord0_name.c_str() );
	log_manager->log( LogManager::VBO_MANAGER, "Bumped Instancing locations set." );
}
//
//=======================================================================================
