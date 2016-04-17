/// @file RhsSocket.cpp
/// @author rev

#include "StdAfx.h"
#include "SocketProxy.h"

RPW::SocketProxy::RhsSocket::RhsSocket( SocketProxy& proxy, SOCKET sock /*= INVALID_SOCKET*/ )
	: RPW::Core::SelectSocket( sock )
	, m_pProxy( &proxy )
{
}

RPW::SocketProxy::RhsSocket::RhsSocket( const RhsSocket& sock )
	: RPW::Core::SelectSocket( sock )
	, m_pProxy( sock.m_pProxy )
{
}

RPW::SocketProxy::RhsSocket::~RhsSocket( void )
{
}

void RPW::SocketProxy::RhsSocket::OnAccept( void )
{
	m_pProxy->RhsOnAccept();
}

void RPW::SocketProxy::RhsSocket::OnAddressListChange( void )
{
	m_pProxy->RhsOnAddressListChange();
}

void RPW::SocketProxy::RhsSocket::OnClose( void )
{
	m_pProxy->RhsOnClose();
}

void RPW::SocketProxy::RhsSocket::OnConnect( void )
{
	m_pProxy->RhsOnConnect();
}

void RPW::SocketProxy::RhsSocket::OnOutOfBand( void )
{
	m_pProxy->RhsOnOutOfBand();
}

void RPW::SocketProxy::RhsSocket::OnQualityOfService( void )
{
	m_pProxy->RhsOnQualityOfService();
}

void RPW::SocketProxy::RhsSocket::OnReceive( void )
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

	m_pProxy->RhsOnReceive( &data.front(), len );
}

void RPW::SocketProxy::RhsSocket::OnRoutingChange( void )
{
	m_pProxy->RhsOnRoutingChange();
}

void RPW::SocketProxy::RhsSocket::OnSend( void )
{
	m_pProxy->RhsOnSend();
}

void RPW::SocketProxy::RhsSocket::OnSocketError( unsigned long lastError )
{
	m_pProxy->RhsOnSocketError( lastError );
}

void RPW::SocketProxy::RhsSocket::OnWaitFailed( void )
{
	m_pProxy->RhsOnWaitFailed();
}

void RPW::SocketProxy::RhsSocket::OnStop( void )
{
	m_pProxy->RhsOnStop();
}

void RPW::SocketProxy::RhsSocket::OnKeepAlive( void )
{
	m_pProxy->RhsOnKeepAlive();
}
