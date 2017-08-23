
uniform sampler2DRect	depth;
uniform vec2			camrange;
varying float dpth;


float LinearizeDepth( float zoverw )
{
	return (2.0 * camrange.x) / (camrange.y + camrange.x - zoverw * (camrange.y - camrange.x));
}

void main()
{
	//float depth_value = LinearizeDepth( texture2DRect( depth, gl_TexCoord[0].st ).r );
	
	float depth_value = texture2DRect( depth, gl_TexCoord[0].st ).r;
	//float depth_value = 0.0
	gl_FragColor = vec4( depth_value, depth_value, depth_value, 1 );
	//gl_FragColor = vec4 (1,0,0,1);

}
