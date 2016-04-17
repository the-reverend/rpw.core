/// @file IniReader.cpp
/// @author rev

#include "StdAfx.h"
#include "IniReader.h"

#include <sstream>
#include <iostream>
#include <fstream>

/// @brief constructor
RPW::Core::IniReader::IniReader( bool caseSensitive )
	: m_bCaseSensitive( caseSensitive )
	, m_strFilePath()
	, m_strLastSection()
	, m_mapKeyValue()
{
}

/// @brief destructor
RPW::Core::IniReader::~IniReader()
{
}

/// @brief load the ini file to the key/value map
/// opens the file and returns true if successful
bool RPW::Core::IniReader::Load( const std::string& filePath )
{
	bool retVal = false;
	std::ifstream inFile;
	inFile.open( filePath.c_str() );
	if ( inFile.is_open() )
	{
		Load( inFile );
		retVal = true;
	}
	inFile.close();

	return retVal;
}

/// @brief load the ini file to the key/value map
/// opens the file and returns true if successful
bool RPW::Core::IniReader::Load( std::istream& inFile )
{
	bool retVal = false;
	std::string line;

	m_strLastSection = "";
	while ( std::getline( inFile, line ) && !inFile.fail() )
	{
		retVal = true;
		// note: getline(inFile)  - requires a std::string buffer
		//       inFile.getline() - requires a char buffer[256]

		Parse( line );

		// note: inFile.eof() is true after successfully reading the last line.
		//       inFile.fail() is true after attempting to read past the eof.
		if ( inFile.eof() ) break;
	}

	return retVal;
}

void RPW::Core::IniReader::Clear()
{
	m_mapKeyValue.clear();
	m_setSections.clear();
	m_vecSections.clear();
}

std::string RPW::Core::IniReader::GetString( const std::string& key, const char* const defaultString, bool throwOnFail )
{
	std::string searchKey = key;

	if ( !m_bCaseSensitive )
	{
		std::transform( searchKey.begin(), searchKey.end(), searchKey.begin(), tolower );
	}

	if ( m_mapKeyValue.find( searchKey ) != m_mapKeyValue.end() )
	{
		return m_mapKeyValue[searchKey];
	}

	if ( throwOnFail )
	{
		std::ostringstream oss;
		oss << "unable to find the key/value pair in the ini map: " << key;
		throw std::runtime_error( oss.str().c_str() );
	}

	return std::string( defaultString );
}

std::string RPW::Core::IniReader::GetString( const std::string& key, const std::string& defaultString, bool throwOnFail )
{
	return GetString( key, defaultString.c_str(), throwOnFail );
}

unsigned long RPW::Core::IniReader::GetHex( const std::string& key, const std::string& defaultString, bool throwOnFail )
{
	std::string value = GetString( key, defaultString.c_str(), throwOnFail );
	unsigned long tmpVal = 0;
	sscanf_s( value.c_str(), "%x", &tmpVal );
	return tmpVal;
}

/// @brief parse a line from the ini file
/// @details parse a line from the ini file and build the key/value map
/// @note the map keys are formatted with the section embedded as "section::key"
void RPW::Core::IniReader::Parse( const std::string& line )
{
	std::string comment;
	std::string entry;

	std::string::size_type semicolon = line.find_first_of( ";", 0 );
	if ( semicolon != line.npos )
	{
		comment = RPW::Core::Utilities::Trim( line.substr( semicolon + 1 ) );
		entry = RPW::Core::Utilities::Trim( line.substr( 0, semicolon ) );
	}
	else
	{
		entry = RPW::Core::Utilities::Trim( line );
	}

	std::string::size_type leftbracket  = entry.find_first_of( "[", 0 );
	std::string::size_type rightbracket = entry.find_first_of( "]", 0 );
	std::string::size_type equals = entry.find_first_of( "=", 0 );

	bool equalsBeforeBracket = equals != entry.npos && ( leftbracket == entry.npos || equals < leftbracket );
	bool malFormedBrackets = leftbracket == entry.npos || rightbracket == entry.npos || rightbracket < leftbracket ;

	if ( !equalsBeforeBracket && !malFormedBrackets )
	{
		m_strLastSection = RPW::Core::Utilities::Trim( entry.substr( leftbracket + 1, rightbracket - leftbracket - 1 ) );
		if ( !m_bCaseSensitive )
			std::transform( m_strLastSection.begin(), m_strLastSection.end(), m_strLastSection.begin(), tolower );
		m_setSections.insert( m_strLastSection );
		m_vecSections.push_back( m_strLastSection );
	}
	else if ( equals != entry.npos )
	{
		bool blankVal = equals >= ( entry.length() - 1 );
		std::string k = RPW::Core::Utilities::Trim( entry.substr( 0, equals ) );
		if ( !m_bCaseSensitive )
			std::transform( k.begin(), k.end(), k.begin(), tolower );
		std::string key = m_strLastSection + "::" + k;
		std::string val( "" );
		if ( !blankVal )
		{
			/// @todo use StringEx.split1 instead
			std::vector<std::string> tokens = RPW::Core::Utilities::TokenizeStr( RPW::Core::Utilities::Trim( entry.substr( equals + 1 ) ), "\"" );
			if ( tokens.size() > 0 )
				val = tokens[0];
		}
		m_mapKeyValue[key] = val;
	}
}
