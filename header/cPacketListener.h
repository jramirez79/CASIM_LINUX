
#ifndef __PACKET_LISTENER__
#define __PACKET_LISTENER__

#include <string>
#include "OscPacketListener.h"

// CONTROLA LA CANTIDAD DE AGENTES. 
// SOLO MOVER LA CONSTANTE NUM_CHARACTERS, TANTO EN EL SERVER
// COMO EN EL CLIENTE

#define NUM_CHARACTERS			4096
#define NUM_CHARACTERS_LIMIT	256
#define PACKET_SIZE				NUM_CHARACTERS_LIMIT * 3

using namespace std;

class cPacketListener : public osc::OscPacketListener 
{
public:
					cPacketListener ();
					~cPacketListener ();
			float	**pos;
private:
			void	receiveOSCMsg ( const osc::ReceivedMessage &msg, string tag, int index );
protected:
    // Implement the required ProcessMessage()
    virtual	void	ProcessMessage( const osc::ReceivedMessage& msg, const IpEndpointName& remoteEndpoint );
};

#endif
