/// @file Socket.cpp
/// @author rev

#include "StdAfx.h"
#include "Socket.h"

#include "SocketException.h"

#include <assert.h>
#include <iostream>

RPW::Core::Socket::Socket( SOCKET sock )
	: m_socket( sock )
{
}

RPW::Core::Socket::Socket( const RPW::Core::Socket& sock )
	: m_socket( sock.m_socket )
{
}

RPW::Core::Socket::~Socket( void )
{
}

RPW::Core::Socket::operator SOCKET( void ) const
{
	return m_socket;
}

RPW::Core::Socket& RPW::Core::Socket::operator=( const RPW::Core::Socket & rhs )
{
	if ( this != &rhs )
	{
		m_socket = rhs.m_socket;
	}

	return *this;
}

RPW::Core::Socket& RPW::Core::Socket::operator=( const SOCKET & rhs )
{
	if ( &this->m_socket != &rhs )
	{
		m_socket = rhs;
	}

	return *this;
}

bool RPW::Core::Socket::operator==( const Socket& rhs ) const
{
	return this == &rhs || m_socket == rhs.m_socket;
}

bool RPW::Core::Socket::operator!=( const Socket& rhs ) const
{
	return !( *this == rhs );
}

bool RPW::Core::Socket::operator<( const Socket& rhs ) const
{
	return this != &rhs && m_socket < rhs.m_socket;
}

bool RPW::Core::Socket::operator<=( const Socket& rhs ) const
{
	return *this < rhs || *this == rhs;
}

bool RPW::Core::Socket::operator>( const Socket& rhs ) const
{
	return !( *this <= rhs );
}

bool RPW::Core::Socket::operator>=( const Socket& rhs ) const
{
	return !( *this < rhs );
}

void RPW::Core::Socket::setSocket( SOCKET sock )
{
	m_socket = sock;
}

SOCKET RPW::Core::Socket::getSocket( void ) const
{
	return m_socket;
}

unsigned long RPW::Core::Socket::getIp( void )
{
	RPW::Core::IpEndpoint ep;
	this->getPeerName( ep );
	return ep.getAddress();
}

unsigned long RPW::Core::Socket::getPeerIp( void )
{
	return RPW::Core::IpEndpoint( *this ).getAddress();
}

void RPW::Core::Socket::destroy( void )
throw( RPW::Core::SocketException )
{
	shutdown( SD_BOTH );
	close();
}

bool RPW::Core::Socket::isValid( void ) const
{
	return INVALID_SOCKET != m_socket;
}

bool RPW::Core::Socket::isConnected( void )
throw( RPW::Core::SocketException )
{
	if ( !isValid() )
	{
		return false;
	}

	DWORD optVal = 0;
	int optLen = sizeof( DWORD );

	bool success = handleSocketSuccess( ::getsockopt( m_socket, SOL_SOCKET, 0x700C /*SO_CONNECT_TIME*/, ( char* ) & optVal, &optLen ) );

	return success && optVal < UINT_MAX;
}

SOCKET RPW::Core::Socket::strip( void )
{
	SOCKET sock = m_socket;
	m_socket = NULL;
	return sock;
}

SOCKET RPW::Core::Socket::accept( __out struct sockaddr* addr, __inout int* addrlen )
throw( RPW::Core::SocketException )
{
	SOCKET acceptedSocket = ::accept( m_socket, addr, addrlen );
	handleSocketSuccess( INVALID_SOCKET == acceptedSocket ? SOCKET_ERROR : 0 );
	return acceptedSocket;
}

SOCKET RPW::Core::Socket::accept( __out RPW::Core::IpEndpoint& ep )
throw( RPW::Core::SocketException )
{
	int len = sizeof( struct sockaddr );
	return this->accept( ep, &len );
}

SOCKET RPW::Core::Socket::accept( void )
throw( RPW::Core::SocketException )
{
	return this->accept( NULL, NULL );
}

bool RPW::Core::Socket::bind( __in const struct sockaddr* name, __in int namelen )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::bind( m_socket, name, namelen ) );
}

bool RPW::Core::Socket::bind( __in IpEndpoint& name )
throw( RPW::Core::SocketException )
{
	return this->bind( name, sizeof( struct sockaddr ) );
}

