//an attribute will receive data from a buffer

var vertShSource =
    "attribute vec3 a_vpos; "+
    //"attribute vec2 a_tpos; "+
    "uniform mat4 uMVMatrix; "+
    "uniform mat4 uPMatrix; "+
    //"varying vec2 vTextureCoord; "+
//VertexShader Main Function
    "void main(void){ "+
    //"vec3 trans = vec3(1.0,1.0,1.0); "+
    //"vec3 result = (trans+a_position)*256.0; "+
    //"gl_Position = vec4(result,1.0); "+
    //"gl_Position= uPMatrix * uMVMatrix * vec4(a_vpos,1.0); "+
    "gl_Position= vec4(a_vpos,1.0); "+
    //"vTextureCoord = a_tpos; "+
    //"gl_Position=vec4(1,1,0.5,1);"+
"}";
