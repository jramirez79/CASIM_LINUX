
uniform sampler2DRect	screen;
uniform int 			cmp_flg;

void main (void)
{
	vec4 color 		= texture2DRect( screen, gl_TexCoord[0].st ).rgba; 
	
	
	float C = 1.0;
	float zNear = 0.01;
	float zFar = 300.0;	
	gl_FragDepth 	= color.a;
	if(cmp_flg)
		gl_FragDepth	= (-1.0*color.a*zNear)/(color.a*(zFar-zNear)-zFar);
	
	//gl_FragColor 	= vec4 (gl_FragDepth+0.1,gl_FragDepth+0.1,gl_FragDepth+0.1, 1.0 );
	
	gl_FragColor 	= vec4 (color.rgb, 1.0 );
	//gl_FragDepth 	= color.a;
}