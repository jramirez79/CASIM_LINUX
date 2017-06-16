
#version 120
#extension GL_EXT_geometry_shader4 : enable

void main( void )
{   
	gl_TexCoord[0].s = float( gl_VertexID );
	gl_Position = gl_Vertex;
}
