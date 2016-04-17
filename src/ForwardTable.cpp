/// @file ForwardTable.cpp
/// @author rev

#include "StdAfx.h"
#include "ForwardTable.h"

#include "IpEndpoint.h"

#include <winsock2.h>
#include <iphlpapi.h>

RPW::Core::ForwardTable::ForwardTable( void )
{
	interface_map int_map;

	if ( getRouteTable()
	      && getInterfaces( int_map ) )
	{
		alignInterfaces( int_map );
	}
}

RPW::Core::ForwardTable::~ForwardTable( void )
{
}

bool RPW::Core::ForwardTable::getRouteTable()
{
	std::auto_ptr<unsigned char> buf;
	unsigned long ft_size = 0;
	DWORD ret = ::GetIpForwardTable( reinterpret_cast<MIB_IPFORWARDTABLE*>( buf.get() ), &ft_size, TRUE );

	if ( ret == ERROR_INSUFFICIENT_BUFFER )
	{
		buf = std::auto_ptr<unsigned char>( new unsigned char[ft_size] );
		ret = ::GetIpForwardTable( reinterpret_cast<MIB_IPFORWARDTABLE*>( buf.get() ), &ft_size, TRUE );

		if ( ret == NO_ERROR )
		{
			MIB_IPFORWARDTABLE* ft = reinterpret_cast<MIB_IPFORWARDTABLE*>( buf.get() );

			for ( DWORD i = 0; i < ft->dwNumEntries; ++i )
			{
				MIB_IPFORWARDROW& row = ft->table[i];
				route_info ri;
				ri.network = row.dwForwardDest;
				ri.mask = row.dwForwardMask;
				ri.net_interface = row.dwForwardIfIndex;
				ri.next_hop = row.dwForwardNextHop;
				ri.local_ip = 0;  //not used yet
				m_routes.push_back( ri );
			}

			return true;
		}
	}

	return false;
}

bool RPW::Core::ForwardTable::getInterfaces( interface_map& int_map )
{
	std::auto_ptr<unsigned char> buf;
	unsigned long buf_size = 0;
	unsigned long ret = ::GetAdaptersAddresses( AF_INET,
	                    0,   //all info
	                    0,
	                    reinterpret_cast<IP_ADAPTER_ADDRESSES*>( buf.get() ),
	                    &buf_size );

	if ( ret == ERROR_BUFFER_OVERFLOW )
	{
		buf = std::auto_ptr<unsigned char>( new unsigned char[buf_size] );
		ret = ::GetAdaptersAddresses( AF_INET,
		                              0,   //all info
		                              0,
		                              reinterpret_cast<IP_ADAPTER_ADDRESSES*>( buf.get() ),
		                              &buf_size );

		if ( ret == NO_ERROR )
		{
			IP_ADAPTER_ADDRESSES* aa = reinterpret_cast<IP_ADAPTER_ADDRESSES*>( buf.get() );

			while ( aa != 0 )
			{
				ip_addresses ip_addrs;
				unsigned long index = aa->IfIndex;
				IP_ADAPTER_UNICAST_ADDRESS* uni = aa->FirstUnicastAddress;

				while ( uni != 0 )
				{
					sockaddr_in& addr = *reinterpret_cast<sockaddr_in*>( uni->Address.lpSockaddr );
					ip_addrs.push_back( ::ntohl( addr.sin_addr.S_un.S_addr ) );
					uni = uni->Next;
				}

				int_map.insert( interface_map::value_type( index, ip_addrs ) );

				aa = aa->Next;
			}

			return true;
		}
	}

	return false;
}

/// @details Take the forward table info and the adapter info and mix the
/// addresses with the route table information.
void RPW::Core::ForwardTable::alignInterfaces( interface_map& int_map )
{
	route_vec::iterator rit = m_routes.begin();
	route_vec::iterator rend = m_routes.end();

	while ( rit != rend )
	{
		route_info& ri = *rit;
		interface_map::iterator mip = int_map.find( ri.net_interface );

		if ( mip != int_map.end() )
		{
			ip_addresses& ips = ( *mip ).second;

			if ( ips.size() == 1 )
			{
				ri.local_ip = ::htonl( *ips.begin() );
			}
			else
			{
				ip_addresses::iterator ip_it = ips.begin();
				ip_addresses::iterator ip_end = ips.end();

				while ( ip_it != ip_end )
				{
					if (( ri.network & ri.mask ) == ( ::htonl( *ip_it ) & ri.mask ) ||
					      ( ::htonl( *ip_it ) == ri.next_hop ) )
					{
						ri.local_ip = ::htonl( *ip_it );
						break;
					}

					++ip_it;
				}

				if ( ri.local_ip == 0 )
				{
					route_info ri2 = ri;
					int count = 0;

					while ( ri.local_ip == 0 && count++ < 5 )
					{
						ri2 = getRouteForIp( ri2.next_hop );
						ip_addresses::iterator ip_it = ips.begin();
						ip_addresses::iterator ip_end = ips.end();

						while ( ip_it != ip_end )
						{
							if (( ri2.network & ri2.mask ) == ( ::htonl( *ip_it ) & ri2.mask ) ||
							      ( ::htonl( *ip_it ) == ri2.next_hop ) )
							{
								ri.local_ip = ::htonl( *ip_it );
								break;
							}

							++ip_it;
						}
					}
				}
			}

		}

		++rit;
	}
}

RPW::Core::ForwardTable::route_vec RPW::Core::ForwardTable::getRoutes()
{
	return m_routes;
}

unsigned long RPW::Core::ForwardTable::getLocalIpForDestination( const char* const ip_str )
{
	unsigned long ip = ::inet_addr( ip_str );
	return getLocalIpForDestination( ip );
}

unsigned long RPW::Core::ForwardTable::getLocalIpForDestination( unsigned long dest )
{
	route_vec::reverse_iterator rit = m_routes.rbegin();
	route_vec::reverse_iterator rend = m_routes.rend();

	while ( rit != rend )
	{
		route_info& ri = *rit;

		if (( dest & ri.mask ) == ( ri.network & ri.mask ) || dest == ri.next_hop )
		{
			return ::ntohl( ri.local_ip );
		}

		++rit;
	}

	return 0;
}

RPW::Core::ForwardTable::route_info RPW::Core::ForwardTable::getRouteForIp( unsigned long ip )
{
	route_info ret_ri;
	route_vec::reverse_iterator rit = m_routes.rbegin();
	route_vec::reverse_iterator rend = m_routes.rend();

	while ( rit != rend )
	{
		route_info& ri = *rit;

		if (( ri.mask & ri.network ) == ( ri.mask & ip ) || ip == ri.next_hop )
		{
			ret_ri = ri;
		}

		++rit;
	}

	return ret_ri;
}
