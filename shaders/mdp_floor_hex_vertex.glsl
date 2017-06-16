#version 130
#extension GL_ARB_texture_rectangle : require

varying float			level;
varying float			intensity;
varying vec2			policoord;

uniform sampler2DRect	policy;
uniform sampler2DRect	density;
uniform float			fbo_width;
uniform float			fbo_height;
uniform float			policy_width;
uniform float			policy_height;
uniform float			tile_height;
uniform float			tile_side;

void main()
{
//->MANAGE_GLSL
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	vec4 vertex		= gl_Vertex;
	float wFactor	= fbo_width / (policy_width * tile_side);
//<-MANAGE_GLSL

//->MANAGE_POLICY
	policoord.x		= wFactor * policy_width * gl_TexCoord[0].s / fbo_width;
	policoord.y		= policy_height * gl_TexCoord[0].t / fbo_height;
	level			= texture2DRect( policy, policoord ).x;
	intensity		= texture2DRect( density, policoord ).x / 20.0;
	intensity		= clamp( intensity, 0.0, 1.0 );
//<-MANAGE_POLICY

//->MANAGE_HEX_GRID
	policoord.x		= trunc( policoord.x );
	policoord.y		= trunc( policoord.y );
	float fmod		= policoord.x - 2.0 * floor( policoord.x / 2.0 );
	if( fmod == 1.0 )
	{
		vertex.y   -= tile_height / 2.0f;
	}
//<-MANAGE_HEX_GRID

	gl_Position		= gl_ModelViewProjectionMatrix * vertex;
}
