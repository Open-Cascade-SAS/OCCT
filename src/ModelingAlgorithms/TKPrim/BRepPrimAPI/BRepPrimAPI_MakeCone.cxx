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

#include <BRepPrimAPI_MakeCone.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>

//=================================================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const double R1,
                                           const double R2,
                                           const double H)
    : myCone(gp::XOY(), R1, R2, H)
{
}

//=================================================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const double R1,
                                           const double R2,
                                           const double H,
                                           const double angle)
    : myCone(R1, R2, H)
{
  myCone.Angle(angle);
}

//=================================================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const gp_Ax2&       Axes,
                                           const double R1,
                                           const double R2,
                                           const double H)
    : myCone(Axes, R1, R2, H)
{
}

//=================================================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const gp_Ax2&       Axes,
                                           const double R1,
                                           const double R2,
                                           const double H,
                                           const double angle)
    : myCone(Axes, R1, R2, H)
{
  myCone.Angle(angle);
}

//=================================================================================================

void* BRepPrimAPI_MakeCone::OneAxis()
{
  return &myCone;
}

//=================================================================================================

BRepPrim_Cone& BRepPrimAPI_MakeCone::Cone()
{
  return myCone;
}
