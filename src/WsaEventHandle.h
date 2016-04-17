/// @file WsaEventHandle.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h> // HANDLE
#include <Winsock2.h>
#include <string>
#include <vector>

namespace RPW
{
	namespace Core
	{

		class WsaEventHandle
		{
		public:
			WsaEventHandle( void );
			virtual ~WsaEventHandle( void );

			operator WSAEVENT( void ) const;

			int select( SOCKET sock, long events );
			int enumEvents( SOCKET sock, LPWSANETWORKEVENTS lpNetworkEvents );
			DWORD waitFor( DWORD dwMilliseconds = INFINITE );

		private:
			WSAEVENT m_handle;
		};

	}
}
