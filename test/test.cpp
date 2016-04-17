/// @file test.cpp
/// @author rev

#include "stdafx.h"

#include "testharness.h"
#include "XmlReportPrinter.h"

//#define NOGDI // avoid #define ERROR - conflicts with glog
#include "EventHandle.h"
#include "ForwardTable.h"
#include "SocketException.h"
#include "SocketProxy.h"
#include "StringEx.h"
#include "FileSystem.h"
#include "deelx.h"
#include "MD5.h"

#undef ERROR
#include <glog/logging.h>
#include <sstream>

using namespace easyunit;

class client1 : public RPW::Core::SelectSocket
{
public:
	client1( SOCKET sock = INVALID_SOCKET ) : RPW::Core::SelectSocket( sock ), received( NULL, FALSE, FALSE, NULL ) {}
	virtual void OnReceive( void )
	{
		int len = receive( buffer, sizeof( buffer ) - 1 );
		buffer[len] = 0;
		std::cout << "received on socket " <<  getSocket() << " '" << buffer << "'" << std::endl;
		received.set();
	}

	char buffer[4096];
	RPW::Core::EventHandle received;
};

class listener1 : public RPW::Core::SelectSocket
{
public:
	listener1() : accepted( NULL, FALSE, FALSE, NULL ) {}
	virtual void OnAccept( void )
	{
		client.Stop();
		client.destroy();
		RPW::Core::IpEndpoint ep;
		client.setSocket( accept( ep ) );
		std::cout << "twgs.client = " << client.getSocket() << " > " << ep.getAddressString() << ":" << ep.getPort() << std::endl;
		client.Start();
		accepted.set();
	}

	client1 client;
	RPW::Core::EventHandle accepted;
};

