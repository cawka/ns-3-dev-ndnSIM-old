/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
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

#ifndef TYPE_TAG_H
#define TYPE_TAG_H

#include <ns3/tag.h>
#include <ns3/type-id.h>

using namespace ns3;

class TypeTag : public Tag
{
public:
  static TypeId GetTypeId (void)
  {
    static TypeId tid = TypeId ("TypeTag")
      .SetParent<Tag> ()
      ;
    return tid;
  }

  enum Type
    {
      DATA,
      INTEREST,
      DATA2,
      INTEREST2
    };

  TypeTag (Type type)
  : m_type (type)
  {
  }

  Type GetType () const
  {
    return m_type;
  }
  
  virtual uint32_t GetSerializedSize (void) const
  {
    return 0;
  }

  virtual void Serialize (TagBuffer i) const
  {
    return;
  }
  
  virtual void Deserialize (TagBuffer i)
  {
    return;
  }

  virtual void Print (std::ostream &os) const
  {
    switch (m_type)
      {
      case DATA:
      case DATA2:
        os << "DATA";
        break;
      case INTEREST:
      case INTEREST2:
        os << "INTEREST";
        break;
      }
  }

private:
  Type m_type;
};

#endif // TYPE_TAG_H
