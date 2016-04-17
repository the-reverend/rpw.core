/// @file SocketProxy.cpp
/// @author rev

#include "StdAfx.h"
#include "SocketProxy.h"

#pragma warning (push)
#pragma warning (disable : 4355) // 'this' : used in base member initializer list
RPW::SocketProxy::SocketProxy( const RPW::Core::IpEndpoint& ep )
	: m_rhsSocket( *this )
	, m_rhsEndpoint( ep )
{
}
#pragma warning (pop)

RPW::SocketProxy::~SocketProxy( void )
{
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		try
		{
			it->second.destroy();
		}
		catch ( const RPW::Core::SocketException& )
		{
			// fail silently
		}
	}
	m_lhsSockets.clear();

	try
	{
		m_rhsSocket.destroy();
	}
	catch ( const RPW::Core::SocketException& )
	{
		// fail silently
	}
}

bool RPW::SocketProxy::AcceptedSocketIsAllowed( RPW::Core::Socket& sock )
{
	// override this
	return true;
}

void RPW::SocketProxy::OnAccept( void )
{
	// do some lhs socket map maintenance.  remove any lhs sockets that are invalid.
	// this is important because the OS will reuse socket numbers over time and
	// we don't want any collisions when we go to add new sockets to the map; also
	// we don't want this map to grow uncontrolled.
	LhsSocketMap::iterator it = m_lhsSockets.begin();
	while ( it != m_lhsSockets.end() )
	{
		if ( !it->second.isValid() )
		{
			std::cout << "removing expired socket " << it->first << " from the map" << std::endl;
			// the iterator gets invalidated by erase but the ++ unary operator
			// creates a 'magical' copy in the background which is what gets passed
			// into the erase() method and goes out of scope leaving us with
			// an iterator that is still valid and incremented.  yay.
			m_lhsSockets.erase( it++ );
		}
		else
		{
			++it;
		}
	}

	SOCKET sock = INVALID_SOCKET;
	try
	{
		sock = accept();
		RPW::Core::IpEndpoint ep;
		RPW::Core::Socket s;
		s.setSocket( sock );
		s.getPeerName( ep );

		std::cout << "new lhs connection accepted from " << ep.getAddressString() << ':' << ep.getPort() << std::endl;
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << e.formatErrorMessage( RPW::Core::Socket( sock ) );
		assert( false );
		return;
	}

	if ( INVALID_SOCKET != sock )
	{
		LhsSocket lhs( *this, sock );

		if ( AcceptedSocketIsAllowed( lhs ) )
		{
			// this insert will invoke the LhsSocket copy constructor
			std::pair< LhsSocketMap::iterator, bool> p = 
				m_lhsSockets.insert( std::pair<SOCKET, LhsSocket>( lhs.getSocket(), lhs ) );

			assert( p.second );
			if ( p.second )
			{
				// the LhsSocket was copied and inserted because its internal SOCKET was unique in the map

				if ( lhs.isValid() && m_rhsSocket.isValid() && !m_rhsSocket.IsStarted() )
				{
					// start the rhs socket and connect it up
					m_rhsSocket.Start();
					m_rhsSocket.connect( m_rhsEndpoint );
					RPW::Core::IpEndpoint rep( m_rhsSocket.getSocket() );
					std::cout << "rhs connected to " << rep.getAddressString() << ':' << rep.getPort() << std::endl;
				}

				p.first->second.Start(); // start the SelectSocket thread
			}
			else
			{
				// socket was not inserted into the list, so shutdown+close it.
				lhs.destroy();
			}
		}
		else
		{
			// socket is not allowed to connect.
			lhs.destroy();
		}
	}
}

void RPW::SocketProxy::OnAddressListChange( void )
{
	std::cout << "proxy listener socket reports address list change... write some code here?" << std::endl;
}

void RPW::SocketProxy::OnClose( void )
{
	destroy();
}

