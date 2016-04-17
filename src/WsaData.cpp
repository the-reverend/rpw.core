/// @file WsaData.cpp
/// @author rev

#include "StdAfx.h"
#include "WsaData.h"

#include <stdexcept>

RPW::Core::WsaData::WsaData( BYTE major, BYTE minor )
{
	if ( 0 != ::WSAStartup( MAKEWORD( major, minor ), &m_wsa ) )
	{
		throw std::runtime_error( "WSAStartup failed" );
	}
}

RPW::Core::WsaData::~WsaData( void )
{
	::WSACleanup();
}
