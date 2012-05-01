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

#ifndef POINT_TO_POINT_LOSS_MODEL_H
#define POINT_TO_POINT_LOSS_MODEL_H

#include "ns3/object.h"
#include "ns3/random-variable.h"
#include "ns3/ptr.h"

namespace ns3 {

class Packet;

/**
 * \defgroup point-to-point Loss Models
 *
 */

/**
 * \ingroup point-to-point
 *
 * \brief Model losses in point-to-point links
 */
class PointToPointLossModel : public Object
{
public:
  static TypeId GetTypeId (void);

  PointToPointLossModel ();
  virtual ~PointToPointLossModel ();

  /**
   * \brief Enables a chain of loss models to act on the signal
   * \param next The next PointToPointLossModel to add to the chain
   */
  void
  SetNext (Ptr<PointToPointLossModel> next);

  /**
   * \brief Decide whether or not packet should be lost
   */
  bool
  IsLoss (Ptr<const Packet> pkt);
  
private:
  PointToPointLossModel (const PointToPointLossModel &o);
  PointToPointLossModel &operator = (const PointToPointLossModel &o);

  virtual bool
  DoIsLoss (Ptr<const Packet> pkt) = 0;
  
  Ptr<PointToPointLossModel> m_next;
};

/**
 * \ingroup propagation
 *
 * \brief The propagation loss follows a random distribution.
 */ 
class RandomPointToPointLossModel : public PointToPointLossModel
{
public:
  static TypeId GetTypeId (void);

  RandomPointToPointLossModel ();
  virtual ~RandomPointToPointLossModel ();

private:
  RandomPointToPointLossModel (const RandomPointToPointLossModel &o);
  RandomPointToPointLossModel & operator = (const RandomPointToPointLossModel &o);

  virtual bool
  DoIsLoss (Ptr<const Packet> pkt);

private:
  RandomVariable m_variable;
  double m_probability;
};


} // namespace ns3

#endif /* POINT_TO_POINT_LOSS_MODEL_H */
