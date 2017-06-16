
#define NUM_LIGHTS		4

uniform int textured;

varying vec3 lightVec[ NUM_LIGHTS ];
varying vec3 N;
varying vec3 E;

void main( void )
{
	if( textured == 1 )
	{
		gl_TexCoord[0]	= gl_MultiTexCoord0;
	}
	N				= gl_NormalMatrix * gl_Normal;
	vec4 P			= (gl_ModelViewMatrix * gl_Vertex);
	E				= normalize( P );
	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		lightVec[l]	= normalize( gl_LightSource[l].position.xyz - E.xyz );
	}
	gl_Position		= ftransform();
}
