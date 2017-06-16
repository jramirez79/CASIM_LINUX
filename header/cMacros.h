// @REV_TAG SRL_09_2013
//
//																		 VBO MANAGER v1.0
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

#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#define DEMO_SHADER

//#define DEMO_ALONE
#define DEMO_TEASER
//#define DEMO_LEMMINGS

#define CROWD_OK								0
#define CROWD_ERROR								1
#define CROWD_FBO_ERROR							2
#define CROWD_SHADER_ERROR						3

//#define MPI_PATHS
#define MPI_PATHS_ON_NODES
//#define CUDA_PATHS
//#define OSC_PATHS
//#define MDP_SQUARE
#define MDP_HEXAGON
//#define CASIM_CUDA_PATH_DEBUG


#ifndef NUM_LOD
	#define NUM_LOD 3
#endif

#ifndef __LOD_TYPES
#define __LOD_TYPES 1
	enum LOD_TYPE { LOD_HI, LOD_ME, LOD_LO };
#endif

#define GLOBAL_POS_TEXTURE	1
//#define LOCAL_POS_TEXTURE	1

//=======================================================================================

/* This will expose:
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */
#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES	1
#endif

#include <math.h>

//=======================================================================================

#ifndef DEG2RAD
	#define DEG2RAD	0.01745329251994329576f
#endif

//=======================================================================================

#ifndef RAD2DEG
	#define RAD2DEG	57.29577951308232087679f
#endif

//=======================================================================================

// For cScreenText.cpp
#ifndef SCREEN_TEXT_BUFFER_SIZE
	#define SCREEN_TEXT_BUFFER_SIZE	1024
#endif

//=======================================================================================

#ifndef FREE_TEXTURE
	#define FREE_TEXTURE( ptr )				\
	if( ptr )								\
	{										\
		glDeleteTextures( 1, &ptr );		\
		ptr = 0;							\
	}
#endif

//=======================================================================================

#ifndef FREE_INSTANCE
	#define FREE_INSTANCE( ptr )			\
	if( ptr )								\
	{										\
		delete ptr;							\
		ptr = 0;							\
	}
#endif

//=======================================================================================

#ifndef BYTE2KB
	#define BYTE2KB( b ) b / 1024
#endif

//=======================================================================================

#ifndef BYTE2MB
	#define BYTE2MB( b ) b / 1048576
#endif

//=======================================================================================

#ifndef BYTE2GB
	#define BYTE2GB( b ) b / 1073741824
#endif

//=======================================================================================


// For cXmlParser.cpp:
#ifndef TIXML_USE_STL
	#define TIXML_USE_STL 1
#endif

#define NUM_INDENTS_PER_SPACE 2;

//=======================================================================================

// For cGlslManager.cpp:
#ifndef SHADER_OBJECT_MAX_VARIABLES
    #define SHADER_OBJECT_MAX_VARIABLES 64
#endif

#ifndef USE_GEOMETRY_SHADERS
    #define USE_GEOMETRY_SHADERS		1
#endif

#ifndef USE_GI_MODELS
    #define USE_GI_MODELS				0
#endif

#ifndef USE_INSTANCING
    #define USE_INSTANCING				1
#endif

#ifndef STRING_UTILS_BUFFER_SIZE
    #define STRING_UTILS_BUFFER_SIZE	65536
#endif

//=======================================================================================

#ifndef __XYZ
#define __XYZ 1
typedef struct {
   double x,y,z;
} XYZ;
#endif

#ifndef __PIXELA
#define __PIXELA 1
typedef struct {
   unsigned char r,g,b,a;
} PIXELA;
#endif

#ifndef __COLOUR
#define __COLOUR 1
typedef struct {
   double r,g,b;
} COLOUR;
#endif

#define CROSSPROD( p1, p2, p3 ) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

#ifndef __NORMALIZE
#define __NORMALIZE 1
#define NORMALIZE(p,length) \
   length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z); \
   if( length != 0 ) { \
      p.x /= length; \
      p.y /= length; \
      p.z /= length; \
   } else { \
      p.x = 0; \
      p.y = 0; \
      p.z = 0; \
   }
#endif

#ifndef LOD_STRUCT
#define LOD_STRUCT

struct sVBOLod
{
	unsigned int id;
	unsigned int primitivesWritten;
	unsigned int primitivesGenerated;
};

#endif

#ifndef FREE_MEMORY
#define FREE_MEMORY(ptr)	\
    if (ptr) {				\
		delete ptr;			\
        ptr=0;			\
    }
