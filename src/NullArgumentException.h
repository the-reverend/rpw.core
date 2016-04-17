/// @file NullArgumentException.h
/// @author rev

#pragma once

#include <stdexcept>

namespace RPW
{
	namespace Core
	{

		/// @brief exception type for null argument errors
		class NullArgumentException : public std::invalid_argument
		{
		public:
			NullArgumentException( void );
		};

	}
}