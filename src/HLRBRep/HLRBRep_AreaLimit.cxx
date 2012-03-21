// Created on: 1997-04-17
// Created by: Christophe MARION
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


#include <HLRBRep_AreaLimit.ixx>

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
