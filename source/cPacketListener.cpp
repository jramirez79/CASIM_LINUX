
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "cPacketListener.h"
#include "OscReceivedElements.h"
#include "OscPrintReceivedElements.h"
#include "UdpSocket.h"

using namespace std;

vector <float> oscPos;
bool unlocked = true;
extern float PLANE_SCALE;

void print ()
{
	int i;
	for (i=0;i<oscPos.size();i+=4)
	{
		cout << "i: " << i << " id: " << oscPos[i+1] << " x: " << oscPos[i] << " z: " << oscPos[i+2]  << " w: " << oscPos[i+3] << "\n";
	}
//	unlocked = !unlocked;
//	oscPos.clear();
}

//extern pthread_mutex_t	mutex; // Mutual-exclusion control var
//extern pthread_cond_t	dataIsReady;	// ctrl var. to send data to OGL when ready
//extern	void*	print (void *threadArgs);
//extern 	void*	setData ();
//
//=======================================================================================
//
cPacketListener::cPacketListener ()
{
	int i,j;

	pos = new float *[NUM_CHARACTERS / NUM_CHARACTERS_LIMIT];
	for (i = 0; i< NUM_CHARACTERS / NUM_CHARACTERS_LIMIT; i++)
	{
		pos[i] = new float [PACKET_SIZE];
		for (j = 0; j< PACKET_SIZE; ++j)
			pos[i][j] = 0.0;
	}
	cout << "i: " << i << " j " << j << "\n";
}
//
//=======================================================================================
//
cPacketListener::~cPacketListener ()
{
	int i = 0;
	for (i = 0; i< NUM_CHARACTERS / NUM_CHARACTERS_LIMIT; i++)
		delete [] pos[i];

	delete [] pos;

}
//
//=======================================================================================
//
void cPacketListener::receiveOSCMsg ( const osc::ReceivedMessage &msg, string tag, int index )
{
	int i;
	size_t numItems;

	try 
	{
		// if the message is a position

		if( strcmp( msg.AddressPattern(), tag.data() ) == 0 )
		{
			osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
			osc::ReceivedMessageArgumentIterator iter = msg.ArgumentsBegin();
			numItems = iter->ComputeArrayItemCount();

			cout << "numItems: " << numItems << endl;

			if ( iter->IsArrayBegin () &&  numItems == PACKET_SIZE) 
			{

				for (i=0;i<numItems;++i)
				{
					//cout <<  "Msg Pattern: " << msg.AddressPattern() << " Tag: " << tag.data() << " index:" << index << " i: " << i << "\n";
					pos[index][i] = iter->AsFloatUnchecked ();
					//std::cout << pos[i] << "|" << i << "  ";
					iter++;
				}
//				cout <<  "Msg Pattern: " << msg.AddressPattern() << " Tag: " << tag.data() << " index:" << index << " i: " << i << "\n";
//				cout << "Done!\n";
			}
			//cout << msg.AddressPattern() << " " << tag << "\n";
		}
	}
	catch ( osc::Exception& e )
	{
		// any parsing errors such as unexpected argument types, or 
		// missing arguments get thrown as exceptions.
		std::cout << "Error while parsing message: " << msg.AddressPattern() << ": " << e.what() << "\n";

	}
}
//
//=======================================================================================
//
void cPacketListener::ProcessMessage(const osc::ReceivedMessage &msg, const IpEndpointName &remoteEndpoint)
{
	int i,j;
//	float *ptr;
	(void) remoteEndpoint; // suppress unused parameter warning
	string tag;



	for (i=0; i< NUM_CHARACTERS / NUM_CHARACTERS_LIMIT; i++)
	{
		ostringstream number;
		number << i;
		tag = "/pos" + number.str();
		receiveOSCMsg ( msg, tag, i );
		//cout << msg.AddressPattern() << " " << tag << "\n";
	}

	if (unlocked)
	{
		oscPos.clear();
		for (i=0; i< NUM_CHARACTERS / NUM_CHARACTERS_LIMIT; i++)
		{
			for (j=1;j<PACKET_SIZE-2;j+=3)
			{
				/* HF5 Server */
/*
				oscPos.push_back ( pos[i][j] / 100.0f  * PLANE_SCALE - PLANE_SCALE/2.0f );
				oscPos.push_back ( pos[i][j+1] * 0.0f );
				oscPos.push_back ( pos[i][j+2] /100.f * PLANE_SCALE - PLANE_SCALE/2.0f );
				oscPos.push_back ( 1.0f );
*/
				/* MINOTAURO */

				oscPos.push_back ( pos[i][j] / 1024.0f  * PLANE_SCALE - PLANE_SCALE/2.0f );
				oscPos.push_back ( pos[i][j+1] * 0.0f );
				oscPos.push_back ( pos[i][j+2] /1024.f * PLANE_SCALE - PLANE_SCALE/2.0f );
				oscPos.push_back ( 1.0f );


			}
		}

	unlocked = !unlocked;
	}

}
//
//=======================================================================================
//

