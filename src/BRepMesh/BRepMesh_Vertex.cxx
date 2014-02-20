// Created on: 1993-09-23
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

