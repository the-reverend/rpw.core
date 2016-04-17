/// @file NullArgumentException.cpp
/// @author rev

#include "StdAfx.h"
#include "NullArgumentException.h"

RPW::Core::NullArgumentException::NullArgumentException( void )
	: std::invalid_argument( "unexpected null argument" )
{
}
