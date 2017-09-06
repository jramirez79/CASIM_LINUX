// @REV_TAG SRL_10_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjamín Hernández A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <mpi.h>
#include "CImg.h"

#include "cMacros.h"
#include "cVertex.h"
#include "cTimer.h"

#include "cGlErrorManager.h"
#include "cLogManager.h"
#include "cTextureManager.h"

#include "cGlslManager.h"
#include "cVboManager.h"
#include "cFboManager.h"
#include "cCrowdManager.h"

#include "cCamera.h"
#include "cObstacleManager.h"
#include "cAxes.h"
#include "cSkyboxManager.h"
#include "cModel3D.h"

#include "OscReceivedElements.h"
#include "OscPacketListener.h"
#include "UdpSocket.h"
#include "cPacketListener.h"

// websockets stuff
#include "cBroadcastServer.h"
#include "cMouseEventHandler.h"
#include "cKeyboardHandler.h"
#include "glx_x11_events.h"


broadcast_server 	_server_;
cMouseHandler		*mouseHandler = 0;
cKeyboardHandler	*keyboardHandler = 0;
cFrameClientHandler* frmClntHndlr = nullptr;
pthread_mutex_t 	mutexObj;

int Gargc;
char **Gargv;
using namespace std;
//using namespace cimg_library;
//======================================================================================

// GLOBAL_VARIABLES
#include "cGlobals.h"
#include "cOSCGlobals.h"



//=======================================================================================

void cleanup( void );

//=======================================================================================

#if defined MPI_PATHS || defined MPI_PATHS_ON_NODES
	int pid = -1, np = -1;
#endif

#if defined MPI_PATHS_ON_NODES
	float 			**colorPixels 	= 0;
	float			*depthPixels	= 0;
	unsigned int 	culling[2]; // will control if screen shots are captured and send by slaves; received and updated by master
	unsigned int 	pixelSize 		= 0;
	bool 			shutDown 		= false;
#endif


//=======================================================================================

// INIT_FUNCTIONS
#include "cInits.h"

//=======================================================================================

// MPI

#if defined MPI_PATHS
	#include "behaviorMPI.h"
#endif


#ifdef MPI_PATHS_ON_NODES
	#include "behaviorMPIpathsOnNodes.h"
	#include "cInitsMaster.h"
#endif



//=======================================================================================
//
void draw( void )
{
    //glPushAttrib( GL_DEPTH_BUFFER_BIT );
    //{
    //    glDisable( GL_DEPTH_TEST );
	//	skybox_manager->draw( false, false );
    //}
    //glPopAttrib();

    glGetFloatv( GL_MODELVIEW_MATRIX,	view_mat );
    glGetFloatv( GL_PROJECTION_MATRIX, 	proj_mat );

#ifdef CUDA_PATHS
    obstacle_manager->draw( view_mat );
#endif
	if( !hideCharacters )
	{
#ifdef DEMO_SHADER
		crowd_manager->draw(	camera,
								view_mat,
								proj_mat,
								shadow_mat,
								wireframe,
								drawShadows,
								doHeightAndDisplacement,
								doPatterns,
								doColor,
								doFacial			);
#else
		crowd_manager->draw(	camera,
								view_mat,
								proj_mat,
								shadow_mat,
								wireframe,
								drawShadows	);
#endif
	}

	if( showstats )
	{
		axes->draw();
		camera1->draw();
		camera2->draw();
	}
}
//
//=======================================================================================
//
#if defined MPI_PATHS_ON_NODES

vector<unsigned int> screenTextures;
bool loadedTexture = false;