void RPW::SocketProxy::OnConnect( void )
{
	std::cout << "proxy listener socket connected; why?" << std::endl;
}

void RPW::SocketProxy::OnOutOfBand( void )
{
	std::cout << "proxy listener socket reports out of band... write some code here?" << std::endl;
}

void RPW::SocketProxy::OnQualityOfService( void )
{
	std::cout << "proxy listener socket reports quality of service change... write some code here?" << std::endl;
}

void RPW::SocketProxy::OnReceive( void )
{
	std::cout << "proxy listener socket received something; why?" << std::endl;
}

void RPW::SocketProxy::OnRoutingChange( void )
{
	std::cout << "proxy listener socket reports routing change... write some code here?" << std::endl;
}

void RPW::SocketProxy::OnSend( void )
{
	std::cout << "proxy listener socket sent something; why?" << std::endl;
}

void RPW::SocketProxy::OnSocketError( unsigned long lastError )
{
	std::cout << "proxy listener socket error " << lastError << " - " << RPW::Core::SocketException::getShortErrorText( lastError ) << std::endl;
}

void RPW::SocketProxy::OnWaitFailed( void )
{
	std::cout << "proxy listener wait failed" << std::endl;
}

void RPW::SocketProxy::OnStop( void )
{
	std::cout << "proxy listener stopped" << std::endl;
}

void RPW::SocketProxy::OnKeepAlive( void )
{
	std::cout << "proxy listener keep alive" << std::endl;
}

RPW::Core::SelectSocket& RPW::SocketProxy::Rhs( void )
{
	return m_rhsSocket;
}

RPW::Core::SelectSocket* RPW::SocketProxy::Lhs( SOCKET sock )
{
	RPW::Core::SelectSocket* pSock = NULL;

	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		if ( it->second.getSocket() == sock )
		{
			pSock = &it->second;
			break;
		}
	}

	return pSock;
}

RPW::SocketProxy::SocketSet RPW::SocketProxy::LhsByPeer( unsigned long ip )
{
	RPW::SocketProxy::SocketSet list;

	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		if ( it->second.getPeerIp() == ip )
		{
			list.insert( &it->second );
		}
	}

	return list;
}

RPW::SocketProxy::SocketSet RPW::SocketProxy::LhsByIp( unsigned long ip )
{
	RPW::SocketProxy::SocketSet list;

	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		if ( it->second.getIp() == ip )
		{
			list.insert( &it->second );
		}
	}

	return list;
}

void RPW::SocketProxy::LhsSendToAll( const char* const data, int len )
{
	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		if ( it->second.isValid() )
		{
			try
			{
				int bytesSent = LhsSend( it->second, data, len );
				assert( bytesSent == len );
			}
			catch ( const RPW::Core::SocketException& e )
			{
				std::cout << "SendToAllLhs - send - " << e.formatErrorMessage( it->second ) << std::endl;

				try
				{
					it ->second.destroy();
				}
				catch ( const RPW::Core::SocketException& ee )
				{
					std::cout << "SendToAllLhs - destroy - " << ee.formatErrorMessage( it->second ) << std::endl;
					assert( false );
				}
			}
		}
	}
}

int RPW::SocketProxy::LhsSend( LhsSocket& sock, const char* const data, int len )
{
	return sock.send( data, len );
}

void RPW::SocketProxy::LhsCloseAll( void )
{
	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		it->second.destroy();
	}
}

void RPW::SocketProxy::LhsStopAll( void )
{
	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		it->second.Stop();
	}
}

void RPW::SocketProxy::RhsOnAccept( void )
{
	assert( false );
	throw std::logic_error( "RhsOnAccept" );
}

void RPW::SocketProxy::RhsOnAddressListChange( void )
{
	std::cout << "rhs socket reports address list change... write some code here?" << std::endl;
}

void RPW::SocketProxy::RhsOnClose( void )
{
	m_rhsSocket.destroy();
}

