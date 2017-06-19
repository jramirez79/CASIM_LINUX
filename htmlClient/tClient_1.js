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
var quadCoord=[-1.0,-1.0,1.0,		1.0,-1.0,1.0,		1.0,1.0,1.0,		-1.0,1.0,1.0];
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
var S_width = 512;
var S_height = 512;


function init(){
	if (window.WebSocket) {
		connectAndCallbacks();
	}
	else
	{
		console.log ("This browser does not support Websocket.");
	}

	// adding mouse events
	canvas2 = document.getElementById('canvas2');

	canvas2.addEventListener( "contextmenu", preventDefaultHandler,	true );
	canvas2.addEventListener( "mousedown", 	mouseDownHandler,	true );
    canvas2.addEventListener( "mouseup",	mouseUpHandler, 	true );
    canvas2.addEventListener( "mousemove", 	mouseMoveHandler, 	true );
    canvas2.addEventListener( "mousewheel", 	mouseWheelHandler, 	true );

    document.addEventListener("keydown", 	keyDownHandler, 	false );
    document.addEventListener("keyup", 	keyUpHandler, 		false );

	contextCanvas2 	= canvas2.getContext('2d');
	ImageData = contextCanvas2.getImageData(0,0,canvas2.width,canvas2.height);
}
/*----------------------------------------------------------------------------------------------------------------*/
function connectAndCallbacks ()
{
	websocket			= new WebSocket(wsUri);
	websocket.onopen 	= function(evt) { onOpen	(evt)	};
	websocket.onclose 	= function(evt) { onClose	(evt)	};
	websocket.onmessage	= function(evt) { onMessage	(evt)	};
	websocket.onerror 	= function(evt) { onError	(evt)	};

	// sets websocket's binary messages as ArrayBuffer type
	//websocket.binaryType = "arraybuffer";
	// sets websocket's binary messages as Blob type by default is Blob type
	//websocket.binaryType = "blob";

	reader.onload		= function(evt) { readBlob		(evt) };
	reader.onloadend	= function(evt) { nextBlob		(evt) };
	reader.onerror		= function(evt) { fileReaderError 	(evt) };

	peripherals		= new Peripherals ();
	peripherals.init(websocket);
}
/*----------------------------------------------------------------------------------------------------------------*/

