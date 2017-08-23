varying	float	depthZ;
void main( void )
{
	gl_FragColor = vec4( 0.2, 0.2, 0.2, 1.0 );
	gl_FragDepth = depthZ;
	
}
