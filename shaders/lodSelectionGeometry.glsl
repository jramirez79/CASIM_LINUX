//#version 150 compatibility
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_geometry_shader4 : enable

uniform float lod;
uniform float groupId;

void main( void )
{
	vec4 pos = gl_PositionIn[0];

	if ( lod == pos.w && pos.y == groupId )
	{
		gl_Position = pos;
		EmitVertex();
		EndPrimitive();
	}
}
