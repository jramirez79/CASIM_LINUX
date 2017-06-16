
#extension GL_EXT_gpu_shader4: enable

uniform samplerBuffer posTextureBuffer;
uniform float PLANE_SCALE;
varying vec3 color;

void main()
{
	color = abs( texelFetchBuffer( posTextureBuffer, gl_VertexID ).xyz ) / PLANE_SCALE;
	gl_Position = ftransform();
}
