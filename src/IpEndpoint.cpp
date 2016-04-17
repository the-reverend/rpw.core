/// @file IpEndpoint.cpp
/// @author rev

#include "StdAfx.h"
#include "IpEndpoint.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

RPW::Core::IpEndpoint::IpEndpoint( void )
{
	zero();
}

RPW::Core::IpEndpoint::IpEndpoint( unsigned long hostOrderAddress, unsigned short hostOrderPort, ADDRESS_FAMILY family )
{
	init( hostOrderAddress, hostOrderPort, family );
}

RPW::Core::IpEndpoint::IpEndpoint( const char* const ip, unsigned short hostOrderPort, ADDRESS_FAMILY family )
{
	init( ip, hostOrderPort, family );
}

RPW::Core::IpEndpoint::IpEndpoint( SOCKET sock )
{
	init( sock );
}

RPW::Core::IpEndpoint::~IpEndpoint( void )
{
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::init( unsigned long hostOrderAddress, unsigned short hostOrderPort, ADDRESS_FAMILY family )
{
	return zero().setAddress( hostOrderAddress ).setPort( hostOrderPort ).setFamily( family );
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::init( const char* const ip, unsigned short hostOrderPort, ADDRESS_FAMILY family )
{
	return init( getAddressULong( ip ), hostOrderPort, family );
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::init( SOCKET sock )
{
	zero();
	getSockName( sock );
	return *this;
}

RPW::Core::IpEndpoint::operator sockaddr*( void )
{
//	std::cout << "3-" << reinterpret_cast<LONGLONG>( &m_addr ) << std::endl;
	return reinterpret_cast<struct sockaddr*>( &m_addr );
}

RPW::Core::IpEndpoint::operator sockaddr_in( void ) const
{
	return m_addr;
}

unsigned short RPW::Core::IpEndpoint::getPort( void ) const
{
	// note: sin_port is stored in network order
	return ::ntohs( m_addr.sin_port );
}

unsigned long RPW::Core::IpEndpoint::getAddress( void ) const
{
	// note: sin_addr is stored in network order
	return ::ntohl( m_addr.sin_addr.S_un.S_addr );
}

std::string RPW::Core::IpEndpoint::getAddressString( unsigned long hostOrderAddress )
{
	// note: S_addr is stored in network order
	in_addr i;
	i.S_un.S_addr = ::htonl( hostOrderAddress );
	return std::string( ::inet_ntoa( i ) );
}

std::string RPW::Core::IpEndpoint::getAddressString( void ) const
{
	// note:  sin_addr is stored in network order
	return std::string( ::inet_ntoa( m_addr.sin_addr ) );
}

unsigned long RPW::Core::IpEndpoint::getAddressULong( const char* const ip )
{
	// ::inet_addr returns an address in network order
	return ::ntohl( ::inet_addr( ip ) );
}

std::string RPW::Core::IpEndpoint::getAddressString2( void ) const
{
	unsigned short a = 0;
	unsigned short b = 0;
	unsigned short c = 0;
	unsigned short d = 0;

	::sscanf_s( getAddressString().c_str(), "%d.%d.%d.%d", &a, &b, &c, &d );

	std::ostringstream oss;
	oss << std::setfill( '0' ) << std::setw( 3 ) << a;
	oss << "." << std::setfill( '0' ) << std::setw( 3 ) << b;
	oss << "." << std::setfill( '0' ) << std::setw( 3 ) << c;
	oss << "." << std::setfill( '0' ) << std::setw( 3 ) << d;

	return oss.str();
}

bool RPW::Core::IpEndpoint::getSockName( SOCKET socket )
{
	int len = sizeof( m_addr );
	return SOCKET_ERROR != ::getsockname( socket, *this, &len );
}

ADDRESS_FAMILY RPW::Core::IpEndpoint::getFamily( void ) const
{
	// note:  sin_family is stored in host order !!!
	return m_addr.sin_family;
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::zero( void )
{
	::memset( &m_addr, 0, sizeof( m_addr ) );
	return *this;
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::setPort( unsigned short hostOrderPort )
{
	// note:  sin_port is stored in network order
	m_addr.sin_port = ::htons( hostOrderPort );
	return *this;
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::setAddress( unsigned long hostOrderAddress )
{
	// note:  sin_addr is stored in network order
	m_addr.sin_addr.S_un.S_addr = ::htonl( hostOrderAddress );
	return *this;
}

RPW::Core::IpEndpoint& RPW::Core::IpEndpoint::setFamily( ADDRESS_FAMILY family )
{
	// note:  sin_family is stored in host order !!!
	m_addr.sin_family = family;
	return *this;
}
