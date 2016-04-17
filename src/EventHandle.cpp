/// @file EventHandle.cpp
/// @author rev

#include "StdAfx.h"
#include "EventHandle.h"

#include <assert.h>

RPW::Core::EventHandle::EventHandle( HANDLE handle, bool autoClose )
	: m_handle( handle )
	, m_bAutoClose( autoClose )
{
}

RPW::Core::EventHandle::EventHandle( LPSECURITY_ATTRIBUTES lpEventAttributes
                                     , BOOL bManualReset
                                     , BOOL bInitialState
                                     , const char* const szName
                                     , bool autoClose )
	: m_handle( ::CreateEventA( lpEventAttributes, bManualReset, bInitialState, szName ) )
	, m_sName( szName ? szName : "" )
	, m_bAutoClose( autoClose )
{
	assert( NULL != m_handle );
}

RPW::Core::EventHandle::~EventHandle()
{
	if ( m_bAutoClose )
	{
		assert( 0 != ::CloseHandle( m_handle ) );
		m_handle = NULL;
	}
}

void RPW::Core::EventHandle::setAutoClose( bool on )
{
	m_bAutoClose = on;
}

bool RPW::Core::EventHandle::getAutoClose( void )
{
	return m_bAutoClose;
}

HANDLE& RPW::Core::EventHandle::get()
{
	return m_handle;
}

RPW::Core::EventHandle::operator HANDLE() const
{
	return m_handle;
}

RPW::Core::EventHandle& RPW::Core::EventHandle::operator=( const RPW::Core::EventHandle & rhs )
{
	if ( this != &rhs )
	{
		m_handle = rhs;
	}

	return *this;
}

RPW::Core::EventHandle& RPW::Core::EventHandle::operator=( const HANDLE & rhs )
{
	if ( &this->m_handle != &rhs )
	{
		m_handle = rhs;
	}

	return *this;
}

BOOL RPW::Core::EventHandle::set()
{
	return ::SetEvent( m_handle );
}

BOOL RPW::Core::EventHandle::reset()
{
	return ::ResetEvent( m_handle );
}

DWORD RPW::Core::EventHandle::waitFor( DWORD dwMilliseconds )
{
	return ::WaitForSingleObject( m_handle, dwMilliseconds );
}

std::string RPW::Core::EventHandle::name()
{
	return m_sName;
}

bool RPW::Core::EventHandle::isValid()
{
	return NULL != m_handle;
}