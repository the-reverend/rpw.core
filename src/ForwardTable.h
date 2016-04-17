/// @file ForwardTable.h
/// @author rev

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h>
#include <vector>
#include <map>

namespace RPW
{
	namespace Core
	{
		/// @details Handles the operating system Forward Table (route table) and
		/// provides access to route information and other such stuff.
		class ForwardTable
		{
		public:

			struct route_info
			{
				unsigned long network;
				unsigned long mask;
				unsigned long next_hop;
				unsigned long net_interface;
				unsigned long local_ip; ///< may be 127.0.0.1 sometimes
			};

			typedef std::vector<route_info> route_vec;

			ForwardTable( void );
			~ForwardTable( void );

			route_vec getRoutes();
			unsigned long getLocalIpForDestination( unsigned long ip );
			unsigned long getLocalIpForDestination( const char* const ip_str );

		private:
			typedef std::vector<unsigned long> ip_addresses;
			typedef std::map<unsigned long, ip_addresses> interface_map;

			bool getRouteTable();

			bool getInterfaces( interface_map& int_map );
			void alignInterfaces( interface_map& int_map );
			route_info getRouteForIp( unsigned long ip );

			route_vec m_routes;
		};
	}
}