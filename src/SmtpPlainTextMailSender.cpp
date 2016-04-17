/// @file SmtpPlainTextMailSender.cpp
/// @author rev

#include "StdAfx.h"
#include "SmtpPlainTextMailSender.h"

#include "Socket.h"
#include "WsaData.h"
#include "SocketException.h"
#include "LastErrorException.h"
#include "ErrnoException.h"
#include "Utilities.h"
#include "MD5.h"
#include "StringEx.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <time.h>

RPW::Core::SmtpPlainTextMailSender::SmtpPlainTextMailSender( void )
{
}

RPW::Core::SmtpPlainTextMailSender::~SmtpPlainTextMailSender( void )
{
}

void RPW::Core::SmtpPlainTextMailSender::Send(
   const char* const szSmtpServerName,
   const char* const szToAddr,
   const char* const szFromAlias,
   const char* const szFromAddr,
   const char* const szSubject,
   const char* const szMessageBody )
{
	RPW::Core::WsaData wsa;

	// Lookup email server's IP address.
	LPHOSTENT lpHostEntry = ::gethostbyname( szSmtpServerName );
	if ( NULL == lpHostEntry )
	{
		throw RPW::Core::SocketException( ::WSAGetLastError() );
	}

	RPW::Core::Socket sock;
	sock.create( PF_INET, SOCK_STREAM, 0 );
	if ( !sock.isValid() )
	{
		throw std::runtime_error( "cannot open mail server socket" );
	}

	// Get the mail service port
	LPSERVENT lpServEntry = ::getservbyname( "mail", 0 );

	// Use the SMTP default port if no other port is specified
	int port = ( NULL == lpServEntry ) ? ::htons( IPPORT_SMTP ) : lpServEntry->s_port;

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port   = port;
	SockAddr.sin_addr   = *(( LPIN_ADDR ) * lpHostEntry->h_addr_list );

	sock.connect( reinterpret_cast<struct sockaddr*>( &SockAddr ), sizeof( struct sockaddr ) ); // this throws on failure

	char szBuffer[4096] = { 0 };
	unsigned long smtpResponse = 0;
	Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after connect" );
	Check2( szBuffer, 220, "recv after connect" );

	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		std::ostringstream oss;
		oss << "HELO " << szSmtpServerName << "\r\n";
		Check1( sock.send( oss.str() ), "send HELO" );
		Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after HELO" );
		Check2( szBuffer, 250, "recv after HELO" );
	}

	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		std::ostringstream oss;
		oss << "MAIL FROM: <" << szFromAddr << ">\r\n";
		Check1( sock.send( oss.str() ), "send MAIL FROM" );
		Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after MAIL FROM" );
		Check2( szBuffer, 250, "recv after MAIL FROM" );
	}

	std::vector<std::string> vToAddresses;
	RPW::Core::StringEx::split1( std::string( szToAddr ), vToAddresses, ';' );
	for ( std::vector<std::string>::const_iterator it = vToAddresses.begin(); it != vToAddresses.end(); ++it )
	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		std::ostringstream oss;
		oss << "RCPT TO: <" << *it << ">\r\n";
		Check1( sock.send( oss.str() ), "send RCPT TO" );
		Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after RCPT TO" );
		Check2( szBuffer, 250, "recv after RCPT TO" );
	}

	::memset( szBuffer, 0, sizeof( szBuffer ) );
	Check1( sock.send( "DATA\r\n" ), "send DATA" );
	Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after DATA" );
	Check2( szBuffer, 354, "recv after DATA" );

	{
		//From: "ron wilson" <rwilson@thereverend.org>
		//To: <rev@thereverend.org>
		//Subject: test 3333
		//Date: Sat, 22 Jan 2011 21:44:22 -0500
		//Message-ID: <019b01cbbaa7$71197670$534c6350$@org>
		//MIME-Version: 1.0
		//Content-Type: text/plain;
		//.charset="us-ascii"
		//Content-Transfer-Encoding: 7bit
		//X-Mailer: Microsoft Office Outlook 12.0
		//Thread-Index: Acu6p3CKm73xFayhSNGQjUQ8FYSLvQ==
		//Content-Language: en-us

		const char* wdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		__time64_t gmt64 = ::_time64( NULL );
		struct tm gmt;
		if ( errno_t e = ::_gmtime64_s( &gmt, &gmt64 ) != 0 )
		{
			throw RPW::Core::ErrnoException( e );
		}

		std::ostringstream oss;
		oss << "From: \"" << szFromAlias << "\" <" << szFromAddr << ">" << "\r\n";
		for ( std::vector<std::string>::const_iterator it = vToAddresses.begin(); it != vToAddresses.end(); ++it )
		{
			if ( it == vToAddresses.begin() )
			{
				oss << "To: <" << *it << ">";
			}
			else
			{
				oss << ",\r\n\t<" << *it << ">";
			}
		}
		oss << "\r\n";
		oss << "Subject: " << szSubject << "\r\n";
		oss << "Date: " << wdays[gmt.tm_wday]
		    << ", " << gmt.tm_mday
		    << " " << months[gmt.tm_mon]
		    << " " << gmt.tm_year + 1900;
		oss << " " << std::setfill( '0' ) << std::right << std::setw( 2 ) << gmt.tm_hour;
		oss << ":" << std::setfill( '0' ) << std::right << std::setw( 2 ) << gmt.tm_min;
		oss << ":" << std::setfill( '0' ) << std::right << std::setw( 2 ) << gmt.tm_sec;
		oss << " -0000\r\n";

		// hash the header so far to create a unique message id
		RPW::Core::MD5 md5;
		std::string msgid = md5.getHashFromString( oss.str() );

		oss << "Message-ID: <" << msgid << ">\r\n";
		oss << "MIME-Version: 1.0\r\n";
		oss << "Content-Type: text/plain;\r\n";
		oss << ".charset=\"us-ascii\"\r\n";
		oss << "Content-Transfer-Encoding: 7bit\r\n";
		oss << "X-Mailer: RPW::Core::SmtpPlainTextMailSender\r\n";
		oss << "Thread-Index: " << msgid << "==\r\n";
		oss << "Content-Language: en-us\r\n";
		oss << "\r\n";
		oss << szMessageBody;
		oss << "\r\n.\r\n";

		::memset( szBuffer, 0, sizeof( szBuffer ) );
		Check1( sock.send( oss.str() ), "send BODY" );
		Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after BODY" );
		Check2( szBuffer, 250, "recv after BODY" );
	}

	::memset( szBuffer, 0, sizeof( szBuffer ) );
	Check1( sock.send( "QUIT\r\n" ), "send QUIT" );
	Check1( sock.receive( szBuffer, sizeof( szBuffer ), 0 ), "recv after QUIT" );
	Check2( szBuffer, 221, "recv after QUIT" );
}

void RPW::Core::SmtpPlainTextMailSender::Check1( int length, const char* const szFunction )
{
	if ( length == 0 )
	{
		std::ostringstream oss;
		oss << "zero bytes sent/received after '" << szFunction << "'";
		throw std::runtime_error( oss.str().c_str() );
	}
}

void RPW::Core::SmtpPlainTextMailSender::Check2( const char* const smtpResponse, unsigned long expectedNumber, const char* const szFunction )
{
	unsigned long num = 0;
	RPW::Core::Utilities::FromStringValue( smtpResponse, num, false );
	if ( num != expectedNumber )
	{
		std::ostringstream oss;
		oss << "unexpected response from SMTP server after '" << szFunction << "': " << smtpResponse;
		throw std::runtime_error( oss.str().c_str() );
	}
}
