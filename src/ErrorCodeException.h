/// @file ErrorCodeException.h
/// @author rev

#pragma once

#include <stdexcept>
#include <string>

namespace RPW
{
	namespace Core
	{

		class ErrorCodeException : public std::runtime_error
		{
		public:
			ErrorCodeException( const char* const message = NULL, unsigned long errorCode = 0 );
			virtual ~ErrorCodeException( void );

			unsigned long getErrorCode() const;

		protected:
			unsigned long m_errorCode;
		};

	}
}
