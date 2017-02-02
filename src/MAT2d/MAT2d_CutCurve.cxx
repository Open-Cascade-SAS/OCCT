// Created on: 1994-09-23
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <MAT2d_CutCurve.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
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
  if (UnModified()) {throw Standard_OutOfRange();}
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
  if (UnModified()) {throw Standard_OutOfRange();}
  if ( Index < 1 || Index > theCurves.Length()) {
    throw Standard_OutOfRange();
  }
  return Handle(Geom2d_TrimmedCurve)::DownCast(theCurves.Value(Index));
}


