/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
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
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "point-to-point-loss-model.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/packet.h"
#include <math.h>

NS_LOG_COMPONENT_DEFINE ("PointToPointLossModel");

namespace ns3 {

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (PointToPointLossModel);

TypeId 
PointToPointLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PointToPointLossModel")
    .SetParent<Object> ()
  ;
  return tid;
}

PointToPointLossModel::PointToPointLossModel ()
  : m_next (0)
{
}

PointToPointLossModel::~PointToPointLossModel ()
{
}

void
PointToPointLossModel::SetNext (Ptr<PointToPointLossModel> next)
{
  m_next = next;
}

bool
PointToPointLossModel::IsLoss (Ptr<const Packet> pkt)
{
  bool loss = DoIsLoss (pkt);
  
  if (!loss && m_next != 0)
    {
      loss = m_next->IsLoss (pkt);
    }
  return loss;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (RandomPointToPointLossModel);

TypeId 
RandomPointToPointLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RandomPointToPointLossModel")
    .SetParent<PointToPointLossModel> ()
    .AddConstructor<RandomPointToPointLossModel> ()
    .AddAttribute ("Variable", "The random variable used to pick a random los",
                   RandomVariableValue (UniformVariable ()),
                   MakeRandomVariableAccessor (&RandomPointToPointLossModel::m_variable),
                   MakeRandomVariableChecker ())
    .AddAttribute ("Probability", "Loss probability",
                   DoubleValue (-1), // impossible
                   MakeDoubleAccessor (&RandomPointToPointLossModel::m_probability),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}
RandomPointToPointLossModel::RandomPointToPointLossModel ()
  : PointToPointLossModel ()
{
}

RandomPointToPointLossModel::~RandomPointToPointLossModel ()
{
}

bool
RandomPointToPointLossModel::DoIsLoss (Ptr<const Packet> pkt)
{
  bool loss = (m_variable.GetValue () <= m_probability);
  NS_LOG_DEBUG ("Loss: " << loss);
  return loss;
}


// ------------------------------------------------------------------------- //

} // namespace ns3
