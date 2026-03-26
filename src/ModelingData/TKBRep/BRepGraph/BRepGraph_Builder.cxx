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

#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_IncAllocator.hxx>
#include <MathUtils_Random.hxx>
#include <Standard_GUID.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <cstring>
#include <random>

namespace
{

//! Generate a random Standard_GUID using MathUtils::RandomGenerator
//! seeded with std::random_device for platform entropy.
static Standard_GUID generateRandomGUID()
{
  std::random_device         aRD;
  MathUtils::RandomGenerator aRNG(aRD());
  // Fill 16 bytes with random data, then construct Standard_UUID field by field.
  const uint64_t aRand1 = aRNG.NextInt();
  const uint64_t aRand2 = aRNG.NextInt();
  Standard_UUID  aUUID;
  aUUID.Data1 = static_cast<uint32_t>(aRand1);
  aUUID.Data2 = static_cast<uint16_t>(aRand1 >> 32);
  aUUID.Data3 = static_cast<uint16_t>(aRand1 >> 48);
  for (int i = 0; i < 8; ++i)
    aUUID.Data4[i] = static_cast<uint8_t>(aRand2 >> (i * 8));
  return Standard_GUID(aUUID);
}

} // namespace

//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = 0; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(BRepGraph_VertexId(i)).Id);
  for (int i = 0; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(BRepGraph_EdgeId(i)).Id);
  for (int i = 0; i < aStorage.NbCoEdges(); ++i)
    theGraph.allocateUID(aStorage.CoEdge(BRepGraph_CoEdgeId(i)).Id);
  for (int i = 0; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(BRepGraph_WireId(i)).Id);
  for (int i = 0; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(BRepGraph_FaceId(i)).Id);
  for (int i = 0; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(BRepGraph_ShellId(i)).Id);
  for (int i = 0; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(BRepGraph_SolidId(i)).Id);
  for (int i = 0; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(BRepGraph_CompoundId(i)).Id);
  for (int i = 0; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(BRepGraph_CompSolidId(i)).Id);
  for (int i = 0; i < aStorage.NbProducts(); ++i)
    theGraph.allocateUID(aStorage.Product(BRepGraph_ProductId(i)).Id);
  for (int i = 0; i < aStorage.NbOccurrences(); ++i)
    theGraph.allocateUID(aStorage.Occurrence(BRepGraph_OccurrenceId(i)).Id);
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&          theGraph,
                                const TopoDS_Shape& theShape,
                                const bool          theParallel)
{
  Perform(theGraph, theShape, theParallel, BRepGraphInc_Populate::Options());
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&                            theGraph,
                                const TopoDS_Shape&                   theShape,
                                const bool                            theParallel,
                                const BRepGraphInc_Populate::Options& theOptions)
{
  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myGraphGUID = generateRandomGUID();
  theGraph.myData->myIsDone = false;

  // Notify registered layers that graph data is being cleared.
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         theGraph.myLayers);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Clear();
  }

  if (theShape.IsNull())
    return;

  // Temporary allocator for populate scratch data, discarded after build.
  occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator;

  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage,
                                 theShape,
                                 theParallel,
                                 theOptions,
                                 aTmpAlloc);
  if (!theGraph.myData->myIncStorage.GetIsDone())
  {
    theGraph.myData->myIncStorage.Clear();
    return;
  }

  populateUIDs(theGraph);

  // Auto-create a single root Product pointing to the top-level topology node.
  // aTopologyRoot defaults to invalid (Index = -1); set only if topology exists.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
    BRepGraph_NodeId      aTopologyRoot; // default: invalid (Index = -1)
    switch (theShape.ShapeType())
    {
      case TopAbs_COMPOUND:
        if (aStorage.NbCompounds() > 0)
          aTopologyRoot = BRepGraph_NodeId::Compound(0);
        break;
      case TopAbs_COMPSOLID:
        if (aStorage.NbCompSolids() > 0)
          aTopologyRoot = BRepGraph_NodeId::CompSolid(0);
        break;
      case TopAbs_SOLID:
        if (aStorage.NbSolids() > 0)
          aTopologyRoot = BRepGraph_NodeId::Solid(0);
        break;
      case TopAbs_SHELL:
        if (aStorage.NbShells() > 0)
          aTopologyRoot = BRepGraph_NodeId::Shell(0);
        break;
      case TopAbs_FACE:
        if (aStorage.NbFaces() > 0)
          aTopologyRoot = BRepGraph_NodeId::Face(0);
        break;
      case TopAbs_WIRE:
        if (aStorage.NbWires() > 0)
          aTopologyRoot = BRepGraph_NodeId::Wire(0);
        break;
      case TopAbs_EDGE:
        if (aStorage.NbEdges() > 0)
          aTopologyRoot = BRepGraph_NodeId::Edge(0);
        break;
      case TopAbs_VERTEX:
        if (aStorage.NbVertices() > 0)
          aTopologyRoot = BRepGraph_NodeId::Vertex(0);
        break;
      default:
        break;
    }

    BRepGraphInc::ProductEntity& aProduct    = aStorage.AppendProduct();
    const int                    aProductIdx = aStorage.NbProducts() - 1;
    aProduct.Id                              = BRepGraph_NodeId::Product(aProductIdx);
    aProduct.ShapeRootId                     = aTopologyRoot; // invalid if no topology matched
    aProduct.RootOrientation                 = theShape.Orientation();
    aProduct.RootLocation                    = theShape.Location();
    theGraph.allocateUID(aProduct.Id);
  }

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph&          theGraph,
                               const TopoDS_Shape& theShape,
                               const bool          theParallel)
{
  if (theShape.IsNull())
    return;

  // Snapshot entity counts before append to allocate UIDs only for new entities.
  BRepGraphInc_Storage& aStorage        = theGraph.myData->myIncStorage;
  const int             anOldVtx        = aStorage.NbVertices();
  const int             anOldEdge       = aStorage.NbEdges();
  const int             anOldCoEdge     = aStorage.NbCoEdges();
  const int             anOldWire       = aStorage.NbWires();
  const int             anOldFace       = aStorage.NbFaces();
  const int             anOldShell      = aStorage.NbShells();
  const int             anOldSolid      = aStorage.NbSolids();
  const int             anOldComp       = aStorage.NbCompounds();
  const int             anOldCS         = aStorage.NbCompSolids();
  const int             anOldProduct    = aStorage.NbProducts();
  const int             anOldOccurrence = aStorage.NbOccurrences();

  occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator;
  BRepGraphInc_Populate::Append(aStorage, theShape, theParallel, aTmpAlloc);

  if (!aStorage.GetIsDone())
    return;

  theGraph.myData->myCurrentShapes.Clear();

  populateUIDsIncremental(theGraph,
                          anOldVtx,
                          anOldEdge,
                          anOldCoEdge,
                          anOldWire,
                          anOldFace,
                          anOldShell,
                          anOldSolid,
                          anOldComp,
                          anOldCS,
                          anOldProduct,
                          anOldOccurrence);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::populateUIDsIncremental(BRepGraph& theGraph,
                                                const int  theOldVtx,
                                                const int  theOldEdge,
                                                const int  theOldCoEdge,
                                                const int  theOldWire,
                                                const int  theOldFace,
                                                const int  theOldShell,
                                                const int  theOldSolid,
                                                const int  theOldComp,
                                                const int  theOldCS,
                                                const int  theOldProduct,
                                                const int  theOldOccurrence)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = theOldVtx; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(BRepGraph_VertexId(i)).Id);
  for (int i = theOldEdge; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(BRepGraph_EdgeId(i)).Id);
  for (int i = theOldCoEdge; i < aStorage.NbCoEdges(); ++i)
    theGraph.allocateUID(aStorage.CoEdge(BRepGraph_CoEdgeId(i)).Id);
  for (int i = theOldWire; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(BRepGraph_WireId(i)).Id);
  for (int i = theOldFace; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(BRepGraph_FaceId(i)).Id);
  for (int i = theOldShell; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(BRepGraph_ShellId(i)).Id);
  for (int i = theOldSolid; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(BRepGraph_SolidId(i)).Id);
  for (int i = theOldComp; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(BRepGraph_CompoundId(i)).Id);
  for (int i = theOldCS; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(BRepGraph_CompSolidId(i)).Id);
  for (int i = theOldProduct; i < aStorage.NbProducts(); ++i)
    theGraph.allocateUID(aStorage.Product(BRepGraph_ProductId(i)).Id);
  for (int i = theOldOccurrence; i < aStorage.NbOccurrences(); ++i)
    theGraph.allocateUID(aStorage.Occurrence(BRepGraph_OccurrenceId(i)).Id);
}
