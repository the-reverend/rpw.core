/// @file IniReader.h
/// @author rev

#pragma once

#include "Utilities.h"

#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <vector>

namespace RPW
{
	namespace Core
	{

		/// @brief ini file config manager
		class IniReader
		{
		public:
			typedef std::set<std::string> SectionSet;
			typedef std::vector<std::string> SectionVector;
			typedef std::map<std::string, std::string> KeyValueMap;

			IniReader( bool caseSensitive = true );
			virtual ~IniReader();

			bool Load( const std::string& filePath );
			bool Load( std::istream& inFile );
			void Clear();

			template<typename TYPE>
			inline TYPE GetValue( const std::string& key, const TYPE defaultValue, bool throwOnFail = false )
			{
				std::string searchKey = key;

				if ( !m_bCaseSensitive )
					std::transform( searchKey.begin(), searchKey.end(), searchKey.begin(), tolower );

				if ( m_mapKeyValue.find( searchKey ) != m_mapKeyValue.end() )
				{
					TYPE val;
					try
					{
						RPW::Core::Utilities::FromStringValue( m_mapKeyValue[searchKey], val );
					}
					catch ( std::string se )
					{
						val = defaultValue;
					}

					return val;
				}

				if ( throwOnFail )
				{
					std::ostringstream oss;
					oss << "unable to find the key/value pair in the ini map: " << key;
					throw std::runtime_error( oss.str().c_str() );
				}

				return defaultValue;
			}

			std::string GetString( const std::string& key, const char* const defaultString, bool throwOnFail = false );
			std::string GetString( const std::string& key, const std::string& defaultString, bool throwOnFail = false );

			unsigned long GetHex( const std::string& key, const std::string& defaultString, bool throwOnFail = false );

			const SectionSet& GetSections( void ) const { return m_setSections; }
			const SectionVector& GetSectionsVector( void ) const { return m_vecSections; }
			const KeyValueMap& GetKeyMap( void ) const { return m_mapKeyValue; }

			bool HasSection( const std::string& section ) { return m_setSections.find( section ) != m_setSections.end(); }

		protected:
			void Parse( const std::string& line );

			bool m_bCaseSensitive;
			std::string m_strFilePath;    ///< filename and path for the ini file
			std::string m_strLastSection; ///< last section parsed from the ini file
			KeyValueMap m_mapKeyValue; ///< map of key/value pairs
			SectionSet m_setSections;
			SectionVector m_vecSections;
		};

	}
}
