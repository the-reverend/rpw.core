/// @file WsaEventHandle.cpp
/// @author rev

#include "StdAfx.h"
#include "WsaEventHandle.h"

RPW::Core::WsaEventHandle::WsaEventHandle( void )
	: m_handle( ::WSACreateEvent() )
{
}

RPW::Core::WsaEventHandle::~WsaEventHandle( void )
{
	::WSACloseEvent( m_handle );
}

RPW::Core::WsaEventHandle::operator WSAEVENT( void ) const
{
	return m_handle;
}

int RPW::Core::WsaEventHandle::select( SOCKET sock, long events )
{
	return ::WSAEventSelect( sock, m_handle, events );
}

int RPW::Core::WsaEventHandle::enumEvents( SOCKET sock, LPWSANETWORKEVENTS lpNetworkEvents )
{
	return ::WSAEnumNetworkEvents( sock, m_handle, lpNetworkEvents );
}

DWORD RPW::Core::WsaEventHandle::waitFor( DWORD dwMilliseconds /*= INFINITE*/ )
{
	return ::WSAWaitForMultipleEvents( 1, &m_handle, FALSE, dwMilliseconds, FALSE );
}