TEST( socketproxy, basic )
{
	LOG( INFO ) << "---- socketproxybasicTest ----";
	std::cout << "---- socketproxybasicTest ----" << std::endl;

#pragma region setup the twgs listener endpoint
	RPW::Core::IpEndpoint twgsEp( INADDR_ANY, 5023 );
	listener1 twgs;
	try
	{
		// setup the listener
		if ( twgs.create( AF_INET, SOCK_STREAM, IPPROTO_TCP )
		      && twgs.bind( twgsEp )
		      && twgs.listen() )
		{
			twgs.Start();
		}
		else
		{
			// fail if the listener socket isn't working
			twgs.destroy();
			FAIL();
		}
	}
	catch ( const RPW::Core::SocketException& )
	{
		twgs.destroy();
		FAIL();
	}
	std::cout << "twgs listener = " << twgs.getSocket() << " > " << twgsEp.getAddressString() << ":" << twgsEp.getPort() << std::endl;
#pragma endregion

#pragma region setup the proxy listener endpoint
	RPW::Core::IpEndpoint proxyEp( INADDR_ANY, 5044 );
	RPW::SocketProxy proxy( RPW::Core::IpEndpoint( "127.0.0.1", 5023 ) );
	try
	{
		// setup the listener
		if ( proxy.create( AF_INET, SOCK_STREAM, IPPROTO_TCP )
		      && proxy.bind( proxyEp )
		      && proxy.listen() )
		{
			proxy.Start();
		}
		else
		{
			// fail if the listener socket isn't working
			proxy.destroy();
			FAIL();
		}
	}
	catch ( const RPW::Core::SocketException& )
	{
		proxy.destroy();
		FAIL();
	}
	std::cout << "proxy listener = " << proxy.getSocket() << " > " << proxyEp.getAddressString() << ":" << proxyEp.getPort() << std::endl;
#pragma endregion

#pragma region setup proxy RHS socket
	RPW::Core::ForwardTable ft;
	unsigned long localIp = ft.getLocalIpForDestination( "127.0.0.1" );
	RPW::Core::IpEndpoint rhsEp( localIp, 0 );
	ASSERT_NOTHROW( ASSERT_TRUE_M( proxy.Rhs().create( AF_INET, SOCK_STREAM, IPPROTO_TCP ), "create failed" ), RPW::Core::SocketException );
	ASSERT_NOTHROW( ASSERT_TRUE_M( proxy.Rhs().bind( rhsEp ), "bind failed" ), RPW::Core::SocketException );
	rhsEp.getSockName( proxy.Rhs() );
	std::cout << "rhs = " << proxy.Rhs().getSocket() << " > " << rhsEp.getAddressString() << ":" << rhsEp.getPort() << std::endl;
#pragma endregion

#pragma region setup zoc client
	client1 zoc;
	RPW::Core::IpEndpoint zocEp( localIp, 0 );
	ASSERT_NOTHROW( ASSERT_TRUE_M( zoc.create( AF_INET, SOCK_STREAM, IPPROTO_TCP ), "create failed" ), RPW::Core::SocketException );
	ASSERT_NOTHROW( ASSERT_TRUE_M( zoc.bind( zocEp ), "bind failed" ), RPW::Core::SocketException );
	zocEp.getSockName( zoc );
	std::cout << "zoc = " << zoc.getSocket() << " > " << zocEp.getAddressString() << ":" << zocEp.getPort() << std::endl;
#pragma endregion

	std::cout << "connecting to proxy..." << std::endl;
	try
	{
		zoc.connect( RPW::Core::IpEndpoint( "127.0.0.1", 5044 ) );
		zoc.Start();
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << e.formatErrorMessage( zoc ) << std::endl;
	}
	DWORD dw1 = twgs.accepted.waitFor( 1000 );
	LOG( INFO ) << "dw1 = " << dw1;
	ASSERT_TRUE( dw1 != WAIT_TIMEOUT );
	LOG_IF( INFO, proxy.Rhs().isConnected() ) << "proxy rhs connected to twgs";
	ASSERT_TRUE_M( proxy.Rhs().isConnected(), "proxy rhs failed to connect to twgs" );

	zoc.send( "hi there", 8 );
	DWORD dw2 = twgs.client.received.waitFor( 1000 );
	LOG( INFO ) << "dw2 = " << dw1;
	ASSERT_TRUE( dw2 != WAIT_TIMEOUT );

	twgs.client.send( "back atcha", 10 );
	DWORD dw3 = zoc.received.waitFor( 1000 );
	LOG( INFO ) << "dw3 = " << dw1;
	ASSERT_TRUE( dw3 != WAIT_TIMEOUT );

	// clean up
	try
	{
		zoc.destroy();
		zoc.Stop();

		proxy.Rhs().destroy();
		proxy.Rhs().Stop();

		proxy.destroy();
		proxy.Stop();

		twgs.destroy();
		twgs.Stop();

		twgs.client.destroy();
		twgs.client.Stop();
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << e.formatErrorMessage( proxy ) << std::endl;
	}
}

TEST( StringEx, split1 )
{
	RPW::Core::StringEx s( " one \"two three\" " );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 2, v.size() );
}

TEST( StringEx, split2 )
{
	RPW::Core::StringEx s( "one two three" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 3, v.size() );
}

TEST( StringEx, split3 )
{
	RPW::Core::StringEx s( "one" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 1, v.size() );
}

TEST( StringEx, split4 )
{
	RPW::Core::StringEx s( "\"one\"" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 1, v.size() );
}

TEST( StringEx, split5 )
{
	RPW::Core::StringEx s( "\"one two\" three" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 2, v.size() );
}

TEST( StringEx, split6 )
{
	RPW::Core::StringEx s( "" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 0, v.size() );
}

TEST( StringEx, split7 )
{
	RPW::Core::StringEx s( "  " );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( suc );
	ASSERT_EQUALS_V( 0, v.size() );
}