void sendReceiveFrames (void)
{
	int i;

	//cout<<"*********************** SENDRECEIVE FRAMES  pid "<<pid<< " ************************"<<endl;

	if (pid != 0)
	{
		culling[0] = d_culled; // how many characters are culled
		culling[1] = d_total;  // the total of characters

		//cout<<"*********************** culling "<<culling[0]<<","<<culling[1]<<" pid "<<pid<<" ************************"<<endl;

		MPI_Send ((unsigned int*)culling, 2, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD); // culling control variable

		//cout<<"*********************** sended  culled "<<" pid "<<pid<<" ************************"<<endl;
		if (culling[0] != culling[1] ){
			MPI_Send ((float*)colorPixels, pixelSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD); // master pID = 0
			//cout<<"*********************** sended  colorPixels "<<" pid "<<pid<<" ************************"<<endl;
		}
	}
	else
	{
		MPI_Status status;
		for (i=1;i<np;i++)
		{

			//cout<<"*********************** SENDRECEIVE FRAMES  i "<<i<< " pid "<<pid<<" ************************"<<endl;
			//cout<<"*********************** culling "<<culling[0]<<","<<culling[1]<<" pid "<<pid<<" ************************"<<endl;
			MPI_Recv(&culling, 2, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
			//cout<<"*********************** status.source "<<status.MPI_SOURCE<< " status.error  "<<status.MPI_ERROR<< " status.TAG "<<status.MPI_TAG<<"  i  "<<i<< " pid "<<pid<<" ************************"<<endl;




			if (culling[0] != culling[1] ){

				MPI_Recv (colorPixels[i-1], pixelSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);
				//cout<<"*********************** MPI_Recv ColorPixels i "<<i<<" pid "<<pid<<" ************************"<<endl;

			}
//			printf ("Np: %d, Character culled: %d, Total Chars: %d\n", i-1, culling[0], culling[1]);
		}
/*
		printf ("\n\n");
		for (i=0;i<64*4;i+=4)
		{
			printf ("[%f, %f, %f, %f]\t", colorPixels[0][i], colorPixels[0][i+1], colorPixels[0][i+2], colorPixels[0][i+3]);
		}
		printf ("\n\n");
*/
	}

}
//
//=======================================================================================
//
// captureScreen must be called inside
// fbo_manager[...]->fbos[...].fbo->bind()
//	fbo_manager->setTarget( ... )
//     ...
// fbo_manager[...]->fbos[...].fbo->Disable()
// block

void captureScreen (float* pixels, string fboName, string colorTarget, string depthTarget) // Target: screen color or depth
{
	int i,j,w,h;
	float *depth = 0;
	LocalFbo fbo 	= fbo_manager->fbos[fboName];
	w 				= fbo.fbo_width;
	h 				= fbo.fbo_height;
//	depth			= new float [w*h];
	//float depthVal[2];

//	err_manager->getError("BEGIN: void captureScreen () ");

	//camera->getFrustum()->setFarD(1000.0);
	//camera->setView();

	//camera->getFrustum()->setNearD(1000.0);
	//camera->setView();
	//depthVal[0] = camera->getFrustum()->getFarD();
	//depthVal[1] = camera->getFrustum()->getNearD();
	//float zDiff = depthVal[1]- depthVal[0];

	glPushMatrix();

	//glsl_manager->updateUniformf("instancing_culled_rigged","ZBufFlg",1);
	//glsl_manager->updateUniformf("instancing_culled_rigged_shadow","ZBufFlg",1);

	glReadBuffer	( fbo_manager->targets_map[colorTarget]);
// Reading Color
	glReadPixels	( 0, 0, w, h, GL_RGBA, GL_FLOAT, pixels );

// Reading Depth



	glReadPixels	( 0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, depthPixels );
	//glReadPixels	( 0, 0, w, h, GL_RGBA, GL_FLOAT, depthPixels );

	//glsl_manager->updateUniformf("instancing_culled_rigged","ZBufFlg",0);
	//glsl_manager->updateUniformf("instancing_culled_rigged_shadow","ZBufFlg",0);

//	err_manager->getError("END: void captureScreen () ");

	for (i=0, j=0;i<w*h*4;i+=4,++j)
	//for (i=0;i<w*h*4;i+=4)
	{
		//if(pid==2){

			//pixels[i+3] = (-depthPixels[j]*depthVal[0])/(depthPixels[j]*zDiff-depthVal[1]);
		//pixels[i+3] = (2.0*depthVal[0])/(depthVal[1]+depthVal[0]-depthPixels[j]*zDiff);

<<<<<<< HEAD


		//pixels[i] = pixels[i+1] = pixels[i+2] = pixels[i+3] = depthPixels[j];
		pixels[i+3] = depthPixels[j];

=======
			pixels[i+3] = depthPixels[j];
>>>>>>> 30e048104403fa5ef7bed9d37f608936d5c07194
		//pixels[i+3] = depthPixels[i];

		//}
		//else
		//if(ant<depthPixels[j])
			//ant = depthPixels[j];
			//	pixels[i+3] = 1.0;
	}


	//cout << "FarD "<<camera->getFrustum()->getFarD()<<" NearD "<<camera->getFrustum()->getNearD()<<" PID "<<pid<<endl;
	//cout << "Max depth level "<<ant<<" PID "<<pid<<endl;
	glPopMatrix();
}
//
//=======================================================================================
//
void updateScreenTextures () // Target: screen color or depth
{
	int i;
	//cout<<"**********--------PID "<<pid<<" updateTexture -------------*********"<<endl;
	if (!loadedTexture){
	//cout<<"**********--------PID "<<pid<<" updateTexture !loadedTexture -------------*********"<<endl;
		for  (i=1;i<np;++i)
		{
			ostringstream	screenName;
			screenName << "Slave_" << i-1;
			//cout << screenName.str();
			string str = screenName.str();

			screenTextures.push_back(	TextureManager::getInstance()->loadRawTexture(	str,
																					colorPixels[i-1],
																					GL_LINEAR,
																					512,
																					512,
																					GL_RGBA32F,
																					GL_RGBA,
																					GL_TEXTURE_RECTANGLE )	);
		}

		string str2 = string("Composition BG");

		if(!webScktsFlg)
			screenTextures.push_back(	TextureManager::getInstance()->loadRawTexture(	str2,
																					imgCmpPtr,
																					GL_LINEAR,
																					512,
																					512,
																					GL_RGBA32F,
																					GL_RGBA,
																					GL_TEXTURE_RECTANGLE )	);
		else{
			pthread_mutex_lock (&mutexObj);
			screenTextures.push_back(	TextureManager::getInstance()->loadRawTexture(	str2,
																						frmClntHndlr->getFrameBufferMyScTyp(),
																						//frmClntHndlr->getFrameBufferNULL(),
																						GL_LINEAR,
																						512,
																						512,
																						GL_RGBA32F,
																						GL_RGBA,
																						GL_TEXTURE_RECTANGLE )	);


		/*
		if(cntTest<10){
			cimg_library::CImg<float>* img_test = new cimg_library::CImg<float>(WIN_WIDTH,WIN_HEIGHT,1,4,0);
			float * pixData = frmClntHndlr->getFrameBufferMyScTyp();
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
		pthread_mutex_unlock (&mutexObj);
		}
		loadedTexture = true;
	}
	else
	{
		//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture-------------*********"<<endl;
		for  (i=1;i<np;++i)
		{
			//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture screenTextures-------------*********"<<endl;
			//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture screenTextures number of items "<<screenTextures.size()<<" -------------*********"<<endl;
			TextureManager::getInstance()->updateTexture(	screenTextures[i-1],
															colorPixels[i-1],
															GL_LINEAR,
															512,
															512,
															GL_RGBA32F,
															GL_RGBA,
															GL_TEXTURE_RECTANGLE );

		}

		if(webScktsFlg){
			//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture screenBG webSckts-------------*********"<<endl;
			//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture screenTextures number of items "<<screenTextures.size()<<" -------------*********"<<endl;
			//cout<<"**********--------PID "<<pid<<" updateTexture loadedTexture screenTextures BG item "<<i<<" -------------*********"<<endl;

			pthread_mutex_lock (&mutexObj);


			TextureManager::getInstance()->updateTexture(	screenTextures[i-1],
														//colorPixels[i-1],
														frmClntHndlr->getFrameBufferMyScTyp(),
														//frmClntHndlr->getFrameBufferNULL(),
														GL_LINEAR,
														512,
														512,
														GL_RGBA32F,
														GL_RGBA,
														GL_TEXTURE_RECTANGLE );

			pthread_mutex_unlock (&mutexObj);
		}
		else{
			TextureManager::getInstance()->updateTexture(	screenTextures[i-1],
																imgCmpPtr,
																	GL_LINEAR,
																	512,
																	512,
																	GL_RGBA32F,
																	GL_RGBA,
																	GL_TEXTURE_RECTANGLE );
		}
	}
	//cout<<"**********--------PID "<<pid<<" updateTexture END-------------*********"<<endl;
}
//
//=======================================================================================
//



void display_master( void )
{
	int i;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//cout<<"***************** display_master pid "<<pid<<" ********************"<<endl;

// image composition
	if (loadedTexture)
	{
		fbo_manager->fbos[str_display_fbo].fbo->Bind();
		fbo_manager->setTarget( str_display_tex, true, true );

		GLint viewp[4];
		glGetIntegerv( GL_VIEWPORT, &viewp[0] );
		int wScene = viewp[2];
		int hScene = viewp[3];

		fbo_manager->proj_manager->setOrthoProjection(	0,
														0,
														wScene,
														hScene,
														0,
														512,
														0,
														512,
														true										);

		glActiveTexture	( GL_TEXTURE0 );


		for (i=0; i<np; i++ )
		{
			glBindTexture(GL_TEXTURE_RECTANGLE, screenTextures[i] );
			glsl_manager->activate( str_screen_composition_rect );

			fbo_manager->renderQuadMA(str_screen_composition_rect, 512,512, 512, 512);
			glsl_manager->deactivate( str_screen_composition_rect );
		}
		glBindTexture(GL_TEXTURE_RECTANGLE, 0 );

		fbo_manager->proj_manager->restoreProjection();

		fbo_manager->fbos[str_display_fbo].fbo->Disable();

	}



	if(!webScktsFlg){
		fbo_manager->displayTexture( str_pass_rect,
										 str_display_tex,
										 fbo_manager->fbos["display_fbo"].fbo_width,
										 fbo_manager->fbos["display_fbo"].fbo_height );
		glutSwapBuffers();
	}
	else{


		if (_server_.sendMoreFrames() ){
			//cout<<"sendMoreFrames true"<<endl;

			//unsigned char pixels[fbo_manager->fbos["display_fbo"].fbo_width*fbo_manager->fbos["display_fbo"].fbo_height*3];
			unsigned char *pixels = fbo_manager->getswpBuffin("display_fbo");
			//unsigned char pixels[256*512*3];
			unsigned char *pixels2 = fbo_manager->getswpBuffout("display_fbo");

			//glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
			//cout<<"***************** display_master pid "<<pid<<" Before displayTexture********************"<<endl;
			//cout<<"***************** display_master fbo_width "<<fbo_manager->fbos["display_fbo"].fbo_width<<" fbo_height "<<fbo_manager->fbos["display_fbo"].fbo_height<<" ********************"<<endl;

			fbo_manager->displayTexture( str_pass_rect,
											 str_display_tex,
											 fbo_manager->fbos["display_fbo"].fbo_width,
											 fbo_manager->fbos["display_fbo"].fbo_height,
											 pixels);
			//cout<<"***************** display_master pid "<<pid<<" After displayTexture********************"<<endl;
			//glReadPixels (0,0,WIN_WIDTH,WIN_HEIGHT,GL_DEPTH_COMPONENT, GL_FLOAT, pixelsf );
			/*for(unsigned int cf=0,c=0;cf<(WIN_WIDTH*WIN_HEIGHT);++cf){

				//aux = ( 2.0 * zNear ) / ( zFar + zNear - pixelsf[cf] * ( zFar - zNear ) );
				aux = pixelsf[cf];
				pixels[c] = (unsigned char)(aux*255.0);
				++c;
				pixels[c] = (pixels[c-1]);
				++c;
				pixels[c] = (pixels[c-1]);
				++c;
			}*/

			/*
			cout<<" ------------------------------------Out first two bytes ";
			for(unsigned int cf=0;cf<8;++cf)
				cout<<" "<<(int)pixels[cf]<<" ";
			cout<<"----------------------------------------"<<endl;
			*/

			for(unsigned int c1=0,c2=0;c1<fbo_manager->getswpBuffinLen();++c1,++c2){
				pixels2[c2]=pixels[c1];
				pixels2[++c2]=pixels[++c1];
				pixels2[++c2]=pixels[++c1];
				pixels2[++c2]=255;
			}


			_server_.sendFrame(pixels2);
		}
		//else
			//cout<<"sendMoreFrames false"<<endl;

		if (rcx.windowless)
			glXSwapBuffers(rcx.dpy, rcx.pbuffer);
		else
			glXSwapBuffers(rcx.dpy, rcx.win);
	}


}

#endif
//
//=======================================================================================
//
void display( void )
{
	//cout<<"***************** display pid "<<pid<<" ********************"<<endl;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	fbo_manager->fbos[crowd_manager->getCrowds()[0]->getFboLodName()].fbo->Bind();
	{
		fbo_manager->setTarget( crowd_manager->getCrowds()[0]->getFboPosTexName(), true, true );
		fbo_manager->proj_manager->setOrthoProjection(
			crowd_manager->getCrowds()[0]->getWidth(),
			crowd_manager->getCrowds()[0]->getHeight(),
			true										);


		glsl_manager->activate( "tbo" );
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_BUFFER, crowd_manager->getCrowds()[0]->getCudaTBO() );
			glBindBuffer( GL_ARRAY_BUFFER, crowd_manager->getCrowds()[0]->getPosTexCoords() );
			{
				glEnableClientState			( GL_VERTEX_ARRAY												);
				glVertexPointer				( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET					);
				glDrawArrays				( GL_POINTS, 0, crowd_manager->getCrowds()[0]->getPosTexSize()	);
				glDisableClientState		( GL_VERTEX_ARRAY												);
			}
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_BUFFER, 0 );
		}
		glsl_manager->deactivate( "tbo" );
		fbo_manager->proj_manager->restoreProjection();
	}
	fbo_manager->fbos[crowd_manager->getCrowds()[0]->getFboLodName()].fbo->Disable();


	fbo_manager->fbos["mdp_fbo"].fbo->Bind();
	{
		fbo_manager->setTarget( "mdp_tex", true, true );
		fbo_manager->proj_manager->setOrthoProjection(	fbo_manager->fbos["mdp_fbo"].fbo_width,
														fbo_manager->fbos["mdp_fbo"].fbo_height,
														true									);
		glsl_manager->activate( "mdp_floor" );
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_RECTANGLE, obstacle_manager->getPolicyTextureId() );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_RECTANGLE, obstacle_manager->getDensityTextureId() );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D_ARRAY, obstacle_manager->getArrowsTextureId() );
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, obstacle_manager->getLayer0TextureId() );
			glActiveTexture( GL_TEXTURE4 );
			glBindTexture( GL_TEXTURE_2D, obstacle_manager->getLayer1TextureId() );
			glBindBuffer( GL_ARRAY_BUFFER, obstacle_manager->getMdpTexCoordsId() );
			{
				glEnableClientState			( GL_VERTEX_ARRAY										);
				glVertexPointer				( 4, GL_FLOAT, sizeof(Vertex), LOCATION_OFFSET			);
				glEnableClientState			( GL_TEXTURE_COORD_ARRAY								);
				glTexCoordPointer			( 2, GL_FLOAT, sizeof(Vertex), TEXTURE_OFFSET			);
				glDrawArrays				( GL_POINTS, 0, obstacle_manager->getMdpTexCoordsSize()	);
				glDisableClientState		( GL_TEXTURE_COORD_ARRAY								);
				glDisableClientState		( GL_VERTEX_ARRAY										);
			}
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glActiveTexture( GL_TEXTURE4 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		glsl_manager->deactivate( "mdp_floor" );
		fbo_manager->proj_manager->restoreProjection();
	}
	fbo_manager->fbos["mdp_fbo"].fbo->Disable();


	fbo_manager->fbos["display_fbo"].fbo->Bind();
	{
		camera->setView();
		fbo_manager->setTarget( str_display_tex, true, true );
		draw();

		glPushAttrib( GL_LIGHTING_BIT );
		{
			glDisable(GL_LIGHTING);
			if( showstats )
			{
				GLint viewp[4];
				glGetIntegerv( GL_VIEWPORT, &viewp[0] );
				int wScene = viewp[2];
				int hScene = viewp[3];

				fbo_manager->proj_manager->setOrthoProjection(	0,
																0,
																hScene/4,
																hScene/4,
																0,
																crowd_manager->getCrowds()[0]->getWidth(),
																0,
																crowd_manager->getCrowds()[0]->getHeight(),
																true										);


				fbo_manager->pushActiveBind( crowd_manager->getCrowds()[0]->getFboPosTexName(), 0 );
				fbo_manager->renderQuad(	crowd_manager->getCrowds()[0]->getFboLodName(),
											str_pass_rect,
											crowd_manager->getCrowds()[0]->getWidth(),
											crowd_manager->getCrowds()[0]->getHeight() );
				fbo_manager->popActiveBind();
				fbo_manager->proj_manager->restoreProjection();

			}
#ifdef CUDA_PATHS
			fbo_manager->pushActiveBind( "mdp_tex", 0 );
			glsl_manager->activate( str_pass_rect );
			float tw = (float)fbo_manager->fbos["mdp_fbo"].fbo_width;
			float th = (float)fbo_manager->fbos["mdp_fbo"].fbo_height;
			glBegin( GL_QUADS );
			{
				glTexCoord2f( 0.0f,	0.0f	);	glVertex3f( -PLANE_SCALE,	0.0f,	 PLANE_SCALE	);
				glTexCoord2f( tw,	0.0f	);	glVertex3f(  PLANE_SCALE,	0.0f,	 PLANE_SCALE	);
				glTexCoord2f( tw,	th		);	glVertex3f(	 PLANE_SCALE,	0.0f,	-PLANE_SCALE	);
				glTexCoord2f( 0.0f,	th		);	glVertex3f( -PLANE_SCALE,	0.0f,	-PLANE_SCALE	);
			}
			glEnd();
			glsl_manager->deactivate( str_pass_rect );
			fbo_manager->popActiveBind();
#endif
		}
		glPopAttrib();

		if( showstats )
		{
			glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
			{
				glDisable( GL_LIGHTING );
				glDisable( GL_TEXTURE_2D );
				ProjectionManager::displayText( 10,  20, (char*)str_fps.c_str()			);
				ProjectionManager::displayText( 10,  40, (char*)str_delta_time.c_str()	);
				ProjectionManager::displayText( 10,  60, (char*)str_culled.c_str()		);
				ProjectionManager::displayText( 10, 100, (char*)str_lod1.c_str()		);
				ProjectionManager::displayText( 10, 120, (char*)str_lod2.c_str()		);
				ProjectionManager::displayText( 10, 140, (char*)str_lod3.c_str()		);
			}
			glPopAttrib();
		}
	}
