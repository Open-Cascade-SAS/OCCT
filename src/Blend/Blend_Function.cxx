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
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Standard_DomainError.hxx>

Standard_Integer Blend_Function::NbVariables () const
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

Standard_Boolean Blend_Function::TwistOnS1() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::TwistOnS2() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::Section (const Blend_Point& /*P*/, 
                                          TColgp_Array1OfPnt& /*Poles*/, 
                                          TColgp_Array1OfVec& /*DPoles*/, 
                                          TColgp_Array1OfVec& /*D2Poles*/, 
                                          TColgp_Array1OfPnt2d& /*Poles2d*/, 
                                          TColgp_Array1OfVec2d& /*DPoles2d*/, 
                                          TColgp_Array1OfVec2d& /*D2Poles2d*/, 
                                          TColStd_Array1OfReal& /*Weigths*/, 
                                          TColStd_Array1OfReal& /*DWeigths*/,
                                          TColStd_Array1OfReal& /*D2Weigths*/)
{
  return Standard_False;
}
