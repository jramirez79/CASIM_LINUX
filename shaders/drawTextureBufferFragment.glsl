
varying vec3 color;
varying float depthZ;

void main()
{
	gl_FragColor = vec4( color, 1.0 );
	//gl_FragColor = vec4( vec3(depthZ), 1.0 );
}
