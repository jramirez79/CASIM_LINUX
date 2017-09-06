// @REV_TAG SRL_09_2013

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
//
//=======================================================================================
//
enum
{
	_keyChangedFlag = 0,
	_key,
	_ctrlDown,
	_lMouseDown,
	_rMouseDown,
	_mMouseDown,
	_mouseChangedFlag,
	_mouseX,
	_mouseY,
	_cameraChangedFlag,
	_dist2ground,
	_rectX,
	_rectZ,
	_camAngl,
	_scalCam,
	_camZoom,
	_ModelViewMat,_ModelViewMat1,_ModelViewMat2,_ModelViewMat3,_ModelViewMat4,_ModelViewMat5,_ModelViewMat6,_ModelViewMat7,
	_ModelViewMat8,_ModelViewMat9,_ModelViewMat10,_ModelViewMat11,_ModelViewMat12,_ModelViewMat13,_ModelViewMat14,
	_ModelViewMat15,
	_flagExit

};
int				MPI_interaction[_flagExit+1] = {0};
float MainRotAngX = 0.0;

//
//=======================================================================================
//
#if defined MPI_PATHS_ON_NODES
	unsigned char	MPI_key 		= 64;
	bool			keyChangedFlag 	= true;
#endif

//
//=======================================================================================
//

vec3 XAxis(1.0,0.0,0.0);

float deltaRot = 1.0;

