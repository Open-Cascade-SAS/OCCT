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

namespace
{

template <typename T>
bool containsNodeIndex(const NCollection_Vector<T>* theVec, const int theIndex)
{
  if (theVec == nullptr)
  {
    return false;
  }
  for (int anIdx = 0; anIdx < theVec->Length(); ++anIdx)
  {
    if (theVec->Value(anIdx).Index == theIndex)
    {
      return true;
    }
  }
  return false;
}

} // namespace

//=================================================================================================

BRepGraphInc_Storage::BRepGraphInc_Storage(const occ::handle<NCollection_BaseAllocator>& theAlloc)
    : myVertices(256, theAlloc),
      myEdges(256, theAlloc),
      myCoEdges(256, theAlloc),
      myWires(256, theAlloc),
      myFaces(256, theAlloc),
      myShells(256, theAlloc),
      mySolids(256, theAlloc),
      myCompounds(256, theAlloc),
      myCompSolids(256, theAlloc),
      myProducts(256, theAlloc),
      myOccurrences(256, theAlloc),
      myShellRefs(256, theAlloc),
      myFaceRefs(256, theAlloc),
      myWireRefs(256, theAlloc),
      myCoEdgeRefs(256, theAlloc),
      myVertexRefs(256, theAlloc),
      mySolidRefs(256, theAlloc),
      myChildRefs(256, theAlloc),
      myOccurrenceRefs(256, theAlloc),
      mySurfaces(256, theAlloc),
      myCurves3D(256, theAlloc),
      myCurves2D(256, theAlloc),
      myTriangulationsRep(256, theAlloc),
      myPolygons3D(256, theAlloc),
      myPolygons2D(256, theAlloc),
      myPolygonsOnTri(256, theAlloc),
      myTShapeToNodeId(1, theAlloc),
      myOriginalShapes(1, theAlloc),
      myAllocator(theAlloc.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAlloc)
{
}

//=================================================================================================

const NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::UIDs(
  const BRepGraph_NodeId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return myVertices.UIDs;
    case BRepGraph_NodeId::Kind::Edge:
      return myEdges.UIDs;
    case BRepGraph_NodeId::Kind::CoEdge:
      return myCoEdges.UIDs;
    case BRepGraph_NodeId::Kind::Wire:
      return myWires.UIDs;
    case BRepGraph_NodeId::Kind::Face:
      return myFaces.UIDs;
    case BRepGraph_NodeId::Kind::Shell:
      return myShells.UIDs;
    case BRepGraph_NodeId::Kind::Solid:
      return mySolids.UIDs;
    case BRepGraph_NodeId::Kind::Compound:
      return myCompounds.UIDs;
    case BRepGraph_NodeId::Kind::CompSolid:
      return myCompSolids.UIDs;
    case BRepGraph_NodeId::Kind::Product:
      return myProducts.UIDs;
    case BRepGraph_NodeId::Kind::Occurrence:
      return myOccurrences.UIDs;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "UIDs: unhandled Kind");
  static const NCollection_Vector<BRepGraph_UID> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================

NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::ChangeUIDs(
  const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return myVertices.UIDs;
    case BRepGraph_NodeId::Kind::Edge:
      return myEdges.UIDs;
    case BRepGraph_NodeId::Kind::CoEdge:
      return myCoEdges.UIDs;
    case BRepGraph_NodeId::Kind::Wire:
      return myWires.UIDs;
    case BRepGraph_NodeId::Kind::Face:
      return myFaces.UIDs;
    case BRepGraph_NodeId::Kind::Shell:
      return myShells.UIDs;
    case BRepGraph_NodeId::Kind::Solid:
      return mySolids.UIDs;
    case BRepGraph_NodeId::Kind::Compound:
      return myCompounds.UIDs;
    case BRepGraph_NodeId::Kind::CompSolid:
      return myCompSolids.UIDs;
    case BRepGraph_NodeId::Kind::Product:
      return myProducts.UIDs;
    case BRepGraph_NodeId::Kind::Occurrence:
      return myOccurrences.UIDs;
  }
  Standard_ASSERT_RETURN(false, "ChangeUIDs: invalid Kind value", myVertices.UIDs);
  return myVertices.UIDs;
}

//=================================================================================================