#if defined MPI_PATHS_ON_NODES
	if (d_total != d_culled ) // if there are some characters in screen, then
	{
//		printf ("Capturing!\n");
		captureScreen ((float*)colorPixels, str_display_fbo, str_display_tex, str_display_depth_tex);
	}
#endif
	fbo_manager->fbos["display_fbo"].fbo->Disable();
	fbo_manager->displayTexture( str_pass_rect,
								 str_display_tex,
								 fbo_manager->fbos["display_fbo"].fbo_width,
								 fbo_manager->fbos["display_fbo"].fbo_height );
	glFlush();
	if(!webScktsFlg){


	/*
		fbo_manager->renderTiled( str_depth_rect,
								  str_display_depth_tex,
								  0,
								  fbo_manager->fbos["display_fbo"].fbo_width,
								  fbo_manager->fbos["display_fbo"].fbo_height,
								  fbo_manager->fbos["display_fbo"].fbo_width,
								  fbo_manager->fbos["display_fbo"].fbo_height );
	*/

		glutSwapBuffers();
	}
	else{
		if (rcx.windowless)
			glXSwapBuffers(rcx.dpy, rcx.pbuffer);
		else
			glXSwapBuffers(rcx.dpy, rcx.win);
	}


}
//
//=======================================================================================
//
void reshape( int w, int h )
{
	glViewport( 0, 0, (GLsizei) w, (GLsizei) h );
	if( !fbo_manager->updateFboDims( str_display_fbo, (unsigned int) w, (unsigned int) h ) )
	{
		log_manager->log( LogManager::LERROR, "FBO update failed!" );
		cleanup();
		exit( 1 );
	}
	else
	{
		log_manager->log( LogManager::FBO_MANAGER, "Reshaped to %ix%i", w, h );
	}
	glsl_manager->activate( "vfc_lod_assignment" );
	{
		glsl_manager->setUniformf( "vfc_lod_assignment", (char*)str_tang.c_str(),      	camera->getFrustum()->TANG			);
		glsl_manager->setUniformf( "vfc_lod_assignment", (char*)str_farPlane.c_str(),  	camera->getFrustum()->getFarD()		);
		glsl_manager->setUniformf( "vfc_lod_assignment", (char*)str_nearPlane.c_str(), 	camera->getFrustum()->getNearD()	);
		glsl_manager->setUniformf( "vfc_lod_assignment", (char*)str_ratio.c_str(),		camera->getFrustum()->getRatio()	);
	}
	glsl_manager->deactivate( "vfc_lod_assignment" );

	camera1->setView();
    camera2->setView();
    camera->setView();
}
//
//=======================================================================================
//
void reshape_master ( int w, int h )
{
	glViewport( 0, 0, (GLsizei) w, (GLsizei) h );
	if( !fbo_manager->updateFboDims( str_display_fbo, (unsigned int) w, (unsigned int) h ) )
	{
		log_manager->log( LogManager::LERROR, "FBO update failed!" );
		cleanup();
		exit( 1 );
	}
	else
	{
		log_manager->log( LogManager::FBO_MANAGER, "Reshaped to %ix%i", w, h );
	}

	camera1->setView();
    camera2->setView();
    camera->setView();
}
//
//=======================================================================================
//
//->KEYBOARD_AND_MOUSE_FUNCTIONS
#include "cPeripherals.h"
int wh = WIN_WIDTH * WIN_HEIGHT* 4;
//<-KEYBOARD_AND_MOUSE_FUNCTIONS
//
//=======================================================================================
//
// TODO: update idle
//
//=======================================================================================
//
// TODO: update idle
//=======================================================================================
//
void uploadScreenTextures () // Target: screen color or depth
{
	int i;

	//for (i=0;i<np-1;i++)
	for (i=0;i<np;i++)
	{
		//cout << " screenTexIds " << screenTexIds[i] << " np " << np << endl;
		glTextureImage2DEXT (screenTexIds[i], GL_TEXTURE_2D, 0, GL_RGBA32F,
				WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, colorPixels[i]  );


	}
// For debugging:
//			printf ("\n\n");
//			for (i=64;i<128*4;i+=4)
//			{
//				printf ("[%d, %d, %d, %d]\t", colorPixels[0][i], colorPixels[0][i+1], colorPixels[0][i+2], colorPixels[0][i+3]);
//			}
//			printf ("\n\n");

}
//
//=======================================================================================
//
void idle2 (void)
{
	frames++;
	time_=glutGet(GLUT_ELAPSED_TIME);

	if (time_ - timebase > 1000) {
		fps	= frames*1000.0f/(time_-timebase);
		//printf ("Process: %d FPS:%4.2f ms:%4.3f \n",pid, fps, 1.0f/fps*1000.0f);
		timebase	= time_;
		frames		= 0;

	}

	if (np>1)
	{
		sendReceiveFrames 		();
		checkMasterUserInput   	();
	}
	if ( pid==0 && np > 1)
	{
		uploadScreenTextures	();
		eventsFromBrowser 		(); // mouse and keyboard events are caught from the browser
	}
}
//
//=======================================================================================
//

