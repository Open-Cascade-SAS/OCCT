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

#include <PBRep_TEdge.ixx>

static const Standard_Integer ParameterMask       = 1;
static const Standard_Integer RangeMask           = 2;
static const Standard_Integer DegeneratedMask     = 4;

//=======================================================================
//function : PBRep_TEdge
//purpose  : 
//=======================================================================

PBRep_TEdge::PBRep_TEdge() : 
    myTolerance(0.),
    myFlags(0)
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TEdge::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge::SameParameter()const 
{
  return myFlags & ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void  PBRep_TEdge::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= ParameterMask;
  else   myFlags &= ~ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  PBRep_TEdge::SameRange()const 
{
  return myFlags & RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  PBRep_TEdge::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= RangeMask;
  else   myFlags &= ~RangeMask;
}

//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge::Degenerated()const 
{
  return myFlags & DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= DegeneratedMask; 
  else   myFlags &= ~DegeneratedMask;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_TEdge::Curves()const 
{
  return myCurves;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Curves(const Handle(PBRep_CurveRepresentation)& C)
{
  myCurves = C;
}


