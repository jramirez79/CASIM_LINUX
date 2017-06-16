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

#pragma once
#include <tinyxml.h>

#include "cMacros.h"
#include "cLogManager.h"

#ifndef __XML_PARSER
#define __XML_PARSER

class XmlParser
{
public:
	XmlParser( LogManager* log_manager, char* pFilename );
	~XmlParser( void );

	char*			getElement				( TiXmlNode*	pParent,
											  char*			name,
											  char*			currname	);
	unsigned int	getUI					( char*			name		);
	unsigned long	getUL					( char*			name		);
	char*			getC					( char*			name		);
	float			getF					( char*			name		);
	bool			getB					( char*			name		);
	void			dump					( void						);
	TiXmlDocument*	getDoc					( void						);
private:
	TiXmlDocument*	doc;
	LogManager*		log_manager;
private:
	void			dump_to_stdout			( TiXmlNode*	pParent,
											  unsigned int	indent = 0	);
	const char*		getIndent				( unsigned int	numIndents	);
	const char*		getIndentAlt			( unsigned int	numIndents	);
	int				dump_attribs_to_stdout	( TiXmlElement* pElement,
											  unsigned int	indent		);
};

#endif
