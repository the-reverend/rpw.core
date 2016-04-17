/// @file EventHandle.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h> // HANDLE
#include <string>

namespace RPW
{
	namespace Core
	{
		class EventHandle
		{
		public:
			EventHandle( HANDLE handle = NULL, bool autoClose = false );
			EventHandle( LPSECURITY_ATTRIBUTES lpEventAttributes
			             , BOOL bManualReset
			             , BOOL bInitialState
			             , const char* const szName
			             , bool autoClose = true );
			virtual ~EventHandle();

			void setAutoClose( bool on );
			bool getAutoClose( void );
			HANDLE& get( void );
			operator HANDLE( void ) const;
			EventHandle& operator=( const EventHandle& rhs );
			EventHandle& operator=( const HANDLE& rhs );
			BOOL set( void );
			BOOL reset( void );
			DWORD waitFor( DWORD dwMilliseconds = INFINITE );
			std::string name( void );
			bool isValid( void );

		private:
			HANDLE m_handle;
			std::string m_sName;
			bool m_bAutoClose;
		};
	}
}
