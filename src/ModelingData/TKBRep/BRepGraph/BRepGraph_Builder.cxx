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
#include <BRepGraph_Layer.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_IncAllocator.hxx>

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
  Perform(theGraph, theShape, theParallel, BRepGraphInc_Populate::Options());
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&                            theGraph,
                                const TopoDS_Shape&                   theShape,
                                bool                                  theParallel,
                                const BRepGraphInc_Populate::Options& theOptions)
{
  BRepGraph_BackRefManager::ClearAll(theGraph);

  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myIsDone = false;

  // Notify registered layers that graph data is being cleared.
  for (NCollection_DataMap<TCollection_AsciiString, Handle(BRepGraph_Layer)>::Iterator
         anIter(theGraph.myLayers); anIter.More(); anIter.Next())
  {
    anIter.Value()->Clear();
  }

  if (theShape.IsNull())
    return;

  // Temporary allocator for populate scratch data, discarded after build.
  Handle(NCollection_IncAllocator) aTmpAlloc = new NCollection_IncAllocator;

  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage,
                                 theShape, theParallel, theOptions, aTmpAlloc);
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

  // Snapshot entity counts before append to allocate UIDs only for new entities.
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  const int anOldVtx   = aStorage.NbVertices();
  const int anOldEdge  = aStorage.NbEdges();
  const int anOldWire  = aStorage.NbWires();
  const int anOldFace  = aStorage.NbFaces();
  const int anOldShell = aStorage.NbShells();
  const int anOldSolid = aStorage.NbSolids();
  const int anOldComp  = aStorage.NbCompounds();
  const int anOldCS    = aStorage.NbCompSolids();

  Handle(NCollection_IncAllocator) aTmpAlloc = new NCollection_IncAllocator;
  BRepGraphInc_Populate::Append(aStorage, theShape, theParallel, aTmpAlloc);

  if (!aStorage.GetIsDone())
    return;

  theGraph.myData->myCurrentShapes.Clear();

  populateUIDsIncremental(theGraph, anOldVtx, anOldEdge, anOldWire, anOldFace,
                          anOldShell, anOldSolid, anOldComp, anOldCS);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::populateUIDsIncremental(BRepGraph& theGraph,
                                                 int theOldVtx,   int theOldEdge,
                                                 int theOldWire,  int theOldFace,
                                                 int theOldShell, int theOldSolid,
                                                 int theOldComp,  int theOldCS)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = theOldVtx; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(i).Id);
  for (int i = theOldEdge; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(i).Id);
  for (int i = theOldWire; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(i).Id);
  for (int i = theOldFace; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(i).Id);
  for (int i = theOldShell; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(i).Id);
  for (int i = theOldSolid; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(i).Id);
  for (int i = theOldComp; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(i).Id);
  for (int i = theOldCS; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(i).Id);
}
