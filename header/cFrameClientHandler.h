//
//										Jorge E. Ramirez Flores, PhD. 	jramfl@gmail.com
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================
#pragma once
#ifndef CFRAMECLIENTHANDLER_H_
#define CFRAMECLIENTHANDLER_H_



#endif /* CFRAMECLIENTHANDLER_H_ */


#define CLIENT_FRAME_EVENT	7		///This codes are made up according to our needs,

struct wCamInfo{
	float	f_Zoom;
	float	f_Init_Lat;
	float	f_Init_Long;
	float	f_Init_lat_rect;
	float	f_Init_long_rect;
	float	f_Lat;
	float	f_Long;
	float	f_RotAngCam;
	float   f_rectX;
	float	f_rectZ;
	float	f_scal_mod;
	float	f_offset;
	float	f_dist2Grnd;
	float	*fp_modelViewMat;
};

class cFrameClientHandler{

private:
	char * pc_frameBuffer;
	char * pc_depthBuffer;
	float * pf_frameBuffer;
	float * pf_frameBuffNULL;
	float * pf_depthBuffer;
	char * pf_coordBuffer;
	unsigned int ui_width;
	unsigned int ui_height;
	unsigned int ui_BffrSz;
	unsigned int ui_BffrSzBytes;
	unsigned int ui_CoordSz;
	bool	b_cmbndSrcImg;
	bool 	b_intDtCrds;
	wCamInfo s_cmraInf;
	std::mutex c_frmLock;

	bool	b_refresh;

public:

	cFrameClientHandler(){
		initData();
		}

	cFrameClientHandler(const unsigned int width,const unsigned int height){initData();setFramesSizes(width,height);}


	~cFrameClientHandler(){
		deleteData();
	}



	char * getFrameBuffer(){return pc_frameBuffer;}
	char * getDepthBuffer(){return pc_depthBuffer;}

	float * getFrameBufferMyScTyp(){return pf_frameBuffer;}
	float * getDepthBufferMyScTyp(){return pf_depthBuffer;}
	float * getFrameBufferNULL(){return pf_frameBuffNULL;}

	unsigned int getWidth(){return ui_width;}
	unsigned int getHeight(){return ui_height;}

	void printInfo(){
		std::cout<<" UnNormalized data: "
				<<(unsigned int)((unsigned char)pc_frameBuffer[263656])<<","
				<<(unsigned int)((unsigned char)pc_frameBuffer[263657])<<","
				<<(unsigned int)((unsigned char)pc_frameBuffer[263658])<<","
				<<(unsigned int)((unsigned char)pc_frameBuffer[263659])<<","
				<<(unsigned int)((unsigned char)pc_frameBuffer[263660])<<","<<std::endl;

				std::cout<<" Pointer: "<<pf_frameBuffer<<std::endl;
				std::cout<<" Normalized data: "
				<<pf_frameBuffer[263656]<<","
				<<pf_frameBuffer[263657]<<","
				<<pf_frameBuffer[263658]<<","
				<<pf_frameBuffer[263659]<<","
				<<pf_frameBuffer[263660]<<","<<std::endl;

				std::cout<<" Normalized data depth buffer: "
								<<pf_depthBuffer[0]<<std::endl;
	}

	bool processDataBuffer(std::stringstream * val){

		unsigned int aux = val->str().length();
		unsigned int tLenght = 1 + ui_CoordSz + ui_BffrSzBytes;

		if(aux==0 || (aux!=(tLenght-ui_CoordSz) && aux!=tLenght) ){
			//std::cout<<" aux "<<aux<<" tLenght "<<tLenght<<" tLenghtWoData "<<tLenght-ui_CoordSz<<std::endl;
			return false;
		}

		val->str().copy(pc_frameBuffer,ui_BffrSzBytes,0);
		pc_frameBuffer[0] = val->str().data()[ui_BffrSzBytes];

		if(aux==tLenght){
			//std::cout<<" aux "<<aux<<" tLenght "<<tLenght<<" tLenghtWoData "<<tLenght-ui_CoordSz<<std::endl;
			val->str().copy(pf_coordBuffer,ui_CoordSz,1+ui_BffrSzBytes);
			setCoordVal(pf_coordBuffer,b_intDtCrds);
			setRefresh(true);
		}

		if(b_intDtCrds)
			b_intDtCrds = false;

		normalizeData();
		return true;
	}

