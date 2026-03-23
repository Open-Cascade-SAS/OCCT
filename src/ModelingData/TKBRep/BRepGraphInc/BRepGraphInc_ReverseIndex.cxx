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
#include <BRepGraphInc_RelGeom.hxx>

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
  const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
  const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
  const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
  const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
  const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids,
  const NCollection_Vector<BRepGraphInc::EdgeFaceGeom>& theEdgeFaceGeoms)
{
  Clear();

  // Vertex -> Edges: scan edge entities for start/end vertex indices.
  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = theEdges.Value(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;
    if (anEdge.StartVertexIdx >= 0)
      appendUnique(myVertexToEdges, anEdge.StartVertexIdx, anEdgeIdx);
    if (anEdge.EndVertexIdx >= 0)
      appendUnique(myVertexToEdges, anEdge.EndVertexIdx, anEdgeIdx);
  }

  // Edge -> Wires: scan wire entities for their edge refs.
  for (int aWireIdx = 0; aWireIdx < theWires.Length(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theWires.Value(aWireIdx);
    if (aWire.IsRemoved)
      continue;
    for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
    {
      appendUnique(myEdgeToWires, aWire.EdgeRefs.Value(i).EdgeIdx, aWireIdx);
    }
  }

  // Edge -> Faces: scan EdgeFaceGeom rows.
  for (int i = 0; i < theEdgeFaceGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceGeom& aRow = theEdgeFaceGeoms.Value(i);
    appendUnique(myEdgeToFaces, aRow.EdgeIdx, aRow.FaceIdx);
  }

  // Wire -> Faces: scan face entities for their wire refs.
  for (int aFaceIdx = 0; aFaceIdx < theFaces.Length(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceEntity& aFace = theFaces.Value(aFaceIdx);
    if (aFace.IsRemoved)
      continue;
    for (int i = 0; i < aFace.WireRefs.Length(); ++i)
    {
      appendUnique(myWireToFaces, aFace.WireRefs.Value(i).WireIdx, aFaceIdx);
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
      appendUnique(myFaceToShells, aShell.FaceRefs.Value(i).FaceIdx, aShellIdx);
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
      appendUnique(myShellToSolids, aSolid.ShellRefs.Value(i).ShellIdx, aSolidIdx);
    }
  }
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
