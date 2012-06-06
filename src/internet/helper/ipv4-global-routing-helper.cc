/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "ipv4-global-routing-helper.h"
#include "ns3/global-router-interface.h"
#include "ns3/ipv4-global-routing.h"
#include "ns3/ipv4-global-routing-one-nexthop.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("GlobalRoutingHelper");

namespace ns3 {

UniformVariable Ipv4GlobalRoutingHelper::m_rand;

Ipv4GlobalRoutingHelper::Ipv4GlobalRoutingHelper (const std::string &type/* = "ns3::Ipv4GlobalRoutingOneNexthop"*/)
: m_type (type)
{
}

Ipv4GlobalRoutingHelper::Ipv4GlobalRoutingHelper (const Ipv4GlobalRoutingHelper &o)
: m_type (o.m_type)
{
}

Ipv4GlobalRoutingHelper*
Ipv4GlobalRoutingHelper::Copy (void) const
{
  return new Ipv4GlobalRoutingHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
Ipv4GlobalRoutingHelper::Create (Ptr<Node> node) const
{
  NS_LOG_LOGIC ("Adding GlobalRouter interface to node " <<
                node->GetId ());

  Ptr<GlobalRouter> globalRouter = CreateObject<GlobalRouter> ();
  node->AggregateObject (globalRouter);

  NS_LOG_LOGIC ("Adding GlobalRouting Protocol to node " << node->GetId ());

  ObjectFactory factory;
  factory.SetTypeId (m_type);
  Ptr<Ipv4GlobalRouting> globalRouting = DynamicCast<Ipv4GlobalRouting> (factory.Create<Object> ());

  return globalRouting;
}

void 
Ipv4GlobalRoutingHelper::PopulateRoutingTables (void)
{
  GlobalRouteManager::BuildGlobalRoutingDatabase ();
  GlobalRouteManager::InitializeRoutes ();
}

void
Ipv4GlobalRoutingHelper::PopulateAllPossibleRoutingTables (void)
{
  NodeList::Iterator listEnd = NodeList::End ();
  for (NodeList::Iterator node = NodeList::Begin (); node != listEnd; node++)
    {
      // if ((*node)->GetId ()!=3) continue;
      
      Ptr<Ipv4> ipv4 = (*node)->GetObject<Ipv4> ();
      NS_ASSERT (ipv4 != 0);

      // remember interface statuses
      std::vector<uint16_t> originalMetric (ipv4->GetNInterfaces ());
      for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
        {
          originalMetric[iface] = ipv4->GetMetric (iface);
        }

      UniformVariable m_rand;
      // enable interfaces one by one and calculate routes
      for (uint32_t enabledInterface = 1; enabledInterface < ipv4->GetNInterfaces (); enabledInterface++)
        {
          NS_LOG_ERROR ("Enabled interface: " << enabledInterface);
          
          for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
            {
              ipv4->SetMetric (iface,  m_rand.GetInteger (1, UINT16_MAX));
            }
          ipv4->SetMetric (enabledInterface, originalMetric[enabledInterface]);

          GlobalRouteManager::ClearLSDB ();
          GlobalRouteManager::BuildGlobalRoutingDatabase ();
          GlobalRouteManager::InitializeRoutes ();
        }

      // restore original interface statuses
      for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
        {
          ipv4->SetMetric (iface, originalMetric[iface]);
        }
    }  
}

// void
// Ipv4GlobalRoutingHelper::PopulateAllPossibleRoutingTables (void)
// {
//   for (int xx = 0; xx < 100; xx++)
//     {
//       for (NodeList::Iterator node = NodeList::Begin (); node != NodeList::End (); node++)
//         {
//           Ptr<Ipv4> ipv4 = (*node)->GetObject<Ipv4> ();
//           NS_ASSERT (ipv4 != 0);

//           // // remember interface statuses
//           // std::vector<uint16_t> originalMetric (ipv4->GetNInterfaces ());
//           // for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
//           //   {
//           //     originalMetric[iface] = ipv4->GetMetric (iface);
//           //   }

//           // enable interfaces one by one and calculate routes
//           // for (uint32_t enabledInterface = 1; enabledInterface < ipv4->GetNInterfaces (); enabledInterface++)
//           //   {
//           //     NS_LOG_ERROR ("Enabled interface: " << enabledInterface);
          
//               for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
//                 {
//                   ipv4->SetMetric (iface,  m_rand.GetInteger (1, 500));
//                 }
//               // ipv4->SetMetric (enabledInterface, originalMetric[enabledInterface]);

//             // }

//           // // restore original interface statuses
//           // for (uint32_t iface = 1; iface < ipv4->GetNInterfaces (); iface++)
//           //   {
//           //     ipv4->SetMetric (iface, originalMetric[iface]);
//           //   }
//         }  
//       GlobalRouteManager::ClearLSDB ();
//       GlobalRouteManager::BuildGlobalRoutingDatabase ();
//       GlobalRouteManager::InitializeRoutes ();

//       // Ptr<GlobalRouter> rtr = 
//       //   (*node)->GetObject<GlobalRouter> ();
      
//       // if (rtr && rtr->GetNumLSAs () )
//       //   {
//       //     SimulationSingleton<GlobalRouteManagerImpl>::Get ()->
//       //       SPFCalculate (rtr->GetRouterId ());
//       //   }
//     }
// }


void 
Ipv4GlobalRoutingHelper::RecomputeRoutingTables (void)
{
  GlobalRouteManager::DeleteGlobalRoutes ();
  GlobalRouteManager::BuildGlobalRoutingDatabase ();
  GlobalRouteManager::InitializeRoutes ();
}


} // namespace ns3
