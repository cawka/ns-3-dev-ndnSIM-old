// -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-
//
// Copyright (c) 2008 University of Washington
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-route.h"
#include "ipv4-global-routing-one-nexthop.h"

#include <iomanip>

NS_LOG_COMPONENT_DEFINE ("Ipv4GlobalRoutingOneNexthop");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Ipv4GlobalRoutingOneNexthop);

TypeId 
Ipv4GlobalRoutingOneNexthop::GetTypeId (void)
{ 
  static TypeId tid = TypeId ("ns3::Ipv4GlobalRoutingOneNexthop")
    .SetParent<Ipv4GlobalRouting> ()
  ;
  return tid;
}

void
Ipv4GlobalRoutingOneNexthop::AddRouteTo (Ipv4Address dest, 
                                         Ipv4Mask destMask, 
                                         Ipv4Address nextHop, 
                                         uint32_t interface,
                                         uint32_t metric/*=0*/)
{
  NS_LOG_FUNCTION (dest << nextHop << interface);

  // First, make sure we don't try to add route to ourselves
  int32_t iface = m_ipv4->GetInterfaceForPrefix (dest, destMask);
  NS_LOG_LOGIC ("Iface " << iface << " for " << dest);
  if (iface >= 0)
    {
      NS_LOG_LOGIC ("Do not add route to ourselves");
      return;
    }

  // Second, there is no reason to add p2p route that equals to the next hop
  if (destMask == Ipv4Mask::GetOnes () && dest == nextHop)
    {
      NS_LOG_LOGIC ("Ignore route to nexthop via nexhop");
      return;
    }
  
  Ipv4AddressTrie<Ipv4RoutingTableEntry>::iterator route =
    m_routes.find (dest.CombineMask (destMask));
  if (route == m_routes.end ())
    {
      NS_LOG_LOGIC ("Adding new route");
      m_routes[dest.CombineMask (destMask)] =
        Ipv4RoutingTableEntry::CreateNetworkRouteTo (dest, destMask, nextHop, interface, metric);
    }
  else
    if (route->second.GetMetric () > metric)
      {
        NS_LOG_LOGIC ("Replacing larger-metric route");
        m_routes[dest.CombineMask (destMask)] =
          Ipv4RoutingTableEntry::CreateNetworkRouteTo (dest, destMask, nextHop, interface, metric);
      }
    else
      {
        NS_LOG_LOGIC ("Don't update lower-metric route");
      }
}

Ptr<Ipv4Route>
Ipv4GlobalRoutingOneNexthop::LookupGlobal (Ipv4Address dest, Ptr<NetDevice> oif)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("Looking for route for destination " << dest);

  Ipv4AddressTrieMap::const_iterator longest_prefix_map = m_routes.longest_prefix_match (dest);
  if (longest_prefix_map == m_routes.end ())
    {
      NS_LOG_LOGIC ("Found " << longest_prefix_map->second);
      return 0;
    }

  if (oif != 0 && oif == m_ipv4->GetNetDevice (longest_prefix_map->second.GetInterface ()))
    {
      NS_LOG_LOGIC ("Route points to the incoming interface. Return empty route");
      return 0;
    }
  
  // create a Ipv4Route object from the selected routing table entry
  Ptr<Ipv4Route> rtentry = Create<Ipv4Route> ();
  rtentry->SetDestination (longest_prefix_map->second.GetDest ());
  rtentry->SetSource (m_ipv4->GetAddress (longest_prefix_map->second.GetInterface (), 0).GetLocal ());
  rtentry->SetGateway (longest_prefix_map->second.GetGateway ());
  rtentry->SetOutputDevice (m_ipv4->GetNetDevice (longest_prefix_map->second.GetInterface ()));
  return rtentry;
}

void
Ipv4GlobalRoutingOneNexthop::DeleteRoutes ()
{
  m_routes.clear ();
}

