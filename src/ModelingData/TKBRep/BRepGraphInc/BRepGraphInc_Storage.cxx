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

#include <BRepGraphInc_Storage.hxx>

//=================================================================================================

const NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::UIDs(
  BRepGraph_NodeId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:    return myVertexUIDs;
    case BRepGraph_NodeId::Kind::Edge:      return myEdgeUIDs;
    case BRepGraph_NodeId::Kind::Wire:      return myWireUIDs;
    case BRepGraph_NodeId::Kind::Face:      return myFaceUIDs;
    case BRepGraph_NodeId::Kind::Shell:     return myShellUIDs;
    case BRepGraph_NodeId::Kind::Solid:     return mySolidUIDs;
    case BRepGraph_NodeId::Kind::Compound:  return myCompoundUIDs;
    case BRepGraph_NodeId::Kind::CompSolid: return myCompSolidUIDs;
  }
  static const NCollection_Vector<BRepGraph_UID> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================

NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::ChangeUIDs(
  BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:    return myVertexUIDs;
    case BRepGraph_NodeId::Kind::Edge:      return myEdgeUIDs;
    case BRepGraph_NodeId::Kind::Wire:      return myWireUIDs;
    case BRepGraph_NodeId::Kind::Face:      return myFaceUIDs;
    case BRepGraph_NodeId::Kind::Shell:     return myShellUIDs;
    case BRepGraph_NodeId::Kind::Solid:     return mySolidUIDs;
    case BRepGraph_NodeId::Kind::Compound:  return myCompoundUIDs;
    case BRepGraph_NodeId::Kind::CompSolid: return myCompSolidUIDs;
  }
  // Unreachable for valid Kind values; return Vertex as fallback.
  return myVertexUIDs;
}

//=================================================================================================

void BRepGraphInc_Storage::ResetAllUIDs()
{
  myVertexUIDs.Clear();
  myEdgeUIDs.Clear();
  myWireUIDs.Clear();
  myFaceUIDs.Clear();
  myShellUIDs.Clear();
  mySolidUIDs.Clear();
  myCompoundUIDs.Clear();
  myCompSolidUIDs.Clear();
}

//=================================================================================================

void BRepGraphInc_Storage::Clear()
{
  myVertices.Clear();
  myEdges.Clear();
  myWires.Clear();
  myFaces.Clear();
  myShells.Clear();
  mySolids.Clear();
  myCompounds.Clear();
  myCompSolids.Clear();
  myReverseIdx.Clear();
  myTShapeToNodeId.Clear();
  myOriginalShapes.Clear();
  ResetAllUIDs();
  myIsDone = false;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildReverseIndex()
{
  myReverseIdx.Build(myEdges, myWires, myFaces, myShells, mySolids);
}
