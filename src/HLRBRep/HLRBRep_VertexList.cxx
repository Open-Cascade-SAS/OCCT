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
#include <HLRBRep_VertexList.ixx>

#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : HLRBRep_VertexList
//purpose  : 
//=======================================================================

HLRBRep_VertexList::
HLRBRep_VertexList(const HLRBRep_EdgeInterferenceTool& T, 
		   const HLRAlgo_ListIteratorOfInterferenceList& I) :
       myIterator(I),
       myTool(T),
       fromEdge(Standard_False),
       fromInterf(Standard_False)
{
  myTool.InitVertices();
  Next();
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_VertexList::IsPeriodic()const 
{
  return myTool.IsPeriodic();
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_VertexList::More()const 
{
  return (fromEdge || fromInterf);
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  HLRBRep_VertexList::Next()
{
  if (fromInterf)
    myIterator.Next();
  if (fromEdge)
    myTool.NextVertex();
  fromInterf = myIterator.More();
  fromEdge   = myTool.MoreVertices();
  if (fromEdge && fromInterf) {
    if (!myTool.SameVertexAndInterference( myIterator.Value())) {
      if (myTool.CurrentParameter() <
	  myTool.ParameterOfInterference(myIterator.Value())) {
	fromInterf = Standard_False;
      }
      else {
	fromEdge = Standard_False;
      }
    }
  }
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

const HLRAlgo_Intersection &  HLRBRep_VertexList::Current() const 
{
  if (fromEdge)
    return myTool.CurrentVertex();
  else if (fromInterf)
    return myIterator.Value().Intersection();
  else
    Standard_NoSuchObject::Raise("HLRBRep_VertexList::Current");
  return myTool.CurrentVertex(); // only for WNT.
}

//=======================================================================
//function : IsBoundary
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_VertexList::IsBoundary() const 
{
  return fromEdge;
}

//=======================================================================
//function : IsInterference
//purpose  : 
//=======================================================================

Standard_Boolean  HLRBRep_VertexList::IsInterference() const 
{
  return fromInterf;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  HLRBRep_VertexList::Orientation() const 
{
  if (fromEdge)
    return myTool.CurrentOrientation();
  else
    Standard_DomainError::Raise("HLRBRep_VertexList::Orientation");
  return TopAbs_EXTERNAL; // only for WNT.
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

TopAbs_Orientation  HLRBRep_VertexList::Transition() const 
{
  if (fromInterf)
    return myIterator.Value().Transition();
  else
    Standard_DomainError::Raise("HLRBRep_VertexList::Transition");
  return TopAbs_EXTERNAL; // only for WNT.
}

//=======================================================================
//function : BoundaryTransition
//purpose  : 
//=======================================================================

TopAbs_Orientation  HLRBRep_VertexList::BoundaryTransition() const 
{
  if (fromInterf)
    return myIterator.Value().BoundaryTransition();
  else
    Standard_DomainError::Raise("HLRBRep_VertexList::BoundaryTransition");
  return TopAbs_EXTERNAL; // only for WNT.
}
