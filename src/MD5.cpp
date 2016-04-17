#include "StdAfx.h"
#include "MD5.h"

#include <assert.h>
#include <windows.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

RPW::Core::MD5::MD5()
{
}

RPW::Core::MD5::~MD5()
{
}

// creates a MD5 hash from
// "text" and returns it as
// string
std::string RPW::Core::MD5::getHashFromString( const std::string& text )
{
	MD5_CTX ctx;

	// init md5
	MD5Init( &ctx );

	// update with our string
	MD5Update( &ctx,
	           ( unsigned char* )text.c_str(),
	           static_cast<unsigned int>( text.length() ) );

	// create the hash
	unsigned char buff[16] = "";
	MD5Final(( unsigned char* )buff, &ctx );

	// convert the hash to a string and return it
	return convToString( buff, sizeof( buff ) );
}

// creates a MD5 hash from
// a file specified in "filename" and
// returns it as string
// (based on Ronald L. Rivest's code
// from RFC1321 "The MD5 Message-Digest Algorithm")
std::string RPW::Core::MD5::getHashFromFile( const std::string& filename )
{
	std::string hash;

	FILE *file;
	if ( 0 == fopen_s( &file, filename.c_str(), "rb" ) )
	{
		// init md5
		MD5_CTX context;
		MD5Init( &context );

		// read the filecontent
		size_t len = 0;
		unsigned char buffer[1024];
		while (( len = fread( buffer, 1, 1024, file ) ) )
		{
			MD5Update( &context, buffer, static_cast<unsigned int>( len ) );
		}

		// generate hash, close the file and return the
		// hash as std::string
		unsigned char digest[16];
		MD5Final( digest, &context );
		fclose( file );

		hash = convToString( digest, sizeof( digest ) );
	}

	return hash;
}

/// @brief converts the numeric hash to a valid std::string.
std::string RPW::Core::MD5::convToString( const unsigned char* const bytes, unsigned long len )
{
	std::ostringstream oss;

	for ( unsigned long i = 0; i < len; ++i )
	{
		oss << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( bytes[i] );
	}

	return oss.str();
}