void keyboard( unsigned char key, int x, int y )
{
	//printf( "KEY=%d\n", key );
	int pdi     = 0;
	vec3 camPos = vec3( 0.0f );
	vec3 camDir = vec3( 0.0f );
	vec3 camUp  = vec3( 0.0f );
	switch( key )
	{
#ifdef DEMO_SHADER
	case 49:
		doHeightAndDisplacement = !doHeightAndDisplacement;
		break;
	case 50:
		doColor = !doColor;
		break;
	case 51:
		doPatterns = !doPatterns;
		break;
	case 52:
		doFacial = !doFacial;
		break;
#endif
	case 43:	//'+'
		predef_cam_index = (predef_cam_index + 1) % predef_cam_pos.size();
		camPos = vec3(	predef_cam_pos[predef_cam_index].x,
                        predef_cam_pos[predef_cam_index].y,
                        predef_cam_pos[predef_cam_index].z );
		camDir = vec3(  predef_cam_dir[predef_cam_index].x,
                        predef_cam_dir[predef_cam_index].y,
                        predef_cam_dir[predef_cam_index].z );
		camUp  = vec3(  predef_cam_up[predef_cam_index].x,
                        predef_cam_up[predef_cam_index].y,
                        predef_cam_up[predef_cam_index].z );
        camera->setPosition( camPos );
        camera->setDirection( camDir );
        camera->setUpVec( camUp );
        camera->setView();
		break;
	case 45:	//'-'
		pdi = (int)predef_cam_index;
		if( (pdi-1) >= 0  )
		{
			predef_cam_index = predef_cam_index - 1;
		}
		else
		{
			predef_cam_index = predef_cam_pos.size() - 1;
		}
		camPos = vec3(	predef_cam_pos[predef_cam_index].x,
                        predef_cam_pos[predef_cam_index].y,
                        predef_cam_pos[predef_cam_index].z );
		camDir = vec3(  predef_cam_dir[predef_cam_index].x,
                        predef_cam_dir[predef_cam_index].y,
                        predef_cam_dir[predef_cam_index].z );
		camUp  = vec3(  predef_cam_up[predef_cam_index].x,
                        predef_cam_up[predef_cam_index].y,
                        predef_cam_up[predef_cam_index].z );
        camera->setPosition( camPos );
        camera->setDirection( camDir );
        camera->setUpVec( camUp );
        camera->setView();
		break;
	case 19:	//'CTRL+s'
		camera->moveDown( camAccel );
		camera->setView();
		break;
	case 23:	//'CTRL+w'
		camera->moveUp( camAccel );
		camera->setView();
		break;
#if !defined MPI_PATHS_ON_NODES
	case 27:
		glutExit();
		exit( 0 );
		break;
#endif
	case 'a':
		camera->moveLeft( camAccel );
		camera->setView();
		break;
	case 'c':
		if( camera == camera1 )
		{
			camera = camera2;
		}
		else
		{
			camera = camera1;
		}
		camNear = camera->getFrustum()->getNearD();
		break;
	case 'd':
		camera->moveRight( camAccel );
		camera->setView();
		break;
	case 'w':
		camera->moveForward( camAccel );
		camera->setView();
		break;
	case 's':
		camera->moveBackward( camAccel );
		camera->setView();
		break;
	case 'i':
		camera->moveUp( camAccel );
		camera->setView();
		break;
	case 'k':
		camera->moveDown( camAccel );
		camera->setView();
		break;
	case 'g':
		camera->moveAround(deltaRot,XAxis);
		camera->setView();
		break;
	case 'b':
		camera->moveAround(-1.0*deltaRot,XAxis);
		camera->setView();
		break;
	case 'H':
		hideCharacters = !hideCharacters;
		break;
	case ' ':
		animating = !animating;
		break;
	case 'o':
		obstacle_manager->toggleObstacle();
		break;
	case 'e':
		obstacle_manager->toggleExit();
		break;
	case 'r':
		wireframe = !wireframe;
		break;
	case 'R':
		drawShadows = !drawShadows;
		break;
	case 'm':
		drawShadows				= !drawShadows;
		animating				= !animating;
		doHeightAndDisplacement = !doHeightAndDisplacement;
		doColor					= !doColor;
		doPatterns				= !doPatterns;
		doFacial				= !doFacial;
		runpaths				= !runpaths;

		/*
		predef_cam_index = predef_cam_pos.size() - 1;
		camPos = vec3(	predef_cam_pos[predef_cam_index].x,
                        predef_cam_pos[predef_cam_index].y,
                        predef_cam_pos[predef_cam_index].z );
		camDir = vec3(  predef_cam_dir[predef_cam_index].x,
                        predef_cam_dir[predef_cam_index].y,
                        predef_cam_dir[predef_cam_index].z );
		camUp  = vec3(  predef_cam_up[predef_cam_index].x,
                        predef_cam_up[predef_cam_index].y,
                        predef_cam_up[predef_cam_index].z );
		camera->setPosition( camPos );
		camera->setDirection( camDir );
		camera->setUpVec( camUp );
		camera->setView();
		*/
		break;
	case '.':
		policy_floor = !policy_floor;
		glsl_manager->activate( "mdp_floor" );
		{
			if( policy_floor )
			{
				glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_on.c_str(), 1.0f );
			}
			else
			{
				glsl_manager->setUniformf( "mdp_floor", (char*)str_policy_on.c_str(), 0.0f );
			}
		}
		glsl_manager->deactivate( "mdp_floor" );
		break;
	case ',':
		density_floor = !density_floor;
		glsl_manager->activate( "mdp_floor" );
		{
			if( density_floor )
			{
				glsl_manager->setUniformf( "mdp_floor", (char*)str_density_on.c_str(), 1.0f );
			}
			else
			{
				glsl_manager->setUniformf( "mdp_floor", (char*)str_density_on.c_str(), 0.0f );
			}
		}
		glsl_manager->deactivate( "mdp_floor" );
		break;
	case 'S':
		showstats = !showstats;
		break;
	}
}
//
//=======================================================================================
//
void keyboard_master ( unsigned char key, int x, int y)
{
	int pdi     = 0;
	vec3 camPos = vec3( 0.0f );
	vec3 camDir = vec3( 0.0f );
	vec3 camUp  = vec3( 0.0f );
	switch( key )
	{
	case 43:	//'+'
		predef_cam_index = (predef_cam_index + 1) % predef_cam_pos.size();
		camPos = vec3(	predef_cam_pos[predef_cam_index].x,
                        predef_cam_pos[predef_cam_index].y,
                        predef_cam_pos[predef_cam_index].z );
		camDir = vec3(  predef_cam_dir[predef_cam_index].x,
                        predef_cam_dir[predef_cam_index].y,
                        predef_cam_dir[predef_cam_index].z );
		camUp  = vec3(  predef_cam_up[predef_cam_index].x,
                        predef_cam_up[predef_cam_index].y,
                        predef_cam_up[predef_cam_index].z );
        camera->setPosition( camPos );
        camera->setDirection( camDir );
        camera->setUpVec( camUp );
        camera->setView();


		break;
	case 45:	//'-'
		pdi = (int)predef_cam_index;
		if( (pdi-1) >= 0  )
		{
			predef_cam_index = predef_cam_index - 1;
		}
		else
		{
			predef_cam_index = predef_cam_pos.size() - 1;
		}
		camPos = vec3(	predef_cam_pos[predef_cam_index].x,
                        predef_cam_pos[predef_cam_index].y,
                        predef_cam_pos[predef_cam_index].z );
		camDir = vec3(  predef_cam_dir[predef_cam_index].x,
                        predef_cam_dir[predef_cam_index].y,
                        predef_cam_dir[predef_cam_index].z );
		camUp  = vec3(  predef_cam_up[predef_cam_index].x,
                        predef_cam_up[predef_cam_index].y,
                        predef_cam_up[predef_cam_index].z );
        camera->setPosition( camPos );
        camera->setDirection( camDir );
        camera->setUpVec( camUp );
        camera->setView();
		break;
	case 19:	//'CTRL+s'
		camera->moveDown( camAccel );
		camera->setView();
		break;
	case 23:	//'CTRL+w'
		camera->moveUp( camAccel );
		camera->setView();
		break;
	case 27:
#ifdef MPI_PATHS_ON_NODES
		shutDown = true;
#endif
		exit( 0 );
		break;
	case 'a':
		camera->moveLeft( camAccel );
		camera->setView();
		break;
	case 'c':
		if( camera == camera1 )
		{
			camera = camera2;
		}
		else
		{
			camera = camera1;
		}
		camNear = camera->getFrustum()->getNearD();
		break;
	case 'd':
		camera->moveRight( camAccel );
		camera->setView();
		break;
	case 'w':
		camera->moveForward( camAccel );
		camera->setView();
		break;
	case 's':
		camera->moveBackward( camAccel );
		camera->setView();
		break;
	case 'i':
		camera->moveUp( camAccel );
		camera->setView();
		break;
	case 'k':
		camera->moveDown( camAccel );
		camera->setView();
		break;
	case 'g':
		camera->moveAround(deltaRot,XAxis);
		camera->setView();
		break;
	case 'b':
		camera->moveAround(-1.0*deltaRot,XAxis);
		camera->setView();
		break;
	case 'S':
		showstats = !showstats;
		break;
	default:
		return;
	}

#if defined MPI_PATHS_ON_NODES
	keyChangedFlag = true;
	MPI_key = key;
#endif
}
//
//=======================================================================================
//
void special( int key, int x, int y )
{
	//printf( "SKEY=%d\n", key );
	glut_mod = glutGetModifiers();
	switch( key )
	{
		case GLUT_KEY_LEFT:
			if( glut_mod == GLUT_ACTIVE_CTRL )
			{
				obstacle_manager->moveCursorLeft();
			}
			else
			{
				camera->moveLeft( camAccel * 20.0f );
			}
			break;
		case GLUT_KEY_RIGHT:
			if( glut_mod == GLUT_ACTIVE_CTRL )
			{
				obstacle_manager->moveCursorRight();
			}
			else
			{
				camera->moveRight( camAccel * 20.0f );
			}
			break;
		case GLUT_KEY_UP:
			if( glut_mod == GLUT_ACTIVE_CTRL )
			{
				obstacle_manager->moveCursorUp();
			}
			else
			{
				camera->moveForward( camAccel * 20.0f );
			}
			break;
		case GLUT_KEY_DOWN:
			if( glut_mod == GLUT_ACTIVE_CTRL )
			{
				obstacle_manager->moveCursorDown();
			}
			else
			{
				camera->moveBackward( camAccel * 20.0f );
			}
			break;
	}
}
//
//=======================================================================================
//
void special_master ( int key, int x, int y )
{
	//printf( "SKEY=%d\n", key );
	glut_mod = glutGetModifiers();
	switch( key )
	{
		case GLUT_KEY_LEFT:
			camera->moveLeft( camAccel * 20.0f );
			break;
		case GLUT_KEY_RIGHT:
			camera->moveRight( camAccel * 20.0f );
			break;
		case GLUT_KEY_UP:
			camera->moveForward( camAccel * 20.0f );
			break;
		case GLUT_KEY_DOWN:
			camera->moveBackward( camAccel * 20.0f );
			break;
	}

#if defined MPI_PATHS_ON_NODES
	// TODO: check Process Master and Broadcast KEYS
	// MPI_Bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
#endif

}
//
//=======================================================================================
//
void mouse( int button, int state, int x, int y )
{
	if( state == GLUT_DOWN )
	{
		if( button == GLUT_LEFT_BUTTON )
		{
			lMouseDown = true;
			rMouseDown = false;
			mMouseDown = false;
		}
		else if( button == GLUT_RIGHT_BUTTON )
		{
			lMouseDown = false;
			rMouseDown = true;
			mMouseDown = false;
		}
		else if( button ==  GLUT_MIDDLE_BUTTON  )
		{
			lMouseDown = false;
			rMouseDown = false;
			mMouseDown = true;
		}
	}
}
//
//=======================================================================================
//
void motion( int x, int y )
{
    vec3 up( 0.0f, 1.0f, 0.0f );
    vec3 ri( 1.0f, 0.0f, 0.0f );

	int modifiers = glutGetModifiers();
	if( modifiers & GLUT_ACTIVE_CTRL )
	{
		//printf( "GLUT_ACTIVE_CTRL\n" );
		ctrlDown = true;
	}
	else
	{
		ctrlDown = false;
	}

	static int xlast = -1, ylast = -1;
	int dx, dy;

	dx = x - xlast;
	dy = y - ylast;
	if( lMouseDown )
	{
		if( ctrlDown == false )
		{
			if( dx > 0 )
			{
				camera->rotateAngle(  camAccel/4.0f, up );
			}
			else if( dx < 0 )
			{
				camera->rotateAngle( -camAccel/4.0f, up );
			}
		}
		else
		{
			if( dy > 0 )
			{
				camera->rotateAngle( -camAccel/4.0f, ri );
			}
			else if( dy < 0 )
			{
				camera->rotateAngle(  camAccel/4.0f, ri );
			}
		}
	}
	else if( rMouseDown )
	{
		if( dy > 0 )
		{
			camera->rotateAngle( -camAccel/4.0f, ri );
		}
		else if( dy < 0 )
		{
			camera->rotateAngle(  camAccel/4.0f, ri );
		}
	}
	xlast = x;
	ylast = y;

}
//
//=======================================================================================
//

