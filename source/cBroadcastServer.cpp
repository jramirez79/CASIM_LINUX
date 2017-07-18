//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include <cBroadcastServer.h>
#include "cMacros.h"
#include "cMouseEventHandler.h"
#include "cKeyboardHandler.h"
#include "CImg.h"


unsigned char 	webSocketKey;
//extern bool		keyChangedFlag;
pthread_mutex_t 	*ptrmutexObj;

unsigned int cntTest = 0;
unsigned char frmCnt = 0;

broadcast_server::broadcast_server()
{
	 m_server.init_asio();

	 // set up access channels to only log interesting things
	m_server.clear_access_channels(websocketpp::log::alevel::all);
	m_server.set_access_channels(websocketpp::log::alevel::connect);
	m_server.set_access_channels(websocketpp::log::alevel::disconnect);
	//m_server.set_access_channels(websocketpp::log::alevel::app);


	 m_server.set_open_handler(bind(&broadcast_server::on_open,this,::_1));
	 m_server.set_close_handler(bind(&broadcast_server::on_close,this,::_1));
	 m_server.set_message_handler(bind(&broadcast_server::on_message,this,::_1,::_2));

	 needMoreFrames = false;
	 stop 			= true;

	 mouseHandler		= 0;
	 keyboardHandler	= 0;
	 /* Create a mutex */
}
//
//=======================================================================================
//
broadcast_server::~broadcast_server()
{
	// TODO: stop the server

}
//
//=======================================================================================
//
void broadcast_server::on_open(connection_hdl hdl)
{
	 m_connections.insert(hdl);
}
//
//=======================================================================================
//
void broadcast_server::on_close(connection_hdl hdl)
{
    m_connections.erase(hdl);
}
//
//=======================================================================================
//

