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

#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Data.hxx>

#include <shared_mutex>

namespace
{

//=================================================================================================

void appendRefUIDReverseIndex(BRepGraph_Data& theData, const BRepGraph_RefId::Kind theKind)
{
  const NCollection_Vector<BRepGraph_RefUID>& aUIDs = theData.myIncStorage.RefUIDs(theKind);
  for (BRepGraph_RefId aRefId = BRepGraph_RefId::Start(theKind); aRefId.IsValidIn(aUIDs); ++aRefId)
  {
    const BRepGraph_RefUID aUID = aUIDs.Value(aRefId.Index);
    if (aUID.IsValid())
    {
      theData.myRefUIDToRefId.Bind(aUID, aRefId);
    }
  }
}

//=================================================================================================

void appendUIDReverseIndex(BRepGraph_Data& theData, const BRepGraph_NodeId::Kind theKind)
{
  const NCollection_Vector<BRepGraph_UID>& aUIDs = theData.myIncStorage.UIDs(theKind);
  for (BRepGraph_NodeId aNodeId = BRepGraph_NodeId::Start(theKind); aNodeId.IsValidIn(aUIDs);
       ++aNodeId)
  {
    const BRepGraph_UID aUID = aUIDs.Value(aNodeId.Index);
    if (aUID.IsValid())
    {
      theData.myUIDToNodeId.Bind(aUID, aNodeId);
    }
  }
}

//=================================================================================================

void ensureUIDReverseIndex(BRepGraph_Data& theData)
{
  const uint32_t aGeneration = theData.myGeneration.load();
  {
    std::shared_lock<std::shared_mutex> aReadLock(theData.myUIDToNodeIdMutex);
    if (!theData.myUIDToNodeIdDirty && theData.myUIDToNodeIdGeneration == aGeneration)
    {
      return;
    }
  }

  std::unique_lock<std::shared_mutex> aWriteLock(theData.myUIDToNodeIdMutex);
  if (!theData.myUIDToNodeIdDirty && theData.myUIDToNodeIdGeneration == aGeneration)
  {
    return;
  }

  theData.myUIDToNodeId.Clear();
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Vertex);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Edge);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::CoEdge);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Wire);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Face);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Shell);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Solid);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Compound);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::CompSolid);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Product);
  appendUIDReverseIndex(theData, BRepGraph_NodeId::Kind::Occurrence);
  theData.myUIDToNodeIdGeneration = aGeneration;
  theData.myUIDToNodeIdDirty      = false;
}

//=================================================================================================

void ensureRefUIDReverseIndex(BRepGraph_Data& theData)
{
  const uint32_t aGeneration = theData.myGeneration.load();
  {
    std::shared_lock<std::shared_mutex> aReadLock(theData.myRefUIDToRefIdMutex);
    if (!theData.myRefUIDToRefIdDirty && theData.myRefUIDToRefIdGeneration == aGeneration)
    {
      return;
    }
  }

  std::unique_lock<std::shared_mutex> aWriteLock(theData.myRefUIDToRefIdMutex);
  if (!theData.myRefUIDToRefIdDirty && theData.myRefUIDToRefIdGeneration == aGeneration)
  {
    return;
  }

  theData.myRefUIDToRefId.Clear();
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Shell);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Face);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Wire);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::CoEdge);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Vertex);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Solid);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Child);
  appendRefUIDReverseIndex(theData, BRepGraph_RefId::Kind::Occurrence);
  theData.myRefUIDToRefIdGeneration = aGeneration;
  theData.myRefUIDToRefIdDirty      = false;
}

} // namespace

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_UID();

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr || aDef->IsRemoved)
    return BRepGraph_UID();

  const NCollection_Vector<BRepGraph_UID>& aVec =
    myGraph->myData->myIncStorage.UIDs(theNode.NodeKind);
  if (!theNode.IsValidIn(aVec))
    return BRepGraph_UID();
  return aVec.Value(theNode.Index);
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::UIDsView::Of(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_RefUID();

  const NCollection_Vector<BRepGraph_RefUID>& aVec =
    myGraph->myData->myIncStorage.RefUIDs(theRefId.RefKind);
  if (!theRefId.IsValidIn(aVec))
    return BRepGraph_RefUID();

  const BRepGraphInc::BaseRef& aBase = myGraph->myData->myIncStorage.BaseRef(theRefId);
  if (aBase.IsRemoved)
    return BRepGraph_RefUID();

  return aVec.Value(theRefId.Index);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_NodeId();
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return BRepGraph_NodeId();

  BRepGraph_Data& aData = *myGraph->myData;
  ensureUIDReverseIndex(aData);

  std::shared_lock<std::shared_mutex> aReadLock(aData.myUIDToNodeIdMutex);
  const BRepGraph_NodeId*             aNodeId = aData.myUIDToNodeId.Seek(theUID);
  if (aNodeId == nullptr)
    return BRepGraph_NodeId();

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(*aNodeId);
  if (aDef == nullptr || aDef->IsRemoved)
    return BRepGraph_NodeId();

  return *aNodeId;
}

