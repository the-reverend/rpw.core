#include "StdAfx.h"
#include "SelectSocket.h"
#include "SocketException.h"

#include <iostream>
#include <assert.h>
#include <stdexcept>

RPW::Core::SelectSocket::SelectSocket( SOCKET sock /*= INVALID_SOCKET*/ )
	: RPW::Core::Socket( sock )
	, m_pThread( NULL )
	, m_threadTimeout( THREAD_TIMEOUT )
	, m_hKeepAliveTimer( ::CreateWaitableTimerA( NULL, FALSE, NULL ) )
	, m_dwKeepAliveTimeoutMs( 0 )
{
	m_liKeepAliveTimeout.QuadPart = 0LL;
}

RPW::Core::SelectSocket::SelectSocket( const SelectSocket& sock )
	: RPW::Core::Socket( sock )
	, m_pThread( NULL )
	, m_threadTimeout( THREAD_TIMEOUT )
	, m_hKeepAliveTimer( ::CreateWaitableTimerA( NULL, FALSE, NULL ) )
	, m_dwKeepAliveTimeoutMs( sock.m_dwKeepAliveTimeoutMs )
	, m_liKeepAliveTimeout( sock.m_liKeepAliveTimeout )
{
	assert( !sock.IsStarted() );
	if ( sock.IsStarted() )
	{
		throw std::logic_error( "don't copy SelectSocket objects while the threadproc is running" );
	}
}

RPW::Core::SelectSocket::~SelectSocket( void )
{
	::CloseHandle( m_hKeepAliveTimer );
}

void RPW::Core::SelectSocket::Start( DWORD timeout /*= THREAD_TIMEOUT*/ )
{
	Stop(); // Ensure the thread isn't already running
	m_threadTimeout = timeout;
	m_pThread = new RPW::Core::SimpleThread<SelectSocket>( *this, &SelectSocket::ThreadProc );
	m_pThread->start();
}

bool RPW::Core::SelectSocket::IsStarted( void ) const
{
	return m_pThread != NULL;
}

void RPW::Core::SelectSocket::Stop( void )
{
	// Terminates the event thread gracefully (if possible)

	if ( NULL != m_pThread )
	{
		m_pThread->setPriority( THREAD_PRIORITY_HIGHEST );
		m_pThread->stop();

		while ( WAIT_TIMEOUT == m_pThread->thread_proc_done_event().waitFor( m_threadTimeout * 10 ) )
		{
			// Note: An excessive delay in processing any of the notification functions
			// can cause us to get here. (Yes one second is an excessive delay. Its a bug!)
			assert( false );
		}

		delete m_pThread;
		m_pThread = NULL;
	}
}

void RPW::Core::SelectSocket::SetupKeepAlive( DWORD keepalivetimeoutseconds /*= 0*/ )
{
	m_dwKeepAliveTimeoutMs = keepalivetimeoutseconds * 1000;
	m_liKeepAliveTimeout.QuadPart = - ( static_cast<LONGLONG>( m_dwKeepAliveTimeoutMs ) * 10000LL );
}

void RPW::Core::SelectSocket::ResetWaitableTimer()
{
	if ( m_dwKeepAliveTimeoutMs == 0 )
	{
		::CancelWaitableTimer( m_hKeepAliveTimer );
	}
	else
	{
		::SetWaitableTimer( m_hKeepAliveTimer, &m_liKeepAliveTimeout, m_dwKeepAliveTimeoutMs, NULL, NULL, FALSE );
	}
}

int RPW::Core::SelectSocket::send( __in const char* buffer, __in int len, __in int flags /*= 0*/ )
{
	ResetWaitableTimer();
	return RPW::Core::Socket::send( buffer, len, flags );
}

int RPW::Core::SelectSocket::send( __in char c, __in int flags /*= 0*/ )
{
	return RPW::Core::Socket::send( c, flags );
}

int RPW::Core::SelectSocket::send( __in unsigned char c, __in int flags /*= 0*/ )
{
	return RPW::Core::Socket::send( c, flags );
}

int RPW::Core::SelectSocket::send( __in const unsigned char* buffer, __in int len, __in int flags /*= 0*/ )
{
	return RPW::Core::Socket::send( buffer, len, flags );
}

int RPW::Core::SelectSocket::send( __in const std::string& buffer, __in int flags /*= 0*/ )
{
	return RPW::Core::Socket::send( buffer, flags );
}

int RPW::Core::SelectSocket::send( __in const RPW::Core::ustring& buffer, __in int flags /*= 0*/ )
{
	return RPW::Core::Socket::send( buffer, flags );
}

int RPW::Core::SelectSocket::sendTo( __in const char* buffer, __in int len, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	ResetWaitableTimer();
	return RPW::Core::Socket::sendTo( buffer, len, flags, to, tolen );
}