void BRepGraphInc_Storage::ResetAllUIDs()
{
  myVertices.UIDs.Clear();
  myEdges.UIDs.Clear();
  myCoEdges.UIDs.Clear();
  myWires.UIDs.Clear();
  myFaces.UIDs.Clear();
  myShells.UIDs.Clear();
  mySolids.UIDs.Clear();
  myCompounds.UIDs.Clear();
  myCompSolids.UIDs.Clear();
  myProducts.UIDs.Clear();
  myOccurrences.UIDs.Clear();
}

//=================================================================================================

const NCollection_Vector<BRepGraph_RefUID>& BRepGraphInc_Storage::RefUIDs(
  const BRepGraph_RefId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.UIDs;
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.UIDs;
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.UIDs;
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.UIDs;
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.UIDs;
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.UIDs;
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.UIDs;
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.UIDs;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "RefUIDs: unhandled Kind");
  static const NCollection_Vector<BRepGraph_RefUID> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================

NCollection_Vector<BRepGraph_RefUID>& BRepGraphInc_Storage::ChangeRefUIDs(
  const BRepGraph_RefId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.UIDs;
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.UIDs;
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.UIDs;
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.UIDs;
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.UIDs;
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.UIDs;
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.UIDs;
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.UIDs;
    default:
      break;
  }
  Standard_ASSERT_RETURN(false, "ChangeRefUIDs: invalid Kind value", myShellRefs.UIDs);
  return myShellRefs.UIDs;
}

//=================================================================================================

void BRepGraphInc_Storage::ResetAllRefUIDs()
{
  myShellRefs.UIDs.Clear();
  myFaceRefs.UIDs.Clear();
  myWireRefs.UIDs.Clear();
  myCoEdgeRefs.UIDs.Clear();
  myVertexRefs.UIDs.Clear();
  mySolidRefs.UIDs.Clear();
  myChildRefs.UIDs.Clear();
  myOccurrenceRefs.UIDs.Clear();
}

//=================================================================================================

const BRepGraphInc::BaseRef& BRepGraphInc_Storage::BaseRef(const BRepGraph_RefId theRefId) const
{
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.Get(theRefId.Index);
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.Get(theRefId.Index);
  }
  static const BRepGraphInc::BaseRef anInvalid;
  return anInvalid;
}

//=================================================================================================

BRepGraphInc::BaseRef& BRepGraphInc_Storage::ChangeBaseRef(const BRepGraph_RefId theRefId)
{
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.Change(theRefId.Index);
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.Change(theRefId.Index);
  }
  static BRepGraphInc::BaseRef anInvalid;
  return anInvalid;
}

//=================================================================================================

void BRepGraphInc_Storage::Clear()
{
  myVertices.Clear();
  myEdges.Clear();
  myCoEdges.Clear();
  myWires.Clear();
  myFaces.Clear();
  myShells.Clear();
  mySolids.Clear();
  myCompounds.Clear();
  myCompSolids.Clear();
  myProducts.Clear();
  myOccurrences.Clear();
  myShellRefs.Clear();
  myFaceRefs.Clear();
  myWireRefs.Clear();
  myCoEdgeRefs.Clear();
  myVertexRefs.Clear();
  mySolidRefs.Clear();
  myChildRefs.Clear();
  myOccurrenceRefs.Clear();
  mySurfaces.Clear();
  myCurves3D.Clear();
  myCurves2D.Clear();
  myTriangulationsRep.Clear();
  myPolygons3D.Clear();
  myPolygons2D.Clear();
  myPolygonsOnTri.Clear();
  myReverseIdx.Clear();
  myTShapeToNodeId.Clear();
  myOriginalShapes.Clear();
  myIsDone = false;
}

//=================================================================================================