	void normalizeData(){
		unsigned int x,conta=3;
		if(pf_frameBuffer==nullptr||pf_depthBuffer==nullptr||pc_frameBuffer==nullptr||pc_depthBuffer==nullptr)
			return;

		for(x=0;x<ui_BffrSzBytes;++x){
			pf_frameBuffer[x]=(float)((unsigned int)((unsigned char)pc_frameBuffer[x]))/255.0;
			pf_frameBuffNULL[x]=1.0;
		}

		if(!b_cmbndSrcImg){
			for(x=0;x<ui_BffrSz;++x){
				pf_depthBuffer[x]=(float)((unsigned int)((unsigned char)pc_depthBuffer[x]))/255.0;

				/////////////////Combining both buffers in one /////////////////
				pf_frameBuffer[conta]=pf_depthBuffer[x];
				conta+=4;
				/////////////////Combining both buffers in one /////////////////
			}
		}

		//printInfo();
	}

	void initData(){
		pc_frameBuffer = pc_depthBuffer = nullptr;
		pf_frameBuffNULL = pf_frameBuffer = pf_depthBuffer = nullptr;
		ui_CoordSz=ui_width=ui_height=ui_BffrSz=ui_BffrSzBytes=0;
		s_cmraInf.f_scal_mod = s_cmraInf.f_offset = 0.0;
		b_refresh = false;
		s_cmraInf.fp_modelViewMat = new float[16];
	}

	void setFramesSizes(const unsigned int width,const unsigned int height){
		deleteData();
		ui_width = width;
		ui_height = height;
		ui_BffrSz = width*height;
		ui_BffrSzBytes = ui_BffrSz*4;
		pc_frameBuffer = new char[ui_BffrSzBytes];
		pc_depthBuffer = new char [ui_BffrSz];
		pf_frameBuffer = new float[ui_BffrSzBytes];
		pf_depthBuffer = new float[ui_BffrSz];
		pf_frameBuffNULL = new float[ui_BffrSzBytes];
		b_cmbndSrcImg  = true;
		ui_CoordSz	   = 92;	// four bytes per float, four floats: zoom,lat,long and rotAng
		pf_coordBuffer = new char[ui_CoordSz];
		b_intDtCrds    = true;
	};

	void deleteData(){
		//std::cout<<"Destructor frmclinthndlr: "<<this<<std::endl;
		try{
			if(pc_frameBuffer!=nullptr)
				delete pc_frameBuffer;
			if(pf_frameBuffer!=nullptr)
				delete pc_frameBuffer;

			if(pf_frameBuffNULL != nullptr)
				delete pf_frameBuffNULL;
			if(pc_depthBuffer != nullptr)
				delete pc_depthBuffer;
			if(pf_depthBuffer != nullptr)
				delete pc_depthBuffer;
			if(s_cmraInf.fp_modelViewMat != nullptr)
				delete s_cmraInf.fp_modelViewMat;
			}
		catch(...){}
		ui_width=ui_height=ui_BffrSz=ui_BffrSzBytes=0;
	}

	void setCmbndSrcImg(bool flg){b_cmbndSrcImg = flg;}

	wCamInfo getCoordInfo(){return s_cmraInf;}

