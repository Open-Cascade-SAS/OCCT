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


#include <BRepMesh_Vertex.ixx>
#include <Precision.hxx>


BRepMesh_Vertex::BRepMesh_Vertex()
: myLocation(0), myMovability(BRepMesh_Free)
{}

BRepMesh_Vertex::BRepMesh_Vertex(const gp_XY& UV,
                                 const Standard_Integer Locat3d,
                                 const BRepMesh_DegreeOfFreedom Move)
                                 : myUV(UV), myLocation(Locat3d), myMovability(Move)
{}

BRepMesh_Vertex::BRepMesh_Vertex(const Standard_Real U,
                                 const Standard_Real V,
                                 const BRepMesh_DegreeOfFreedom Move)
                                 : myUV(U, V), myLocation(0), myMovability(Move)
{}

void  BRepMesh_Vertex::Initialize(const gp_XY& UV,
                                  const Standard_Integer Locat3d,
                                  const BRepMesh_DegreeOfFreedom Move)
{
  myUV=UV;
  myLocation=Locat3d;
  myMovability=Move;
}

void  BRepMesh_Vertex::SetMovability(const BRepMesh_DegreeOfFreedom Move)
{
  myMovability=Move;
}

//=======================================================================
//function : HashCode IsEqual 
//purpose  : Services for Map
//=======================================================================
Standard_Integer  BRepMesh_Vertex::HashCode(const Standard_Integer Upper)const
{
  return ::HashCode (Floor(1e5*myUV.X())*Floor(1e5*myUV.Y()), Upper);
}

Standard_Boolean  BRepMesh_Vertex::IsEqual(const BRepMesh_Vertex& Other)const
{
  if (myMovability!=BRepMesh_Deleted && Other.myMovability!=BRepMesh_Deleted)
    return (myUV.IsEqual(Other.myUV, Precision::PConfusion()));
  return Standard_False;
}

