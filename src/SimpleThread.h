/// @file SimpleThread.h
/// @author rev

#pragma once

#include "CriticalSection.h"
#include "EventHandle.h"
#include "LastErrorException.h"
#include "ErrnoException.h"

#include <assert.h>
#include <process.h> // _beginthreadex()
//#include <stdlib.h> // for _get_errno()
//#include <string.h> // for strerror_s()
#define WIN32_LEAN_AND_MEAN
#include <wtypes.h> // HANDLE

namespace RPW
{
	namespace Core
	{
/// @details
/// This class template provides a simple method to use and delete threads.
/// The class takes in its constructor, the owner, and the function pointer for
/// the thread to use.  The function pointer as implemented has the signature of:
///
/// @code
/// void function(void)
/// @endcode
///
/// If one wanted to add parameters, the template could be altered to allow
/// passing of paramters either at instantiation or at the call of start().
///
/// Use of this template allows multiple threads to be used with various functions
/// used by the thread.
///
/// Declare variable for thread as:
///
/// @code
/// SimpleThread<some_class_name>*   my_thread_variable;
/// @endcode
///
/// Create variable at runtime as:
///
/// @code
/// my_thread_variable = new SimpleThread<some_class_name>(*this, &some_class_name::proc_name);
/// @endcode
///
/// For example, using a class named UdpSocket
///
/// @code
/// private:
///  SimpleThread<UdpSocket>*  m_recv_thread;
///  void recv_thread_proc();
/// [...]
/// m_recv_thread = new SimpleThread<UdpSocket>(*this, &UdpSocket::recv_thread_proc);
/// @endcode
		template < class TO >
		class SimpleThread
		{
		public:
			typedef void ( TO::*Func )();

			SimpleThread( TO& owner, Func fun )
				: m_terminate( 0, TRUE, 0, 0 )
				, m_thread_proc_done( 0, TRUE, 0, 0 )
				, m_thread( NULL )
				, m_thread_id( 0 )
				, m_thread_owner( owner )
				, m_fcn_ptr( fun )
			{
			}

			virtual ~SimpleThread()
			{
				stop();
			}

			RPW::Core::EventHandle& terminate_event()
			{
				return m_terminate;
			}

			RPW::Core::EventHandle& thread_proc_done_event()
			{
				return m_thread_proc_done;
			}

			operator HANDLE() const
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );
				return m_thread;
			}

			const HANDLE& thread_handle()
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );
				return m_thread;
			}

			const unsigned long thread_id()
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );
				return m_thread_id;
			}

			bool setPriority( int nPriority )
			{
				return 0 != ::SetThreadPriority( m_thread, nPriority );
			}

			void start()
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );

				if ( !m_terminate.isValid() )
				{
					throw std::logic_error( "terminate event handle is invalid" );
				}

				if ( NULL != m_thread )
				{
					throw std::logic_error( "thread is not null" );
				}

				m_thread = reinterpret_cast<HANDLE>
				           (
				              ::_beginthreadex(
				                 0 // void* security
				                 , 0 // unsigned stack_size
				                 , ( unsigned int( __stdcall* )( void* ) ) thread_starter // unsigned ( __stdcall *start_address )( void * )
				                 , reinterpret_cast<void*>( this ) // void* arglist
				                 , CREATE_SUSPENDED // unsigned initflag
				                 , reinterpret_cast<unsigned int*>( &m_thread_id ) // unsigned *thrdaddr
				              )
				           );

				if ( 0 == m_thread )
				{
					errno_t e = RPW::Core::ErrnoException::getErrNo();

					// cleanup after the failure
					cleanup();

					throw RPW::Core::ErrnoException( e );
				}
				else
				{
					// thread was successfully created; reset the terminate and done events
					m_terminate.reset();
					m_thread_proc_done.reset();

					if ( -1 == ::ResumeThread( m_thread ) )
					{
						unsigned long lastError = ::GetLastError();

						// thread failed to resume; so set the terminate and done events
						m_terminate.set();
						m_thread_proc_done.set();

						// cleanup after the failure
						cleanup();

						throw RPW::Core::LastErrorException( lastError );
					}
				}
			}

			bool isStarted( void ) const
			{
				return NULL != m_thread;
			}

			void stop()
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );

				m_terminate.set();

				DWORD id = ::GetCurrentThreadId();

				assert( id != m_thread_id );

				if ( NULL != m_thread && id != m_thread_id )
				{
					scope.leave();
					::WaitForSingleObject( m_thread, INFINITE );
					cleanup();
				}
			}

			TO& get_owner()
			{
				return m_thread_owner;
			}

			Func get_function()
			{
				return m_fcn_ptr;
			}

		private:
			SimpleThread();

			void cleanup()
			{
				RPW::Core::CriticalSection::Scope scope( m_thread_lock );

				if ( NULL != m_thread )
				{
					::CloseHandle( m_thread );
					m_thread = NULL;
				}

				if ( 0 != m_thread_id )
				{
					m_thread_id = 0;
				}
			}

			static unsigned int __stdcall thread_starter( void* vpThis )
			{
				RPW::Core::SimpleThread<TO>* pThread = reinterpret_cast<RPW::Core::SimpleThread<TO>*>( vpThis );
				pThread->thread_starter();

				return 0;
			}

			void thread_starter()
			{
				( get_owner().*( get_function() ) )();

				RPW::Core::CriticalSection::Scope scope( m_thread_lock );

				m_thread_proc_done.set();

				switch ( m_terminate.waitFor( 0 ) )
				{
					case WAIT_TIMEOUT:
						cleanup();
						break;

					case WAIT_OBJECT_0:
					case WAIT_ABANDONED:
					case WAIT_FAILED:
					default:
						break;
				}

				//::_endthreadex( 0 ); // C++ destructors pending in the thread will not be called.
			}

		private:
			RPW::Core::CriticalSection m_thread_lock;
			RPW::Core::EventHandle     m_terminate;
			RPW::Core::EventHandle     m_thread_proc_done;

			HANDLE        m_thread;
			unsigned long m_thread_id;

			TO&  m_thread_owner;
			Func m_fcn_ptr;
		};
	}
}