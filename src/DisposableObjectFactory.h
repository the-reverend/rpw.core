/// @file DisposableObjectFactory.h
/// @author rev

#pragma once

#include "SimpleThread.h"
#include "CriticalSection.h"

#include <set>

namespace RPW
{
	namespace Core
	{

		class IDisposable;

		template < class TO >
		class DisposableObjectFactory
		{
		public:

			DisposableObjectFactory( DWORD disposalIntervalMs )
				: m_pPruneThread( NULL )
				, m_pruneIntervalMs( disposalIntervalMs )
			{
			}

			virtual ~DisposableObjectFactory( void )
			{
				m_cs.enter();
				cleanup();
				prune( true );
			}

			void init( void )
			{
				if ( NULL != m_pPruneThread )
				{
					throw std::logic_error( "init() called twice" );
				}

				m_pPruneThread = new RPW::Core::SimpleThread< RPW::Core::DisposableObjectFactory<TO> >( *this, &RPW::Core::DisposableObjectFactory<TO>::run );
			}

			void cleanup( void )
			{
				if ( NULL != m_pPruneThread )
				{
					m_pPruneThread->stop();
					delete m_pPruneThread;
					m_pPruneThread = NULL;
				}
			}

			TO* create( void )
			{
				TO* pObj = new TO();
				m_objects.insert( pObj );
				return pObj;
			}

			void prune( bool force = false )
			{
				RPW::Core::CriticalSection::Scope scope( m_cs );

				std::set<TO*>::iterator it = m_objects.begin();

				while ( m_objects.end() != it )
				{
					if ( force || ( *it )->ReadyToDispose() )
					{
						delete *it;
						m_objects.erase( it++ );
					}
					else
					{
						++it;
					}
				}
			}

		protected:

			void run( void )
			{
				bool done = false;

				while ( !done )
				{
					if ( NULL == m_pPruneThread )
					{
						done = true;
					}
					else
					{
						switch ( m_pPruneThread->terminate_event().waitFor( m_pruneIntervalMs ) )
						{
							case WAIT_FAILED: // fall through
							case WAIT_OBJECT_0:
								done = true;
								break;

							case WAIT_TIMEOUT:
								prune();
								break;

							default:
								throw std::runtime_error( "unexpected case" );
						}
					}
				}
			}

			RPW::Core::SimpleThread< DisposableObjectFactory<TO> >* m_pPruneThread;
			DWORD m_pruneIntervalMs;

			std::set<TO*> m_objects;
			RPW::Core::CriticalSection m_cs;
		};

	}
}