function onOpen(evt)
{

}
/*----------------------------------------------------------------------------------------------------------------*/
function fileReaderError (e)
{
	console.error("FileReader error. Code " + event.target.error.code);
}
/*----------------------------------------------------------------------------------------------------------------*/
// this method is launched when FileReader ends loading the blob
function nextBlob (e)
{
	if (!stop)
		websocket.send ("GiveMeMore");
	//console.log (stop);
}
/*----------------------------------------------------------------------------------------------------------------*/
/// this method is launched when the user is having interaction with the xml3d element
function send3DElement (){
	if(!stop)
		websocket.send(ImageData3);
		//websocket.send("SendingFrameBuffer");
}
/*----------------------------------------------------------------------------------------------------------------*/
// readBlob is called when reader.readAsBinaryString(blob); from onMessage method occurs
function readBlob (e)
{
	//console.log (canvas.width, canvas.height );
	var i,j=0;

    // if receiving float images
	//img = new Float32Array(reader.result);

	// if receiving unsigned byte images
	img = new Uint8Array(reader.result);

//console.log (imgdata.data.length);

// TODO: send data as  picture format to avoid this:

	//console.log ("result: ", + reader.target.result);
//	console.log("Blob size ", + reader.result.length + " ", + reader.result[10]);
//	console.log("Blob size ", + img.length + " ", + img[10]);


	// if img is float
/*
    for(i=0;i<imgdata.data.length;i+=4)
	{
		imgdata.data[i]		= img[j  ] * 255; //.charCodeAt(j);
		imgdata.data[i+1] 	= img[j+1] * 255; //reader.result.charCodeAt(j+1);
		imgdata.data[i+2] 	= img[j+2] * 255; //reader.result.charCodeAt(j+2);
		imgdata.data[i+3] 	= 255;
		j+=3;
	}
*/

 	 for(i=0;i<ImageData.data.length;i+=4)
	{
		ImageData.data[i]		= img[j  ] ; //.charCodeAt(j);
		ImageData.data[i+1] 	= img[j+1] ; //reader.result.charCodeAt(j+1);
		ImageData.data[i+2] 	= img[j+2] ; //reader.result.charCodeAt(j+2);
		ImageData.data[i+3] 	= 255;
		j+=3;
	}

	contextCanvas2.putImageData(ImageData,0,0);

}
/*----------------------------------------------------------------------------------------------------------------*/
// message from the server
function onMessage(e)
{
	if (typeof e.data == "string")
	{
		console.log ("String msg: ", e, e.data);
	}
	else if (e.data instanceof Blob)
	{
		var blob = e.data;

		//console.log ("Blob msg: ", blob);

		//reading data from Blob
		//reader.readAsBinaryString(blob);
		reader.readAsArrayBuffer(blob);

	}
	else if (e.data instanceof ArrayBuffer)
	{
		var img = new Uint8Array(e.data);

		console.log("Array msg:", + img[63] );


	}


	//websocket.close ();
}
/*----------------------------------------------------------------------------------------------------------------*/
// is there is an error
function onError (e)
{
	console.log("Websocket Error: ", e);
	// Custom functions for handling errors
	// handleErrors (e);
}
/*----------------------------------------------------------------------------------------------------------------*/
// when closing
function onClose (e)
{
	console.log ("Connection closed", e);

}
/*----------------------------------------------------------------------------------------------------------------*/
function startingConnection()
{
	alert('Streaming ON...  display size: '+canvas2.width+' x '+canvas2.height);

	websocket.send ("STSIM");
	stop = false;

}
/*----------------------------------------------------------------------------------------------------------------*/
function closingConnection()
{
    	alert('Streaming OFF...');
	websocket.send ("END  ");
	stop = true;



	//websocket.close ();
}
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
var mvMatrix = mat4.create();
var mvMatrixStack = [];
var pMatrix = mat4.create();

function mvPushMatrix() {
		var copy = mat4.create();
		// Update: mat4.set(mvMatrix, copy); mat4.set() was removed from gl-matrix, use mat4.copy().
		mat4.copy(copy, mvMatrix);
		mvMatrixStack.push(copy);
}

function mvPopMatrix() {
		if (mvMatrixStack.length == 0) {
				throw "Invalid popMatrix!";
		}
		mvMatrix = mvMatrixStack.pop();
}


function setMatrixUniforms() {
		gl.uniformMatrix4fv(glslProg.pMatrixUniform, false, pMatrix);
		gl.uniformMatrix4fv(glslProg.mvMatrixUniform, false, mvMatrix);
}


function degToRad(degrees) {
		return degrees * Math.PI / 180;
}

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

