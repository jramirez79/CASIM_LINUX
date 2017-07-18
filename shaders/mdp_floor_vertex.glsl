#version 130
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect policy;
uniform sampler2DRect density;

uniform float fbo_width;
uniform float fbo_height;
uniform float policy_width;
uniform float policy_height;

varying float level;
varying float intensity;
varying vec2 policoord;
varying float depthZ;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec2 texcoord;
	policoord.x = policy_width  * gl_TexCoord[0].s / fbo_width;
	policoord.y = policy_height * gl_TexCoord[0].t / fbo_height;
	level = texture2DRect( policy, policoord ).x;
	intensity = texture2DRect( density, policoord ).x / 20.0;
	intensity = clamp( intensity, 0.0, 1.0 );
	gl_Position = ftransform();
	depthZ = gl_Position.z; 
}
