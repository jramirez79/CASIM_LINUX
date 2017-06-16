
uniform sampler2D diffuseTexture;

void main( void )
{	
	gl_TexCoord[0] = gl_MultiTexCoord0; 
	
	vec3 v3Normal;		
		
	float fAngle;
	float fShininessFactor;
	
	// transform the vertext normal the same the object is transformed
	v3Normal   = gl_NormalMatrix * gl_Normal;

	// set normal length to 1.
	v3Normal   = normalize(v3Normal);

	// calculate the angle eye-position - vertex - light direction
	// the angle must not be less than 0.0
	fAngle = max(0.0, dot(v3Normal, vec3(gl_LightSource[0].halfVector)));
	
	// calculate the vertex shininess as the calculated angle powered to the materials shininess factor
	fShininessFactor = pow(fAngle, gl_FrontMaterial.shininess);
	
	vec4 diffuseC = texture2D( diffuseTexture, gl_TexCoord[0].st );
	// calculate the vertex color with the normal gouraud lighting calculation
	gl_FrontColor = gl_LightSource[0].ambient * gl_FrontMaterial.ambient +
       			    gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * fAngle +
		            gl_LightSource[0].specular * gl_FrontMaterial.specular * fShininessFactor;
	gl_FrontColor = diffuseC;

	// transform the vertex by the current opengl transformation matrix		
	gl_Position = ftransform();
}