// Formatted like output of "route -n" command
void
Ipv4GlobalRoutingOneNexthop::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
{
  std::ostream* os = stream->GetStream ();
  if (m_routes.size () > 0)
    {
      *os << "Destination     Gateway         Genmask         Flags Metric Ref    Use Iface" << std::endl;
      for (Ipv4AddressTrieMap::const_iterator i=m_routes.begin (); i != m_routes.end (); i++)
        {
          std::ostringstream dest, gw, mask, flags;
          const Ipv4RoutingTableEntry &route = i->second;
          dest << route.GetDest ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << dest.str ();
          gw << route.GetGateway ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << gw.str ();
          mask << route.GetDestNetworkMask ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << mask.str ();
          flags << "U";
          if (route.IsHost ())
            {
              flags << "H";
            }
          else if (route.IsGateway ())
            {
              flags << "G";
            }
          *os << std::setiosflags (std::ios::left) << std::setw (6) << flags.str ();
          *os << std::setiosflags (std::ios::left) << std::setw (6) << route.GetMetric ();
          // Ref ct not implemented
          *os << "-" << "      ";
          // Use not implemented
          *os << "-" << "   ";
          if (Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ())) != "")
            {
              *os << Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ()));
            }
          else
            {
              *os << route.GetInterface ();
            }
          *os << std::endl;
        }
    }
}

Ptr<Ipv4Route>
Ipv4GlobalRoutingOneNexthop::RouteOutput (Ptr<Packet> p, const Ipv4Header &header,
                                          Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
//
// First, see if this is a multicast packet we have a route for.  If we
// have a route, then send the packet down each of the specified interfaces.
//
  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return 0; // Let other routing protocols try to handle this
    }
//
// See if this is a unicast packet we have a route for.
//
  NS_LOG_LOGIC ("Unicast destination- looking up");
  Ptr<Ipv4Route> rtentry = LookupGlobal (header.GetDestination (), oif);
  if (rtentry)
    {
      sockerr = Socket::ERROR_NOTERROR;
    }
  else
    {
      sockerr = Socket::ERROR_NOROUTETOHOST;
    }
  return rtentry;
}

bool 
Ipv4GlobalRoutingOneNexthop::RouteInput (Ptr<const Packet> p, const Ipv4Header &header,
                                         Ptr<const NetDevice> idev,
                                         UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                                         LocalDeliverCallback lcb, ErrorCallback ecb)
{ 

  NS_LOG_FUNCTION (this << p << header << header.GetSource () << header.GetDestination () << idev);
  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv4->GetInterfaceForDevice (idev);

  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return false; // Let other routing protocols try to handle this
    }

  if (header.GetDestination ().IsBroadcast ())
    {
      NS_LOG_LOGIC ("For me (Ipv4Addr broadcast address)");
      // TODO:  Local Deliver for broadcast
      // TODO:  Forward broadcast
    }

  // TODO:  Configurable option to enable RFC 1222 Strong End System Model
  // Right now, we will be permissive and allow a source to send us
  // a packet to one of our other interface addresses; that is, the
  // destination unicast address does not match one of the iif addresses,
  // but we check our other interfaces.  This could be an option
  // (to remove the outer loop immediately below and just check iif).
  for (uint32_t j = 0; j < m_ipv4->GetNInterfaces (); j++)
    {
      for (uint32_t i = 0; i < m_ipv4->GetNAddresses (j); i++)
        {
          Ipv4InterfaceAddress iaddr = m_ipv4->GetAddress (j, i);
          Ipv4Address addr = iaddr.GetLocal ();
          if (addr.IsEqual (header.GetDestination ()))
            {
              if (j == iif)
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match)");
                }
              else
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match) on another interface " << header.GetDestination ());
                }
              lcb (p, header, iif);
              return true;
            }
          if (header.GetDestination ().IsEqual (iaddr.GetBroadcast ()))
            {
              NS_LOG_LOGIC ("For me (interface broadcast address)");
              lcb (p, header, iif);
              return true;
            }
          NS_LOG_LOGIC ("Address "<< addr << " not a match");
        }
    }
  // Check if input device supports IP forwarding
  if (m_ipv4->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }
  // Next, try to find a route
  NS_LOG_LOGIC ("Unicast destination- looking up global route");
  Ptr<Ipv4Route> rtentry = LookupGlobal (header.GetDestination ());
  if (rtentry != 0)
    {
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");
      ucb (rtentry, p, header);
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Did not find unicast destination- returning false");
      return false; // Let other routing protocols try to handle this
                    // route request.
    }
}

} // namespace ns3
