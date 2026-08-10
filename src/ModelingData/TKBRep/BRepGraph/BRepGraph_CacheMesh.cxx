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

#include <BRepGraph_CacheMesh.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_CacheMesh::Driver, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_CacheMesh, BRepGraph_Cache)

namespace
{
const Standard_GUID& theGUID()
{
  static const Standard_GUID aGUID("d706ee7b-3c3a-4994-8fdc-c6902bd81450");
  return aGUID;
}

bool isSameDriver(const occ::handle<BRepGraph_CacheMesh::Driver>& theLeft,
                  const occ::handle<BRepGraph_CacheMesh::Driver>& theRight)
{
  if (theLeft.IsNull() || theRight.IsNull())
  {
    return theLeft == theRight;
  }
  return theLeft->ID().IsSame(theRight->ID()) && theLeft->RecipeHash() == theRight->RecipeHash();
}

template <typename IdT>
IdT remappedNode(const BRepGraph_CopyRemap& theCopy, const IdT theId)
{
  if (!theId.IsValid())
  {
    return IdT();
  }
  const BRepGraph_ItemId aTarget = theCopy.TargetItem(BRepGraph_ItemId(theId));
  if (!aTarget.IsNode())
  {
    return IdT();
  }
  return IdT::FromNodeId(aTarget.NodeId());
}

void appendPolygonsOnTri(
  NCollection_LinearVector<occ::handle<Poly_PolygonOnTriangulation>>&       theTarget,
  const NCollection_LinearVector<occ::handle<Poly_PolygonOnTriangulation>>& theSource)
{
  for (const occ::handle<Poly_PolygonOnTriangulation>& aPolygon : theSource)
  {
    if (!aPolygon.IsNull())
    {
      theTarget.Append(aPolygon);
    }
  }
}
} // namespace

//=================================================================================================

struct BRepGraph_CacheMesh::Slot
{
  Slot()
      : Allocator(new NCollection_IncAllocator),
        FaceMeshes(256, Allocator),
        CoEdgeMeshes(256, Allocator),
        EdgeMeshes(256, Allocator)
  {
    Allocator->SetThreadSafe(true);
  }

  SlotId              Id = DefaultDisplaySlot;
  occ::handle<Driver> MeshDriver;
  uint64_t            RecipeHash = 0;
  uint32_t            Generation = 1;

  occ::handle<NCollection_IncAllocator>     Allocator;
  NCollection_DynamicArray<FaceMeshEntry>   FaceMeshes;
  NCollection_DynamicArray<CoEdgeMeshEntry> CoEdgeMeshes;
  NCollection_DynamicArray<EdgeMeshEntry>   EdgeMeshes;

  void Clear()
  {
    FaceMeshes.Clear(true);
    CoEdgeMeshes.Clear(true);
    EdgeMeshes.Clear(true);
    Allocator->Reset(false);
    ++Generation;
  }
};

//=================================================================================================

template <typename T>
void BRepGraph_CacheMesh::ensureSize(NCollection_DynamicArray<T>& theVec, const size_t theIndex)
{
  while (theVec.Size() <= theIndex)
  {
    theVec.Appended();
  }
}

//=================================================================================================

BRepGraph_CacheMesh::BRepGraph_CacheMesh()
{
  (void)changeSlot(DefaultDisplaySlot);
}

//=================================================================================================

const Standard_GUID& BRepGraph_CacheMesh::GetID()
{
  return theGUID();
}

//=================================================================================================

const Standard_GUID& BRepGraph_CacheMesh::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_CacheMesh::Name() const
{
  static const TCollection_AsciiString aName("BRepGraph.CacheMesh");
  return aName;
}

//=================================================================================================

void BRepGraph_CacheMesh::Clear() noexcept
{
  for (size_t aSlotIdx = 0; aSlotIdx < mySlots.Size(); ++aSlotIdx)
  {
    mySlots.ChangeValue(aSlotIdx).Clear();
  }
}

//=================================================================================================

