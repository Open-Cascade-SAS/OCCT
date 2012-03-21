// Created on: 1994-09-01
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <Draft_VertexInfo.ixx>

#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : Draft_VertexInfo
//purpose  : 
//=======================================================================

Draft_VertexInfo::Draft_VertexInfo () {}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Draft_VertexInfo::Add(const TopoDS_Edge& E)
{
  for (myItEd.Initialize(myEdges); myItEd.More(); myItEd.Next()) {
    if (E.IsSame(myItEd.Value())) {
      break;
    }
  }
  if (!myItEd.More()) {
    myEdges.Append(E);
    myParams.Append(RealLast());
  }
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

const gp_Pnt& Draft_VertexInfo::Geometry () const
{
  return myGeom;
}


//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

gp_Pnt& Draft_VertexInfo::ChangeGeometry ()
{
  return myGeom;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Draft_VertexInfo::Parameter (const TopoDS_Edge& E)
{
  TColStd_ListIteratorOfListOfReal itp(myParams);
  myItEd.Initialize(myEdges);
  for (; myItEd.More(); myItEd.Next(),itp.Next()) {
    if (myItEd.Value().IsSame(E)) {
      return itp.Value();
    }
  }
  Standard_DomainError::Raise(); return 0;
}


//=======================================================================
//function : ChangeParameter
//purpose  : 
//=======================================================================

Standard_Real& Draft_VertexInfo::ChangeParameter (const TopoDS_Edge& E)
{
  TColStd_ListIteratorOfListOfReal itp(myParams);
  myItEd.Initialize(myEdges);
  for (; myItEd.More(); myItEd.Next(),itp.Next()) {
    if (myItEd.Value().IsSame(E)) {
      return itp.Value();
    }
  }
  Standard_DomainError::Raise(); return itp.Value();
}


//=======================================================================
//function : InitEdgeIterator
//purpose  : 
//=======================================================================

void Draft_VertexInfo::InitEdgeIterator () 
{
  myItEd.Initialize(myEdges);
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge& Draft_VertexInfo::Edge () const
{
  return TopoDS::Edge(myItEd.Value());
}


//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================

Standard_Boolean Draft_VertexInfo::MoreEdge() const
{
  return myItEd.More();
}


//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void Draft_VertexInfo::NextEdge()
{
  myItEd.Next();
}