void BRepGraphInc_Storage::DecrementActiveCount(const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      myVertices.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Edge:
      myEdges.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      myCoEdges.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Wire:
      myWires.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Face:
      myFaces.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Shell:
      myShells.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Solid:
      mySolids.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Compound:
      myCompounds.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      myCompSolids.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Product:
      myProducts.DecrementActive();
      break;
    case BRepGraph_NodeId::Kind::Occurrence:
      myOccurrences.DecrementActive();
      break;
    default:
      Standard_ASSERT_VOID(false, "DecrementActiveCount: unhandled Kind");
      break;
  }
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemoved(const BRepGraph_NodeId theNodeId)
{
  if (!theNodeId.IsValid())
    return false;

  BRepGraphInc::BaseDef* anEnt = nullptr;
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      if (theNodeId.Index >= 0 && theNodeId.Index < myVertices.Nb())
        anEnt = &myVertices.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Edge:
      if (theNodeId.Index >= 0 && theNodeId.Index < myEdges.Nb())
        anEnt = &myEdges.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      if (theNodeId.Index >= 0 && theNodeId.Index < myCoEdges.Nb())
        anEnt = &myCoEdges.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Wire:
      if (theNodeId.Index >= 0 && theNodeId.Index < myWires.Nb())
        anEnt = &myWires.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Face:
      if (theNodeId.Index >= 0 && theNodeId.Index < myFaces.Nb())
        anEnt = &myFaces.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Shell:
      if (theNodeId.Index >= 0 && theNodeId.Index < myShells.Nb())
        anEnt = &myShells.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Solid:
      if (theNodeId.Index >= 0 && theNodeId.Index < mySolids.Nb())
        anEnt = &mySolids.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Compound:
      if (theNodeId.Index >= 0 && theNodeId.Index < myCompounds.Nb())
        anEnt = &myCompounds.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      if (theNodeId.Index >= 0 && theNodeId.Index < myCompSolids.Nb())
        anEnt = &myCompSolids.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Product:
      if (theNodeId.Index >= 0 && theNodeId.Index < myProducts.Nb())
        anEnt = &myProducts.Change(theNodeId.Index);
      break;
    case BRepGraph_NodeId::Kind::Occurrence:
      if (theNodeId.Index >= 0 && theNodeId.Index < myOccurrences.Nb())
        anEnt = &myOccurrences.Change(theNodeId.Index);
      break;
    default:
      return false;
  }

  if (anEnt == nullptr || anEnt->IsRemoved)
    return false;

  anEnt->IsRemoved = true;
  DecrementActiveCount(theNodeId.NodeKind);
  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemovedRef(const BRepGraph_RefId theRefId)
{
  if (!theRefId.IsValid())
    return false;

  BRepGraphInc::BaseRef* aRef = nullptr;
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      if (theRefId.Index >= 0 && theRefId.Index < myShellRefs.Nb())
        aRef = &myShellRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Face:
      if (theRefId.Index >= 0 && theRefId.Index < myFaceRefs.Nb())
        aRef = &myFaceRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Wire:
      if (theRefId.Index >= 0 && theRefId.Index < myWireRefs.Nb())
        aRef = &myWireRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::CoEdge:
      if (theRefId.Index >= 0 && theRefId.Index < myCoEdgeRefs.Nb())
        aRef = &myCoEdgeRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Vertex:
      if (theRefId.Index >= 0 && theRefId.Index < myVertexRefs.Nb())
        aRef = &myVertexRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Solid:
      if (theRefId.Index >= 0 && theRefId.Index < mySolidRefs.Nb())
        aRef = &mySolidRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Child:
      if (theRefId.Index >= 0 && theRefId.Index < myChildRefs.Nb())
        aRef = &myChildRefs.Change(theRefId.Index);
      break;
    case BRepGraph_RefId::Kind::Occurrence:
      if (theRefId.Index >= 0 && theRefId.Index < myOccurrenceRefs.Nb())
        aRef = &myOccurrenceRefs.Change(theRefId.Index);
      break;
    default:
      return false;
  }

  if (aRef == nullptr || aRef->IsRemoved)
    return false;

  aRef->IsRemoved = true;
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      myShellRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Face:
      myFaceRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Wire:
      myWireRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::CoEdge:
      myCoEdgeRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Vertex:
      myVertexRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Solid:
      mySolidRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Child:
      myChildRefs.DecrementActive();
      break;
    case BRepGraph_RefId::Kind::Occurrence:
      myOccurrenceRefs.DecrementActive();
      break;
    default:
      return false;
  }

  return true;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildReverseIndex()
{
  myReverseIdx.SetAllocator(myAllocator);
  myReverseIdx.Build(myEdges.Entities,
                     myCoEdges.Entities,
                     myWires.Entities,
                     myFaces.Entities,
                     myShells.Entities,
                     mySolids.Entities,
                     myCompounds.Entities,
                     myCompSolids.Entities,
                     myShellRefs.Refs,
                     myFaceRefs.Refs,
                     myWireRefs.Refs,
                     myCoEdgeRefs.Refs,
                     mySolidRefs.Refs,
                     myChildRefs.Refs,
                     myVertexRefs.Refs);
  myReverseIdx.BuildProductOccurrences(myOccurrences.Entities, myProducts.Nb());

  // Recount active entities to sync counters after Build.
  // Populate may have set IsRemoved on some entities without going through RemoveNode.
  myVertices.NbActive    = 0;
  myEdges.NbActive       = 0;
  myCoEdges.NbActive     = 0;
  myWires.NbActive       = 0;
  myFaces.NbActive       = 0;
  myShells.NbActive      = 0;
  mySolids.NbActive      = 0;
  myCompounds.NbActive   = 0;
  myCompSolids.NbActive  = 0;
  myProducts.NbActive    = 0;
  myOccurrences.NbActive = 0;
  for (int i = 0; i < myVertices.Nb(); ++i)
    if (!myVertices.Get(i).IsRemoved)
      ++myVertices.NbActive;
  for (int i = 0; i < myEdges.Nb(); ++i)
    if (!myEdges.Get(i).IsRemoved)
      ++myEdges.NbActive;
  for (int i = 0; i < myCoEdges.Nb(); ++i)
    if (!myCoEdges.Get(i).IsRemoved)
      ++myCoEdges.NbActive;
  for (int i = 0; i < myWires.Nb(); ++i)
    if (!myWires.Get(i).IsRemoved)
      ++myWires.NbActive;
  for (int i = 0; i < myFaces.Nb(); ++i)
    if (!myFaces.Get(i).IsRemoved)
      ++myFaces.NbActive;
  for (int i = 0; i < myShells.Nb(); ++i)
    if (!myShells.Get(i).IsRemoved)
      ++myShells.NbActive;
  for (int i = 0; i < mySolids.Nb(); ++i)
    if (!mySolids.Get(i).IsRemoved)
      ++mySolids.NbActive;
  for (int i = 0; i < myCompounds.Nb(); ++i)
    if (!myCompounds.Get(i).IsRemoved)
      ++myCompounds.NbActive;
  for (int i = 0; i < myCompSolids.Nb(); ++i)
    if (!myCompSolids.Get(i).IsRemoved)
      ++myCompSolids.NbActive;
  for (int i = 0; i < myProducts.Nb(); ++i)
    if (!myProducts.Get(i).IsRemoved)
      ++myProducts.NbActive;
  for (int i = 0; i < myOccurrences.Nb(); ++i)
    if (!myOccurrences.Get(i).IsRemoved)
      ++myOccurrences.NbActive;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildDeltaReverseIndex(const int theOldNbEdges,
                                                  const int theOldNbWires,
                                                  const int theOldNbFaces,
                                                  const int theOldNbShells,
                                                  const int theOldNbSolids)
{
  myReverseIdx.BuildDelta(myEdges.Entities,
                          myCoEdges.Entities,
                          myWires.Entities,
                          myFaces.Entities,
                          myShells.Entities,
                          mySolids.Entities,
                          myShellRefs.Refs,
                          myFaceRefs.Refs,
                          myWireRefs.Refs,
                          myCoEdgeRefs.Refs,
                          myVertexRefs.Refs,
                          theOldNbEdges,
                          theOldNbWires,
                          theOldNbFaces,
                          theOldNbShells,
                          theOldNbSolids);
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateReverseIndex() const
{
  if (!myReverseIdx.Validate(myEdges.Entities,
                             myCoEdges.Entities,
                             myWires.Entities,
                             myFaces.Entities,
                             myShells.Entities,
                             mySolids.Entities,
                             myShellRefs.Refs,
                             myFaceRefs.Refs,
                             myWireRefs.Refs,
                             myCoEdgeRefs.Refs,
                             myVertexRefs.Refs))
  {
    return false;
  }

  // Wire -> CoEdge and Edge -> CoEdge coherence via coedge ref entries.
  for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < myCoEdgeRefs.Nb(); ++aCoEdgeRefIdx)
  {
    const BRepGraphInc::CoEdgeRef& aRef = myCoEdgeRefs.Get(aCoEdgeRefIdx);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire || !aRef.CoEdgeDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myWires.Nb() || aRef.CoEdgeDefId.Index < 0
        || aRef.CoEdgeDefId.Index >= myCoEdges.Nb())
    {
      return false;
    }

    const BRepGraphInc::WireDef&   aWire   = myWires.Get(aRef.ParentId.Index);
    const BRepGraphInc::CoEdgeDef& aCoEdge = myCoEdges.Get(aRef.CoEdgeDefId.Index);
    if (aWire.IsRemoved || aCoEdge.IsRemoved)
    {
      continue;
    }
    if (!containsNodeIndex(myReverseIdx.WiresOfCoEdge(aRef.CoEdgeDefId), aRef.ParentId.Index))
    {
      return false;
    }
    if (aCoEdge.EdgeDefId.IsValid()
        && !containsNodeIndex(myReverseIdx.CoEdgesOfEdge(aCoEdge.EdgeDefId),
                              aRef.CoEdgeDefId.Index))
    {
      return false;
    }
  }

  // Reverse Edge -> CoEdges entries must point back to active CoEdges.
  for (int anEdgeIdx = 0; anEdgeIdx < myEdges.Nb(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdges =
      myReverseIdx.CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    if (aCoEdges == nullptr)
    {
      continue;
    }
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aCoEdges->Length(); ++aCoEdgeIdx)
    {
      const int aRefIdx = aCoEdges->Value(aCoEdgeIdx).Index;
      if (aRefIdx < 0 || aRefIdx >= myCoEdges.Nb())
      {
        return false;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = myCoEdges.Get(aRefIdx);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid() || aCoEdge.EdgeDefId.Index != anEdgeIdx)
      {
        return false;
      }
    }
  }

  // Compound child reverse maps via child ref entries.
  for (int aChildRefIdx = 0; aChildRefIdx < myChildRefs.Nb(); ++aChildRefIdx)
  {
    const BRepGraphInc::ChildRef& aRef = myChildRefs.Get(aChildRefIdx);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Compound || !aRef.ChildDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myCompounds.Nb())
    {
      return false;
    }

    const BRepGraphInc::CompoundDef& aCompound = myCompounds.Get(aRef.ParentId.Index);
    if (aCompound.IsRemoved)
    {
      continue;
    }

    switch (aRef.ChildDefId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfSolid(BRepGraph_SolidId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Shell:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfShell(BRepGraph_ShellId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Face:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfFace(BRepGraph_FaceId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Compound:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfCompound(BRepGraph_CompoundId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfCompSolid(BRepGraph_CompSolidId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      default:
        break;
    }
  }

  // CompSolid -> Solid reverse map via solid ref entries.
  for (int aSolidRefIdx = 0; aSolidRefIdx < mySolidRefs.Nb(); ++aSolidRefIdx)
  {
    const BRepGraphInc::SolidRef& aRef = mySolidRefs.Get(aSolidRefIdx);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::CompSolid
        || !aRef.SolidDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myCompSolids.Nb()
        || aRef.SolidDefId.Index < 0 || aRef.SolidDefId.Index >= mySolids.Nb())
    {
      return false;
    }

    const BRepGraphInc::CompSolidDef& aCompSolid = myCompSolids.Get(aRef.ParentId.Index);
    if (aCompSolid.IsRemoved || mySolids.Get(aRef.SolidDefId.Index).IsRemoved)
    {
      continue;
    }
    if (!containsNodeIndex(myReverseIdx.CompSolidsOfSolid(aRef.SolidDefId), aRef.ParentId.Index))
    {
      return false;
    }
  }

  // Occurrence -> Product reverse map.
  for (int anOccIdx = 0; anOccIdx < myOccurrences.Nb(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceDef& anOcc = myOccurrences.Get(anOccIdx);
    if (anOcc.IsRemoved)
    {
      continue;
    }
    if (!anOcc.ProductDefId.IsValid() || anOcc.ProductDefId.Index >= myProducts.Nb())
    {
      return false;
    }
    if (!containsNodeIndex(myReverseIdx.OccurrencesOfProduct(anOcc.ProductDefId), anOccIdx))
    {
      return false;
    }
  }

  // Reverse Product -> Occurrences entries must point to active occurrences
  // that reference the same product.
  for (int aProductIdx = 0; aProductIdx < myProducts.Nb(); ++aProductIdx)
  {
    const NCollection_Vector<BRepGraph_OccurrenceId>* anOccs =
      myReverseIdx.OccurrencesOfProduct(BRepGraph_ProductId(aProductIdx));
    if (anOccs == nullptr)
    {
      continue;
    }
    for (int anOccRefIdx = 0; anOccRefIdx < anOccs->Length(); ++anOccRefIdx)
    {
      const int anOccIdx = anOccs->Value(anOccRefIdx).Index;
      if (anOccIdx < 0 || anOccIdx >= myOccurrences.Nb())
      {
        return false;
      }
      const BRepGraphInc::OccurrenceDef& anOcc = myOccurrences.Get(anOccIdx);
      if (anOcc.IsRemoved || !anOcc.ProductDefId.IsValid()
          || anOcc.ProductDefId.Index != aProductIdx)
      {
        return false;
      }
    }
  }

  return true;
}
