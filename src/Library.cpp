/// @file Library.cpp
/// @author rev

#include "StdAfx.h"
#include "Library.h"

#include <Process.h>
#include <sstream>

RPW::Core::Library::Library( LPCTSTR module )
	: m_hModule( NULL )
{
	m_hModule = ::LoadLibrary( module );
	if ( NULL == m_hModule )
	{
		std::stringstream oss;
		oss << "Failed to load module : " << module;
		throw std::runtime_error( oss.str().c_str() );
	}
}

RPW::Core::Library::~Library()
{
	::FreeLibrary( m_hModule );
}

RPW::Core::Library::operator HMODULE() const
{
	return m_hModule;
}
