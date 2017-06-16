// @REV_TAG SRL_09_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjam�n Hern�ndez A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include "cStringUtils.h"

//=======================================================================================
//
StringUtils::StringUtils( void )
{

}
//
//=======================================================================================
//
StringUtils::~StringUtils( void )
{

}
//
//=======================================================================================
//
vector<string>& StringUtils::split( const string&	s,
									char			delim,
									vector<string>&	elems   )
{
    stringstream ss( s );
	ss << s;
    string item;
    while( getline( ss, item, delim ) )
	{
        elems.push_back( item );
    }
    return elems;
}
//
//=======================================================================================
//
vector<string> StringUtils::split( const string&	s,
								   char				delim   )
{
    vector<string> elems;
    return split( s, delim, elems );
}
//
//=======================================================================================
//
void StringUtils::toUpper( char* in )
{
	unsigned int size = strlen( in );
	for( unsigned int num = 0; num < size; num++ )
	{
		if( in[num] >= 'a' && in[num] <= 'z' ) in[num] += (char)('A' - 'a');
	}
}
//
//=======================================================================================
//
const char* StringUtils::getNameFromPath( char* name )
{
	string fname( name );
	vector<string> sep = split( fname, '/' );
	string last = sep[ sep.size() - 1 ];
	return last.c_str();
}
//
//=======================================================================================
//
string StringUtils::getStrNameFromPath( char* name )
{
	string fname( name );
	vector<string> sep = split( fname, '/' );
	return sep[ sep.size() - 1 ];
}
//
//=======================================================================================
