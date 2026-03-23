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

//=================================================================================================

void BRepGraphInc_ReverseIndex::Clear()
{
  myEdgeToWires.Clear();
  myEdgeToFaces.Clear();
  myVertexToEdges.Clear();
  myWireToFaces.Clear();
  myFaceToShells.Clear();
  myShellToSolids.Clear();
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::Build(
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids)
{
  Clear();

  // Vertex -> Edges: scan edge entities for start/end vertex indices.
  // Closed edges have StartVertexIdx == EndVertexIdx, so use appendUnique
  // to avoid recording the same edge twice for the same vertex.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx >= 0)
      appendDirect(myVertexToEdges, anEdge.StartVertexIdx, anEdgeIdx);
    if (anEdge.EndVertexIdx >= 0 && anEdge.EndVertexIdx != anEdge.StartVertexIdx) // skip duplicate for closed edges
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
  // Seam edges produce two PCurve entries with the same FaceDefId but opposite
  // orientations; deduplicate by checking the already-built vector for this edge.
  // No extra allocation needed — the output vector itself serves as the seen-set.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const int aFaceIdx = anEdge.PCurves.Value(aPCIdx).FaceDefId.Index;
      // Check if this face is already recorded for this edge.
      const NCollection_Vector<int>* aExisting = myEdgeToFaces.Seek(anEdgeIdx);
      bool aAlreadySeen = false;
      if (aExisting != nullptr)
      {
        for (int j = 0; j < aExisting->Length(); ++j)
        {
          if (aExisting->Value(j) == aFaceIdx)
          {
            aAlreadySeen = true;
            break;
          }
        }
      }
      if (!aAlreadySeen)
        appendDirect(myEdgeToFaces, anEdgeIdx, aFaceIdx);
    }
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

void BRepGraphInc_ReverseIndex::BindEdgeToWire(int theEdgeIdx, int theWireIdx)
{
  appendUnique(myEdgeToWires, theEdgeIdx, theWireIdx);
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::UnbindEdgeFromWire(int theEdgeIdx, int theWireIdx)
{
  NCollection_Vector<int>* aWires = myEdgeToWires.ChangeSeek(theEdgeIdx);
  if (aWires == nullptr)
    return;
  for (int i = 0; i < aWires->Length(); ++i)
  {
    if (aWires->Value(i) == theWireIdx)
    {
      if (i < aWires->Length() - 1)
        aWires->ChangeValue(i) = aWires->Value(aWires->Length() - 1);
      aWires->EraseLast();
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

void BRepGraphInc_ReverseIndex::appendUnique(
  NCollection_DataMap<int, NCollection_Vector<int>>& theMap,
  int theKey, int theVal)
{
  NCollection_Vector<int>* aVec = theMap.ChangeSeek(theKey);
  if (aVec != nullptr)
  {
    for (int i = 0; i < aVec->Length(); ++i)
    {
      if (aVec->Value(i) == theVal)
        return;
    }
    aVec->Append(theVal);
  }
  else
  {
    NCollection_Vector<int> aNewVec;
    aNewVec.Append(theVal);
    theMap.Bind(theKey, aNewVec);
  }
}

//=================================================================================================

void BRepGraphInc_ReverseIndex::appendDirect(
  NCollection_DataMap<int, NCollection_Vector<int>>& theMap,
  int theKey, int theVal)
{
  NCollection_Vector<int>* aVec = theMap.ChangeSeek(theKey);
  if (aVec != nullptr)
  {
    aVec->Append(theVal);
  }
  else
  {
    NCollection_Vector<int> aNewVec;
    aNewVec.Append(theVal);
    theMap.Bind(theKey, aNewVec);
  }
}
