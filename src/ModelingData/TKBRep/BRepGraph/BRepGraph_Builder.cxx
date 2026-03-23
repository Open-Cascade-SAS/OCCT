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

  if (!aStorage.IsDone)
    return;

  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
    theGraph.allocateUID(aStorage.Vertices.Value(i).Id);
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
    theGraph.allocateUID(aStorage.Edges.Value(i).Id);
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
    theGraph.allocateUID(aStorage.Wires.Value(i).Id);
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
    theGraph.allocateUID(aStorage.Faces.Value(i).Id);
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
    theGraph.allocateUID(aStorage.Shells.Value(i).Id);
  for (int i = 0; i < aStorage.Solids.Length(); ++i)
    theGraph.allocateUID(aStorage.Solids.Value(i).Id);
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
    theGraph.allocateUID(aStorage.Compounds.Value(i).Id);
  for (int i = 0; i < aStorage.CompSolids.Length(); ++i)
    theGraph.allocateUID(aStorage.CompSolids.Value(i).Id);
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
  if (!theGraph.myData->myIncStorage.IsDone)
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

  // Clear UIDs for re-population (entity vectors may have grown).
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  aStorage.VertexUIDs.Clear();
  aStorage.EdgeUIDs.Clear();
  aStorage.WireUIDs.Clear();
  aStorage.FaceUIDs.Clear();
  aStorage.ShellUIDs.Clear();
  aStorage.SolidUIDs.Clear();
  aStorage.CompoundUIDs.Clear();
  aStorage.CompSolidUIDs.Clear();

  theGraph.myData->myCurrentShapes.Clear();

  populateUIDs(theGraph);

  theGraph.myData->myIsDone = true;
}
