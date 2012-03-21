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


#include <BRepMesh_Triangle.ixx>

BRepMesh_Triangle::BRepMesh_Triangle()
: Edge1(0), Edge2(0), Edge3(0), myMovability(BRepMesh_Free)
{}

BRepMesh_Triangle::BRepMesh_Triangle (const Standard_Integer e1, 
                                      const Standard_Integer e2,
                                      const Standard_Integer e3,
                                      const Standard_Boolean o1, 
                                      const Standard_Boolean o2,
                                      const Standard_Boolean o3,
                                      const BRepMesh_DegreeOfFreedom  canMove)
                                      : Edge1(e1),  Orientation1(o1),Edge2(e2), Orientation2(o2), 
                                      Edge3(e3), Orientation3(o3), 
                                      myMovability(canMove)
{}

void  BRepMesh_Triangle::Initialize(const Standard_Integer e1,
                                    const Standard_Integer e2,
                                    const Standard_Integer e3,
                                    const Standard_Boolean o1, 
                                    const Standard_Boolean o2,
                                    const Standard_Boolean o3,
                                    const BRepMesh_DegreeOfFreedom  canMove)
{
  Edge1        =e1;
  Edge2        =e2;
  Edge3        =e3;
  Orientation1 =o1;
  Orientation2 =o2;
  Orientation3 =o3;
  myMovability =canMove;
}

void  BRepMesh_Triangle::Edges(Standard_Integer& e1,
                               Standard_Integer& e2,
                               Standard_Integer& e3,
                               Standard_Boolean& o1, 
                               Standard_Boolean& o2,
                               Standard_Boolean& o3)const 
{
  e1=Edge1;
  e2=Edge2;
  e3=Edge3;
  o1=Orientation1;
  o2=Orientation2;
  o3=Orientation3;
}

void  BRepMesh_Triangle::SetMovability(const BRepMesh_DegreeOfFreedom  Move)
{
  myMovability =Move;
}

Standard_Integer BRepMesh_Triangle::HashCode
(const Standard_Integer Upper)const 
{
  return ::HashCode(Edge1+Edge2+Edge3, Upper);
}

Standard_Boolean BRepMesh_Triangle::IsEqual
(const BRepMesh_Triangle& Other)const 
{
  if (myMovability==BRepMesh_Deleted || Other.myMovability==BRepMesh_Deleted)
    return Standard_False;
  if (Edge1==Other.Edge1 && Edge2==Other.Edge2 && Edge3==Other.Edge3)
    return Standard_True;
  if (Edge1==Other.Edge2 && Edge2==Other.Edge3 && Edge3==Other.Edge1)
    return Standard_True;
  if (Edge1==Other.Edge3 && Edge2==Other.Edge1 && Edge3==Other.Edge2)
    return Standard_True;
  return Standard_False;
}
