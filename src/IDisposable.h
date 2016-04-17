/// @file IDisposable.h
/// @author rev

#pragma once

namespace RPW
{
	namespace Core
	{

		class IDisposable
		{
		public:
			virtual bool ReadyToDispose() = 0;
		};

	}
}
