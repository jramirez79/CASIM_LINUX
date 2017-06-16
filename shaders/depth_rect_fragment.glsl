
uniform sampler2DRect	depth;
uniform vec2			camrange;

float LinearizeDepth( float zoverw )
{
	return (2.0 * camrange.x) / (camrange.y + camrange.x - zoverw * (camrange.y - camrange.x));
}

void main()
{
	float depth_value = LinearizeDepth( texture2DRect( depth, gl_TexCoord[0].st ).r );
	gl_FragColor = vec4( depth_value, depth_value, depth_value, 1 );
	//gl_FragColor = vec4 (1,0,0,1);

}
