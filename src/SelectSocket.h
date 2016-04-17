#pragma once

#include "Socket.h"

#include "SimpleThread.h"
#include "WsaEventHandle.h"

#define THREAD_TIMEOUT 100

namespace RPW
{
	namespace Core
	{

		class SelectSocket : public RPW::Core::Socket
		{
		public:
			SelectSocket( SOCKET sock = INVALID_SOCKET );
			SelectSocket( const SelectSocket& sock );
			virtual ~SelectSocket( void );

			virtual void OnAccept( void ) {}
			virtual void OnAddressListChange( void ) {}
			virtual void OnClose( void ) {}
			virtual void OnConnect( void ) {}
			virtual void OnOutOfBand( void ) {}
			virtual void OnQualityOfService( void ) {}
			virtual void OnReceive( void ) {}
			virtual void OnRoutingChange( void ) {}
			virtual void OnSend( void ) {}
			virtual void OnSocketError( unsigned long lastError ) {}
			virtual void OnWaitFailed( void ) {}
			virtual void OnStop( void ) {}
			virtual void OnKeepAlive( void ) {}

			virtual void Start( DWORD timeout = THREAD_TIMEOUT );
			virtual bool IsStarted( void ) const;
			virtual void Stop( void );

			virtual void SetupKeepAlive( DWORD keepalivetimeoutseconds = 0 );
			virtual void ResetWaitableTimer();

			virtual int send( __in const char* buffer, __in int len, __in int flags = 0 );
			virtual int send( __in char c, __in int flags = 0 );
			virtual int send( __in unsigned char c, __in int flags = 0 );
			virtual int send( __in const unsigned char* buffer, __in int len, __in int flags = 0 );
			virtual int send( __in const std::string& buffer, __in int flags = 0 );
			virtual int send( __in const RPW::Core::ustring& buffer, __in int flags = 0 );

			virtual int sendTo( __in const char* buffer, __in int len, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in char c, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in unsigned char c, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const unsigned char* buffer, __in int len, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const std::string& buffer, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const RPW::Core::ustring& buffer, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );

		private:
			void ThreadProc( void );

			SimpleThread<SelectSocket>* m_pThread;
			DWORD m_threadTimeout;

			HANDLE m_hKeepAliveTimer;

			// don't assign these directly.  use SetupKeepAlive() to keep them in sync.
			DWORD m_dwKeepAliveTimeoutMs;
			LARGE_INTEGER m_liKeepAliveTimeout;
		};

	}
}