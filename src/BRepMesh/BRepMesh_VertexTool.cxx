// Created on: 2011-06-02
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <BRepMesh_VertexTool.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_VertexInspector.hxx>

//=======================================================================
//function : Inspect
//purpose  : 
//=======================================================================
NCollection_CellFilter_Action BRepMesh_VertexInspector::Inspect(
  const Standard_Integer theTarget)
{
  const BRepMesh_Vertex& aVertex = myVertices->Value(theTarget - 1);
  if(aVertex.Movability() == BRepMesh_Deleted)
  {
    myDelNodes.Append(theTarget);
    return CellFilter_Purge;
  }
  
  gp_XY aVec = (myPoint - aVertex.Coord());
  Standard_Boolean inTol;
  if (Abs(myTolerance[1]) < Precision::Confusion())
  {
    inTol = aVec.SquareModulus() < myTolerance[0];
  }
  else
  {
    inTol = ((aVec.X() * aVec.X()) < myTolerance[0]) && 
            ((aVec.Y() * aVec.Y()) < myTolerance[1]);
  }
  if (inTol)
    myResIndices.Append(theTarget);

  return CellFilter_Keep;
}

//=======================================================================
//function : BRepMesh_VertexTool
//purpose  : 
//=======================================================================
BRepMesh_VertexTool::BRepMesh_VertexTool(
  const Handle(NCollection_IncAllocator)& theAllocator)
  : myAllocator (theAllocator),
    myCellFilter(0., myAllocator),
    mySelector  (myAllocator)
{
  const Standard_Real aTol = Precision::Confusion();
  SetCellSize ( aTol + 0.05 * aTol );
  SetTolerance( aTol, aTol );
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_VertexTool::Add(
  const BRepMesh_Vertex& theVertex,
  const Standard_Boolean isForceAdd)
{
  Standard_Integer aIndex = isForceAdd ? 0 : FindIndex(theVertex);
  if (aIndex == 0)
  {
    aIndex = mySelector.Add(theVertex);

    gp_XY aMinPnt, aMaxPnt;
    expandPoint(theVertex.Coord(), aMinPnt, aMaxPnt);
    myCellFilter.Add(aIndex, aMinPnt, aMaxPnt);
  }
  return aIndex;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::Delete(const Standard_Integer theIndex)
{
  BRepMesh_Vertex& aV = mySelector.GetVertex(theIndex);

  gp_XY aMinPnt, aMaxPnt;
  expandPoint(aV.Coord(), aMinPnt, aMaxPnt);

  myCellFilter.Remove(theIndex, aMinPnt, aMaxPnt);
  mySelector.Delete(theIndex);
}

//=======================================================================
//function : Substitute
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::Substitute(
  const Standard_Integer theIndex,
  const BRepMesh_Vertex& theVertex)
{
  BRepMesh_Vertex& aV = mySelector.GetVertex(theIndex);

  gp_XY aMinPnt, aMaxPnt;
  expandPoint(aV.Coord(), aMinPnt, aMaxPnt);

  myCellFilter.Remove(theIndex, aMinPnt, aMaxPnt);

  aV = theVertex;
  expandPoint(aV.Coord(), aMinPnt, aMaxPnt);
  myCellFilter.Add(theIndex, aMinPnt, aMaxPnt);
}

//=======================================================================
//function : Statistics
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::Statistics(Standard_OStream& theStream) const
{
  theStream << "\nStructure Statistics\n---------------\n\n";
  theStream << "This structure has " << mySelector.NbVertices() << " Nodes\n\n";
}
