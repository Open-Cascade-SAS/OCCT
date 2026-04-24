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

#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Assert.hxx>
#include <algorithm>

namespace
{

template <typename T>
BRepGraph_VertexId resolveVertexDefId(
  const NCollection_DynamicArray<BRepGraphInc::VertexRef>& theVertexRefs,
  const T                                                  theRefId)
{
  if (!theRefId.IsValid(static_cast<uint32_t>(theVertexRefs.Size())))
    return BRepGraph_VertexId();
  return theVertexRefs.Value(static_cast<size_t>(theRefId.Index)).VertexDefId;
}

template <typename TKey, typename TVal>
bool containsIndexInTable(const NCollection_DynamicArray<NCollection_DynamicArray<TVal>>& theIdx,
                          const TKey                                                      theKey,
                          const TVal                                                      theVal)
{
  if (!theKey.IsValid(static_cast<uint32_t>(theIdx.Size())))
    return false;
  const NCollection_DynamicArray<TVal>& aVec = theIdx.Value(static_cast<size_t>(theKey.Index));
  for (const TVal& anElem : aVec)
    if (anElem == theVal)
      return true;
  return false;
}

static bool hasActiveFaceForEdgeInCoEdges(
  const NCollection_DynamicArray<BRepGraph_CoEdgeId>&      theCoEdgeIds,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>& theCoEdges,
  const BRepGraph_EdgeId                                   theEdgeId,
  const BRepGraph_FaceId                                   theFaceId)
{
  for (const BRepGraph_CoEdgeId& aCoEdgeId : theCoEdgeIds)
  {
    if (!aCoEdgeId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
    if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid() || !aCoEdge.FaceDefId.IsValid())
    {
      continue;
    }
    if (aCoEdge.EdgeDefId == theEdgeId && aCoEdge.FaceDefId == theFaceId)
    {
      return true;
    }
  }
  return false;
}

} // namespace

//=================================================================================================