void RPW::SocketProxy::RhsOnConnect( void )
{
	// nop
}
void RPW::SocketProxy::RhsOnOutOfBand( void )
{
	std::cout << "rhs socket reports out of band... write some code here?" << std::endl;
}

void RPW::SocketProxy::RhsOnQualityOfService( void )
{
	std::cout << "rhs socket reports quality of service change... write some code here?" << std::endl;
}

void RPW::SocketProxy::RhsOnReceive( const char* const data, int len )
{
	LhsSendToAll( data, len );
}

void RPW::SocketProxy::RhsOnRoutingChange( void )
{
	std::cout << "rhs socket reports routing change... write some code here?" << std::endl;
}

void RPW::SocketProxy::RhsOnSend( void )
{
	// nop
}

void RPW::SocketProxy::RhsOnSocketError( unsigned long lastError )
{
	std::cout << "rhs socket - socket error " << lastError << " - " << RPW::Core::SocketException::getLongErrorText( lastError ) << std::endl;
}

void RPW::SocketProxy::RhsOnWaitFailed( void )
{
	std::cout << "rhs socket wait failed" << std::endl;
}

void RPW::SocketProxy::RhsOnStop( void )
{
	std::cout << "rhs socket stopped" << std::endl;
}

void RPW::SocketProxy::RhsOnKeepAlive( void )
{
	std::cout << "rhs keep alive" << std::endl;
}

void RPW::SocketProxy::LhsOnAccept( LhsSocket& sock )
{
	assert( false ); throw std::logic_error( "LhsOnAccept" );
}

void RPW::SocketProxy::LhsOnAddressListChange( LhsSocket& sock )
{
	std::cout << "lhs socket " << ( SOCKET ) sock << " reports address list change... write some code here?" << std::endl;
}

void RPW::SocketProxy::LhsOnClose( LhsSocket& sock )
{
	sock.destroy();

	bool allDisconnected = true;

	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( LhsSocketMap::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		if ( it->second.isValid() )
		{
			allDisconnected = false;
			break;
		}
	}
	scope.leave();

	if ( allDisconnected )
	{
		LhsOnCloseAll();
	}
}

void RPW::SocketProxy::LhsOnConnect( LhsSocket& sock )
{
	// nop
}

void RPW::SocketProxy::LhsOnOutOfBand( LhsSocket& sock )
{
	std::cout << "lhs socket " << ( SOCKET ) sock << " reports out of band... write some code here?" << std::endl;
}

void RPW::SocketProxy::LhsOnQualityOfService( LhsSocket& sock )
{
	std::cout << "lhs socket " << ( SOCKET ) sock << " reports quality of service change... write some code here?" << std::endl;
}

void RPW::SocketProxy::LhsOnReceive( LhsSocket& sock, const char* const data, int len )
{
	try
	{
		m_rhsSocket.send( data, len );
	}
	catch ( RPW::Core::SocketException& e )
	{
		std::cout << e.formatErrorMessage( sock );
	}
}

void RPW::SocketProxy::LhsOnRoutingChange( LhsSocket& sock )
{
	std::cout << "lhs socket " << ( SOCKET ) sock << " reports routing change... write some code here?" << std::endl;
}
void RPW::SocketProxy::LhsOnSend( LhsSocket& sock )
{
	// nop
}

void RPW::SocketProxy::LhsOnSocketError( LhsSocket& sock, unsigned long lastError )
{
	std::cout << "lhs socket error " << lastError << " - " << RPW::Core::SocketException::getShortErrorText( lastError ) << std::endl;
}

void RPW::SocketProxy::LhsOnWaitFailed( LhsSocket& sock )
{
	// override this
}

void RPW::SocketProxy::LhsOnStop( LhsSocket& sock )
{
	// override this
}

void RPW::SocketProxy::LhsOnCloseAll( void )
{
	// override this
}

void RPW::SocketProxy::LhsOnKeepAlive( LhsSocket& sock )
{
	// override this
}