//=================================================================================================

BRepGraph_RefId BRepGraph::UIDsView::RefIdFrom(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_RefId();
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return BRepGraph_RefId();

  BRepGraph_Data& aData = *myGraph->myData;
  ensureRefUIDReverseIndex(aData);

  std::shared_lock<std::shared_mutex> aReadLock(aData.myRefUIDToRefIdMutex);
  const BRepGraph_RefId*              aRefId = aData.myRefUIDToRefId.Seek(theUID);
  if (aRefId == nullptr)
    return BRepGraph_RefId();

  const BRepGraphInc::BaseRef& aBase = myGraph->myData->myIncStorage.BaseRef(*aRefId);
  if (aBase.IsRemoved)
    return BRepGraph_RefId();

  return *aRefId;
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  return NodeIdFrom(theUID).IsValid();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_RefUID& theUID) const
{
  return RefIdFrom(theUID).IsValid();
}

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const
{
  return myGraph->myData->myGeneration.load();
}

//=================================================================================================

const Standard_GUID& BRepGraph::UIDsView::GraphGUID() const
{
  return myGraph->myData->myGraphGUID;
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_VersionStamp();

  const NCollection_Vector<BRepGraph_UID>& aVec =
    myGraph->myData->myIncStorage.UIDs(theNode.NodeKind);
  if (!theNode.IsValidIn(aVec))
    return BRepGraph_VersionStamp();

  const BRepGraph_UID          aUID = aVec.Value(theNode.Index);
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr || aDef->IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUID, aDef->OwnGen, myGraph->myData->myGeneration.load());
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
    return BRepGraph_VersionStamp();

  const NCollection_Vector<BRepGraph_RefUID>& aUIDs =
    myGraph->myData->myIncStorage.RefUIDs(theRefId.RefKind);
  if (!theRefId.IsValidIn(aUIDs))
    return BRepGraph_VersionStamp();

  const BRepGraphInc::BaseRef& aBase = myGraph->myData->myIncStorage.BaseRef(theRefId);
  if (aBase.IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUIDs.Value(theRefId.Index),
                                aBase.OwnGen,
                                myGraph->myData->myGeneration.load());
}

//=================================================================================================

bool BRepGraph::UIDsView::IsStale(const BRepGraph_VersionStamp& theStamp) const
{
  if (!theStamp.IsValid())
    return true;

  if (theStamp.myGeneration != myGraph->myData->myGeneration.load())
    return true;

  if (theStamp.IsEntityStamp())
  {
    const BRepGraph_NodeId aNodeId = NodeIdFrom(theStamp.myUID);
    if (!aNodeId.IsValid())
      return true;

    const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(aNodeId);
    if (aDef == nullptr || aDef->IsRemoved)
      return true;

    return aDef->OwnGen != theStamp.myMutationGen;
  }

  if (theStamp.IsRefStamp())
  {
    const BRepGraph_RefId aRefId = RefIdFrom(theStamp.myRefUID);
    if (!aRefId.IsValid())
      return true;

    const BRepGraphInc::BaseRef& aRef = myGraph->myData->myIncStorage.BaseRef(aRefId);
    if (aRef.IsRemoved)
      return true;

    return aRef.OwnGen != theStamp.myMutationGen;
  }

  return true;
}
