
#define NUM_LIGHTS		4

uniform sampler2D diffuseTexture;
uniform int textured;
uniform vec3 tint;
uniform float amb;

varying vec3 lightVec[NUM_LIGHTS];
varying vec3 N;
varying vec3 E;

void main( void )
{
	vec4 diffuseMaterial = vec4( tint, 1 );
	if( textured > 0 )
	{
		diffuseMaterial	*= texture2D( diffuseTexture, gl_TexCoord[0].st );
	}
	
	float NdotL; //cosin normal lightDir
	float HdotN; //cosin half way vector normal
	vec3 lightDir;
	vec3 halfVector;
	vec4 diffuseC;
	vec4 specularC;
	gl_FragColor = diffuseMaterial * vec4( vec3( amb ), 1 );
	for( int l = 0; l < NUM_LIGHTS; l++ )
	{
		//lightDir = normalize(vec3(gl_LightSource[l].position));
		lightDir = lightVec[l];
		NdotL = max( dot( N, lightDir ), 0.0 );
		diffuseC = diffuseMaterial * gl_LightSource[l].diffuse * NdotL;
		gl_FragColor += diffuseC;
		/*
		if( NdotL > 0 )
		{
			halfVector = normalize( lightDir - E );
			HdotN = max( 0, dot( N, halfVector ) );
			specularC = gl_FrontMaterial.specular * gl_LightSource[l].specular * pow( HdotN, gl_FrontMaterial.shininess );
			gl_FragColor += specularC;
		}
		*/
	}
	gl_FragColor.a = 1.0;
	//gl_FragColor = diffuseMaterial;
	//gl_FragColor /= float(NUM_LIGHTS);
}
