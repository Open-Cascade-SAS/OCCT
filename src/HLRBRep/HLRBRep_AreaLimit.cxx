// Created on: 1997-04-17
// Created by: Christophe MARION
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


#include <HLRAlgo_Intersection.hxx>
#include <HLRBRep_AreaLimit.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_AreaLimit,Standard_Transient)

//=======================================================================
//function : HLRBRep_AreaLimit
//purpose  : 
//=======================================================================
HLRBRep_AreaLimit::HLRBRep_AreaLimit(const HLRAlgo_Intersection& V, 
				     const Standard_Boolean Boundary, 
				     const Standard_Boolean Interference, 
				     const TopAbs_State StateBefore,
				     const TopAbs_State StateAfter,
				     const TopAbs_State EdgeBefore,
				     const TopAbs_State EdgeAfter) :
				     myVertex      (V),
				     myBoundary    (Boundary),
				     myInterference(Interference),
				     myStateBefore (StateBefore),
				     myStateAfter  (StateAfter),
				     myEdgeBefore  (EdgeBefore),
				     myEdgeAfter   (EdgeAfter)
{
}

//=======================================================================
//function : StateBefore
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::StateBefore(const TopAbs_State Stat)
{
  myStateBefore = Stat;
}

//=======================================================================
//function : StateAfter
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::StateAfter(const TopAbs_State Stat)
{
  myStateAfter = Stat;
}

//=======================================================================
//function : EdgeBefore
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::EdgeBefore(const TopAbs_State Stat)
{
  myEdgeBefore = Stat;
}

//=======================================================================
//function : EdgeAfter
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::EdgeAfter(const TopAbs_State Stat)
{
  myEdgeAfter = Stat;
}

//=======================================================================
//function : Previous
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::Previous(const Handle(HLRBRep_AreaLimit)& P)
{
  myPrevious = P;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::Next(const Handle(HLRBRep_AreaLimit)& N)
{
  myNext = N;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const HLRAlgo_Intersection & HLRBRep_AreaLimit::Vertex() const
{
  return myVertex;
}


//=======================================================================
//function : IsBoundary
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_AreaLimit::IsBoundary() const
{
  return myBoundary;
}

//=======================================================================
//function : IsInterference
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_AreaLimit::IsInterference() const
{
  return myInterference;
}

//=======================================================================
//function : StateBefore
//purpose  : 
//=======================================================================

TopAbs_State  HLRBRep_AreaLimit::StateBefore() const
{
  return myStateBefore;
}

//=======================================================================
//function : StateAfter
//purpose  : 
//=======================================================================

TopAbs_State  HLRBRep_AreaLimit::StateAfter() const
{
  return myStateAfter;
}

//=======================================================================
//function : EdgeBefore
//purpose  : 
//=======================================================================

TopAbs_State  HLRBRep_AreaLimit::EdgeBefore() const
{
  return myEdgeBefore;
}

//=======================================================================
//function : EdgeAfter
//purpose  : 
//=======================================================================

TopAbs_State  HLRBRep_AreaLimit::EdgeAfter() const
{
  return myEdgeAfter;
}

//=======================================================================
//function : Previous
//purpose  : 
//=======================================================================

Handle(HLRBRep_AreaLimit) HLRBRep_AreaLimit::Previous () const
{
  return myPrevious;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

Handle(HLRBRep_AreaLimit) HLRBRep_AreaLimit::Next () const
{
  return myNext;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void HLRBRep_AreaLimit::Clear()
{
  myPrevious.Nullify();
  myNext.Nullify();
}