void idle( void )
{
#if defined MPI_PATHS_ON_NODES

	int i = 0;

	/*
	if(webScktsFlg){
		checkMasterUserInput 	();
	//	MPI_Barrier				(MPI_COMM_WORLD);
		sendReceiveFrames 		();
	}
	else{
		sendReceiveFrames 		();
		checkMasterUserInput 	();
	}
	*/
	//cout<<"**********--------PID "<<pid<<" idle start-------------*********"<<endl;
	sendReceiveFrames 		();
	//cout<<"**********--------PID "<<pid<<" idle sendReceiveFrames-------------*********"<<endl;
	checkMasterUserInput 	();
	//cout<<"**********--------PID "<<pid<<" idle checkMasterUserInput-------------*********"<<endl;
	MPI_Barrier				(MPI_COMM_WORLD);

	//cout<<"**********--------PID "<<pid<<" idle  MPI_BARRIER-------------*********"<<endl;

	if (pid != 0)
	{
		//cout<<"**********--------PID "<<pid<<" idle  child cicle start-------------*********"<<endl;
#endif
		Timer::getInstance()->update();
		fps = Timer::getInstance()->getFps();
		delta_time = (fps <= 1.0f ? one_thirtieth : 1.0f / fps);
		frame_counter++;
		time_counter += delta_time;
		//cout<<"**********--------PID "<<pid<<" idle  child cicle 1-------------*********"<<endl;
		if( time_counter > one_sixtieth )
		{
			if( runpaths )
			{
				crowd_manager->runPaths();
			}
			if( animating )
			{
				crowd_manager->nextFrame();
			}
			time_counter = 0.0f;
		}

		//cout<<"**********--------PID "<<pid<<" idle  child cicle 2-------------*********"<<endl;
		// EVERY 40 FRAMES, UPDATE STATS
		if( frame_counter % 40 == 0 )
		{
			str_fps = string( "FPS:        " );
			stringstream ss1;
			ss1 << fps;
			str_fps.append( ss1.str() );

			str_delta_time = string( "DELTA TIME: " );
			stringstream ss6;
			ss6 << delta_time;
			str_delta_time.append( ss6.str() );

			d_lod1 	= 0;
			d_lod2 	= 0;
			d_lod3 	= 0;
			d_total	= 0;

			//cout<<"**********--------PID "<<pid<<" idle  child cicle 3-------------*********"<<endl;

			vector<Crowd*> d_crowds = crowd_manager->getCrowds();
			for( unsigned int c = 0; c < d_crowds.size(); c++ )
			{
				Crowd* d_crowd = d_crowds[c];
				d_lod1 += d_crowd->models_drawn[0];
				d_lod2 += d_crowd->models_drawn[1];
				d_lod3 += d_crowd->models_drawn[2];
				d_total += ( d_crowd->getWidth() * d_crowd->getHeight() );
			}
			d_culled = d_total - d_lod1 - d_lod2 - d_lod3;

			str_culled = string( "CULLED:     " );
			stringstream ss_culled;
			ss_culled << d_culled;
			str_culled.append( ss_culled.str() );

			str_lod1   = string( "LOD1:       " );
			stringstream ss_lod1;

			ss_lod1 << d_lod1;
			str_lod1.append( ss_lod1.str() );

			str_lod2   = string( "LOD2:       " );
			stringstream ss_lod2;
			ss_lod2 << d_lod2;
			str_lod2.append( ss_lod2.str() );

			//cout<<"**********--------PID "<<pid<<" idle  child cicle 4-------------*********"<<endl;

			str_lod3   = string( "LOD3:       " );
			stringstream ss_lod3;
			ss_lod3 << d_lod3;
			str_lod3.append( ss_lod3.str() );
		}

		//cout<<"**********--------PID "<<pid<<" idle  child cicle 5-------------*********"<<endl;
	#ifdef CUDA_PATHS
		// EVERY 30 FRAMES, UPDATE DENSITY
		if( frame_counter % 30 == 0 )
		{
			vector<float> a_density;
			crowd_manager->getDensity( a_density, 0 );
			obstacle_manager->updateDensity( a_density );
		}

	//->MDP_ITERATION
	#ifdef _WIN32
		DWORD init_tickAtStart = GetTickCount();
		DWORD init_elapsedTicks;
	#elif defined __unix
		timeval start, stop, result_cpu;
		gettimeofday( &start, NULL );
	#endif
		float time_cpu;
		switch( obstacle_manager->getState() )
		{
			case MDPS_IDLE:
			case MDPS_READY:
			case MDPS_ERROR:
				break;
			case MDPS_INIT_STRUCTURES_ON_HOST:
				obstacle_manager->initStructuresOnHost();
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "HOST_INITED_STRUCTURES_IN: %010.7fms.\n", time_cpu );
				break;
			case MDPS_INIT_PERMS_ON_DEVICE:
				obstacle_manager->initPermsOnDevice();
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "DEVICE_INITED_PERMS_IN:    %010.7fms.\n", time_cpu );
				break;
			case MDPS_UPLOADING_TO_DEVICE:
				obstacle_manager->uploadToDevice();
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "UPLOADED_TO_DEVICE_IN:     %010.7fms.\n", time_cpu );
				break;
			case MDPS_ITERATING_ON_DEVICE:
				obstacle_manager->iterateOnDevice();
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "DEVICE_ITERATED_IN:        %010.7fms.\n", time_cpu );
				break;
			case MDPS_DOWNLOADING_TO_HOST:
				obstacle_manager->downloadToHost();
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "DOWNLOADED_TO_HOST_IN:     %010.7fms.\n", time_cpu );
				break;
			case MDPS_UPDATING_POLICY:
				obstacle_manager->updatePolicy();
				crowd_manager->updatePolicy( obstacle_manager->getPolicy() );
	#ifdef _WIN32
				init_elapsedTicks = GetTickCount() - init_tickAtStart;
				time_cpu = (float)init_elapsedTicks;
	#elif defined __unix
				gettimeofday( &stop, NULL );
				timersub( &stop, &start, &result_cpu );
				time_cpu = (float)result_cpu.tv_sec*1000.0f+(float)result_cpu.tv_usec/1000.0f;
	#endif
				printf( "POLICY_UPDATED_IN:         %010.7fms.\n", time_cpu );
				break;
		}
	//<-MDP_ITERATION
	#endif

