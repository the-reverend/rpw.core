#include "StdAfx.h"
#include "StringEx.h"

RPW::Core::StringEx::StringEx( void )
{
}

RPW::Core::StringEx::StringEx( const char* const initial )
	: std::string( initial )
{
}

RPW::Core::StringEx::StringEx( const char* const initial, std::string::size_type len )
	: std::string( initial, len )
{
}

RPW::Core::StringEx::StringEx( const std::string& initial )
	: std::string( initial )
{
}

RPW::Core::StringEx::~StringEx( void )
{
}

bool RPW::Core::StringEx::split1( std::vector<RPW::Core::StringEx>& vec, char delim, char leftquote, char rightquote )
{
	return RPW::Core::StringEx::split1<RPW::Core::StringEx>( *this, vec, delim, leftquote, rightquote );
}

std::vector<RPW::Core::StringEx> RPW::Core::StringEx::split1( char delim, char leftquote, char rightquote )
{
	std::vector<RPW::Core::StringEx> vec;
	split1( vec, delim, leftquote, rightquote );
	return vec;
}

std::string RPW::Core::StringEx::stripAnsi( void )
{
	return stripAnsi( *this );
}

unsigned __int64 RPW::Core::StringEx::hash( void )
{
	return hash( *this );
}

bool RPW::Core::StringEx::split1( const std::string& str, std::vector<std::string>& vec, char delim, char leftquote, char rightquote )
{
	return RPW::Core::StringEx::split1<std::string>( str, vec, delim, leftquote, rightquote );
}

std::string RPW::Core::StringEx::stripAnsi( const std::string& ansiText )
{
	std::ostringstream oss;

	// ansi regex
	// \x1b\[([0-9]+(;[0-9]+)*)?[mHfABCDsuJKhl]

	unsigned short state = 0;
	for ( std::string::size_type i = 0; i < ansiText.length(); i++ )
	{
		char c = ansiText[i];
		switch ( state )
		{
			case 0: // not in ansi sequence
				if ( c == '\x1b' ) state = 1;
				else oss << c;
				break;

			case 1: // in ansi, but let's look for the [ else jump out
				if ( c == '[' ) state = 2;
				else
				{
					state = 0;
					oss << '\x1b' << c;
				}
				break;

			case 2: // in ansi sequence for sure; look for the out
				switch ( c )
				{
					case 'm':
					case 'H':
					case 'f':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 's':
					case 'u':
					case 'J':
					case 'K':
					case 'h':
					case 'l':
						state = 0;
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
	}

	return oss.str();
}

unsigned __int64 RPW::Core::StringEx::hash( const std::string& str )
{
	unsigned __int64 hash = 5381;

	for ( std::string::const_iterator it = str.begin(); it != str.end(); ++it )
	{
		hash = hash * 33 + *it;
	}

	return hash;
}
