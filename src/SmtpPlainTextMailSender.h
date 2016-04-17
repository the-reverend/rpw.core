/// @file SmtpPlainTextMailSender.h
/// @author rev

#pragma once

// example usage:
//try
//{
// RPW::Core::SmtpPlainTextMailSender::Send(
//    "smtp.comcast.net",
//    "rev@thereverend.org;sqlite@thereverend.org",
//    "ron wilson",
//    "rwilson@thereverend.org",
//    "testtesttest",
//    "whoa nellie\r\n\r\nthis is a test from trader.exe\r\n" );
//}
//catch ( const RPW::Core::SocketException& e )
//{
// std::cout << ":: " << e.longErrorText() << std::endl;
//}
//catch ( const RPW::Core::LastErrorException& e )
//{
// std::cout << ":: " << e.longErrorText() << std::endl;
//}
//catch ( const RPW::Core::ErrnoException& e )
//{
// std::cout << ":: " << e.longErrorText() << std::endl;
//}
//catch ( const std::runtime_error& e )
//{
// std::cout << ":: " << e.what() << std::endl;
//}

namespace RPW
{
	namespace Core
	{

		class SmtpPlainTextMailSender
		{
		public:
			SmtpPlainTextMailSender( void );
			virtual ~SmtpPlainTextMailSender( void );

			static void Send( const char* const szSmtpServerName,
			                  const char* const szToAddr,
			                  const char* const szFromAlias,
			                  const char* const szFromAddr,
			                  const char* const szSubject,
			                  const char* const szMessageBody );

		private:
			static void Check1( int iStatus, const char* const szFunction );
			static void Check2( const char* const smtpResponse, unsigned long expectedNumber, const char* const szFunction );
		};

	}
}