function initGL(){
    //var xml3dElement = document.getElementById("myXml3dElement");
    //var renderInterface = xml3dElement.getRenderInterface();
    //canvas2 = document.getElementById("canvas2");

    gl = null;

    initCanvas();
    try {
	// Tratar de tomar el contexto estandar. Si falla, retornar al experimental.

	var map = L.map('map');
	var layer = Tangram.leafletLayer({
            scene: 'scene.yaml',
            attribution: '<a href="https://mapzen.com/tangram" target="_blank">Tangram</a> | &copy; OSM contributors | <a href="https://mapzen.com/" target="_blank">Mapzen</a>',
            //logLevel: 'debug',
            preUpdate: function(will_render){
		if (!will_render) {
		    console.log("preUpdate no render");
		    return;
		}
		//console.log("preUpdate render");
		gl = layer.scene.gl;
		gl.viewportWidth = 512;
		gl.viewportHeight = 512;
		initFrameBuffer();
	    },
            postUpdate: function(didRender) {
		if (didRender) {
                    gl = layer.scene.gl;
		    
		    initShaders();
		    initBuffers();
		    
		    //layer.scene.requestRedraw();

		    //createImageFromTexture(gl,rttTexture,canvas3.width,canvas3.height);
		    //toRenderDepthTexture(gl,depthTexture,canvas3.width,canvas3.height);
		    
		    createDepthImageFromTexture(gl,depthTexture,canvas3.width,canvas3.height);
		    //layer.scene.requestRedraw();
             }
          }
        });


        layer.addTo(map);

        map.setView([41.3893997, 2.1139643], 17);


    }
    catch(e) {}
}
/*----------------------------------------------------------------------------------------------------------------*/
function activeShader(){
    gl.useProgram(glslProg);
    
    glslProg.vertexPositionAttribute = gl.getAttribLocation(glslProg,"a_vpos");
    gl.enableVertexAttribArray(glslProg.vertexPositionAttribute);
    
    glslProg.textureCoordAttribute = gl.getAttribLocation(glslProg,"a_tpos");
    gl.enableVertexAttribArray(glslProg.textureCoordAttribute);
    
    glslProg.samplerUniform = gl.getUniformLocation(glslProg,"uSampler");
    glslProg.pMatrixUniform = gl.getUniformLocation(glslProg, "uPMatrix");
    glslProg.mvMatrixUniform = gl.getUniformLocation(glslProg, "uMVMatrix");
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
    
    glslProg.pMatrixUniform = gl.getUniformLocation(glslProg, "uPMatrix");
    glslProg.mvMatrixUniform = gl.getUniformLocation(glslProg, "uMVMatrix");
    glslProg.samplerUniform = gl.getUniformLocation(glslProg,"uSampler");
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
    ImageData3 = data = new Uint8Array(canvas3.width*canvas3.height*8 + 1);
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
    
    /*/Target Texture
    displayTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D,rttTexture);
    gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);
    //gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR_MIPMAP_NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,rttFrameBuffer.width,rttFrameBuffer.height,0,gl.RGBA,gl.UNSIGNED_BYTE,null);
    gl.bindTexture(gl.TEXTURE_2D,null);
    */

    //var depthTextureExt = gl.getExtension("WEBKIT_WEBGL_depth_texture"); // Or browser-appropriate prefix
    //if(!depthTextureExt) { alert("depthTextureExt was not in the browser"); return; }
    /////Framebuffer object
    rttFrameBuffer = gl.createFramebuffer();
    //rttFrameBuffer.width=512;
    //rttFrameBuffer.height=512;
    
    gl.bindFramebuffer(gl.FRAMEBUFFER,rttFrameBuffer);
    gl.enable(gl.DEPTH_TEST);
    


    ////attach the texture to the framebuffer object color point
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,rttTexture,0);
    ////attach the texture to the framebuffer object depth point
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT,gl.TEXTURE_2D,depthTexture,0);

    
    
    gl.bindFramebuffer(gl.FRAMEBUFFER,null);
    gl.bindRenderbuffer(gl.RENDERBUFFER,null);
}
/*----------------------------------------------------------------------------------------------------------------*/
function toRenderDepthTexture(){
    gl.bindFramebuffer(gl.FRAMEBUFFER, rttFrameBuffer);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT,gl.TEXTURE_2D,depthTexture,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
function createImageFromTexture(gl,texture,width,height){
    ////read the contents of the framebuffer
    //alert("222----------222");
    //console.log("createImageFromTexture");
    if(data.length!=width*height*4)
	data = new Uint8Array(width*height*4);
    
    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data);

    //var cnt4 = 0;
    //var cnt3=width*height*4 + 1;
    //for(var i=0;i<data.length;++i){
//	if(i%4==3){
//	    data[i]=255;
//	    //console.log(" alpha data to one ");
//	}
  //  }

    /*for(var i=0;i<data.length;++i){
    	ImageData3[i+1]=data[i];
    	ImageData3[i+cnt3]=data[i];
			/*
    	if(data[i]!=0&&cnt4<5){
    		console.log(data[i]+","+i);
    		++cnt4;
    		}
    		* /
    }*/


    ImageData2.data.set(data);
    //ImageData2.data.set(NvaImage);
    contextCanvas3.putImageData(ImageData2,0,0);

    //ImageData4.data.set(data3);
    //contextCanvas4.putImageData(ImageData4,0,0);

   //console.log(ImageData3[263657]+","+ImageData3[263658]+","+ImageData3[263659]+","+ImageData3[263660]+","+ImageData3[263661]);

   //send3DElement();

//    return img;

}
/*----------------------------------------------------------------------------------------------------------------*/
function createDepthImageFromTexture(gl,texture,width,height){
    
    //if(data3.length!=width*height)
//	data3 = new Uint16Array(width*height);

    //console.log("createDepthImageFromTexture");
    
    if(data4.lenght!= width*height*4)
	data4 = new Uint8Array(width*height*4);
    
    //gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    DrawQuad(gl,depthTexture);

    var depthTextureExt = gl.getExtension("WEBGL_depth_texture"); // Or browser-appropriate prefix
    if(!depthTextureExt) { console.log("no existe la extension"); return; }
    
    gl.bindTexture(gl.TEXTURE_2D, rttTexture);
    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data4);
    
    for(var i=0;i<data4.length;++i){
	if(i%4==3){
	    data4[i]=255;
	    //console.log(" alpha data to one ");
	}
    }

    ImageData4.data.set(data4);
    contextCanvas4.putImageData(ImageData4,0,0);

   //console.log(ImageData3[263657]+","+ImageData3[263658]+","+ImageData3[263659]+","+ImageData3[263660]+","+ImageData3[263661]);

   //send3DElement();

