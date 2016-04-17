/// @file CritcalSection.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h>

namespace RPW
{
	namespace Core
	{
		class CriticalSection : CRITICAL_SECTION
		{
		public:
			CriticalSection();
			virtual ~CriticalSection();
			void enter();
			void leave();

			bool isOwnedByCurrentThread( void );
			DWORD getOwnerThreadId( void );
			DWORD getDepth( void );

			class Scope
			{
			public:
				Scope( CriticalSection& cs );
				virtual ~Scope();

				void enter();
				void leave();

			protected:
				RPW::Core::CriticalSection* m_pcs;
				bool m_locked;

			private:
				Scope();
				Scope( const Scope& );
				void operator = ( const Scope& );
			};

		protected:
			DWORD m_owner;
			int m_depth;

		private:
			CriticalSection( const CriticalSection& );
			void operator = ( const CriticalSection& );
		};

	}
}
