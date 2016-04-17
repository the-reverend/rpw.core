/// @file CritcalSection.cpp
/// @author rev

#include "StdAfx.h"
#include "CriticalSection.h"

RPW::Core::CriticalSection::CriticalSection()
	: m_owner( 0 )
	, m_depth( 0 )
{
	::InitializeCriticalSection( this );
}

RPW::Core::CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection( this );
}

void RPW::Core::CriticalSection::enter()
{
	::EnterCriticalSection( this );
	++m_depth;
	m_owner = ::GetCurrentThreadId();
}

void RPW::Core::CriticalSection::leave()
{
	if ( 0 == --m_depth )
	{
		m_owner = 0;
	}

	::LeaveCriticalSection( this );
}

bool RPW::Core::CriticalSection::isOwnedByCurrentThread( void )
{
	return ::GetCurrentThreadId() == m_owner;
}

DWORD RPW::Core::CriticalSection::getOwnerThreadId( void )
{
	return m_owner;
}

DWORD RPW::Core::CriticalSection::getDepth( void )
{
	return isOwnedByCurrentThread() ? m_depth : 0;
}

RPW::Core::CriticalSection::Scope::Scope( CriticalSection& cs )
	: m_pcs( &cs )
	, m_locked( true )
{
	m_pcs->enter();
}

RPW::Core::CriticalSection::Scope::~Scope()
{
	leave();
}

void RPW::Core::CriticalSection::Scope::enter()
{
	if ( !m_locked )
	{
		m_pcs->enter();
		m_locked = true;
	}
}

void RPW::Core::CriticalSection::Scope::leave()
{
	if ( m_locked )
	{
		m_locked = false;
		m_pcs->leave();
	}
}
