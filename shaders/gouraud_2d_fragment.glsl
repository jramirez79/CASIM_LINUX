uniform sampler2D diffuseTexture;

varying vec2 vTexCoord0;

void main( void )
{
	gl_FragColor = texture2D( diffuseTexture, vTexCoord0.xy );
}
