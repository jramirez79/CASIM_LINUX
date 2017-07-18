uniform float z_Far;
varying float zFAR;
varying float dpth;

void main ()
{
	gl_TexCoord[0] = gl_MultiTexCoord0; 
	gl_Position = ftransform();
	zFAR = z_Far;
	dpth = gl_Position.z;
}
