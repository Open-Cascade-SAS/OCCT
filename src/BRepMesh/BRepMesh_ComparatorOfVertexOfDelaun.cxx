// Created on: 1993-06-18
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


#include <BRepMesh_ComparatorOfVertexOfDelaun.ixx>

//=======================================================================
//function : BRepMesh_ComparatorOfVertexOfDelaun
//purpose  : 
//=======================================================================

BRepMesh_ComparatorOfVertexOfDelaun::BRepMesh_ComparatorOfVertexOfDelaun(const gp_XY& theDir,
                                                                         const Standard_Real theTol)
                                                                         : DirectionOfSort(theDir), Tolerance(theTol)
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

