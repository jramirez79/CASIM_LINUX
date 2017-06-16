
#define NUM_LIGHTS		1

uniform sampler2D diffuseTexture;
uniform sampler2D zonesTexture;
uniform sampler2D weightsTexture;

varying vec3 lightVec[NUM_LIGHTS];
varying vec3 normalVec;

void main( void )
{
	vec4 diffuseMaterial = texture2D( diffuseTexture, gl_TexCoord[0].st );
	//vec4 diffuseMaterial = texture2DLod( zonesTexture, gl_TexCoord[0].st, 1.0 );
	//vec4 diffuseMaterial = texture2DLod( weightsTexture, gl_TexCoord[0].st, 1.0 );
	float ambient = 0.0;
	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		ambient = max( gl_LightSource[l].ambient.r, ambient );
		float lambertFactor = max( dot( lightVec[l], normalVec ), ambient );
		lightVec[l] *= vec3( -1 );
		float lambertFactorNeg = max( dot( lightVec[l], normalVec ), ambient );
		vec4 diffuseLight = gl_LightSource[l].diffuse;
		gl_FragColor += diffuseMaterial * diffuseLight * ( lambertFactor + lambertFactorNeg );
	}
	gl_FragColor /= NUM_LIGHTS;
	gl_FragColor = diffuseMaterial;
}
