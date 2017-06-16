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

#include "cXmlParser.h"
#include "cStringUtils.h"
#include <sstream>

//=======================================================================================

XmlParser::XmlParser( LogManager* log_manager, char* pFilename )
{
	this->log_manager	= log_manager;
	doc					= new TiXmlDocument( pFilename );
	if( doc->LoadFile() )
	{
		log_manager->file_log( LogManager::XML, "%s loaded OK.", pFilename );
	}
	else
	{
		log_manager->log( LogManager::LERROR,
						  "XmlParser::Failed to load file \"%s\".",
						  pFilename );
		log_manager->separator();
	}
}

//=======================================================================================

XmlParser::~XmlParser( void )
{
	FREE_INSTANCE( doc );
}

//=======================================================================================

char* XmlParser::getElement( TiXmlNode* pParent, char* name, char* currname )
{
    string ret( "" );
	if( !pParent )
	{
		return (char*)ret.c_str();
	}
	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	if( t == TiXmlNode::TINYXML_ELEMENT )
	{
		currname = (char *)pParent->Value();
	}
	else if ( t == TiXmlNode::TINYXML_TEXT && strcmp( name, currname ) == 0 )
	{
		pText = pParent->ToText();
		return (char *)pText->Value();
	}
	for( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling() )
	{
		char *temp = getElement( pChild, name, currname );
		if( strcmp( temp, "" ) != 0 )
		{
			return temp;
		}
	}
	return (char*)ret.c_str();
}

//=======================================================================================

void XmlParser::dump( void )
{
	dump_to_stdout( doc );
}

//=======================================================================================

TiXmlDocument* XmlParser::getDoc( void )
{
	return doc;
}

//=======================================================================================

void XmlParser::dump_to_stdout( TiXmlNode* pParent, unsigned int indent )
{
	if( !pParent )
	{
		return;
	}
	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	log_manager->log( LogManager::XML, "%s", getIndent( indent ) );
	int num;

	switch( t )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			log_manager->log( LogManager::XML, "Document" );
			break;
		case TiXmlNode::TINYXML_ELEMENT:
			log_manager->log( LogManager::XML, "Element [%s]", pParent->Value() );
			num = dump_attribs_to_stdout( pParent->ToElement(), indent+1 );
			switch( num )
			{
				case 0:
					log_manager->log( LogManager::XML, " (No attributes)");
					break;
				case 1:
					log_manager->log( LogManager::XML,
									  "%s1 attribute",
									  getIndentAlt( indent ) );
					break;
				default:
					log_manager->log( LogManager::XML,
									  "%s%d attributes",
									  getIndentAlt(indent),
									  num );
					break;
			}
			break;
		case TiXmlNode::TINYXML_COMMENT:
			log_manager->log( LogManager::XML, "Comment: [%s]", pParent->Value() );
			break;

		case TiXmlNode::TINYXML_UNKNOWN:
			log_manager->log( LogManager::XML, "Unknown" );
			break;

		case TiXmlNode::TINYXML_TEXT:
			pText = pParent->ToText();
			log_manager->log( LogManager::XML, "Text: [%s]", pText->Value() );
			break;

		case TiXmlNode::TINYXML_DECLARATION:
			log_manager->log( LogManager::XML, "Declaration" );
			break;
		default:
			break;
	}
	for( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling() )
	{
		dump_to_stdout( pChild, indent + 1 );
	}
}

//=======================================================================================

const char* XmlParser::getIndent( unsigned int numIndents )
{
	static const char *pINDENT = "                                      + ";
	static const unsigned int LENGTH = strlen( pINDENT );
	unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
	if( n > LENGTH )
	{
		n = LENGTH;
	}
	return &pINDENT[ LENGTH-n ];
}

//=======================================================================================

const char* XmlParser::getIndentAlt( unsigned int numIndents )
{
	static const char *pINDENT = "                                        ";
	static const unsigned int LENGTH = strlen( pINDENT );
	unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;
	return &pINDENT[ LENGTH-n ];
}

//=======================================================================================

int XmlParser::dump_attribs_to_stdout( TiXmlElement* pElement, unsigned int indent )
{
	if( !pElement )
	{
		return 0;
	}
	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	int i = 0;
	int ival;
	double dval;
	const char* pIndent = getIndent( indent );
	while( pAttrib )
	{
		log_manager->log( LogManager::XML,
						  "%s%s: value=[%s]",
						  pIndent,
						  pAttrib->Name(),
						  pAttrib->Value() );
		if( pAttrib->QueryIntValue( &ival ) == TIXML_SUCCESS )
		{
			log_manager->log( LogManager::XML, " int=%d", ival );
		}
		if( pAttrib->QueryDoubleValue( &dval ) == TIXML_SUCCESS )
		{
			log_manager->log( LogManager::XML, " d=%1.1f", dval );
		}
		i++;
		pAttrib = pAttrib->Next();
	}
	return i;
}

//=======================================================================================

unsigned int XmlParser::getUI( char* name )
{
    string empty( "" );
	char* element = getElement( doc, name, (char*)empty.c_str() );
	std::stringstream ss1( element );
	unsigned int ELEMENT;
	ss1 >> ELEMENT;
	if( strlen( element ) > 0 )
	{
		log_manager->file_log( LogManager::XML, "%s = %u.", name, ELEMENT );
	}
	return ELEMENT;
}

//=======================================================================================

unsigned long XmlParser::getUL( char* name )
{
    string empty( "" );
	char *element = getElement( doc, name, (char*)empty.c_str() );
	unsigned long ELEMENT = atol( element );
	if( strlen( element ) > 0 )
	{
		log_manager->file_log( LogManager::XML, "%s = %u.", name, ELEMENT );
	}
	return ELEMENT;
}

//=======================================================================================

float XmlParser::getF( char* name )
{
    string empty( "" );
	char *element = getElement( doc, name, (char*)empty.c_str() );
	std::istringstream iss1( element );
	float ELEMENT;
    iss1 >> ELEMENT;
	if( strlen( element ) > 0 )
	{
		log_manager->file_log( LogManager::XML, "%s = %.2ff.", name, ELEMENT );
	}
	return ELEMENT;
}

//=======================================================================================

char* XmlParser::getC( char* name )
{
    string empty( "" );
	char *element = getElement( doc, name, (char*)empty.c_str() );
	if( strlen( element ) > 0 )
	{
		log_manager->file_log( LogManager::XML, "%s = %s.", name, element );
	}
	return element;
}

//=======================================================================================

bool XmlParser::getB( char* name )
{
    string empty( "" );
	char *element = getElement( doc, name, (char*)empty.c_str() );
	StringUtils::toUpper( element );
	if( strcmp( element, "TRUE" ) == 0 )
	{
		log_manager->file_log( LogManager::XML, "%s = TRUE.", name );
		return true;
	}
	log_manager->file_log( LogManager::XML, "%s = FALSE.", name );
	return false;
}

//=======================================================================================
