/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
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
#include "vector.h"
#include "fatal-error.h"
#include <cmath>
#include <sstream>

namespace ns3 {

ATTRIBUTE_HELPER_CPP (Vector3D);
ATTRIBUTE_HELPER_CPP (Vector2D);
// compatibility for mobility code
Ptr<const AttributeChecker> MakeVectorChecker (void)
{
  return MakeVector3DChecker ();
}


Vector3D::Vector3D (double _x, double _y, double _z)
  : x (_x),
    y (_y),
    z (_z)
{
}

Vector3D::Vector3D ()
  : x (0.0),
    y (0.0),
    z (0.0)
{
}

double
Vector3D::GetLength () const
{
  return sqrt (x*x + y*y + z*z);
}

Vector3D& 
Vector3D::operator += (const Vector3D &v)
{
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

Vector3D& 
Vector3D::operator += (double v)
{
  x += v;
  y += v;
  z += v;

  return *this;
}

Vector3D  
Vector3D::operator + (const Vector3D &v) const
{
  return Vector3D (x+v.x, y+v.y, z+v.z);
}

Vector3D  
Vector3D::operator + (double v) const
{
  return Vector3D (x+v, y+v, z+v);
}

Vector3D  
Vector3D::operator * (const Vector3D &v) const
{
  return Vector3D (x*v.x, y*v.y, z*v.z);
}

Vector3D  
Vector3D::operator * (double v) const
{
  return Vector3D (x*v, y*v, z*v);
}

Vector3D
Vector3D::operator - (const Vector3D &v) const
{
  return Vector3D (x-v.x, y-v.y, z-v.z);
}

Vector3D
Vector3D::operator / (double div) const
{
  return Vector3D (x/div, y/div, z/div);
}

Vector2D::Vector2D (double _x, double _y)
  : x (_x),
    y (_y)
{
}

Vector2D::Vector2D ()
  : x (0.0),
    y (0.0)
{
}

double
Vector2D::GetLength () const
{
  return sqrt (x*x + y*y);
}

Vector2D& 
Vector2D::operator += (const Vector2D &v)
{
  x += v.x;
  y += v.y;

  return *this;
}

Vector2D& 
Vector2D::operator += (double v)
{
  x += v;
  y += v;

  return *this;
}

Vector2D  
Vector2D::operator + (const Vector2D &v) const
{
  return Vector2D (x+v.x, y+v.y);
}

Vector2D  
Vector2D::operator + (double v) const
{
  return Vector2D (x+v, y+v);
}

Vector2D  
Vector2D::operator * (const Vector2D &v) const
{
  return Vector2D (x*v.x, y*v.y);
}

Vector2D  
Vector2D::operator * (double v) const
{
  return Vector2D (x*v, y*v);
}

Vector2D
Vector2D::operator - (const Vector2D &v) const
{
  return Vector2D (x-v.x, y-v.y);
}

Vector2D
Vector2D::operator / (double div) const
{
  return Vector2D (x/div, y/div);
}

double
CalculateDistance (const Vector3D &a, const Vector3D &b)
{
  double dx = b.x - a.x;
  double dy = b.y - a.y;
  double dz = b.z - a.z;
  double distance = std::sqrt (dx * dx + dy * dy + dz * dz);
  return distance;
}
double 
CalculateDistance (const Vector2D &a, const Vector2D &b)
{
  double dx = b.x - a.x;
  double dy = b.y - a.y;
  double distance = std::sqrt (dx * dx + dy * dy);
  return distance;
}


double
ScalarMultiplication (const Vector3D &a, const Vector3D &b)
{
  Vector3D tmp = a * b;
  return tmp.x + tmp.y + tmp.z;
}

double
ScalarMultiplication (const Vector2D &a, const Vector2D &b)
{
  Vector2D tmp = a * b;
  return tmp.x + tmp.y;
}

std::ostream &operator << (std::ostream &os, const Vector3D &vector)
{
  os << vector.x << ":" << vector.y << ":" << vector.z;
  return os;
}
std::istream &operator >> (std::istream &is, Vector3D &vector)
{
  char c1, c2;
  is >> vector.x >> c1 >> vector.y >> c2 >> vector.z;
  if (c1 != ':' ||
      c2 != ':')
    {
      is.setstate (std::ios_base::failbit);
    }
  return is;
}
std::ostream &operator << (std::ostream &os, const Vector2D &vector)
{
  os << vector.x << ":" << vector.y;
  return os;
}
std::istream &operator >> (std::istream &is, Vector2D &vector)
{
  char c1;
  is >> vector.x >> c1 >> vector.y;
  if (c1 != ':')
    {
      is.setstate (std::ios_base::failbit);
    }
  return is;
}

} // namespace ns3
