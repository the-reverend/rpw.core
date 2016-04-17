/// @file SocketProxy.h
/// @author rev

#pragma once

#include "CriticalSection.h"
#include "SocketException.h"
#include "SelectSocket.h"
#include "SimpleThread.h"

#include <map>
#include <set>
#include <assert.h>
#include <iostream>

#define PAGE_SIZE 4096

namespace RPW
{
	/// @brief allow multiple local endpoints to connect simultaneously to a remote endpoint for full eavesdropping
	/// @verbatim
	//  .-----.
	//  | LHS |-.             .-----.
	//  `-----' |-. <==TCP==> | RHS |
	//    `-----' |           `-----'
	//      `-----'
	/// @endverbatim
	class SocketProxy : public RPW::Core::SelectSocket
	{
	protected:
		class RhsSocket : public RPW::Core::SelectSocket
		{
		public:
			RhsSocket( SocketProxy& proxy, SOCKET sock = INVALID_SOCKET );
			RhsSocket( const RhsSocket& sock );
			virtual ~RhsSocket( void );

#pragma region SelectSocket overrides
			virtual void OnAccept( void );
			virtual void OnAddressListChange( void );
			virtual void OnClose( void );
			virtual void OnConnect( void );
			virtual void OnOutOfBand( void );
			virtual void OnQualityOfService( void );
			virtual void OnReceive( void );
			virtual void OnRoutingChange( void );
			virtual void OnSend( void );
			virtual void OnSocketError( unsigned long lastError );
			virtual void OnWaitFailed( void );
			virtual void OnStop( void );
			virtual void OnKeepAlive( void );
#pragma endregion

		protected:
			SocketProxy* m_pProxy;
		};

		class LhsSocket : public RPW::Core::SelectSocket
		{
		public:
			LhsSocket( SocketProxy& proxy, SOCKET sock = INVALID_SOCKET );
			LhsSocket( const LhsSocket& sock );
			virtual ~LhsSocket( void );

#pragma region SelectSocket overrides
			virtual void OnAccept( void );
			virtual void OnAddressListChange( void );
			virtual void OnClose( void );
			virtual void OnConnect( void );
			virtual void OnOutOfBand( void );
			virtual void OnQualityOfService( void );
			virtual void OnReceive( void );
			virtual void OnRoutingChange( void );
			virtual void OnSend( void );
			virtual void OnSocketError( unsigned long lastError );
			virtual void OnWaitFailed( void );
			virtual void OnStop( void );
			virtual void OnKeepAlive( void );
#pragma endregion

			LhsSocket& operator=( const LhsSocket & rhs );

		protected:
			SocketProxy* m_pProxy;
			bool m_bReadyToRemove;
		};

	public:
		typedef std::map<SOCKET, LhsSocket> LhsSocketMap;
		typedef std::set<RPW::Core::Socket*> SocketSet;

		SocketProxy( const RPW::Core::IpEndpoint& ep );
		virtual ~SocketProxy( void );

		virtual bool AcceptedSocketIsAllowed( RPW::Core::Socket& sock );

#pragma region SelectSocket overrides
		virtual void OnAccept( void );
		virtual void OnAddressListChange( void );
		virtual void OnClose( void );
		virtual void OnConnect( void );
		virtual void OnOutOfBand( void );
		virtual void OnQualityOfService( void );
		virtual void OnReceive( void );
		virtual void OnRoutingChange( void );
		virtual void OnSend( void );
		virtual void OnSocketError( unsigned long lastError );
		virtual void OnWaitFailed( void );
		virtual void OnStop( void );
		virtual void OnKeepAlive( void );
#pragma endregion

		RPW::Core::SelectSocket& Rhs( void );
		RPW::Core::SelectSocket* Lhs( SOCKET sock );

		SocketSet LhsByPeer( unsigned long ip );
		SocketSet LhsByIp( unsigned long ip );
		virtual void LhsSendToAll( const char* const data, int len );
		virtual int LhsSend( LhsSocket& sock, const char* const data, int len );
		virtual void LhsCloseAll( void );
		virtual void LhsStopAll( void );

	protected:
		virtual void RhsOnAccept( void );
		virtual void RhsOnAddressListChange( void );
		virtual void RhsOnClose( void );
		virtual void RhsOnConnect( void );
		virtual void RhsOnOutOfBand( void );
		virtual void RhsOnQualityOfService( void );
		virtual void RhsOnReceive( const char* const data, int len );
		virtual void RhsOnRoutingChange( void );
		virtual void RhsOnSend( void );
		virtual void RhsOnSocketError( unsigned long lastError );
		virtual void RhsOnWaitFailed( void );
		virtual void RhsOnStop( void );
		virtual void RhsOnKeepAlive( void );

		virtual void LhsOnAccept( LhsSocket& sock );
		virtual void LhsOnAddressListChange( LhsSocket& sock );
		virtual void LhsOnClose( LhsSocket& sock );
		virtual void LhsOnConnect( LhsSocket& sock );
		virtual void LhsOnOutOfBand( LhsSocket& sock );
		virtual void LhsOnQualityOfService( LhsSocket& sock );
		virtual void LhsOnReceive( LhsSocket& sock, const char* const data, int len );
		virtual void LhsOnRoutingChange( LhsSocket& sock );
		virtual void LhsOnSend( LhsSocket& sock );
		virtual void LhsOnSocketError( LhsSocket& sock, unsigned long lastError );
		virtual void LhsOnWaitFailed( LhsSocket& sock );
		virtual void LhsOnStop( LhsSocket& sock );
		virtual void LhsOnCloseAll( void );
		virtual void LhsOnKeepAlive( LhsSocket& sock );

		RhsSocket m_rhsSocket;
		RPW::Core::IpEndpoint m_rhsEndpoint;
		RPW::Core::CriticalSection m_csLhsSockets;
		LhsSocketMap m_lhsSockets;
	};

}