
#extension GL_ARB_uniform_buffer_object: enable
#extension GL_EXT_gpu_shader4: enable
#extension GL_ARB_draw_instanced: enable

#define NUM_LIGHTS		1
#define DEG2RAD			0.01745329251994329576

attribute vec2 texCoord0;
attribute vec3 normal;

varying vec3 lightVec[ NUM_LIGHTS ];
varying vec3 normalVec;

uniform samplerBuffer posTextureBuffer;
uniform mat4 ViewMat4x4;

mat4 cenital = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
mat4 azimuth = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );

mat4 rotationToMatrix( vec2 rotation )
{
	float sinC = sin( rotation.x );
	float cosC = cos( rotation.x );
	float sinA = sin( rotation.y );
	float cosA = cos( rotation.y );
	cenital[1][1] =  cosC;
	cenital[1][2] =  sinC;
	cenital[2][1] = -sinC;
	cenital[2][2] =  cosC;
	azimuth[0][0] =  cosA;
	azimuth[0][2] = -sinA;
	azimuth[2][0] =  sinA;
	azimuth[2][2] =  cosA;
	return azimuth * cenital;
}

void main( void )
{
	gl_TexCoord[ 0 ].st		= texCoord0.xy;

	mat4 transMat4x4;
	vec4 pos				= texelFetchBuffer( posTextureBuffer, gl_InstanceID );
	vec4 position			= vec4( pos.x, -30.0, pos.y, 1.0 );
	vec2 rotation			= vec2( 90.0*DEG2RAD, 0.0 );

	transMat4x4				= rotationToMatrix( rotation );
	transMat4x4[ 3 ][ 0 ]	= position.x;
	transMat4x4[ 3 ][ 1 ]	= position.y;
	transMat4x4[ 3 ][ 2 ]	= position.z;

	mat4 modelViewMat		= ViewMat4x4 * transMat4x4;
	vec4 P					= modelViewMat * gl_Vertex;
	normalVec				= normal;
	gl_Position				= gl_ProjectionMatrix * P;

	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		lightVec[l]			= normalize( gl_LightSource[l].position.xyz - P.xyz );
	}
}
