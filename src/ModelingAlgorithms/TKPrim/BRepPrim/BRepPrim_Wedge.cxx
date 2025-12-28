// Created on: 1995-01-09
// Created by: Modelistation
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

#include <BRepPrim_Wedge.hxx>
#include <gp_Ax2.hxx>

//=================================================================================================

BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2&       Axes,
                               const double dx,
                               const double dy,
                               const double dz)
    : BRepPrim_GWedge(BRepPrim_Builder(), Axes, dx, dy, dz)
{
}

//=================================================================================================

BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2&       Axes,
                               const double dx,
                               const double dy,
                               const double dz,
                               const double ltx)
    : BRepPrim_GWedge(BRepPrim_Builder(), Axes, dx, dy, dz, ltx)
{
}

//=================================================================================================

BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2&       Axes,
                               const double xmin,
                               const double ymin,
                               const double zmin,
                               const double z2min,
                               const double x2min,
                               const double xmax,
                               const double ymax,
                               const double zmax,
                               const double z2max,
                               const double x2max)
    : BRepPrim_GWedge(BRepPrim_Builder(),
                      Axes,
                      xmin,
                      ymin,
                      zmin,
                      z2min,
                      x2min,
                      xmax,
                      ymax,
                      zmax,
                      z2max,
                      x2max)
{
}
