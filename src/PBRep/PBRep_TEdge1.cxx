// Created on: 1993-07-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PBRep_TEdge1.ixx>

// Enum terms are better than statics who must be intialized.
enum {
  PBRep_ParameterMask       = 1,
  PBRep_RangeMask           = 2,
  PBRep_DegeneratedMask     = 4
};

//=======================================================================
//function : PBRep_TEdge1
//purpose  : 
//=======================================================================

PBRep_TEdge1::PBRep_TEdge1() : 
    myTolerance(0.),
    myFlags(0)
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TEdge1::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge1::SameParameter()const 
{
  return myFlags & PBRep_ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_ParameterMask;
  else   myFlags &= ~PBRep_ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  PBRep_TEdge1::SameRange()const 
{
  return myFlags & PBRep_RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  PBRep_TEdge1::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_RangeMask;
  else   myFlags &= ~PBRep_RangeMask;
}

//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge1::Degenerated()const 
{
  return myFlags & PBRep_DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_DegeneratedMask; 
  else   myFlags &= ~PBRep_DegeneratedMask;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_TEdge1::Curves()const 
{
  return myCurves;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Curves(const Handle(PBRep_CurveRepresentation)& C)
{
  myCurves = C;
}