//    return img;

}
/*----------------------------------------------------------------------------------------------------------------*/
function useGLProgram(){
    gl.useProgram(glslProg);
    
    glslProg.vertexPositionAttribute = gl.getAttribLocation(glslProg,"a_vpos");
    gl.enableVertexAttribArray(glslProg.vertexPositionAttribute);
    
    glslProg.textureCoordAttribute = gl.getAttribLocation(glslProg,"a_tpos");
    gl.enableVertexAttribArray(glslProg.textureCoordAttribute);
    
    glslProg.samplerUniform = gl.getUniformLocation(glslProg,"uSampler");
    setMatrixUniforms();
}
/*----------------------------------------------------------------------------------------------------------------*/
function handleLoadedTexture(gl,texture){
    gl.bindTexture(gl.TEXTURE_2D,texture);
    if(texture.image)
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);
    
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
    gl.generateMipmap(gl.TEXTURE_2D);
    //gl.bindTexture(gl.TEXTURE_2D, null);

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
function BindQuadBuffers(){
    gl.bindBuffer(gl.ARRAY_BUFFER,quadPostionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertPosData,gl.STATIC_DRAW);

    gl.bindBuffer(gl.ARRAY_BUFFER,textCoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER,textCoorData,gl.STATIC_DRAW);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, quadVertexIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indexData, gl.STATIC_DRAW);
}
/*----------------------------------------------------------------------------------------------------------------*/
function DrawQuad(gl,texture){

    //useGLProgram();

    //BindQuadBuffers();
    
    gl.viewport(0,0,canvas4.width,canvas4.height);
    //console.log("viewportWidth: " + canvas4.width);
    gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
    gl.clearColor(0.0, 0.0, 1.0, 1.0);
    //gl.enable(gl.DEPTH_TEST);
    mat4.perspective (pMatrix, 45.0, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);

    mat4.identity(mvMatrix);
    
    // Update: mat4.translate(mvMatrix, [0.0, 0.0, -5.0]); mat4.translate() API has changed to mat4.translate(out, a, v)
    // where out is the receiving matrix, a is the matrix to translate, and v is the vector to translate by. z altered to
    // approximate original scene.
    mat4.translate(mvMatrix, mvMatrix, [0.0, 0.0, -2.8]);

    
    gl.bindBuffer(gl.ARRAY_BUFFER,quadPostionBuffer);
    gl.vertexAttribPointer(glslProg.vertexPositionAttribute,quadPostionBuffer.itemSize,gl.FLOAT,false,0,0);

    gl.bindBuffer(gl.ARRAY_BUFFER,textCoordBuffer);
    gl.vertexAttribPointer(glslProg.textureCoordAttribute,textCoordBuffer.itemSize,gl.FLOAT,false,0,0);

    //activeShader();
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D,texture);
    //gl.copyTexImage2D(gl.TEXTURE_2D,0,gl.RGBA,0,0,canvas4.width,canvas4.height,0);
    
    gl.uniform1i(glslProg.samplerUniform,0);
    setMatrixUniforms();

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
window.addEventListener("load", init, false);
