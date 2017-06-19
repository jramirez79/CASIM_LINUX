//fragment shaders don't have a deffault precission so we need
//to pick one. mediump is a good default, it means 'medium precision'.

var fragShSource =
    "precision highp float; "+
    //"varying vec2 vTextureCoord; "+
    //"uniform sampler2D uSampler; "+
    //"uniform int uFlag; "+

    "void main(void){ "+
    //"float grayColor = texture2D(uSampler,vec2(vTextureCoord.s, vTextureCoord.t)).x; "+
    //"gl_FragColor = texture2D(uSampler,vec2(vTextureCoord.s, vTextureCoord.t)); "+
    //"gl_FragColor = vec4(1.0,0.0,1.0,1.0); "+

    //"gl_FragColor = texture2D(uSampler,vec2(vTextureCoord.s, vTextureCoord.t)); "+
    //"if(uFlag==1) "+
    //"gl_FragColor = vec4(grayColor,grayColor,grayColor,1.0); "+
    "} ";

var camFragShSource =
    "precision highp float; "+
    "varying vec4 vColor; "+

    "void main(void){ "+
    "   gl_FragColor = vColor; "+
    "} ";
