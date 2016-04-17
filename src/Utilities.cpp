/// @file Utilities.cpp
/// @author rev

#include "StdAfx.h"
#include "Utilities.h"

#include <assert.h>
#include <psapi.h> // link with psapi.lib
#include <strsafe.h>

bool RPW::Core::Utilities::FloatEquals( float lhs, float rhs, int maxUlps )
{
	// glossary
	//  ULP - "units in the last place"
	//   - a difference of 1 ULP is a one bit difference
	//   - an error of 1 ULP is between 1/8E6 and 1/16E6

	// Sanity check; may vary with platform
	assert( sizeof( float ) == sizeof( int ) );

	// Make sure maxUlps is non-negative and small enough that the
	// default NAN won't compare as equal to anything.
	assert( maxUlps > 0 && maxUlps < 4 * 1024 * 1024 );

	// less confusing than typecasting
	union { float f; int i; } a, b;

	// Make 'a' lexicographically ordered as a twos-complement int
	a.f = lhs;
	if ( a.i < 0 )
		a.i = 0x80000000 - a.i;

	// Make 'b' lexicographically ordered as a twos-complement int
	b.f = rhs;
	if ( b.i < 0 )
		b.i = 0x80000000 - b.i;

	int diff = a.i - b.i;

	return ( diff <= maxUlps ) && ( diff >= -maxUlps ); // save an abs() call
}

long long RPW::Core::Utilities::SystemTimeDiff( const SYSTEMTIME start, const SYSTEMTIME finish )
{
	union timeunion
	{
		FILETIME fileTime;
		ULARGE_INTEGER ul;
	};

	timeunion ft1;
	timeunion ft2;

	::SystemTimeToFileTime( &start, &ft1.fileTime );
	::SystemTimeToFileTime( &finish, &ft2.fileTime );

	return ft2.ul.QuadPart - ft1.ul.QuadPart;
}

long long RPW::Core::Utilities::SystemTimeDiffMs( const SYSTEMTIME start, const SYSTEMTIME finish )
{
	return RPW::Core::Utilities::SystemTimeDiff( start, finish ) / 10000;
}

std::string RPW::Core::Utilities::Trim( const std::string& str, const std::string& whiteSet )
{
	std::string::size_type const first = str.find_first_not_of( whiteSet.c_str() );
	return ( first == std::string::npos )
	       ? std::string()
	       : str.substr( first, str.find_last_not_of( whiteSet.c_str() ) - first + 1 );
}

std::vector<std::string> RPW::Core::Utilities::TokenizeStr( const std::string& str, const std::string& delims )
{
	// Skip delims at beginning, find start of first token
	std::string::size_type lastPos = str.find_first_not_of( delims, 0 );
	// Find next delimiter @ end of token
	std::string::size_type pos     = str.find_first_of( delims, lastPos );

	// output vector
	std::vector<std::string> tokens;

	while ( std::string::npos != pos || std::string::npos != lastPos )
	{
		// Found a token, add it to the vector.
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = str.find_first_not_of( delims, pos );
		// Find next delimiter at end of token.
		pos     = str.find_first_of( delims, lastPos );
	}

	return tokens;
}

std::string RPW::Core::Utilities::GetRealFilename( const char* const szFilename )
{
	char szRealFilename[ MAX_PATH + 1 ] = {0};
	char szTempFilename[ MAX_PATH + 1 ] = {0};
	if ( GetShortPathNameA( szFilename, szTempFilename, MAX_PATH ) )
	{
		if ( GetLongPathNameA( szTempFilename, szRealFilename, MAX_PATH ) )
		{
			return std::string( szRealFilename );
		}
	}

	return std::string( szFilename );
}

std::string RPW::Core::Utilities::GetFileNameFromHandle( HANDLE hFile )
{
	char pszFilename[MAX_PATH + 1] = {0};

	// Get the file size.
	unsigned long dwFileSizeHi = 0;
	unsigned long dwFileSizeLo = GetFileSize( hFile, &dwFileSizeHi );

	if ( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
	{
		throw std::runtime_error( "Cannot map a file with a length of zero." );
	}

	// Create a file mapping object.
	HANDLE hFileMap = ::CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 1, NULL );

	if ( hFileMap )
	{
		// Create a file mapping to get the file name.
		void* pMem = ::MapViewOfFile( hFileMap, FILE_MAP_READ, 0, 0, 1 );

		if ( pMem )
		{
			// note need to link with psapi.lib
			if ( ::GetMappedFileNameA( GetCurrentProcess(), pMem, pszFilename, MAX_PATH ) )
			{
				// Translate path with device name to drive letters.
				char szTemp[MAX_PATH + 1] = {0};
				szTemp[0] = '\0';

				if ( ::GetLogicalDriveStringsA( MAX_PATH, szTemp ) )
				{
					char szName[MAX_PATH + 1] = {0};
					char szDrive[3] = " :";
					bool bFound = false;
					char* p = szTemp;

					do
					{
						// Copy the drive letter to the template string
						*szDrive = *p;

						// Look up each device name
						if ( ::QueryDosDeviceA( szDrive, szName, MAX_PATH ) )
						{
							unsigned int uNameLen = ::strlen( szName );

							if ( uNameLen < MAX_PATH )
							{
								bFound = ::_strnicmp( pszFilename, szName, uNameLen ) == 0;

								if ( bFound )
								{
									// Reconstruct pszFilename using szTempFile
									// Replace device path with DOS path
									char szTempFile[MAX_PATH] = {0};
									::StringCchPrintfA( szTempFile,
									                    MAX_PATH,
									                    "%s%s",
									                    szDrive,
									                    pszFilename + uNameLen );
									::StringCchCopyNA( pszFilename, MAX_PATH + 1, szTempFile, ::strlen( szTempFile ) );
								}
							}
						}

						while ( *p++ ); // Go to the next NULL character.

					}
					while ( !bFound && *p ); // end of string
				}
				else
				{
					throw std::runtime_error( "Unable to get logical drive strings." );
				}
			}
			else
			{
				throw std::runtime_error( "Unable to get mapped filename." );
			}

			::UnmapViewOfFile( pMem );
		}
		else
		{
			throw std::runtime_error( "Unable to create file map." );
		}

		::CloseHandle( hFileMap );
	}
	else
	{
		throw std::runtime_error( "Unable to create file mapping object." );
	}

	return std::string( pszFilename );
}
