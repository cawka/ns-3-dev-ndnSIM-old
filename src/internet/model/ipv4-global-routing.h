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
//

#ifndef IPV4_GLOBAL_ROUTING_H
#define IPV4_GLOBAL_ROUTING_H

#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ptr.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"

namespace ns3 {

class Packet;
class NetDevice;

/**
 * \brief Global routing protocol for IP version 4 stacks.
 *
 * In ns-3 we have the concept of a pluggable routing protocol.  Routing
 * protocols are added to a list maintained by the Ipv4L3Protocol.  Every 
 * stack gets one routing protocol for free -- the Ipv4StaticRouting routing
 * protocol is added in the constructor of the Ipv4L3Protocol (this is the 
 * piece of code that implements the functionality of the IP layer).
 *
 * As an option to running a dynamic routing protocol, a GlobalRouteManager
 * object has been created to allow users to build routes for all participating
 * nodes.  One can think of this object as a "routing oracle"; it has
 * an omniscient view of the topology, and can construct shortest path
 * routes between all pairs of nodes.  These routes must be stored 
 * somewhere in the node, so therefore this class Ipv4GlobalRouting
 * is used as one of the pluggable routing protocols.  It is kept distinct
 * from Ipv4StaticRouting because these routes may be dynamically cleared
 * and rebuilt in the middle of the simulation, while manually entered
 * routes into the Ipv4StaticRouting may need to be kept distinct.
 *
 * This class deals with Ipv4 unicast routes only.
 *
 * \see Ipv4RoutingProtocol
 * \see GlobalRouteManager
 */
class Ipv4GlobalRouting : public Ipv4RoutingProtocol
{
public:
  static TypeId GetTypeId (void);

  Ipv4GlobalRouting ();

  // These methods inherited from base class
  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void SetIpv4 (Ptr<Ipv4> ipv4);

/**
 * \brief Add a route to the global routing table.
 *
 * \param dest The Ipv4Address destination for this route.
 * \param destMask The Ipv4Mask to extract the network.
 * \param nextHop The Ipv4Address of the next hop in the route.
 * \param interface The network interface index used to send packets to the
 * destination.
 *
 * \see Ipv4Address
 */
  virtual void AddRouteTo (Ipv4Address dest, 
                           Ipv4Mask destMask, 
                           Ipv4Address nextHop, 
                           uint32_t interface,
                           uint32_t metric=0) = 0;

  /**
   * \brief Function to remove all routes
   */
  virtual void DeleteRoutes () = 0;

protected:
  // virtual Ptr<Ipv4Route> LookupGlobal (Ipv4Address dest, Ptr<NetDevice> oif = 0) = 0;
  
  Ptr<Ipv4> m_ipv4;

private:
  bool m_respondToInterfaceEvents;
};

} // Namespace ns3

#endif /* IPV4_GLOBAL_ROUTING_H */
