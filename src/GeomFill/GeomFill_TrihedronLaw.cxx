// Created on: 1997-12-05
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



#include <GeomFill_TrihedronLaw.ixx>

#include <Standard_NotImplemented.hxx>

void GeomFill_TrihedronLaw::SetCurve(const Handle(Adaptor3d_HCurve)& C) 
{
  myCurve = C;
  myTrimmed = myCurve;
}

//==================================================================
//Function : ErrorStatus
//Purpose :
//==================================================================
 GeomFill_PipeError GeomFill_TrihedronLaw::ErrorStatus() const
{
  return GeomFill_PipeOk;
}

 Standard_Boolean GeomFill_TrihedronLaw::D1(const Standard_Real,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec&,gp_Vec&,gp_Vec& ) 
{
  Standard_NotImplemented::Raise(" GeomFill_TrihedronLaw::D2");
  return Standard_False; 
}

 Standard_Boolean GeomFill_TrihedronLaw::D2(const Standard_Real,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec&,gp_Vec& ,gp_Vec&) 
{
  Standard_NotImplemented::Raise(" GeomFill_TrihedronLaw::D2");
  return Standard_False;
}

void GeomFill_TrihedronLaw::SetInterval(const Standard_Real First,
					 const Standard_Real Last) 
{
 myTrimmed = myCurve->Trim(First, Last, 0);  
}

 void GeomFill_TrihedronLaw::GetInterval(Standard_Real& First,
					 Standard_Real& Last) 
{
  First =  myTrimmed->FirstParameter();
  Last  =  myTrimmed->LastParameter();
}

 Standard_Boolean GeomFill_TrihedronLaw::IsConstant() const
{
  return Standard_False;
}

 Standard_Boolean GeomFill_TrihedronLaw::IsOnlyBy3dCurve() const
{
  return Standard_False;
}
