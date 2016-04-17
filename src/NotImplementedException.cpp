/// @file NotImplementedException.cpp
/// @author rev

#include "StdAfx.h"
#include "NotImplementedException.h"

RPW::Core::NotImplementedException::NotImplementedException( void )
	: std::logic_error( "not implemented" )
{
}

RPW::Core::NotImplementedException::NotImplementedException( const char* const message )
	: std::logic_error( message )
{
}

RPW::Core::NotImplementedException::~NotImplementedException( void )
{
}
