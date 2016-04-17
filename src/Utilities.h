/// @file Utilities.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>
#include <sstream>

namespace RPW
{
	namespace Core
	{
		class Utilities
		{
		public:
			template<typename TYPE>
			static inline void ToStringValue( const TYPE& myVal, std::string& myStr )
			{
				std::ostringstream ostr;
				if ( !( ostr << myVal ) )
				{
					throw std::runtime_error( "ToStringValue conversion failed" );
				}
				myStr = ostr.str();
			}

			template<typename TYPE>
			static inline void FromStringValue( const std::string& myStr, TYPE& myVal, bool failIfLeftCharsLeft = true )
			{
				std::istringstream i( myStr );
				char c;
				if ( !( i >> myVal )
				      || ( failIfLeftCharsLeft
				           && i.get( c ) ) )
				{
					throw std::runtime_error( "FromStringValue conversion failed" );
				}
			}

			/// @brief compare floats for equality
			/// @see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
			static bool FloatEquals( float lhs, float rhs, int maxUlps );

			/// @brief calculate the difference between two instances of SYSTEMTIME
			/// @returns time difference in 10,000ths of a millisecond
			static long long SystemTimeDiff( const SYSTEMTIME start, const SYSTEMTIME finish );

			/// @brief calculate the difference between two instances of SYSTEMTIME
			/// @returns time difference in milliseconds
			static long long SystemTimeDiffMs( const SYSTEMTIME start, const SYSTEMTIME finish );

			/// @brief trim leading and trailing white space
			/// @return a string with leading/trailing characters of a set stripped
			/// @param str the string to be trimmed
			/// @param whiteSet C string with characters to be dropped
			static std::string Trim( const std::string& str, const std::string& whiteSet = " \t" );

			/// @brief convert input string into vector of string tokens
			/// @note consecutive delimiters will be treated as single delimiter
			/// @note delimiters are _not_ included in return data
			/// @param input string to be parsed
			/// @param delims list of delimiters
			static std::vector<std::string> TokenizeStr( const std::string& str, const std::string& delims = " \t" );

			static std::string GetRealFilename( const char* const szFilename );

			static std::string GetFileNameFromHandle( HANDLE hFile );
		};

	}
}