bool RPW::Core::Socket::close( void )
throw( RPW::Core::SocketException )
{
	bool success = isValid();

	if ( success )
	{
		success = handleSocketSuccess( ::closesocket( m_socket ) );
	}

	if ( success )
	{
		m_socket = INVALID_SOCKET;
	}

	return success;
}

bool RPW::Core::Socket::connect( __in const struct sockaddr* name, __in int namelen )
throw( RPW::Core::SocketException )
{
	int r = ::connect( m_socket, name, namelen );
	return handleSocketSuccess( r );
}

bool RPW::Core::Socket::connect( __in RPW::Core::IpEndpoint& ep )
throw( RPW::Core::SocketException )
{
	return connect( ep, sizeof( struct sockaddr ) );
}

bool RPW::Core::Socket::create( __in int af, __in int type, __in int protocol )
throw( RPW::Core::SocketException )
{
	destroy();
	m_socket = ::socket( af, type, protocol );

	return isValid();
}

bool RPW::Core::Socket::create( __in int af, __in int type, __in int protocol, __in LPWSAPROTOCOL_INFOA lpProtocolInfo, __in GROUP g, __in DWORD dwFlags )
throw( RPW::Core::SocketException )
{
	destroy();
	m_socket = ::WSASocketA( af, type, protocol, lpProtocolInfo, g, dwFlags );

	return isValid();
}

bool RPW::Core::Socket::getPeerName( __out struct sockaddr* name, __inout int* namelen )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::getpeername( m_socket, name, namelen ) );
}

bool RPW::Core::Socket::getPeerName( __out RPW::Core::IpEndpoint& name )
throw( RPW::Core::SocketException )
{
	int len = sizeof( struct sockaddr_in );
	return this->getPeerName( name, &len );
}

bool RPW::Core::Socket::getSocketName( __out struct sockaddr* name, __inout int* namelen )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::getsockname( m_socket, name, namelen ) );
}

bool RPW::Core::Socket::getSocketName( __out RPW::Core::IpEndpoint& name )
throw( RPW::Core::SocketException )
{
	int len = sizeof( struct sockaddr );
	return this->getSocketName( name, &len );
}

bool RPW::Core::Socket::getSocketOption( __in int level, __in int optname, __out char* optval, __inout int* optlen )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::getsockopt( m_socket, level, optname, optval, optlen ) );
}

bool RPW::Core::Socket::ioctlSocket( __in long cmd, __inout u_long* argp )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::ioctlsocket( m_socket, cmd, argp ) );
}

bool RPW::Core::Socket::listen( __in int backlog )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::listen( m_socket, backlog ) );
}

int RPW::Core::Socket::receive( __out char* buffer, __in int len, __in int flags )
throw( RPW::Core::SocketException )
{
	int ret = ::recv( m_socket, buffer, len, flags );
	handleSocketSuccess( ret );
	return ret;
}

int RPW::Core::Socket::receive( __out unsigned char* buffer, __in int len, __in int flags )
throw( RPW::Core::SocketException )
{
	return receive( reinterpret_cast<char*>( buffer ), len, flags );
}

int RPW::Core::Socket::receiveFrom( __out char* buffer, __in int len, __in int flags, __out struct sockaddr* from, __inout_opt int* fromlen )
throw( RPW::Core::SocketException )
{
	int ret = 0;
	if ( len > 0 )
	{
		ret = ::recvfrom( m_socket, buffer, len, flags, from, fromlen );
		handleSocketSuccess( ret );
	}

	return ret;
}

int RPW::Core::Socket::receiveFrom( __out unsigned char* buffer, __in int len, __in int flags, __out struct sockaddr* from, __inout_opt int* fromlen )
throw( RPW::Core::SocketException )
{
	return receiveFrom( reinterpret_cast<char*>( buffer ), len, flags, from, fromlen );
}

int RPW::Core::Socket::send( __in const char* buffer, __in int len, __in int flags )
throw( RPW::Core::SocketException )
{
	int pos = 0;

	// 1. only actually send if len > 0
	// 2. keep sending until the entire buffer is sent
	while ( len > 0 && pos < len )
	{
		// this is a blocking socket call
		int r = ::send( m_socket, buffer + pos, len - pos, flags );
		handleSocketSuccess( r );

		assert( r != 0 ); // if r is zero, we could get stuck in here infinitely
		if ( r > 0 )
		{
			// increment the buffer position pointer
			pos += r;
		}
	}

	return pos;
}

