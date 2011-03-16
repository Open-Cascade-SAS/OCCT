// File:	HLRBRep_VertexList.cxx
// Created:	Thu Apr 17 21:25:08 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>
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