#if defined MPI_PATHS_ON_NODES
	//cout<<"**********--------PID "<<pid<<" idle  child cicle 6-------------*********"<<endl;
	}
	else //  if (pid == 0)
	{
		//cout<<"**********--------PID "<<pid<<" idle  master cicle start-------------*********"<<endl;
		Timer::getInstance()->update();
		fps = Timer::getInstance()->getFps();
		delta_time = (fps <= 1.0f ? one_thirtieth : 1.0f / fps);
		//cout<<"**********--------PID "<<pid<<" idle  master cicle 1-------------*********"<<endl;
		frame_counter++;
		time_counter += delta_time;


		if( time_counter > one_sixtieth )
		{
			time_counter = 0.0f;
		}

		// EVERY 40 FRAMES, UPDATE STATS
		if( frame_counter % 40 == 0 )
		{
			//cout<<"**********--------PID "<<pid<<" idle  master cicle 2-------------*********"<<endl;
			str_fps = string( "FPS:        " );
			stringstream ss1;
			ss1 << fps;
			str_fps.append( ss1.str() );

			str_delta_time = string( "DELTA TIME: " );
			stringstream ss2;
			ss2 << delta_time;
			str_delta_time.append( ss2.str() );
		}

		//cout<<"**********--------PID "<<pid<<" idle updateScreenTextures-------------*********"<<endl;
		updateScreenTextures 	();
		//cout<<"**********--------PID "<<pid<<" idle after updateScreenTextures-------------*********"<<endl;

		if(webScktsFlg){
			//cout<<"**********--------PID "<<pid<<" idle eventsFromBrowser-------------*********"<<endl;
			eventsFromBrowser 		(); // mouse and keyboard events are caught from the browser
		}
	}

