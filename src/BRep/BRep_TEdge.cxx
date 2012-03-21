// Created on: 1992-08-25
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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



#include <BRep_TEdge.ixx>
#include <TopAbs.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_CurveOn2Surfaces.hxx>

static const Standard_Integer ParameterMask       = 1;
static const Standard_Integer RangeMask           = 2;
static const Standard_Integer DegeneratedMask     = 4;

//=======================================================================
//function : BRep_TEdge
//purpose  : 
//=======================================================================

BRep_TEdge::BRep_TEdge() :
       TopoDS_TEdge(),
       myTolerance(RealEpsilon()),
       myFlags(0)
{
  SameParameter(Standard_True);
  SameRange(Standard_True);
}

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::SameParameter()const 
{
  return myFlags & ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

 void  BRep_TEdge::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= ParameterMask;
  else   myFlags &= ~ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::SameRange()const 
{
  return myFlags & RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  BRep_TEdge::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= RangeMask;
  else   myFlags &= ~RangeMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::Degenerated()const 
{
  return myFlags & DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

 void  BRep_TEdge::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= DegeneratedMask; 
  else   myFlags &= ~DegeneratedMask;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TEdge::EmptyCopy() const
{
  Handle(BRep_TEdge) TE = 
    new BRep_TEdge();
  TE->Tolerance(myTolerance);
  // copy the curves representations
  BRep_ListOfCurveRepresentation& l = TE->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation itr(myCurves);
  
  while (itr.More()) {
    // on ne recopie PAS les polygones
    if ( itr.Value()->IsKind(STANDARD_TYPE(BRep_GCurve)) ||
         itr.Value()->IsKind(STANDARD_TYPE(BRep_CurveOn2Surfaces)) ) {
      l.Append(itr.Value()->Copy());
    }
    itr.Next();
  }

  TE->Degenerated(Degenerated());
  TE->SameParameter(SameParameter());
  TE->SameRange(SameRange());
  
  return TE;
}

