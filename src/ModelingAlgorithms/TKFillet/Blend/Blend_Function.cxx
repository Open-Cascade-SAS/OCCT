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

#include <Blend_Function.hxx>
#include <Blend_Point.hxx>
#include <gp_Pnt.hxx>

int Blend_Function::NbVariables() const
{
  return 4;
}

const gp_Pnt& Blend_Function::Pnt1() const
{
  return PointOnS1();
}

const gp_Pnt& Blend_Function::Pnt2() const
{
  return PointOnS2();
}

bool Blend_Function::TwistOnS1() const
{
  return false;
}

bool Blend_Function::TwistOnS2() const
{
  return false;
}

bool Blend_Function::Section(const Blend_Point& /*P*/,
                                         NCollection_Array1<gp_Pnt>& /*Poles*/,
                                         NCollection_Array1<gp_Vec>& /*DPoles*/,
                                         NCollection_Array1<gp_Vec>& /*D2Poles*/,
                                         NCollection_Array1<gp_Pnt2d>& /*Poles2d*/,
                                         NCollection_Array1<gp_Vec2d>& /*DPoles2d*/,
                                         NCollection_Array1<gp_Vec2d>& /*D2Poles2d*/,
                                         NCollection_Array1<double>& /*Weigths*/,
                                         NCollection_Array1<double>& /*DWeigths*/,
                                         NCollection_Array1<double>& /*D2Weigths*/)
{
  return false;
}
