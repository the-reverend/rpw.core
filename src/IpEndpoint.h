/// @file IpEndpoint.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <string>

namespace RPW
{
	namespace Core
	{
		class IpEndpoint
		{
		public:
			IpEndpoint( void );
			IpEndpoint( unsigned long hostOrderAddress, unsigned short hostOrderPort, ADDRESS_FAMILY family = AF_INET );
			IpEndpoint( const char* const ip, unsigned short hostOrderPort, ADDRESS_FAMILY family = AF_INET );
			IpEndpoint( SOCKET sock );
			~IpEndpoint( void );

			IpEndpoint& init( unsigned long hostOrderAddress, unsigned short hostOrderPort, ADDRESS_FAMILY family = AF_INET );
			IpEndpoint& init( const char* const ip, unsigned short hostOrderPort, ADDRESS_FAMILY family = AF_INET );
			IpEndpoint& init( SOCKET sock );

			operator sockaddr*( void );
			operator sockaddr_in( void ) const;
			unsigned short getPort( void ) const;
			unsigned long getAddress( void ) const;
			static std::string getAddressString( unsigned long hostOrderAddress );
			std::string getAddressString( void ) const;
			static unsigned long getAddressULong( const char* const ip );
			std::string getAddressString2( void ) const;
			ADDRESS_FAMILY getFamily( void ) const;

			IpEndpoint& zero( void );
			IpEndpoint& setPort( unsigned short hostOrderPort );
			IpEndpoint& setAddress( unsigned long hostOrderAddress );
			IpEndpoint& setFamily( ADDRESS_FAMILY family );

			/// @return true on success
			bool getSockName( SOCKET socket );

		public:
			struct sockaddr_in m_addr;
		};
	}
}
