#include "StdAfx.h"
#include "FileSystem.h"

#include "LastErrorException.h"

#include <windows.h>
#include <sstream>
#include <iomanip>

RPW::Core::FileSystem::FileSystem( void )
	: m_activePath( GetExecutingAssemblyPath() )
{
}

RPW::Core::FileSystem::~FileSystem( void )
{
}

void RPW::Core::FileSystem::cd( const char* const path )
{
	RPW::Core::CriticalSection::Scope scope( m_cs );
	SetCurrentFolder( m_activePath.c_str() );
	SetCurrentFolder( path );
	m_activePath = GetCurrentFolder();
}

void RPW::Core::FileSystem::ls( std::vector<std::string>& listing
                                , int detailLevel /*= 0*/
                                , const char* const glob /*= "*"*/ )
{
	RPW::Core::CriticalSection::Scope scope( m_cs );
	return ls( m_activePath.c_str(), listing, detailLevel, glob );
}

void RPW::Core::FileSystem::ls( const char* const path
                                , std::vector<std::string>& listing
                                , int detailLevel /*= 0*/
                                , const char* const glob /*= "*"*/ )
{
	WIN32_FIND_DATAA fdata;
	HANDLE dhandle;

	SetCurrentFolder( path );

	// must append \* to the path
	{
		char buf[MAX_PATH];
		::sprintf_s( buf, sizeof( buf ), "%s\\%s", path, glob );
		if (( dhandle = ::FindFirstFileA( buf, &fdata ) ) == INVALID_HANDLE_VALUE )
		{
			throw RPW::Core::LastErrorException( ::GetLastError() );
		}
	}

	while ( true )
	{
		switch ( detailLevel )
		{
			case 0:
			default:
				listing.push_back( std::string( fdata.cFileName ) );
				break;

			case 1:
			{
				bool isFolder = ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) > 0;
				LONGLONG size = ( static_cast<LONGLONG>( fdata.nFileSizeHigh ) << 32 )
				                + static_cast<LONGLONG>( fdata.nFileSizeLow );
				std::ostringstream oss;
				if ( isFolder )
				{
					oss << std::right << std::setw( 9 ) << "(dir)";
				}
				else
				{
					oss << std::right << std::setw( 9 ) << size;
				}
				oss << " " << std::left << fdata.cFileName;
				listing.push_back( oss.str() );
			}
			break;
		}

		if ( FALSE == ::FindNextFileA( dhandle, &fdata ) )
		{
			DWORD err = ::GetLastError();
			if ( ERROR_NO_MORE_FILES == err )
			{
				break;
			}
			else
			{
				::FindClose( dhandle );
				throw RPW::Core::LastErrorException( err );
			}
		}
	}

	if ( FALSE == ::FindClose( dhandle ) )
	{
		throw RPW::Core::LastErrorException( ::GetLastError() );
	}
}

std::string RPW::Core::FileSystem::GetActivePath( void ) const
{
	return m_activePath;
}

std::string RPW::Core::FileSystem::GetExecutingAssemblyPath( void )
{
	std::string eap( ::GetCommandLineA() );
	std::string::size_type left = eap.find_first_not_of( '"' );
	std::string::size_type right = eap.find_last_of( '\\' );

	if ( std::string::npos == left
	      || std::string::npos == right )
	{
		throw std::runtime_error( "unexpected result from ::GetCommandLine" );
	}

	return eap.substr( left, right - left );
}

bool RPW::Core::FileSystem::FileExists( const char* const name ) const
{
	return FileExists( m_activePath.c_str(), name );
}

bool RPW::Core::FileSystem::FileExists( const char* const path, const char* const name )
{
	std::ostringstream oss;
	oss << path << '\\' << name;
	DWORD fa = ::GetFileAttributesA( oss.str().c_str() );
	return 0xFFFFFFFF != fa;
}

std::string RPW::Core::FileSystem::GetCurrentFolder( void )
{
	char dir[MAX_PATH];
	DWORD dir_len = ::GetCurrentDirectoryA( sizeof( dir ), dir );
	if ( 0 == dir_len )
	{
		throw RPW::Core::LastErrorException( ::GetLastError() );
	}

	return std::string( dir, dir_len );
}

void RPW::Core::FileSystem::SetCurrentFolder( const char* const path )
{
	if ( FALSE == ::SetCurrentDirectoryA( path ) )
	{
		throw RPW::Core::LastErrorException( ::GetLastError() );
	}
}

