//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

var canvas;
var wsUri	= "ws://localhost:9005/";
//var wsUri = "ws://localhost:1337/";
var reader	= new FileReader();
var ctx;
var imgdata;
var stop 	= false;

var mouseDownFlag = false;

var customCanvas;
var context;
var imageData;
var img = new Image();
var canvasResult;
var ctxResult;

var imageheight = 512;
var imagewidth	= 512;


function init()
{
	// cheking for websocket support
	if (window.WebSocket) {
		connectAndCallbacks();
	}
	else
	{
		console.log ("This browser does not support Websocket.");
	}

	customCanvas = document.getElementById('customCanvas');
	context = customCanvas.getContext("2d");
	canvasResult = document.getElementById('canvasResult');
	ctxResult = canvasResult.getContext("2d");
	imageData = context.createImageData(imagewidth,imageheight);
	canvas = document.getElementById('canvas');
	ctx 	= canvas.getContext('2d');
	imgdata = ctx.getImageData(0,0,canvas.width,canvas.height);
	//console.log("imgdata.length: " + imgdata.data.length);
	//console.log("data.length: " + window.data.length);

	// adding mouse events
	canvas.addEventListener( "contextmenu", preventDefaultHandler,	true );
	canvas.addEventListener( "mousedown", 	mouseDownHandler,	true );
  canvas.addEventListener( "mouseup",	mouseUpHandler, 	true );
  canvas.addEventListener( "mousemove", 	mouseMoveHandler, 	true );
  canvas.addEventListener( "mousewheel", 	mouseWheelHandler, 	true );

  document.addEventListener("keydown", 	keyDownHandler, 	false );
  document.addEventListener("keyup", 	keyUpHandler, 		false );
}


function connectAndCallbacks ()
{
	websocket		= new WebSocket(wsUri);
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


// connection has been opened
function onOpen(evt)
{

}

function fileReaderError (e)
{
	console.error("FileReader error. Code " + event.target.error.code);
}

// this method is launched when FileReader ends loading the blob
function nextBlob (e)
{
	if (!stop)
		websocket.send ("GiveMeMore");
	//console.log (stop);
}

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
/*
var data = window.data;
imageData.data.set(data);
context.putImageData(imageData,0,0);
*/

	 for(i=0;i<imgdata.data.length;i+=4)
	{
		imgdata.data[i]		= img[j  ] ; //.charCodeAt(j);
		imgdata.data[i+1] 	= img[j+1] ; //reader.result.charCodeAt(j+1);
		imgdata.data[i+2] 	= img[j+2] ; //reader.result.charCodeAt(j+2);
		imgdata.data[i+3] 	= 255;
		j+=3;
	}


	ctx.putImageData(imgdata,0,0);



/*
	ctxResult.globalCompositeOperation = "source-over";
	ctxResult.drawImage(customCanvas, 0, 0, customCanvas.width, customCanvas.height);
	ctxResult.drawImage(canvas, 0, 0, canvas.width, canvas.height);
*/
	//console.log("imgdata.data: " + imgdata.data);
	//console.log("data: " + window.data);


}

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

		//reading data from Blob
		//reader.readAsBinaryString(blob);
		reader.readAsArrayBuffer(blob);
		//console.log("blob msg:", + blob );

	}
	else if (e.data instanceof ArrayBuffer)
	{
		var img = new Uint8Array(e.data);

		console.log("Array msg:", + img[63] );
	}
}

// is there is an error
function onError (e)
{
	console.log("Websocket Error: ", e);
	// Custom functions for handling errors
	// handleErrors (e);
}

// when closing
function onClose (e)
{
	console.log ("Connection closed", e);
}

function startingConnection()
{
	alert('Streaming ON...');
	websocket.send ("STSIM");
	stop = false;
}

function closingConnection()
{
	alert('Streaming OFF...');
	websocket.send ("END  ");
	stop = true;
	//websocket.close ();
}


window.addEventListener("load", init, false);