int RPW::Core::Socket::send( __in char c, __in int flags )
throw( RPW::Core::SocketException )
{
	return send( &c, 1, flags );
}

int RPW::Core::Socket::send( __in unsigned char c, __in int flags )
throw( RPW::Core::SocketException )
{
	return send( reinterpret_cast<const char*>( &c ), 1, flags );
}

int RPW::Core::Socket::send( __in const unsigned char* buffer, __in int len, __in int flags )
throw( RPW::Core::SocketException )
{
	return send( reinterpret_cast<const char*>( buffer ), len, flags );
}

int RPW::Core::Socket::send( __in const std::string& buffer, __in int flags )
throw( RPW::Core::SocketException )
{
	return send( buffer.c_str(), buffer.length(), flags );
}

int RPW::Core::Socket::send( __in const RPW::Core::ustring& buffer, __in int flags )
throw( RPW::Core::SocketException )
{
	return send( buffer.c_str(), buffer.length(), flags );
}

int RPW::Core::Socket::sendTo( __in const char* buffer, __in int len, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	int pos = 0;

	// 1. only actually send if len > 0
	// 2. keep sending until the entire buffer is sent
	while ( len > 0 && pos < len )
	{
		// this is a blocking socket call
		int r = ::sendto( m_socket, buffer + pos, len - pos, flags, to, tolen );
		handleSocketSuccess( r );

		assert( r != 0 ); // if r is zero, we could get stuck in here infinitely
		if ( r > 0 )
		{
			// increment the buffer position pointer
			pos += r;
		}
	}

	return pos;
}

int RPW::Core::Socket::sendTo( __in char c, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	return sendTo( &c, 1, flags, to, tolen );
}

int RPW::Core::Socket::sendTo( __in unsigned char c, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	return sendTo( reinterpret_cast<const char*>( &c ), 1, flags, to, tolen );
}

int RPW::Core::Socket::sendTo( __in const unsigned char* buffer, __in int len, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	return sendTo( reinterpret_cast<const char*>( buffer ), len, flags, to, tolen );
}

int RPW::Core::Socket::sendTo( __in const std::string& buffer, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	return sendTo( buffer.c_str(), static_cast<int>( buffer.length() ), flags, to, tolen );
}

int RPW::Core::Socket::sendTo( __in const RPW::Core::ustring& buffer, __in int flags, __in const struct sockaddr* to, __in int tolen )
throw( RPW::Core::SocketException )
{
	return sendTo( buffer.c_str(), static_cast<int>( buffer.length() ), flags, to, tolen );
}

bool RPW::Core::Socket::setSocketOption( __in int level, __in int optname, __in const char* optval, __in int optlen )
throw( RPW::Core::SocketException )
{
	return handleSocketSuccess( ::setsockopt( m_socket, level, optname, optval, optlen ) );
}

bool RPW::Core::Socket::shutdown( __in int how )
throw( RPW::Core::SocketException )
{
	bool success = isValid();

	if ( success )
	{
		std::vector<int> f;
		f.push_back( WSAENOTCONN );
		success = handleSocketSuccess( ::shutdown( m_socket, how ), &f );
	}

	return success;
}

/// @throws RPW::Core::SocketException
bool RPW::Core::Socket::handleSocketSuccess( int socketError, const std::vector<int>* forgive )
throw( RPW::Core::SocketException )
{
	bool success = true;

	if ( SOCKET_ERROR == socketError )
	{
		int wsalasterror = ::WSAGetLastError();

		bool ignore = false;
		if ( NULL != forgive )
		{
			for ( std::vector<int>::const_iterator it = forgive->begin(); it != forgive->end(); ++it )
			{
				if (( *it ) == wsalasterror )
				{
					ignore = true;
					break;
				}
			}
		}

		if ( !ignore )
		{
			success = false;
			throw RPW::Core::SocketException( wsalasterror );
		}
	}

	return success;
}
