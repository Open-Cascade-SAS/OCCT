// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>

//=================================================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const double R, const double H)
    : myCylinder(gp::XOY(), R, H)
{
}

//=================================================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const double R,
                                                   const double H,
                                                   const double Angle)
    : myCylinder(R, H)
{
  myCylinder.Angle(Angle);
}

//=================================================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,
                                                   const double  R,
                                                   const double  H)
    : myCylinder(Axes, R, H)
{
}

//=================================================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,
                                                   const double  R,
                                                   const double  H,
                                                   const double  Angle)
    : myCylinder(Axes, R, H)
{
  myCylinder.Angle(Angle);
}

//=================================================================================================

void* BRepPrimAPI_MakeCylinder::OneAxis()
{
  return &myCylinder;
}

//=================================================================================================

BRepPrim_Cylinder& BRepPrimAPI_MakeCylinder::Cylinder()
{
  return myCylinder;
}
