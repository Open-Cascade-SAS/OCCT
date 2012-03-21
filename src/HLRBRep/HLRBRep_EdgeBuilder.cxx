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


#ifndef No_Exception
#define No_Exception
#endif
#include <HLRBRep_EdgeBuilder.ixx>

#include <TopAbs.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : HLRBRep_EdgeBuilder
//purpose  : 
//=======================================================================


HLRBRep_EdgeBuilder::HLRBRep_EdgeBuilder (HLRBRep_VertexList& VList)
{
  // at creation the EdgeBuilder explore the VertexList
  // and use it to build a list of "AreaLimit" on the edge.
  // An area is a part of the curve between 
  // two consecutive vertices
  
  Standard_DomainError_Raise_if(!VList.More(),
				"EdgeBuilder  : Empty vertex list");

  Handle(HLRBRep_AreaLimit) last,cur;
  TopAbs_State before,after,ebefore,eafter;
  HLRAlgo_Intersection V;

  // loop on the Vertices
  for (;VList.More();VList.Next()) {
    before = after = ebefore = eafter = TopAbs_UNKNOWN;
    // compute the states
    if (VList.IsBoundary()) {
      switch (VList.Orientation()) {

      case TopAbs_FORWARD :
	ebefore = TopAbs_OUT;
	eafter  = TopAbs_IN;
	break;

      case TopAbs_REVERSED :
	ebefore = TopAbs_IN;
	eafter  = TopAbs_OUT;
	break;

      case TopAbs_INTERNAL :
	ebefore = TopAbs_IN;
	eafter  = TopAbs_IN;
	break;

      case TopAbs_EXTERNAL :
	ebefore = TopAbs_OUT;
	eafter  = TopAbs_OUT;
	break;
      }
    }

    if (VList.IsInterference()) {
      switch (VList.Transition()) {

      case TopAbs_FORWARD :
	before = TopAbs_OUT;
	after  = TopAbs_IN;
	break;

      case TopAbs_REVERSED :
	before = TopAbs_IN;
	after  = TopAbs_OUT;
	break;

      case TopAbs_INTERNAL :
	before = TopAbs_IN;
	after  = TopAbs_IN;
	break;

      case TopAbs_EXTERNAL :
	before = TopAbs_OUT;
	after  = TopAbs_OUT;
	break;
      }

      switch (VList.BoundaryTransition()) {

      case TopAbs_FORWARD :
	after  = TopAbs_ON;
	break;

      case TopAbs_REVERSED :
	before = TopAbs_ON;
	break;

      case TopAbs_INTERNAL :
	before = TopAbs_ON;
	after  = TopAbs_ON;
	break;

      case TopAbs_EXTERNAL :
	break;
      }
    }

    // create the Limit and connect to list
    V = VList.Current();
    cur = new HLRBRep_AreaLimit(V,
				 VList.IsBoundary(),
				 VList.IsInterference(),
				 before,after,
				 ebefore,eafter);
    if (myLimits.IsNull()) {
      myLimits = cur;
      last     = cur;
    }
    else {
      last->Next(cur);
      cur->Previous(last);
      last = cur;
    }
  }

  // periodicity, make a circular list
  if (VList.IsPeriodic()) {
    last->Next(myLimits);
    myLimits->Previous(last);
  }

  // process UNKNOWN areas
  TopAbs_State  stat = TopAbs_UNKNOWN;
  TopAbs_State estat = TopAbs_UNKNOWN;

  cur = myLimits;
  while (!cur.IsNull()) {
    if (stat == TopAbs_UNKNOWN) {
      stat = cur->StateBefore();
      if (stat == TopAbs_UNKNOWN) {
	stat = cur->StateAfter();
      }
    }
    if (estat == TopAbs_UNKNOWN) {
      estat = cur->EdgeBefore();
      if (estat == TopAbs_UNKNOWN) {
	estat = cur->EdgeAfter();
      }
    }
    cur = cur->Next();
    // test for periodicicity
    if (cur == myLimits)
      break;
  }

  // error if no interferences
  Standard_DomainError_Raise_if(stat == TopAbs_UNKNOWN,
				"EdgeBuilder : No interferences");
  // if no boundary the edge covers the whole curve
  if (estat == TopAbs_UNKNOWN)
    estat = TopAbs_IN;
  
  // propagate states
  cur = myLimits;
  while (!cur.IsNull()) {
    if (cur->StateBefore() == TopAbs_UNKNOWN)
      cur->StateBefore(stat);
    else
      stat = cur->StateAfter();
    if (cur->StateAfter() == TopAbs_UNKNOWN)
      cur->StateAfter(stat);
    if (cur->EdgeBefore() == TopAbs_UNKNOWN)
      cur->EdgeBefore(estat);
    else
      estat = cur->EdgeAfter();
    if (cur->EdgeAfter() == TopAbs_UNKNOWN)
      cur->EdgeAfter(estat);

    cur = cur->Next();
    if (cur == myLimits)
      break;
  }

  // initialise with IN parts
  Builds(TopAbs_IN);
}

//=======================================================================
//function : InitAreas
//purpose  : set on the first area
//=======================================================================

void HLRBRep_EdgeBuilder::InitAreas()
{
  left = myLimits->Previous();
  right = myLimits;
}

//=======================================================================
//function : NextArea
//purpose  : 
//=======================================================================

void HLRBRep_EdgeBuilder::NextArea()
{
  left = right;
  if (!right.IsNull())
    right = right->Next();
}

