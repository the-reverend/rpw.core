/// @file ModuleVersion.cpp
/// @author rev

#include "StdAfx.h"
#include "ModuleVersion.h"

#include <sstream>
#include <iomanip>

RPW::Core::ModuleVersion::ModuleVersion()
	: m_pVersionInfo( NULL )
{
}

RPW::Core::ModuleVersion::~ModuleVersion()
{
	delete [] m_pVersionInfo;
	m_pVersionInfo = NULL;
}

bool RPW::Core::ModuleVersion::Read()
{
	return Read( ::GetModuleHandle( NULL ) );
}

bool RPW::Core::ModuleVersion::Read( const char* const modulename )
{
	// get module handle
	HMODULE hModule = ::GetModuleHandleA( modulename );

	return (( hModule == NULL ) && ( modulename != NULL ) ) ? FALSE : Read( hModule );
}

bool RPW::Core::ModuleVersion::Read( HMODULE hModule )
{
	m_translation.charset = 1252; // default = ANSI code page
	::memset(( VS_FIXEDFILEINFO* )this, 0, sizeof( VS_FIXEDFILEINFO ) );

	bool success = false; // assume failure

	switch ( 0 )
	{
		default: // fallthrough
		case 0:
		{
			// get module file name
			char filename[_MAX_PATH] = {0};
			if ( 0 >= ::GetModuleFileNameA( hModule, filename, sizeof( filename ) ) )
				{ break; }

			// read file version info
			DWORD dwDummyHandle = 0;
			DWORD len = ::GetFileVersionInfoSizeA( filename, &dwDummyHandle );
			if ( len <= 0 )
				{ break; }

			// get file version info
			delete [] m_pVersionInfo;
			m_pVersionInfo = new BYTE[len]; // allocate version info
			if ( FALSE == ::GetFileVersionInfoA( filename, 0, len, m_pVersionInfo ) )
				{ break; }

			// query version value
			LPVOID lpvi = NULL;
			UINT iLen = 0;
			if ( FALSE == ::VerQueryValueA( m_pVersionInfo, "\\", &lpvi, &iLen ) )
				{ break; }

			// copy fixed info to myself; i am derived from VS_FIXEDFILEINFO
			*( VS_FIXEDFILEINFO* )this = *( VS_FIXEDFILEINFO* )lpvi;

			// Get translation info
			if ( FALSE == ::VerQueryValueA( m_pVersionInfo, "\\VarFileInfo\\Translation", &lpvi, &iLen )
			      && iLen >= 4 )
				{ break; }

			m_translation = *( TRANSLATION* )lpvi;
			success = true; // no failures if case did not break
		}
		break;
	}

	return success && ( dwSignature == VS_FFI_SIGNATURE );
}

std::string RPW::Core::ModuleVersion::GetValue( const char* const szKeyName )
{
	std::string value;
	if ( m_pVersionInfo )
	{
		// To get a string value must pass query in the form
		//
		//    "\StringFileInfo\<langID-codepage>\keyname"
		//
		// where <lang-codepage> is the languageID concatenated with the
		// code page, in hex.

		std::ostringstream oss;
		oss << "\\StringFileInfo\\"
		    << std::setw( 4 ) << std::setfill( '0' ) << std::setbase( 16 ) << m_translation.langID
		    << std::setw( 4 ) << std::setfill( '0' ) << std::setbase( 16 ) << m_translation.charset;
		oss << "\\" << szKeyName;

		char* pVal;
		UINT iLenVal;
		if ( ::VerQueryValueA( m_pVersionInfo, oss.str().c_str(), ( LPVOID* )&pVal, &iLenVal ) )
		{
			value = pVal;
		}
	}

	return value;
}
