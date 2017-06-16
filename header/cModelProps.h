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

#pragma once
#include <vector>
#include <string>
#include <map>

#include "cMacros.h"
#include "cModel3D.h"

using namespace std;

//=======================================================================================

#ifndef __MODEL_PROP
#define __MODEL_PROP
class ModelProp
{
public:
	ModelProp( void )
	{
		type			= PP_HEAD;
		subtype			= PST_SKIN;
		name			= "";
		file			= "";
		ref_wrinkles	= NULL;
		model3D			= NULL;
		textureID		= 0;
		loaded			= false;
	};
	~ModelProp( void )
	{

	}

	PROP_PART				type;
	PROP_SUBTYPE			subtype;
	string					name;
	string					file;
	ModelProp*				ref_wrinkles;
	vector<ModelProp*>		ref_pattern;
	vector<ModelProp*>		ref_atlas;
	Model3D*				model3D;
	GLuint					textureID;
	bool					loaded;
};
#endif

//=======================================================================================

#ifndef __MODEL_PROPS
#define __MODEL_PROPS

class ModelProps
{
public:
								ModelProps	(	MODEL_PROPS_TYPE	_type,
												string				_name,
												MODEL_GENDER		_gender			);
								~ModelProps	(	void								);

	void						addProp		(	ModelProp*			prop			);
	void						addProp		(	PROP_PART			type,
												PROP_SUBTYPE		subtype,
												string				name,
												string				file,
												ModelProp*			ref_wrinkles,
												vector<ModelProp*>&	ref_pattern,
												vector<ModelProp*>&	ref_atlas,
												Model3D*			model3D,
												GLuint&				textureID,
												bool				loaded			);
	ModelProp*					getProp		(	string				prop_name		);
	vector<ModelProp*>&			getProps	(	void								);
	string&						getName		(	void								);
	MODEL_PROPS_TYPE&			getType		(	void								);

private:
	MODEL_PROPS_TYPE			type;
	string						name;
	MODEL_GENDER				gender;
	vector<ModelProp*>			props;
	map<string, unsigned int>	name_prop_map;
};

#endif
//=======================================================================================
