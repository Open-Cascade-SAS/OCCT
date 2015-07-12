// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 modifs suite a la premiere revue de projet

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

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

