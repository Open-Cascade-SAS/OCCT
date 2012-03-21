// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 modifs suite a la premiere revue de projet

#include <gp_Lin.ixx>

Standard_Real gp_Lin::Distance (const gp_Lin& Other) const
{
  if (pos.IsParallel (Other.pos, gp::Resolution())) { 
    return Other.Distance(pos.Location());
  }
  else {
    gp_Dir dir(pos.Direction().Crossed(Other.pos.Direction()));
    Standard_Real D = gp_Vec (pos.Location(),Other.pos.Location())
      .Dot(gp_Vec(dir));
    if (D < 0) D = - D;
    return D;
  }
}

void gp_Lin::Mirror (const gp_Pnt& P)
{ pos.Mirror(P);  }

gp_Lin gp_Lin::Mirrored (const gp_Pnt& P)  const
{
  gp_Lin L = *this;    
  L.pos.Mirror (P);
  return L;
}

void gp_Lin::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Lin gp_Lin::Mirrored (const gp_Ax1& A1) const
{
  gp_Lin L = *this;
  L.pos.Mirror (A1);
  return L;
}

void gp_Lin::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Lin gp_Lin::Mirrored (const gp_Ax2& A2) const
{
  gp_Lin L = *this;
  L.pos.Mirror (A2);
  return L;
}