void BRepGraph_CacheMesh::CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const
{
  occ::handle<BRepGraph_CacheMesh> aTargetCache =
    theCopy.TargetGraph().CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  aTargetCache->SetActiveDisplaySlot(myActiveSlot);

  for (size_t aSlotIdx = 0; aSlotIdx < mySlots.Size(); ++aSlotIdx)
  {
    const Slot& aSrcSlot = mySlots.Value(aSlotIdx);
    Slot&       aDstSlot = aTargetCache->changeSlot(aSrcSlot.Id);

    aDstSlot.MeshDriver = aSrcSlot.MeshDriver;
    aDstSlot.RecipeHash = aSrcSlot.RecipeHash;
    aDstSlot.Clear();

    for (BRepGraph_FaceId aSrcFace = BRepGraph_FaceId::Start();
         aSrcFace.IsValid(static_cast<uint32_t>(aSrcSlot.FaceMeshes.Size()));
         ++aSrcFace)
    {
      const FaceMeshEntry* aSrcEntry = FindFaceMesh(aSrcSlot.Id, aSrcFace);
      if (aSrcEntry == nullptr)
      {
        continue;
      }
      const BRepGraph_FaceId aDstFace = remappedNode(theCopy, aSrcFace);
      if (!aDstFace.IsValidIn(theCopy.TargetGraph().Topo().Faces()))
      {
        continue;
      }
      FaceMeshEntry& aDstEntry = aTargetCache->ChangeFaceMesh(aSrcSlot.Id, aDstFace);
      aDstEntry.Triangulation  = aSrcEntry->Triangulation;
      aTargetCache->bindEntry(aDstEntry, aDstFace, aDstSlot);
      ++aDstEntry.MeshGeneration;
    }

    for (BRepGraph_EdgeId aSrcEdge = BRepGraph_EdgeId::Start();
         aSrcEdge.IsValid(static_cast<uint32_t>(aSrcSlot.EdgeMeshes.Size()));
         ++aSrcEdge)
    {
      const EdgeMeshEntry* aSrcEntry = FindEdgeMesh(aSrcSlot.Id, aSrcEdge);
      if (aSrcEntry == nullptr)
      {
        continue;
      }
      const BRepGraph_EdgeId aDstEdge = remappedNode(theCopy, aSrcEdge);
      if (!aDstEdge.IsValidIn(theCopy.TargetGraph().Topo().Edges()))
      {
        continue;
      }
      EdgeMeshEntry& aDstEntry = aTargetCache->ChangeEdgeMesh(aSrcSlot.Id, aDstEdge);
      aDstEntry.Polygon3D      = aSrcEntry->Polygon3D;
      aTargetCache->bindEntry(aDstEntry, aDstEdge, aDstSlot);
    }

    for (BRepGraph_CoEdgeId aSrcCoEdge = BRepGraph_CoEdgeId::Start();
         aSrcCoEdge.IsValid(static_cast<uint32_t>(aSrcSlot.CoEdgeMeshes.Size()));
         ++aSrcCoEdge)
    {
      const CoEdgeMeshEntry* aSrcRaw = findCoEdgeEntryRaw(aSrcSlot.Id, aSrcCoEdge);
      if (aSrcRaw == nullptr)
      {
        continue;
      }

      const bool hasFreshPolygon2D     = FindCoEdgePolygon2D(aSrcSlot.Id, aSrcCoEdge) != nullptr;
      bool       hasFreshPolygonsOnTri = FindCoEdgePolygonOnTri(aSrcSlot.Id, aSrcCoEdge) != nullptr;
      if (!hasFreshPolygon2D && !hasFreshPolygonsOnTri)
      {
        continue;
      }

      const BRepGraph_CoEdgeId aDstCoEdge = remappedNode(theCopy, aSrcCoEdge);
      if (!aDstCoEdge.IsValidIn(theCopy.TargetGraph().Topo().CoEdges()))
      {
        continue;
      }

      if (hasFreshPolygonsOnTri)
      {
        const BRepGraphInc::CoEdgeDef& aDstDef =
          theCopy.TargetGraph().Topo().CoEdges().Definition(aDstCoEdge);
        hasFreshPolygonsOnTri =
          aDstDef.FaceId.IsValidIn(aDstSlot.FaceMeshes)
          && aTargetCache->findFaceEntryRaw(aSrcSlot.Id, aDstDef.FaceId) != nullptr;
      }

      CoEdgeMeshEntry& aDstEntry = aTargetCache->ChangeCoEdgeMesh(aSrcSlot.Id, aDstCoEdge);
      if (hasFreshPolygon2D)
      {
        aDstEntry.Polygon2D = aSrcRaw->Polygon2D;
      }
      if (hasFreshPolygonsOnTri)
      {
        appendPolygonsOnTri(aDstEntry.PolygonsOnTri, aSrcRaw->PolygonsOnTri);
      }
      if (aDstEntry.IsPresent())
      {
        aTargetCache->bindEntry(aDstEntry, aDstCoEdge, aDstSlot);
      }
      else
      {
        aDstEntry.Reset();
      }
    }
  }
}

//=================================================================================================

BRepGraph_CacheMesh::Slot& BRepGraph_CacheMesh::changeSlot(const SlotId theSlot)
{
  while (mySlots.Size() <= theSlot)
  {
    mySlots.EmplaceAppend();
  }
  Slot& aSlot = mySlots.ChangeValue(static_cast<size_t>(theSlot));
  aSlot.Id    = theSlot;
  return aSlot;
}

//=================================================================================================

