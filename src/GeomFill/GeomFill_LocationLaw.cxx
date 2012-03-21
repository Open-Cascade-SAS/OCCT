// Created on: 1997-11-21
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <GeomFill_LocationLaw.ixx>

Standard_Boolean GeomFill_LocationLaw::D1(const Standard_Real, gp_Mat&, gp_Vec&,gp_Mat&, gp_Vec&,TColgp_Array1OfPnt2d&,TColgp_Array1OfVec2d&) 
{
  Standard_NotImplemented::Raise("GeomFill_LocationLaw::D1");
  return 0;
}

 Standard_Boolean GeomFill_LocationLaw::D2(const Standard_Real,
					   gp_Mat&,gp_Vec&,
					   gp_Mat&, gp_Vec&,
					   gp_Mat&, gp_Vec&, 
					   TColgp_Array1OfPnt2d&,TColgp_Array1OfVec2d&,TColgp_Array1OfVec2d&) 
{
  Standard_NotImplemented::Raise("GeomFill_LocationLaw::D2");
  return 0;
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
  Standard_NotImplemented::Raise("GeomFill_LocationLaw::Resolution");
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
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::Rotation");
}
