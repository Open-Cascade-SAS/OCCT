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

#include <BRepGraph_RegularityLayer.hxx>

#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_RegularityLayer, BRepGraph_Layer)

namespace
{

static const TCollection_AsciiString THE_LAYER_NAME("Regularity");

void appendUnique(
  NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_EdgeId>>& theMap,
  const BRepGraph_FaceId                                                       theFace,
  const BRepGraph_EdgeId                                                       theEdge)
{
  if (!theMap.IsBound(theFace))
  {
    NCollection_Vector<BRepGraph_EdgeId> anEdges;
    anEdges.Append(theEdge);
    theMap.Bind(theFace, anEdges);
    return;
  }

  NCollection_Vector<BRepGraph_EdgeId>& anEdges = theMap.ChangeFind(theFace);
  for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
  {
    if (anEdges.Value(anIdx) == theEdge)
      return;
  }
  anEdges.Append(theEdge);
}

void removeEdge(NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_EdgeId>>& theMap,
                const BRepGraph_FaceId theFace,
                const BRepGraph_EdgeId theEdge) noexcept
{
  if (!theMap.IsBound(theFace))
    return;

  NCollection_Vector<BRepGraph_EdgeId>& anEdges = theMap.ChangeFind(theFace);
  for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
  {
    if (anEdges.Value(anIdx) != theEdge)
      continue;
    if (anIdx < anEdges.Length() - 1)
      anEdges.ChangeValue(anIdx) = anEdges.Value(anEdges.Length() - 1);
    anEdges.EraseLast();
    break;
  }

  if (anEdges.IsEmpty())
    theMap.UnBind(theFace);
}

static BRepGraph_EdgeId remapEdge(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_EdgeId                                         theEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_EdgeId(theEdge.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Edge)
    return BRepGraph_EdgeId();
  return BRepGraph_EdgeId(aNewId->Index);
}

static BRepGraph_FaceId remapFace(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_FaceId                                         theFace)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_FaceId(theFace.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Face)
    return BRepGraph_FaceId();
  return BRepGraph_FaceId(aNewId->Index);
}

} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_RegularityLayer::GetID()
{
  static const Standard_GUID THE_LAYER_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10002");
  return THE_LAYER_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_RegularityLayer::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_RegularityLayer::Name() const
{
  return THE_LAYER_NAME;
}

//=================================================================================================

int BRepGraph_RegularityLayer::SubscribedKinds() const
{
  return KindBit(BRepGraph_NodeId::Kind::Edge) | KindBit(BRepGraph_NodeId::Kind::Face);
}

//=================================================================================================

const BRepGraph_RegularityLayer::EdgeRegularities* BRepGraph_RegularityLayer::FindEdgeRegularities(
  const BRepGraph_EdgeId theEdge) const
{
  return myEdgeRegularities.Seek(theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::normalizeFacePair(BRepGraph_FaceId& theFace1,
                                                  BRepGraph_FaceId& theFace2) const noexcept
{
  if (theFace2.Index < theFace1.Index)
    std::swap(theFace1, theFace2);
}

//=================================================================================================

bool BRepGraph_RegularityLayer::FindContinuity(const BRepGraph_EdgeId theEdge,
                                               const BRepGraph_FaceId theFace1,
                                               const BRepGraph_FaceId theFace2,
                                               GeomAbs_Shape* const   theContinuity) const
{
  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  if (aRegularities == nullptr)
    return false;

  for (int anIdx = 0; anIdx < aRegularities->Entries.Length(); ++anIdx)
  {
    const RegularityEntry& anEntry = aRegularities->Entries.Value(anIdx);
    if (anEntry.FaceEntity1 != aFace1 || anEntry.FaceEntity2 != aFace2)
      continue;
    if (theContinuity != nullptr)
      *theContinuity = anEntry.Continuity;
    return true;
  }
  return false;
}

//=================================================================================================

int BRepGraph_RegularityLayer::NbRegularities(const BRepGraph_EdgeId theEdge) const
{
  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  return aRegularities == nullptr ? 0 : aRegularities->Entries.Length();
}

//=================================================================================================

GeomAbs_Shape BRepGraph_RegularityLayer::MaxContinuity(const BRepGraph_EdgeId theEdge) const
{
  const EdgeRegularities* aRegularities = FindEdgeRegularities(theEdge);
  if (aRegularities == nullptr)
    return GeomAbs_C0;

  GeomAbs_Shape aMaxContinuity = GeomAbs_C0;
  for (int anIdx = 0; anIdx < aRegularities->Entries.Length(); ++anIdx)
  {
    if (aRegularities->Entries.Value(anIdx).Continuity > aMaxContinuity)
      aMaxContinuity = aRegularities->Entries.Value(anIdx).Continuity;
  }
  return aMaxContinuity;
}

//=================================================================================================

BRepGraph_RegularityLayer::EdgeRegularities& BRepGraph_RegularityLayer::changeEdgeRegularities(
  const BRepGraph_EdgeId theEdge)
{
  if (!myEdgeRegularities.IsBound(theEdge))
    myEdgeRegularities.Bind(theEdge, EdgeRegularities());
  return myEdgeRegularities.ChangeFind(theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::bindFaceToEdge(const BRepGraph_FaceId theFace,
                                               const BRepGraph_EdgeId theEdge)
{
  appendUnique(myFaceToEdges, theFace, theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::unbindFaceFromEdge(const BRepGraph_FaceId theFace,
                                                   const BRepGraph_EdgeId theEdge) noexcept
{
  removeEdge(myFaceToEdges, theFace, theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::SetRegularity(const BRepGraph_EdgeId theEdge,
                                              const BRepGraph_FaceId theFace1,
                                              const BRepGraph_FaceId theFace2,
                                              const GeomAbs_Shape    theContinuity)
{
  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  EdgeRegularities& aRegularities = changeEdgeRegularities(theEdge);
  for (int anIdx = 0; anIdx < aRegularities.Entries.Length(); ++anIdx)
  {
    RegularityEntry& anEntry = aRegularities.Entries.ChangeValue(anIdx);
    if (anEntry.FaceEntity1 != aFace1 || anEntry.FaceEntity2 != aFace2)
      continue;
    anEntry.Continuity = theContinuity;
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

void BRepGraph_RegularityLayer::removeRegularity(const BRepGraph_EdgeId theEdge,
                                                 const BRepGraph_FaceId theFace1,
                                                 const BRepGraph_FaceId theFace2) noexcept
{
  if (!myEdgeRegularities.IsBound(theEdge))
    return;

  BRepGraph_FaceId aFace1 = theFace1;
  BRepGraph_FaceId aFace2 = theFace2;
  normalizeFacePair(aFace1, aFace2);

  EdgeRegularities& aRegularities = myEdgeRegularities.ChangeFind(theEdge);
  bool              aFound        = false;
  for (int anIdx = 0; anIdx < aRegularities.Entries.Length(); ++anIdx)
  {
    const RegularityEntry& anEntry = aRegularities.Entries.Value(anIdx);
    if (anEntry.FaceEntity1 != aFace1 || anEntry.FaceEntity2 != aFace2)
      continue;
    if (anIdx < aRegularities.Entries.Length() - 1)
      aRegularities.Entries.ChangeValue(anIdx) =
        aRegularities.Entries.Value(aRegularities.Entries.Length() - 1);
    aRegularities.Entries.EraseLast();
    aFound = true;
    break;
  }

  if (!aFound)
    return;

  // Only unbind a face if no remaining entry still references it on this edge.
  bool aFace1StillReferenced = false;
  bool aFace2StillReferenced = false;
  for (int anIdx = 0; anIdx < aRegularities.Entries.Length(); ++anIdx)
  {
    const RegularityEntry& anEntry = aRegularities.Entries.Value(anIdx);
    if (anEntry.FaceEntity1 == aFace1 || anEntry.FaceEntity2 == aFace1)
      aFace1StillReferenced = true;
    if (anEntry.FaceEntity1 == aFace2 || anEntry.FaceEntity2 == aFace2)
      aFace2StillReferenced = true;
  }
  if (!aFace1StillReferenced)
    unbindFaceFromEdge(aFace1, theEdge);
  if (!aFace2StillReferenced)
    unbindFaceFromEdge(aFace2, theEdge);

  if (aRegularities.IsEmpty())
    myEdgeRegularities.UnBind(theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::removeEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept
{
  const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(theEdge);
  if (aRegularities == nullptr)
    return;

  for (int anIdx = 0; anIdx < aRegularities->Entries.Length(); ++anIdx)
  {
    const RegularityEntry& anEntry = aRegularities->Entries.Value(anIdx);
    unbindFaceFromEdge(anEntry.FaceEntity1, theEdge);
    unbindFaceFromEdge(anEntry.FaceEntity2, theEdge);
  }
  myEdgeRegularities.UnBind(theEdge);
}

//=================================================================================================

void BRepGraph_RegularityLayer::invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept
{
  const NCollection_Vector<BRepGraph_EdgeId>* anEdges = myFaceToEdges.Seek(theFace);
  if (anEdges == nullptr)
    return;

  const NCollection_Vector<BRepGraph_EdgeId> aBoundEdges = *anEdges;
  for (int anIdx = 0; anIdx < aBoundEdges.Length(); ++anIdx)
  {
    const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(aBoundEdges.Value(anIdx));
    if (aRegularities == nullptr)
      continue;
    const EdgeRegularities aEntries = *aRegularities;
    for (int aRegIdx = 0; aRegIdx < aEntries.Entries.Length(); ++aRegIdx)
    {
      const RegularityEntry& anEntry = aEntries.Entries.Value(aRegIdx);
      if (anEntry.FaceEntity1 == theFace || anEntry.FaceEntity2 == theFace)
        removeRegularity(aBoundEdges.Value(anIdx), anEntry.FaceEntity1, anEntry.FaceEntity2);
    }
  }
}

//=================================================================================================

void BRepGraph_RegularityLayer::migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                                                    const BRepGraph_EdgeId theNewEdge) noexcept
{
  const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(theOldEdge);
  if (aRegularities == nullptr)
    return;

  const EdgeRegularities anOldRegularities = *aRegularities;
  removeEdgeBindings(theOldEdge);
  for (int anIdx = 0; anIdx < anOldRegularities.Entries.Length(); ++anIdx)
  {
    const RegularityEntry& anEntry = anOldRegularities.Entries.Value(anIdx);
    SetRegularity(theNewEdge, anEntry.FaceEntity1, anEntry.FaceEntity2, anEntry.Continuity);
  }
}

//=================================================================================================

void BRepGraph_RegularityLayer::migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                                                    const BRepGraph_FaceId theNewFace) noexcept
{
  const NCollection_Vector<BRepGraph_EdgeId>* anEdges = myFaceToEdges.Seek(theOldFace);
  if (anEdges == nullptr)
    return;

  const NCollection_Vector<BRepGraph_EdgeId> aBoundEdges = *anEdges;
  for (int anIdx = 0; anIdx < aBoundEdges.Length(); ++anIdx)
  {
    const EdgeRegularities* aRegularities = myEdgeRegularities.Seek(aBoundEdges.Value(anIdx));
    if (aRegularities == nullptr)
      continue;
    const EdgeRegularities aEntries = *aRegularities;
    for (int aRegIdx = 0; aRegIdx < aEntries.Entries.Length(); ++aRegIdx)
    {
      const RegularityEntry& anEntry = aEntries.Entries.Value(aRegIdx);
      if (anEntry.FaceEntity1 != theOldFace && anEntry.FaceEntity2 != theOldFace)
        continue;
      removeRegularity(aBoundEdges.Value(anIdx), anEntry.FaceEntity1, anEntry.FaceEntity2);
      const BRepGraph_FaceId aFace1 =
        anEntry.FaceEntity1 == theOldFace ? theNewFace : anEntry.FaceEntity1;
      const BRepGraph_FaceId aFace2 =
        anEntry.FaceEntity2 == theOldFace ? theNewFace : anEntry.FaceEntity2;
      SetRegularity(aBoundEdges.Value(anIdx), aFace1, aFace2, anEntry.Continuity);
    }
  }
}

//=================================================================================================

void BRepGraph_RegularityLayer::OnNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge:
      removeEdgeBindings(BRepGraph_EdgeId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Face:
      invalidateFaceBindings(BRepGraph_FaceId(theNode.Index));
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_RegularityLayer::OnNodesModified(
  const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes) noexcept
{
  for (int anIdx = 0; anIdx < theModifiedNodes.Length(); ++anIdx)
    OnNodeModified(theModifiedNodes.Value(anIdx));
}

//=================================================================================================

void BRepGraph_RegularityLayer::OnNodeRemoved(const BRepGraph_NodeId theNode,
                                              const BRepGraph_NodeId theReplacement) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid())
        migrateEdgeBindings(BRepGraph_EdgeId(theNode.Index),
                            BRepGraph_EdgeId(theReplacement.Index));
      else
        removeEdgeBindings(BRepGraph_EdgeId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Face:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Face && theReplacement.IsValid())
        migrateFaceBindings(BRepGraph_FaceId(theNode.Index),
                            BRepGraph_FaceId(theReplacement.Index));
      else
        invalidateFaceBindings(BRepGraph_FaceId(theNode.Index));
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_RegularityLayer::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  NCollection_DataMap<BRepGraph_EdgeId, EdgeRegularities>                     aNewEdgeRegs;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_EdgeId>> aNewFaceToEdges;

  for (NCollection_DataMap<BRepGraph_EdgeId, EdgeRegularities>::Iterator anIter(myEdgeRegularities);
       anIter.More();
       anIter.Next())
  {
    const BRepGraph_EdgeId aNewEdge = remapEdge(theRemapMap, anIter.Key());
    if (!aNewEdge.IsValid())
      continue;

    const EdgeRegularities& aOldRegularities = anIter.Value();
    for (int anIdx = 0; anIdx < aOldRegularities.Entries.Length(); ++anIdx)
    {
      const RegularityEntry& anOldEntry = aOldRegularities.Entries.Value(anIdx);
      BRepGraph_FaceId       aNewFace1  = remapFace(theRemapMap, anOldEntry.FaceEntity1);
      BRepGraph_FaceId       aNewFace2  = remapFace(theRemapMap, anOldEntry.FaceEntity2);
      if (!aNewFace1.IsValid() || !aNewFace2.IsValid())
        continue;
      if (aNewFace2.Index < aNewFace1.Index)
        std::swap(aNewFace1, aNewFace2);

      if (!aNewEdgeRegs.IsBound(aNewEdge))
        aNewEdgeRegs.Bind(aNewEdge, EdgeRegularities());
      EdgeRegularities& aRegularities = aNewEdgeRegs.ChangeFind(aNewEdge);

      // Deduplicate: if same face pair already exists for this edge, update continuity.
      bool aDuplicate = false;
      for (int aExIdx = 0; aExIdx < aRegularities.Entries.Length(); ++aExIdx)
      {
        RegularityEntry& anExisting = aRegularities.Entries.ChangeValue(aExIdx);
        if (anExisting.FaceEntity1 == aNewFace1 && anExisting.FaceEntity2 == aNewFace2)
        {
          anExisting.Continuity = anOldEntry.Continuity;
          aDuplicate            = true;
          break;
        }
      }
      if (aDuplicate)
        continue;

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

void BRepGraph_RegularityLayer::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_RegularityLayer::Clear() noexcept
{
  myEdgeRegularities.Clear();
  myFaceToEdges.Clear();
}
