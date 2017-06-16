
attribute vec4 location;
attribute vec2 texCoord0;

varying vec2 vTexCoord0;

void main( void )
{
	vTexCoord0		= texCoord0;
	vec3 vNormal	= normalize( gl_NormalMatrix * gl_Normal );
	// calculate the angle eye-position - vertex - light direction
	// the angle must not be less than 0.0
	float fAngle	= max( 0.0, dot(vNormal, vec3( gl_LightSource[0].halfVector )) );
	// calculate the vertex shininess as the calculated angle powered to the materials shininess factor
	float shine		= pow( fAngle, gl_FrontMaterial.shininess );
	// calculate the vertex color with the normal gouraud lighting calculation
	gl_FrontColor	= gl_LightSource[0].ambient * gl_FrontMaterial.ambient +
       			      gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * fAngle +
		              gl_LightSource[0].specular * gl_FrontMaterial.specular * shine;
	// transform the vertex by the current opengl transformation matrix		
	gl_Position		= gl_ProjectionMatrix * gl_ModelViewMatrix * location;
}
