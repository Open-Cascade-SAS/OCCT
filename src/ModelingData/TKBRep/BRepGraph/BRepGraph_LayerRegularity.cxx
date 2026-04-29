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

#include <BRepGraph_LayerRegularity.hxx>

#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerRegularity, BRepGraph_Layer)

namespace
{

static const TCollection_AsciiString THE_LAYER_NAME("Regularity");

void appendUnique(
  NCollection_DataMap<BRepGraph_FaceId, NCollection_DynamicArray<BRepGraph_EdgeId>>& theMap,
  const BRepGraph_FaceId                                                             theFace,
  const BRepGraph_EdgeId                                                             theEdge)
{
  if (!theMap.IsBound(theFace))
  {
    NCollection_DynamicArray<BRepGraph_EdgeId> anEdges;
    anEdges.Append(theEdge);
    theMap.Bind(theFace, anEdges);
    return;
  }

  NCollection_DynamicArray<BRepGraph_EdgeId>& anEdges = theMap.ChangeFind(theFace);
  for (const BRepGraph_EdgeId& aEdge : anEdges)
  {
    if (aEdge == theEdge)
    {
      return;
    }
  }
  anEdges.Append(theEdge);
}

void removeEdge(
  NCollection_DataMap<BRepGraph_FaceId, NCollection_DynamicArray<BRepGraph_EdgeId>>& theMap,
  const BRepGraph_FaceId                                                             theFace,
  const BRepGraph_EdgeId theEdge) noexcept
{
  if (!theMap.IsBound(theFace))
  {
    return;
  }

  NCollection_DynamicArray<BRepGraph_EdgeId>& anEdges = theMap.ChangeFind(theFace);
  uint32_t                                    anIdx   = 0;
  for (NCollection_DynamicArray<BRepGraph_EdgeId>::Iterator anIt(anEdges); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() != theEdge)
    {
      continue;
    }
    if (anIdx < static_cast<uint32_t>(anEdges.Size()) - 1u)
    {
      anEdges.ChangeValue(static_cast<size_t>(anIdx)) = anEdges.Value(anEdges.Size() - 1u);
    }
    anEdges.EraseLast();
    break;
  }

  if (anEdges.IsEmpty())
  {
    theMap.UnBind(theFace);
  }
}

static BRepGraph_EdgeId remapEdge(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_EdgeId                                         theEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theEdge);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Edge)
  {
    return BRepGraph_EdgeId();
  }
  return BRepGraph_EdgeId(*aNewId);
}

