/// @file LhsSocket.cpp
/// @author rev

#include "StdAfx.h"
#include "SocketProxy.h"

RPW::SocketProxy::LhsSocket::LhsSocket( RPW::SocketProxy& proxy, SOCKET sock /*= INVALID_SOCKET*/ )
	: RPW::Core::SelectSocket( sock )
	, m_pProxy( &proxy )
{
}

RPW::SocketProxy::LhsSocket::LhsSocket( const RPW::SocketProxy::LhsSocket& sock )
	: RPW::Core::SelectSocket( sock )
	, m_pProxy( sock.m_pProxy )
{
}

RPW::SocketProxy::LhsSocket::~LhsSocket( void )
{
}

void RPW::SocketProxy::LhsSocket::OnAccept( void )
{
	m_pProxy->LhsOnAccept( *this );
}

void RPW::SocketProxy::LhsSocket::OnAddressListChange( void )
{
	m_pProxy->LhsOnAddressListChange( *this );
}

void RPW::SocketProxy::LhsSocket::OnClose( void )
{
	m_pProxy->LhsOnClose( *this );
}

void RPW::SocketProxy::LhsSocket::OnConnect( void )
{
	m_pProxy->LhsOnConnect( *this );
}

void RPW::SocketProxy::LhsSocket::OnOutOfBand( void )
{
	m_pProxy->LhsOnOutOfBand( *this );
}

void RPW::SocketProxy::LhsSocket::OnQualityOfService( void )
{
	m_pProxy->LhsOnQualityOfService( *this );
}

void RPW::SocketProxy::LhsSocket::OnReceive( void )
{
	std::vector<char> data( PAGE_SIZE, '\0' );
	int len = 0;

	try
	{
		len = receive( &data.front(), PAGE_SIZE );
	}
	catch ( const RPW::Core::SocketException& )
	{
		destroy();
	}

	m_pProxy->LhsOnReceive( *this, &data.front(), len );
}

void RPW::SocketProxy::LhsSocket::OnRoutingChange( void )
{
	m_pProxy->LhsOnRoutingChange( *this );
}

void RPW::SocketProxy::LhsSocket::OnSend( void )
{
	m_pProxy->LhsOnSend( *this );
}

void RPW::SocketProxy::LhsSocket::OnSocketError( unsigned long lastError )
{
	m_pProxy->LhsOnSocketError( *this, lastError );
}

void RPW::SocketProxy::LhsSocket::OnWaitFailed( void )
{
	m_pProxy->LhsOnWaitFailed( *this );
}

void RPW::SocketProxy::LhsSocket::OnStop( void )
{
	m_pProxy->LhsOnStop( *this );
}

void RPW::SocketProxy::LhsSocket::OnKeepAlive( void )
{
	m_pProxy->LhsOnKeepAlive( *this );
}

RPW::SocketProxy::LhsSocket& RPW::SocketProxy::LhsSocket::operator=( const RPW::SocketProxy::LhsSocket & rhs )
{
	if ( this != &rhs )
	{
		RPW::Core::SelectSocket::operator=( rhs );
		m_pProxy = rhs.m_pProxy;
	}

	return *this;
}
