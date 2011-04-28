// File:        BRepMesh_ComparatorOfIndexedVertexOfDelaun.cxx
// Created:     Tue Apr  5 11:50:40 1994
// Author:      Didier PIFFAULT
//              <dpf@zerox>

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