unsigned int numMsg =0;
void broadcast_server::on_message(connection_hdl hdl, server::message_ptr msg)
{
	// TODO: Process Interaction msgs
	std::stringstream val;
	int type;
	int buttonMask;
	int xPosition;
	int yPosition;

	int keyMask;

	//std::cout << "broadcast_server::on_message "<<numMsg++<<std::endl;

    for (auto it : m_connections)
    {
    	try
    	{
    		val << msg->get_payload();

    		//std::cout << "Stop val  "<<stop<<std::endl;
    		if (!stop)
    		{
    			type = val.str().data()[0];

    			//std::cout << "Type  "<<type<<std::endl;
    			/*
				switch(type){
				case MOUSE_EVENT:
					std::cout << "MOUSE_EVENT "<<std::endl;
					break;
				case KEY_EVENT:
					std::cout << "KEY_EVENT "<<std::endl;
					break;
				case CLIENT_FRAME_EVENT:
					std::cout << "CLIENT_FRAME_EVENT "<<std::endl;
					break;
				default:
					std::cout << "  "<<type<<std::endl;
				}
				*/

				if (type == MOUSE_EVENT)
				{

					buttonMask = val.str().data()[1];
					xPosition = (unsigned char)val.str().data()[2]*255 + (unsigned char)val.str().data()[3];
					yPosition = (unsigned char)val.str().data()[4]*255 + (unsigned char)val.str().data()[5];

					if (mouseHandler)
					{
						mouseHandler->setCoords	( xPosition, yPosition	);
						mouseHandler->setButton ( buttonMask			);
						mouseHandler->refresh	( true 					);

						//std::cout << "button: " << mouseHandler->getButton() << " state: " << mouseHandler->getState() << std::endl;
						//std::cout << "x: " << xPosition << " y: " << xPosition << std::endl;
					}
					else
					{
						std::cout << "Is Mouse handler initialized? \n";
					}

				}else if (type == KEY_EVENT)
				{
					keyMask = (unsigned char)val.str().data()[4]*255 + (unsigned char)val.str().data()[5]*255 + (unsigned char)val.str().data()[6]*255 + (unsigned char)val.str().data()[7];

					if (keyboardHandler)
					{
						keyboardHandler->setState 	( val.str().data()[1] 	);
						keyboardHandler->setKey   	( keyMask				);
						keyboardHandler->refresh	( true 					);


						//if (keyboardHandler->getState()==cKeyboardHandler::DOWN)
						//std::cout << "Key: " << keyboardHandler->getKey () << std::endl;
					}
					else
					{
						std::cout << "Is keyboard handler initialized? \n";
					}
					//std::cout << "rstate: " << val.str().data()[1] << "cstate: " << keyboardHandler->getState() << std::endl;
				}
				else if(type == CLIENT_FRAME_EVENT){
					pthread_mutex_lock (pc_MutexObj);
					//std::cout<<"CLIENT_FRAME_EVENT first byte of data is "<< (unsigned int)((unsigned char)val.str().data()[0])<<std::endl;


					if(!(pc_frmClientHandler->processDataBuffer(&val)))
							std::cout<<"Error at receiving the frameData from the client"<<std::endl;


					//else
							//std::cout<<"Sucess at receiving the frameData from the client"<<std::endl;
						//pc_frmClientHandler->printInfo();

						/*
						if(cntTest<10){
									cimg_library::CImg<float>* img_test = new cimg_library::CImg<float>(WIN_WIDTH,WIN_HEIGHT,1,4,0);
									float * pixData = pc_frmClientHandler->getFrameBufferMyScTyp();
									unsigned int conta=0;

									for(unsigned int ucnty=0; ucnty<WIN_WIDTH;++ucnty){
												for(unsigned int  ucntx=0; ucntx<WIN_HEIGHT;++ucntx){
													(*(img_test->data(ucntx,ucnty,0,0))) = pixData[conta++]*255.0;
													(*(img_test->data(ucntx,ucnty,0,1))) = pixData[conta++]*255.0;
													(*(img_test->data(ucntx,ucnty,0,2))) = pixData[conta++]*255.0;
													(*(img_test->data(ucntx,ucnty,0,3))) = pixData[conta++]*255.0;
												}
											}


									img_test->save_png(std::string("testPNG_"+std::to_string(cntTest++)+".png").c_str());
								}

						*/

						//needMoreFrames = true;
						//stop = false;
						pthread_mutex_unlock (pc_MutexObj);
				}
    		}
    		if (val.str().compare("GiveMeMore") == 0 || val.str().compare("STSIM")==0 )
    		{
    			//std::cout << "Give me More Frames...\n";
    			needMoreFrames = true;
    			stop = false;
    		}
    		if(val.str().compare("SendingFrameBuffer") == 0){
    			std::cout << "Sendig the 3D background\n";
    		}
    		if ( val.str().compare("END  ") == 0 )
    		{
    			stop = true;
    			std::cout << "dedos";
    			needMoreFrames = false;
    		}
    	}
    	catch (const websocketpp::lib::error_code& e)
    	{
    		std::cout	<< "SEND failed because: " << e
    					<< "(" << e.message() << ")" << std::endl;
		}
    }

}
//
//=======================================================================================
//
void broadcast_server::run(uint16_t port)
{
    m_server.listen(port);
    m_server.start_accept();
    // Start the ASIO io_service run loop
    try
    {
    	m_server.run();
    }
    catch (const std::exception & e)
    {
    	std::cout << e.what() << std::endl;
    }
    catch (websocketpp::lib::error_code e)
    {
    	std::cout << e.message() << std::endl;
    }
    catch (...)
    {
    	std::cout << "other exception" << std::endl;
    }
}
//
//=======================================================================================
//
void broadcast_server::sendFrame (float *img)
{
	//setFrame (img);

	for (auto it : m_connections)
	{
		try
		{
			// when img is unsigned char
			//m_server.send(it, m_img, (size_t)width*height*3 , websocketpp::frame::opcode::BINARY);
			// when img is float
			m_server.send(it, img, (size_t)WIN_WIDTH*WIN_HEIGHT*3*sizeof(float) , websocketpp::frame::opcode::BINARY);


			needMoreFrames = false;
		}
		catch (const websocketpp::lib::error_code& e)
		{
			std::cout	<< "SEND failed because: " << e
					<< "(" << e.message() << ")" << std::endl;
		}
	}

}
//
//=======================================================================================
//
void broadcast_server::sendFrame (unsigned char *img)
{
	//setFrame (img);



	for (auto it : m_connections)
	{
		try
		{
			// when img is unsigned char
			m_server.send(it, img, (size_t)WIN_WIDTH*WIN_HEIGHT*3 , websocketpp::frame::opcode::BINARY);

			//std::cout << "m_server.send(it, img, (size_t)WIN_WIDTH*WIN_HEIGHT*3 , websocketpp::frame::opcode::BINARY);\n";
			//m_server.send(it, "END  ", websocketpp::frame::opcode::text);

			needMoreFrames = false;
		}
		catch (const websocketpp::lib::error_code& e)
		{
			std::cout	<< "SEND failed because: " << e
					<< "(" << e.message() << ")" << std::endl;
		}
	}

}
//
//=======================================================================================
//
void broadcast_server::setMouseHandler			( cMouseHandler	*mouseH	)
{
	mouseHandler = mouseH;
}
//
//=======================================================================================
//
void broadcast_server::setKeyboardHandler		( cKeyboardHandler	*keyboardH	)
{
	keyboardHandler = keyboardH;
}
//
//=======================================================================================
//
cFrameClientHandler* broadcast_server::getFrmClntHndler( )
{
	return pc_frmClientHandler;
}
//
//=======================================================================================
//
void broadcast_server::setFrmClntHandler(cFrameClientHandler* frmClntHndlr){
	pc_frmClientHandler = frmClntHndlr;
}
//
//=======================================================================================
//
void broadcast_server::setMutexObj(pthread_mutex_t *ptrMutex){
	pc_MutexObj = ptrMutex;
}
//
//=======================================================================================
//