int RPW::Core::SelectSocket::sendTo( __in char c, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	return RPW::Core::Socket::sendTo( c, flags, to, tolen );
}

int RPW::Core::SelectSocket::sendTo( __in unsigned char c, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	return RPW::Core::Socket::sendTo( c, flags, to, tolen );
}

int RPW::Core::SelectSocket::sendTo( __in const unsigned char* buffer, __in int len, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	return RPW::Core::Socket::sendTo( buffer, len, flags, to, tolen );
}

int RPW::Core::SelectSocket::sendTo( __in const std::string& buffer, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	return RPW::Core::Socket::sendTo( buffer, flags, to, tolen );
}

int RPW::Core::SelectSocket::sendTo( __in const RPW::Core::ustring& buffer, __in int flags /*= 0*/, __in const struct sockaddr* to /*= 0*/, __in int tolen /*= 0*/ )
{
	return RPW::Core::Socket::sendTo( buffer, flags, to, tolen );
}

void RPW::Core::SelectSocket::ThreadProc( void )
{
	// These are the possible network event notifications:
	// FD_READ  Notification of readiness for reading.
	// FD_WRITE  Motification of readiness for writing.
	// FD_OOB   Notification of the arrival of Out Of Band data.
	// FD_ACCEPT  Notification of incoming connections.
	// FD_CONNECT  Notification of completed connection or multipoint join operation.
	// FD_CLOSE  Notification of socket closure.
	// FD_QOS  Notification of socket Quality Of Service changes
	// FD_ROUTING_INTERFACE_CHANGE Notification of routing interface changes for the specified destination.
	// FD_ADDRESS_LIST_CHANGE  Notification of local address list changes for the address family of the socket.

	RPW::Core::WsaEventHandle wsaEvent;
	bool done = false;
	long Events = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE |
	              FD_QOS | FD_ROUTING_INTERFACE_CHANGE | FD_ADDRESS_LIST_CHANGE;

	if ( SOCKET_ERROR == wsaEvent.select( *this, Events ) )
	{
		done = true;
	}

	DWORD keepalivetimeoutms = 0;

	std::vector<HANDLE> ha;
	ha.push_back( wsaEvent );
	ha.push_back( m_pThread->terminate_event() );
	ha.push_back( m_hKeepAliveTimer );

	// loop until the stop event is set
	while ( !done )
	{
		if ( keepalivetimeoutms != m_dwKeepAliveTimeoutMs )
		{
			// keepalive settings have changed
			keepalivetimeoutms = m_dwKeepAliveTimeoutMs;

			ResetWaitableTimer();
		}

		// Wait 100 ms for a network event
		DWORD dwResult = ::WSAWaitForMultipleEvents( static_cast<DWORD>( ha.size() ), &ha[0], FALSE, m_threadTimeout, FALSE );

		WSANETWORKEVENTS NetworkEvents;
		switch ( dwResult )
		{
			case WSA_WAIT_TIMEOUT:
				// nop
				break;

			case WSA_WAIT_FAILED:
				OnWaitFailed();
				done = true;
				break;

			case WAIT_OBJECT_0: // socket event
				if ( SOCKET_ERROR == wsaEvent.enumEvents( *this, &NetworkEvents ) )
				{
					OnSocketError( ::GetLastError() );
					done = true;
					break;
				}

				if ( NetworkEvents.lNetworkEvents & FD_ACCEPT )
				{
					OnAccept();
				}

				if ( NetworkEvents.lNetworkEvents & FD_READ )
				{
					OnReceive();
				}

				if ( NetworkEvents.lNetworkEvents & FD_WRITE )
				{
					OnSend();
				}

				if ( NetworkEvents.lNetworkEvents & FD_OOB )
				{
					OnOutOfBand();
				}

				if ( NetworkEvents.lNetworkEvents & FD_QOS )
				{
					OnQualityOfService();
				}

				if ( NetworkEvents.lNetworkEvents & FD_CONNECT )
				{
					OnConnect();
				}

				if ( NetworkEvents.lNetworkEvents & FD_ROUTING_INTERFACE_CHANGE )
				{
					OnRoutingChange();
				}

				if ( NetworkEvents.lNetworkEvents & FD_ADDRESS_LIST_CHANGE )
				{
					OnAddressListChange();
				}

				if ( NetworkEvents.lNetworkEvents & FD_CLOSE )
				{
					OnClose();
					done = true;
				}
				break;

			case WAIT_OBJECT_0 + 1: // stop request
				OnStop();
				done = true;
				break;

			case WAIT_OBJECT_0 + 2: // keepalive timer event
				OnKeepAlive();
				break;

			default:
				/// @todo figure out what to do here
				break;
		}
	}

	::CancelWaitableTimer( m_hKeepAliveTimer );

	if ( isValid() )
	{
		wsaEvent.select( *this, 0 );
	}
}