void BRepGraphInc_ReverseIndex::Clear()
{
  myEdgeToWires.Clear();
  myEdgeToFaces.Clear();
  myEdgeToCoEdges.Clear();
  myVertexToEdges.Clear();
  myWireToFaces.Clear();
  myFaceToShells.Clear();
  myShellToSolids.Clear();
  myCompoundsOfSolid.Clear();
  myCompSolidsOfSolid.Clear();
  myCompoundsOfShell.Clear();
  myCompoundsOfFace.Clear();
  myCompoundsOfCompound.Clear();
  myCompoundsOfCompSolid.Clear();
  myCompoundsOfWire.Clear();
  myCompoundsOfEdge.Clear();
  myCompoundsOfVertex.Clear();
  myCoEdgeToWires.Clear();
  myProductToOccurrences.Clear();
  myNbIndexedCoEdges = 0;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(const BRepGraphInc_Storage& theStorage)
{
  Build(theStorage.myVertices.Entities,
        theStorage.myEdges.Entities,
        theStorage.myCoEdges.Entities,
        theStorage.myWires.Entities,
        theStorage.myFaces.Entities,
        theStorage.myShells.Entities,
        theStorage.mySolids.Entities,
        theStorage.myCompounds.Entities,
        theStorage.myCompSolids.Entities,
        theStorage.myShellRefs.Refs,
        theStorage.myFaceRefs.Refs,
        theStorage.myWireRefs.Refs,
        theStorage.myCoEdgeRefs.Refs,
        theStorage.mySolidRefs.Refs,
        theStorage.myChildRefs.Refs,
        theStorage.myVertexRefs.Refs);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(
  const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
  const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
  const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
  const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
  const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
  const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
  const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
  const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
  const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
  const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
  const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
  const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
  const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
  const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs)
{
  myNbIndexedCoEdges = 0;

  // Reconstruct outer index tables with allocator if set.
  if (!myAllocator.IsNull())
  {
    myEdgeToWires   = TypedIndexTable<BRepGraph_WireId>(256, myAllocator);
    myEdgeToFaces   = TypedIndexTable<BRepGraph_FaceId>(256, myAllocator);
    myVertexToEdges = TypedIndexTable<BRepGraph_EdgeId>(256, myAllocator);
    myWireToFaces   = TypedIndexTable<BRepGraph_FaceId>(256, myAllocator);
    myFaceToShells  = TypedIndexTable<BRepGraph_ShellId>(256, myAllocator);
    myShellToSolids = TypedIndexTable<BRepGraph_SolidId>(256, myAllocator);
  }
  else
  {
    Clear();
  }

  // Helper: resolve a VertexRefId to the corresponding VertexDefId (BRepGraph_VertexId).
  // Returns an invalid id if the ref id is invalid or out of range.

  // Scan edges for max vertex index to pre-size myVertexToEdges.
  uint32_t       aNewVertexCapacity = 0;
  const uint32_t aNbEdges           = static_cast<uint32_t>(theEdges.Size());
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(anEdgeId.Index));
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid() && aStartVtx.Index + 1 > aNewVertexCapacity)
      aNewVertexCapacity = aStartVtx.Index + 1;
    if (anEndVtx.IsValid() && anEndVtx.Index + 1 > aNewVertexCapacity)
      aNewVertexCapacity = anEndVtx.Index + 1;
  }

  // Pre-size all outer vectors to their known key range.
  // Pass allocator so inner vectors use IncAllocator for O(1) alloc/free.
  preSize(myVertexToEdges, aNewVertexCapacity, myAllocator);
  preSize(myEdgeToWires, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  preSize(myEdgeToFaces, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  preSize(myWireToFaces, static_cast<uint32_t>(theWires.Size()), myAllocator);
  preSize(myFaceToShells, static_cast<uint32_t>(theFaces.Size()), myAllocator);
  preSize(myShellToSolids, static_cast<uint32_t>(theShells.Size()), myAllocator);

  // Vertex -> Edges: scan edge entities for start/end vertex indices.
  // Closed edges have StartVertexRefId and EndVertexRefId resolving to the same VertexDefId,
  // so skip duplicate.
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(anEdgeId.Index));
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid())
      appendDirect(myVertexToEdges, aStartVtx.Index, anEdgeId);
    if (anEndVtx.IsValid() && anEndVtx != aStartVtx)
      appendDirect(myVertexToEdges, anEndVtx.Index, anEdgeId);
  }

  // Edge -> Wires: iterate wire entities and their CoEdgeRefIds for O(1) parent lookup.
  const uint32_t aNbWires = static_cast<uint32_t>(theWires.Size());
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aWireId.Index));
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      if (!aCoEdgeRefId.IsValid(static_cast<uint32_t>(theCoEdgeRefs.Size())))
        continue;
      const BRepGraphInc::CoEdgeRef& aRef =
        theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
        continue;
      appendDirect(myEdgeToWires, aCoEdge.EdgeDefId.Index, aWireId);
    }
  }

  // Edge -> CoEdges: derive from CoEdge.EdgeDefId field.
  preSize(myEdgeToCoEdges, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  const uint32_t aNbCoEdges = static_cast<uint32_t>(theCoEdges.Size());
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendDirect(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, aCoEdgeId);
  }
  myNbIndexedCoEdges = static_cast<uint32_t>(theCoEdges.Size());

  // Edge -> Faces: derive from CoEdge.FaceDefId (replaces legacy PCurve-based derivation).
  // Seam edges have two CoEdges with same FaceDefId but opposite Orientation.
  // Deduplicate per edge using the edge->coedges index built above.
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    if (theEdges.Value(static_cast<size_t>(anEdgeId.Index)).IsRemoved)
      continue;
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      seekVec(myEdgeToCoEdges, anEdgeId.Index);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const size_t aNbCE = aCoEdgeIdxs->Size();

    // Collect face indices from coedges (stack-allocated for small counts).
    NCollection_LocalArray<uint32_t, 8> aFaces(aNbCE);
    size_t                              aNbFaces = 0;
    for (const auto& aCoEdgeId : *aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
      if (aCoEdge.FaceDefId.IsValid())
        aFaces[aNbFaces++] = aCoEdge.FaceDefId.Index;
    }
    if (aNbFaces == 0)
      continue;

    std::sort(static_cast<uint32_t*>(aFaces), static_cast<uint32_t*>(aFaces) + aNbFaces);

    // Append unique sorted values.
    uint32_t aPrev = UINT32_MAX;
    for (size_t i = 0; i < aNbFaces; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendDirect(myEdgeToFaces, anEdgeId.Index, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Wire -> Faces: iterate face entities and their WireRefIds for O(1) parent lookup.
  const uint32_t aNbFaces = static_cast<uint32_t>(theFaces.Size());
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(static_cast<size_t>(aFaceId.Index));
    if (aFace.IsRemoved)
      continue;
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      if (!aWireRefId.IsValid(static_cast<uint32_t>(theWireRefs.Size())))
        continue;
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(static_cast<size_t>(aWireRefId.Index));
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid(static_cast<uint32_t>(theWires.Size())))
        continue;
      if (theWires.Value(static_cast<size_t>(aRef.WireDefId.Index)).IsRemoved)
        continue;
      appendDirect(myWireToFaces, aRef.WireDefId.Index, aFaceId);
    }
  }

  // Face -> Shells: iterate shell entities and their FaceRefIds for O(1) parent lookup.
  const uint32_t aNbShells = static_cast<uint32_t>(theShells.Size());
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
  {
    const BRepGraphInc::ShellDef& aShell = theShells.Value(static_cast<size_t>(aShellId.Index));
    if (aShell.IsRemoved)
      continue;
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      if (!aFaceRefId.IsValid(static_cast<uint32_t>(theFaceRefs.Size())))
        continue;
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(static_cast<size_t>(aFaceRefId.Index));
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(static_cast<uint32_t>(theFaces.Size())))
        continue;
      if (theFaces.Value(static_cast<size_t>(aRef.FaceDefId.Index)).IsRemoved)
        continue;
      appendDirect(myFaceToShells, aRef.FaceDefId.Index, aShellId);
    }
  }

  // Shell -> Solids: iterate solid entities and their ShellRefIds for O(1) parent lookup.
  const uint32_t aNbSolids = static_cast<uint32_t>(theSolids.Size());
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbSolids); ++aSolidId)
  {
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(static_cast<size_t>(aSolidId.Index));
    if (aSolid.IsRemoved)
      continue;
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      if (!aShellRefId.IsValid(static_cast<uint32_t>(theShellRefs.Size())))
        continue;
      const BRepGraphInc::ShellRef& aRef =
        theShellRefs.Value(static_cast<size_t>(aShellRefId.Index));
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid(static_cast<uint32_t>(theShells.Size())))
        continue;
      if (theShells.Value(static_cast<size_t>(aRef.ShellDefId.Index)).IsRemoved)
        continue;
      appendDirect(myShellToSolids, aRef.ShellDefId.Index, aSolidId);
    }
  }

  // Compound -> child reverse indices: iterate compound entities and their ChildRefIds.
  // Covers all legal TopoDS_Compound child kinds (Solid/Shell/Face/Compound/CompSolid
  // + atomic Wire/Edge/Vertex); missing any kind causes silent unindexing of legal
  // compounds and later reverse-lookup returns empty.
  preSize(myCompoundsOfSolid, static_cast<uint32_t>(theSolids.Size()), myAllocator);
  preSize(myCompoundsOfShell, static_cast<uint32_t>(theShells.Size()), myAllocator);
  preSize(myCompoundsOfFace, static_cast<uint32_t>(theFaces.Size()), myAllocator);
  preSize(myCompoundsOfCompound, static_cast<uint32_t>(theCompounds.Size()), myAllocator);
  preSize(myCompoundsOfCompSolid, static_cast<uint32_t>(theCompSolids.Size()), myAllocator);
  preSize(myCompoundsOfWire, static_cast<uint32_t>(theWires.Size()), myAllocator);
  preSize(myCompoundsOfEdge, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  preSize(myCompoundsOfVertex, static_cast<uint32_t>(theVertices.Size()), myAllocator);
  const uint32_t aNbCompounds = static_cast<uint32_t>(theCompounds.Size());
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
  {
    const BRepGraphInc::CompoundDef& aComp =
      theCompounds.Value(static_cast<size_t>(aCompoundId.Index));
    if (aComp.IsRemoved)
      continue;
    for (const BRepGraph_ChildRefId& aChildRefId : aComp.ChildRefIds)
    {
      const uint32_t aChildRefIdx = aChildRefId.Index;
      if (aChildRefIdx >= theChildRefs.Size())
        continue;
      const BRepGraphInc::ChildRef& aRef = theChildRefs.Value(static_cast<size_t>(aChildRefIdx));
      if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
        continue;
      if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid
          && aRef.ChildDefId.Index < theSolids.Size())
        appendDirect(myCompoundsOfSolid, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Shell
               && aRef.ChildDefId.Index < theShells.Size())
        appendDirect(myCompoundsOfShell, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Face
               && aRef.ChildDefId.Index < theFaces.Size())
        appendDirect(myCompoundsOfFace, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Compound
               && aRef.ChildDefId.Index < theCompounds.Size())
        appendDirect(myCompoundsOfCompound, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::CompSolid
               && aRef.ChildDefId.Index < theCompSolids.Size())
        appendDirect(myCompoundsOfCompSolid, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Wire
               && aRef.ChildDefId.Index < theWires.Size())
        appendDirect(myCompoundsOfWire, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Edge
               && aRef.ChildDefId.Index < theEdges.Size())
        appendDirect(myCompoundsOfEdge, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Vertex
               && aRef.ChildDefId.Index < theVertices.Size())
        appendDirect(myCompoundsOfVertex, aRef.ChildDefId.Index, aCompoundId);
    }
  }

  // CompSolid -> Solid reverse index: iterate comp-solid entities and their SolidRefIds.
  preSize(myCompSolidsOfSolid, static_cast<uint32_t>(theSolids.Size()), myAllocator);
  const uint32_t aNbCompSolids = static_cast<uint32_t>(theCompSolids.Size());
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompSolids); ++aCompSolidId)
  {
    const BRepGraphInc::CompSolidDef& aCS =
      theCompSolids.Value(static_cast<size_t>(aCompSolidId.Index));
    if (aCS.IsRemoved)
      continue;
    for (const BRepGraph_SolidRefId& aSolidRefId : aCS.SolidRefIds)
    {
      if (!aSolidRefId.IsValid(static_cast<uint32_t>(theSolidRefs.Size())))
        continue;
      const BRepGraphInc::SolidRef& aRef =
        theSolidRefs.Value(static_cast<size_t>(aSolidRefId.Index));
      if (aRef.IsRemoved || !aRef.SolidDefId.IsValid(static_cast<uint32_t>(theSolids.Size())))
        continue;
      if (theSolids.Value(static_cast<size_t>(aRef.SolidDefId.Index)).IsRemoved)
        continue;
      appendDirect(myCompSolidsOfSolid, aRef.SolidDefId.Index, aCompSolidId);
    }
  }

  // CoEdge -> Wires: iterate wire entities and their CoEdgeRefIds for O(1) parent lookup.
  preSize(myCoEdgeToWires, static_cast<uint32_t>(theCoEdges.Size()), myAllocator);
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aWireId.Index));
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      if (!aCoEdgeRefId.IsValid(static_cast<uint32_t>(theCoEdgeRefs.Size())))
        continue;
      const BRepGraphInc::CoEdgeRef& aRef =
        theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
        continue;
      if (theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index)).IsRemoved)
        continue;
      appendDirect(myCoEdgeToWires, aRef.CoEdgeDefId.Index, aWireId);
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildDelta(
  const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
  const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
  const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
  const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
  const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
  const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
  const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
  const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
  const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
  const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
  const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
  const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
  const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
  const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs,
  const uint32_t                                              theOldNbEdges,
  const uint32_t                                              theOldNbWires,
  const uint32_t                                              theOldNbFaces,
  const uint32_t                                              theOldNbShells,
  const uint32_t                                              theOldNbSolids,
  const uint32_t                                              theOldNbCompounds,
  const uint32_t                                              theOldNbCompSolids,
  const uint32_t                                              theOldNbChildRefs,
  const uint32_t                                              theOldNbSolidRefs)
{

  // Helper: resolve a VertexRefId to the corresponding VertexDefId (BRepGraph_VertexId).
  // Returns an invalid id if the ref id is invalid or out of range.

  // Scan new edges for max vertex index to possibly extend myVertexToEdges.
  uint32_t       aNewVertexCapacity = static_cast<uint32_t>(myVertexToEdges.Size());
  const uint32_t aNbEdges           = static_cast<uint32_t>(theEdges.Size());
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(anEdgeId.Index));
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid() && aStartVtx.Index + 1 > aNewVertexCapacity)
      aNewVertexCapacity = aStartVtx.Index + 1;
    if (anEndVtx.IsValid() && anEndVtx.Index + 1 > aNewVertexCapacity)
      aNewVertexCapacity = anEndVtx.Index + 1;
  }

  // Extend outer vectors if needed (pre-size for new key ranges).
  ensureSize(myVertexToEdges, aNewVertexCapacity, myAllocator);
  ensureSize(myEdgeToWires, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  ensureSize(myEdgeToFaces, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  ensureSize(myEdgeToCoEdges, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  ensureSize(myWireToFaces, static_cast<uint32_t>(theWires.Size()), myAllocator);
  ensureSize(myFaceToShells, static_cast<uint32_t>(theFaces.Size()), myAllocator);
  ensureSize(myShellToSolids, static_cast<uint32_t>(theShells.Size()), myAllocator);
  ensureSize(myCompoundsOfSolid, static_cast<uint32_t>(theSolids.Size()), myAllocator);
  ensureSize(myCompSolidsOfSolid, static_cast<uint32_t>(theSolids.Size()), myAllocator);
  ensureSize(myCompoundsOfShell, static_cast<uint32_t>(theShells.Size()), myAllocator);
  ensureSize(myCompoundsOfFace, static_cast<uint32_t>(theFaces.Size()), myAllocator);
  ensureSize(myCompoundsOfCompound, static_cast<uint32_t>(theCompounds.Size()), myAllocator);
  ensureSize(myCompoundsOfCompSolid, static_cast<uint32_t>(theCompSolids.Size()), myAllocator);
  ensureSize(myCompoundsOfWire, static_cast<uint32_t>(theWires.Size()), myAllocator);
  ensureSize(myCompoundsOfEdge, static_cast<uint32_t>(theEdges.Size()), myAllocator);
  ensureSize(myCompoundsOfVertex, static_cast<uint32_t>(theVertices.Size()), myAllocator);

  // Vertex -> Edges: only new edges.
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(anEdgeId.Index));
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid())
      appendUnique(myVertexToEdges, aStartVtx.Index, anEdgeId);
    if (anEndVtx.IsValid() && anEndVtx != aStartVtx)
      appendUnique(myVertexToEdges, anEndVtx.Index, anEdgeId);
  }

  // Edge -> Wires: iterate only new wire entities and their CoEdgeRefIds.
  const uint32_t aNbWires = static_cast<uint32_t>(theWires.Size());
  for (BRepGraph_WireId aWireId(theOldNbWires); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aWireId.Index));
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      if (!aCoEdgeRefId.IsValid(static_cast<uint32_t>(theCoEdgeRefs.Size())))
        continue;
      const BRepGraphInc::CoEdgeRef& aRef =
        theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
        continue;
      appendUnique(myEdgeToWires, aCoEdge.EdgeDefId.Index, aWireId);
    }
  }

  // Edge -> CoEdges: scan only newly appended coedges (they may reference old edges).
  Standard_ASSERT_RAISE(
    myNbIndexedCoEdges <= theCoEdges.Size(),
    "BRepGraphInc_ReverseIndex::BuildDelta: myNbIndexedCoEdges cursor out of range "
    "(indicates CoEdge vector shrank between delta calls, which is unsupported)");
  const uint32_t aOldNbIndexedCoEdges = myNbIndexedCoEdges;
  const uint32_t aNbCoEdges           = static_cast<uint32_t>(theCoEdges.Size());
  for (BRepGraph_CoEdgeId aCoEdgeId(aOldNbIndexedCoEdges); aCoEdgeId.IsValid(aNbCoEdges);
       ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendUnique(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, aCoEdgeId);
  }
  myNbIndexedCoEdges = static_cast<uint32_t>(theCoEdges.Size());

  // CoEdge -> Wires: iterate only new wire entities and their CoEdgeRefIds.
  ensureSize(myCoEdgeToWires, static_cast<uint32_t>(theCoEdges.Size()), myAllocator);
  for (BRepGraph_WireId aWireId(theOldNbWires); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aWireId.Index));
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      if (!aCoEdgeRefId.IsValid(static_cast<uint32_t>(theCoEdgeRefs.Size())))
        continue;
      const BRepGraphInc::CoEdgeRef& aRef =
        theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
        continue;
      if (theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index)).IsRemoved)
        continue;
      appendUnique(myCoEdgeToWires, aRef.CoEdgeDefId.Index, aWireId);
    }
  }

  // Edge -> Faces: derive from CoEdge.FaceDefId for new edges.
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    if (theEdges.Value(static_cast<size_t>(anEdgeId.Index)).IsRemoved)
      continue;
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      seekVec(myEdgeToCoEdges, anEdgeId.Index);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const size_t aNbCE = aCoEdgeIdxs->Size();

    NCollection_LocalArray<uint32_t, 8> aFaces(aNbCE);
    size_t                              aNbFaces = 0;
    for (const auto& aCoEdgeId : *aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
      if (aCoEdge.FaceDefId.IsValid())
        aFaces[aNbFaces++] = aCoEdge.FaceDefId.Index;
    }
    if (aNbFaces == 0)
      continue;

    std::sort(static_cast<uint32_t*>(aFaces), static_cast<uint32_t*>(aFaces) + aNbFaces);

    uint32_t aPrev = UINT32_MAX;
    for (size_t i = 0; i < aNbFaces; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendDirect(myEdgeToFaces, anEdgeId.Index, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Wire -> Faces: iterate only new face entities and their WireRefIds.
  const uint32_t aNbFaces = static_cast<uint32_t>(theFaces.Size());
  for (BRepGraph_FaceId aFaceId(theOldNbFaces); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(static_cast<size_t>(aFaceId.Index));
    if (aFace.IsRemoved)
      continue;
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      if (!aWireRefId.IsValid(static_cast<uint32_t>(theWireRefs.Size())))
        continue;
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(static_cast<size_t>(aWireRefId.Index));
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid(static_cast<uint32_t>(theWires.Size())))
        continue;
      if (theWires.Value(static_cast<size_t>(aRef.WireDefId.Index)).IsRemoved)
        continue;
      appendUnique(myWireToFaces, aRef.WireDefId.Index, aFaceId);
    }
  }

  // Face -> Shells: iterate only new shell entities and their FaceRefIds.
  const uint32_t aNbShells = static_cast<uint32_t>(theShells.Size());
  for (BRepGraph_ShellId aShellId(theOldNbShells); aShellId.IsValid(aNbShells); ++aShellId)
  {
    const BRepGraphInc::ShellDef& aShell = theShells.Value(static_cast<size_t>(aShellId.Index));
    if (aShell.IsRemoved)
      continue;
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      if (!aFaceRefId.IsValid(static_cast<uint32_t>(theFaceRefs.Size())))
        continue;
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(static_cast<size_t>(aFaceRefId.Index));
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(static_cast<uint32_t>(theFaces.Size())))
        continue;
      if (theFaces.Value(static_cast<size_t>(aRef.FaceDefId.Index)).IsRemoved)
        continue;
      appendUnique(myFaceToShells, aRef.FaceDefId.Index, aShellId);
    }
  }

  // Shell -> Solids: iterate only new solid entities and their ShellRefIds.
  const uint32_t aNbSolids = static_cast<uint32_t>(theSolids.Size());
  for (BRepGraph_SolidId aSolidId(theOldNbSolids); aSolidId.IsValid(aNbSolids); ++aSolidId)
  {
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(static_cast<size_t>(aSolidId.Index));
    if (aSolid.IsRemoved)
      continue;
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      if (!aShellRefId.IsValid(static_cast<uint32_t>(theShellRefs.Size())))
        continue;
      const BRepGraphInc::ShellRef& aRef =
        theShellRefs.Value(static_cast<size_t>(aShellRefId.Index));
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid(static_cast<uint32_t>(theShells.Size())))
        continue;
      if (theShells.Value(static_cast<size_t>(aRef.ShellDefId.Index)).IsRemoved)
        continue;
      appendUnique(myShellToSolids, aRef.ShellDefId.Index, aSolidId);
    }
  }

  uint32_t anOldNbCompounds = theOldNbCompounds;
  if (anOldNbCompounds > theCompounds.Size())
    anOldNbCompounds = 0;

  // Compound -> child reverse indices:
  // 1) process all refs of newly appended compound parents
  // 2) process refs attached to pre-existing parents (covers appended refs on old parents)
  const uint32_t aNbCompounds = static_cast<uint32_t>(theCompounds.Size());
  for (BRepGraph_CompoundId aCompoundId(anOldNbCompounds); aCompoundId.IsValid(aNbCompounds);
       ++aCompoundId)
  {
    const BRepGraphInc::CompoundDef& aComp =
      theCompounds.Value(static_cast<size_t>(aCompoundId.Index));
    if (aComp.IsRemoved)
      continue;
    for (const BRepGraph_ChildRefId& aChildRefId : aComp.ChildRefIds)
    {
      const uint32_t aChildRefIdx = aChildRefId.Index;
      if (aChildRefIdx >= theChildRefs.Size())
        continue;
      const BRepGraphInc::ChildRef& aRef = theChildRefs.Value(static_cast<size_t>(aChildRefIdx));
      if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
        continue;
      if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid
          && aRef.ChildDefId.Index < theSolids.Size())
        appendUnique(myCompoundsOfSolid, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Shell
               && aRef.ChildDefId.Index < theShells.Size())
        appendUnique(myCompoundsOfShell, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Face
               && aRef.ChildDefId.Index < theFaces.Size())
        appendUnique(myCompoundsOfFace, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Compound
               && aRef.ChildDefId.Index < theCompounds.Size())
        appendUnique(myCompoundsOfCompound, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::CompSolid
               && aRef.ChildDefId.Index < theCompSolids.Size())
        appendUnique(myCompoundsOfCompSolid, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Wire
               && aRef.ChildDefId.Index < theWires.Size())
        appendUnique(myCompoundsOfWire, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Edge
               && aRef.ChildDefId.Index < theEdges.Size())
        appendUnique(myCompoundsOfEdge, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Vertex
               && aRef.ChildDefId.Index < theVertices.Size())
        appendUnique(myCompoundsOfVertex, aRef.ChildDefId.Index, aCompoundId);
    }
  }

  // Loop 2: process ChildRefs appended during this delta that point to a
  // pre-existing compound parent. Iterate only [theOldNbChildRefs, NbChildRefs)
  // so cost stays O(delta refs) regardless of pre-existing graph size.
  uint32_t aOldNbChildRefs = theOldNbChildRefs;
  if (aOldNbChildRefs > theChildRefs.Size())
    aOldNbChildRefs = 0;
  const uint32_t aNbChildRefs = static_cast<uint32_t>(theChildRefs.Size());
  for (BRepGraph_ChildRefId aChildRefId(aOldNbChildRefs); aChildRefId.IsValid(aNbChildRefs);
       ++aChildRefId)
  {
    const BRepGraphInc::ChildRef& aRef = theChildRefs.Value(static_cast<size_t>(aChildRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid() || !aRef.ChildDefId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Compound)
      continue;

    const uint32_t aCompoundIdx = aRef.ParentId.Index;
    if (aCompoundIdx >= theCompounds.Size() || aCompoundIdx >= anOldNbCompounds)
      continue;

    const BRepGraphInc::CompoundDef& aComp = theCompounds.Value(static_cast<size_t>(aCompoundIdx));
    if (aComp.IsRemoved)
      continue;

    if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid
        && aRef.ChildDefId.Index < theSolids.Size())
      appendUnique(myCompoundsOfSolid, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Shell
             && aRef.ChildDefId.Index < theShells.Size())
      appendUnique(myCompoundsOfShell, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Face
             && aRef.ChildDefId.Index < theFaces.Size())
      appendUnique(myCompoundsOfFace, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Compound
             && aRef.ChildDefId.Index < theCompounds.Size())
      appendUnique(myCompoundsOfCompound,
                   aRef.ChildDefId.Index,
                   BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::CompSolid
             && aRef.ChildDefId.Index < theCompSolids.Size())
      appendUnique(myCompoundsOfCompSolid,
                   aRef.ChildDefId.Index,
                   BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Wire
             && aRef.ChildDefId.Index < theWires.Size())
      appendUnique(myCompoundsOfWire, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Edge
             && aRef.ChildDefId.Index < theEdges.Size())
      appendUnique(myCompoundsOfEdge, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
    else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Vertex
             && aRef.ChildDefId.Index < theVertices.Size())
      appendUnique(myCompoundsOfVertex, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompoundIdx));
  }

  uint32_t anOldNbCompSolids = theOldNbCompSolids;
  if (anOldNbCompSolids > theCompSolids.Size())
    anOldNbCompSolids = 0;

  // CompSolid -> Solid reverse index:
  // 1) process all refs of newly appended compsolid parents
  // 2) process refs attached to pre-existing parents (covers appended refs on old parents)
  const uint32_t aNbCompSolids = static_cast<uint32_t>(theCompSolids.Size());
  for (BRepGraph_CompSolidId aCompSolidId(anOldNbCompSolids); aCompSolidId.IsValid(aNbCompSolids);
       ++aCompSolidId)
  {
    const BRepGraphInc::CompSolidDef& aCS =
      theCompSolids.Value(static_cast<size_t>(aCompSolidId.Index));
    if (aCS.IsRemoved)
      continue;
    for (const BRepGraph_SolidRefId& aSolidRefId : aCS.SolidRefIds)
    {
      if (!aSolidRefId.IsValid(static_cast<uint32_t>(theSolidRefs.Size())))
        continue;
      const BRepGraphInc::SolidRef& aRef =
        theSolidRefs.Value(static_cast<size_t>(aSolidRefId.Index));
      if (aRef.IsRemoved || !aRef.SolidDefId.IsValid(static_cast<uint32_t>(theSolids.Size())))
        continue;
      if (theSolids.Value(static_cast<size_t>(aRef.SolidDefId.Index)).IsRemoved)
        continue;
      appendUnique(myCompSolidsOfSolid, aRef.SolidDefId.Index, aCompSolidId);
    }
  }

  // Loop 2: process SolidRefs appended during this delta that target a
  // pre-existing compsolid parent. Iterate only [theOldNbSolidRefs, NbSolidRefs).
  uint32_t aOldNbSolidRefs = theOldNbSolidRefs;
  if (aOldNbSolidRefs > theSolidRefs.Size())
    aOldNbSolidRefs = 0;
  const uint32_t aNbSolidRefs = static_cast<uint32_t>(theSolidRefs.Size());
  for (BRepGraph_SolidRefId aSolidRefId(aOldNbSolidRefs); aSolidRefId.IsValid(aNbSolidRefs);
       ++aSolidRefId)
  {
    const BRepGraphInc::SolidRef& aRef = theSolidRefs.Value(static_cast<size_t>(aSolidRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid() || !aRef.SolidDefId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::CompSolid)
      continue;

    const uint32_t aCompSolidIdx = aRef.ParentId.Index;
    if (aCompSolidIdx >= theCompSolids.Size() || aCompSolidIdx >= anOldNbCompSolids
        || !aRef.SolidDefId.IsValid(static_cast<uint32_t>(theSolids.Size())))
      continue;

    const BRepGraphInc::CompSolidDef& aCS = theCompSolids.Value(static_cast<size_t>(aCompSolidIdx));
    if (aCS.IsRemoved || theSolids.Value(static_cast<size_t>(aRef.SolidDefId.Index)).IsRemoved)
      continue;

    appendUnique(myCompSolidsOfSolid, aRef.SolidDefId.Index, BRepGraph_CompSolidId(aCompSolidIdx));
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToWire(const BRepGraph_EdgeId theEdgeId,
                                               const BRepGraph_WireId theWireId)
{
  appendUnique(myEdgeToWires, theEdgeId.Index, theWireId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindCoEdgeToWire(const BRepGraph_CoEdgeId theCoEdgeId,
                                                 const BRepGraph_WireId   theWireId)
{
  appendUnique(myCoEdgeToWires, theCoEdgeId.Index, theWireId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromWire(const BRepGraph_EdgeId theEdgeId,
                                                   const BRepGraph_WireId theWireId)
{
  if (theEdgeId.Index >= myEdgeToWires.Size())
    return;
  NCollection_DynamicArray<BRepGraph_WireId>& aWires =
    myEdgeToWires.ChangeValue(static_cast<size_t>(theEdgeId.Index));
  uint32_t anIdx = 0;
  for (NCollection_DynamicArray<BRepGraph_WireId>::Iterator anIt(aWires); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() == theWireId)
    {
      if (anIdx < static_cast<uint32_t>(aWires.Size()) - 1u)
        aWires.ChangeValue(static_cast<size_t>(anIdx)) = aWires.Value(aWires.Size() - 1u);
      aWires.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindCoEdgeFromWire(const BRepGraph_CoEdgeId theCoEdgeId,
                                                     const BRepGraph_WireId   theWireId)
{
  if (theCoEdgeId.Index >= myCoEdgeToWires.Size())
    return;
  NCollection_DynamicArray<BRepGraph_WireId>& aWires =
    myCoEdgeToWires.ChangeValue(static_cast<size_t>(theCoEdgeId.Index));
  uint32_t anIdx = 0;
  for (NCollection_DynamicArray<BRepGraph_WireId>::Iterator anIt(aWires); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() == theWireId)
    {
      if (anIdx < static_cast<uint32_t>(aWires.Size()) - 1u)
        aWires.ChangeValue(static_cast<size_t>(anIdx)) = aWires.Value(aWires.Size() - 1u);
      aWires.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::ReplaceEdgeInWireMap(const BRepGraph_EdgeId theOldEdgeId,
                                                     const BRepGraph_EdgeId theNewEdgeId,
                                                     const BRepGraph_WireId theWireId)
{
  UnbindEdgeFromWire(theOldEdgeId, theWireId);
  BindEdgeToWire(theNewEdgeId, theWireId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindVertexToEdge(const BRepGraph_VertexId theVertexId,
                                                 const BRepGraph_EdgeId   theEdgeId)
{
  appendUnique(myVertexToEdges, theVertexId.Index, theEdgeId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindVertexFromEdge(const BRepGraph_VertexId theVertexId,
                                                     const BRepGraph_EdgeId   theEdgeId)
{
  if (theVertexId.Index >= myVertexToEdges.Size())
    return;
  NCollection_DynamicArray<BRepGraph_EdgeId>& anEdges =
    myVertexToEdges.ChangeValue(static_cast<size_t>(theVertexId.Index));
  uint32_t anIdx = 0;
  for (NCollection_DynamicArray<BRepGraph_EdgeId>::Iterator anIt(anEdges); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() == theEdgeId)
    {
      if (anIdx < static_cast<uint32_t>(anEdges.Size()) - 1u)
        anEdges.ChangeValue(static_cast<size_t>(anIdx)) = anEdges.Value(anEdges.Size() - 1u);
      anEdges.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToCoEdge(const BRepGraph_EdgeId   theEdgeId,
                                                 const BRepGraph_CoEdgeId theCoEdgeId)
{
  appendUnique(myEdgeToCoEdges, theEdgeId.Index, theCoEdgeId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromCoEdge(const BRepGraph_EdgeId   theEdgeId,
                                                     const BRepGraph_CoEdgeId theCoEdgeId)
{
  if (theEdgeId.Index >= myEdgeToCoEdges.Size())
    return;
  NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
    myEdgeToCoEdges.ChangeValue(static_cast<size_t>(theEdgeId.Index));
  uint32_t anIdx = 0;
  for (NCollection_DynamicArray<BRepGraph_CoEdgeId>::Iterator anIt(aCoEdges); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() == theCoEdgeId)
    {
      if (anIdx < static_cast<uint32_t>(aCoEdges.Size()) - 1u)
        aCoEdges.ChangeValue(static_cast<size_t>(anIdx)) = aCoEdges.Value(aCoEdges.Size() - 1u);
      aCoEdges.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToFace(const BRepGraph_EdgeId theEdgeId,
                                               const BRepGraph_FaceId theFaceId)
{
  appendUnique(myEdgeToFaces, theEdgeId.Index, theFaceId);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromFace(const BRepGraph_EdgeId theEdgeId,
                                                   const BRepGraph_FaceId theFaceId)
{
  if (theEdgeId.Index >= myEdgeToFaces.Size())
    return;
  NCollection_DynamicArray<BRepGraph_FaceId>& aFaces =
    myEdgeToFaces.ChangeValue(static_cast<size_t>(theEdgeId.Index));
  uint32_t anIdx = 0;
  for (NCollection_DynamicArray<BRepGraph_FaceId>::Iterator anIt(aFaces); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() == theFaceId)
    {
      if (anIdx < static_cast<uint32_t>(aFaces.Size()) - 1u)
        aFaces.ChangeValue(static_cast<size_t>(anIdx)) = aFaces.Value(aFaces.Size() - 1u);
      aFaces.EraseLast();
      break;
    }
  }
}

//=================================================================================================

bool BRepGraphInc_ReverseIndex::Validate(
  const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
  const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
  const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
  const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
  const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
  const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
  const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
  const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
  const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
  const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
  const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
  const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
  const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
  const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
  const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs) const
{
  auto hasActiveWireUsageOfEdge = [&](const BRepGraph_WireId theWireId,
                                      const BRepGraph_EdgeId theEdgeId) -> bool {
    if (!theWireId.IsValid(static_cast<uint32_t>(theWires.Size()))
        || !theEdgeId.IsValid(static_cast<uint32_t>(theEdges.Size())))
    {
      return false;
    }
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(theWireId.Index));
    if (aWire.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      if (!aCoEdgeRefId.IsValid(static_cast<uint32_t>(theCoEdgeRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::CoEdgeRef& aRef =
        theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_WireId(aRef.ParentId) != theWireId)
      {
        continue;
      }
      if (!aRef.CoEdgeDefId.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
      {
        continue;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
      {
        continue;
      }
      if (aCoEdge.EdgeDefId == theEdgeId)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveFaceRefForWire = [&](const BRepGraph_WireId theWireId,
                                     const BRepGraph_FaceId theFaceId) -> bool {
    if (!theWireId.IsValid(static_cast<uint32_t>(theWires.Size()))
        || !theFaceId.IsValid(static_cast<uint32_t>(theFaces.Size())))
    {
      return false;
    }
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(static_cast<size_t>(theFaceId.Index));
    if (aFace.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      if (!aWireRefId.IsValid(static_cast<uint32_t>(theWireRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(static_cast<size_t>(aWireRefId.Index));
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_FaceId(aRef.ParentId) != theFaceId)
      {
        continue;
      }
      if (aRef.WireDefId == theWireId)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveFaceForEdge = [&](const BRepGraph_EdgeId theEdgeId,
                                  const BRepGraph_FaceId theFaceId) -> bool {
    if (!theEdgeId.IsValid(static_cast<uint32_t>(theEdges.Size()))
        || !theFaceId.IsValid(static_cast<uint32_t>(theFaces.Size())))
    {
      return false;
    }
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(theEdgeId.Index));
    if (anEdge.IsRemoved)
    {
      return false;
    }

    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdgeIds =
      seekVec(myEdgeToCoEdges, theEdgeId.Index);
    if (aCoEdgeIds == nullptr)
    {
      return false;
    }

    return hasActiveFaceForEdgeInCoEdges(*aCoEdgeIds, theCoEdges, theEdgeId, theFaceId);
  };

  auto hasActiveFaceRef = [&](const BRepGraph_ShellId theShellId,
                              const BRepGraph_FaceId  theFaceId) -> bool {
    if (!theShellId.IsValid(static_cast<uint32_t>(theShells.Size()))
        || !theFaceId.IsValid(static_cast<uint32_t>(theFaces.Size())))
    {
      return false;
    }
    const BRepGraphInc::ShellDef& aShell = theShells.Value(static_cast<size_t>(theShellId.Index));
    if (aShell.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      if (!aFaceRefId.IsValid(static_cast<uint32_t>(theFaceRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(static_cast<size_t>(aFaceRefId.Index));
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_ShellId(aRef.ParentId) != theShellId)
      {
        continue;
      }
      if (aRef.FaceDefId == theFaceId)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveShellRef = [&](const BRepGraph_SolidId theSolidId,
                               const BRepGraph_ShellId theShellId) -> bool {
    if (!theSolidId.IsValid(static_cast<uint32_t>(theSolids.Size()))
        || !theShellId.IsValid(static_cast<uint32_t>(theShells.Size())))
    {
      return false;
    }
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(static_cast<size_t>(theSolidId.Index));
    if (aSolid.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      if (!aShellRefId.IsValid(static_cast<uint32_t>(theShellRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::ShellRef& aRef =
        theShellRefs.Value(static_cast<size_t>(aShellRefId.Index));
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_SolidId(aRef.ParentId) != theSolidId)
      {
        continue;
      }
      if (aRef.ShellDefId == theShellId)
      {
        return true;
      }
    }
    return false;
  };

  auto isActiveChildNode = [&](const BRepGraph_NodeId theChildId) -> bool {
    if (!theChildId.IsValid())
    {
      return false;
    }

    switch (theChildId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:
        return theChildId.Index < theSolids.Size()
               && !theSolids.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Shell:
        return theChildId.Index < theShells.Size()
               && !theShells.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Face:
        return theChildId.Index < theFaces.Size()
               && !theFaces.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Compound:
        return theChildId.Index < theCompounds.Size()
               && !theCompounds.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::CompSolid:
        return theChildId.Index < theCompSolids.Size()
               && !theCompSolids.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Wire:
        return theChildId.Index < theWires.Size()
               && !theWires.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Edge:
        return theChildId.Index < theEdges.Size()
               && !theEdges.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      case BRepGraph_NodeId::Kind::Vertex:
        return theChildId.Index < theVertices.Size()
               && !theVertices.Value(static_cast<size_t>(theChildId.Index)).IsRemoved;
      default:
        return false;
    }
  };

  auto hasActiveCompoundChildRef = [&](const BRepGraph_CompoundId theCompoundId,
                                       const BRepGraph_NodeId     theChildId) -> bool {
    if (!theCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size()))
        || !isActiveChildNode(theChildId))
    {
      return false;
    }

    const BRepGraphInc::CompoundDef& aCompound =
      theCompounds.Value(static_cast<size_t>(theCompoundId.Index));
    if (aCompound.IsRemoved)
    {
      return false;
    }

    for (const BRepGraph_ChildRefId& aChildRefId : aCompound.ChildRefIds)
    {
      if (!aChildRefId.IsValid(static_cast<uint32_t>(theChildRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::ChildRef& aRef =
        theChildRefs.Value(static_cast<size_t>(aChildRefId.Index));
      if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_CompoundId(aRef.ParentId) != theCompoundId)
      {
        continue;
      }
      if (aRef.ChildDefId == theChildId)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveCompSolidRef = [&](const BRepGraph_CompSolidId theCompSolidId,
                                   const BRepGraph_SolidId     theSolidId) -> bool {
    if (!theCompSolidId.IsValid(static_cast<uint32_t>(theCompSolids.Size()))
        || !theSolidId.IsValid(static_cast<uint32_t>(theSolids.Size())))
    {
      return false;
    }

    const BRepGraphInc::CompSolidDef& aCompSolid =
      theCompSolids.Value(static_cast<size_t>(theCompSolidId.Index));
    if (aCompSolid.IsRemoved || theSolids.Value(static_cast<size_t>(theSolidId.Index)).IsRemoved)
    {
      return false;
    }

    for (const BRepGraph_SolidRefId& aSolidRefId : aCompSolid.SolidRefIds)
    {
      if (!aSolidRefId.IsValid(static_cast<uint32_t>(theSolidRefs.Size())))
      {
        continue;
      }
      const BRepGraphInc::SolidRef& aRef =
        theSolidRefs.Value(static_cast<size_t>(aSolidRefId.Index));
      if (aRef.IsRemoved || !aRef.SolidDefId.IsValid())
      {
        continue;
      }
      if (BRepGraph_CompSolidId(aRef.ParentId) != theCompSolidId)
      {
        continue;
      }
      if (aRef.SolidDefId == theSolidId)
      {
        return true;
      }
    }
    return false;
  };

  // Check: for each coedge ref entry, edge->wire reverse entry must exist.
  const uint32_t aNbCoEdgeRefs = static_cast<uint32_t>(theCoEdgeRefs.Size());
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs); ++aCoEdgeRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef =
      theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire || !aRef.CoEdgeDefId.IsValid())
      continue;
    if (aRef.ParentId.Index >= theWires.Size() || aRef.CoEdgeDefId.Index >= theCoEdges.Size())
      return false;
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aWire.IsRemoved)
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index));
    if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
      return false;
    if (!containsIndexInTable(myEdgeToWires, aCoEdge.EdgeDefId, BRepGraph_WireId(aRef.ParentId)))
      return false;
  }

  // Check: for each active coedge ref entry, coedge->wire reverse entry must exist.
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs); ++aCoEdgeRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef =
      theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire || !aRef.CoEdgeDefId.IsValid())
      continue;
    if (aRef.ParentId.Index >= theWires.Size() || aRef.CoEdgeDefId.Index >= theCoEdges.Size())
      return false;
    const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aWire.IsRemoved)
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      theCoEdges.Value(static_cast<size_t>(aRef.CoEdgeDefId.Index));
    if (aCoEdge.IsRemoved)
      continue;
    if (!containsIndexInTable(myCoEdgeToWires, aRef.CoEdgeDefId, BRepGraph_WireId(aRef.ParentId)))
      return false;
  }

  // Check: for each edge's start/end vertex, vertex->edge reverse entry must exist.
  const uint32_t aNbEdges = static_cast<uint32_t>(theEdges.Size());
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(static_cast<size_t>(anEdgeId.Index));
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid())
    {
      if (!containsIndexInTable(myVertexToEdges, aStartVtx, anEdgeId))
        return false;
    }
    if (anEndVtx.IsValid() && anEndVtx != aStartVtx)
    {
      if (!containsIndexInTable(myVertexToEdges, anEndVtx, anEdgeId))
        return false;
    }
  }

  // Check: for each edge's CoEdges with valid FaceDefId, edge->face reverse entry must exist.
  const uint32_t aNbCoEdges = static_cast<uint32_t>(theCoEdges.Size());
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(static_cast<size_t>(aCoEdgeId.Index));
    if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      continue;
    if (!containsIndexInTable(myEdgeToFaces, aCoEdge.EdgeDefId, aCoEdge.FaceDefId))
      return false;
  }

  // Check: for each wire ref entry, wire->face reverse entry must exist.
  const uint32_t aNbWireRefs = static_cast<uint32_t>(theWireRefs.Size());
  for (BRepGraph_WireRefId aWireRefId(0); aWireRefId.IsValid(aNbWireRefs); ++aWireRefId)
  {
    const BRepGraphInc::WireRef& aRef = theWireRefs.Value(static_cast<size_t>(aWireRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Face || !aRef.WireDefId.IsValid())
      continue;
    if (aRef.ParentId.Index >= theFaces.Size() || aRef.WireDefId.Index >= theWires.Size())
      return false;
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aFace.IsRemoved || theWires.Value(static_cast<size_t>(aRef.WireDefId.Index)).IsRemoved)
      continue;
    if (!containsIndexInTable(myWireToFaces, aRef.WireDefId, BRepGraph_FaceId(aRef.ParentId)))
      return false;
  }

  // Check: for each face ref entry, face->shell reverse entry must exist.
  const uint32_t aNbFaceRefs = static_cast<uint32_t>(theFaceRefs.Size());
  for (BRepGraph_FaceRefId aFaceRefId(0); aFaceRefId.IsValid(aNbFaceRefs); ++aFaceRefId)
  {
    const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(static_cast<size_t>(aFaceRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Shell || !aRef.FaceDefId.IsValid())
      continue;
    if (aRef.ParentId.Index >= theShells.Size() || aRef.FaceDefId.Index >= theFaces.Size())
      return false;
    const BRepGraphInc::ShellDef& aShell =
      theShells.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aShell.IsRemoved || theFaces.Value(static_cast<size_t>(aRef.FaceDefId.Index)).IsRemoved)
      continue;
    if (!containsIndexInTable(myFaceToShells, aRef.FaceDefId, BRepGraph_ShellId(aRef.ParentId)))
      return false;
  }

  // Check: for each shell ref entry, shell->solid reverse entry must exist.
  const uint32_t aNbShellRefs = static_cast<uint32_t>(theShellRefs.Size());
  for (BRepGraph_ShellRefId aShellRefId(0); aShellRefId.IsValid(aNbShellRefs); ++aShellRefId)
  {
    const BRepGraphInc::ShellRef& aRef = theShellRefs.Value(static_cast<size_t>(aShellRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Solid || !aRef.ShellDefId.IsValid())
      continue;
    if (aRef.ParentId.Index >= theSolids.Size() || aRef.ShellDefId.Index >= theShells.Size())
      return false;
    const BRepGraphInc::SolidDef& aSolid =
      theSolids.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aSolid.IsRemoved || theShells.Value(static_cast<size_t>(aRef.ShellDefId.Index)).IsRemoved)
      continue;
    if (!containsIndexInTable(myShellToSolids, aRef.ShellDefId, BRepGraph_SolidId(aRef.ParentId)))
      return false;
  }

  // Check: for each solid ref entry, solid->compsolid reverse entry must exist.
  const uint32_t aNbSolidRefs = static_cast<uint32_t>(theSolidRefs.Size());
  for (BRepGraph_SolidRefId aSolidRefId(0); aSolidRefId.IsValid(aNbSolidRefs); ++aSolidRefId)
  {
    const BRepGraphInc::SolidRef& aRef = theSolidRefs.Value(static_cast<size_t>(aSolidRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::CompSolid
        || !aRef.SolidDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index >= theCompSolids.Size() || aRef.SolidDefId.Index >= theSolids.Size())
    {
      return false;
    }
    const BRepGraphInc::CompSolidDef& aCompSolid =
      theCompSolids.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aCompSolid.IsRemoved
        || theSolids.Value(static_cast<size_t>(aRef.SolidDefId.Index)).IsRemoved)
    {
      continue;
    }
    if (!containsIndexInTable(myCompSolidsOfSolid,
                              aRef.SolidDefId,
                              BRepGraph_CompSolidId(aRef.ParentId)))
    {
      return false;
    }
  }

  // Check: for each compound child ref entry, child->compound reverse entry must exist.
  const uint32_t aNbChildRefs = static_cast<uint32_t>(theChildRefs.Size());
  for (BRepGraph_ChildRefId aChildRefId(0); aChildRefId.IsValid(aNbChildRefs); ++aChildRefId)
  {
    const BRepGraphInc::ChildRef& aRef = theChildRefs.Value(static_cast<size_t>(aChildRefId.Index));
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Compound || !aRef.ChildDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index >= theCompounds.Size())
    {
      return false;
    }
    const BRepGraphInc::CompoundDef& aCompound =
      theCompounds.Value(static_cast<size_t>(aRef.ParentId.Index));
    if (aCompound.IsRemoved || !isActiveChildNode(aRef.ChildDefId))
    {
      continue;
    }

    switch (aRef.ChildDefId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:
        if (!containsIndexInTable(myCompoundsOfSolid,
                                  BRepGraph_SolidId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Shell:
        if (!containsIndexInTable(myCompoundsOfShell,
                                  BRepGraph_ShellId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Face:
        if (!containsIndexInTable(myCompoundsOfFace,
                                  BRepGraph_FaceId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Compound:
        if (!containsIndexInTable(myCompoundsOfCompound,
                                  BRepGraph_CompoundId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        if (!containsIndexInTable(myCompoundsOfCompSolid,
                                  BRepGraph_CompSolidId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Wire:
        if (!containsIndexInTable(myCompoundsOfWire,
                                  BRepGraph_WireId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Edge:
        if (!containsIndexInTable(myCompoundsOfEdge,
                                  BRepGraph_EdgeId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      case BRepGraph_NodeId::Kind::Vertex:
        if (!containsIndexInTable(myCompoundsOfVertex,
                                  BRepGraph_VertexId(aRef.ChildDefId),
                                  BRepGraph_CompoundId(aRef.ParentId)))
          return false;
        break;
      default:
        return false;
    }
  }

  // Check reverse tables for stale/extra entries not backed by active forward refs.
  const uint32_t aNbEdgeToWires = static_cast<uint32_t>(myEdgeToWires.Size());
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToWires); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_WireId>& aWires = WiresOfEdgeRef(anEdgeId);
    for (const BRepGraph_WireId& aWireId2 : aWires)
    {
      if (!aWireId2.IsValid(static_cast<uint32_t>(theWires.Size())))
      {
        return false;
      }
      if (!hasActiveWireUsageOfEdge(aWireId2, anEdgeId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbEdgeToCoEdges = static_cast<uint32_t>(myEdgeToCoEdges.Size());
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToCoEdges); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges = CoEdgesOfEdgeRef(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId2 : aCoEdges)
    {
      if (!aCoEdgeId2.IsValid(static_cast<uint32_t>(theCoEdges.Size())))
      {
        return false;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCoEdges.Value(static_cast<size_t>(aCoEdgeId2.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid() || aCoEdge.EdgeDefId != anEdgeId)
      {
        return false;
      }
    }
  }

  const uint32_t aNbWireToFaces = static_cast<uint32_t>(myWireToFaces.Size());
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWireToFaces); ++aWireId)
  {
    const NCollection_DynamicArray<BRepGraph_FaceId>& aFaces = FacesOfWireRef(aWireId);
    for (const BRepGraph_FaceId& aFaceId2 : aFaces)
    {
      if (!aFaceId2.IsValid(static_cast<uint32_t>(theFaces.Size())))
      {
        return false;
      }
      if (!hasActiveFaceRefForWire(aWireId, aFaceId2))
      {
        return false;
      }
    }
  }

  const uint32_t aNbEdgeToFaces = static_cast<uint32_t>(myEdgeToFaces.Size());
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToFaces); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
      seekVec(myEdgeToFaces, anEdgeId.Index);
    if (aFaces == nullptr)
    {
      continue;
    }
    for (const BRepGraph_FaceId& aFaceId2 : *aFaces)
    {
      if (!aFaceId2.IsValid(static_cast<uint32_t>(theFaces.Size())))
      {
        return false;
      }
      if (!hasActiveFaceForEdge(anEdgeId, aFaceId2))
      {
        return false;
      }
    }
  }

  const uint32_t aNbFaceToShells = static_cast<uint32_t>(myFaceToShells.Size());
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaceToShells); ++aFaceId)
  {
    const NCollection_DynamicArray<BRepGraph_ShellId>* aShellsVec =
      seekVec(myFaceToShells, aFaceId.Index);
    if (aShellsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_ShellId& aShellId2 : *aShellsVec)
    {
      if (!aShellId2.IsValid(static_cast<uint32_t>(theShells.Size())))
      {
        return false;
      }
      if (!hasActiveFaceRef(aShellId2, aFaceId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbShellToSolids = static_cast<uint32_t>(myShellToSolids.Size());
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShellToSolids); ++aShellId)
  {
    const NCollection_DynamicArray<BRepGraph_SolidId>* aSolidsVec =
      seekVec(myShellToSolids, aShellId.Index);
    if (aSolidsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_SolidId& aSolidId2 : *aSolidsVec)
    {
      if (!aSolidId2.IsValid(static_cast<uint32_t>(theSolids.Size())))
      {
        return false;
      }
      if (!hasActiveShellRef(aSolidId2, aShellId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompoundsOfSolid = static_cast<uint32_t>(myCompoundsOfSolid.Size());
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbCompoundsOfSolid); ++aSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfSolid, aSolidId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aSolidId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompSolidsOfSolid = static_cast<uint32_t>(myCompSolidsOfSolid.Size());
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbCompSolidsOfSolid); ++aSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCompSolidsVec =
      seekVec(myCompSolidsOfSolid, aSolidId.Index);
    if (aCompSolidsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompSolidId& aCompSolidId : *aCompSolidsVec)
    {
      if (!aCompSolidId.IsValid(static_cast<uint32_t>(theCompSolids.Size())))
      {
        return false;
      }
      if (!hasActiveCompSolidRef(aCompSolidId, aSolidId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompoundsOfShell = static_cast<uint32_t>(myCompoundsOfShell.Size());
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbCompoundsOfShell); ++aShellId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfShell, aShellId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aShellId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompoundsOfFace = static_cast<uint32_t>(myCompoundsOfFace.Size());
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbCompoundsOfFace); ++aFaceId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfFace, aFaceId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aFaceId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompoundsOfCompound = static_cast<uint32_t>(myCompoundsOfCompound.Size());
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompoundsOfCompound);
       ++aCompoundId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfCompound, aCompoundId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aParentCompoundId : *aCompoundsVec)
    {
      if (!aParentCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aParentCompoundId, aCompoundId))
      {
        return false;
      }
    }
  }

  const uint32_t aNbCompoundsOfCompSolid = static_cast<uint32_t>(myCompoundsOfCompSolid.Size());
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompoundsOfCompSolid);
       ++aCompSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfCompSolid, aCompSolidId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aCompSolidId))
      {
        return false;
      }
    }
  }

  // Check reverse: myCompoundsOfWire - each entry must be backed by an active compound ChildRef.
  const uint32_t aNbCompoundsOfWire = static_cast<uint32_t>(myCompoundsOfWire.Size());
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbCompoundsOfWire); ++aWireId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfWire, aWireId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aWireId))
      {
        return false;
      }
    }
  }

  // Check reverse: myCompoundsOfEdge - each entry must be backed by an active compound ChildRef.
  const uint32_t aNbCompoundsOfEdge = static_cast<uint32_t>(myCompoundsOfEdge.Size());
  for (BRepGraph_EdgeId anEdgeId2(0); anEdgeId2.IsValid(aNbCompoundsOfEdge); ++anEdgeId2)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfEdge, anEdgeId2.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, anEdgeId2))
      {
        return false;
      }
    }
  }

  // Check reverse: myCompoundsOfVertex - each entry must be backed by an active compound ChildRef.
  const uint32_t aNbCompoundsOfVertex = static_cast<uint32_t>(myCompoundsOfVertex.Size());
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbCompoundsOfVertex); ++aVertexId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsVec =
      seekVec(myCompoundsOfVertex, aVertexId.Index);
    if (aCompoundsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CompoundId& aCompoundId : *aCompoundsVec)
    {
      if (!aCompoundId.IsValid(static_cast<uint32_t>(theCompounds.Size())))
      {
        return false;
      }
      if (!hasActiveCompoundChildRef(aCompoundId, aVertexId))
      {
        return false;
      }
    }
  }

  // Check reverse: myCoEdgeToWires - each entry must be backed by an active CoEdgeRef in that wire.
  const uint32_t aNbCoEdgeToWires = static_cast<uint32_t>(myCoEdgeToWires.Size());
  for (BRepGraph_CoEdgeId aCoEdgeId2(0); aCoEdgeId2.IsValid(aNbCoEdgeToWires); ++aCoEdgeId2)
  {
    const NCollection_DynamicArray<BRepGraph_WireId>* aWiresVec =
      seekVec(myCoEdgeToWires, aCoEdgeId2.Index);
    if (aWiresVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_WireId& aWireId2 : *aWiresVec)
    {
      if (aWireId2.Index >= theWires.Size())
      {
        return false;
      }
      const BRepGraphInc::WireDef& aWire = theWires.Value(static_cast<size_t>(aWireId2.Index));
      if (aWire.IsRemoved)
      {
        // Stale entry: wire was removed but myCoEdgeToWires was not updated.
        return false;
      }
      bool aFound = false;
      for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId2 : aWire.CoEdgeRefIds)
      {
        if (aCoEdgeRefId2.Index >= theCoEdgeRefs.Size())
          continue;
        const BRepGraphInc::CoEdgeRef& aRef2 =
          theCoEdgeRefs.Value(static_cast<size_t>(aCoEdgeRefId2.Index));
        if (aRef2.IsRemoved || !aRef2.CoEdgeDefId.IsValid())
          continue;
        if (aRef2.CoEdgeDefId.Index == aCoEdgeId2.Index)
        {
          aFound = true;
          break;
        }
      }
      if (!aFound)
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildProductOccurrences(
  const NCollection_DynamicArray<BRepGraphInc::OccurrenceDef>& theOccurrences,
  const uint32_t                                               theNbProducts)
{
  myProductToOccurrences.Clear();
  preSize(myProductToOccurrences, theNbProducts, myAllocator);

  BRepGraph_OccurrenceId anOccurrenceId(0);
  for (const BRepGraphInc::OccurrenceDef& anOcc : theOccurrences)
  {
    if (!anOcc.IsRemoved && anOcc.ChildDefId.IsValid()
        && anOcc.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      appendDirect(myProductToOccurrences, anOcc.ChildDefId.Index, anOccurrenceId);
    }
    ++anOccurrenceId;
  }
}