#endif
	if(!webScktsFlg)
		glutPostRedisplay();
	//cout<<"**********--------PID "<<pid<<" idle end-------------*********"<<endl;
}
//
//=======================================================================================
//
void cleanup( void )
{
	log_manager->separator			(														);
	log_manager->log				( LogManager::EXIT, "Slave. CASIM shutting down..." 	);
    glutIdleFunc					( NULL 													);
    glFlush							(														);
    Timer::freeInstance				(														);
    log_manager->log				( LogManager::EXIT, "Clearing TEXTURES..." 				);
    TextureManager::freeInstance	(														);
    log_manager->log				( LogManager::EXIT, "Clearing MANAGERS..." 				);
	FREE_INSTANCE					( err_manager  											);
	FREE_INSTANCE					( vbo_manager											);
	FREE_INSTANCE					( fbo_manager											);
	FREE_INSTANCE					( glsl_manager											);
	FREE_INSTANCE					( crowd_manager											);
	FREE_INSTANCE					( skybox_manager										);
	log_manager->log				( LogManager::EXIT, "Clearing MODELS..." 				);
	FREE_INSTANCE					( camera1      											);
	FREE_INSTANCE					( camera2      											);
	FREE_INSTANCE					( axes         											);
	log_manager->log				( LogManager::EXIT, "Memory free. Exiting." 			);
	log_manager->console_separator	(														);
	FREE_INSTANCE					( log_manager  											);

#if defined OSC_PATHS
	pthread_cancel( threads[0] );
#endif

#if defined MPI_PATHS_ON_NODES
	FREE_ARRAY(colorPixels);
	FREE_ARRAY(depthPixels);
#endif

}
//
//=======================================================================================
//

#ifdef MPI_PATHS_ON_NODES

void cleanup_master ( void )
{
	log_manager->separator			(														);
	log_manager->log				( LogManager::EXIT, "Master. CASIM shutting down..." 	);
//    glutIdleFunc					( NULL 													);
    glFlush							(														);
    Timer::freeInstance				(														);
    log_manager->log				( LogManager::EXIT, "Clearing TEXTURES..." 				);
    TextureManager::freeInstance	(														);
    log_manager->log				( LogManager::EXIT, "Clearing MANAGERS..." 				);
	FREE_INSTANCE					( err_manager  											);
	FREE_INSTANCE					( vbo_manager											);
	FREE_INSTANCE					( fbo_manager											);
	FREE_INSTANCE					( glsl_manager											);
	FREE_INSTANCE					( skybox_manager										);
	log_manager->log				( LogManager::EXIT, "Clearing MODELS..." 				);
	FREE_INSTANCE					( camera1      											);
	FREE_INSTANCE					( camera2      											);
	FREE_INSTANCE					( axes         											);
	log_manager->log				( LogManager::EXIT, "Memory free. Exiting." 			);
	log_manager->console_separator	(														);
	FREE_INSTANCE					( log_manager  											);

	int i;
	for (i=0;i<np-1;i++)
	{
		FREE_ARRAY(colorPixels[i]);
	}
	FREE_ARRAY(colorPixels);



}

//
//=======================================================================================
//

bool readPng2Mem( bool ident ){
	try{

		if(!ident){
			imageBG = new cimg_library::CImg<float>("assets/background/background_7.png");
			imageDM = new cimg_library::CImg<float>("assets/background/background_depth_7.png");


			unsigned int sizeImg = imageBG->height()*imageBG->width()*4;
			unsigned char spImg1,spImg2;
			imgCmpPtr = new float[sizeImg];

			cout<<" imgBG width "<<imageBG->width()<<" height "<<imageBG->height()<<" spectrum "<<imageBG->spectrum()<<" size " <<imageBG->size()<<
				" -------- imgDM width "<<imageDM->width()<<" height "<<imageDM->height()<<" spectrum "<<imageDM->spectrum()<<" size " <<imageDM->size()<<endl;

			spImg1 = imageBG->spectrum();
			spImg2 = imageDM->spectrum();

			if(!(spImg1==3 || spImg1==4))
				return false;

			unsigned int cnt=0,cntx,cnty;
			float valueF,ZNEAR,ZFAR,A,B;
			ZNEAR = 10.1;
			ZFAR = 500.0;
			A=ZNEAR-ZFAR;
			B=ZFAR+ZNEAR;
			for(cnty=0; cnty<imageBG->width();++cnty){
				for(cntx=0; cntx<imageBG->height();++cntx){
					imgCmpPtr[cnt++] = (*(imageBG->data(cntx,cnty,0,0)))/(255.0);
					imgCmpPtr[cnt++] = (*(imageBG->data(cntx,cnty,0,1)))/(255.0);
					imgCmpPtr[cnt++] = (*(imageBG->data(cntx,cnty,0,2)))/(255.0);
					//imgCmpPtr[cnt++] = 0.99;
					valueF = (*(imageDM->data(cntx,cnty,0,0)))/(255.0);
					valueF += 0.03;
					//valueF = 1.0;
					//valueF = (-1.0*valueF*ZNEAR)/(valueF*(ZFAR-ZNEAR)-ZFAR);
					//imgCmpPtr[cnt++]	= (valueF*ZNEAR)/(ZFAR+A*valueF);
					imgCmpPtr[cnt++]	= valueF;
					//cout<<" valueF "<<valueF <<" cnt "<<cnt<<" zNEW "<<imgCmpPtr[cnt-1]<<endl;
					//imgCmpPtr[cnt++] = (valueF*A + ZFAR)/(valueF*B - ZFAR);

					//imgCmpPtr[cnt++] = (*(imageDM->data(cntx,cnty,0,0)))/(255.0);


				}
			}
		}
		else{
			imgCmpPtr = new float[512*512*4];
			for(unsigned cnt=0;cnt<(512*512*4);++cnt)
				imgCmpPtr[cnt] = 1.0;
		}


		/*
		for(cnt=0,cnt2=0,cnt3=0;cnt<90;cnt++){
			cout<<" cnt "<<cnt<<" value "<<(*imageBG)[cnt];
			cout<<" --------- cnt2 "<<cnt2<<" cnt3 "<<cnt3<<" value "<<*(imageBG->data(cnt2,0,0,cnt3))<<endl;
			++cnt3;
			if(cnt3==3){
				cnt3=0;
				++cnt2;
			}

		}*/

		//cout<<" cnt "<<cnt<<" cnt2 "<<cnt2<<" cnt3 "<<cnt3<<endl;

	}
	catch(...){
		return false;
	}
	return true;
}
//
//=======================================================================================
//

