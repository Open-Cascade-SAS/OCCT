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
#include <NCollection_LocalArray.hxx>

#include <type_traits>

namespace
{

template <typename T>
BRepGraph_VertexId resolveVertexDefId(
  const NCollection_Vector<BRepGraphInc::VertexRef>& theVertexRefs,
  const T                                            theRefId)
{
  if (!theRefId.IsValid() || theRefId.Index < 0 || theRefId.Index >= theVertexRefs.Length())
    return BRepGraph_VertexId();
  return theVertexRefs.Value(theRefId.Index).VertexDefId;
}

template <typename T>
bool containsIndexInTable(const NCollection_Vector<NCollection_Vector<T>>& theIdx,
                          const int                                        theKey,
                          const int                                        theVal)
{
  if (theKey < 0 || theKey >= theIdx.Length())
    return false;
  const NCollection_Vector<T>& aVec = theIdx.Value(theKey);
  for (const T& anElem : aVec)
  {
    if (anElem.Index == theVal)
      return true;
  }
  return false;
}

static bool hasActiveFaceForEdgeInCoEdges(
  const NCollection_Vector<BRepGraph_CoEdgeId>&      theCoEdgeIds,
  const NCollection_Vector<BRepGraphInc::CoEdgeDef>& theCoEdges,
  const int                                          theEdgeIdx,
  const int                                          theFaceIdx)
{
  for (const BRepGraph_CoEdgeId& aCoEdgeId : theCoEdgeIds)
  {
    const int aCoEdgeIdx = aCoEdgeId.Index;
    if (aCoEdgeIdx < 0 || aCoEdgeIdx >= theCoEdges.Length())
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
    if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid() || !aCoEdge.FaceDefId.IsValid())
    {
      continue;
    }
    if (aCoEdge.EdgeDefId.Index == theEdgeIdx && aCoEdge.FaceDefId.Index == theFaceIdx)
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
  myCoEdgeToWires.Clear();
  myEdgeFaceCount.Clear();
  myProductToOccurrences.Clear();
  myNbIndexedCoEdges = 0;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(
  const NCollection_Vector<BRepGraphInc::EdgeDef>&      theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeDef>&    theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireDef>&      theWires,
  const NCollection_Vector<BRepGraphInc::FaceDef>&      theFaces,
  const NCollection_Vector<BRepGraphInc::ShellDef>&     theShells,
  const NCollection_Vector<BRepGraphInc::SolidDef>&     theSolids,
  const NCollection_Vector<BRepGraphInc::CompoundDef>&  theCompounds,
  const NCollection_Vector<BRepGraphInc::CompSolidDef>& theCompSolids,
  const NCollection_Vector<BRepGraphInc::ShellRef>&     theShellRefs,
  const NCollection_Vector<BRepGraphInc::FaceRef>&      theFaceRefs,
  const NCollection_Vector<BRepGraphInc::WireRef>&      theWireRefs,
  const NCollection_Vector<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
  const NCollection_Vector<BRepGraphInc::SolidRef>&     theSolidRefs,
  const NCollection_Vector<BRepGraphInc::ChildRef>&     theChildRefs,
  const NCollection_Vector<BRepGraphInc::VertexRef>&    theVertexRefs)
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
  int aMaxVertexIdx = -1;
  const int aNbEdges = theEdges.Length();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(anEdgeId.Index);
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid() && aStartVtx.Index > aMaxVertexIdx)
      aMaxVertexIdx = aStartVtx.Index;
    if (anEndVtx.IsValid() && anEndVtx.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEndVtx.Index;
  }

  // Pre-size all outer vectors to their known key range.
  // Pass allocator so inner vectors use IncAllocator for O(1) alloc/free.
  preSize(myVertexToEdges, aMaxVertexIdx + 1, myAllocator);
  preSize(myEdgeToWires, theEdges.Length(), myAllocator);
  preSize(myEdgeToFaces, theEdges.Length(), myAllocator);
  preSize(myWireToFaces, theWires.Length(), myAllocator);
  preSize(myFaceToShells, theFaces.Length(), myAllocator);
  preSize(myShellToSolids, theShells.Length(), myAllocator);

