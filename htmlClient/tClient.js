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
var ImgDatabffSiz;
var ImageData4;
var websocket;

var glslProg;
var quadCoord=[-1.0,-1.0,-1.0,		1.0,-1.0,-1.0,		1.0,1.0,-1.0,		-1.0,1.0,-1.0];
var quadCoord2=[-10.0,-10.0,-10.0,		10.0,-10.0,-10.0,		10.0,10.0,-10.0,		-10.0,10.0,-10.0];

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
var S_width = 512;
var S_height = 512;

var initLong = 2.1139643;
var initLat = 41.3893997;
var initZoom = 17;
var lat = initLat;
var long = initLong;
var zoom = initZoom;
var rotAng;
var coordMsg;

var initX;
var initZ;
var xrect;
var zrect;
var mtsPerPixel;
var initflg = true;
var zFarVar;
var zNearVar = 1.0;


var camDirIni;
var camDir
var camOrg;
var depthflg;
var ctx;

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
var swap;
function send3DElement (modViewMat){
    var cnta = ImgDatabffSiz;
    //var aux = 0;
    //var i;
		setCoords(modViewMat);
    swap = new Uint8Array(coordMsg.buffer);

		//console.log("send3DElement "+swap.length);
		ImageData3[cnta] = ImageData3[0];
		ImageData3[0] = 7;
		++cnta;
		for(var i=0;i<(swap.length);++i,++cnta)
        ImageData3[cnta] = swap[i];

    //for(i=0;i<data.length;++i,++cnta,++aux){
		/*
		while(cnta<data.length+1){
    	//ImageData3[cnta] = data[i];
			ImageData3[cnta] = data4[i];
			++cnta;
			/ *
			if(aux==3){
    	    ImageData3[cnta] = data4[i-1];
    	    aux=-1;
    	} * /
    }*/

		//console.log( "cnta: "+cnta+" data lenght:"+data.length);




////********************************************************************///////
    //for(var i=0;i<(swap.length*4);++i,++cnta)
        //ImageData3[cnta] = swap[i];

////********************************************************************///////
		//console.log("Coord Buffer "+coordMsg+" ;;;; "+swap+" cnta: "+cnta);
			//console.log("Distance to ground :"+distance2Ground(lat,zoom)+ "mts from the camera");
		//console.log("sending data");


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
		try{
				reader.readAsArrayBuffer(blob);
		}
		catch(err){
			console.log("Error "+err.message);
		}


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

function initGL(){

    gl = null;

    initCanvas();
    //mat4.identity(IMatrix);
    camDirIni 	= vec3.fromValues(0,1,0);
    camOrg 	= vec3.fromValues(0,0,0);
    camDir	= vec3.create();
    depthflg = 0;

    try {
	// Tratar de tomar el contexto estandar. Si falla, retornar al experimental.

	var map = L.map('map');

	//var shaders;
	//console.log(map);
	var zoom_ant = 0;
	//var printsh = 0;
	var tglslProg;
	var regTProg = 0;

	toMercatorCoordinates();
	initXrect = xrect;
	initZrect = zrect;


	var layer = Tangram.leafletLayer({
            scene: 'scene_1.yaml',
            attribution: '<a href="https://mapzen.com/tangram" target="_blank">Tangram</a> | &copy; OSM contributors | <a href="https://mapzen.com/" target="_blank">Mapzen</a>',
            //logLevel: 'debug',
            preUpdate: function(will_render){
		if (!will_render) {
		    console.log("preUpdate no render");

		    return;
		}
		//console.log(map);

        zoom = map.getZoom();
        lat = map.getCenter().lat;
        long = map.getCenter().lng;

	var  sizeMap = map.getSize();

		//console.log(" map size object "+Geo.metersPerTile(map._zoom));

        if(zoom_ant!=zoom && (zoom>14.0 && zoom<21.0)){
            if(map._zoom<14)
                rotAng = 0.0;
            else if(zoom<17)
                rotAng = (60.0*zoom-840.0)/3.0;
            else if(zoom<21)
                rotAng = (30.0*zoom-270.0)/4.0;
            else
                rotAng = 90.0;

	    //rotAng = 0.0;

	    vec3.rotateX(camDir,camDirIni,camOrg,-rotAng);
	    layer.scene.config.lights.light1.direction = [camDir[0],camDir[1],camDir[2]];
			//layer.scene.config.lights.light1.visible = true;
			//layer.scene.config.lights.light2.visible = false;
      layer.scene.config.styles.customPosition.shaders.uniforms.u_angle = rotAng;
	    //layer.scene.config.styles.customPosition.shaders.uniforms.u_mode = depthflg;

      zoom_ant = zoom;
      layer.scene.config.cameras.camera1=zoom_ant;
      layer.scene.config.cameras.camera2=zoom_ant;
      layer.scene.config.cameras.camera3=zoom_ant;
	    zFarVar = layer.scene.view.meters_per_pixel*256;
	    layer.scene.config.styles.customPosition.shaders.uniforms.u_zFar = zFarVar;

			}


			layer.scene.config.styles.customPosition.shaders.uniforms.u_mode = depthflg;
			layer.scene.updateConfig();
			//layer.scene.requestRedraw();
        //console.log("zoom :"+zoom + " lat: "+ lat + " long: "+long);
        //console.log("Distance to ground :"+distance2Ground(lat,zoom)+ "mts from the camera");
        //console.log("Rot Angle "+rotAng);
        //sendInitCoords();

        //mat4.scalar.rotateX(rMatrix,IMatrix,degToRad(-rotAng));
		//initTangramShaders();

		/*
        if(regTProg==0){
            tglslProg = gl.getParameter (gl.CURRENT_PROGRAM);
            //if(printShadersProgConsole(tglslProg))
            if(tglslProg!=null && isValidProgram(tglslProg))
                ++regTProg;
        }
	*/
		gl = layer.scene.gl;
		/*shaders = gl.getAttachedShaders(gl.getParameter (gl.CURRENT_PROGRAM));
		if(shaders!=null && shaders.length>0 && printsh==0){
            console.log("1---CURRENT GLSL Program Shader 1: "+gl.getShaderSource(shaders[0]));
            }*/

        //if(regTProg)
				if(depthflg)
            initFrameBufferFSH();
	    },
            postUpdate: function(didRender) {
		if (didRender) {

            //if(regTProg){
              //  releaseTextures()
                //initShaders();
                //initBuffers();
                //createDepthImageFromTexture(gl,depthTexture,canvas3.width,canvas3.height);
                //createImageFromTexture(gl,rttTexture,canvas3.width,canvas3.height);
                //disShader();
                //gl.useProgram(tglslProg);





		if(depthflg){
		  	depthflg = 0;
		  	createDepthImageFromScreen(gl,canvas3.width,canvas3.height);
				//console.log("release texture");
				releaseTextures();
				send3DElement(layer.scene.view.matrices.model_view32);
				layer.scene.config.lights.light1.visible= false;
				layer.scene.config.lights.light2.visible= true;
		}
		else{
		   	depthflg = 1;
		   	//createImageFromScreen(gl,canvas3.width,canvas3.height);
			 	layer.scene.config.lights.light1.visible= true;
				layer.scene.config.lights.light2.visible= false;
		}



		if(initflg){
			initX = layer.scene.view.center.meters.x;
			initZ = layer.scene.view.center.meters.y;
			initflg = false;
		}

		xrect = layer.scene.view.center.meters.x - initX;
		zrect = layer.scene.view.center.meters.y - initZ;

		//console.log(" xrect: "+xrect+" zrect: "+zrect+" viewport_height: "+((layer.scene.view.size.css.height*layer.scene.view.meters_per_pixel)/2)+" meters per pixel "+layer.scene.view.meters_per_pixel);
		//console.log(" viewport Mat: "+layer.scene.view.matrices.model_view32);
		//console.log("camera: "+layer.scene.view.camera.fov);
		//console.log(" viewport Mat: "+layer.scene.view.camera.projection_matrix);
		mtsPerPixel = ((layer.scene.view.size.css.height * layer.scene.view.meters_per_pixel)/2);
		zFarVar = mtsPerPixel;
		//send3DElement(layer.scene.view.matrices.model_view32);
		//send3DElement(layer.scene.view.camera.projection_matrix);
            //}

		  ///layer.scene.rebuild();
             }
          }
        });


        layer.addTo(map);

        map.setView([initLat,initLong], initZoom);


    }
    catch(e) {}
}
/*----------------------------------------------------------------------------------------------------------------*/
function initShaders(){
    try {
	glslProg = createProgramFromScripts(gl,0);
    } catch (e) {
	alert("Error is: " + e);
    }

    gl.useProgram(glslProg);

    glslProg.vertexPositionAttribute = gl.getAttribLocation(glslProg,"a_vpos");
    gl.enableVertexAttribArray(glslProg.vertexPositionAttribute);

    glslProg.textureCoordAttribute = gl.getAttribLocation(glslProg,"a_tpos");
    gl.enableVertexAttribArray(glslProg.textureCoordAttribute);

    glslProg.flagUniform = gl.getUniformLocation(glslProg, "uFlag");
    glslProg.pMatrixUniform = gl.getUniformLocation(glslProg, "uPMatrix");
    glslProg.zFarUniform = gl.getUniformLocation(glslProg, "zFar");
    glslProg.zNearUniform = gl.getUniformLocation(glslProg, "zNear");
}
/*----------------------------------------------------------------------------------------------------------------*/
function disShader(){
    gl.disableVertexAttribArray(glslProg.vertexPositionAttribute);
    gl.disableVertexAttribArray(glslProg.textureCoordAttribute);
}
/*----------------------------------------------------------------------------------------------------------------*/
function initTangramShaders(){
    if(glslProg==null){
        try {
        glslProg = createProgramFromScripts(gl,1);
        } catch (e) {
        alert("Error is: " + e);
        }

        gl.useProgram(glslProg);

        glslProg.vertexPositionAttribute = gl.getAttribLocation(glslProg,"a_vpos");
        gl.enableVertexAttribArray(glslProg.vertexPositionAttribute);

        glslProg.vertexColordAttribute = gl.getAttribLocation(glslProg,"a_color");
        gl.enableVertexAttribArray(glslProg.vertexColordAttribute);

        glslProg.pMatrixUniform = gl.getUniformLocation(glslProg, "uRotMatrix");
    }
    else
        gl.useProgram(glslProg);
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
    data = new Uint8Array(canvas3.width*canvas3.height*4 + 1);
    ImageData3 = new Uint8Array(canvas3.width*canvas3.height*4 + 93);
		ImgDatabffSiz = canvas3.width*canvas3.height*4;
    //ImageData3[0] = 7;
		coordMsg = new Float32Array(23);
    ImageData4 = contextCanvas4.createImageData(canvas4.width,canvas4.height);

}
/*----------------------------------------------------------------------------------------------------------------*/
function initFrameBuffer(){
    /////Texture to store framebuffer

    if(rttTexture==null)
        rttTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D,rttTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,S_width, S_height,0,gl.RGBA,gl.UNSIGNED_BYTE,null);
    //gl.bindTexture(gl.TEXTURE_2D,null);

    ////Depht texture

    if(depthTexture==null)
        depthTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, depthTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.DEPTH_COMPONENT,S_width, S_height, 0, gl.DEPTH_COMPONENT, gl.UNSIGNED_SHORT, null);


    /////Framebuffer object
    if(rttFrameBuffer==null)
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

