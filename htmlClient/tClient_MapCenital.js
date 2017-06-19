var output;
var wsUri	= "ws://localhost:9005/";
var reader	= new FileReader();
var ctx;
var stop 	= false;

var gl;
var rttFrameBuffer;
var rttTexture;
var depthTexture;
var renderbuffer;
var data=[0,1,2];
var data2=[];
var data3=[];
var data4=[];
var tmplin=[0,1,2];
var canvas2;
var contextCanvas2;
var canvas3;
var contextCanvas3;
var canvas4;
var contextCanvas4;
var ImageData;
var ImageData2;
var ImageData3;
var ImageData4;
var websocket;

var glslProg;
var quadCoord=[-1.0,-1.0,-1.0,		1.0,-1.0,-1.0,		1.0,1.0,-1.0,		-1.0,1.0,-1.0];
//var quadCoord=[-1.0,1.0,-1.0,		1.0,1.0,-1.0,		1.0,-1.0,-1.0,		-1.0,-1.0,-1.0];
//var quadCoord=[-512.0,512.0,0.0,		512.0,512.0,0.0,		512.0,-512.0,0.0,		-512.0,-512.0,0.0];
var textureCoords = [0.0,0.0,  1.0,0.0,  1.0,1.0,  0.0,1.0];
var quadVertexIndices = [0, 1, 2,      0, 2, 3];
var quadPostionBuffer;
var textCoordBuffer;
var quadVertexIndexBuffer;
var vertPosData;
var textCoorData;
var indexData;

var textTest;
var imageTest;
var S_width;
var S_height;
var mapImgData=[];
var imgCrtrCnvs;
var contextImgCrtrCnvs;
var saveImgData;