//float camAngAnt = 0.0;

//bool fst = true;
float DistCam;
float CanvasSizeMts = 0.0;

void cameraChanged(){
	float data[6];
	float ModViewMatbuff[16];
	float aux1,aux2,aux3;
	float scalfac  = 100.0;


	//pthread_mutex_lock(&mutexObj);
	vec3 AngleAxis(1.0,0.0,0.0);
	vec3 CamDir (0.0, -1.0, 0.0 );
	vec3 CamUp	( 0.0f, 0.0f, 1.0f );


	data[0] = *((float *)MPI_interaction+_dist2ground);
	data[1] = *((float *)MPI_interaction+_rectX);
	data[2] = *((float *)MPI_interaction+_rectZ);
	data[3] = *((float *)MPI_interaction+_camAngl);
	data[4] = *((float *)MPI_interaction+_scalCam);
	data[5] = *((float *)MPI_interaction+_camZoom);

	/*
	for(unsigned int c=0;c<16;++c)
		*(ModViewMatbuff+c) = *((float *)MPI_interaction+_ModelViewMat+c);
	*/

	/*for(unsigned int c=0;c<6;++c)
		cout<<"camera PID "<<pid<<" data:"<<c<<" "<<data[c]<<endl;
	*/

	//aux1 = -1.0*data[1]*data[4];
	aux1 = -1.0*data[1]*scalfac;

	//aux2 = data[2]*data[4];
	aux2 = data[2]*scalfac;
	aux3 = data[0];

	if(!(CanvasSizeMts!=0.0) || CanvasSizeMts != aux3){
		CanvasSizeMts = aux3;
		DistCam = (CanvasSizeMts / tan(45.0*0.5))*scalfac;
		//glsl_manager->setUniformf("depth_rect","z_Far",(DistCam/scalfac));
<<<<<<< HEAD
		glsl_manager->vZFar 	= DistCam;
=======
		glsl_manager->vZFar 	= DistCam*100.0;
>>>>>>> 30e048104403fa5ef7bed9d37f608936d5c07194
		glsl_manager->vZNear	= 100.0;

		/*
		glsl_manager->updateUniformf("instancing_culled_rigged","zNear",DistCam*100.0);
		glsl_manager->updateUniformf("instancing_culled_rigged","zFar",);


		glsl_manager->updateUniformf("instancing_culled_rigged_shadow","zNear",DistCam*100.0);
		glsl_manager->updateUniformf("instancing_culled_rigged_shadow","zFar",camera->getFrustum()->getFarD());
		*/
		//cout<<"Canvas Size Mts "<<CanvasSizeMts<<" DistCam "<<DistCam<<endl;
	}

	//DistCam = 50.0;

	//cout<<"DistCam "<<DistCam<<endl;
	//DistCam*=100.0;


	/**********************************/
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glLoadMatrixf(ModViewMatbuff);
	//gluPerspective(45, 1.0,1.0, CanvasSizeMts);

	//cout<<" *************************camera setPosition pid "<<pid<<"*****************************"<<endl;
	vec3 CamPos(aux1,DistCam,aux2);
	//cout<<"Canvas Size Mts "<<CanvasSizeMts<<" DistCam "<<DistCam<<" x:"<<aux1<<" y:"<<DistCam<<" z:"<<aux2<<endl;
	camera->setPosition(CamPos);
	//cout<<" camera position setted"<<endl;
	/**************************************/


	//if(pid==2)
		//cout<<" dist to ground: "<<DistCam<<endl;

	//aux3 = cos(data[3]*M_PI/180.0)*data[0];

/*
	if(fst){
		camAngAnt = data[3];
		fst = false;
	}
*/

	/****************************/
	vec3 Pivot(aux1,0.0,aux2);
	camera->setPivot(Pivot);


	camera->setDirection(CamDir);
	camera->setUpVec(CamUp);

	camera->moveAround(-1.0f*(data[3]),AngleAxis);

	camera->setView();

	/***************************/

	//rotAng = data[3]-camAngAnt;


	//camera->moveAround(-1.0f*(data[3]),AngleAxis);


	//camera->setViewModelView(ModViewMatbuff);

	//if(pid==2)
		//cout<<" dist to ground: "<<data[0]<<" rectX: "<<data[1]<<" rectZ: "<<data[2]<<" desp cm: "<<aux1<<" , "<<aux2<<
			//" camera angle: "<<data[3]<<" dist2ground camera: "<<(data[0])<<" dist2Ground Proy: "<<DistCam<<endl;

	//cout<<" rectX: "<<data[1]<<" rectZ: "<<data[2]<<endl;

/*
	if(pid==2)
		cout<<" modelviewMatrix: "<<ModViewMatbuff[0]<<" , "<<ModViewMatbuff[1]<<" , "<<ModViewMatbuff[2]<<" , "
		<<ModViewMatbuff[3]<<" , "<<ModViewMatbuff[4]<<" , "<<ModViewMatbuff[5]<<" , "<<ModViewMatbuff[6]<<" , "
		<<ModViewMatbuff[7]<<" , "<<ModViewMatbuff[8]<<" , "<<ModViewMatbuff[9]<<" , "<<ModViewMatbuff[10]<<" , "
		<<ModViewMatbuff[11]<<" , "<<ModViewMatbuff[12]<<" , "<<ModViewMatbuff[13]<<" , "<<ModViewMatbuff[14]<<" , "
		<<ModViewMatbuff[15]<<endl;

	//camAngAnt = data[3];
	 *
	 */
	//pthread_mutex_unlock(&mutexObj);
}
//
//=======================================================================================
//
#if defined MPI_PATHS_ON_NODES
void checkMasterUserInput (void)
{
	if(!webScktsFlg){
		int	mpiErr = -1;
			mpiErr = MPI_Bcast ( &keyChangedFlag, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
		//	MPI_Barrier(MPI_COMM_WORLD); // MPI_Bcast requires sync.

			mpiErr = MPI_Bcast ( &MPI_key, 1, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
		//	MPI_Barrier(MPI_COMM_WORLD); // MPI_Bcast requires sync.

			MPI_Bcast( &shutDown, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
		//	MPI_Barrier(MPI_COMM_WORLD);

			if (shutDown)
			{
				printf ("\n\nThis is process %d, Number of processes %d\n",pid, np);
				MPI_Finalize();
				glutLeaveMainLoop();
			}

			if (pid==0 && keyChangedFlag) //master
			{
				keyChangedFlag = false;
			}
			else if ( pid!=0 && keyChangedFlag)
			{
				keyboard (MPI_key, 0,0);
			}
	}
	else{

		//cout<<"**********--------PID "<<pid<<" checkMasterUserInput MPI_Bcast-------------*********"<<endl;
		MPI_Bcast ( MPI_interaction,_flagExit+1, MPI_INT, 	0, MPI_COMM_WORLD );

		//cout << "MPI_interaction[_mouseX] " << MPI_interaction[_mouseX] << endl;
		//cout<<"**********--------PID "<<pid<<" checkMasterUserInput MPI_interaction-------------*********"<<endl;
		if (MPI_interaction[_flagExit])
		{
			MPI_Barrier(MPI_COMM_WORLD);
			cout << "Ending process " << pid << " of " << np << endl;
			MPI_Finalize();
			if (pid==0)
				glutLeaveMainLoop();
			else
			{
				running = false;
			}
		}

		if (pid == 0 )
		{
			//cout<<"**********--------PID "<<pid<<" checkMasterUserInput _keyChangedFlag-------------*********"<<endl;
			if (MPI_interaction[_keyChangedFlag])
			{
				MPI_interaction[_keyChangedFlag] = false;
			}
			//cout<<"**********--------PID "<<pid<<" checkMasterUserInput _mouseChangedFlag-------------*********"<<endl;

			if (MPI_interaction[_mouseChangedFlag])
			{
				MPI_interaction[_mouseChangedFlag] = false;
			}

			if(MPI_interaction[_cameraChangedFlag]){
				MPI_interaction[_cameraChangedFlag] = false;
			}
		}
		else
		{
			//cout<<"**********--------PID "<<pid<<" checkMasterUserInput _keyChangedFlag-------------*********"<<endl;
			if (MPI_interaction[_keyChangedFlag])
			{
				keyboard (MPI_interaction[_key], MPI_interaction[_mouseX], MPI_interaction[_mouseY]);
			}
			//cout<<"**********--------PID "<<pid<<" checkMasterUserInput _mouseChangedFlag-------------*********"<<endl;
			if (MPI_interaction[_mouseChangedFlag])
			{
				motion (MPI_interaction[_mouseX], MPI_interaction[_mouseY]);
			}
			if(MPI_interaction[_cameraChangedFlag]){
				//cout<<"**********--------PID "<<pid<<" checkMasterUserInput _cameraChangedFlag " <<MPI_interaction[_cameraChangedFlag]<<"-------------*********"<<endl;
				cameraChanged();
				//cout<<"**********--------PID "<<pid<<" checkMasterUserInput  cameraChanged END-------------*********"<<endl;
			}
		}
	}


}
#endif
//
//=======================================================================================
//
// This method is used to respond to  interaction events read in the browser
// Results are sent to the nodes
//
void eventsFromBrowser ( )
{
    vec3 up( 0.0f, 1.0f, 0.0f );
    vec3 ri( 1.0f, 0.0f, 0.0f );

	static int xlast = -1, ylast = -1;
	int dx, dy;

	//cout<<"**********--------PID "<<pid<<" idle eventsFromBrowser check mouseHandler->refreshed()-------------*********"<<endl;
	if ( mouseHandler->refreshed() )
	{
		MPI_interaction[_mouseX] 		= mouseHandler->getX();
		MPI_interaction[_mouseY] 		= mouseHandler->getY();

		if (mouseHandler->getState() == cMouseHandler::DOWN)
		{
			switch (mouseHandler->getButton())
			{
			case 0:
				MPI_interaction[_lMouseDown] = true;
				MPI_interaction[_rMouseDown] = false;
				MPI_interaction[_mMouseDown] = false;
				break;
			case 1:
				MPI_interaction[_lMouseDown] = false;
				MPI_interaction[_rMouseDown] = false;
				MPI_interaction[_mMouseDown] = true;
				break;
			case 2:
				MPI_interaction[_lMouseDown] = false;
				MPI_interaction[_rMouseDown] = true;
				MPI_interaction[_mMouseDown] = false;
				break;
			}
		}
		else
		{
			MPI_interaction[_lMouseDown] = false;
			MPI_interaction[_rMouseDown] = false;
			MPI_interaction[_mMouseDown] = false;
		}
		MPI_interaction[_mouseChangedFlag] 	= true;
		mouseHandler->refresh(false);
	}

	//cout<<"**********--------PID "<<pid<<" idle eventsFromBrowser check keyboardHandler->refreshed()-------------*********"<<endl;
	if ( keyboardHandler->refreshed())
	{
		if (keyboardHandler->getState() == cKeyboardHandler::DOWN)
		{
			MPI_interaction[_key] 				= keyboardHandler->getKey();
		}
		else
		{
			MPI_interaction[_key] 				= -1;
		}
		MPI_interaction[_keyChangedFlag] 	= true;

		keyboardHandler->refresh (false);
	}
	//cout<<"**********--------PID "<<pid<<" idle eventsFromBrowser check frmClntHndlr->getRefreshFlg()-------------*********"<<endl;
	pthread_mutex_lock (&mutexObj);
	if(frmClntHndlr->getRefreshFlg()){

		wCamInfo camInf = frmClntHndlr->getCoordInfo();
		*(MPI_interaction+_dist2ground)=*(int*)(&(camInf.f_dist2Grnd));
		*(MPI_interaction+_rectX)=*(int*)(&(camInf.f_rectX));
		*(MPI_interaction+_rectZ)=*(int*)(&(camInf.f_rectZ));
		*(MPI_interaction+_camAngl)=*(int*)(&(camInf.f_RotAngCam));
		*(MPI_interaction+_scalCam)=*(int*)(&(camInf.f_scal_mod));
		*(MPI_interaction+_camZoom)=*(int*)(&(camInf.f_Zoom));

		/*
		for(unsigned int c=0;c<16;++c)
			*(MPI_interaction+_ModelViewMat+c)=*(int*)(&camInf.fp_modelViewMat[c]);
		*/
		MPI_interaction[_cameraChangedFlag] = true;
		frmClntHndlr->setRefresh(false);

	}
	pthread_mutex_unlock (&mutexObj);
	//cout<<"**********--------PID "<<pid<<" idle eventsFromBrowser end-------------*********"<<endl;

}
//
//=======================================================================================
//
