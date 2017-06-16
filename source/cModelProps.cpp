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

#include "cModelProps.h"

//=======================================================================================
//
ModelProps::ModelProps( MODEL_PROPS_TYPE _type, string _name, MODEL_GENDER _gender )
{
	type	= _type;
	name	= string( _name );
	gender	= _gender;
}
//
//=======================================================================================
//
ModelProps::~ModelProps( void )
{

}
//
//=======================================================================================
//
//
void ModelProps::addProp( ModelProp* prop )
{
	if( name_prop_map.find( prop->name ) == name_prop_map.end() )
	{
		name_prop_map[prop->name] = props.size();
		props.push_back( prop );
	}
}
//
//=======================================================================================
//
void ModelProps::addProp(	PROP_PART				type,
							PROP_SUBTYPE			subtype,
							string					name,
							string					file,
							ModelProp*				ref_wrinkles,
							vector<ModelProp*>&		ref_pattern,
							vector<ModelProp*>&		ref_atlas,
							Model3D*				model3D,
							GLuint&					textureID,
							bool					loaded		)
{
	if( name_prop_map.find( name ) == name_prop_map.end() )
	{
		ModelProp* mp				= new ModelProp();
		mp->type					= type;
		mp->subtype					= subtype;
		mp->name					= name;
		mp->file					= file;
		mp->ref_wrinkles			= ref_wrinkles;
		for( unsigned int p = 0; p < ref_pattern.size(); p++ )
		{
			mp->ref_pattern.push_back( ref_pattern[p] );
		}
		for( unsigned int a = 0; a < ref_atlas.size(); a++ )
		{
			mp->ref_atlas.push_back( ref_atlas[a] );
		}
		mp->model3D					= model3D;
		mp->textureID				= textureID;
		mp->loaded					= loaded;
		name_prop_map[mp->name]		= props.size();
		props.push_back( mp );
	}
}
//
//=======================================================================================
//
ModelProp* ModelProps::getProp( string prop_name )
{
	if( name_prop_map.find( prop_name ) != name_prop_map.end() )
	{
		return props[name_prop_map[prop_name]];
	}
	else
	{
		return NULL;
	}
}
//
//=======================================================================================
//
vector<ModelProp*>& ModelProps::getProps( void )
{
	return props;
}
//
//=======================================================================================
//
string& ModelProps::getName( void )
{
	return name;
}
//
//=======================================================================================
//
MODEL_PROPS_TYPE& ModelProps::getType( void )
{
	return type;
}
//
//=======================================================================================
