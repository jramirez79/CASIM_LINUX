//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#pragma once

// WebSockets
#include <set>
#include <memory>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "cFrameClientHandler.h"


#define SERVER_PORT 9005

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class cMouseHandler;
class cKeyboardHandler;

class broadcast_server {
public:
    broadcast_server				(	);
    ~broadcast_server				(	);

    void on_open					( connection_hdl 		hdl								);
    void on_close					( connection_hdl 		hdl								);
    void on_message					( connection_hdl 		hdl, server::message_ptr msg	);
    void run						( uint16_t 				port							);
    void sendFrame 					( float 				*img							);
    void sendFrame 					( unsigned char			*img							);
    void setFrame 					( float 				*img							);
    void setMouseHandler			( cMouseHandler			*mouseH							);
    void setKeyboardHandler			( cKeyboardHandler		*keyboardH						);
    void setFrmClntHandler			( cFrameClientHandler   *frmClntHdlr					);

    bool sendMoreFrames				(	) 				{return needMoreFrames; };
    cFrameClientHandler* getFrmClntHndler();


private:

    bool				needMoreFrames;
    bool				stop;
    typedef	std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;

    server 				m_server;
    con_list 			m_connections;
    std::stringstream 	string;

    cMouseHandler		*mouseHandler;
    cKeyboardHandler	*keyboardHandler;
    cFrameClientHandler *pc_frmClientHandler;



};