void *webSocketServer (void *threadArgs)
{
	cout << "launching server at port " << SERVER_PORT << " in process with id:"<<pid<< endl;
    _server_.run(SERVER_PORT);

    cerr << "Exiting thread!" << endl;
	pthread_exit(NULL);
}
//
//=======================================================================================
//
void launchWebSocketServer ()
{
	// Next line solve the bug in Ubuntu 3.10 and Nvidia Drivers
	//	https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
	int thread=pthread_getconcurrency();

	mouseHandler	= new cMouseHandler 	();
	keyboardHandler = new cKeyboardHandler 	();

	frmClntHndlr	= new cFrameClientHandler(WIN_WIDTH,WIN_HEIGHT);

	frmClntHndlr->compute_scalMod(0.0,171.524,1.7);

	_server_.setMouseHandler 	( mouseHandler		);
	_server_.setKeyboardHandler	( keyboardHandler 	);
	_server_.setFrmClntHandler	( frmClntHndlr		);
	_server_.setMutexObj		( &mutexObj );

	int rc = pthread_create( &threads2, NULL, webSocketServer, NULL );
	if( rc )
	{
		cout << "Websocket Server Launch Error. The Websocket Server thread can not be created. \n";

		FREE_MEMORY		( mouseHandler				);
		FREE_MEMORY		( keyboardHandler			);
		FREE_MEMORY		( frmClntHndlr				);
	    exit( 1 );
	}
}
//
//=======================================================================================
//
void initScreenArraysMaster ()
{
	int i;

	// initializing arrays that will store each node screen
	colorPixels = new myScreenType *[np]; // a screenBuffer per slave and one for the background sent by the browse client
	//colorPixels = new myScreenType *[np-1]; // a screenBuffer per slave
	//for (i=0;i<np-1;i++)
	for (i=0;i<np;i++)
	{
		colorPixels[i] = new myScreenType [ wh ];
	}
	pixelSizeMsg = wh * 4 * sizeof (myScreenType );
	//screenTexIds = new unsigned int [np-1];
	screenTexIds = new unsigned int [np];

//	for (i = 0; i< np; i++)
//	{
//		glTextureImage2DEXT (screenTexIds[i], GL_TEXTURE_RECTANGLE, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0  );
//	}
}
//
//=======================================================================================
//
void initScreenArrays ()
{
	int wh = WIN_WIDTH * WIN_HEIGHT;
	colorPixels = new myScreenType*[ wh * 4 ];
	depthPixels	= new myScreenType [ wh ];
	//depthPixels	= new myScreenType [ wh * 4];
	pixelSizeMsg 	= wh * 4 * sizeof (myScreenType );
}
//
//=======================================================================================
//
void display_wbSckts(RenderContext *rcx)
{

	/*
	int i,j;
	myScreenType *ptr;

	glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	camera->setView ( MVPmatUBO,  transformMat, WIN_WIDTH, WIN_HEIGHT);

	glUseProgram	( passthruInstancingShader );
		glUniform1i (colorLoc, pid );
		glBindBufferBase		( GL_UNIFORM_BUFFER, 0, MVPmatUBO 	); // passing Modelview Projection matrix
		model->drawInstanced	( NUM_CHARACTERS, GL_TEXTURE_BUFFER, agentPosTBO );
	glUseProgram	(0);

	//displayAxis ();


	if (depthPixels)
	{


	// Reading color and depth information
	glReadBuffer ( GL_BACK );
	glReadPixels ( 0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGBA, GL_FLOAT, (myScreenType*)colorPixels);
	glReadPixels ( 0, 0, WIN_WIDTH, WIN_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, depthPixels );
	// Storing depth into color pixels

	ptr = (myScreenType*)colorPixels;

	for (i=0, j=0;i<WIN_WIDTH*WIN_HEIGHT*4;i+=4,j++)
			{
				ptr[i+3] = depthPixels[j];
			}
	}
	*/

	display();

	if (rcx->windowless)
			glXSwapBuffers(rcx->dpy, rcx->pbuffer);
		else
			glXSwapBuffers(rcx->dpy, rcx->win);
}
//
//=======================================================================================
//
void display_master_wbSckts (RenderContext *rcx)
{

	/*
	int i = 0;
	unsigned char pixels[WIN_WIDTH*WIN_HEIGHT*3];

	float	pixelsf[WIN_WIDTH*WIN_HEIGHT];

	glEnable		(	GL_DEPTH_TEST	);


	glClear		 	( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	fboMaster->Bind();
		glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
		glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//for (i=0;i<np-1;i++)
		for (i=0;i<np;i++)
		{
			glUseProgram	( screenCompositionShader );
				model->draw(GL_TEXTURE_2D, screenTexIds[i]);
			glUseProgram	(0);
		}

		if (_server_.sendMoreFrames() )
		{

			float aux, zFar , zNear;
			zFar  = 350.0;
			zNear = 1.0;

			glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
			//glReadPixels (0,0,WIN_WIDTH,WIN_HEIGHT,GL_RGB, GL_UNSIGNED_BYTE, pixels );
			glReadPixels (0,0,WIN_WIDTH,WIN_HEIGHT,GL_DEPTH_COMPONENT, GL_FLOAT, pixelsf );
			for(unsigned int cf=0,c=0;cf<(WIN_WIDTH*WIN_HEIGHT);++cf){

				//aux = ( 2.0 * zNear ) / ( zFar + zNear - pixelsf[cf] * ( zFar - zNear ) );
				aux = pixelsf[cf];
				pixels[c] = (unsigned char)(aux*255.0);
				++c;
				pixels[c] = (pixels[c-1]);
				++c;
				pixels[c] = (pixels[c-1]);
				++c;
			}
			_server_.sendFrame(pixels);
		}

	fboMaster->Disable();

	glUseProgram	( screenCompositionShader );
		model->draw(GL_TEXTURE_2D, fboMasterColorTexId);
	glUseProgram	(0);
	*/

	display_master();

	if (rcx->windowless)
		glXSwapBuffers(rcx->dpy, rcx->pbuffer);
	else
		glXSwapBuffers(rcx->dpy, rcx->win);

}
//
//=======================================================================================
// TODO: freeAll_master
void freeAll_master ()
{
	/*
	glDeleteProgram ( passthruInstancingShader	);
	glDeleteProgram ( drawAxisShader			);
	glDeleteBuffers ( 1, &MVPmatUBO 			);

	FREE_MEMORY		( model 					);

	cout << "Calling MPI_Finalize() \n";
	MPI_Finalize();
	*/

	int i;
	//for (i=0;i<np-1;i++)
	for (i=0;i<np;i++)
	{
		FREE_ARRAY(colorPixels[i]);
	}
	FREE_ARRAY	( colorPixels		);
	FREE_ARRAY	( screenTexIds		);
	//FREE_MEMORY ( fboMaster 		);
	FREE_MEMORY	( mouseHandler		);
	FREE_MEMORY	( keyboardHandler	);
	FREE_MEMORY ( frmClntHndlr		);
//
//=======================================================================================
//
}
void freeAll ()
{
	glDeleteProgram ( passthruInstancingShader	);
	glDeleteBuffers ( 1, &MVPmatUBO 			);

	//FREE_MEMORY		( model 					);
	FREE_ARRAY		( colorPixels				);
	FREE_ARRAY		( depthPixels				);
}
//
//=======================================================================================
//
void glLoop (RenderContext *rcx)
{
	while (running)
	{
		if (pid==0) // composite node
			display_master ();
		else
			display ();

		//cout<<"**********************************-glLoop "<<pid <<"  idle-**********************************"<<endl;
		idle	();
		//cout<<"**********************************-glLoop "<<pid <<" end-**********************************"<<endl;
	}
}
//
//=======================================================================================
//
void runCompositeWindowless (int argc, char** argv)
{
	//cout<<"***************** runCompositeWindowless pid "<<pid<<" ********************"<<endl;
	pthread_mutex_init (&mutexObj, NULL);
	glLoop  		( &rcx );
	closeContext 	( &rcx );
	freeAll_master	(	);
	pthread_mutex_destroy(&mutexObj);

}
//
//=======================================================================================
//
void runRenderingWindowless (int argc, char** argv)
{
	//cout<<"***************** runRenderingWindowless pid "<<pid<<" ********************"<<endl;
	/*
	int glew_status = glewInit();
	if( glew_status != GLEW_OK )
	{
		cout <<	"GLEW initialization failed!.\n";
		exit	( 0 );
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	*/

	// Next line solve the bug in Ubuntu 3.10 and Nvidia Drivers
	//	https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642


	glLoop  		( &rcx );
	closeContext 	( &rcx );
	freeAll			(	);
}
//
//=======================================================================================
//
int main ( int argc, char *argv[] )
{
#ifdef _WIN32
	srand						(	time( NULL )				);
#elif defined __unix
    srandom						( 	getpid() 					);
#endif

    int i;


    Gargc = argc;
    Gargv = new char*[Gargc];

    for(char cCnt=0,ca,cb;cCnt<Gargc;++cCnt){
    	ca = 0;
    	while(argv[cCnt][ca]!=0)
    		++ca;

    	Gargv[cCnt] = new char[ca];

    	for(cb=0;cb<ca;++cb)
    		Gargv[cCnt][cb]=argv[cCnt][cb];
    }


	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &pid);
	MPI_Comm_size (MPI_COMM_WORLD, &np);


	if(pid==0 && !webScktsFlg){
		//cout <<"-*************************************root readPng2Mem PID "<<pid<<" -***************************************-"<<endl;
		readPng2Mem(true);
	}

	ostringstream log_file;
	log_file << "CASIM_LOG_" << pid << ".html";
	string str_log_file = log_file.str ();
    log_manager = new LogManager( str_log_file );

	// pixelSize is the slaves' screen size. Harcoded for now!
	// the screen size of the slaves is specified in cInits.h at init_fbos ()
    int w = 512, h = 512;
	pixelSize = w * h * 4 * sizeof (float);

	//cout<<"-*************************************-FASE 0.0 PID "<<pid<<" -***************************************-"<<endl;
	MPI_Barrier(MPI_COMM_WORLD);

	if ( pid == 0 ) // MASTER NODE
	{
		if (np != 5 && np != 10 && np != 17)
		{
			log_manager->log( LogManager::MPI, "Needed 5, 10 or 17 processes.\n");
			cleanup ();
			MPI_Abort( MPI_COMM_WORLD, 1);
			exit ( 1 );
		}

		colorPixels = new float *[np-1]; // a screenBuffer per slave
		for (i=0;i<np-1;i++)
		{
			colorPixels[i] = new float [w * h* 4];

		}


		init_master	( 	argc, argv	);

//		log_manager->file_log (LogManager::MPI, "calling data_server() by process %d BEGIN: \n", pid);
		data_server(argc, argv);
		//runSimulation ();
//		log_manager->file_log (LogManager::MPI, "calling data_server() by process %d END: \n", pid);

		if(!webScktsFlg){
			glutReshapeFunc 			( 	reshape_master				);
			glutDisplayFunc 			( 	display_master				);
			glutKeyboardFunc			( 	keyboard_master				);
			glutSpecialFunc 			(   special_master				);
			glutCloseFunc   			(   cleanup_master   			);
		}
		else{
		//	cout <<"-*************************************-FASE 1.1 root runRenderingAndComposite PID "<<pid<<" -***************************************-"<<endl;
			launchWebSocketServer	( );
			//cout<<"-*************************************-FASE 1.2 root PID "<<pid<<" -***************************************-"<<endl;

			initScreenArraysMaster	( );
			//cout<<"-*************************************-FASE 2 root PID "<<pid<<" -***************************************-"<<endl;
		}
	}
	else {

		colorPixels = new float*[w * h * 4];
		depthPixels	= new float [w*h];
		//depthPixels	= new float [w * h * 4];
		init						( 	argc,argv );
		err_manager->getError		( "BEFORE_RENDERING::" 			);

		//cout<<"-*************************************-FASE 1 slave PID "<<pid<<" -***************************************-"<<endl;

		if(!webScktsFlg){
			//cout<<"-*************************************-FASE 2 slave PID "<<pid<<" -***************************************-"<<endl;
			glutReshapeFunc 			( reshape   					);
			glutDisplayFunc 			( display   					);
			glutKeyboardFunc			( keyboard  					);
			glutSpecialFunc 			( special   					);
			glutCloseFunc   			( cleanup   					);
		}
		else{

			//cout<<"-*************************************-FASE 3 slave PID "<<pid<<" -***************************************-"<<endl;
			initScreenArrays();
		}
		//		log_manager->log 			(LogManager::MPI, "calling recv_data() by process %d BEGIN: \n", pid);
		recv_data					(								);
		//cout<<"-*************************************-FASE 4 slave PID "<<pid<<" -***************************************-"<<endl;
		//		compute_process				(								);
		//		log_manager->log 			(LogManager::MPI, "calling recv_data() by process %d END: \n", pid);
	}

	if(!webScktsFlg){
		glutMouseFunc				( mouse								);
		glutMotionFunc				( motion							);
		glutIdleFunc    			( idle								);

		glutMainLoop    			(           						);


		if(imageBG != nullptr)
			delete imageBG;

		if(imageDM != nullptr)
			delete imageBG;

		if(imgCmpPtr != nullptr)
			delete imgCmpPtr;

		//cout<<"-*************************************-FASE 5 PID "<<pid<<" -***************************************-"<<endl;
		//glutExit ();
	}
	else{
		if(pid==0){

			runCompositeWindowless  (argc, argv); // Runs composition mode
			//cout<<"-*************************************-FASE 6 root PID "<<pid<<" -***************************************-"<<endl;
		}
		else{
			runRenderingWindowless (argc, argv);
			//cout<<"-*************************************-FASE 6 slave PID "<<pid<<" -***************************************-"<<endl;
		}

	}


	return 0;

}

