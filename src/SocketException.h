/// @file SocketException.h
/// @author rev

#pragma once

#include "LastErrorException.h"

#include <stdexcept>
#include <string>

namespace RPW
{
	namespace Core
	{
		class Socket;

		/// @brief exception type for socket errors
		class SocketException : public RPW::Core::LastErrorException
		{
		public:
			SocketException( unsigned long wsaLastError = 0 );
			virtual ~SocketException( void );

			std::string formatErrorMessage( const RPW::Core::Socket& socket ) const;
			static std::string formatErrorMessage( unsigned long lastError, const RPW::Core::Socket& socket );
		};
	}
}