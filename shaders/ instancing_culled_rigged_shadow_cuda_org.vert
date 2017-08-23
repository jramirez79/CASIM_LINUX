#version 130
#extension GL_ARB_texture_rectangle : require
#extension GL_ARB_uniform_buffer_object: enable
#extension GL_EXT_gpu_shader4: enable
#extension GL_ARB_draw_instanced: enable

#define DEG2RAD			0.01745329251994329576

in vec2			texCoord0;
in vec3			normalV;

uniform sampler2DArray	riggingMT;
uniform sampler2DArray	animationMT;
uniform samplerBuffer	idsTextureBuffer;
uniform samplerBuffer	posTextureBuffer;

uniform float			zNear;
uniform float			zFar;

uniform mat4			ViewMat4x4;
uniform mat4			ProjMat4x4;
uniform mat4			ShadowMat4x4;
uniform vec3			camPos;
uniform int				AGENTS_NPOT;
uniform int				ANIMATION_LENGTH;
uniform int				STEP;
uniform float			lod;
out	float				depthZ;

mat4 cenital = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
mat4 azimuth = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );

float FRAME = 0.0;

//=======================================================================================
// Explicit bilinear texture lookup to circumvent bad hardware precision.
// The extra arguments specify the dimension of the texture. (GLSL 1.30
// introduced textureSize() to get that information from the sampler.)
// 'dims' is the width and height of the texture, 'one' is 1.0/dims.
// (Precomputing 'one' saves two divisions for each lookup.)
vec4 texture2D_bilinear(sampler2D tex, vec2 st, vec2 dims, vec2 one) {
	vec2 uv = st * dims;
	vec2 uv00 = floor(uv - vec2(0.5)); // Lower left corner of lower left texel
	vec2 uvlerp = uv - uv00 - vec2(0.5); // Texel-local lerp blends [0,1]
	vec2 st00 = (uv00 + vec2(0.5)) * one;
	vec4 texel00 = texture2D(tex, st00);
	vec4 texel10 = texture2D(tex, st00 + vec2(one.x, 0.0));
	vec4 texel01 = texture2D(tex, st00 + vec2(0.0, one.y));
	vec4 texel11 = texture2D(tex, st00 + one);
	vec4 texel0 = mix(texel00, texel01, uvlerp.y); 
	vec4 texel1 = mix(texel10, texel11, uvlerp.y); 
	return mix(texel0, texel1, uvlerp.x);
}
//=======================================================================================
// Explicit bilinear texture lookup to circumvent bad hardware precision.
// The extra arguments specify the dimension of the texture. (GLSL 1.30
// introduced textureSize() to get that information from the sampler.)
// 'dims' is the width and height of the texture, 'one' is 1.0/dims.
// (Precomputing 'one' saves two divisions for each lookup.)
vec4 texture2D_bilinear2(sampler2DArray tex, int level, vec2 st, vec2 dims, vec2 one) {
	vec2 uv = st * dims;
	vec2 uv00 = floor(uv - vec2(0.5)); // Lower left corner of lower left texel
	vec2 uvlerp = uv - uv00 - vec2(0.5); // Texel-local lerp blends [0,1]
	vec2 st00 = (uv00 + vec2(0.5)) * one;
	vec4 texel00 = texture2DArray(tex, vec3(st00,level));
	vec4 texel10 = texture2DArray(tex, vec3(st00 + vec2(one.x, 0.0),level));
	vec4 texel01 = texture2DArray(tex, vec3(st00 + vec2(0.0, one.y),level));
	vec4 texel11 = texture2DArray(tex, vec3(st00 + one,level));
	vec4 texel0 = mix(texel00, texel01, uvlerp.y); 
	vec4 texel1 = mix(texel10, texel11, uvlerp.y); 
	return mix(texel0, texel1, uvlerp.x);
}
//=======================================================================================
mat4 rotationToMatrix( vec2 rotation )
{
	float sinC = sin( rotation.x );
	float cosC = cos( rotation.x );
	float sinA = sin( rotation.y );
	float cosA = cos( rotation.y );
	cenital[1][1] =  cosC;
	cenital[1][2] =  sinC;
	cenital[2][1] = -sinC;
	cenital[2][2] =  cosC;
	azimuth[0][0] =  cosA;
	azimuth[0][2] = -sinA;
	azimuth[2][0] =  sinA;
	azimuth[2][2] =  cosA;
	return azimuth * cenital;
}
//=======================================================================================
mat4 matFromMT( sampler2DArray tex, float boneID, float frame )
{
	return mat4(	texture2DArray( tex, vec3( 0.01/4.0, (boneID+0.001)/42.0, frame ) ),
					texture2DArray( tex, vec3( 1.01/4.0, (boneID+0.001)/42.0, frame ) ),
					texture2DArray( tex, vec3( 2.01/4.0, (boneID+0.001)/42.0, frame ) ),
					texture2DArray( tex, vec3( 3.01/4.0, (boneID+0.001)/42.0, frame ) ) );
}
//=======================================================================================
void main( void )
{
	gl_TexCoord[ 0 ].st		= texCoord0.xy;

	int ss,tt;
	vec4 ids				= texelFetchBuffer( idsTextureBuffer, gl_InstanceID );
	float agent_id			= ids.x;
	vec4 inputv				= texelFetchBuffer( posTextureBuffer, int(agent_id) );					// Get current instance 'global' position given the character Id
	ss						= int(ids.x) / AGENTS_NPOT;
	tt						= int(ids.x) - ss * AGENTS_NPOT;
	vec3 ti					= vec3(float(ss),float(tt),0.0);
	vec4 position			= vec4( inputv.x, 5.0, inputv.z, 1.0 );							// Y is fixed to -30.0 for now...
	vec2 rotation			= vec2( 0.0, inputv.w );												// Model orientation.

	float f = ids.x;	
	vec3 weightVal			= texture2DArray( riggingMT, vec3(gl_TexCoord[0].st, 1) ).rgb;
	vec2 disp				= 1.0 - texture2D_bilinear2(	riggingMT, 
															2, 
															gl_TexCoord[0].st, 
															vec2(2048, 2048), 
															vec2(0.00048828125, 0.00048828125)).ra;
	disp.r					= (agent_id / float(AGENTS_NPOT*AGENTS_NPOT)) * (2.0 * disp.r - 1.0);														// Displacement in [-1,1].
	vec3 zoneVal			= texture2DArray( riggingMT,  vec3(gl_TexCoord[0].st, 0) ).rgb;			// Current zone.
	int zone1				= int( round( zoneVal.r * 255.0 ) );

	vec4 tempva				= vec4( 0.0, 0.0, 0.0, 0.0 );
	float rem_rgb			= (1.0 - (weightVal.r + weightVal.g + weightVal.b))/3.0;
	weightVal.r += rem_rgb;
	weightVal.g += rem_rgb; 
	weightVal.b += rem_rgb;
	vec4 vertex = gl_Vertex;
	vec4 normal = vec4(normalV,0.0);
	vertex.xyz = vertex.xyz + (normalV.xyz*disp.r*4.0);												// the four value came from Maya's transfer maps -> Displacement map -> Maximum Value
	vertex.w = 1.0;

	FRAME = float((int(ids.x) + STEP) % ANIMATION_LENGTH);

/*
	if(zone1==1){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matL_Thigh			=	matFromMT( animationMT, 34.0, FRAME );
		tempva	= (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matL_Thigh)  * vertex;
		normal  = (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matL_Thigh)  * normal;
	}
	else if(zone1==2){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matR_Thigh			=	matFromMT( animationMT, 38.0, FRAME );
		tempva	= (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matR_Thigh) * vertex;
		normal  = (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matR_Thigh) * normal;
	}
	else if(zone1==3){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		tempva	= (weightVal.g * matSpine + weightVal.r * matPelvis + weightVal.b * matSpine1) * vertex;
		normal  = (weightVal.g * matSpine + weightVal.r * matPelvis + weightVal.b * matSpine1) * normal;
	}
	else if(zone1==4){
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		tempva	= (weightVal.b * matSpine1 + weightVal.g * matSpine + weightVal.r * matSpine2) * vertex;
		normal  = (weightVal.b * matSpine1 + weightVal.g * matSpine + weightVal.r * matSpine2) * normal;
	}
	else if(zone1==5){
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		tempva	= (weightVal.r * matSpine2 +weightVal.b * matSpine1 + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matSpine2 +weightVal.b * matSpine1 + weightVal.g * matNeck) * normal;
	}
	else if(zone1==6){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matL_Clavicle		=	matFromMT( animationMT,  6.0, FRAME );
		tempva	= (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matL_Clavicle) * vertex;
		normal  = (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matL_Clavicle) * normal;
	}
	else if(zone1==7){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matR_Clavicle		=	matFromMT( animationMT, 20.0, FRAME );
		tempva	= (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matR_Clavicle) * vertex;
		normal  = (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matR_Clavicle) * normal;
	}
	else if(zone1==8){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matHead			=	matFromMT( animationMT,  5.0, FRAME );
		tempva	= (weightVal.b * matHead * matID + weightVal.g * matNeck + weightVal.r * matSpine2)* vertex;
		normal  = (weightVal.b * matHead * matID + weightVal.g * matNeck + weightVal.r * matSpine2)* normal;
	}
	else if(zone1==9){
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matL_Clavicle		=	matFromMT( animationMT,  6.0, FRAME );
		mat4 matL_UpperArm		=	matFromMT( animationMT,  7.0, FRAME );
		tempva	= (weightVal.r * matL_UpperArm + weightVal.b * matL_Clavicle + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matL_UpperArm + weightVal.b * matL_Clavicle + weightVal.g * matNeck) * normal;
	}
	else if(zone1==10){
		mat4 matL_UpperArm		=	matFromMT( animationMT,  7.0, FRAME );
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		tempva	= (weightVal.g * matL_Forearm + weightVal.r * matL_UpperArm + weightVal.b * matL_Hand) * vertex;
		normal  = (weightVal.g * matL_Forearm + weightVal.r * matL_UpperArm + weightVal.b * matL_Hand) * normal;
	}
	else if(zone1==11){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger0		=	matFromMT( animationMT, 10.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger0) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger0) * normal;
	}
	else if(zone1==12){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger3		=	matFromMT( animationMT, 16.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger3) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger3) * normal;
	}
	else if(zone1==13){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger4		=	matFromMT( animationMT, 18.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger4) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger4) * normal;
	}
	else if(zone1==14){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger0		=	matFromMT( animationMT, 10.0, FRAME );
		mat4 matL_Finger01		=	matFromMT( animationMT, 11.0, FRAME );
		tempva	= (weightVal.r * matL_Finger0 + weightVal.b * matL_Hand + weightVal.g * matL_Finger01) * vertex;
		normal  = (weightVal.r * matL_Finger0 + weightVal.b * matL_Hand + weightVal.g * matL_Finger01) * normal;
	}
	else if(zone1==15){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger1		=	matFromMT( animationMT, 12.0, FRAME );
		mat4 matL_Finger11		=	matFromMT( animationMT, 13.0, FRAME );
		tempva	= (weightVal.r * matL_Finger1 + weightVal.b * matL_Hand + weightVal.g * matL_Finger11) * vertex;
		normal  = (weightVal.r * matL_Finger1 + weightVal.b * matL_Hand + weightVal.g * matL_Finger11) * normal;
	}
	else if(zone1==16){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger2		=	matFromMT( animationMT, 14.0, FRAME );
		mat4 matL_Finger21		=	matFromMT( animationMT, 15.0, FRAME );
		tempva	= (weightVal.r * matL_Finger2 + weightVal.b * matL_Hand + weightVal.g * matL_Finger21) * vertex;
		normal  = (weightVal.r * matL_Finger2 + weightVal.b * matL_Hand + weightVal.g * matL_Finger21) * normal;
	}
	else if(zone1==17){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger3		=	matFromMT( animationMT, 16.0, FRAME );
		mat4 matL_Finger31		=	matFromMT( animationMT, 17.0, FRAME );
		tempva	= (weightVal.r * matL_Finger3 + weightVal.b * matL_Hand + weightVal.g * matL_Finger31) * vertex;
		normal  = (weightVal.r * matL_Finger3 + weightVal.b * matL_Hand + weightVal.g * matL_Finger31) * normal;
	}
	else if(zone1==18){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger4		=	matFromMT( animationMT, 18.0, FRAME );
		mat4 matL_Finger41		=	matFromMT( animationMT, 19.0, FRAME );
		tempva	= (weightVal.r * matL_Finger4 + weightVal.b * matL_Hand + weightVal.g * matL_Finger41) * vertex;
		normal  = (weightVal.r * matL_Finger4 + weightVal.b * matL_Hand + weightVal.g * matL_Finger41) * normal;
	}
	else if(zone1==19){
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matR_Clavicle		=	matFromMT( animationMT, 20.0, FRAME );
		mat4 matR_UpperArm		=	matFromMT( animationMT, 21.0, FRAME );
		tempva	= (weightVal.r * matR_UpperArm + weightVal.b * matR_Clavicle + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matR_UpperArm + weightVal.b * matR_Clavicle + weightVal.g * matNeck) * normal;
	}
	else if(zone1==20){
		mat4 matR_UpperArm		=	matFromMT( animationMT, 21.0, FRAME );
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		tempva	= (weightVal.g * matR_Forearm + weightVal.r * matR_UpperArm + weightVal.b * matR_Hand) * vertex;
		normal  = (weightVal.g * matR_Forearm + weightVal.r * matR_UpperArm + weightVal.b * matR_Hand) * normal;
	}
	else if(zone1==21){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger0		=	matFromMT( animationMT, 24.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger0) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger0) * normal;
	}
	else if(zone1==22){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger3		=	matFromMT( animationMT, 30.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger3) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger3) * normal;
	}
	else if(zone1==23){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger4		=	matFromMT( animationMT, 32.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger4) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger4) * normal;
	}
	else if(zone1==24){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger0		=	matFromMT( animationMT, 24.0, FRAME );
		mat4 matR_Finger01		=	matFromMT( animationMT, 25.0, FRAME );
		tempva	= (weightVal.r * matR_Finger0 + weightVal.b * matR_Hand + weightVal.g * matR_Finger01) * vertex;
		normal  = (weightVal.r * matR_Finger0 + weightVal.b * matR_Hand + weightVal.g * matR_Finger01) * normal;
	}
	else if(zone1==25){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger1		=	matFromMT( animationMT, 26.0, FRAME );
		mat4 matR_Finger11		=	matFromMT( animationMT, 27.0, FRAME );
		tempva	= (weightVal.r * matR_Finger1 + weightVal.b * matR_Hand + weightVal.g * matR_Finger11) * vertex;
		normal  = (weightVal.r * matR_Finger1 + weightVal.b * matR_Hand + weightVal.g * matR_Finger11) * normal;
	}
	else if(zone1==26){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger2		=	matFromMT( animationMT, 28.0, FRAME );
		mat4 matR_Finger21		=	matFromMT( animationMT, 29.0, FRAME );
		tempva	= (weightVal.r * matR_Finger2 + weightVal.b * matR_Hand + weightVal.g * matR_Finger21) * vertex;
		normal  = (weightVal.r * matR_Finger2 + weightVal.b * matR_Hand + weightVal.g * matR_Finger21) * normal;
	}
	else if(zone1==27){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger3		=	matFromMT( animationMT, 30.0, FRAME );
		mat4 matR_Finger31		=	matFromMT( animationMT, 31.0, FRAME );
		tempva	= (weightVal.r * matR_Finger3 + weightVal.b * matR_Hand + weightVal.g * matR_Finger31) * vertex;
		normal  = (weightVal.r * matR_Finger3 + weightVal.b * matR_Hand + weightVal.g * matR_Finger31) * normal;
	}
	else if(zone1==28){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger4		=	matFromMT( animationMT, 32.0, FRAME );
		mat4 matR_Finger41		=	matFromMT( animationMT, 33.0, FRAME );
		tempva	= (weightVal.r * matR_Finger4 + weightVal.b * matR_Hand + weightVal.g * matR_Finger41) * vertex;
		normal  = (weightVal.r * matR_Finger4 + weightVal.b * matR_Hand + weightVal.g * matR_Finger41) * normal;
	}
	else if(zone1==29){
		mat4 matL_Thigh			=	matFromMT( animationMT, 34.0, FRAME );
		mat4 matL_Calf			=	matFromMT( animationMT, 35.0, FRAME );
		mat4 matL_Foot			=	matFromMT( animationMT, 36.0, FRAME );
		tempva	= (weightVal.r * matL_Calf + weightVal.b * matL_Thigh + weightVal.g * matL_Foot) * vertex;
		normal  = (weightVal.r * matL_Calf + weightVal.b * matL_Thigh + weightVal.g * matL_Foot) * normal;
	}
	else if(zone1==30){
		mat4 matL_Calf			=	matFromMT( animationMT, 35.0, FRAME );
		mat4 matL_Foot			=	matFromMT( animationMT, 36.0, FRAME );
		mat4 matL_Toe0			=	matFromMT( animationMT, 37.0, FRAME );
		tempva	= (weightVal.g * matL_Foot + weightVal.r * matL_Calf + weightVal.b * matL_Toe0) * vertex;
		normal  = (weightVal.g * matL_Foot + weightVal.r * matL_Calf + weightVal.b * matL_Toe0) * normal;
	}
	else if(zone1==31){
		mat4 matR_Thigh			=	matFromMT( animationMT, 38.0, FRAME );
		mat4 matR_Calf			=	matFromMT( animationMT, 39.0, FRAME );
		mat4 matR_Foot			=	matFromMT( animationMT, 40.0, FRAME );
		tempva	= (weightVal.r * matR_Calf + weightVal.b * matR_Thigh + weightVal.g * matR_Foot) * vertex;
		normal  = (weightVal.r * matR_Calf + weightVal.b * matR_Thigh + weightVal.g * matR_Foot) * normal;
	}
	else if(zone1==32){
		mat4 matR_Calf			=	matFromMT( animationMT, 39.0, FRAME );
		mat4 matR_Foot			=	matFromMT( animationMT, 40.0, FRAME );
		mat4 matR_Toe0			=	matFromMT( animationMT, 41.0, FRAME );
		tempva	= (weightVal.g * matR_Foot + weightVal.r * matR_Calf + weightVal.b * matR_Toe0) * vertex;
		normal  = (weightVal.g * matR_Foot + weightVal.r * matR_Calf + weightVal.b * matR_Toe0) * normal;
	}
*/


	if(zone1==1){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matL_Thigh			=	matFromMT( animationMT, 34.0, FRAME );
		tempva	= (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matL_Thigh)  * vertex;
		normal  = (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matL_Thigh)  * normal;
	}
	else if(zone1==2){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matR_Thigh			=	matFromMT( animationMT, 38.0, FRAME );
		tempva	= (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matR_Thigh) * vertex;
		normal  = (weightVal.r * matPelvis + weightVal.g * matSpine + weightVal.b * matR_Thigh) * normal;
	}
	else if(zone1==3){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		tempva	= (weightVal.g * matSpine + weightVal.r * matPelvis + weightVal.b * matSpine1) * vertex;
		normal  = (weightVal.g * matSpine + weightVal.r * matPelvis + weightVal.b * matSpine1) * normal;
	}
	else if(zone1==4){
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		tempva	= (weightVal.b * matSpine1 + weightVal.g * matSpine + weightVal.r * matSpine2) * vertex;
		normal  = (weightVal.b * matSpine1 + weightVal.g * matSpine + weightVal.r * matSpine2) * normal;
	}
	else if(zone1==5){
		mat4 matSpine1			=	matFromMT( animationMT,  2.0, FRAME );
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		tempva	= (weightVal.r * matSpine2 +weightVal.b * matSpine1 + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matSpine2 +weightVal.b * matSpine1 + weightVal.g * matNeck) * normal;
	}
	else if(zone1==6){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matL_Clavicle		=	matFromMT( animationMT,  6.0, FRAME );
		tempva	= (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matL_Clavicle) * vertex;
		normal  = (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matL_Clavicle) * normal;
	}
	else if(zone1==7){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matR_Clavicle		=	matFromMT( animationMT, 20.0, FRAME );
		tempva	= (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matR_Clavicle) * vertex;
		normal  = (weightVal.g * matNeck + weightVal.r * matSpine2 + weightVal.b * matR_Clavicle) * normal;
	}
	else if(zone1==8){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matHead			=	matFromMT( animationMT,  5.0, FRAME );
		tempva	= (weightVal.b * matHead + weightVal.g * matNeck + weightVal.r * matSpine2)* vertex;
		normal  = (weightVal.b * matHead + weightVal.g * matNeck + weightVal.r * matSpine2)* normal;
	}
	else if(zone1==9){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matL_Clavicle		=	matFromMT( animationMT,  6.0, FRAME );
		tempva	= (weightVal.b * matL_Clavicle + weightVal.g * matNeck + weightVal.r * matSpine2) * vertex;
		normal  = (weightVal.b * matL_Clavicle + weightVal.g * matNeck + weightVal.r * matSpine2) * normal;
	}
	else if(zone1==10){
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matL_Clavicle		=	matFromMT( animationMT,  6.0, FRAME );
		mat4 matL_UpperArm		=	matFromMT( animationMT,  7.0, FRAME );
		tempva	= (weightVal.r * matL_UpperArm + weightVal.b * matL_Clavicle + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matL_UpperArm + weightVal.b * matL_Clavicle + weightVal.g * matNeck) * normal;
	}
	else if(zone1==11){
		mat4 matL_UpperArm		=	matFromMT( animationMT,  7.0, FRAME );
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		tempva	= (weightVal.g * matL_Forearm + weightVal.r * matL_UpperArm + weightVal.b * matL_Hand) * vertex;
		normal  = (weightVal.g * matL_Forearm + weightVal.r * matL_UpperArm + weightVal.b * matL_Hand) * normal;
	}
	else if(zone1==12){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger0		=	matFromMT( animationMT, 10.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger0) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger0) * normal;
	}
	else if(zone1==13){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger3		=	matFromMT( animationMT, 16.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger3) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger3) * normal;
	}
	else if(zone1==14){
		mat4 matL_Forearm		=	matFromMT( animationMT,  8.0, FRAME );
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger4		=	matFromMT( animationMT, 18.0, FRAME );
		tempva	= (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger4) * vertex;
		normal  = (weightVal.b * matL_Hand + weightVal.g * matL_Forearm + weightVal.r * matL_Finger4) * normal;
	}
	else if(zone1==15){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger0		=	matFromMT( animationMT, 10.0, FRAME );
		mat4 matL_Finger01		=	matFromMT( animationMT, 11.0, FRAME );
		tempva	= (weightVal.r * matL_Finger0 + weightVal.b * matL_Hand + weightVal.g * matL_Finger01) * vertex;
		normal  = (weightVal.r * matL_Finger0 + weightVal.b * matL_Hand + weightVal.g * matL_Finger01) * normal;
	}
	else if(zone1==16){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger1		=	matFromMT( animationMT, 12.0, FRAME );
		mat4 matL_Finger11		=	matFromMT( animationMT, 13.0, FRAME );
		tempva	= (weightVal.r * matL_Finger1 + weightVal.b * matL_Hand + weightVal.g * matL_Finger11) * vertex;
		normal  = (weightVal.r * matL_Finger1 + weightVal.b * matL_Hand + weightVal.g * matL_Finger11) * normal;
	}
	else if(zone1==17){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger2		=	matFromMT( animationMT, 14.0, FRAME );
		mat4 matL_Finger21		=	matFromMT( animationMT, 15.0, FRAME );
		tempva	= (weightVal.r * matL_Finger2 + weightVal.b * matL_Hand + weightVal.g * matL_Finger21) * vertex;
		normal  = (weightVal.r * matL_Finger2 + weightVal.b * matL_Hand + weightVal.g * matL_Finger21) * normal;
	}
	else if(zone1==18){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger3		=	matFromMT( animationMT, 16.0, FRAME );
		mat4 matL_Finger31		=	matFromMT( animationMT, 17.0, FRAME );
		tempva	= (weightVal.r * matL_Finger3 + weightVal.b * matL_Hand + weightVal.g * matL_Finger31) * vertex;
		normal  = (weightVal.r * matL_Finger3 + weightVal.b * matL_Hand + weightVal.g * matL_Finger31) * normal;
	}
	else if(zone1==19){
		mat4 matL_Hand			=	matFromMT( animationMT,  9.0, FRAME );
		mat4 matL_Finger4		=	matFromMT( animationMT, 18.0, FRAME );
		mat4 matL_Finger41		=	matFromMT( animationMT, 19.0, FRAME );
		tempva	= (weightVal.r * matL_Finger4 + weightVal.b * matL_Hand + weightVal.g * matL_Finger41) * vertex;
		normal  = (weightVal.r * matL_Finger4 + weightVal.b * matL_Hand + weightVal.g * matL_Finger41) * normal;
	}
	else if(zone1==20){
		mat4 matSpine2			=	matFromMT( animationMT,  3.0, FRAME );
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matR_Clavicle		=	matFromMT( animationMT, 20.0, FRAME );
		tempva	= (weightVal.b * matR_Clavicle + weightVal.g * matNeck + weightVal.r * matSpine2) * vertex;
		normal  = (weightVal.b * matR_Clavicle + weightVal.g * matNeck + weightVal.r * matSpine2) * normal;
	}
	else if(zone1==21){
		mat4 matNeck			=	matFromMT( animationMT,  4.0, FRAME );
		mat4 matR_Clavicle		=	matFromMT( animationMT, 20.0, FRAME );
		mat4 matR_UpperArm		=	matFromMT( animationMT, 21.0, FRAME );
		tempva	= (weightVal.r * matR_UpperArm + weightVal.b * matR_Clavicle + weightVal.g * matNeck) * vertex;
		normal  = (weightVal.r * matR_UpperArm + weightVal.b * matR_Clavicle + weightVal.g * matNeck) * normal;
	}
	else if(zone1==22){
		mat4 matR_UpperArm		=	matFromMT( animationMT, 21.0, FRAME );
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		tempva	= (weightVal.g * matR_Forearm + weightVal.r * matR_UpperArm + weightVal.b * matR_Hand) * vertex;
		normal  = (weightVal.g * matR_Forearm + weightVal.r * matR_UpperArm + weightVal.b * matR_Hand) * normal;
	}
	else if(zone1==23){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger0		=	matFromMT( animationMT, 24.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger0) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger0) * normal;
	}
	else if(zone1==24){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger3		=	matFromMT( animationMT, 30.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger3) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger3) * normal;
	}
	else if(zone1==25){
		mat4 matR_Forearm		=	matFromMT( animationMT, 22.0, FRAME );
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger4		=	matFromMT( animationMT, 32.0, FRAME );
		tempva	= (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger4) * vertex;
		normal  = (weightVal.b * matR_Hand + weightVal.g * matR_Forearm + weightVal.r * matR_Finger4) * normal;
	}
	else if(zone1==26){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger0		=	matFromMT( animationMT, 24.0, FRAME );
		mat4 matR_Finger01		=	matFromMT( animationMT, 25.0, FRAME );
		tempva	= (weightVal.r * matR_Finger0 + weightVal.b * matR_Hand + weightVal.g * matR_Finger01) * vertex;
		normal  = (weightVal.r * matR_Finger0 + weightVal.b * matR_Hand + weightVal.g * matR_Finger01) * normal;
	}
	else if(zone1==27){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger1		=	matFromMT( animationMT, 26.0, FRAME );
		mat4 matR_Finger11		=	matFromMT( animationMT, 27.0, FRAME );
		tempva	= (weightVal.r * matR_Finger1 + weightVal.b * matR_Hand + weightVal.g * matR_Finger11) * vertex;
		normal  = (weightVal.r * matR_Finger1 + weightVal.b * matR_Hand + weightVal.g * matR_Finger11) * normal;
	}
	else if(zone1==28){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger2		=	matFromMT( animationMT, 28.0, FRAME );
		mat4 matR_Finger21		=	matFromMT( animationMT, 29.0, FRAME );
		tempva	= (weightVal.r * matR_Finger2 + weightVal.b * matR_Hand + weightVal.g * matR_Finger21) * vertex;
		normal  = (weightVal.r * matR_Finger2 + weightVal.b * matR_Hand + weightVal.g * matR_Finger21) * normal;
	}
	else if(zone1==29){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger3		=	matFromMT( animationMT, 30.0, FRAME );
		mat4 matR_Finger31		=	matFromMT( animationMT, 31.0, FRAME );
		tempva	= (weightVal.r * matR_Finger3 + weightVal.b * matR_Hand + weightVal.g * matR_Finger31) * vertex;
		normal  = (weightVal.r * matR_Finger3 + weightVal.b * matR_Hand + weightVal.g * matR_Finger31) * normal;
	}
	else if(zone1==30){
		mat4 matR_Hand			=	matFromMT( animationMT, 23.0, FRAME );
		mat4 matR_Finger4		=	matFromMT( animationMT, 32.0, FRAME );
		mat4 matR_Finger41		=	matFromMT( animationMT, 33.0, FRAME );
		tempva	= (weightVal.r * matR_Finger4 + weightVal.b * matR_Hand + weightVal.g * matR_Finger41) * vertex;
		normal  = (weightVal.r * matR_Finger4 + weightVal.b * matR_Hand + weightVal.g * matR_Finger41) * normal;
	}
	else if(zone1==31){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matL_Thigh			=	matFromMT( animationMT, 34.0, FRAME );
		tempva	= (weightVal.b * matL_Thigh + weightVal.g * matSpine + weightVal.r * matPelvis) * vertex;
		normal  = (weightVal.b * matL_Thigh + weightVal.g * matSpine + weightVal.r * matPelvis) * normal;
	}
	else if(zone1==32){
		mat4 matL_Thigh			=	matFromMT( animationMT, 34.0, FRAME );
		mat4 matL_Calf			=	matFromMT( animationMT, 35.0, FRAME );
		mat4 matL_Foot			=	matFromMT( animationMT, 36.0, FRAME );
		tempva	= (weightVal.r * matL_Calf + weightVal.b * matL_Thigh + weightVal.g * matL_Foot) * vertex;
		normal  = (weightVal.r * matL_Calf + weightVal.b * matL_Thigh + weightVal.g * matL_Foot) * normal;
	}
	else if(zone1==33){
		mat4 matL_Calf			=	matFromMT( animationMT, 35.0, FRAME );
		mat4 matL_Foot			=	matFromMT( animationMT, 36.0, FRAME );
		mat4 matL_Toe0			=	matFromMT( animationMT, 37.0, FRAME );
		tempva	= (weightVal.g * matL_Foot + weightVal.r * matL_Calf + weightVal.b * matL_Toe0) * vertex;
		normal  = (weightVal.g * matL_Foot + weightVal.r * matL_Calf + weightVal.b * matL_Toe0) * normal;
	}
	else if(zone1==34){
		mat4 matPelvis			=	matFromMT( animationMT,  0.0, FRAME );
		mat4 matSpine			=	matFromMT( animationMT,  1.0, FRAME );	
		mat4 matR_Thigh			=	matFromMT( animationMT, 38.0, FRAME );
		tempva	= (weightVal.b * matR_Thigh + weightVal.g * matSpine + weightVal.r * matPelvis) * vertex;
		normal  = (weightVal.b * matR_Thigh + weightVal.g * matSpine + weightVal.r * matPelvis) * normal;
	}
	else if(zone1==35){
		mat4 matR_Thigh			=	matFromMT( animationMT, 38.0, FRAME );
		mat4 matR_Calf			=	matFromMT( animationMT, 39.0, FRAME );
		mat4 matR_Foot			=	matFromMT( animationMT, 40.0, FRAME );
		tempva	= (weightVal.r * matR_Calf + weightVal.b * matR_Thigh + weightVal.g * matR_Foot) * vertex;
		normal  = (weightVal.r * matR_Calf + weightVal.b * matR_Thigh + weightVal.g * matR_Foot) * normal;
	}
	else if(zone1==36){
		mat4 matR_Calf			=	matFromMT( animationMT, 39.0, FRAME );
		mat4 matR_Foot			=	matFromMT( animationMT, 40.0, FRAME );
		mat4 matR_Toe0			=	matFromMT( animationMT, 41.0, FRAME );
		tempva	= (weightVal.g * matR_Foot + weightVal.r * matR_Calf + weightVal.b * matR_Toe0) * vertex;
		normal  = (weightVal.g * matR_Foot + weightVal.r * matR_Calf + weightVal.b * matR_Toe0) * normal;
	}

	mat4 transMat4x4		= mat4( 1.0 );
	transMat4x4[ 3 ][ 0 ]	= position.x;								// Locate whole model's instance.
	transMat4x4[ 3 ][ 1 ]	= position.y;								// Locate whole model's instance.
	transMat4x4[ 3 ][ 2 ]	= position.z;								// Locate whole model's instance.
	mat4 modelViewProjMat	= (ProjMat4x4 * ViewMat4x4 * transMat4x4);	// Create model's View Matrix.



	//vec4 P					= (modelViewProjMat *ShadowMat4x4 * tempva );
	vec4 P					= (modelViewProjMat * tempva );
	
	P						= P/P.w;
	gl_Position				= P;

	if(tempva.z < 0.9)
		depthZ = 0.0;
	else
		tempva.z = P.z;
	//depthZ					= (newPos.z- PZNear.z)/(PZFar.z - PZNear.z);
}