function initFrameBufferFSH(){
	/////Texture to store framebuffer

	if(rttTexture==null)
			rttTexture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D,rttTexture);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
	gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,S_width, S_height,0,gl.RGBA,gl.UNSIGNED_BYTE,null);
	//gl.bindTexture(gl.TEXTURE_2D,null);


    /////Framebuffer object
    if(rttFrameBuffer==null)
        rttFrameBuffer = gl.createFramebuffer();

    gl.bindFramebuffer(gl.FRAMEBUFFER,rttFrameBuffer);

		////attach the texture to the framebuffer object color point
		gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,rttTexture,0);

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

function createImageFromScreen(gl,width,height){
  if(data.length!=width*height*4)
        data = new Uint8Array(width*height*4);

    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data);

    ImageData2.data.set(data);
    contextCanvas3.putImageData(ImageData2,0,0);
}
/*----------------------------------------------------------------------------------------------------------------*/
function createDepthImageFromTexture(gl,texture,width,height){
    if(data4.lenght!= width*height*4)
        data4 = new Uint8Array(width*height*4);

    DrawQuad(gl,depthTexture,true);

    gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data4);

    ImageData4.data.set(data4);
    contextCanvas4.putImageData(ImageData4,0,0);
}

function createDepthImageFromScreen(gl,width,height){
  //if(data4.lenght!= width*height*4)
    //    data4 = new Uint8Array(width*height*4);

    //gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,data4);
		gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,ImageData3);

		//for(var cn=0;cn<width*height*4;++cn)
		//	data4[cn]=ImageData3[cn+93];

    //ImageData4.data.set(data4);
		//ImageData4.data.set(ImageData3);
    //contextCanvas4.putImageData(ImageData4,0,0);
		//console.log(ImageData3[263656]+","+ImageData3[263657]+","+ImageData3[263658]+","+ImageData3[263659]+","+ImageData3[263660]);
}

