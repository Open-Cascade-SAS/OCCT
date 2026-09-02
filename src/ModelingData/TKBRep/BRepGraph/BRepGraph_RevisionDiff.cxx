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

#include <BRepGraph_Revision.hxx>

#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RevisionHash.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>

#include <algorithm>

namespace
{

struct NodeInfo
{
  BRepGraph_NodeId       Id;
  BRepGraph_RevisionHash Signature;
};

struct RefInfo
{
  BRepGraph_RefId        Id;
  BRepGraph_RevisionHash Signature;
};

template <typename TheDefinition>
void collectNodes(const BRepGraph& theGraph, NCollection_DataMap<BRepGraph_UID, NodeInfo>& theNodes)
{
  for (BRepGraph_Iterator<TheDefinition> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_NodeId anId = anIt.CurrentId();
    const BRepGraph_UID    aUID = theGraph.UIDs().Of(anId);
    if (aUID.IsValid())
    {
      theNodes.Bind(aUID, {anId, BRepGraph_RevisionHash::Hasher::Node(theGraph, anId)});
    }
  }
}

void collectReferences(const BRepGraph&                                theGraph,
                       NCollection_DataMap<BRepGraph_RefUID, RefInfo>& theRefs)
{
  for (BRepGraph_ShellRefId anId = theGraph.Refs().Shells().StartId();
       anId < theGraph.Refs().Shells().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_FaceRefId anId = theGraph.Refs().Faces().StartId();
       anId < theGraph.Refs().Faces().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_WireRefId anId = theGraph.Refs().Wires().StartId();
       anId < theGraph.Refs().Wires().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_VertexRefId anId = theGraph.Refs().Vertices().StartId();
       anId < theGraph.Refs().Vertices().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_SolidRefId anId = theGraph.Refs().Solids().StartId();
       anId < theGraph.Refs().Solids().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_ChildRefId anId = theGraph.Refs().Children().StartId();
       anId < theGraph.Refs().Children().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
  for (BRepGraph_OccurrenceRefId anId = theGraph.Refs().Occurrences().StartId();
       anId < theGraph.Refs().Occurrences().EndId();
       ++anId)
  {
    if (theGraph.Refs().Gen().IsActive(anId))
      theRefs.Bind(theGraph.UIDs().Of(anId),
                   {anId, BRepGraph_RevisionHash::Hasher::Reference(theGraph, anId)});
  }
}

template <typename TheMapType, typename TheKeyType>
bool contains(const TheMapType& theMap, const TheKeyType& theKey)
{
  return theMap.Seek(theKey) != nullptr;
}

void sortChanges(BRepGraph_RevisionDiff& theDiff)
{
  std::sort(theDiff.CreatedUIDs.begin(), theDiff.CreatedUIDs.end());
  std::sort(theDiff.ModifiedUIDs.begin(), theDiff.ModifiedUIDs.end());
  std::sort(theDiff.RemovedUIDs.begin(), theDiff.RemovedUIDs.end());
  std::sort(theDiff.CreatedRefUIDs.begin(), theDiff.CreatedRefUIDs.end());
  std::sort(theDiff.ModifiedRefUIDs.begin(), theDiff.ModifiedRefUIDs.end());
  std::sort(theDiff.RemovedRefUIDs.begin(), theDiff.RemovedRefUIDs.end());
  std::sort(theDiff.LocalIdRemapsForDiagnostics.begin(),
            theDiff.LocalIdRemapsForDiagnostics.end(),
            [](const BRepGraph_RevisionDiff::LocalIdRemap& theLeft,
               const BRepGraph_RevisionDiff::LocalIdRemap& theRight) {
              return theLeft.UID < theRight.UID;
            });
  std::sort(theDiff.UsageLinkChanges.begin(),
            theDiff.UsageLinkChanges.end(),
            [](const BRepGraph_RevisionDiff::UsageLinkChange& theLeft,
               const BRepGraph_RevisionDiff::UsageLinkChange& theRight) {
              if (theLeft.LinkUID != theRight.LinkUID)
              {
                return theLeft.LinkUID < theRight.LinkUID;
              }
              return static_cast<int>(theLeft.Kind) < static_cast<int>(theRight.Kind);
            });
}

void appendRootChanges(const BRepGraph&        theBaseGraph,
                       const BRepGraph&        theResultGraph,
                       BRepGraph_RevisionDiff& theDiff)
{
  NCollection_LinearVector<BRepGraph_UID> aBaseRoots;
  NCollection_LinearVector<BRepGraph_UID> aResultRoots;
  for (const BRepGraph_ProductId aRoot : theBaseGraph.RootProductIds())
  {
    aBaseRoots.Append(theBaseGraph.UIDs().Of(aRoot));
  }
  for (const BRepGraph_ProductId aRoot : theResultGraph.RootProductIds())
  {
    aResultRoots.Append(theResultGraph.UIDs().Of(aRoot));
  }

  std::sort(aBaseRoots.begin(), aBaseRoots.end());
  std::sort(aResultRoots.begin(), aResultRoots.end());
  NCollection_FlatMap<BRepGraph_UID> aBaseRootSet;
  NCollection_FlatMap<BRepGraph_UID> aResultRootSet;
  aBaseRootSet.Reserve(aBaseRoots.Size());
  aResultRootSet.Reserve(aResultRoots.Size());
  for (const BRepGraph_UID& aUID : aBaseRoots)
  {
    aBaseRootSet.Add(aUID);
  }
  for (const BRepGraph_UID& aUID : aResultRoots)
  {
    aResultRootSet.Add(aUID);
  }
  for (const BRepGraph_UID& aUID : aResultRoots)
  {
    if (!aBaseRootSet.Contains(aUID))
    {
      theDiff.RootChanges.Append({aUID, true});
    }
  }
  for (const BRepGraph_UID& aUID : aBaseRoots)
  {
    if (!aResultRootSet.Contains(aUID))
    {
      theDiff.RootChanges.Append({aUID, false});
    }
  }
}

void appendComponentChanges(const BRepGraph_Revision& theBaseRevision,
                            const BRepGraph_Revision& theResultRevision,
                            BRepGraph_RevisionDiff&   theDiff)
{
  const auto appendChange =
    [&theDiff](const BRepGraph_RevisionComponent::ComponentDescriptor& theDescriptor) {
      if (theDescriptor.ComponentDomain == BRepGraph_RevisionComponent::Domain::SupplementalStore)
      {
        theDiff.SupplementChangesByStoreGUID.Append(theDescriptor.StableGUID);
      }
      else
      {
        theDiff.LayerChangesByGUID.Append(theDescriptor.StableGUID);
      }
    };
  NCollection_FlatDataMap<Standard_GUID, occ::handle<BRepGraph_RevisionComponent>> aBaseComponents;
  NCollection_FlatDataMap<Standard_GUID, occ::handle<BRepGraph_RevisionComponent>> aResultComponents;
  for (const occ::handle<BRepGraph_RevisionComponent>& aComponent : theBaseRevision.Components())
  {
    aBaseComponents.Bind(aComponent->Descriptor().StableGUID, aComponent);
  }
  for (const occ::handle<BRepGraph_RevisionComponent>& aComponent : theResultRevision.Components())
  {
    aResultComponents.Bind(aComponent->Descriptor().StableGUID, aComponent);
  }
  for (const occ::handle<BRepGraph_RevisionComponent>& aBaseComponent :
       theBaseRevision.Components())
  {
    const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor =
      aBaseComponent->Descriptor();
    const occ::handle<BRepGraph_RevisionComponent>* aResultComponent =
      aResultComponents.Seek(aDescriptor.StableGUID);
    if (aResultComponent == nullptr
        || (*aResultComponent)->Descriptor().ComponentDomain != aDescriptor.ComponentDomain
        || theResultRevision.ComponentSemanticHash(aDescriptor.StableGUID)
             != theBaseRevision.ComponentSemanticHash(aDescriptor.StableGUID))
    {
      appendChange(aDescriptor);
    }
  }
  for (const occ::handle<BRepGraph_RevisionComponent>& aResultComponent :
       theResultRevision.Components())
  {
    const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor =
      aResultComponent->Descriptor();
    const occ::handle<BRepGraph_RevisionComponent>* aBaseComponent =
      aBaseComponents.Seek(aDescriptor.StableGUID);
    if (aBaseComponent == nullptr
        || (*aBaseComponent)->Descriptor().ComponentDomain != aDescriptor.ComponentDomain)
    {
      appendChange(aDescriptor);
    }
  }
}

} // namespace

//=================================================================================================

BRepGraph_RevisionDiff BRepGraph_Revision::Diff(const BRepGraph_Revision& theResultRevision) const
{
  const BRepGraph_Revision& theBaseRevision = *this;
  if (&theBaseRevision == &theResultRevision)
  {
    return BRepGraph_RevisionDiff();
  }
  if (theBaseRevision.GraphGUID() == theResultRevision.GraphGUID()
      && theBaseRevision.StorageRootHash() == theResultRevision.StorageRootHash())
  {
    return BRepGraph_RevisionDiff();
  }
  BRepGraph_RevisionDiff aDiff;

  const BRepGraph* aBaseGraph   = theBaseRevision.TryCoreGraph();
  const BRepGraph* aResultGraph = theResultRevision.TryCoreGraph();
  if (aBaseGraph == nullptr || aResultGraph == nullptr)
  {
    return aDiff;
  }

  NCollection_DataMap<BRepGraph_UID, NodeInfo> aBaseNodes;
  NCollection_DataMap<BRepGraph_UID, NodeInfo> aResultNodes;
  collectNodes<BRepGraphInc::VertexDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::EdgeDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::CoEdgeDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::WireDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::FaceDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::ShellDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::SolidDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::CompoundDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::CompSolidDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::ProductDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::OccurrenceDef>(*aBaseGraph, aBaseNodes);
  collectNodes<BRepGraphInc::VertexDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::EdgeDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::CoEdgeDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::WireDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::FaceDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::ShellDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::SolidDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::CompoundDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::CompSolidDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::ProductDef>(*aResultGraph, aResultNodes);
  collectNodes<BRepGraphInc::OccurrenceDef>(*aResultGraph, aResultNodes);

  for (NCollection_DataMap<BRepGraph_UID, NodeInfo>::Iterator anIt(aBaseNodes); anIt.More();
       anIt.Next())
  {
    const NodeInfo* aResultInfo = aResultNodes.Seek(anIt.Key());
    if (aResultInfo == nullptr)
    {
      aDiff.RemovedUIDs.Append(anIt.Key());
      if (anIt.Key().Kind == BRepGraph_NodeId::Kind::CoEdge)
      {
        aDiff.UsageLinkChanges.Append({anIt.Key(), BRepGraph_RevisionDiff::ChangeKind::Removed});
      }
      continue;
    }
    if (aResultInfo->Signature != anIt.Value().Signature)
    {
      aDiff.ModifiedUIDs.Append(anIt.Key());
      if (anIt.Key().Kind == BRepGraph_NodeId::Kind::CoEdge)
      {
        aDiff.UsageLinkChanges.Append({anIt.Key(), BRepGraph_RevisionDiff::ChangeKind::Modified});
      }
    }
    if (aResultInfo->Id != anIt.Value().Id)
    {
      aDiff.LocalIdRemapsForDiagnostics.Append(
        {BRepGraph_ItemUID::Node(anIt.Key().Kind, anIt.Key().Counter),
         BRepGraph_ItemId(aResultInfo->Id)});
    }
  }
  for (NCollection_DataMap<BRepGraph_UID, NodeInfo>::Iterator anIt(aResultNodes); anIt.More();
       anIt.Next())
  {
    if (!contains(aBaseNodes, anIt.Key()))
    {
      aDiff.CreatedUIDs.Append(anIt.Key());
      if (anIt.Key().Kind == BRepGraph_NodeId::Kind::CoEdge)
      {
        aDiff.UsageLinkChanges.Append({anIt.Key(), BRepGraph_RevisionDiff::ChangeKind::Created});
      }
    }
  }

  NCollection_DataMap<BRepGraph_RefUID, RefInfo> aBaseRefs;
  NCollection_DataMap<BRepGraph_RefUID, RefInfo> aResultRefs;
  collectReferences(*aBaseGraph, aBaseRefs);
  collectReferences(*aResultGraph, aResultRefs);
  for (NCollection_DataMap<BRepGraph_RefUID, RefInfo>::Iterator anIt(aBaseRefs); anIt.More();
       anIt.Next())
  {
    const RefInfo* aResultInfo = aResultRefs.Seek(anIt.Key());
    if (aResultInfo == nullptr)
    {
      aDiff.RemovedRefUIDs.Append(anIt.Key());
      continue;
    }
    if (aResultInfo->Signature != anIt.Value().Signature)
    {
      aDiff.ModifiedRefUIDs.Append(anIt.Key());
    }
    if (aResultInfo->Id != anIt.Value().Id)
    {
      aDiff.LocalIdRemapsForDiagnostics.Append(
        {BRepGraph_ItemUID::Reference(anIt.Key().Kind, anIt.Key().Counter),
         BRepGraph_ItemId(aResultInfo->Id)});
    }
  }
  for (NCollection_DataMap<BRepGraph_RefUID, RefInfo>::Iterator anIt(aResultRefs); anIt.More();
       anIt.Next())
  {
    if (!contains(aBaseRefs, anIt.Key()))
    {
      aDiff.CreatedRefUIDs.Append(anIt.Key());
    }
  }

  appendRootChanges(*aBaseGraph, *aResultGraph, aDiff);
  appendComponentChanges(theBaseRevision, theResultRevision, aDiff);
  sortChanges(aDiff);
  return aDiff;
}
