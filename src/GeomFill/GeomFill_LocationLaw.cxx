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


#include <Adaptor3d_HCurve.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_LocationLaw,Standard_Transient)

Standard_Boolean GeomFill_LocationLaw::D1(const Standard_Real, gp_Mat&, gp_Vec&,gp_Mat&, gp_Vec&,TColgp_Array1OfPnt2d&,TColgp_Array1OfVec2d&) 
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::D1");
}

 Standard_Boolean GeomFill_LocationLaw::D2(const Standard_Real,
					   gp_Mat&,gp_Vec&,
					   gp_Mat&, gp_Vec&,
					   gp_Mat&, gp_Vec&, 
					   TColgp_Array1OfPnt2d&,TColgp_Array1OfVec2d&,TColgp_Array1OfVec2d&) 
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::D2");
}

 Standard_Integer GeomFill_LocationLaw::Nb2dCurves() const
{
  Standard_Integer N = TraceNumber();
  if  (HasFirstRestriction()) N++;
  if  (HasLastRestriction()) N++;

  return N;
}

 Standard_Boolean GeomFill_LocationLaw::HasFirstRestriction() const
{
  return Standard_False;
}

 Standard_Boolean GeomFill_LocationLaw::HasLastRestriction() const
{
  return Standard_False;
}

 Standard_Integer GeomFill_LocationLaw::TraceNumber() const
{
  return 0;
}

//==================================================================
//Function : ErrorStatus
//Purpose :
//==================================================================
 GeomFill_PipeError GeomFill_LocationLaw::ErrorStatus() const
{
  return GeomFill_PipeOk;
}

// void GeomFill_LocationLaw::Resolution(const Standard_Integer Index,const Standard_Real Tol,Standard_Real& TolU,Standard_Real& TolV) const
 void GeomFill_LocationLaw::Resolution(const Standard_Integer ,const Standard_Real ,Standard_Real& ,Standard_Real& ) const
{
  throw Standard_NotImplemented("GeomFill_LocationLaw::Resolution");
}

 void GeomFill_LocationLaw::SetTolerance(const Standard_Real,
					 const Standard_Real ) 
{
 // Ne fait rien !!
}
 Standard_Boolean GeomFill_LocationLaw::IsTranslation(Standard_Real&) const
{
  return Standard_False;
}

 Standard_Boolean GeomFill_LocationLaw::IsRotation(Standard_Real&) const
{
  return Standard_False;
}
 void GeomFill_LocationLaw::Rotation(gp_Pnt&) const
{
  throw Standard_NotImplemented("GeomFill_SectionLaw::Rotation");
}