TEST( StringEx, split8 )
{
	RPW::Core::StringEx s( "\"" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( !suc );
	ASSERT_EQUALS_V( 0, v.size() );
}

TEST( StringEx, split9 )
{
	RPW::Core::StringEx s( "\"one two" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( !suc );
	ASSERT_EQUALS_V( 0, v.size() );
}

TEST( StringEx, split10 )
{
	RPW::Core::StringEx s( "one \"two three" );

	std::vector<RPW::Core::StringEx> v;
	bool suc = s.split1( v );

	ASSERT_TRUE( !suc );
	ASSERT_EQUALS_V( 1, v.size() );
}

TEST( FileSystem, listing0 )
{
	RPW::Core::FileSystem fs;
	std::vector<std::string> vec;
	fs.ls( ".", vec, 1 );

}

TEST( FileSystem, executingassemblypath )
{
	RPW::Core::FileSystem fs;

	std::string abc = fs.GetExecutingAssemblyPath();

}

TEST( deelx, regex1 )
{
	// pattern and text
	char ptn[] = "\\d+\\.?\\d*|\\.\\d+";
	char txt[] = "such as 1, 234, 12.5, .78 etc";

	// compile
	CRegexpT<char> regexp( ptn );

	std::string pattern( regexp.m_builder.GetPattern().GetBuffer(), regexp.m_builder.GetPattern().GetSize() );
	ASSERT_EQUALS_V( std::string( ptn ), pattern );

	// find first
	MatchResult result = regexp.Match( txt );
	ASSERT_TRUE( result.IsMatched() != 0 );
	std::string r1( txt + result.GetStart(), result.GetEnd() - result.GetStart() );
	ASSERT_EQUALS_V( std::string( "1" ), r1 );

	result = regexp.Match( txt, result.GetEnd() );
	ASSERT_TRUE( result.IsMatched() != 0 );
	std::string r2( txt + result.GetStart(), result.GetEnd() - result.GetStart() );
	ASSERT_EQUALS_V( std::string( "234" ), r2 );

	result = regexp.Match( txt, result.GetEnd() );
	ASSERT_TRUE( result.IsMatched() != 0 );
	std::string r3( txt + result.GetStart(), result.GetEnd() - result.GetStart() );
	ASSERT_EQUALS_V( std::string( "12.5" ), r3 );

	result = regexp.Match( txt, result.GetEnd() );
	ASSERT_TRUE( result.IsMatched() != 0 );
	std::string r4( txt + result.GetStart(), result.GetEnd() - result.GetStart() );
	ASSERT_EQUALS_V( std::string( ".78" ), r4 );

	result = regexp.Match( txt, result.GetEnd() );
	ASSERT_TRUE( result.IsMatched() == 0 );
}

TEST( Socket, getpeername )
{
	RPW::Core::Socket sock;
	RPW::Core::IpEndpoint ep1( "192.168.2.199", 5023 );
	sock.create( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	sock.bind( RPW::Core::IpEndpoint( INADDR_ANY, 5023 ) );
	sock.connect( ep1 );

	RPW::Core::IpEndpoint ep2;
	sock.getPeerName( ep2 );

	ASSERT_EQUALS_V( ep1.getAddress(), ep2.getAddress() );
	ASSERT_EQUALS_V( ep1.getAddressString(), ep2.getAddressString() );
}

TEST( md5, rfc1321test1 )
{
	RPW::Core::MD5 md5;

	ASSERT_EQUALS_V( "d41d8cd98f00b204e9800998ecf8427e", md5.getHashFromString( "" ) );
	ASSERT_EQUALS_V( "0cc175b9c0f1b6a831c399e269772661", md5.getHashFromString( "a" ) );
	ASSERT_EQUALS_V( "900150983cd24fb0d6963f7d28e17f72", md5.getHashFromString( "abc" ) );
	ASSERT_EQUALS_V( "f96b697d7cb7938d525a2f31aaf161d0", md5.getHashFromString( "message digest" ) );
	ASSERT_EQUALS_V( "c3fcd3d76192e4007dfb496cca67e13b", md5.getHashFromString( "abcdefghijklmnopqrstuvwxyz" ) );
	ASSERT_EQUALS_V( "d174ab98d277d9f5a5611c2c9f419d9f", md5.getHashFromString( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" ) );
	ASSERT_EQUALS_V( "57edf4a22be3c955ac49da2e2107b67a", md5.getHashFromString( "12345678901234567890123456789012345678901234567890123456789012345678901234567890" ) );
}
