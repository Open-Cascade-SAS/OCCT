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
