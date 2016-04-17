#pragma once

#include "CriticalSection.h"
#include "IReferenceCounter.h"

namespace RPW
{
	namespace Core
	{

		template<class T>
		class RefCountSingleton : public RPW::Core::IReferenceCounter
		{
		public:
			static T& Instance( void )
			{
				RPW::Core::CriticalSection::Scope scope( m_lock );
				if ( !m_instance )
				{
					m_instance = new T;
				}
				m_instance->AddRef();
				return *m_instance;
			}

			unsigned long AddRef( void )
			{
				RPW::Core::CriticalSection::Scope scope( m_lock );
				InterlockedIncrement( &m_ref_cnt );
				return m_ref_cnt;
			}

			unsigned long Release( void )
			{
				RPW::Core::CriticalSection::Scope scope( m_lock );
				::InterlockedDecrement( &m_ref_cnt );
				if ( m_ref_cnt == 0 )
				{
					if ( m_instance )
					{
						delete m_instance;
						m_instance = NULL;
					}
					return 0;
				}
				return m_ref_cnt;
			}

		protected:
			RefCountSingleton( void ) : m_ref_cnt( 0 ) {}
			~RefCountSingleton( void ) {}
			RefCountSingleton( const RefCountSingleton& );
			RefCountSingleton& operator=( const RefCountSingleton& );
			static T* m_instance;

			long GetRefCount( void )
			{
				return m_ref_cnt;
			}

		private:
			static RPW::Core::CriticalSection m_lock;
			long m_ref_cnt;
		};

		template<class T> RPW::Core::CriticalSection RPW::Core::RefCountSingleton<T>::m_lock;
		template<class T> T* RPW::Core::RefCountSingleton<T>::m_instance = NULL;

	}
}