  // Vertex -> Edges: scan edge entities for start/end vertex indices.
  // Closed edges have StartVertexRefId and EndVertexRefId resolving to the same VertexDefId,
  // so skip duplicate.
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(anEdgeId.Index);
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
  const int aNbWires = theWires.Length();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(aWireId.Index);
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      const int aRefIdx = aCoEdgeRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theCoEdgeRefs.Length())
        continue;
      const BRepGraphInc::CoEdgeRef& aRef = theCoEdgeRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid() || aRef.CoEdgeDefId.Index < 0
          || aRef.CoEdgeDefId.Index >= theCoEdges.Length())
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aRef.CoEdgeDefId.Index);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
        continue;
      appendDirect(myEdgeToWires, aCoEdge.EdgeDefId.Index, aWireId);
    }
  }

  // Edge -> CoEdges: derive from CoEdge.EdgeDefId field.
  preSize(myEdgeToCoEdges, theEdges.Length(), myAllocator);
  const int aNbCoEdges = theCoEdges.Length();
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeId.Index);
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendDirect(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, aCoEdgeId);
  }
  myNbIndexedCoEdges = theCoEdges.Length();

  // Edge -> Faces: derive from CoEdge.FaceDefId (replaces legacy PCurve-based derivation).
  // Seam edges have two CoEdges with same FaceDefId but opposite Sense.
  // Deduplicate per edge using the edge->coedges index built above.
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    if (theEdges.Value(anEdgeId.Index).IsRemoved)
      continue;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs = seekVec(myEdgeToCoEdges, anEdgeId.Index);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const int aNbCE = aCoEdgeIdxs->Length();

    // Collect face indices from coedges (stack-allocated for small counts).
    NCollection_LocalArray<int, 8> aFaces(aNbCE);
    int                            aNbFaces = 0;
    for (int i = 0; i < aNbCE; ++i)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeIdxs->Value(i).Index);
      if (aCoEdge.FaceDefId.IsValid())
        aFaces[aNbFaces++] = aCoEdge.FaceDefId.Index;
    }
    if (aNbFaces == 0)
      continue;

    // Insertion sort (optimal for small N).
    for (int i = 1; i < aNbFaces; ++i)
    {
      const int aKey = aFaces[i];
      int       j    = i - 1;
      while (j >= 0 && aFaces[j] > aKey)
      {
        aFaces[j + 1] = aFaces[j];
        --j;
      }
      aFaces[j + 1] = aKey;
    }

    // Append unique sorted values.
    int aPrev = -1;
    for (int i = 0; i < aNbFaces; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendDirect(myEdgeToFaces, anEdgeId.Index, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Populate cached face counts from the edge-to-faces index.
  myEdgeFaceCount.Clear();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaceVec = seekVec(myEdgeToFaces, anEdgeId.Index);
    myEdgeFaceCount.Append(aFaceVec != nullptr ? aFaceVec->Length() : 0);
  }

  // Wire -> Faces: iterate face entities and their WireRefIds for O(1) parent lookup.
  const int aNbFaces = theFaces.Length();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(aFaceId.Index);
    if (aFace.IsRemoved)
      continue;
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      const int aRefIdx = aWireRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theWireRefs.Length())
        continue;
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid() || aRef.WireDefId.Index < 0
          || aRef.WireDefId.Index >= theWires.Length())
        continue;
      if (theWires.Value(aRef.WireDefId.Index).IsRemoved)
        continue;
      appendDirect(myWireToFaces, aRef.WireDefId.Index, aFaceId);
    }
  }

  // Face -> Shells: iterate shell entities and their FaceRefIds for O(1) parent lookup.
  const int aNbShells = theShells.Length();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
  {
    const BRepGraphInc::ShellDef& aShell = theShells.Value(aShellId.Index);
    if (aShell.IsRemoved)
      continue;
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      const int aRefIdx = aFaceRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theFaceRefs.Length())
        continue;
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid() || aRef.FaceDefId.Index < 0
          || aRef.FaceDefId.Index >= theFaces.Length())
        continue;
      if (theFaces.Value(aRef.FaceDefId.Index).IsRemoved)
        continue;
      appendDirect(myFaceToShells, aRef.FaceDefId.Index, aShellId);
    }
  }

  // Shell -> Solids: iterate solid entities and their ShellRefIds for O(1) parent lookup.
  const int aNbSolids = theSolids.Length();
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbSolids); ++aSolidId)
  {
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(aSolidId.Index);
    if (aSolid.IsRemoved)
      continue;
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      const int aRefIdx = aShellRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theShellRefs.Length())
        continue;
      const BRepGraphInc::ShellRef& aRef = theShellRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid() || aRef.ShellDefId.Index < 0
          || aRef.ShellDefId.Index >= theShells.Length())
        continue;
      if (theShells.Value(aRef.ShellDefId.Index).IsRemoved)
        continue;
      appendDirect(myShellToSolids, aRef.ShellDefId.Index, aSolidId);
    }
  }

  // Compound -> child reverse indices: iterate compound entities and their ChildRefIds.
  preSize(myCompoundsOfSolid, theSolids.Length(), myAllocator);
  preSize(myCompoundsOfShell, theShells.Length(), myAllocator);
  preSize(myCompoundsOfFace, theFaces.Length(), myAllocator);
  preSize(myCompoundsOfCompound, theCompounds.Length(), myAllocator);
  preSize(myCompoundsOfCompSolid, theCompSolids.Length(), myAllocator);
  const int aNbCompounds = theCompounds.Length();
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
  {
    const BRepGraphInc::CompoundDef& aComp = theCompounds.Value(aCompoundId.Index);
    if (aComp.IsRemoved)
      continue;
    for (const BRepGraph_ChildRefId& aChildRefId : aComp.ChildRefIds)
    {
      const int aChildRefIdx = aChildRefId.Index;
      if (aChildRefIdx < 0 || aChildRefIdx >= theChildRefs.Length())
        continue;
      const BRepGraphInc::ChildRef& aRef = theChildRefs.Value(aChildRefIdx);
      if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
        continue;
      if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid && aRef.ChildDefId.Index >= 0
          && aRef.ChildDefId.Index < theSolids.Length())
        appendDirect(myCompoundsOfSolid, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Shell
               && aRef.ChildDefId.Index >= 0 && aRef.ChildDefId.Index < theShells.Length())
        appendDirect(myCompoundsOfShell, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Face
               && aRef.ChildDefId.Index >= 0 && aRef.ChildDefId.Index < theFaces.Length())
        appendDirect(myCompoundsOfFace, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Compound
               && aRef.ChildDefId.Index >= 0 && aRef.ChildDefId.Index < theCompounds.Length())
        appendDirect(myCompoundsOfCompound, aRef.ChildDefId.Index, aCompoundId);
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::CompSolid
               && aRef.ChildDefId.Index >= 0 && aRef.ChildDefId.Index < theCompSolids.Length())
        appendDirect(myCompoundsOfCompSolid, aRef.ChildDefId.Index, aCompoundId);
    }
  }

  // CompSolid -> Solid reverse index: iterate comp-solid entities and their SolidRefIds.
  preSize(myCompSolidsOfSolid, theSolids.Length(), myAllocator);
  const int aNbCompSolids = theCompSolids.Length();
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompSolids);
       ++aCompSolidId)
  {
    const BRepGraphInc::CompSolidDef& aCS = theCompSolids.Value(aCompSolidId.Index);
    if (aCS.IsRemoved)
      continue;
    for (const BRepGraph_SolidRefId& aSolidRefId : aCS.SolidRefIds)
    {
      const int aRefIdx = aSolidRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theSolidRefs.Length())
        continue;
      const BRepGraphInc::SolidRef& aRef = theSolidRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.SolidDefId.IsValid() || aRef.SolidDefId.Index < 0
          || aRef.SolidDefId.Index >= theSolids.Length())
        continue;
      if (theSolids.Value(aRef.SolidDefId.Index).IsRemoved)
        continue;
      appendDirect(myCompSolidsOfSolid, aRef.SolidDefId.Index, aCompSolidId);
    }
  }

  // CoEdge -> Wires: iterate wire entities and their CoEdgeRefIds for O(1) parent lookup.
  preSize(myCoEdgeToWires, theCoEdges.Length(), myAllocator);
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(aWireId.Index);
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      const int aRefIdx = aCoEdgeRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theCoEdgeRefs.Length())
        continue;
      const BRepGraphInc::CoEdgeRef& aRef = theCoEdgeRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid() || aRef.CoEdgeDefId.Index < 0
          || aRef.CoEdgeDefId.Index >= theCoEdges.Length())
        continue;
      if (theCoEdges.Value(aRef.CoEdgeDefId.Index).IsRemoved)
        continue;
      appendDirect(myCoEdgeToWires, aRef.CoEdgeDefId.Index, aWireId);
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildDelta(
  const NCollection_Vector<BRepGraphInc::EdgeDef>&   theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeDef>& theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireDef>&   theWires,
  const NCollection_Vector<BRepGraphInc::FaceDef>&   theFaces,
  const NCollection_Vector<BRepGraphInc::ShellDef>&  theShells,
  const NCollection_Vector<BRepGraphInc::SolidDef>&  theSolids,
  const NCollection_Vector<BRepGraphInc::ShellRef>&  theShellRefs,
  const NCollection_Vector<BRepGraphInc::FaceRef>&   theFaceRefs,
  const NCollection_Vector<BRepGraphInc::WireRef>&   theWireRefs,
  const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
  const NCollection_Vector<BRepGraphInc::VertexRef>& theVertexRefs,
  const int                                          theOldNbEdges,
  const int                                          theOldNbWires,
  const int                                          theOldNbFaces,
  const int                                          theOldNbShells,
  const int                                          theOldNbSolids)
{
  // Helper: resolve a VertexRefId to the corresponding VertexDefId (BRepGraph_VertexId).
  // Returns an invalid id if the ref id is invalid or out of range.

  // Scan new edges for max vertex index to possibly extend myVertexToEdges.
  int aMaxVertexIdx = myVertexToEdges.Length() - 1;
  const int aNbEdges = theEdges.Length();
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(anEdgeId.Index);
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid() && aStartVtx.Index > aMaxVertexIdx)
      aMaxVertexIdx = aStartVtx.Index;
    if (anEndVtx.IsValid() && anEndVtx.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEndVtx.Index;
  }

  // Extend outer vectors if needed (pre-size for new key ranges).
  ensureSize(myVertexToEdges, aMaxVertexIdx + 1, myAllocator);
  ensureSize(myEdgeToWires, theEdges.Length(), myAllocator);
  ensureSize(myEdgeToFaces, theEdges.Length(), myAllocator);
  ensureSize(myEdgeToCoEdges, theEdges.Length(), myAllocator);
  ensureSize(myWireToFaces, theWires.Length(), myAllocator);
  ensureSize(myFaceToShells, theFaces.Length(), myAllocator);
  ensureSize(myShellToSolids, theShells.Length(), myAllocator);
  ensureSize(myEdgeFaceCount, theEdges.Length());

  // Vertex -> Edges: only new edges.
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(anEdgeId.Index);
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
  const int aNbWires = theWires.Length();
  for (BRepGraph_WireId aWireId(theOldNbWires); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theWires.Value(aWireId.Index);
    if (aWire.IsRemoved)
      continue;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      const int aRefIdx = aCoEdgeRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theCoEdgeRefs.Length())
        continue;
      const BRepGraphInc::CoEdgeRef& aRef = theCoEdgeRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid() || aRef.CoEdgeDefId.Index < 0
          || aRef.CoEdgeDefId.Index >= theCoEdges.Length())
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aRef.CoEdgeDefId.Index);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
        continue;
      appendUnique(myEdgeToWires, aCoEdge.EdgeDefId.Index, aWireId);
    }
  }

  // Edge -> CoEdges: scan only newly appended coedges (they may reference old edges).
  int aOldNbIndexedCoEdges = myNbIndexedCoEdges;
  if (aOldNbIndexedCoEdges < 0 || aOldNbIndexedCoEdges > theCoEdges.Length())
    aOldNbIndexedCoEdges = 0;
  const int aNbCoEdges = theCoEdges.Length();
  for (BRepGraph_CoEdgeId aCoEdgeId(aOldNbIndexedCoEdges); aCoEdgeId.IsValid(aNbCoEdges);
       ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeId.Index);
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendUnique(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, aCoEdgeId);
  }
  myNbIndexedCoEdges = theCoEdges.Length();

  // Edge -> Faces: derive from CoEdge.FaceDefId for new edges.
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    if (theEdges.Value(anEdgeId.Index).IsRemoved)
      continue;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs = seekVec(myEdgeToCoEdges, anEdgeId.Index);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const int aNbCE = aCoEdgeIdxs->Length();

    NCollection_LocalArray<int, 8> aFaces(aNbCE);
    int                            aNbFaces = 0;
    for (int i = 0; i < aNbCE; ++i)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeIdxs->Value(i).Index);
      if (aCoEdge.FaceDefId.IsValid())
        aFaces[aNbFaces++] = aCoEdge.FaceDefId.Index;
    }
    if (aNbFaces == 0)
      continue;

    for (int i = 1; i < aNbFaces; ++i)
    {
      const int aKey = aFaces[i];
      int       j    = i - 1;
      while (j >= 0 && aFaces[j] > aKey)
      {
        aFaces[j + 1] = aFaces[j];
        --j;
      }
      aFaces[j + 1] = aKey;
    }

    int aPrev = -1;
    for (int i = 0; i < aNbFaces; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendDirect(myEdgeToFaces, anEdgeId.Index, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Update cached face counts for new edges.
  for (BRepGraph_EdgeId anEdgeId(theOldNbEdges); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaceVec = seekVec(myEdgeToFaces, anEdgeId.Index);
    myEdgeFaceCount.ChangeValue(anEdgeId.Index) = (aFaceVec != nullptr ? aFaceVec->Length() : 0);
  }

  // Wire -> Faces: iterate only new face entities and their WireRefIds.
  const int aNbFaces = theFaces.Length();
  for (BRepGraph_FaceId aFaceId(theOldNbFaces); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(aFaceId.Index);
    if (aFace.IsRemoved)
      continue;
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      const int aRefIdx = aWireRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theWireRefs.Length())
        continue;
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid() || aRef.WireDefId.Index < 0
          || aRef.WireDefId.Index >= theWires.Length())
        continue;
      if (theWires.Value(aRef.WireDefId.Index).IsRemoved)
        continue;
      appendUnique(myWireToFaces, aRef.WireDefId.Index, aFaceId);
    }
  }

  // Face -> Shells: iterate only new shell entities and their FaceRefIds.
  const int aNbShells = theShells.Length();
  for (BRepGraph_ShellId aShellId(theOldNbShells); aShellId.IsValid(aNbShells); ++aShellId)
  {
    const BRepGraphInc::ShellDef& aShell = theShells.Value(aShellId.Index);
    if (aShell.IsRemoved)
      continue;
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      const int aRefIdx = aFaceRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theFaceRefs.Length())
        continue;
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid() || aRef.FaceDefId.Index < 0
          || aRef.FaceDefId.Index >= theFaces.Length())
        continue;
      if (theFaces.Value(aRef.FaceDefId.Index).IsRemoved)
        continue;
      appendUnique(myFaceToShells, aRef.FaceDefId.Index, aShellId);
    }
  }

  // Shell -> Solids: iterate only new solid entities and their ShellRefIds.
  const int aNbSolids = theSolids.Length();
  for (BRepGraph_SolidId aSolidId(theOldNbSolids); aSolidId.IsValid(aNbSolids); ++aSolidId)
  {
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(aSolidId.Index);
    if (aSolid.IsRemoved)
      continue;
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      const int aRefIdx = aShellRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theShellRefs.Length())
        continue;
      const BRepGraphInc::ShellRef& aRef = theShellRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid() || aRef.ShellDefId.Index < 0
          || aRef.ShellDefId.Index >= theShells.Length())
        continue;
      if (theShells.Value(aRef.ShellDefId.Index).IsRemoved)
        continue;
      appendUnique(myShellToSolids, aRef.ShellDefId.Index, aSolidId);
    }
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
  if (theEdgeId.Index < 0 || theEdgeId.Index >= myEdgeToWires.Length())
    return;
  NCollection_Vector<BRepGraph_WireId>& aWires = myEdgeToWires.ChangeValue(theEdgeId.Index);
  for (int i = 0; i < aWires.Length(); ++i)
  {
    if (aWires.Value(i) == theWireId)
    {
      if (i < aWires.Length() - 1)
        aWires.ChangeValue(i) = aWires.Value(aWires.Length() - 1);
      aWires.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindCoEdgeFromWire(const BRepGraph_CoEdgeId theCoEdgeId,
                                                     const BRepGraph_WireId   theWireId)
{
  if (theCoEdgeId.Index < 0 || theCoEdgeId.Index >= myCoEdgeToWires.Length())
    return;
  NCollection_Vector<BRepGraph_WireId>& aWires = myCoEdgeToWires.ChangeValue(theCoEdgeId.Index);
  for (int i = 0; i < aWires.Length(); ++i)
  {
    if (aWires.Value(i) == theWireId)
    {
      if (i < aWires.Length() - 1)
        aWires.ChangeValue(i) = aWires.Value(aWires.Length() - 1);
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
  if (theVertexId.Index < 0 || theVertexId.Index >= myVertexToEdges.Length())
    return;
  NCollection_Vector<BRepGraph_EdgeId>& anEdges = myVertexToEdges.ChangeValue(theVertexId.Index);
  for (int i = 0; i < anEdges.Length(); ++i)
  {
    if (anEdges.Value(i) == theEdgeId)
    {
      if (i < anEdges.Length() - 1)
        anEdges.ChangeValue(i) = anEdges.Value(anEdges.Length() - 1);
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
  if (theEdgeId.Index < 0 || theEdgeId.Index >= myEdgeToCoEdges.Length())
    return;
  NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = myEdgeToCoEdges.ChangeValue(theEdgeId.Index);
  for (int i = 0; i < aCoEdges.Length(); ++i)
  {
    if (aCoEdges.Value(i) == theCoEdgeId)
    {
      if (i < aCoEdges.Length() - 1)
        aCoEdges.ChangeValue(i) = aCoEdges.Value(aCoEdges.Length() - 1);
      aCoEdges.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToFace(const BRepGraph_EdgeId theEdgeId,
                                               const BRepGraph_FaceId theFaceId)
{
  // Detect new binding by checking vector size before/after appendUnique.
  const int aSizeBefore =
    (theEdgeId.Index < myEdgeToFaces.Length()) ? myEdgeToFaces.Value(theEdgeId.Index).Length() : 0;
  appendUnique(myEdgeToFaces, theEdgeId.Index, theFaceId);
  const int aSizeAfter = myEdgeToFaces.Value(theEdgeId.Index).Length();

  if (aSizeAfter > aSizeBefore)
  {
    // SetValue auto-expands with 0-initialized entries.
    if (theEdgeId.Index >= myEdgeFaceCount.Length())
    {
      myEdgeFaceCount.SetValue(theEdgeId.Index, 1);
    }
    else
    {
      myEdgeFaceCount.ChangeValue(theEdgeId.Index) += 1;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromFace(const BRepGraph_EdgeId theEdgeId,
                                                   const BRepGraph_FaceId theFaceId)
{
  if (theEdgeId.Index < 0 || theEdgeId.Index >= myEdgeToFaces.Length())
    return;
  Standard_ASSERT_VOID(myEdgeFaceCount.Length() == myEdgeToFaces.Length(),
                       "UnbindEdgeFromFace: myEdgeFaceCount out of sync with myEdgeToFaces");
  NCollection_Vector<BRepGraph_FaceId>& aFaces = myEdgeToFaces.ChangeValue(theEdgeId.Index);
  for (int i = 0; i < aFaces.Length(); ++i)
  {
    if (aFaces.Value(i) == theFaceId)
    {
      // Swap-remove: overwrite found entry with the last element, then erase the last.
      // O(1) removal without shifting; adjacency list order is not significant.
      if (i < aFaces.Length() - 1)
        aFaces.ChangeValue(i) = aFaces.Value(aFaces.Length() - 1);
      aFaces.EraseLast();
      Standard_ASSERT_VOID(myEdgeFaceCount.Value(theEdgeId.Index) > 0,
                           "UnbindEdgeFromFace: face count underflow");
      myEdgeFaceCount.ChangeValue(theEdgeId.Index) -= 1;
      break;
    }
  }
}

//=================================================================================================

bool BRepGraphInc_ReverseIndex::Validate(
  const NCollection_Vector<BRepGraphInc::EdgeDef>&   theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeDef>& theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireDef>&   theWires,
  const NCollection_Vector<BRepGraphInc::FaceDef>&   theFaces,
  const NCollection_Vector<BRepGraphInc::ShellDef>&  theShells,
  const NCollection_Vector<BRepGraphInc::SolidDef>&  theSolids,
  const NCollection_Vector<BRepGraphInc::ShellRef>&  theShellRefs,
  const NCollection_Vector<BRepGraphInc::FaceRef>&   theFaceRefs,
  const NCollection_Vector<BRepGraphInc::WireRef>&   theWireRefs,
  const NCollection_Vector<BRepGraphInc::CoEdgeRef>& theCoEdgeRefs,
  const NCollection_Vector<BRepGraphInc::VertexRef>& theVertexRefs) const
{
  auto hasActiveWireUsageOfEdge = [&](const int theWireIdx, const int theEdgeIdx) -> bool {
    if (theWireIdx < 0 || theWireIdx >= theWires.Length() || theEdgeIdx < 0
        || theEdgeIdx >= theEdges.Length())
    {
      return false;
    }
    const BRepGraphInc::WireDef& aWire = theWires.Value(theWireIdx);
    if (aWire.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      const int aRefIdx = aCoEdgeRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theCoEdgeRefs.Length())
      {
        continue;
      }
      const BRepGraphInc::CoEdgeRef& aRef = theCoEdgeRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid())
      {
        continue;
      }
      if (aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire
          || aRef.ParentId.Index != theWireIdx)
      {
        continue;
      }
      const int aCoEdgeIdx = aRef.CoEdgeDefId.Index;
      if (aCoEdgeIdx < 0 || aCoEdgeIdx >= theCoEdges.Length())
      {
        continue;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
      {
        continue;
      }
      if (aCoEdge.EdgeDefId.Index == theEdgeIdx)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveFaceRefForWire = [&](const int theWireIdx, const int theFaceIdx) -> bool {
    if (theWireIdx < 0 || theWireIdx >= theWires.Length() || theFaceIdx < 0
        || theFaceIdx >= theFaces.Length())
    {
      return false;
    }
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(theFaceIdx);
    if (aFace.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
    {
      const int aRefIdx = aWireRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theWireRefs.Length())
      {
        continue;
      }
      const BRepGraphInc::WireRef& aRef = theWireRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.WireDefId.IsValid())
      {
        continue;
      }
      if (aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Face
          || aRef.ParentId.Index != theFaceIdx)
      {
        continue;
      }
      if (aRef.WireDefId.Index == theWireIdx)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveFaceForEdge = [&](const int theEdgeIdx, const int theFaceIdx) -> bool {
    if (theEdgeIdx < 0 || theEdgeIdx >= theEdges.Length() || theFaceIdx < 0
        || theFaceIdx >= theFaces.Length())
    {
      return false;
    }
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(theEdgeIdx);
    if (anEdge.IsRemoved)
    {
      return false;
    }

    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIds = seekVec(myEdgeToCoEdges, theEdgeIdx);
    if (aCoEdgeIds == nullptr)
    {
      return false;
    }

    return hasActiveFaceForEdgeInCoEdges(*aCoEdgeIds, theCoEdges, theEdgeIdx, theFaceIdx);
  };

  auto hasActiveFaceRef = [&](const int theShellIdx, const int theFaceIdx) -> bool {
    if (theShellIdx < 0 || theShellIdx >= theShells.Length() || theFaceIdx < 0
        || theFaceIdx >= theFaces.Length())
    {
      return false;
    }
    const BRepGraphInc::ShellDef& aShell = theShells.Value(theShellIdx);
    if (aShell.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
    {
      const int aRefIdx = aFaceRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theFaceRefs.Length())
      {
        continue;
      }
      const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.FaceDefId.IsValid())
      {
        continue;
      }
      if (aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Shell
          || aRef.ParentId.Index != theShellIdx)
      {
        continue;
      }
      if (aRef.FaceDefId.Index == theFaceIdx)
      {
        return true;
      }
    }
    return false;
  };

  auto hasActiveShellRef = [&](const int theSolidIdx, const int theShellIdx) -> bool {
    if (theSolidIdx < 0 || theSolidIdx >= theSolids.Length() || theShellIdx < 0
        || theShellIdx >= theShells.Length())
    {
      return false;
    }
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(theSolidIdx);
    if (aSolid.IsRemoved)
    {
      return false;
    }
    for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
    {
      const int aRefIdx = aShellRefId.Index;
      if (aRefIdx < 0 || aRefIdx >= theShellRefs.Length())
      {
        continue;
      }
      const BRepGraphInc::ShellRef& aRef = theShellRefs.Value(aRefIdx);
      if (aRef.IsRemoved || !aRef.ShellDefId.IsValid())
      {
        continue;
      }
      if (aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Solid
          || aRef.ParentId.Index != theSolidIdx)
      {
        continue;
      }
      if (aRef.ShellDefId.Index == theShellIdx)
      {
        return true;
      }
    }
    return false;
  };

  // Check: for each coedge ref entry, edge->wire reverse entry must exist.
  const int aNbCoEdgeRefs = theCoEdgeRefs.Length();
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs);
       ++aCoEdgeRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = theCoEdgeRefs.Value(aCoEdgeRefId.Index);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire || !aRef.CoEdgeDefId.IsValid())
      continue;
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= theWires.Length()
        || aRef.CoEdgeDefId.Index < 0 || aRef.CoEdgeDefId.Index >= theCoEdges.Length())
      return false;
    const BRepGraphInc::WireDef& aWire = theWires.Value(aRef.ParentId.Index);
    if (aWire.IsRemoved)
      continue;
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aRef.CoEdgeDefId.Index);
    if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
      return false;
    if (!containsIndexInTable(myEdgeToWires, aCoEdge.EdgeDefId.Index, aRef.ParentId.Index))
      return false;
  }

  // Check: for each edge's start/end vertex, vertex->edge reverse entry must exist.
  const int aNbEdges = theEdges.Length();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theEdges.Value(anEdgeId.Index);
    if (anEdge.IsRemoved)
      continue;
    const BRepGraph_VertexId aStartVtx = resolveVertexDefId(theVertexRefs, anEdge.StartVertexRefId);
    const BRepGraph_VertexId anEndVtx  = resolveVertexDefId(theVertexRefs, anEdge.EndVertexRefId);
    if (aStartVtx.IsValid())
    {
      if (!containsIndexInTable(myVertexToEdges, aStartVtx.Index, anEdgeId.Index))
        return false;
    }
    if (anEndVtx.IsValid() && anEndVtx != aStartVtx)
    {
      if (!containsIndexInTable(myVertexToEdges, anEndVtx.Index, anEdgeId.Index))
        return false;
    }
  }

  // Check: for each edge's CoEdges with valid FaceDefId, edge->face reverse entry must exist.
  const int aNbCoEdges = theCoEdges.Length();
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeId.Index);
    if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      continue;
    if (!containsIndexInTable(myEdgeToFaces, aCoEdge.EdgeDefId.Index, aCoEdge.FaceDefId.Index))
      return false;
  }

  // Check: for each wire ref entry, wire->face reverse entry must exist.
  const int aNbWireRefs = theWireRefs.Length();
  for (BRepGraph_WireRefId aWireRefId(0); aWireRefId.IsValid(aNbWireRefs); ++aWireRefId)
  {
    const BRepGraphInc::WireRef& aRef = theWireRefs.Value(aWireRefId.Index);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Face || !aRef.WireDefId.IsValid())
      continue;
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= theFaces.Length()
        || aRef.WireDefId.Index < 0 || aRef.WireDefId.Index >= theWires.Length())
      return false;
    const BRepGraphInc::FaceDef& aFace = theFaces.Value(aRef.ParentId.Index);
    if (aFace.IsRemoved || theWires.Value(aRef.WireDefId.Index).IsRemoved)
      continue;
    if (!containsIndexInTable(myWireToFaces, aRef.WireDefId.Index, aRef.ParentId.Index))
      return false;
  }

  // Check: for each face ref entry, face->shell reverse entry must exist.
  const int aNbFaceRefs = theFaceRefs.Length();
  for (BRepGraph_FaceRefId aFaceRefId(0); aFaceRefId.IsValid(aNbFaceRefs); ++aFaceRefId)
  {
    const BRepGraphInc::FaceRef& aRef = theFaceRefs.Value(aFaceRefId.Index);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Shell || !aRef.FaceDefId.IsValid())
      continue;
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= theShells.Length()
        || aRef.FaceDefId.Index < 0 || aRef.FaceDefId.Index >= theFaces.Length())
      return false;
    const BRepGraphInc::ShellDef& aShell = theShells.Value(aRef.ParentId.Index);
    if (aShell.IsRemoved || theFaces.Value(aRef.FaceDefId.Index).IsRemoved)
      continue;
    if (!containsIndexInTable(myFaceToShells, aRef.FaceDefId.Index, aRef.ParentId.Index))
      return false;
  }

  // Check: for each shell ref entry, shell->solid reverse entry must exist.
  const int aNbShellRefs = theShellRefs.Length();
  for (BRepGraph_ShellRefId aShellRefId(0); aShellRefId.IsValid(aNbShellRefs); ++aShellRefId)
  {
    const BRepGraphInc::ShellRef& aRef = theShellRefs.Value(aShellRefId.Index);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Solid || !aRef.ShellDefId.IsValid())
      continue;
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= theSolids.Length()
        || aRef.ShellDefId.Index < 0 || aRef.ShellDefId.Index >= theShells.Length())
      return false;
    const BRepGraphInc::SolidDef& aSolid = theSolids.Value(aRef.ParentId.Index);
    if (aSolid.IsRemoved || theShells.Value(aRef.ShellDefId.Index).IsRemoved)
      continue;
    if (!containsIndexInTable(myShellToSolids, aRef.ShellDefId.Index, aRef.ParentId.Index))
      return false;
  }

  // Check reverse tables for stale/extra entries not backed by active forward refs.
  const int aNbEdgeToWires = myEdgeToWires.Length();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToWires); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_WireId>& aWires = WiresOfEdgeRef(anEdgeId);
    for (const BRepGraph_WireId& aWireId2 : aWires)
    {
      const int aWireIdx = aWireId2.Index;
      if (aWireIdx < 0 || aWireIdx >= theWires.Length())
      {
        return false;
      }
      if (!hasActiveWireUsageOfEdge(aWireIdx, anEdgeId.Index))
      {
        return false;
      }
    }
  }

  const int aNbEdgeToCoEdges = myEdgeToCoEdges.Length();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToCoEdges); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = CoEdgesOfEdgeRef(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId2 : aCoEdges)
    {
      const int aCoEdgeIdx = aCoEdgeId2.Index;
      if (aCoEdgeIdx < 0 || aCoEdgeIdx >= theCoEdges.Length())
      {
        return false;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid()
          || aCoEdge.EdgeDefId.Index != anEdgeId.Index)
      {
        return false;
      }
    }
  }

  const int aNbWireToFaces = myWireToFaces.Length();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWireToFaces); ++aWireId)
  {
    const NCollection_Vector<BRepGraph_FaceId>& aFaces = FacesOfWireRef(aWireId);
    for (const BRepGraph_FaceId& aFaceId2 : aFaces)
    {
      const int aFaceIdx = aFaceId2.Index;
      if (aFaceIdx < 0 || aFaceIdx >= theFaces.Length())
      {
        return false;
      }
      if (!hasActiveFaceRefForWire(aWireId.Index, aFaceIdx))
      {
        return false;
      }
    }
  }

  const int aNbEdgeToFaces = myEdgeToFaces.Length();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdgeToFaces); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaces = seekVec(myEdgeToFaces, anEdgeId.Index);
    if (aFaces == nullptr)
    {
      continue;
    }
    for (const BRepGraph_FaceId& aFaceId2 : *aFaces)
    {
      const int aFaceIdx = aFaceId2.Index;
      if (aFaceIdx < 0 || aFaceIdx >= theFaces.Length())
      {
        return false;
      }
      if (!hasActiveFaceForEdge(anEdgeId.Index, aFaceIdx))
      {
        return false;
      }
    }
  }

  const int aNbFaceToShells = myFaceToShells.Length();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaceToShells); ++aFaceId)
  {
    const NCollection_Vector<BRepGraph_ShellId>* aShellsVec = seekVec(myFaceToShells, aFaceId.Index);
    if (aShellsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_ShellId& aShellId2 : *aShellsVec)
    {
      const int aShellIdx = aShellId2.Index;
      if (aShellIdx < 0 || aShellIdx >= theShells.Length())
      {
        return false;
      }
      if (!hasActiveFaceRef(aShellIdx, aFaceId.Index))
      {
        return false;
      }
    }
  }

  const int aNbShellToSolids = myShellToSolids.Length();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShellToSolids); ++aShellId)
  {
    const NCollection_Vector<BRepGraph_SolidId>* aSolidsVec = seekVec(myShellToSolids, aShellId.Index);
    if (aSolidsVec == nullptr)
    {
      continue;
    }
    for (const BRepGraph_SolidId& aSolidId2 : *aSolidsVec)
    {
      const int aSolidIdx = aSolidId2.Index;
      if (aSolidIdx < 0 || aSolidIdx >= theSolids.Length())
      {
        return false;
      }
      if (!hasActiveShellRef(aSolidIdx, aShellId.Index))
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildProductOccurrences(
  const NCollection_Vector<BRepGraphInc::OccurrenceDef>& theOccurrences,
  const int                                              theNbProducts)
{
  myProductToOccurrences.Clear();
  preSize(myProductToOccurrences, theNbProducts, myAllocator);

  const int aNbOccurrences = theOccurrences.Length();
  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(aNbOccurrences);
       ++anOccurrenceId)
  {
    const BRepGraphInc::OccurrenceDef& anOcc = theOccurrences.Value(anOccurrenceId.Index);
    if (anOcc.IsRemoved)
      continue;
    if (anOcc.ProductDefId.IsValid())
      appendDirect(myProductToOccurrences, anOcc.ProductDefId.Index, anOccurrenceId);
  }
}