static BRepGraph_FaceId remapFace(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_FaceId                                         theFace)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theFace);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Face)
  {
    return BRepGraph_FaceId();
  }
  return BRepGraph_FaceId(*aNewId);
}

} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_LayerRegularity::GetID()
{
  static const Standard_GUID THE_LAYER_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10002");
  return THE_LAYER_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerRegularity::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerRegularity::Name() const
{
  return THE_LAYER_NAME;
}

//=================================================================================================

int BRepGraph_LayerRegularity::SubscribedKinds() const
{
  return KindBit(BRepGraph_NodeId::Kind::Edge) | KindBit(BRepGraph_NodeId::Kind::Face);
}

//=================================================================================================

const BRepGraph_LayerRegularity::EdgeRegularities* BRepGraph_LayerRegularity::FindEdgeRegularities(
  const BRepGraph_EdgeId theEdge) const
{
  return myEdgeRegularities.Seek(theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::normalizeFacePair(BRepGraph_FaceId& theFace1,
                                                  BRepGraph_FaceId& theFace2) const noexcept
{
  if (theFace2 < theFace1)
  {
    std::swap(theFace1, theFace2);
  }
}

//=================================================================================================

bool BRepGraph_LayerRegularity::FindContinuity(const BRepGraph_EdgeId theEdge,
                                               const BRepGraph_FaceId theFace1,
                                               const BRepGraph_FaceId theFace2,
                                               GeomAbs_Shape* const   theContinuity) const
{
  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  if (aRegularities == nullptr)
  {
    return false;
  }

  for (const RegularityEntry& anEntry : aRegularities->Entries)
  {
    if (anEntry.FaceEntity1 != aFace1 || anEntry.FaceEntity2 != aFace2)
    {
      continue;
    }
    if (theContinuity != nullptr)
    {
      *theContinuity = anEntry.Continuity;
    }
    return true;
  }
  return false;
}

//=================================================================================================

uint32_t BRepGraph_LayerRegularity::NbRegularities(const BRepGraph_EdgeId theEdge) const
{
  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  return aRegularities == nullptr ? 0u : static_cast<uint32_t>(aRegularities->Entries.Size());
}

//=================================================================================================

GeomAbs_Shape BRepGraph_LayerRegularity::MaxContinuity(const BRepGraph_EdgeId theEdge) const
{
  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  if (aRegularities == nullptr)
  {
    return GeomAbs_C0;
  }

  GeomAbs_Shape aMaxContinuity = GeomAbs_C0;
  for (const RegularityEntry& anEntry : aRegularities->Entries)
  {
    if (anEntry.Continuity > aMaxContinuity)
    {
      aMaxContinuity = anEntry.Continuity;
    }
  }
  return aMaxContinuity;
}

//=================================================================================================

BRepGraph_LayerRegularity::EdgeRegularities& BRepGraph_LayerRegularity::changeEdgeRegularities(
  const BRepGraph_EdgeId theEdge)
{
  if (!myEdgeRegularities.IsBound(theEdge))
  {
    myEdgeRegularities.Bind(theEdge, EdgeRegularities());
  }
  return myEdgeRegularities.ChangeFind(theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::bindFaceToEdge(const BRepGraph_FaceId theFace,
                                               const BRepGraph_EdgeId theEdge)
{
  appendUnique(myFaceToEdges, theFace, theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::unbindFaceFromEdge(const BRepGraph_FaceId theFace,
                                                   const BRepGraph_EdgeId theEdge) noexcept
{
  removeEdge(myFaceToEdges, theFace, theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::SetRegularity(const BRepGraph_EdgeId theEdge,
                                              const BRepGraph_FaceId theFace1,
                                              const BRepGraph_FaceId theFace2,
                                              const GeomAbs_Shape    theContinuity)
{
  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  EdgeRegularities& aRegularities = changeEdgeRegularities(theEdge);
  for (NCollection_DynamicArray<RegularityEntry>::Iterator anIt(aRegularities.Entries); anIt.More();
       anIt.Next())
  {
    if (anIt.Value().FaceEntity1 != aFace1 || anIt.Value().FaceEntity2 != aFace2)
    {
      continue;
    }
    anIt.ChangeValue().Continuity = theContinuity;
    return;
  }

  RegularityEntry& anEntry = aRegularities.Entries.Appended();
  anEntry.FaceEntity1      = aFace1;
  anEntry.FaceEntity2      = aFace2;
  anEntry.Continuity       = theContinuity;
  bindFaceToEdge(aFace1, theEdge);
  bindFaceToEdge(aFace2, theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::removeRegularity(const BRepGraph_EdgeId theEdge,
                                                 const BRepGraph_FaceId theFace1,
                                                 const BRepGraph_FaceId theFace2) noexcept
{
  if (!myEdgeRegularities.IsBound(theEdge))
  {
    return;
  }

  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  EdgeRegularities& aRegularities = myEdgeRegularities.ChangeFind(theEdge);
  bool              aFound        = false;
  uint32_t          anIdx         = 0;
  for (NCollection_DynamicArray<RegularityEntry>::Iterator anIt(aRegularities.Entries); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value().FaceEntity1 != aFace1 || anIt.Value().FaceEntity2 != aFace2)
    {
      continue;
    }
    if (anIdx < static_cast<uint32_t>(aRegularities.Entries.Size()) - 1u)
    {
      aRegularities.Entries.ChangeValue(static_cast<size_t>(anIdx)) =
        aRegularities.Entries.Value(aRegularities.Entries.Size() - 1u);
    }
    aRegularities.Entries.EraseLast();
    aFound = true;
    break;
  }

  if (!aFound)
  {
    return;
  }

  // Only unbind a face if no remaining entry still references it on this edge.
  bool aFace1StillReferenced = false;
  bool aFace2StillReferenced = false;
  for (const RegularityEntry& anEntry : aRegularities.Entries)
  {
    if (anEntry.FaceEntity1 == aFace1 || anEntry.FaceEntity2 == aFace1)
    {
      aFace1StillReferenced = true;
    }
    if (anEntry.FaceEntity1 == aFace2 || anEntry.FaceEntity2 == aFace2)
    {
      aFace2StillReferenced = true;
    }
  }
  if (!aFace1StillReferenced)
  {
    unbindFaceFromEdge(aFace1, theEdge);
  }
  if (!aFace2StillReferenced)
  {
    unbindFaceFromEdge(aFace2, theEdge);
  }

  if (aRegularities.IsEmpty())
  {
    myEdgeRegularities.UnBind(theEdge);
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::removeEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept
{
  const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(theEdge);
  if (aRegularities == nullptr)
  {
    return;
  }

  for (const RegularityEntry& anEntry : aRegularities->Entries)
  {
    unbindFaceFromEdge(anEntry.FaceEntity1, theEdge);
    unbindFaceFromEdge(anEntry.FaceEntity2, theEdge);
  }
  myEdgeRegularities.UnBind(theEdge);
}

//=================================================================================================

void BRepGraph_LayerRegularity::invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept
{
  const NCollection_DynamicArray<BRepGraph_EdgeId>* anEdges = myFaceToEdges.Seek(theFace);
  if (anEdges == nullptr)
  {
    return;
  }

  const NCollection_DynamicArray<BRepGraph_EdgeId> aBoundEdges = *anEdges;
  for (const BRepGraph_EdgeId& aEdgeId : aBoundEdges)
  {
    const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(aEdgeId);
    if (aRegularities == nullptr)
    {
      continue;
    }
    const EdgeRegularities aEntries = *aRegularities;
    for (const RegularityEntry& anEntry : aEntries.Entries)
    {
      if (anEntry.FaceEntity1 == theFace || anEntry.FaceEntity2 == theFace)
      {
        removeRegularity(aEdgeId, anEntry.FaceEntity1, anEntry.FaceEntity2);
      }
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                                                    const BRepGraph_EdgeId theNewEdge) noexcept
{
  const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(theOldEdge);
  if (aRegularities == nullptr)
  {
    return;
  }

  const EdgeRegularities anOldRegularities = *aRegularities;
  removeEdgeBindings(theOldEdge);
  for (const RegularityEntry& anEntry : anOldRegularities.Entries)
  {
    SetRegularity(theNewEdge, anEntry.FaceEntity1, anEntry.FaceEntity2, anEntry.Continuity);
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                                                    const BRepGraph_FaceId theNewFace) noexcept
{
  const NCollection_DynamicArray<BRepGraph_EdgeId>* anEdges = myFaceToEdges.Seek(theOldFace);
  if (anEdges == nullptr)
  {
    return;
  }

  const NCollection_DynamicArray<BRepGraph_EdgeId> aBoundEdges = *anEdges;
  for (const BRepGraph_EdgeId& aEdgeId : aBoundEdges)
  {
    const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(aEdgeId);
    if (aRegularities == nullptr)
    {
      continue;
    }
    const EdgeRegularities aEntries = *aRegularities;
    for (const RegularityEntry& anEntry : aEntries.Entries)
    {
      if (anEntry.FaceEntity1 != theOldFace && anEntry.FaceEntity2 != theOldFace)
      {
        continue;
      }
      removeRegularity(aEdgeId, anEntry.FaceEntity1, anEntry.FaceEntity2);
      const BRepGraph_FaceId aFace1 =
        anEntry.FaceEntity1 == theOldFace ? theNewFace : anEntry.FaceEntity1;
      const BRepGraph_FaceId aFace2 =
        anEntry.FaceEntity2 == theOldFace ? theNewFace : anEntry.FaceEntity2;
      SetRegularity(aEdgeId, aFace1, aFace2, anEntry.Continuity);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::OnNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge:
      removeEdgeBindings(BRepGraph_EdgeId(theNode));
      break;
    case BRepGraph_NodeId::Kind::Face:
      invalidateFaceBindings(BRepGraph_FaceId(theNode));
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::OnNodesModified(
  const NCollection_DynamicArray<BRepGraph_NodeId>& theModifiedNodes) noexcept
{
  for (const BRepGraph_NodeId& aModifiedNode : theModifiedNodes)
  {
    OnNodeModified(aModifiedNode);
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::OnNodeRemoved(const BRepGraph_NodeId theNode,
                                              const BRepGraph_NodeId theReplacement) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid())
      {
        migrateEdgeBindings(BRepGraph_EdgeId(theNode), BRepGraph_EdgeId(theReplacement));
      }
      else
      {
        removeEdgeBindings(BRepGraph_EdgeId(theNode));
      }
      break;
    case BRepGraph_NodeId::Kind::Face:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Face && theReplacement.IsValid())
      {
        migrateFaceBindings(BRepGraph_FaceId(theNode), BRepGraph_FaceId(theReplacement));
      }
      else
      {
        invalidateFaceBindings(BRepGraph_FaceId(theNode));
      }
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_LayerRegularity::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  NCollection_DataMap<BRepGraph_EdgeId, EdgeRegularities>                           aNewEdgeRegs;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_DynamicArray<BRepGraph_EdgeId>> aNewFaceToEdges;

  for (const auto& [aOldEdge, aOldRegularities] : myEdgeRegularities.Items())
  {
    const BRepGraph_EdgeId aNewEdge = remapEdge(theRemapMap, aOldEdge);
    if (!aNewEdge.IsValid())
    {
      continue;
    }
    for (const RegularityEntry& anOldEntry : aOldRegularities.Entries)
    {
      BRepGraph_FaceId aNewFace1 = remapFace(theRemapMap, anOldEntry.FaceEntity1);
      BRepGraph_FaceId aNewFace2 = remapFace(theRemapMap, anOldEntry.FaceEntity2);
      if (!aNewFace1.IsValid() || !aNewFace2.IsValid())
      {
        continue;
      }
      if (aNewFace2 < aNewFace1)
      {
        std::swap(aNewFace1, aNewFace2);
      }

      if (!aNewEdgeRegs.IsBound(aNewEdge))
      {
        aNewEdgeRegs.Bind(aNewEdge, EdgeRegularities());
      }
      EdgeRegularities& aRegularities = aNewEdgeRegs.ChangeFind(aNewEdge);

      // Deduplicate: if same face pair already exists for this edge, update continuity.
      bool aDuplicate = false;
      for (NCollection_DynamicArray<RegularityEntry>::Iterator anIt(aRegularities.Entries);
           anIt.More();
           anIt.Next())
      {
        if (anIt.Value().FaceEntity1 == aNewFace1 && anIt.Value().FaceEntity2 == aNewFace2)
        {
          anIt.ChangeValue().Continuity = anOldEntry.Continuity;
          aDuplicate                    = true;
          break;
        }
      }
      if (aDuplicate)
      {
        continue;
      }

      RegularityEntry& anEntry = aRegularities.Entries.Appended();
      anEntry.FaceEntity1      = aNewFace1;
      anEntry.FaceEntity2      = aNewFace2;
      anEntry.Continuity       = anOldEntry.Continuity;

      appendUnique(aNewFaceToEdges, aNewFace1, aNewEdge);
      appendUnique(aNewFaceToEdges, aNewFace2, aNewEdge);
    }
  }

  myEdgeRegularities = std::move(aNewEdgeRegs);
  myFaceToEdges      = std::move(aNewFaceToEdges);
}

//=================================================================================================

void BRepGraph_LayerRegularity::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerRegularity::Clear() noexcept
{
  myEdgeRegularities.Clear();
  myFaceToEdges.Clear();
}
