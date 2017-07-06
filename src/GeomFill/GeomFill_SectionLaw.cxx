// Created on: 1997-11-21
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_SectionLaw,Standard_Transient)

Standard_Boolean GeomFill_SectionLaw::D1(const Standard_Real,TColgp_Array1OfPnt&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal& ) 
{
  throw Standard_NotImplemented("GeomFill_SectionLaw::D1");
}

 Standard_Boolean GeomFill_SectionLaw::D2(const Standard_Real,TColgp_Array1OfPnt& ,TColgp_Array1OfVec&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal&,TColStd_Array1OfReal&) 
{
  throw Standard_NotImplemented("GeomFill_SectionLaw::D2");
}

 Handle(Geom_BSplineSurface) GeomFill_SectionLaw::BSplineSurface() const
{
 Handle(Geom_BSplineSurface) BS;
 BS.Nullify();
 return BS;
}

 void GeomFill_SectionLaw::SetTolerance(const Standard_Real ,const Standard_Real) 
{
  //Ne fait Rien
}

 gp_Pnt GeomFill_SectionLaw::BarycentreOfSurf() const
{

  throw Standard_NotImplemented("GeomFill_SectionLaw::BarycentreOfSurf");
}

 void GeomFill_SectionLaw::GetMinimalWeight(TColStd_Array1OfReal&) const
{
  throw Standard_NotImplemented("GeomFill_SectionLaw::GetMinimalWeight");
}

 Standard_Boolean GeomFill_SectionLaw::IsConstant(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve)  GeomFill_SectionLaw::ConstantSection() const
{
 Handle(Geom_Curve) C;
 throw Standard_DomainError("GeomFill_SectionLaw::ConstantSection");
}

 Standard_Boolean GeomFill_SectionLaw::IsConicalLaw(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve) GeomFill_SectionLaw::
 CirclSection(const Standard_Real) const
{
 Handle(Geom_Curve) C;
 throw Standard_DomainError("GeomFill_SectionLaw::CirclSection");
}
