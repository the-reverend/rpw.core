#pragma once

#include "CriticalSection.h"

#include <string>
#include <vector>

namespace RPW
{
	namespace Core
	{

		class FileSystem
		{
		public:
			FileSystem( void );
			virtual ~FileSystem( void );

			void cd( const char* const path );
			void ls( std::vector<std::string>& listing, int detailLevel = 0, const char* const glob = "*" );
			static void ls( const char* const path, std::vector<std::string>& listing, int detailLevel = 0, const char* const glob = "*" );
			std::string GetActivePath( void ) const;
			static std::string GetExecutingAssemblyPath( void );
			bool FileExists( const char* const name ) const;
			static bool FileExists( const char* const path, const char* const name );
			static std::string GetCurrentFolder( void );

		protected:
			static void SetCurrentFolder( const char* const path );

			std::string m_activePath;
			RPW::Core::CriticalSection m_cs;
		};

	}
}