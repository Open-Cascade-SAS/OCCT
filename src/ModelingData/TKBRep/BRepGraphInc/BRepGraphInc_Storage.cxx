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
  myIsDone             = false;
  myHasRegularities    = false;
  myHasVertexPointReps = false;
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

  BRepGraphInc::BaseEntity* anEnt = nullptr;
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
                     myCompSolids.Entities);
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
                             mySolids.Entities))
  {
    return false;
  }

  auto containsNode = [](const auto* theVec, const int theIndex) {
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
  };

  // Wire -> CoEdge and Edge -> CoEdge coherence.
  for (int aWireIdx = 0; aWireIdx < myWires.Nb(); ++aWireIdx)
  {
    const BRepGraphInc::WireEntity& aWire = myWires.Get(aWireIdx);
    if (aWire.IsRemoved)
    {
      continue;
    }

    for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < aWire.CoEdgeRefs.Length(); ++aCoEdgeRefIdx)
    {
      const BRepGraph_CoEdgeId aCoEdgeId = aWire.CoEdgeRefs.Value(aCoEdgeRefIdx).CoEdgeDefId;
      if (!aCoEdgeId.IsValid() || aCoEdgeId.Index >= myCoEdges.Nb())
      {
        return false;
      }

      const BRepGraphInc::CoEdgeEntity& aCoEdge = myCoEdges.Get(aCoEdgeId.Index);
      if (aCoEdge.IsRemoved)
      {
        return false;
      }
      if (!containsNode(myReverseIdx.WiresOfCoEdge(aCoEdgeId), aWireIdx))
      {
        return false;
      }
      if (aCoEdge.EdgeDefId.IsValid()
          && !containsNode(myReverseIdx.CoEdgesOfEdge(aCoEdge.EdgeDefId), aCoEdgeId.Index))
      {
        return false;
      }
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
      const BRepGraphInc::CoEdgeEntity& aCoEdge = myCoEdges.Get(aRefIdx);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid() || aCoEdge.EdgeDefId.Index != anEdgeIdx)
      {
        return false;
      }
    }
  }

  // Compound child reverse maps.
  for (int aCompIdx = 0; aCompIdx < myCompounds.Nb(); ++aCompIdx)
  {
    const BRepGraphInc::CompoundEntity& aCompound = myCompounds.Get(aCompIdx);
    if (aCompound.IsRemoved)
    {
      continue;
    }

    for (int aChildIdx = 0; aChildIdx < aCompound.ChildRefs.Length(); ++aChildIdx)
    {
      const BRepGraph_NodeId aChildId = aCompound.ChildRefs.Value(aChildIdx).ChildDefId;
      if (!aChildId.IsValid())
      {
        continue;
      }

      switch (aChildId.NodeKind)
      {
        case BRepGraph_NodeId::Kind::Solid:
          if (!containsNode(myReverseIdx.CompoundsOfSolid(BRepGraph_SolidId(aChildId.Index)), aCompIdx))
          {
            return false;
          }
          break;
        case BRepGraph_NodeId::Kind::Shell:
          if (!containsNode(myReverseIdx.CompoundsOfShell(BRepGraph_ShellId(aChildId.Index)), aCompIdx))
          {
            return false;
          }
          break;
        case BRepGraph_NodeId::Kind::Face:
          if (!containsNode(myReverseIdx.CompoundsOfFace(BRepGraph_FaceId(aChildId.Index)), aCompIdx))
          {
            return false;
          }
          break;
        case BRepGraph_NodeId::Kind::Compound:
          if (!containsNode(myReverseIdx.CompoundsOfCompound(BRepGraph_CompoundId(aChildId.Index)),
                            aCompIdx))
          {
            return false;
          }
          break;
        case BRepGraph_NodeId::Kind::CompSolid:
          if (!containsNode(myReverseIdx.CompoundsOfCompSolid(BRepGraph_CompSolidId(aChildId.Index)),
                            aCompIdx))
          {
            return false;
          }
          break;
        default:
          break;
      }
    }
  }

  // CompSolid -> Solid reverse map.
  for (int aCompSolidIdx = 0; aCompSolidIdx < myCompSolids.Nb(); ++aCompSolidIdx)
  {
    const BRepGraphInc::CompSolidEntity& aCompSolid = myCompSolids.Get(aCompSolidIdx);
    if (aCompSolid.IsRemoved)
    {
      continue;
    }

    for (int aSolidRefIdx = 0; aSolidRefIdx < aCompSolid.SolidRefs.Length(); ++aSolidRefIdx)
    {
      const BRepGraph_SolidId aSolidId = aCompSolid.SolidRefs.Value(aSolidRefIdx).SolidDefId;
      if (!aSolidId.IsValid() || aSolidId.Index >= mySolids.Nb())
      {
        return false;
      }
      if (!containsNode(myReverseIdx.CompSolidsOfSolid(aSolidId), aCompSolidIdx))
      {
        return false;
      }
    }
  }

  // Occurrence -> Product reverse map.
  for (int anOccIdx = 0; anOccIdx < myOccurrences.Nb(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceEntity& anOcc = myOccurrences.Get(anOccIdx);
    if (anOcc.IsRemoved)
    {
      continue;
    }
    if (!anOcc.ProductDefId.IsValid() || anOcc.ProductDefId.Index >= myProducts.Nb())
    {
      return false;
    }
    if (!containsNode(myReverseIdx.OccurrencesOfProduct(anOcc.ProductDefId), anOccIdx))
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
      const BRepGraphInc::OccurrenceEntity& anOcc = myOccurrences.Get(anOccIdx);
      if (anOcc.IsRemoved || !anOcc.ProductDefId.IsValid() || anOcc.ProductDefId.Index != aProductIdx)
      {
        return false;
      }
    }
  }

  return true;
}
