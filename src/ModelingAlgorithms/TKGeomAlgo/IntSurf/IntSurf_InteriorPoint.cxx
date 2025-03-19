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

#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntSurf_InteriorPoint.hxx>

IntSurf_InteriorPoint::IntSurf_InteriorPoint()
    : paramu(0.0),
      paramv(0.0)
{
}

IntSurf_InteriorPoint::IntSurf_InteriorPoint(const gp_Pnt&       P,
                                             const Standard_Real U,
                                             const Standard_Real V,
                                             const gp_Vec&       Direc,
                                             const gp_Vec2d&     Direc2d)
    : point(P),
      paramu(U),
      paramv(V),
      direc(Direc),
      direc2d(Direc2d)

{
}

void IntSurf_InteriorPoint::SetValue(const gp_Pnt&       P,
                                     const Standard_Real U,
                                     const Standard_Real V,
                                     const gp_Vec&       Direc,
                                     const gp_Vec2d&     Direc2d)
{

  point   = P;
  paramu  = U;
  paramv  = V;
  direc   = Direc;
  direc2d = Direc2d;
}
