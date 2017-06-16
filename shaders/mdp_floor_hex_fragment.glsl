#version 130
#extension GL_EXT_texture_array : enable

varying float			level;
varying float			intensity;
varying vec2			policoord;

uniform sampler2D		layer0;
uniform sampler2D		layer1;
uniform sampler2DArray	arrowsTA;

uniform float			tile_side;
uniform float			tile_height;
uniform float			policy_width;

uniform float			policy_on;
uniform float			density_on;

void main()
{
	vec2 texUV;
	texUV.x			= (gl_TexCoord[0].s / tile_side)   - policoord.x;
	texUV.y			= (gl_TexCoord[0].t / tile_height) - policoord.y;

	vec4 color		= vec4( 0.0 );
	vec4 l0color	= texture2D( layer0, texUV );

	vec4 arrow		= vec4( 0.0 );
	if( policoord.x < policy_width && policy_on > 0.0f )
	{
		arrow		= texture2DArray( arrowsTA, vec3( texUV, level ) );
	}
	else
	{
		arrow		= texture2DArray( arrowsTA, vec3( texUV, 6.0 ) );
	}
	vec4 l1color	= texture2D( layer1, texUV );
	if( density_on > 0.0f )
	{
		float factor = 1.0 - intensity / 2.0;
		if( intensity > 0.1 && intensity < 0.33 )
		{
			l1color.rb *= factor;	
		}
		else if ( intensity < 0.66 )
		{
			l1color.b *= factor;
		}
		else
		{
			l1color.gb *= factor;
		}
	}

	color			= (1.0 - arrow.a) * l0color + arrow.a * l1color;

	gl_FragColor	= color;
}
