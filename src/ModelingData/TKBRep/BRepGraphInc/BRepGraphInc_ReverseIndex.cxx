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
#include <BRepGraphInc_Entity.hxx>
#include <NCollection_LocalArray.hxx>

#include <type_traits>

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
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&      theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeEntity>&    theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&      theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&      theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>&     theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>&     theSolids,
  const NCollection_Vector<BRepGraphInc::CompoundEntity>&  theCompounds,
  const NCollection_Vector<BRepGraphInc::CompSolidEntity>& theCompSolids)
{
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

  // Scan edges for max vertex index to pre-size myVertexToEdges.
  int aMaxVertexIdx = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertex.VertexDefId.IsValid()
        && anEdge.StartVertex.VertexDefId.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.StartVertex.VertexDefId.Index;
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && anEdge.EndVertex.VertexDefId.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.EndVertex.VertexDefId.Index;
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
  // Closed edges have StartVertex.VertexDefId == EndVertex.VertexDefId, so skip duplicate.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertex.VertexDefId.IsValid())
      appendDirect(myVertexToEdges,
                   anEdge.StartVertex.VertexDefId.Index,
                   BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && anEdge.EndVertex.VertexDefId != anEdge.StartVertex.VertexDefId)
      appendDirect(myVertexToEdges,
                   anEdge.EndVertex.VertexDefId.Index,
                   BRepGraph_EdgeId(anEdgeIdx));
  }

  // Edge -> Wires: scan wire entities for their coedge refs, resolve edge index.
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
    {
      const int aCoEdgeIdx = aWire.CoEdgeRefs.Value(i).CoEdgeDefId.Index;
      if (aCoEdgeIdx >= 0 && aCoEdgeIdx < theCoEdges.Length())
        appendDirect(myEdgeToWires,
                     theCoEdges.Value(aCoEdgeIdx).EdgeDefId.Index,
                     BRepGraph_WireId(aWireIdx));
    }
  }

  // Edge -> CoEdges: derive from CoEdge.EdgeDefId field.
  preSize(myEdgeToCoEdges, theEdges.Length(), myAllocator);
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < theCoEdges.Length(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendDirect(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, BRepGraph_CoEdgeId(aCoEdgeIdx));
  }

  // Edge -> Faces: derive from CoEdge.FaceDefId (replaces legacy PCurve-based derivation).
  // Seam edges have two CoEdges with same FaceDefId but opposite Sense.
  // Deduplicate per edge using the edge->coedges index built above.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    if (theEdges.Value(anEdgeIdx).IsRemoved)
      continue;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs = seekVec(myEdgeToCoEdges, anEdgeIdx);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const int aNbCE = aCoEdgeIdxs->Length();

    // Collect face indices from coedges (stack-allocated for small counts).
    NCollection_LocalArray<int, 8> aFaces(aNbCE);
    int                            aNbFaces = 0;
    for (int i = 0; i < aNbCE; ++i)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge = theCoEdges.Value(aCoEdgeIdxs->Value(i).Index);
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
        appendDirect(myEdgeToFaces, anEdgeIdx, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Populate cached face counts from the edge-to-faces index.
  myEdgeFaceCount.Clear();
  for (int i = 0; i < theEdges.Length(); ++i)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaceVec = seekVec(myEdgeToFaces, i);
    myEdgeFaceCount.Append(aFaceVec != nullptr ? aFaceVec->Length() : 0);
  }

  // Wire -> Faces: scan face entities for their wire refs.
  for (int aFaceIdx = 0; aFaceIdx < theFaces.Length(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceEntity& aFace = theFaces.Value(aFaceIdx);
    if (aFace.IsRemoved)
      continue;
    for (int i = 0; i < aFace.WireRefs.Length(); ++i)
    {
      appendDirect(myWireToFaces,
                   aFace.WireRefs.Value(i).WireDefId.Index,
                   BRepGraph_FaceId(aFaceIdx));
    }
  }

  // Face -> Shells: scan shell entities for their face refs.
  for (int aShellIdx = 0; aShellIdx < theShells.Length(); ++aShellIdx)
  {
    const BRepGraphInc::ShellEntity& aShell = theShells.Value(aShellIdx);
    if (aShell.IsRemoved)
      continue;
    for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
    {
      appendDirect(myFaceToShells,
                   aShell.FaceRefs.Value(i).FaceDefId.Index,
                   BRepGraph_ShellId(aShellIdx));
    }
  }

  // Shell -> Solids: scan solid entities for their shell refs.
  for (int aSolidIdx = 0; aSolidIdx < theSolids.Length(); ++aSolidIdx)
  {
    const BRepGraphInc::SolidEntity& aSolid = theSolids.Value(aSolidIdx);
    if (aSolid.IsRemoved)
      continue;
    for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
    {
      appendDirect(myShellToSolids,
                   aSolid.ShellRefs.Value(i).ShellDefId.Index,
                   BRepGraph_SolidId(aSolidIdx));
    }
  }

  // Compound -> child reverse indices: scan compound ChildRefs by kind.
  preSize(myCompoundsOfSolid, theSolids.Length(), myAllocator);
  preSize(myCompoundsOfShell, theShells.Length(), myAllocator);
  preSize(myCompoundsOfFace, theFaces.Length(), myAllocator);
  preSize(myCompoundsOfCompound, theCompounds.Length(), myAllocator);
  preSize(myCompoundsOfCompSolid, theCompSolids.Length(), myAllocator);
  for (int aCompIdx = 0; aCompIdx < theCompounds.Length(); ++aCompIdx)
  {
    const BRepGraphInc::CompoundEntity& aComp = theCompounds.Value(aCompIdx);
    if (aComp.IsRemoved)
      continue;
    for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
    {
      const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(i);
      if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid)
        appendDirect(myCompoundsOfSolid, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompIdx));
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Shell)
        appendDirect(myCompoundsOfShell, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompIdx));
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Face)
        appendDirect(myCompoundsOfFace, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompIdx));
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Compound)
        appendDirect(myCompoundsOfCompound, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompIdx));
      else if (aRef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::CompSolid)
        appendDirect(myCompoundsOfCompSolid, aRef.ChildDefId.Index, BRepGraph_CompoundId(aCompIdx));
    }
  }

  // CompSolid -> Solid reverse index: scan compsolid SolidRefs.
  preSize(myCompSolidsOfSolid, theSolids.Length(), myAllocator);
  for (int aCSIdx = 0; aCSIdx < theCompSolids.Length(); ++aCSIdx)
  {
    const BRepGraphInc::CompSolidEntity& aCS = theCompSolids.Value(aCSIdx);
    if (aCS.IsRemoved)
      continue;
    for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
      appendDirect(myCompSolidsOfSolid,
                   aCS.SolidRefs.Value(i).SolidDefId.Index,
                   BRepGraph_CompSolidId(aCSIdx));
  }

  // CoEdge -> Wires: scan wire coedge refs.
  preSize(myCoEdgeToWires, theCoEdges.Length(), myAllocator);
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
      appendDirect(myCoEdgeToWires,
                   aWire.CoEdgeRefs.Value(i).CoEdgeDefId.Index,
                   BRepGraph_WireId(aWireIdx));
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildDelta(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeEntity>& theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids,
  const int                                             theOldNbEdges,
  const int                                             theOldNbWires,
  const int                                             theOldNbFaces,
  const int                                             theOldNbShells,
  const int                                             theOldNbSolids)
{
  // Scan new edges for max vertex index to possibly extend myVertexToEdges.
  int aMaxVertexIdx = myVertexToEdges.Length() - 1;
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertex.VertexDefId.IsValid()
        && anEdge.StartVertex.VertexDefId.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.StartVertex.VertexDefId.Index;
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && anEdge.EndVertex.VertexDefId.Index > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.EndVertex.VertexDefId.Index;
  }

  // Extend outer vectors if needed (pre-size for new key ranges).
  // Construct new inner vectors with allocator for O(1) alloc/free.
  auto extendTable = [&](auto& theIdx, const int theTargetSize) {
    using TableType  = std::remove_reference_t<decltype(theIdx)>;
    using VectorType = typename TableType::value_type;
    while (theIdx.Length() < theTargetSize)
    {
      if (!myAllocator.IsNull())
        theIdx.Append(VectorType(16, myAllocator));
      else
        theIdx.Appended();
    }
  };

  extendTable(myVertexToEdges, aMaxVertexIdx + 1);
  extendTable(myEdgeToWires, theEdges.Length());
  extendTable(myEdgeToFaces, theEdges.Length());
  extendTable(myEdgeToCoEdges, theEdges.Length());
  extendTable(myWireToFaces, theWires.Length());
  extendTable(myFaceToShells, theFaces.Length());
  extendTable(myShellToSolids, theShells.Length());

  // Vertex -> Edges: only new edges.
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertex.VertexDefId.IsValid())
      appendUnique(myVertexToEdges,
                   anEdge.StartVertex.VertexDefId.Index,
                   BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && anEdge.EndVertex.VertexDefId != anEdge.StartVertex.VertexDefId)
      appendUnique(myVertexToEdges,
                   anEdge.EndVertex.VertexDefId.Index,
                   BRepGraph_EdgeId(anEdgeIdx));
  }

  // Edge -> Wires: only new wires (new wires may reference old edges too).
  for (int aWireIdx = theOldNbWires; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
    {
      const int aCoEdgeIdx = aWire.CoEdgeRefs.Value(i).CoEdgeDefId.Index;
      if (aCoEdgeIdx >= 0 && aCoEdgeIdx < theCoEdges.Length())
        appendUnique(myEdgeToWires,
                     theCoEdges.Value(aCoEdgeIdx).EdgeDefId.Index,
                     BRepGraph_WireId(aWireIdx));
    }
  }

  // Edge -> CoEdges: scan ALL coedges (new coedges may reference old edges).
  // We appendUnique to handle incremental additions correctly.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < theCoEdges.Length(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
    if (aCoEdge.IsRemoved)
      continue;
    if (aCoEdge.EdgeDefId.IsValid())
      appendUnique(myEdgeToCoEdges, aCoEdge.EdgeDefId.Index, BRepGraph_CoEdgeId(aCoEdgeIdx));
  }

  // Edge -> Faces: derive from CoEdge.FaceDefId for new edges.
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    if (theEdges.Value(anEdgeIdx).IsRemoved)
      continue;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs = seekVec(myEdgeToCoEdges, anEdgeIdx);
    if (aCoEdgeIdxs == nullptr)
      continue;
    const int aNbCE = aCoEdgeIdxs->Length();

    NCollection_LocalArray<int, 8> aFaces(aNbCE);
    int                            aNbFaces = 0;
    for (int i = 0; i < aNbCE; ++i)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge = theCoEdges.Value(aCoEdgeIdxs->Value(i).Index);
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
        appendUnique(myEdgeToFaces, anEdgeIdx, BRepGraph_FaceId(aFaces[i]));
        aPrev = aFaces[i];
      }
    }
  }

  // Update cached face counts for new edges (SetValue auto-expands with 0).
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaceVec = seekVec(myEdgeToFaces, anEdgeIdx);
    myEdgeFaceCount.SetValue(anEdgeIdx, aFaceVec != nullptr ? aFaceVec->Length() : 0);
  }

  // Wire -> Faces: only new faces (new faces may reference old wires too).
  for (int aFaceIdx = theOldNbFaces; aFaceIdx < theFaces.Length(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceEntity& aFace = theFaces.Value(aFaceIdx);
    if (aFace.IsRemoved)
      continue;
    for (int i = 0; i < aFace.WireRefs.Length(); ++i)
    {
      appendUnique(myWireToFaces,
                   aFace.WireRefs.Value(i).WireDefId.Index,
                   BRepGraph_FaceId(aFaceIdx));
    }
  }

  // Face -> Shells: only new shells.
  for (int aShellIdx = theOldNbShells; aShellIdx < theShells.Length(); ++aShellIdx)
  {
    const BRepGraphInc::ShellEntity& aShell = theShells.Value(aShellIdx);
    if (aShell.IsRemoved)
      continue;
    for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
    {
      appendUnique(myFaceToShells,
                   aShell.FaceRefs.Value(i).FaceDefId.Index,
                   BRepGraph_ShellId(aShellIdx));
    }
  }

  // Shell -> Solids: only new solids.
  for (int aSolidIdx = theOldNbSolids; aSolidIdx < theSolids.Length(); ++aSolidIdx)
  {
    const BRepGraphInc::SolidEntity& aSolid = theSolids.Value(aSolidIdx);
    if (aSolid.IsRemoved)
      continue;
    for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
    {
      appendUnique(myShellToSolids,
                   aSolid.ShellRefs.Value(i).ShellDefId.Index,
                   BRepGraph_SolidId(aSolidIdx));
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
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
  const NCollection_Vector<BRepGraphInc::CoEdgeEntity>& theCoEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids) const
{
  // Helper: check that theVal appears in the vector at theKey in theIdx.
  auto contains = [](const auto& theIdx, const int theKey, const int theVal) -> bool {
    const auto* aVec = seekVec(theIdx, theKey);
    if (aVec == nullptr)
      return false;
    for (int i = 0; i < aVec->Length(); ++i)
    {
      if (aVec->Value(i).Index == theVal)
        return true;
    }
    return false;
  };

  // Check: for each wire's CoEdgeRefs, edge->wire reverse entry must exist.
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
    {
      const int aCoEdgeIdx = aWire.CoEdgeRefs.Value(i).CoEdgeDefId.Index;
      if (aCoEdgeIdx < 0 || aCoEdgeIdx >= theCoEdges.Length())
        return false;
      const int anEdgeIdx = theCoEdges.Value(aCoEdgeIdx).EdgeDefId.Index;
      if (!contains(myEdgeToWires, anEdgeIdx, aWireIdx))
        return false;
    }
  }

  // Check: for each edge's start/end vertex, vertex->edge reverse entry must exist.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertex.VertexDefId.IsValid())
    {
      if (!contains(myVertexToEdges, anEdge.StartVertex.VertexDefId.Index, anEdgeIdx))
        return false;
    }
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && anEdge.EndVertex.VertexDefId != anEdge.StartVertex.VertexDefId)
    {
      if (!contains(myVertexToEdges, anEdge.EndVertex.VertexDefId.Index, anEdgeIdx))
        return false;
    }
  }

  // Check: for each edge's CoEdges with valid FaceDefId, edge->face reverse entry must exist.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < theCoEdges.Length(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = theCoEdges.Value(aCoEdgeIdx);
    if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      continue;
    if (!contains(myEdgeToFaces, aCoEdge.EdgeDefId.Index, aCoEdge.FaceDefId.Index))
      return false;
  }

  // Check: for each face's WireRefs, wire->face reverse entry must exist.
  for (int aFaceIdx = 0; aFaceIdx < theFaces.Length(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceEntity& aFace = theFaces.Value(aFaceIdx);
    if (aFace.IsRemoved)
      continue;
    for (int i = 0; i < aFace.WireRefs.Length(); ++i)
    {
      const int aWireIdx = aFace.WireRefs.Value(i).WireDefId.Index;
      if (!contains(myWireToFaces, aWireIdx, aFaceIdx))
        return false;
    }
  }

  // Check: for each shell's FaceRefs, face->shell reverse entry must exist.
  for (int aShellIdx = 0; aShellIdx < theShells.Length(); ++aShellIdx)
  {
    const BRepGraphInc::ShellEntity& aShell = theShells.Value(aShellIdx);
    if (aShell.IsRemoved)
      continue;
    for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
    {
      const int aFaceIdx = aShell.FaceRefs.Value(i).FaceDefId.Index;
      if (!contains(myFaceToShells, aFaceIdx, aShellIdx))
        return false;
    }
  }

  // Check: for each solid's ShellRefs, shell->solid reverse entry must exist.
  for (int aSolidIdx = 0; aSolidIdx < theSolids.Length(); ++aSolidIdx)
  {
    const BRepGraphInc::SolidEntity& aSolid = theSolids.Value(aSolidIdx);
    if (aSolid.IsRemoved)
      continue;
    for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
    {
      const int aShellIdx = aSolid.ShellRefs.Value(i).ShellDefId.Index;
      if (!contains(myShellToSolids, aShellIdx, aSolidIdx))
        return false;
    }
  }

  return true;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildProductOccurrences(
  const NCollection_Vector<BRepGraphInc::OccurrenceEntity>& theOccurrences,
  const int                                                 theNbProducts)
{
  myProductToOccurrences.Clear();
  preSize(myProductToOccurrences, theNbProducts, myAllocator);

  for (int anOccIdx = 0; anOccIdx < theOccurrences.Length(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceEntity& anOcc = theOccurrences.Value(anOccIdx);
    if (anOcc.IsRemoved)
      continue;
    if (anOcc.ProductDefId.IsValid())
      appendDirect(myProductToOccurrences,
                   anOcc.ProductDefId.Index,
                   BRepGraph_OccurrenceId(anOccIdx));
  }
}
