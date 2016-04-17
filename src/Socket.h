/// @file Socket.h
/// @author rev

#pragma once

#include "IpEndpoint.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <string>
#include <vector>

namespace RPW
{
	namespace Core
	{
		typedef std::basic_string <unsigned char> ustring;

		class Socket
		{
		public:

			Socket( SOCKET sock = INVALID_SOCKET );
			Socket( const Socket& sock );
			virtual ~Socket( void );

			virtual operator SOCKET( void ) const;
			virtual Socket& operator=( const Socket& rhs );
			virtual Socket& operator=( const SOCKET& rhs );
			virtual bool operator==( const Socket& rhs ) const;
			virtual bool operator!=( const Socket& rhs ) const;
			virtual bool operator<( const Socket& rhs ) const;
			virtual bool operator<=( const Socket& rhs ) const;
			virtual bool operator>( const Socket& rhs ) const;
			virtual bool operator>=( const Socket& rhs ) const;

			virtual void setSocket( SOCKET sock );
			virtual SOCKET getSocket( void ) const;
			virtual unsigned long getIp( void );
			virtual unsigned long getPeerIp( void );

			/// @brief shutdown then close socket
			virtual void destroy( void );

			/// @return true if socket is not INVALID_SOCKET
			virtual bool isValid( void ) const;

			/// @return true if the socket is connected
			virtual bool isConnected( void );

			/// @brief strip out and return the internal socket
			/// @return the internal raw socket
			virtual SOCKET strip( void );

#pragma region wrapper methods

			/// @brief accept a socket from a listener
			/// @param addr contains information about the remote endpoint
			/// @param addrlen the size of the sockaddr object
			virtual SOCKET accept( __out struct sockaddr* addr, __inout int* addrlen );

			/// @brief accept a socket from a listener
			/// @param ep contains information about the remote endpoint
			virtual SOCKET accept( __out RPW::Core::IpEndpoint& ep );

			/// @brief accept a socket from a listener
			virtual SOCKET accept( void );

			/// @brief bind the socket to an endpoint
			/// @param name the address to assign to the socket
			/// @param namelen the length of the value in the name parameter, in bytes
			/// @return true on success
			virtual bool bind( __in const struct sockaddr* name, __in int namelen );

			/// @brief bind the socket to an endpoint
			/// @param name the address to assign to the socket
			/// @return true on success
			virtual bool bind( __in IpEndpoint& name );

			/// @brief close the socket
			/// @return true on success
			virtual bool close( void );

			virtual bool connect( __in const struct sockaddr* name, __in int namelen );
			virtual bool connect( __in RPW::Core::IpEndpoint& ep );

			virtual bool create( __in int af, __in int type, __in int protocol );
			virtual bool create( __in int af, __in int type, __in int protocol, __in LPWSAPROTOCOL_INFOA lpProtocolInfo, __in GROUP g, __in DWORD dwFlags );

			/// @return true on success
			virtual bool getPeerName( __out struct sockaddr* name, __inout int* namelen );
			virtual bool getPeerName( __out RPW::Core::IpEndpoint& name );

			/// @return true on success
			virtual bool getSocketName( __out struct sockaddr* name, __inout int* namelen );
			virtual bool getSocketName( __out RPW::Core::IpEndpoint& name );

			/// @return true on success
			virtual bool getSocketOption( __in int level, __in int optname, __out char* optval, __inout int* optlen );

			/// @return true on success
			virtual bool ioctlSocket( __in long cmd, __inout u_long* argp );

			/// @return true on success
			virtual bool listen( __in int backlog = SOMAXCONN );

			virtual int receive( __out char* buffer, __in int len, __in int flags = 0 );
			virtual int receive( __out unsigned char* buffer, __in int len, __in int flags = 0 );

			/// @brief receive data from an endpoint
			/// @param buffer a buffer for incoming data
			/// @param len the length, in bytes, of the buffer pointed to by the buffer parameter
			/// @param flags a set of options that modify the behavior of the function call beyond the options specified for the associated socket
			/// @param from an optional pointer to a buffer in a sockaddr structure that will hold the source address upon return
			/// @param fromlen an optional pointer to the size, in bytes, of the buffer pointed to by the from parameter
			/// @returns the number of bytes recieved; if the connection has been
			/// @todo reorder the flags parameter to be last and implement the overloads below
			virtual int receiveFrom( __out char* buffer, __in int len, __in int flags = 0, __out struct sockaddr* from = 0, __inout_opt int* fromlen = 0 );
			virtual int receiveFrom( __out unsigned char* buffer, __in int len, __in int flags = 0, __out struct sockaddr* from = 0, __inout_opt int* fromlen = 0 );
			//virtual int receiveFrom( __out std::string& buffer, __in int len, __in int flags = 0, __out struct sockaddr* from = 0, __inout_opt int* fromlen = 0 );
			//virtual int receiveFrom( __out RPW::Core::ustring& buffer, __in int len, __in int flags = 0, __out struct sockaddr* from = 0, __inout_opt int* fromlen = 0 );

			/// @brief send data
			/// @param buffer a pointer to a buffer containing the data to be transmitted
			/// @param len the length, in bytes, of the data pointed to by the buffer parameter
			/// @param flags a set of flags that specify the way in which the call is made
			/// @returns the total number of bytes sent, or SOCKET_ERROR on error
			virtual int send( __in const char* buffer, __in int len, __in int flags = 0 );
			virtual int send( __in char c, __in int flags = 0 );
			virtual int send( __in unsigned char c, __in int flags = 0 );
			virtual int send( __in const unsigned char* buffer, __in int len, __in int flags = 0 );
			virtual int send( __in const std::string& buffer, __in int flags = 0 );
			virtual int send( __in const RPW::Core::ustring& buffer, __in int flags = 0 );

			/// @brief send data to an endpoint
			/// @param buffer a pointer to a buffer containing the data to be transmitted
			/// @param len the length, in bytes, of the data pointed to by the buffer parameter
			/// @param flags a set of flags that specify the way in which the call is made
			/// @param to an optional pointer to a sockaddr structure that contains the address of the target socket
			/// @param tolen the size, in bytes, of the address pointed to by the to parameter
			/// @returns the total number of bytes sent, or SOCKET_ERROR on error
			/// @todo reorder the flags parameter to be last and implement the commented out overloads below
			virtual int sendTo( __in const char* buffer, __in int len, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in char c, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in unsigned char c, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const unsigned char* buffer, __in int len, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const std::string& buffer, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );
			virtual int sendTo( __in const RPW::Core::ustring& buffer, __in int flags = 0, __in const struct sockaddr* to = 0, __in int tolen = 0 );

			/// @return true on success
			virtual bool setSocketOption( __in int level, __in int optname, __in const char* optval, __in int optlen );

			/// @brief disable send and/or receive
			/// @param how SD_RECIEVE, SD_SEND, SD_BOTH
			/// @return true on success
			virtual bool shutdown( __in int how );

#pragma endregion

		private:

			/// @brief handle a socket error
			/// @param socketError if SOCKET_ERROR, report an error message
			/// @param throwOnError if an error is found, throw a meaningful SocketException
			/// @param forgive a list of WSA error codes that should be forgiven if identified
			/// @returns false on SOCKET_ERROR
			bool handleSocketSuccess( int socketError, const std::vector<int>* forgive = NULL );

			//bool m_autoDestroy;
			SOCKET m_socket; ///< the socket
		};
	}
}
