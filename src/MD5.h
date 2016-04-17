#pragma once

#include "MD5Base.h"

#include <string>

namespace RPW
{
	namespace Core
	{
		class MD5 : protected RPW::Core::MD5Base
		{
		public:
			MD5( void );
			virtual ~MD5( void );

			std::string getHashFromString( const std::string& text );
			std::string getHashFromFile( const std::string& filename );

		protected:
			static std::string convToString( const unsigned char* const bytes, unsigned long len );
		};
	}
}