#endif

#ifndef FREE_ARRAY
#define FREE_ARRAY(ptr)	\
    if (ptr) {				\
		delete [] ptr;			\
        ptr=0;			\
    }
#endif

#ifndef FREE_VBO
#define FREE_VBO(ptr) \
	if (ptr) { \
		glDeleteBuffers(1,&ptr); \
		ptr = 0; \
	}
#endif

#ifndef FREE_TEXTURE
#define FREE_TEXTURE(ptr) \
	if (ptr) { \
		glDeleteTextures(1,&ptr); \
		ptr = 0; \
	}
#endif

#ifndef FREE_OGL_LIST
#define FREE_OGL_LIST(ptr) \
	if (ptr) { \
		glDeleteLists(ptr,1); \
		ptr = 0; \
	}
#endif

//=======================================================================================

// For cVboManager.cpp:
#ifndef MAX_INSTANCES
	#define MAX_INSTANCES 20000
#endif

//=======================================================================================

// For cCharacterModel.cpp:
#ifndef PARTS_PER_MODEL
	#define PARTS_PER_MODEL	3
	enum MODEL_PART
	{
		MP_HEAD,
		MP_TORSO,
		MP_LEGS
	};
#endif

#ifndef GENDERS_PER_MODEL
	#define GENDERS_PER_MODEL 2
	enum MODEL_GENDER
	{
		MG_MALE,
		MG_FEMALE
	};
#endif

#ifndef TYPES_PER_MODEL
	#define TYPES_PER_MODEL 2
    enum MODEL_TYPE
	{
		MT_HUMAN,
		MT_LEMMING
	};
#endif

//=======================================================================================

#ifndef MODEL_PROPS_TYPES
	#define MODEL_PROPS_TYPES 3
	enum MODEL_PROPS_TYPE
	{
		MPT_CLOTHING,
		MPT_FACIAL,
		MPT_RIGGING
	};
#endif

//=======================================================================================

// For cModelProps.cpp:
#ifndef PARTS_PER_PROP
	#define PARTS_PER_PROP	17
	enum PROP_PART
	{
		PP_HEAD,
		PP_HAIR,
		PP_TORSO,
		PP_LEGS,
		PP_ATLAS,
		PP_WRINKLES,
		PP_PATTERN,

		PP_FACIAL_WRINKLES,
		PP_FACIAL_EYE_SOCKETS,
		PP_FACIAL_SPOTS,
		PP_FACIAL_BEARD,
		PP_FACIAL_MOUSTACHE,
		PP_FACIAL_MAKEUP,

		PP_RIGGING_ZONES,
		PP_RIGGING_WEIGHTS,
		PP_RIGGING_DISPLACEMENT,
		PP_RIGGING_ANIMATION
	};
#endif

#ifndef SUBTYPES_PER_PROP
#define SUBTYPES_PER_PROP	9
	enum PROP_SUBTYPE
	{
		PST_SKIN,
		PST_HAIR,
		PST_CAP,
		PST_HEAD,
		PST_TORSO,
		PST_LEGS,
		PST_TORSO_AND_LEGS,
		PST_FACE,
		PST_RIG
	};
#endif

//=======================================================================================

// For cCrowdManager.cpp:
#ifndef __GROUP_FORMATIONS
#define __GROUP_FORMATIONS
	enum GROUP_FORMATION
	{
		GFRM_TRIANGLE,
		GFRM_SQUARE,
		GFRM_HEXAGON,
		GFRM_CIRCLE
	};
#endif

//=======================================================================================

#ifndef __MDP_TYPE
#define __MDP_TYPE
	enum MDP_TYPE
	{
		MDPT_SQUARE,
		MDPT_HEXAGON
	};
	enum MDP_MACHINE_STATE
	{
		MDPS_IDLE,
		MDPS_INIT_STRUCTURES_ON_HOST,
		MDPS_INIT_PERMS_ON_DEVICE,
		MDPS_UPLOADING_TO_DEVICE,
		MDPS_ITERATING_ON_DEVICE,
		MDPS_DOWNLOADING_TO_HOST,
		MDPS_UPDATING_POLICY,
		MDPS_READY,
		MDPS_ERROR
	};
#endif

//=======================================================================================


#define NUM_THREADS	2
#define PORT 7000


//=======================================================================================
//

#define			WIN_WIDTH		512
#define			WIN_HEIGHT		512
