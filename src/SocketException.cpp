/// @file SocketException.cpp
/// @author rev

#include "StdAfx.h"
#include "SocketException.h"

#include "Socket.h"

#include <iostream>
#include <sstream>
#include <iomanip>

RPW::Core::SocketException::SocketException( unsigned long wsaLastError )
	: RPW::Core::LastErrorException( wsaLastError )
{
}

RPW::Core::SocketException::~SocketException( void )
{
}

std::string RPW::Core::SocketException::formatErrorMessage( const RPW::Core::Socket& socket ) const
{
	return formatErrorMessage( m_errorCode, socket );
}

std::string RPW::Core::SocketException::formatErrorMessage( unsigned long wsaLastError, const RPW::Core::Socket& socket )
{
	std::ostringstream oss;

	if ( socket.isValid() )
	{
		RPW::Core::IpEndpoint ep( socket );
		oss << std::setw( 6 ) << (( SOCKET ) socket )
		    << " > " << std::setw( 15 ) << ep.getAddressString2()
		    << ":" << std::left << std::setw( 5 ) << ep.getPort();
	}
	else
	{
		oss << std::setw( 31 ) << " ";
	}

	oss << " - #" << std::left << std::setw( 5 ) << wsaLastError
	    << " " << std::setw( 15 ) << getShortErrorText( wsaLastError )
	    << " " << getLongErrorText( wsaLastError )
	    << std::endl;

	return oss.str();
}
