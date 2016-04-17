/// @file NotImplementedException.h
/// @author rev

#pragma once

#include <stdexcept>

namespace RPW
{
	namespace Core
	{

		class NotImplementedException : public std::logic_error
		{
		public:
			NotImplementedException( void );
			NotImplementedException( const char* const message );
			virtual ~NotImplementedException();
		};

	}
}
