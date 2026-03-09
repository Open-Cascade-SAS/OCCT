// Created on: 1994-02-23
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <GeomLProp.hxx>

#include <Geom_Curve.hxx>
#include <GeomProp_Curve.hxx>
#include <Precision.hxx>

//=================================================================================================

GeomAbs_Shape GeomLProp::Continuity(const occ::handle<Geom_Curve>& C1,
                                    const occ::handle<Geom_Curve>& C2,
                                    const double                   u1,
                                    const double                   u2,
                                    const bool                     r1,
                                    const bool                     r2,
                                    const double                   tl,
                                    const double                   ta)
{
  return GeomProp_Curve::Continuity(C1, C2, u1, u2, r1, r2, tl, ta);
}

//=================================================================================================

GeomAbs_Shape GeomLProp::Continuity(const occ::handle<Geom_Curve>& C1,
                                    const occ::handle<Geom_Curve>& C2,
                                    const double                   u1,
                                    const double                   u2,
                                    const bool                     r1,
                                    const bool                     r2)
{
  return GeomProp_Curve::Continuity(C1, C2, u1, u2, r1, r2);
}