/*----------------------------------------------------------------------------------------------------------------*/
function initBuffers(){
    if(vertPosData == null && textCoorData==null && indexData==null){
        vertPosData = new Float32Array(quadCoord);
        textCoorData = new Float32Array(textureCoords);
        indexData = new Uint16Array(quadVertexIndices);
    }

    if(quadPostionBuffer==null)
        quadPostionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,quadPostionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertPosData,gl.STATIC_DRAW);
    quadPostionBuffer.itemSize=3;
    quadPostionBuffer.numitems=quadCoord.length/quadPostionBuffer.itemSize;

    if(textCoordBuffer==null)
        textCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,textCoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER,textCoorData,gl.STATIC_DRAW);
    textCoordBuffer.itemSize = 2;
    textCoordBuffer.numItems = textureCoords.length/textCoordBuffer.itemSize;

    if(quadVertexIndexBuffer==null)
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

    gl.uniform1f(glslProg.zNearUniform,1.0);
    gl.uniform1f(glslProg.zFarUniform,zFarVar);

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

function createShaderFromScript(gl,opt_shaderType,shadernum){
	//lookup the tag by element id
    var shaderSource;

    if(opt_shaderType == gl.VERTEX_SHADER){
        if(shadernum==0)
            shaderSource = vertShSource;
        else if(shadernum==1)
            shaderSource = camVertShSource;
    }

    else if(opt_shaderType == gl.FRAGMENT_SHADER){
        if(shadernum==0)
            shaderSource = fragShSource;
        else if(shadernum==1)
            shaderSource = camFragShSource;
    }
    else
	throw "----*****---- ERROR with the variable "+shaderSource;

    //console.log(shaderSource);

    return compileShader(gl,shaderSource,opt_shaderType);

}

