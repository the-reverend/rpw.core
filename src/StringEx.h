#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace RPW
{
	namespace Core
	{
		class StringEx : public std::string
		{
		public:
			StringEx( void );
			StringEx( const char* const initial );
			StringEx( const char* const initial, std::string::size_type len );
			StringEx( const std::string& initial );
			virtual ~StringEx( void );

			/// @brief split a string as you would a command line; space delimiters except within double quotes
			bool split1( std::vector<RPW::Core::StringEx>& vec, char delim = ' ', char leftquote = '"', char rightquote = '"' );
			std::vector<RPW::Core::StringEx> split1( char delim = ' ', char leftquote = '"', char rightquote = '"' );
			std::string stripAnsi( void );
			unsigned __int64 hash( void );

			static bool split1( const std::string& str, std::vector<std::string>& vec, char delim = ' ', char leftquote = '"', char rightquote = '"' );
			static std::string stripAnsi( const std::string& ansiText );
			static unsigned __int64 hash( const std::string& str );

		private:
			template<class STR>
			static bool split1( const STR& str, std::vector<STR>& vec, char delim = ' ', char leftquote = '"', char rightquote = '"' )
			{
				static const int out = 0;
				static const int in = 1;
				static const int inq = 2;

				bool success = true;
				int state = 0;
				std::string::size_type i = 0;
				std::ostringstream oss;

				for ( STR::size_type i = 0; i < str.length(); ++i )
				{
					char c = str[ i ];
					switch ( state )
					{
						case out:
							if ( c == leftquote )
							{
								state = inq;
							}
							else if ( c != delim )
							{
								state = in;
								oss << c;
							}
							break;

						case in:
							if ( c == delim )
							{
								state = out;
								vec.push_back( STR( oss.str() ) );
								oss.str( "" );
							}
							else
							{
								oss << c;
							}
							break;

						case inq:
							if ( c == rightquote )
							{
								state = out;
								vec.push_back( STR( oss.str() ) );
								oss.str( "" );
							}
							else
							{
								oss << c;
							}
							break;

						default: break;
					}
				}

				switch ( state )
				{
					case inq: success = false;           break;
					case in: vec.push_back( oss.str() ); break;
					default:                             break;
				}

				return success;
			}
		};

	}
}