function initGL(){
   
    gl = null;

    S_width = parseInt(document.getElementById("map").style.width);
    S_height = parseInt(document.getElementById("map").style.height);

    imgCrtrCnvs = document.createElement('canvas');
    imgCrtrCnvs.id = "ImageCreator";
    imgCrtrCnvs.width = S_width;
    imgCrtrCnvs.height = S_height;
    contextImgCrtrCnvs = imgCrtrCnvs.getContext("2d");
    saveImgData = contextImgCrtrCnvs.createImageData(imgCrtrCnvs.width,imgCrtrCnvs.height);
    
    
    //alert("resolution "+S_width+" x "+S_height);
    //initCanvas();
    try {
	// Tratar de tomar el contexto estandar. Si falla, retornar al experimental.

	var map = L.map('map');
	var layer = Tangram.leafletLayer({
            scene: 'scene.yaml',
            attribution: '<a href="https://mapzen.com/tangram" target="_blank">Tangram</a> | &copy; OSM contributors | <a href="https://mapzen.com/" target="_blank">Mapzen</a>',
            //logLevel: 'debug',
            preUpdate: function(will_render){
		if (!will_render) {
		    //console.log("preUpdate no render");
		    return;
		}
		//console.log("preUpdate render");
		gl = layer.scene.gl;
		//initFrameBuffer();
	    },
            postUpdate: function(didRender) {
		if (didRender) {
		    storeImgInBuffer();
                    //releaseTextures()
		    //initShaders();
		    //initBuffers();
		    //createDepthImageFromTexture(gl,depthTexture,canvas3.width,canvas3.height);
		    //createImageFromTexture(gl,rttTexture,canvas3.width,canvas3.height);
		    //send3DElement();
             }
          }
        });


        layer.addTo(map);

        map.setView([41.3893997, 2.1139643], 17);


    }
    catch(e) {}
}
/*----------------------------------------------------------------------------------------------------------------*/
function initShaders(){
    try {   
	glslProg = createProgramFromScripts(gl);
    } catch (e) {
	alert("Error is: " + e);
    }
    
    gl.useProgram(glslProg);
    
    glslProg.vertexPositionAttribute = gl.getAttribLocation(glslProg,"a_vpos");
    gl.enableVertexAttribArray(glslProg.vertexPositionAttribute);
    
    glslProg.textureCoordAttribute = gl.getAttribLocation(glslProg,"a_tpos");
    gl.enableVertexAttribArray(glslProg.textureCoordAttribute);

    glslProg.flagUniform = gl.getUniformLocation(glslProg, "uFlag");;
    glslProg.pMatrixUniform = gl.getUniformLocation(glslProg, "uPMatrix");
}
/*----------------------------------------------------------------------------------------------------------------*/
function initCanvas(){
    canvas3 = document.getElementById("canvas3");
    canvas3.width =  S_width;
    canvas3.height = S_height;
    contextCanvas3 = canvas3.getContext("2d");

    canvas4 = document.getElementById("canvas4");
    canvas4.width = S_width;
    canvas4.height = S_height;
    contextCanvas4 = canvas4.getContext("2d");

    //alert("Objeto context:"+context);
    ////copy the pixels to a 2D canvas
    ImageData2 = contextCanvas3.createImageData(canvas3.width,canvas3.height);
    ImageData3 = data = new Uint8Array(canvas3.width*canvas3.height*4 + 1);
    ImageData3[0] = 7;
    ImageData4 = contextCanvas4.createImageData(canvas4.width,canvas4.height);

}
/*----------------------------------------------------------------------------------------------------------------*/
function initFrameBuffer(){
    /////Texture to store framebuffer
    
    rttTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D,rttTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,S_width, S_height,0,gl.RGBA,gl.UNSIGNED_BYTE,null);
    //gl.bindTexture(gl.TEXTURE_2D,null);
    
    ////Depht texture
    
    depthTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, depthTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.DEPTH_COMPONENT,S_width, S_height, 0, gl.DEPTH_COMPONENT, gl.UNSIGNED_SHORT, null);
    
   
    /////Framebuffer object
    rttFrameBuffer = gl.createFramebuffer();
    
    gl.bindFramebuffer(gl.FRAMEBUFFER,rttFrameBuffer);
    gl.enable(gl.DEPTH_TEST);
    
    var depthTextureExt = gl.getExtension("WEBKIT_WEBGL_depth_texture"); // Or browser-appropriate prefix
    if(!depthTextureExt) { alert("depthTextureExt was not in the browser"); return; }

    ////attach the texture to the framebuffer object color point
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,rttTexture,0);
    ////attach the texture to the framebuffer object depth point
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT,gl.TEXTURE_2D,depthTexture,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
function releaseTextures(){    
    gl.bindFramebuffer(gl.FRAMEBUFFER,null);
    gl.bindRenderbuffer(gl.RENDERBUFFER,null);
}
/*----------------------------------------------------------------------------------------------------------------*/
function createImageFromTexture(gl,texture,width,height){
    if(data.length!=width*height*4)
	data = new Uint8Array(width*height*4);

    DrawQuad(gl,rttTexture,false);
    
    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data);

    ImageData2.data.set(data);
    contextCanvas3.putImageData(ImageData2,0,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
function createDepthImageFromTexture(gl,texture,width,height){
    if(data4.length!= width*height*4)
	data4 = new Uint8Array(width*height*4);    
    
    DrawQuad(gl,depthTexture,true);
    
    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data4);
    
    ImageData4.data.set(data4);
    contextCanvas4.putImageData(ImageData4,0,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
var ev;
function saveMapScrSht(){

    //var blob = new Blob([mapImgData],{type:'image/png'})
    imgCrtrCnvs.toBlob(function(blob){
	saveAs(blob,"scrSht.png");
    });
    
}
/*----------------------------------------------------------------------------------------------------------------*/
function storeImgInBuffer(){
    if(mapImgData.length!=S_width*S_height*4)
	mapImgData = new Uint8Array(S_width*S_height*4);

    gl.clearColor(1.0, 1.0, 1.0, 1.0);
    gl.readPixels(0,0,S_width,S_height,gl.RGBA,gl.UNSIGNED_BYTE,mapImgData);
    saveImgData.data.set(mapImgData);
    contextImgCrtrCnvs.putImageData(saveImgData,0,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
function initBuffers(){
    vertPosData = new Float32Array(quadCoord);
    textCoorData = new Float32Array(textureCoords);
    indexData = new Uint16Array(quadVertexIndices);
    
    quadPostionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,quadPostionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertPosData,gl.STATIC_DRAW);
    quadPostionBuffer.itemSize=3;
    quadPostionBuffer.numitems=quadCoord.length/quadPostionBuffer.itemSize;

    textCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,textCoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER,textCoorData,gl.STATIC_DRAW);
    textCoordBuffer.itemSize = 2;
    textCoordBuffer.numItems = textureCoords.length/textCoordBuffer.itemSize;

    quadVertexIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, quadVertexIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indexData, gl.STATIC_DRAW);
    quadVertexIndexBuffer.itemSize = 1;
    quadVertexIndexBuffer.numItems = quadVertexIndices.length;
}
/*----------------------------------------------------------------------------------------------------------------*/
function DrawQuad(gl,texture,flag){

    //useGLProgram();

    //BindQuadBuffers();
    
    gl.viewport(0,0,canvas4.width,canvas4.height);
    //console.log("viewportWidth: " + canvas4.width);
    gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
    gl.clearColor(1.0, 1.0, 1.0, 1.0);
    //gl.enable(gl.DEPTH_TEST);
    
    gl.bindBuffer(gl.ARRAY_BUFFER,quadPostionBuffer);
    gl.vertexAttribPointer(glslProg.vertexPositionAttribute,quadPostionBuffer.itemSize,gl.FLOAT,false,0,0);

    gl.bindBuffer(gl.ARRAY_BUFFER,textCoordBuffer);
    gl.vertexAttribPointer(glslProg.textureCoordAttribute,textCoordBuffer.itemSize,gl.FLOAT,false,0,0);

    //activeShader();
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D,texture);
    //gl.copyTexImage2D(gl.TEXTURE_2D,0,gl.RGBA,0,0,canvas4.width,canvas4.height,0);
    
    gl.uniform1i(glslProg.samplerUniform,0);

    gl.uniform1i(glslProg.flagUniform,0);
    if(flag)
	gl.uniform1i(glslProg.flagUniform,1);
    
    
    gl.drawElements(gl.TRIANGLES, quadVertexIndexBuffer.numItems, gl.UNSIGNED_SHORT, 0);
    //gl.bindFramebuffer(gl.FRAMEBUFFER,null);
    //requestAnimFrame(DrawQuad);

}
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
function compileShader(gl,shaderSource,shaderType){
	//Create shader object
	var shader = gl.createShader(shaderType);

	//set shader source code
	gl.shaderSource(shader,shaderSource);

	//compile the shader
	gl.compileShader(shader);

	//Check if it compileShader
	var sucess = gl.getShaderParameter(shader,gl.COMPILE_STATUS);
	if(!sucess){
		//Something get wrong during the compilation; get the error
		throw "could not compile shader: "+gl.getShaderInfoLog(shader)+" shader name: "+shaderType;
	}
	return shader;
}

