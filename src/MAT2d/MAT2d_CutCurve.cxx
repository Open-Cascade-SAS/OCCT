// Created on: 1994-09-23
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <MAT2d_CutCurve.ixx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>

//=======================================================================
//function : MAT2d_CutCurve
//purpose  : 
//=======================================================================

MAT2d_CutCurve::MAT2d_CutCurve()
{
}


//=======================================================================
//function : MAT2d_CutCurve
//purpose  : 
//=======================================================================

MAT2d_CutCurve::MAT2d_CutCurve(const Handle(Geom2d_Curve)& C)
{
  Perform (C);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void MAT2d_CutCurve::Perform(const Handle(Geom2d_Curve)& C)
{
  theCurves.Clear();

  Geom2dLProp_CurAndInf2d     Sommets;
  Handle(Geom2d_TrimmedCurve) TrimC;
  Standard_Real               UF,UL,UC;
  gp_Pnt2d                    PF,PL,PC;
  Standard_Real               PTol  = Precision::PConfusion()*10;
  Standard_Real               Tol   = Precision::Confusion()*10;
  Standard_Boolean            YaCut = Standard_False;
  Sommets.Perform (C);

  if (Sommets.IsDone() && !Sommets.IsEmpty()) {
    UF = C->FirstParameter();
    UL = C->LastParameter ();
    PF = C->Value(UF);
    PL = C->Value(UL);

    for (Standard_Integer i = 1; i <= Sommets.NbPoints(); i++) {
      UC = Sommets.Parameter(i);
      
      PC = C->Value(UC);
      if (UC - UF > PTol && PC.Distance(PF) > Tol) {
	if ( UL - UC < PTol || PL.Distance(PC) < Tol) {
	  break;
	}
	TrimC = new Geom2d_TrimmedCurve(C,UF,UC);
	theCurves.Append(TrimC);
	UF    = UC;
	PF    = PC;
	YaCut = Standard_True;
      }
    }
    if (YaCut) {
      TrimC = new Geom2d_TrimmedCurve(C,UF,UL);
      theCurves.Append(TrimC);
    }
  }
}


//=======================================================================
//function : UnModified
//purpose  : 
//=======================================================================

Standard_Boolean MAT2d_CutCurve::UnModified() const 
{
  return theCurves.IsEmpty();
}


//=======================================================================
//function : NbCurves
//purpose  : 
//=======================================================================

Standard_Integer MAT2d_CutCurve::NbCurves() const 
{
  if (UnModified()) {Standard_OutOfRange::Raise();}
  return theCurves.Length();
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Geom2d_TrimmedCurve) MAT2d_CutCurve::Value (
   const Standard_Integer Index) 
const 
{
  if (UnModified()) {Standard_OutOfRange::Raise();}
  if ( Index < 1 || Index > theCurves.Length()) {
    Standard_OutOfRange::Raise();
  }
  return Handle(Geom2d_TrimmedCurve)::DownCast(theCurves.Value(Index));
}


