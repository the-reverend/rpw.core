/// @file CommandLineProcessor.h
/// @author rev

#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace RPW
{
	namespace Core
	{

		template <class CONSUMER>
		class CommandLineProcessor
		{
		public:
			typedef void ( CONSUMER::*FunPtr )( const std::string& param );
			typedef std::map<std::string, FunPtr> function_map;

			CommandLineProcessor()
				: m_consumer( NULL )
				, m_errorCallback( NULL )
				, m_halted( false )
			{
			}

			~CommandLineProcessor()
			{
			}

			void RegisterConsumer( CONSUMER& cons )
			{
				m_consumer = &cons;
			}

			void Halt() { m_halted = true; }

			bool Process( const std::vector<std::string>& argv )
			{
				std::string sParam;
				bool expectArg = false;

				for ( std::vector<std::string>::const_iterator it = argv.begin(); it != argv.end() && !m_halted; ++it )
				{
					bool isParam = ( *it ).find_first_of( "-/" ) == 0;

					if ( expectArg && isParam )
					{
						MakeCallback( sParam, "" );
						expectArg = false;
					}

					if ( expectArg )
					{
						MakeCallback( sParam, *( it ) );
						expectArg = false;
					}
					else if ( isParam )
					{
						sParam = ( *it ).substr( 1 );
						expectArg = true;
					}
					else
					{
						ErrorCallback( *( it ) );
					}
				}

				if ( expectArg && !m_halted )
				{
					MakeCallback( sParam, "" );
				}

				return !m_halted;
			}

			bool Process( int argc, char* argv[], int firstIndex = 0 )
			{
				std::vector<std::string> v;

				for ( int i = firstIndex; i < argc; i++ )
				{
					v.push_back( std::string( argv[i] ) );
				}

				return Process( v );
			}

			void MakeCallback( const std::string& param, const std::string& arg )
			{
				std::string p( param );
				std::transform( p.begin(), p.end(), p.begin(), ::tolower );

				function_map::iterator fit = m_functionMap.find( p );
				if ( fit != m_functionMap.end() )
				{
					FunPtr ptr = ( *fit ).second;
					( m_consumer->*ptr )( arg );
				}
				else
				{
					ErrorCallback( std::string( "-" ) + param );
					if ( arg.length() > 0 )
					{
						ErrorCallback( arg );
					}
				}
			}

			void ErrorCallback( const std::string& param )
			{
				if ( m_errorCallback )
				{
					( m_consumer->*m_errorCallback )( param );
				}
			}

			void RegisterCallback( const std::string& param, FunPtr funptr )
			{
				std::string p( param );
				std::transform( p.begin(), p.end(), p.begin(), ::tolower );

				m_functionMap.insert( function_map::value_type( p, funptr ) );
			}

			void RegisterErrorCallback( FunPtr funptr )
			{
				m_errorCallback = funptr;
			}

		private:
			CONSUMER*    m_consumer;
			std::string  m_cmdLine;
			function_map m_functionMap;
			FunPtr       m_errorCallback;
			bool         m_halted;
		};

	}
}
