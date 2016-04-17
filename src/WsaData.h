/// @file WsaData.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

namespace RPW
{
	namespace Core
	{
		class WsaData
		{
		public:
			WsaData( BYTE major = 2, BYTE minor = 2 );
			~WsaData( void );

		private:
			WSADATA m_wsa;
		};
	}
}
