/// @file ErrorCodeException.cpp
/// @author rev

#include "StdAfx.h"
#include "ErrorCodeException.h"

RPW::Core::ErrorCodeException::ErrorCodeException( const char* const message, unsigned long errorCode )
	: std::runtime_error( NULL == message ? "" : message )
	, m_errorCode( errorCode )
{
}

RPW::Core::ErrorCodeException::~ErrorCodeException( void )
{
}

unsigned long RPW::Core::ErrorCodeException::getErrorCode() const
{
	return m_errorCode;
}
