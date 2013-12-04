// Created on: 1993-06-18
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepMesh_ComparatorOfVertexOfDelaun.ixx>

//=======================================================================
//function : BRepMesh_ComparatorOfVertexOfDelaun
//purpose  : 
//=======================================================================

BRepMesh_ComparatorOfVertexOfDelaun::BRepMesh_ComparatorOfVertexOfDelaun(const gp_XY& theDir)
                                                                         : DirectionOfSort(theDir)
{}

//=======================================================================
//function : IsLower
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfVertexOfDelaun::IsLower(const BRepMesh_Vertex& Left,
                                                              const BRepMesh_Vertex& Right) const
{
  return (Left.Coord()*DirectionOfSort) <
    (Right.Coord()*DirectionOfSort);
}

//=======================================================================
//function : IsGreater
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfVertexOfDelaun::IsGreater(const BRepMesh_Vertex& Left,
                                                                const BRepMesh_Vertex& Right) const
{
  return (Left.Coord()*DirectionOfSort) >
    (Right.Coord()*DirectionOfSort);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_ComparatorOfVertexOfDelaun::IsEqual(const BRepMesh_Vertex& Left,
                                                              const BRepMesh_Vertex& Right) const
{
  return Left.IsEqual(Right);
}

