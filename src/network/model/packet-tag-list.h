/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
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
#ifndef PACKET_TAG_LIST_H
#define PACKET_TAG_LIST_H

#include <stdint.h>
#include <list>
#include "ns3/type-id.h"

namespace ns3 {

class Tag;

/**
 * \ingroup constants
 * \brief Tag maximum size
 * The maximum size (in bytes) of a Tag is stored
 * in this constant.
 */
// #define PACKET_TAG_MAX_SIZE 20

class PacketTagList : public std::list<Ptr<const Tag> >
{
public:
  // struct TagData {
  //   std::vector<uint8_t> data;
  //   TypeId tid;
  //   uint32_t count;
  // };

  // inline PacketTagList ();
  // inline PacketTagList (PacketTagList const &o);
  // inline PacketTagList &operator = (PacketTagList const &o);
  // inline ~PacketTagList ();

  void
  Add (Ptr<const Tag> tag);

  Ptr<const Tag>
  Remove (TypeId tagType);

  Ptr<const Tag>
  Peek (TypeId tagType) const;
};

} // namespace ns3

/****************************************************
 *  Implementation of inline methods for performance
 ****************************************************/

namespace ns3 {

// PacketTagList::PacketTagList ()
// {
// }

// PacketTagList::PacketTagList (PacketTagList const &o)
//   : m_tags (o.m_tags)
// {
// }

// PacketTagList &
// PacketTagList::operator = (PacketTagList const &o)
// {
//   // self assignment
//   if (&o == this)
//     {
//       return *this;
//     }
//   // RemoveAll (); // ???
//   m_tags = o.m_tags;
//   return *this;
// }

// PacketTagList::~PacketTagList ()
// {
//   RemoveAll ();
// }

// void
// PacketTagList::RemoveAll (void)
// {
//   m_tags.clear ();
// }

} // namespace ns3

#endif /* PACKET_TAG_LIST_H */
