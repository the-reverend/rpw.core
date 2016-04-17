/// @file Library.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h> // HMODULE

namespace RPW
{
	namespace Core
	{
		class Library
		{
		public:
			Library( LPCTSTR module );
			virtual ~Library();

			operator HMODULE( void ) const;

		private:
			HMODULE m_hModule;
		};
	}
}
