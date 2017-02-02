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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_Intersection.hxx>
#include <HLRBRep_EdgeInterferenceTool.hxx>
#include <HLRBRep_VertexList.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

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
    throw Standard_NoSuchObject("HLRBRep_VertexList::Current");
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
    throw Standard_DomainError("HLRBRep_VertexList::Orientation");
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
    throw Standard_DomainError("HLRBRep_VertexList::Transition");
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
    throw Standard_DomainError("HLRBRep_VertexList::BoundaryTransition");
}
