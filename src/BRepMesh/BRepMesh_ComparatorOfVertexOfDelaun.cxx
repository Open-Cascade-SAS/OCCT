// File:        BRepMesh_ComparatorOfVertexOfDelaun.cxx
// Created:     Fri Jun 18 17:04:44 1993
// Author:      Didier PIFFAULT
//              <dpf@zerox>

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

