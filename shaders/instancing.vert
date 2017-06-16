
#extension GL_ARB_uniform_buffer_object: enable
#extension GL_EXT_gpu_shader4: enable
#extension GL_ARB_draw_instanced: enable

#define NUM_INSTANCES	1000
#define NUM_LIGHTS		4

attribute vec2 texCoord0;
attribute vec3 normal;

varying vec3 lightVec[ NUM_LIGHTS ];
varying vec3 normalVec;

bindable uniform vec4 positions	[ NUM_INSTANCES ];
bindable uniform vec4 rotations	[ NUM_INSTANCES ];
bindable uniform vec4 scales	[ NUM_INSTANCES ];
uniform mat4 ViewMat4x4;

mat4 cenital = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
mat4 azimuth = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );

mat4 rotationToMatrix( vec2 rotation )
{
	float sinC = sin( rotation.x );
	float cosC = cos( rotation.x );
	float sinA = sin( rotation.y );
	float cosA = cos( rotation.y );	
	cenital[1][1] = cosC;
	cenital[1][2] = sinC;
	cenital[2][1] = -sinC;
	cenital[2][2] = cosC;
	azimuth[0][0] = cosA;
	azimuth[0][2] = -sinA;
	azimuth[2][0] = sinA;
	azimuth[2][2] = cosA;
	return azimuth * cenital;
}

void main( void )
{
	gl_TexCoord[ 0 ].st		= texCoord0.xy;

	mat4 transMat4x4;
	vec3 position			= positions[ gl_InstanceID ].xyz;
	vec2 rotation			= rotations[ gl_InstanceID ].xy;
	mat4 scaleMat			= mat4( 1.0 );
	scaleMat[ 0 ][ 0 ]		= scales[ gl_InstanceID ].x;
	scaleMat[ 1 ][ 1 ]		= scales[ gl_InstanceID ].y;
	scaleMat[ 2 ][ 2 ]		= scales[ gl_InstanceID ].z;

	transMat4x4				= rotationToMatrix( rotation );
	transMat4x4[ 3 ][ 0 ]	= position.x;
	transMat4x4[ 3 ][ 1 ]	= position.y;
	transMat4x4[ 3 ][ 2 ]	= position.z;

	mat4 modelViewMat		= ViewMat4x4 * transMat4x4;
	vec4 P					= modelViewMat * scaleMat * gl_Vertex;
	normalVec				= normalize( gl_NormalMatrix * normal );
	gl_Position				= gl_ProjectionMatrix * P;
	
	vec3 vertexPosition		= P.xyz/P.w;

	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		lightVec[l]			= normalize( gl_LightSource[l].position.xyz - vertexPosition );
	}
}