const BRepGraph_CacheMesh::Slot* BRepGraph_CacheMesh::findSlot(const SlotId theSlot) const
{
  if (theSlot >= mySlots.Size())
  {
    return nullptr;
  }
  return &mySlots.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

void BRepGraph_CacheMesh::RegisterDriver(const SlotId theSlot, const occ::handle<Driver>& theDriver)
{
  Slot& aSlot = changeSlot(theSlot);
  if (isSameDriver(aSlot.MeshDriver, theDriver))
  {
    aSlot.MeshDriver = theDriver;
    return;
  }

  aSlot.MeshDriver = theDriver;
  aSlot.RecipeHash = theDriver.IsNull() ? 0 : theDriver->RecipeHash();
  aSlot.Clear();
}

//=================================================================================================

void BRepGraph_CacheMesh::UnregisterDriver(const SlotId theSlot)
{
  Slot* aSlot = const_cast<Slot*>(findSlot(theSlot));
  if (aSlot == nullptr)
  {
    return;
  }
  aSlot->MeshDriver.Nullify();
  aSlot->RecipeHash = 0;
  aSlot->Clear();
}

//=================================================================================================

const occ::handle<BRepGraph_CacheMesh::Driver>& BRepGraph_CacheMesh::DriverOf(
  const SlotId theSlot) const
{
  static const occ::handle<Driver> THE_NULL_DRIVER;
  const Slot*                      aSlot = findSlot(theSlot);
  return aSlot == nullptr ? THE_NULL_DRIVER : aSlot->MeshDriver;
}

//=================================================================================================

BRepGraph_CacheMesh::SlotState BRepGraph_CacheMesh::State(const SlotId theSlot) const
{
  SlotState aState;
  aState.Slot = theSlot;
  if (const Slot* aSlot = findSlot(theSlot))
  {
    aState.RecipeHash = aSlot->RecipeHash;
    aState.Generation = aSlot->Generation;
    aState.HasDriver  = !aSlot->MeshDriver.IsNull();
  }
  return aState;
}

//=================================================================================================

bool BRepGraph_CacheMesh::isSlotActual(const Slot&       theSlot,
                                       const EntryStamp& theStamp) const noexcept
{
  return theStamp.RecipeHash == theSlot.RecipeHash && theStamp.SlotGeneration == theSlot.Generation;
}

//=================================================================================================

void BRepGraph_CacheMesh::bindEntry(FaceMeshEntry&         theEntry,
                                    const BRepGraph_FaceId theFace,
                                    const Slot&            theSlot) const
{
  (void)theEntry.BindSubtreeGen(*this, BRepGraph_NodeId(theFace));
  theEntry.Stamp.RecipeHash     = theSlot.RecipeHash;
  theEntry.Stamp.SlotGeneration = theSlot.Generation;
}

//=================================================================================================

void BRepGraph_CacheMesh::bindEntry(CoEdgeMeshEntry&         theEntry,
                                    const BRepGraph_CoEdgeId theCoEdge,
                                    const Slot&              theSlot) const
{
  (void)theEntry.CoEdgeStamp.BindSubtreeGen(*this, BRepGraph_NodeId(theCoEdge));

  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph != nullptr)
  {
    const BRepGraphInc::CoEdgeDef& aDef = aGraph->Topo().CoEdges().Definition(theCoEdge);
    if (aDef.FaceId.IsValid())
    {
      (void)theEntry.FaceTopologyStamp.BindSubtreeGen(*this, BRepGraph_NodeId(aDef.FaceId));
      theEntry.BoundFaceId            = aDef.FaceId;
      const FaceMeshEntry* aFaceEntry = findFaceEntryRaw(theSlot.Id, aDef.FaceId);
      theEntry.FaceMeshGeneration     = aFaceEntry != nullptr ? aFaceEntry->MeshGeneration : 0;
    }
  }

  theEntry.SlotStamp.RecipeHash     = theSlot.RecipeHash;
  theEntry.SlotStamp.SlotGeneration = theSlot.Generation;
}

//=================================================================================================

void BRepGraph_CacheMesh::bindEntry(EdgeMeshEntry&         theEntry,
                                    const BRepGraph_EdgeId theEdge,
                                    const Slot&            theSlot) const
{
  (void)theEntry.BindSubtreeGen(*this, BRepGraph_NodeId(theEdge));
  theEntry.Stamp.RecipeHash     = theSlot.RecipeHash;
  theEntry.Stamp.SlotGeneration = theSlot.Generation;
}

//=================================================================================================

void BRepGraph_CacheMesh::BindFresh(FaceMeshEntry& theEntry, const BRepGraph_FaceId theFace) const
{
  const Slot* aSlot = findSlot(DefaultDisplaySlot);
  if (aSlot != nullptr)
  {
    bindEntry(theEntry, theFace, *aSlot);
  }
}

//=================================================================================================

