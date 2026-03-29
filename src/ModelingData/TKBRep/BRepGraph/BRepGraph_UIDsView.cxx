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

void appendUIDReverseIndex(BRepGraph_Data&               theData,
                           const BRepGraph_NodeId::Kind  theKind)
{
  const NCollection_Vector<BRepGraph_UID>& aUIDs = theData.myIncStorage.UIDs(theKind);
  for (int anIdx = 0; anIdx < aUIDs.Length(); ++anIdx)
  {
    const BRepGraph_UID aUID = aUIDs.Value(anIdx);
    if (aUID.IsValid())
    {
      theData.myUIDToNodeId.Bind(aUID, BRepGraph_NodeId(theKind, anIdx));
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

} // namespace

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_UID();

  const NCollection_Vector<BRepGraph_UID>& aVec =
    myGraph->myData->myIncStorage.UIDs(theNode.NodeKind);
  if (theNode.Index >= aVec.Length())
    return BRepGraph_UID();
  return aVec.Value(theNode.Index);
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
  return aNodeId != nullptr ? *aNodeId : BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return false;

  BRepGraph_Data& aData = *myGraph->myData;
  ensureUIDReverseIndex(aData);

  std::shared_lock<std::shared_mutex> aReadLock(aData.myUIDToNodeIdMutex);
  return aData.myUIDToNodeId.Seek(theUID) != nullptr;
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
  if (theNode.Index >= aVec.Length())
    return BRepGraph_VersionStamp();

  const BRepGraph_UID          aUID = aVec.Value(theNode.Index);
  const BRepGraphInc::BaseDef* aDef = myGraph->TopoEntity(theNode);
  if (aDef == nullptr || aDef->IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUID, aDef->OwnGen, myGraph->myData->myGeneration.load());
}

//=================================================================================================

bool BRepGraph::UIDsView::IsStale(const BRepGraph_VersionStamp& theStamp) const
{
  if (!theStamp.IsValid())
    return true;

  if (!theStamp.IsEntityStamp())
    return true;

  if (theStamp.myGeneration != myGraph->myData->myGeneration.load())
    return true;

  const BRepGraph_NodeId aNodeId = NodeIdFrom(theStamp.myUID);
  if (!aNodeId.IsValid())
    return true;

  const BRepGraphInc::BaseDef* aDef = myGraph->TopoEntity(aNodeId);
  if (aDef == nullptr || aDef->IsRemoved)
    return true;

  return aDef->OwnGen != theStamp.myMutationGen;
}