#else
int main( int argc, char *argv[] )
{
#ifdef _WIN32
	srand						(	time( NULL )				);
#elif defined __unix
    srandom						( 	getpid() 					);
#endif


#ifdef MPI_PATHS

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &pid);
	MPI_Comm_size (MPI_COMM_WORLD, &np);


	printf ("This is process %d, Number of processes %d\n",pid, np);

	MPI_Barrier(MPI_COMM_WORLD);

	if ( pid == 0 )
	{
#endif
		string log_file				( 	"CASIM_LOG.html" 			);
	    log_manager = new LogManager( 	log_file 					);

#ifdef MPI_PATHS
		if (np != 5 && np != 9 && np != 16)
		{
			log_manager->log( LogManager::MPI, "Needed 5, 9 or 16 processes.\n");
			cleanup ();
			MPI_Abort( MPI_COMM_WORLD, 1);
			exit ( 1 );
		}
		printf ("calling data_server() by process %d BEGIN: \n", pid);
		data_server(pid);
#endif
	    init						( 	argc,
	    								argv 						);

	    err_manager->getError		( "BEFORE_RENDERING::" 			);
	    glutReshapeFunc 			( reshape   					);
	    glutDisplayFunc 			( display   					);
	    glutKeyboardFunc			( keyboard  					);
	    glutSpecialFunc 			( special   					);
		glutMouseFunc				( mouse							);
		glutMotionFunc				( motion						);
	    glutIdleFunc    			( idle      					);
	    glutCloseFunc   			( cleanup   					);
	#ifdef OSC_PATHS
	    launchOSC					(								);
	#endif
	    glutMainLoop    			(           					);

#ifdef MPI_PATHS
	}
	else
	{
		printf ("calling compute_process() by process %d BEGIN: \n", pid);
		compute_process(pid);// DO MPI BEHAVIOR
	}
	MPI_Finalize ();
#endif

    return 0;
}
#endif
//
//=======================================================================================