void BRepGraph_CacheMesh::BindFresh(CoEdgeMeshEntry&         theEntry,
                                    const BRepGraph_CoEdgeId theCoEdge) const
{
  const Slot* aSlot = findSlot(DefaultDisplaySlot);
  if (aSlot != nullptr)
  {
    bindEntry(theEntry, theCoEdge, *aSlot);
  }
}

//=================================================================================================

void BRepGraph_CacheMesh::BindFresh(EdgeMeshEntry& theEntry, const BRepGraph_EdgeId theEdge) const
{
  const Slot* aSlot = findSlot(DefaultDisplaySlot);
  if (aSlot != nullptr)
  {
    bindEntry(theEntry, theEdge, *aSlot);
  }
}

//=================================================================================================

bool BRepGraph_CacheMesh::Ensure(BRepGraph&                   theGraph,
                                 const SlotId                 theSlot,
                                 const Message_ProgressRange& theRange)
{
  Slot& aSlot = changeSlot(theSlot);
  if (aSlot.MeshDriver.IsNull())
  {
    return false;
  }

  const uint64_t aRecipeHash = aSlot.MeshDriver->RecipeHash();
  if (aSlot.RecipeHash != aRecipeHash)
  {
    aSlot.RecipeHash = aRecipeHash;
    aSlot.Clear();
  }

  DirtySet aDirtySet = collectDirty(theGraph, aSlot);
  if (aDirtySet.IsEmpty())
  {
    return true;
  }
  return aSlot.MeshDriver->Fill(theGraph, theSlot, aDirtySet, theRange);
}

//=================================================================================================

bool BRepGraph_CacheMesh::Ensure(BRepGraph&                   theGraph,
                                 const BRepGraph_NodeId       theRoot,
                                 const SlotId                 theSlot,
                                 const Message_ProgressRange& theRange)
{
  Slot& aSlot = changeSlot(theSlot);
  if (aSlot.MeshDriver.IsNull())
  {
    return false;
  }

  const uint64_t aRecipeHash = aSlot.MeshDriver->RecipeHash();
  if (aSlot.RecipeHash != aRecipeHash)
  {
    aSlot.RecipeHash = aRecipeHash;
    aSlot.Clear();
  }

  DirtySet aDirtySet = collectDirty(theGraph, theRoot, aSlot);
  if (aDirtySet.IsEmpty())
  {
    return true;
  }
  return aSlot.MeshDriver->Fill(theGraph, theSlot, aDirtySet, theRange);
}

//=================================================================================================

bool BRepGraph_CacheMesh::Ensure(BRepGraph&                                  theGraph,
                                 const NCollection_Array1<BRepGraph_NodeId>& theNodes,
                                 const SlotId                                theSlot,
                                 const Message_ProgressRange&                theRange)
{
  Slot& aSlot = changeSlot(theSlot);
  if (aSlot.MeshDriver.IsNull())
  {
    return false;
  }

  const uint64_t aRecipeHash = aSlot.MeshDriver->RecipeHash();
  if (aSlot.RecipeHash != aRecipeHash)
  {
    aSlot.RecipeHash = aRecipeHash;
    aSlot.Clear();
  }

  DirtySet aDirtySet = collectDirty(theGraph, theNodes, aSlot);
  if (aDirtySet.IsEmpty())
  {
    return true;
  }
  return aSlot.MeshDriver->Fill(theGraph, theSlot, aDirtySet, theRange);
}

//=================================================================================================

bool BRepGraph_CacheMesh::Needs(BRepGraph&             theGraph,
                                const BRepGraph_NodeId theRoot,
                                const SlotId           theSlot) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr)
  {
    return true;
  }
  if (!aSlot->MeshDriver.IsNull() && aSlot->RecipeHash != aSlot->MeshDriver->RecipeHash())
  {
    return true;
  }
  return !collectDirty(theGraph, theRoot, *aSlot).IsEmpty();
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasFaceMesh(const BRepGraph_FaceId theFace) const
{
  return HasFaceMesh(DefaultDisplaySlot, theFace);
}

//=================================================================================================

const BRepGraph_CacheMesh::FaceMeshEntry* BRepGraph_CacheMesh::FindFaceMesh(
  const BRepGraph_FaceId theFace) const
{
  return FindFaceMesh(DefaultDisplaySlot, theFace);
}

//=================================================================================================