/*----------------------------------------------------------------------------------------------------------------*/

function createProgramFromScripts(gl,shadernum){
    var vertexShader = createShaderFromScript(gl,gl.VERTEX_SHADER,shadernum);
    var fragmentShader = createShaderFromScript(gl,gl.FRAGMENT_SHADER,shadernum);
    return createProgram(gl,vertexShader,fragmentShader);
}
/*----------------------------------------------------------------------------------------------------------------*/
function printShadersProgConsole(program){
    var shaders;

    shaders = gl.getAttachedShaders(program);

	if(shaders!=null && shaders.length>0){
        for(var i=0;i<shaders.length;++i){
            console.log("-----CURRENT GLSL Program Shader"+(i+1)+": "+gl.getShaderSource(shaders[i]));
        }
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------------------------------------------*/
function isValidProgram(program){
    var shaders;

    shaders = gl.getAttachedShaders(program);

	if(shaders!=null && shaders.length>0){
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
var mvMatrix = mat4.create();
var mvMatrixStack = [];
var pMatrix = mat4.create();
var rMatrix = mat4.create();
var IMatrix = mat4.create();

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
		gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
		gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}


function degToRad(degrees) {
		return degrees * Math.PI / 180;
}
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
var EARTH_RADIUS = 6378137;
var EARTH_EQUATORIAL_PERIMETER = 40075016.6856;
var half_circumference_meters = 20037508.342789244;
var cLatini =  Math.cos(degToRad(initLat));


function distance2Ground(lat, zoom){
    return (EARTH_EQUATORIAL_PERIMETER * Math.cos(degToRad(lat)))/Math.pow(2,zoom);
}

function torectCoordinates(){
		var latV = lat - initLat  ;
		var longV = long - initLong ;

		///console.log("Diff lat:"+latV+" long:"+longV);
		latV = degToRad(latV);
		longV = degToRad(longV)
		var P1 = EARTH_EQUATORIAL_PERIMETER * Math.cos(degToRad(initLat));
		var P2 = EARTH_EQUATORIAL_PERIMETER * Math.cos(degToRad(lat));

		rectX = ((Math.PI * P1)+ (P2 * (longV - Math.PI))) / (2.0 * Math.PI);
		rectZ = EARTH_RADIUS * latV;

		//console.log("Rad Diff lat:"+latV+" long:"+longV + " coords x:"+rectX+" , z:"+rectZ+" lat off "+((P1-P2)/2));
		//console.log("----------------------coords x:"+xrect+" , z:"+zrect);
}

function setCoords(modViewMat){

		coordMsg[0] = zoom;
    coordMsg[1] = lat;
    coordMsg[2] = long;
    coordMsg[3] = rotAng;
		coordMsg[4] = xrect;
		coordMsg[5] = zrect;
		coordMsg[6] = mtsPerPixel;

		for(var c=0;c<16;++c)
			coordMsg[c+7] = modViewMat[c];

    //var aux1 = new Uint8Array(coordMsg.buffer);
  	//console.log("InitCoord Buffer "+coordMsg+" ;;;; "+aux1);
    //websocket.send(coordMsg.buffer);
}

function toMercatorCoordinates(){

	// Latitude
    zrect = Math.log(Math.tan(lat*Math.PI/360 + Math.PI/4)) / Math.PI;
    zrect *= half_circumference_meters;

    // Longitude
    xrect = long*half_circumference_meters / 180;
		//console.log("----------------------coords x:"+xrect+" , z:"+zrect);
}

window.onload = initGL;
window.addEventListener("load", init, false);
