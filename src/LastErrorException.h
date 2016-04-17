/// @file LastErrorException.h
/// @author rev

#pragma once

#include "ErrorCodeException.h"

#include <stdexcept>
#include <string>

namespace RPW
{
	namespace Core
	{
		class LastErrorException : public ErrorCodeException
		{
		public:
			LastErrorException( unsigned long lastError = 0 );
			virtual ~LastErrorException( void );

			std::string shortErrorText( void ) const;
			std::string longErrorText( void ) const;
			std::string formatErrorMessage( void ) const;

			static std::string getShortErrorText( unsigned long lastError );
			static std::string getLongErrorText( unsigned long lastError );
			static std::string formatErrorMessage( unsigned long lastError );
		};
	}
}
