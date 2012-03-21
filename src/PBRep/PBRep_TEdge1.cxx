// Created on: 1993-07-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

PBRep_TEdge1::PBRep_TEdge1() 
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


