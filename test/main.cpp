/// @file main.cpp
/// @author rev

#include "stdafx.h"

#include "testharness.h"
#define NOGDI // avoid #define ERROR - conflicts with glog
#include "WsaData.h"
#include <glog/logging.h>

using namespace easyunit;

int main( int argc, const char* const argv[] )
{
	google::InitGoogleLogging( argv[0] );

	RPW::Core::WsaData wsa;

	TestRegistry::runAndPrint();

	return 0;
}
