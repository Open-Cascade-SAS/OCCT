// Created on: 1993-09-23
// Created by: Didier PIFFAULT
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


#include <BRepMesh_Edge.ixx>

BRepMesh_Edge::BRepMesh_Edge(const Standard_Integer vDebut,
                             const Standard_Integer vFin,
                             const BRepMesh_DegreeOfFreedom canMove)
                             : myFirstNode(vDebut), myLastNode(vFin), myMovability(canMove)
{}

void  BRepMesh_Edge::SetMovability(const BRepMesh_DegreeOfFreedom Move)
{
  myMovability =Move;
}

Standard_Integer  BRepMesh_Edge::HashCode(const Standard_Integer Upper)const 
{
  return ::HashCode(myFirstNode+myLastNode, Upper);
}

Standard_Boolean  BRepMesh_Edge::IsEqual(const BRepMesh_Edge& Other)const 
{
  if (myMovability==BRepMesh_Deleted || Other.myMovability==BRepMesh_Deleted)
    return Standard_False;
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode) ||
    (myFirstNode==Other.myLastNode && myLastNode==Other.myFirstNode);
}


Standard_Boolean  BRepMesh_Edge::SameOrientation
(const BRepMesh_Edge& Other)const 
{
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode);
}
