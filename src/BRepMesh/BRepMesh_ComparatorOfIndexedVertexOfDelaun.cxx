// Created on: 1994-04-05
// Created by: Didier PIFFAULT
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_ComparatorOfIndexedVertexOfDelaun.ixx>
#include <BRepMesh_Vertex.hxx>

//=======================================================================
//function : BRepMesh_ComparatorOfIndexedVertexOfDelaun
//purpose  : 
//=======================================================================

BRepMesh_ComparatorOfIndexedVertexOfDelaun::BRepMesh_ComparatorOfIndexedVertexOfDelaun
(const gp_XY& theDir,
 const Standard_Real theTol,
 const Handle(BRepMesh_DataStructureOfDelaun)& HDS)
 :  IndexedStructure(HDS),DirectionOfSort(theDir), Tolerance(theTol)
{}

//=======================================================================
//function : IsLower
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfIndexedVertexOfDelaun::IsLower
(const Standard_Integer Left, const Standard_Integer Right) const
{
  return ((IndexedStructure->GetNode(Left).Coord()*DirectionOfSort) <
    (IndexedStructure->GetNode(Right).Coord()*DirectionOfSort));
}

//=======================================================================
//function : IsGreater
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfIndexedVertexOfDelaun::IsGreater
(const Standard_Integer Left, const Standard_Integer Right) const
{
  return ((IndexedStructure->GetNode(Left).Coord()*DirectionOfSort) >
    (IndexedStructure->GetNode(Right).Coord()*DirectionOfSort));
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfIndexedVertexOfDelaun::IsEqual
(const Standard_Integer Left, const Standard_Integer Right) const
{
  return (IndexedStructure->GetNode(Left).Coord().IsEqual
    (IndexedStructure->GetNode(Right).Coord(), Tolerance));
}

