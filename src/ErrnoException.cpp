/// @file ErrnoException.cpp
/// @author rev

#include "StdAfx.h"
#include "ErrnoException.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

RPW::Core::ErrnoException::ErrnoException( errno_t errNo )
	: std::runtime_error( getShortErrorText( errNo ) )
	, m_errNo( errno )
{
}

RPW::Core::ErrnoException::~ErrnoException( void )
{
}

int RPW::Core::ErrnoException::errNo( void )
{
	return m_errNo;
}

std::string RPW::Core::ErrnoException::shortErrorText( void )
{
	return getShortErrorText( m_errNo );
}

std::string RPW::Core::ErrnoException::longErrorText( void ) const
{
	return getLongErrorText( m_errNo );
}

std::string RPW::Core::ErrnoException::getShortErrorText( int errNo )
{
	switch ( errNo )
	{
		case EPERM:        return "EPERM";
		case ENOENT:       return "ENOENT";
		case ESRCH:        return "ESRCH";
		case EINTR:        return "EINTR";
		case EIO:          return "EIO";
		case ENXIO:        return "ENXIO";
		case E2BIG:        return "E2BIG";
		case ENOEXEC:      return "ENOEXEC";
		case EBADF:        return "EBADF";
		case ECHILD:       return "ECHILD";
		case EAGAIN:       return "EAGAIN";
		case ENOMEM:       return "ENOMEM";
		case EACCES:       return "EACCES";
		case EFAULT:       return "EFAULT";
		case EBUSY:        return "EBUSY";
		case EEXIST:       return "EEXIST";
		case EXDEV:        return "EXDEV";
		case ENODEV:       return "ENODEV";
		case ENOTDIR:      return "ENOTDIR";
		case EISDIR:       return "EISDIR";
		case EINVAL:       return "EINVAL";
		case ENFILE:       return "ENFILE";
		case EMFILE:       return "EMFILE";
		case ENOTTY:       return "ENOTTY";
		case EFBIG:        return "EFBIG";
		case ENOSPC:       return "ENOSPC";
		case ESPIPE:       return "ESPIPE";
		case EROFS:        return "EROFS";
		case EMLINK:       return "EMLINK";
		case EPIPE:        return "EPIPE";
		case EDOM:         return "EDOM";
		case ERANGE:       return "ERANGE";
		case EDEADLK:      return "EDEADLK";
		case ENAMETOOLONG: return "ENAMETOOLONG";
		case ENOLCK:       return "ENOLCK";
		case ENOSYS:       return "ENOSYS";
		case ENOTEMPTY:    return "ENOTEMPTY";
		case EILSEQ:       return "EILSEQ";
		case STRUNCATE:    return "STRUNCATE";
		default:           return "UNKNOWN";
	}
}

std::string RPW::Core::ErrnoException::getLongErrorText( int errNo )
{
	char msg[4096];
	::strerror_s( msg, sizeof( msg ), errNo );
	return std::string( msg );
}

errno_t RPW::Core::ErrnoException::getErrNo()
{
	errno_t e = 0;
	if ( 0 == ::_get_errno( &e ) )
	{
		return e;
	}

	return 0;
}
