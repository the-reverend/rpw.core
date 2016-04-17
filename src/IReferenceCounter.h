#pragma once

#include <wtypes.h>           // Basic windows typedefs

namespace RPW
{
	namespace Core
	{

		class IReferenceCounter
		{
		public:
			virtual unsigned long AddRef( void ) = 0;
			virtual unsigned long Release( void ) = 0;

			class AutoRelease
			{
			public:
				inline AutoRelease( IReferenceCounter* prc = NULL )
					: m_pReferenceCounter( prc )
				{
				}

				inline ~AutoRelease( void )
				{
					release();
				}

				inline void attach( IReferenceCounter* rc )
				{
					release();
					m_pReferenceCounter = rc;
				}

				inline void detach( void )
				{
					m_pReferenceCounter = NULL;
				}

			private:
				void release( void )
				{
					if ( NULL != m_pReferenceCounter )
					{
						m_pReferenceCounter->Release();
					}
				}

				IReferenceCounter* m_pReferenceCounter;
			};
		};

	}
}