	void setCoordVal(char* src,bool mode){ ///true if initial values are stored false otherwise
		unsigned int aux,aux2,aux3;
		unsigned char* swap;

		float resf;

		swap = (unsigned char*)&resf;
		aux3=0;


		for(aux=0;aux<23;++aux){
			for(aux2=0;aux2<4;++aux2,++aux3)
				swap[aux2] = (unsigned char)src[aux3];

			switch(aux){
			case 0:
				s_cmraInf.f_Zoom = resf;
				break;
			case 1:
				s_cmraInf.f_Lat = resf;
				if(mode)
					s_cmraInf.f_Init_Lat = resf;
				break;
			case 2:
				s_cmraInf.f_Long = resf;
				if(mode)
					s_cmraInf.f_Init_Long =  resf;
				break;
			case 3:
				s_cmraInf.f_RotAngCam = resf;
				break;
			case 4:
				s_cmraInf.f_rectX = resf;
				if(mode)
					s_cmraInf.f_Init_Long = resf;
				break;
			case 5:
				s_cmraInf.f_rectZ = resf;
				if(mode)
					s_cmraInf.f_Init_Lat = resf;
				break;
			case 6:
				s_cmraInf.f_dist2Grnd = resf;
				break;
			default:
				s_cmraInf.fp_modelViewMat[aux - 7] = resf;
			}
		}


		/*-------------------//////////////////////DEBUG///////////////////--------------------*
		for(aux=0;aux<ui_CoordSz;++aux)
			std::cout<<"CoordBuff: "<<aux<<" "<<(int)((unsigned char)src[aux])<<" ";
		std::cout<<std::endl;
		*


		std::cout<<"Frame Tangram Coordinates; Zoom: "
						<<s_cmraInf.f_Zoom<<", Latitude: "<<s_cmraInf.f_Lat<<", Initial Latitude: "<<s_cmraInf.f_Init_Lat
						<<", Longitude: "<<s_cmraInf.f_Long<<", Initial Longitude: "<<s_cmraInf.f_Init_Long
						<<", Camera Rotation Angle"<<s_cmraInf.f_RotAngCam<<","<<" rectX:"<<s_cmraInf.f_rectX
						<<" rectz:"<<s_cmraInf.f_rectZ<<std::endl;

		std::cout<<" modelviewMatrix: "<<s_cmraInf.fp_modelViewMat[0]<<" , "<<s_cmraInf.fp_modelViewMat[1]<<" , "<<s_cmraInf.fp_modelViewMat[2]<<" , "
		<<s_cmraInf.fp_modelViewMat[3]<<" , "<<s_cmraInf.fp_modelViewMat[4]<<" , "<<s_cmraInf.fp_modelViewMat[5]<<" , "<<s_cmraInf.fp_modelViewMat[6]<<" , "
		<<s_cmraInf.fp_modelViewMat[7]<<" , "<<s_cmraInf.fp_modelViewMat[8]<<" , "<<s_cmraInf.fp_modelViewMat[9]<<" , "<<s_cmraInf.fp_modelViewMat[10]<<" , "
		<<s_cmraInf.fp_modelViewMat[11]<<" , "<<s_cmraInf.fp_modelViewMat[12]<<" , "<<s_cmraInf.fp_modelViewMat[13]<<" , "<<s_cmraInf.fp_modelViewMat[14]<<" , "
		<<s_cmraInf.fp_modelViewMat[15]<<std::endl;

		/*-------------------//////////////////////DEBUG///////////////////--------------------*/
	}

	void compute_scalMod(float minY,float maxY,float heigth){	////computes a scale factor from a defined height in meters
		float mHeigth = maxY-minY;

		s_cmraInf.f_scal_mod = mHeigth / heigth;
		s_cmraInf.f_offset = -minY * s_cmraInf.f_scal_mod;

		//std::cout<<" minY:"<<minY<<" maxY:"<<maxY<<" height:"<<heigth<<" scalFactor:"<<s_cmraInf.f_scal_mod<<std::endl;
	}

	void	setRefresh(bool state){b_refresh=state;}
	bool	getRefreshFlg(){return b_refresh;}
};


