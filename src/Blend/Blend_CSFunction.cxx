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


#include <Blend_CSFunction.hxx>
#include <Blend_Point.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>

Standard_Integer Blend_CSFunction::NbVariables () const
{
  return 3;
}

const gp_Pnt& Blend_CSFunction::Pnt1() const 
{
  return PointOnC();
}

const gp_Pnt& Blend_CSFunction::Pnt2() const 
{
  return PointOnS();
}

Standard_Boolean Blend_CSFunction::Section (const Blend_Point& /*P*/, 
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

Standard_Real Blend_CSFunction::GetMinimalDistance() const
{
  throw Standard_NotImplemented("Blend_CSFunction::GetMinimalDistance");
}
