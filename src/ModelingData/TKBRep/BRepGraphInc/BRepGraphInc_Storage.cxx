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

BRepGraphInc_Storage::BRepGraphInc_Storage(
  const Handle(NCollection_BaseAllocator)& theAlloc)
  : myVertices(256, theAlloc),
    myEdges(256, theAlloc),
    myWires(256, theAlloc),
    myFaces(256, theAlloc),
    myShells(256, theAlloc),
    mySolids(256, theAlloc),
    myCompounds(256, theAlloc),
    myCompSolids(256, theAlloc),
    myTShapeToNodeId(1, theAlloc),
    myOriginalShapes(1, theAlloc),
    myVertexUIDs(256, theAlloc),
    myEdgeUIDs(256, theAlloc),
    myWireUIDs(256, theAlloc),
    myFaceUIDs(256, theAlloc),
    myShellUIDs(256, theAlloc),
    mySolidUIDs(256, theAlloc),
    myCompoundUIDs(256, theAlloc),
    myCompSolidUIDs(256, theAlloc),
    myAllocator(theAlloc.IsNull()
      ? NCollection_BaseAllocator::CommonBaseAllocator() : theAlloc)
{
}

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
  myIsDone             = false;
  myHasRegularities    = false;
  myHasVertexPointReps = false;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildReverseIndex()
{
  myReverseIdx.SetAllocator(myAllocator);
  myReverseIdx.Build(myEdges, myWires, myFaces, myShells, mySolids);
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateReverseIndex() const
{
  return myReverseIdx.Validate(myEdges, myWires, myFaces, myShells, mySolids);
}
