// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>

//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = 0; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(i).Id);
  for (int i = 0; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(i).Id);
  for (int i = 0; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(i).Id);
  for (int i = 0; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(i).Id);
  for (int i = 0; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(i).Id);
  for (int i = 0; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(i).Id);
  for (int i = 0; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(i).Id);
  for (int i = 0; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(i).Id);
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  BRepGraph_BackRefManager::ClearAll(theGraph);

  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myIsDone = false;

  if (theShape.IsNull())
    return;

  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage, theShape, theParallel);
  if (!theGraph.myData->myIncStorage.GetIsDone())
  {
    theGraph.myData->myIncStorage.Clear();
    return;
  }

  populateUIDs(theGraph);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  if (theShape.IsNull())
    return;

  BRepGraphInc_Populate::Append(theGraph.myData->myIncStorage, theShape, theParallel);

  // Clear UIDs for re-population: Append may add new entities, making
  // existing UID vectors shorter than their parallel entity vectors.
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  aStorage.ResetAllUIDs();

  theGraph.myData->myCurrentShapes.Clear();

  populateUIDs(theGraph);

  theGraph.myData->myIsDone = true;
}
