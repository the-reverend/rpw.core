// This is the C++ implementation of the MD5 Message-Digest
// Algorithm desrcipted in RFC 1321.
// I translated the C code from this RFC to C++.
// There is no warranty.
//
// Feb. 12. 2005
// Benjamin Grüdelbach

// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
//
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
//
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
//
// These notices must be retained in any copies of any part of this
// documentation and/or software.

#pragma once

#include <string>

namespace RPW
{
	namespace Core
	{
		class MD5Base
		{
		protected:
			typedef unsigned char* POINTER;

			struct MD5_CTX
			{
				unsigned long int state[4]; ///< state (ABCD)
				unsigned long int count[2]; ///< number of bits, modulo 2^64 (lsb first)
				unsigned char buffer[64];   ///< input buffer
			};

		public:
			MD5Base( void );
			virtual ~MD5Base( void );
			void MD5Init( MD5_CTX* context );
			void MD5Update( MD5_CTX* context, unsigned char* input, unsigned int inputLen );
			void MD5Final( unsigned char digest[16], MD5_CTX* context );

		protected:
			void MD5Transform( unsigned long int state[4], unsigned char block[64] );
			void Encode( unsigned char* output, unsigned long int* input, unsigned int len );
			void Decode( unsigned long int* output, unsigned char* input, unsigned int len );
			void MD5_memcpy( POINTER output, POINTER input, unsigned int len );
			void MD5_memset( POINTER output, int value, unsigned int len );
		};
	}
}
