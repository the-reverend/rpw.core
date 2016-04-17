/// @file ErrnoException.h
/// @author rev

#pragma once

#include <stdexcept>
#include <string>

namespace RPW
{
	namespace Core
	{

		class ErrnoException : public std::runtime_error
		{
		public:
			ErrnoException( errno_t errNo );
			virtual ~ErrnoException( void );

			int errNo( void );
			virtual std::string shortErrorText( void );
			std::string longErrorText( void ) const;

			static std::string getShortErrorText( int errNo );
			static std::string getLongErrorText( int errNo );
			static errno_t getErrNo();

		protected:
			errno_t m_errNo;
		};

	}
}
