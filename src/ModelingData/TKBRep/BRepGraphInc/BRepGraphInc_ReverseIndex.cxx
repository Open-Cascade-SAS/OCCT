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
#include <BRepGraphInc_Entity.hxx>
#include <NCollection_LocalArray.hxx>

const NCollection_Vector<int> BRepGraphInc_ReverseIndex::THE_EMPTY_VEC;

//=================================================================================================

void BRepGraphInc_ReverseIndex::Clear()
{
  myEdgeToWires.Clear();
  myEdgeToFaces.Clear();
  myVertexToEdges.Clear();
  myWireToFaces.Clear();
  myFaceToShells.Clear();
  myShellToSolids.Clear();
  myEdgeFaceCount.Clear();
  myProductToOccurrences.Clear();
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids)
{
  // Reconstruct outer index tables with allocator if set.
  if (!myAllocator.IsNull())
  {
    myEdgeToWires   = IndexTable(256, myAllocator);
    myEdgeToFaces   = IndexTable(256, myAllocator);
    myVertexToEdges = IndexTable(256, myAllocator);
    myWireToFaces   = IndexTable(256, myAllocator);
    myFaceToShells  = IndexTable(256, myAllocator);
    myShellToSolids = IndexTable(256, myAllocator);
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
    if (anEdge.StartVertexIdx > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.StartVertexIdx;
    if (anEdge.EndVertexIdx > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.EndVertexIdx;
  }

  // Pre-size all outer vectors to their known key range.
  // Pass allocator so inner vectors use IncAllocator for O(1) alloc/free.
  preSize(myVertexToEdges, aMaxVertexIdx + 1, myAllocator);
  preSize(myEdgeToWires,   theEdges.Length(),  myAllocator);
  preSize(myEdgeToFaces,   theEdges.Length(),  myAllocator);
  preSize(myWireToFaces,   theWires.Length(),  myAllocator);
  preSize(myFaceToShells,  theFaces.Length(),  myAllocator);
  preSize(myShellToSolids, theShells.Length(), myAllocator);

  // Vertex -> Edges: scan edge entities for start/end vertex indices.
  // Closed edges have StartVertexIdx == EndVertexIdx, so skip duplicate.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx >= 0)
      appendDirect(myVertexToEdges, anEdge.StartVertexIdx, anEdgeIdx);
    if (anEdge.EndVertexIdx >= 0 && anEdge.EndVertexIdx != anEdge.StartVertexIdx)
      appendDirect(myVertexToEdges, anEdge.EndVertexIdx, anEdgeIdx);
  }

  // Edge -> Wires: scan wire entities for their edge refs.
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
    {
      appendDirect(myEdgeToWires, aWire.EdgeRefs.Value(i).EdgeIdx, aWireIdx);
    }
  }

  // Edge -> Faces: derive from inline PCurve entries on each edge.
  // Seam edges produce two PCurve entries with same FaceDefId but opposite
  // orientations. Collect, sort, and deduplicate per edge to avoid linear scans.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    const int aNbPC = anEdge.PCurves.Length();
    if (aNbPC == 0)
      continue;

    // Collect face indices into local array (stack-allocated for small counts).
    NCollection_LocalArray<int, 8> aFaces(aNbPC);
    for (int i = 0; i < aNbPC; ++i)
      aFaces[i] = anEdge.PCurves.Value(i).FaceDefId.Index;

    // Insertion sort (optimal for small N).
    for (int i = 1; i < aNbPC; ++i)
    {
      const int aKey = aFaces[i];
      int j = i - 1;
      while (j >= 0 && aFaces[j] > aKey)
      {
        aFaces[j + 1] = aFaces[j];
        --j;
      }
      aFaces[j + 1] = aKey;
    }

    // Append unique sorted values.
    int aPrev = -1;
    for (int i = 0; i < aNbPC; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendDirect(myEdgeToFaces, anEdgeIdx, aFaces[i]);
        aPrev = aFaces[i];
      }
    }
  }

  // Populate cached face counts from the edge-to-faces index.
  myEdgeFaceCount.Clear();
  for (int i = 0; i < theEdges.Length(); ++i)
  {
    const NCollection_Vector<int>* aFaceVec = seekVec(myEdgeToFaces, i);
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
      appendDirect(myWireToFaces, aFace.WireRefs.Value(i).WireIdx, aFaceIdx);
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
      appendDirect(myFaceToShells, aShell.FaceRefs.Value(i).FaceIdx, aShellIdx);
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
      appendDirect(myShellToSolids, aSolid.ShellRefs.Value(i).ShellIdx, aSolidIdx);
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildDelta(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids,
  int theOldNbEdges,
  int theOldNbWires,
  int theOldNbFaces,
  int theOldNbShells,
  int theOldNbSolids)
{
  // Scan new edges for max vertex index to possibly extend myVertexToEdges.
  int aMaxVertexIdx = myVertexToEdges.Length() - 1;
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.StartVertexIdx;
    if (anEdge.EndVertexIdx > aMaxVertexIdx)
      aMaxVertexIdx = anEdge.EndVertexIdx;
  }

  // Extend outer vectors if needed (pre-size for new key ranges).
  // Construct new inner vectors with allocator for O(1) alloc/free.
  auto extendTable = [&](IndexTable& theIdx, int theTargetSize) {
    while (theIdx.Length() < theTargetSize)
    {
      if (!myAllocator.IsNull())
      {
        theIdx.Append(NCollection_Vector<int>(16, myAllocator));
      }
      else
      {
        theIdx.Appended();
      }
    }
  };
  extendTable(myVertexToEdges, aMaxVertexIdx + 1);
  extendTable(myEdgeToWires,   theEdges.Length());
  extendTable(myEdgeToFaces,   theEdges.Length());
  extendTable(myWireToFaces,   theWires.Length());
  extendTable(myFaceToShells,  theFaces.Length());
  extendTable(myShellToSolids, theShells.Length());

  // Vertex -> Edges: only new edges.
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx >= 0)
      appendUnique(myVertexToEdges, anEdge.StartVertexIdx, anEdgeIdx);
    if (anEdge.EndVertexIdx >= 0 && anEdge.EndVertexIdx != anEdge.StartVertexIdx)
      appendUnique(myVertexToEdges, anEdge.EndVertexIdx, anEdgeIdx);
  }

  // Edge -> Wires: only new wires (new wires may reference old edges too).
  for (int aWireIdx = theOldNbWires; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
    {
      appendUnique(myEdgeToWires, aWire.EdgeRefs.Value(i).EdgeIdx, aWireIdx);
    }
  }

  // Edge -> Faces: only new edges (derive from their PCurves).
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    const int aNbPC = anEdge.PCurves.Length();
    if (aNbPC == 0)
      continue;

    NCollection_LocalArray<int, 8> aFaces(aNbPC);
    for (int i = 0; i < aNbPC; ++i)
      aFaces[i] = anEdge.PCurves.Value(i).FaceDefId.Index;

    for (int i = 1; i < aNbPC; ++i)
    {
      const int aKey = aFaces[i];
      int j = i - 1;
      while (j >= 0 && aFaces[j] > aKey)
      {
        aFaces[j + 1] = aFaces[j];
        --j;
      }
      aFaces[j + 1] = aKey;
    }

    int aPrev = -1;
    for (int i = 0; i < aNbPC; ++i)
    {
      if (aFaces[i] != aPrev)
      {
        appendUnique(myEdgeToFaces, anEdgeIdx, aFaces[i]);
        aPrev = aFaces[i];
      }
    }
  }

  // Update cached face counts for new edges (SetValue auto-expands with 0).
  for (int anEdgeIdx = theOldNbEdges; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<int>* aFaceVec = seekVec(myEdgeToFaces, anEdgeIdx);
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
      appendUnique(myWireToFaces, aFace.WireRefs.Value(i).WireIdx, aFaceIdx);
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
      appendUnique(myFaceToShells, aShell.FaceRefs.Value(i).FaceIdx, aShellIdx);
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
      appendUnique(myShellToSolids, aSolid.ShellRefs.Value(i).ShellIdx, aSolidIdx);
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToWire(int theEdgeIdx, int theWireIdx)
{
  appendUnique(myEdgeToWires, theEdgeIdx, theWireIdx);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromWire(int theEdgeIdx, int theWireIdx)
{
  if (theEdgeIdx < 0 || theEdgeIdx >= myEdgeToWires.Length())
    return;
  NCollection_Vector<int>& aWires = myEdgeToWires.ChangeValue(theEdgeIdx);
  for (int i = 0; i < aWires.Length(); ++i)
  {
    if (aWires.Value(i) == theWireIdx)
    {
      if (i < aWires.Length() - 1)
        aWires.ChangeValue(i) = aWires.Value(aWires.Length() - 1);
      aWires.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::ReplaceEdgeInWireMap(int theOldEdgeIdx,
                                                     int theNewEdgeIdx,
                                                     int theWireIdx)
{
  UnbindEdgeFromWire(theOldEdgeIdx, theWireIdx);
  BindEdgeToWire(theNewEdgeIdx, theWireIdx);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::preSize(IndexTable&                               theIdx,
                                        int                                       theSize,
                                        const Handle(NCollection_BaseAllocator)& theAlloc)
{
  theIdx.Clear();
  for (int i = 0; i < theSize; ++i)
  {
    if (!theAlloc.IsNull())
    {
      theIdx.Append(NCollection_Vector<int>(16, theAlloc));
    }
    else
    {
      theIdx.Appended();
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::appendUnique(IndexTable& theIdx, int theKey, int theVal)
{
  Standard_ASSERT_RETURN(theKey >= 0, "appendUnique: negative key", );
  // Grow if needed for incremental mutation after Build().
  if (theKey >= theIdx.Length())
    theIdx.SetValue(theKey, NCollection_Vector<int>());

  NCollection_Vector<int>& aVec = theIdx.ChangeValue(theKey);
  for (int i = 0; i < aVec.Length(); ++i)
  {
    if (aVec.Value(i) == theVal)
      return;
  }
  aVec.Append(theVal);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::appendDirect(IndexTable& theIdx, int theKey, int theVal)
{
  Standard_ASSERT_RETURN(theKey >= 0, "appendDirect: negative key", );
  // During Build(), outer vector is pre-sized so theKey < Length().
  // For safety, grow if somehow out of range.
  if (theKey >= theIdx.Length())
    theIdx.SetValue(theKey, NCollection_Vector<int>());

  theIdx.ChangeValue(theKey).Append(theVal);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindVertexToEdge(int theVertexIdx, int theEdgeIdx)
{
  appendUnique(myVertexToEdges, theVertexIdx, theEdgeIdx);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindVertexFromEdge(int theVertexIdx, int theEdgeIdx)
{
  if (theVertexIdx < 0 || theVertexIdx >= myVertexToEdges.Length())
    return;
  NCollection_Vector<int>& anEdges = myVertexToEdges.ChangeValue(theVertexIdx);
  for (int i = 0; i < anEdges.Length(); ++i)
  {
    if (anEdges.Value(i) == theEdgeIdx)
    {
      if (i < anEdges.Length() - 1)
        anEdges.ChangeValue(i) = anEdges.Value(anEdges.Length() - 1);
      anEdges.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BindEdgeToFace(int theEdgeIdx, int theFaceIdx)
{
  // Detect new binding by checking vector size before/after appendUnique.
  const int aSizeBefore = (theEdgeIdx < myEdgeToFaces.Length())
                            ? myEdgeToFaces.Value(theEdgeIdx).Length()
                            : 0;
  appendUnique(myEdgeToFaces, theEdgeIdx, theFaceIdx);
  const int aSizeAfter = myEdgeToFaces.Value(theEdgeIdx).Length();

  if (aSizeAfter > aSizeBefore)
  {
    // SetValue auto-expands with 0-initialized entries.
    if (theEdgeIdx >= myEdgeFaceCount.Length())
    {
      myEdgeFaceCount.SetValue(theEdgeIdx, 1);
    }
    else
    {
      myEdgeFaceCount.ChangeValue(theEdgeIdx) += 1;
    }
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromFace(int theEdgeIdx, int theFaceIdx)
{
  if (theEdgeIdx < 0 || theEdgeIdx >= myEdgeToFaces.Length())
    return;
  Standard_ASSERT_VOID(myEdgeFaceCount.Length() == myEdgeToFaces.Length(),
                        "UnbindEdgeFromFace: myEdgeFaceCount out of sync with myEdgeToFaces");
  NCollection_Vector<int>& aFaces = myEdgeToFaces.ChangeValue(theEdgeIdx);
  for (int i = 0; i < aFaces.Length(); ++i)
  {
    if (aFaces.Value(i) == theFaceIdx)
    {
      // Swap-remove: overwrite found entry with the last element, then erase the last.
      // O(1) removal without shifting; adjacency list order is not significant.
      if (i < aFaces.Length() - 1)
        aFaces.ChangeValue(i) = aFaces.Value(aFaces.Length() - 1);
      aFaces.EraseLast();
      Standard_ASSERT_VOID(myEdgeFaceCount.Value(theEdgeIdx) > 0,
                            "UnbindEdgeFromFace: face count underflow");
      myEdgeFaceCount.ChangeValue(theEdgeIdx) -= 1;
      break;
    }
  }
}

//=================================================================================================

bool BRepGraphInc_ReverseIndex::Validate(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids) const
{
  // Helper: check that theVal appears in the vector at theKey in theIdx.
  auto containsVal = [](const IndexTable& theIdx, int theKey, int theVal) -> bool
  {
    const NCollection_Vector<int>* aVec = seekVec(theIdx, theKey);
    if (aVec == nullptr)
      return false;
    for (int i = 0; i < aVec->Length(); ++i)
    {
      if (aVec->Value(i) == theVal)
        return true;
    }
    return false;
  };

  // Check: for each wire's EdgeRefs, edge->wire reverse entry must exist.
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
    {
      const int anEdgeIdx = aWire.EdgeRefs.Value(i).EdgeIdx;
      if (!containsVal(myEdgeToWires, anEdgeIdx, aWireIdx))
        return false;
    }
  }

  // Check: for each edge's start/end vertex, vertex->edge reverse entry must exist.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx >= 0)
    {
      if (!containsVal(myVertexToEdges, anEdge.StartVertexIdx, anEdgeIdx))
        return false;
    }
    if (anEdge.EndVertexIdx >= 0 && anEdge.EndVertexIdx != anEdge.StartVertexIdx)
    {
      if (!containsVal(myVertexToEdges, anEdge.EndVertexIdx, anEdgeIdx))
        return false;
    }
  }

  // Check: for each edge's PCurves, edge->face reverse entry must exist.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const int aFaceIdx = anEdge.PCurves.Value(aPCIdx).FaceDefId.Index;
      if (!containsVal(myEdgeToFaces, anEdgeIdx, aFaceIdx))
        return false;
    }
  }

  // Check: for each face's WireRefs, wire->face reverse entry must exist.
  for (int aFaceIdx = 0; aFaceIdx < theFaces.Length(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceEntity& aFace = theFaces.Value(aFaceIdx);
    if (aFace.IsRemoved)
      continue;
    for (int i = 0; i < aFace.WireRefs.Length(); ++i)
    {
      const int aWireIdx = aFace.WireRefs.Value(i).WireIdx;
      if (!containsVal(myWireToFaces, aWireIdx, aFaceIdx))
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
      const int aFaceIdx = aShell.FaceRefs.Value(i).FaceIdx;
      if (!containsVal(myFaceToShells, aFaceIdx, aShellIdx))
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
      const int aShellIdx = aSolid.ShellRefs.Value(i).ShellIdx;
      if (!containsVal(myShellToSolids, aShellIdx, aSolidIdx))
        return false;
    }
  }

  return true;
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::BuildProductOccurrences(
  const NCollection_Vector<BRepGraphInc::OccurrenceEntity>& theOccurrences,
  int theNbProducts)
{
  myProductToOccurrences.Clear();
  preSize(myProductToOccurrences, theNbProducts, myAllocator);

  for (int anOccIdx = 0; anOccIdx < theOccurrences.Length(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceEntity& anOcc = theOccurrences.Value(anOccIdx);
    if (anOcc.IsRemoved)
      continue;
    if (anOcc.ProductIdx >= 0)
      appendDirect(myProductToOccurrences, anOcc.ProductIdx, anOccIdx);
  }
}