//=======================================================================
//function : PreviousArea
//purpose  : 
//=======================================================================

void HLRBRep_EdgeBuilder::PreviousArea()
{
  right = left;
  if (!left.IsNull())
    left = left->Previous();
}

//=======================================================================
//function : HasArea
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_EdgeBuilder::HasArea() const
{
  if (left.IsNull())
    if (right.IsNull()) return Standard_False;
  if (right == myLimits) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : AreaState
//purpose  : 
//=======================================================================

TopAbs_State HLRBRep_EdgeBuilder::AreaState() const
{
  TopAbs_State stat = TopAbs_UNKNOWN;
  if (!left.IsNull())
    stat = left->StateAfter();
  if (!right.IsNull())
    stat = right->StateBefore();
  return stat;
}

//=======================================================================
//function : AreaEdgeState
//purpose  : 
//=======================================================================

TopAbs_State HLRBRep_EdgeBuilder::AreaEdgeState() const
{
  TopAbs_State stat = TopAbs_UNKNOWN;
  if (!left.IsNull())
    stat = left->EdgeAfter();
  if (!right.IsNull())
    stat = right->EdgeBefore();
  return stat;
}

//=======================================================================
//function : LeftLimit
//purpose  : 
//=======================================================================

Handle(HLRBRep_AreaLimit) HLRBRep_EdgeBuilder::LeftLimit() const
{
  return left;
}

//=======================================================================
//function : RightLimit
//purpose  : 
//=======================================================================

Handle(HLRBRep_AreaLimit) HLRBRep_EdgeBuilder::RightLimit() const
{
  return right;
}

//=======================================================================
//function : Builds
//purpose  : 
//=======================================================================

void  HLRBRep_EdgeBuilder::Builds(const TopAbs_State ToBuild)
{
  toBuild = ToBuild;
  InitAreas();
  do {
    if ((AreaState() == toBuild) &&
	(AreaEdgeState() == TopAbs_IN)) {
      if (left.IsNull())
	current = 2;
      else
	current = 1;
      return;
    }
    NextArea();
  }
  while (HasArea());
  current = 3;
}

//=======================================================================
//function : MoreEdges
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_EdgeBuilder::MoreEdges() const
{
  return HasArea();
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void  HLRBRep_EdgeBuilder::NextEdge()
{
  // clean the current edge
  while (AreaState() == toBuild)
    NextArea();
  // go to the next edge
  while (HasArea()) {
    if ((AreaState() == toBuild) &&
	(AreaEdgeState() == TopAbs_IN)) {
      if (left.IsNull())
	current = 2;
      else
	current = 1;
      return;
    }
    NextArea();
  }
}

//=======================================================================
//function : MoreVertices
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_EdgeBuilder::MoreVertices() const
{
  return (current < 3);
}

//=======================================================================
//function : NextVertex
//purpose  : 
//=======================================================================

void  HLRBRep_EdgeBuilder::NextVertex()
{
  if (current == 1) {
    current = 2;
    if (right.IsNull())
      current = 3;
  }
  else if (current == 2) {
    NextArea();
    if ((AreaState() == toBuild) && (AreaEdgeState() == TopAbs_IN))
      current = 2;
    else
      current = 3;
  }
  else
    Standard_NoSuchObject::Raise("EdgeBuilder::NextVertex : No current edge");
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

const HLRAlgo_Intersection& HLRBRep_EdgeBuilder::Current() const
{
  if (current == 1)
    return left->Vertex();
  else if (current == 2)
    return right->Vertex();
  else
    Standard_NoSuchObject::Raise("EdgeBuilder::Current : No current vertex");
  return left->Vertex(); // only for WNT.
}

//=======================================================================
//function : IsBoundary
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_EdgeBuilder::IsBoundary() const
{
  if (current == 1)
    return left->IsBoundary();
  else if (current == 2)
    return right->IsBoundary();
  else
    Standard_NoSuchObject::Raise
      ("EdgeBuilder::IsBoundary : No current vertex");
  return left->IsBoundary(); // only for WNT.
}

//=======================================================================
//function : IsInterference
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_EdgeBuilder::IsInterference() const
{
  if (current == 1)
    return left->IsInterference();
  else if (current == 2)
    return right->IsInterference();
  else
    Standard_NoSuchObject::Raise
      ("EdgeBuilder::IsInterference : No current vertex");
  return left->IsInterference(); // only for WNT.
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  HLRBRep_EdgeBuilder::Orientation() const
{
  if (current == 1) {
    if ((left->StateBefore() == left->StateAfter()) &&
	(left->EdgeBefore()  == left->EdgeAfter()))
      return TopAbs_INTERNAL;
    else
      return TopAbs_FORWARD;
  }
  else if (current == 2) {
    if ((right->StateBefore() == right->StateAfter()) &&
	(right->EdgeBefore()  == right->EdgeAfter()))
      return TopAbs_INTERNAL;
    else
      return TopAbs_REVERSED;
  }
  return TopAbs_EXTERNAL; // only for WNT.
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void HLRBRep_EdgeBuilder::Destroy()
{
  Handle(HLRBRep_AreaLimit) cur = myLimits;
  while (!cur.IsNull()) {
    Handle(HLRBRep_AreaLimit) n = cur->Next();
    cur->Clear();
    cur = n;
  }
  left.Nullify();
  right.Nullify();
  myLimits.Nullify();
}