/*----------------------------------------------------------------------------------------------------------------*/

function createProgram(gl,vertexShader,fragmentShader){
	//create a program
	var program = gl.createProgram();

	//attach the shaders
	gl.attachShader(program,vertexShader);
	gl.attachShader(program,fragmentShader);

	//link the program
	gl.linkProgram(program);

	//Check if it linked
	var sucess = gl.getProgramParameter(program,gl.LINK_STATUS);
	if(!sucess){
		// something went wrong with the link
      throw ("program failed to link:" + gl.getProgramInfoLog (program));
	}

	return program;
}

/*----------------------------------------------------------------------------------------------------------------*/

function createShaderFromScript(gl,opt_shaderType){
	//lookup the tag by element id
    var shaderSource;

    //alert(vertShSource + " " +fragShSource );

    if(opt_shaderType == gl.VERTEX_SHADER)
	shaderSource = vertShSource;
    else if(opt_shaderType == gl.FRAGMENT_SHADER)
	shaderSource = fragShSource;
    else
	throw "----*****---- ERROR with the variable "+shaderSource;
    return compileShader(gl,shaderSource,opt_shaderType);

}

/*----------------------------------------------------------------------------------------------------------------*/

function createProgramFromScripts(gl){
    var vertexShader = createShaderFromScript(gl,gl.VERTEX_SHADER);
    var fragmentShader = createShaderFromScript(gl,gl.FRAGMENT_SHADER);
    return createProgram(gl,vertexShader,fragmentShader);
}

/*----------------------------------------------------------------------------------------------------------------*/


window.onload = initGL;
//window.addEventListener("load", init, false);
