
#define NUM_LIGHTS		4

uniform sampler2D diffuseTexture;

varying vec3 lightVec[NUM_LIGHTS];
varying vec3 normalVec;

void main( void )
{
	vec4 diffuseMaterial = texture2D( diffuseTexture, gl_TexCoord[0].st );
	float ambient = 0.0;
	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		ambient = max( gl_LightSource[l].ambient.r, ambient );
		float lambertFactor = max( dot( lightVec[l], normalVec ), ambient );
		lightVec[l] *= vec3( -1 );
		float lambertFactorNeg = max( dot( lightVec[l], normalVec ), ambient );
		//vec4 diffuseLight = gl_LightSource[l].diffuse;
		//gl_FragColor += diffuseMaterial * diffuseLight * ( lambertFactor + lambertFactorNeg );
		gl_FragColor += diffuseMaterial * ( lambertFactor + lambertFactorNeg );
	}
	gl_FragColor /= NUM_LIGHTS;
}