BRepGraph_CacheMesh::FaceMeshEntry& BRepGraph_CacheMesh::ChangeFaceMesh(
  const BRepGraph_FaceId theFace)
{
  return ChangeFaceMesh(DefaultDisplaySlot, theFace);
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearFaceMesh(const BRepGraph_FaceId theFace)
{
  ClearFaceMesh(DefaultDisplaySlot, theFace);
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge) const
{
  return findCoEdgeMesh(DefaultDisplaySlot, theCoEdge) != nullptr;
}

//=================================================================================================

//=================================================================================================

BRepGraph_CacheMesh::CoEdgeMeshEntry& BRepGraph_CacheMesh::ChangeCoEdgeMesh(
  const BRepGraph_CoEdgeId theCoEdge)
{
  return ChangeCoEdgeMesh(DefaultDisplaySlot, theCoEdge);
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge)
{
  ClearCoEdgeMesh(DefaultDisplaySlot, theCoEdge);
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasEdgeMesh(const BRepGraph_EdgeId theEdge) const
{
  return HasEdgeMesh(DefaultDisplaySlot, theEdge);
}

//=================================================================================================

const BRepGraph_CacheMesh::EdgeMeshEntry* BRepGraph_CacheMesh::FindEdgeMesh(
  const BRepGraph_EdgeId theEdge) const
{
  return FindEdgeMesh(DefaultDisplaySlot, theEdge);
}

//=================================================================================================

BRepGraph_CacheMesh::EdgeMeshEntry& BRepGraph_CacheMesh::ChangeEdgeMesh(
  const BRepGraph_EdgeId theEdge)
{
  return ChangeEdgeMesh(DefaultDisplaySlot, theEdge);
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearEdgeMesh(const BRepGraph_EdgeId theEdge)
{
  ClearEdgeMesh(DefaultDisplaySlot, theEdge);
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasFaceMesh(const SlotId theSlot, const BRepGraph_FaceId theFace) const
{
  return FindFaceMesh(theSlot, theFace) != nullptr;
}

//=================================================================================================

const BRepGraph_CacheMesh::FaceMeshEntry* BRepGraph_CacheMesh::FindFaceMesh(
  const SlotId           theSlot,
  const BRepGraph_FaceId theFace) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theFace.IsValidIn(aSlot->FaceMeshes))
  {
    return nullptr;
  }

  const FaceMeshEntry& anEntry = aSlot->FaceMeshes.Value(static_cast<size_t>(theFace.Index));
  if (!anEntry.IsPresent() || !isSlotActual(*aSlot, anEntry.Stamp) || !anEntry.IsFresh(*this))
  {
    return nullptr;
  }
  return &anEntry;
}

//=================================================================================================

BRepGraph_CacheMesh::FaceMeshEntry& BRepGraph_CacheMesh::ChangeFaceMesh(
  const SlotId           theSlot,
  const BRepGraph_FaceId theFace)
{
  Slot& aSlot = changeSlot(theSlot);
  ensureSize(aSlot.FaceMeshes, static_cast<size_t>(theFace.Index));
  return aSlot.FaceMeshes.ChangeValue(static_cast<size_t>(theFace.Index));
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearFaceMesh(const SlotId theSlot, const BRepGraph_FaceId theFace)
{
  Slot* aSlot = const_cast<Slot*>(findSlot(theSlot));
  if (aSlot != nullptr && theFace.IsValidIn(aSlot->FaceMeshes))
  {
    aSlot->FaceMeshes.ChangeValue(static_cast<size_t>(theFace.Index)).Reset();
  }
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasCoEdgeMesh(const SlotId             theSlot,
                                        const BRepGraph_CoEdgeId theCoEdge) const
{
  return findCoEdgeMesh(theSlot, theCoEdge) != nullptr;
}

//=================================================================================================

//=================================================================================================

bool BRepGraph_CacheMesh::isCoEdgePolygon2DFresh(const CoEdgeMeshEntry& theEntry,
                                                 const Slot&            theSlot) const noexcept
{
  return isSlotActual(theSlot, theEntry.SlotStamp) && theEntry.CoEdgeStamp.IsFresh(*this);
}

//=================================================================================================

bool BRepGraph_CacheMesh::isCoEdgePolygonOnTriFresh(const CoEdgeMeshEntry& theEntry,
                                                    const Slot&            theSlot) const noexcept
{
  if (!isSlotActual(theSlot, theEntry.SlotStamp) || !theEntry.CoEdgeStamp.IsFresh(*this))
  {
    return false;
  }
  if (!theEntry.FaceTopologyStamp.IsBound())
  {
    return true; // free coedge: no face dependency
  }
  return theEntry.FaceTopologyStamp.IsFresh(*this) && isFaceMeshFresh(theEntry, theSlot);
}

//=================================================================================================

bool BRepGraph_CacheMesh::isFaceMeshFresh(const CoEdgeMeshEntry& theEntry,
                                          const Slot&            theSlot) const noexcept
{
  if (!theEntry.BoundFaceId.IsValid())
  {
    return true; // no face bound
  }
  if (!theEntry.BoundFaceId.IsValidIn(theSlot.FaceMeshes))
  {
    return false; // face entry doesn't exist
  }
  const FaceMeshEntry& aFaceEntry =
    theSlot.FaceMeshes.Value(static_cast<size_t>(theEntry.BoundFaceId.Index));
  return aFaceEntry.MeshGeneration == theEntry.FaceMeshGeneration;
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::findCoEdgeEntryRaw(
  const SlotId             theSlot,
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theCoEdge.IsValidIn(aSlot->CoEdgeMeshes))
  {
    return nullptr;
  }
  const CoEdgeMeshEntry& anEntry = aSlot->CoEdgeMeshes.Value(static_cast<size_t>(theCoEdge.Index));
  return anEntry.IsPresent() ? &anEntry : nullptr;
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::FindCoEdgePolygon2D(
  const SlotId             theSlot,
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theCoEdge.IsValidIn(aSlot->CoEdgeMeshes))
  {
    return nullptr;
  }
  const CoEdgeMeshEntry& anEntry = aSlot->CoEdgeMeshes.Value(static_cast<size_t>(theCoEdge.Index));
  if (!anEntry.IsPresent() || !isCoEdgePolygon2DFresh(anEntry, *aSlot))
  {
    return nullptr;
  }
  return &anEntry;
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::FindCoEdgePolygonOnTri(
  const SlotId             theSlot,
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theCoEdge.IsValidIn(aSlot->CoEdgeMeshes))
  {
    return nullptr;
  }
  const CoEdgeMeshEntry& anEntry = aSlot->CoEdgeMeshes.Value(static_cast<size_t>(theCoEdge.Index));
  if (!anEntry.IsPresent() || !isCoEdgePolygonOnTriFresh(anEntry, *aSlot))
  {
    return nullptr;
  }
  return &anEntry;
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::FindCoEdgePolygon2D(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return FindCoEdgePolygon2D(DefaultDisplaySlot, theCoEdge);
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::FindCoEdgePolygonOnTri(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return FindCoEdgePolygonOnTri(DefaultDisplaySlot, theCoEdge);
}

//=================================================================================================

BRepGraph_CacheMesh::CoEdgeMeshEntry& BRepGraph_CacheMesh::ChangeCoEdgeMesh(
  const SlotId             theSlot,
  const BRepGraph_CoEdgeId theCoEdge)
{
  Slot& aSlot = changeSlot(theSlot);
  ensureSize(aSlot.CoEdgeMeshes, static_cast<size_t>(theCoEdge.Index));
  return aSlot.CoEdgeMeshes.ChangeValue(static_cast<size_t>(theCoEdge.Index));
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearCoEdgeMesh(const SlotId theSlot, const BRepGraph_CoEdgeId theCoEdge)
{
  Slot* aSlot = const_cast<Slot*>(findSlot(theSlot));
  if (aSlot != nullptr && theCoEdge.IsValidIn(aSlot->CoEdgeMeshes))
  {
    aSlot->CoEdgeMeshes.ChangeValue(static_cast<size_t>(theCoEdge.Index)).Reset();
  }
}

//=================================================================================================

bool BRepGraph_CacheMesh::HasEdgeMesh(const SlotId theSlot, const BRepGraph_EdgeId theEdge) const
{
  return FindEdgeMesh(theSlot, theEdge) != nullptr;
}

//=================================================================================================

const BRepGraph_CacheMesh::EdgeMeshEntry* BRepGraph_CacheMesh::FindEdgeMesh(
  const SlotId           theSlot,
  const BRepGraph_EdgeId theEdge) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theEdge.IsValidIn(aSlot->EdgeMeshes))
  {
    return nullptr;
  }

  const EdgeMeshEntry& anEntry = aSlot->EdgeMeshes.Value(static_cast<size_t>(theEdge.Index));
  if (!anEntry.IsPresent() || !isSlotActual(*aSlot, anEntry.Stamp) || !anEntry.IsFresh(*this))
  {
    return nullptr;
  }
  return &anEntry;
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph_CacheMesh::findCoEdgeMesh(
  const SlotId             theSlot,
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theCoEdge.IsValidIn(aSlot->CoEdgeMeshes))
  {
    return nullptr;
  }

  const CoEdgeMeshEntry& anEntry = aSlot->CoEdgeMeshes.Value(static_cast<size_t>(theCoEdge.Index));
  if (!anEntry.IsPresent() || !isSlotActual(*aSlot, anEntry.SlotStamp)
      || !anEntry.CoEdgeStamp.IsFresh(*this))
  {
    return nullptr;
  }
  return &anEntry;
}

//=================================================================================================

BRepGraph_CacheMesh::EdgeMeshEntry& BRepGraph_CacheMesh::ChangeEdgeMesh(
  const SlotId           theSlot,
  const BRepGraph_EdgeId theEdge)
{
  Slot& aSlot = changeSlot(theSlot);
  ensureSize(aSlot.EdgeMeshes, static_cast<size_t>(theEdge.Index));
  return aSlot.EdgeMeshes.ChangeValue(static_cast<size_t>(theEdge.Index));
}

//=================================================================================================

void BRepGraph_CacheMesh::ClearEdgeMesh(const SlotId theSlot, const BRepGraph_EdgeId theEdge)
{
  Slot* aSlot = const_cast<Slot*>(findSlot(theSlot));
  if (aSlot != nullptr && theEdge.IsValidIn(aSlot->EdgeMeshes))
  {
    aSlot->EdgeMeshes.ChangeValue(static_cast<size_t>(theEdge.Index)).Reset();
  }
}

//=================================================================================================

void BRepGraph_CacheMesh::BumpFaceMeshGeneration(const BRepGraph_FaceId theFace,
                                                 const SlotId           theSlot)
{
  Slot& aSlot = changeSlot(theSlot);
  ensureSize(aSlot.FaceMeshes, static_cast<size_t>(theFace.Index));
  ++aSlot.FaceMeshes.ChangeValue(static_cast<size_t>(theFace.Index)).MeshGeneration;
}

//=================================================================================================

const BRepGraph_CacheMesh::FaceMeshEntry* BRepGraph_CacheMesh::findFaceEntryRaw(
  const SlotId           theSlot,
  const BRepGraph_FaceId theFace) const
{
  const Slot* aSlot = findSlot(theSlot);
  if (aSlot == nullptr || !theFace.IsValidIn(aSlot->FaceMeshes))
  {
    return nullptr;
  }
  const FaceMeshEntry& anEntry = aSlot->FaceMeshes.Value(static_cast<size_t>(theFace.Index));
  return anEntry.IsPresent() ? &anEntry : nullptr;
}

//=================================================================================================

BRepGraph_CacheMesh::DirtySet BRepGraph_CacheMesh::collectDirty(BRepGraph&  theGraph,
                                                                const Slot& theSlot) const
{
  DirtySet aDirtySet;

  for (BRepGraph_FaceId aFaceId = theGraph.Topo().Faces().StartId();
       aFaceId < theGraph.Topo().Faces().EndId();
       ++aFaceId)
  {
    if (aFaceId.IsRemoved(theGraph))
    {
      continue;
    }
    const FaceMeshEntry* anEntry = aFaceId.IsValidIn(theSlot.FaceMeshes)
                                     ? &theSlot.FaceMeshes.Value(static_cast<size_t>(aFaceId.Index))
                                     : nullptr;
    if (anEntry == nullptr || !anEntry->IsPresent() || !isSlotActual(theSlot, anEntry->Stamp)
        || !anEntry->IsFresh(*this))
    {
      aDirtySet.Faces.Append(aFaceId);
    }
  }

  for (BRepGraph_EdgeId anEdgeId = theGraph.Topo().Edges().StartId();
       anEdgeId < theGraph.Topo().Edges().EndId();
       ++anEdgeId)
  {
    if (anEdgeId.IsRemoved(theGraph) || theGraph.Topo().Edges().FacesOf(anEdgeId).More())
    {
      continue;
    }
    const EdgeMeshEntry* anEntry =
      anEdgeId.IsValidIn(theSlot.EdgeMeshes)
        ? &theSlot.EdgeMeshes.Value(static_cast<size_t>(anEdgeId.Index))
        : nullptr;
    if (anEntry == nullptr || !anEntry->IsPresent() || !isSlotActual(theSlot, anEntry->Stamp)
        || !anEntry->IsFresh(*this))
    {
      aDirtySet.FreeEdges.Append(anEdgeId);
    }
  }

  // Check coedges for face-mesh-driven staleness on non-dirty faces.
  for (BRepGraph_FaceId aFaceId = theGraph.Topo().Faces().StartId();
       aFaceId < theGraph.Topo().Faces().EndId();
       ++aFaceId)
  {
    if (aFaceId.IsRemoved(theGraph))
    {
      continue;
    }
    bool aFaceAlreadyDirty = false;
    for (const auto& aDirtyFace : aDirtySet.Faces)
    {
      if (aDirtyFace == aFaceId)
      {
        aFaceAlreadyDirty = true;
        break;
      }
    }
    if (aFaceAlreadyDirty)
    {
      continue;
    }

    for (BRepGraph_ChildExplorer aCoEdgeIt(theGraph,
                                           BRepGraph_NodeId(aFaceId),
                                           BRepGraph_NodeId::Kind::CoEdge,
                                           BRepGraph_ChildExplorer::TraversalMode::Recursive);
         aCoEdgeIt.More();
         aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId(aCoEdgeIt.Current().DefId);
      const CoEdgeMeshEntry*   aRaw = findCoEdgeEntryRaw(theSlot.Id, aCoEdgeId);
      if (aRaw != nullptr && aRaw->IsPresent() && !isCoEdgePolygonOnTriFresh(*aRaw, theSlot))
      {
        aDirtySet.Faces.Append(aFaceId);
        break;
      }
    }
  }

  return aDirtySet;
}

//=================================================================================================

BRepGraph_CacheMesh::DirtySet BRepGraph_CacheMesh::collectDirty(BRepGraph&             theGraph,
                                                                const BRepGraph_NodeId theRoot,
                                                                const Slot& theSlot) const
{
  NCollection_Array1<BRepGraph_NodeId> aNodes(1, 1);
  aNodes.SetValue(1, theRoot);
  return collectDirty(theGraph, aNodes, theSlot);
}

//=================================================================================================

BRepGraph_CacheMesh::DirtySet BRepGraph_CacheMesh::collectDirty(
  BRepGraph&                                  theGraph,
  const NCollection_Array1<BRepGraph_NodeId>& theNodes,
  const Slot&                                 theSlot) const
{
  DirtySet                              aDirtySet;
  NCollection_FlatMap<BRepGraph_FaceId> aFaces;
  NCollection_FlatMap<BRepGraph_EdgeId> aFreeEdges;

  auto addFace = [&](const BRepGraph_FaceId theFace) {
    if (theFace.IsRemoved(theGraph) || !aFaces.Add(theFace))
    {
      return;
    }
    const FaceMeshEntry* anEntry = theFace.IsValidIn(theSlot.FaceMeshes)
                                     ? &theSlot.FaceMeshes.Value(static_cast<size_t>(theFace.Index))
                                     : nullptr;
    if (anEntry == nullptr || !anEntry->IsPresent() || !isSlotActual(theSlot, anEntry->Stamp)
        || !anEntry->IsFresh(*this))
    {
      aDirtySet.Faces.Append(theFace);
    }
  };

  auto addFreeEdge = [&](const BRepGraph_EdgeId theEdge) {
    if (theEdge.IsRemoved(theGraph) || theGraph.Topo().Edges().FacesOf(theEdge).More()
        || !aFreeEdges.Add(theEdge))
    {
      return;
    }
    const EdgeMeshEntry* anEntry = theEdge.IsValidIn(theSlot.EdgeMeshes)
                                     ? &theSlot.EdgeMeshes.Value(static_cast<size_t>(theEdge.Index))
                                     : nullptr;
    if (anEntry == nullptr || !anEntry->IsPresent() || !isSlotActual(theSlot, anEntry->Stamp)
        || !anEntry->IsFresh(*this))
    {
      aDirtySet.FreeEdges.Append(theEdge);
    }
  };

  for (const BRepGraph_NodeId& aNode : theNodes)
  {
    if (!aNode.IsValid())
    {
      continue;
    }

    if (aNode.NodeKind == BRepGraph_NodeId::Kind::Face)
    {
      addFace(BRepGraph_FaceId(aNode));
    }
    else if (aNode.NodeKind == BRepGraph_NodeId::Kind::Edge)
    {
      addFreeEdge(BRepGraph_EdgeId(aNode));
    }

    for (BRepGraph_ChildExplorer aFaceIt(theGraph, aNode, BRepGraph_NodeId::Kind::Face);
         aFaceIt.More();
         aFaceIt.Next())
    {
      addFace(BRepGraph_FaceId(aFaceIt.Current().DefId));
    }
    for (BRepGraph_ChildExplorer anEdgeIt(theGraph, aNode, BRepGraph_NodeId::Kind::Edge);
         anEdgeIt.More();
         anEdgeIt.Next())
    {
      addFreeEdge(BRepGraph_EdgeId(anEdgeIt.Current().DefId));
    }
  }

  // Check coedges of non-dirty faces for face-mesh-driven staleness.
  for (NCollection_FlatMap<BRepGraph_FaceId>::Iterator aFaceIt(aFaces); aFaceIt.More();
       aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId           = aFaceIt.Value();
    bool                   aFaceAlreadyDirty = false;
    for (const auto& aDirtyFace : aDirtySet.Faces)
    {
      if (aDirtyFace == aFaceId)
      {
        aFaceAlreadyDirty = true;
        break;
      }
    }
    if (aFaceAlreadyDirty)
    {
      continue;
    }

    for (BRepGraph_ChildExplorer aCoEdgeIt(theGraph,
                                           BRepGraph_NodeId(aFaceId),
                                           BRepGraph_NodeId::Kind::CoEdge,
                                           BRepGraph_ChildExplorer::TraversalMode::Recursive);
         aCoEdgeIt.More();
         aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId(aCoEdgeIt.Current().DefId);
      const CoEdgeMeshEntry*   aRaw = findCoEdgeEntryRaw(theSlot.Id, aCoEdgeId);
      if (aRaw != nullptr && aRaw->IsPresent() && !isCoEdgePolygonOnTriFresh(*aRaw, theSlot))
      {
        aDirtySet.Faces.Append(aFaceId);
        break;
      }
    }
  }

  return aDirtySet;
}
