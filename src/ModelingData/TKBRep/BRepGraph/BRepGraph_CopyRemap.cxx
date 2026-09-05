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

#include <BRepGraph_CopyRemap.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RevisionHash.hxx>

namespace
{
const BRepGraph_CopyRemap::ItemMap& emptyItemMap()
{
  static const BRepGraph_CopyRemap::ItemMap THE_EMPTY_ITEM_MAP;
  return THE_EMPTY_ITEM_MAP;
}
} // namespace

//=================================================================================================

BRepGraph_CopyRemap::BRepGraph_CopyRemap(const BRepGraph&      theSourceGraph,
                                         BRepGraph&            theTargetGraph,
                                         const ItemMap&        theItemRemap,
                                         const Mode            theMode,
                                         const FreshnessPolicy theFreshnessPolicy) noexcept
    : mySourceGraph(&theSourceGraph),
      myTargetGraph(&theTargetGraph),
      myItemRemap(&theItemRemap),
      myMode(theMode),
      myMappingKind(MappingKind::Explicit),
      myFreshnessPolicy(theFreshnessPolicy)
{
}

//=================================================================================================

BRepGraph_CopyRemap::BRepGraph_CopyRemap(const BRepGraph&                       theSourceGraph,
                                         BRepGraph&                             theTargetGraph,
                                         const BRepGraph_CopyRemap::MappingKind theMappingKind,
                                         const BRepGraph_CopyRemap::Mode        theMode,
                                         const FreshnessPolicy theFreshnessPolicy) noexcept
    : mySourceGraph(&theSourceGraph),
      myTargetGraph(&theTargetGraph),
      myItemRemap(&emptyItemMap()),
      myMode(theMode),
      myMappingKind(theMappingKind),
      myFreshnessPolicy(theFreshnessPolicy)
{
}

//=================================================================================================

BRepGraph_ItemId BRepGraph_CopyRemap::TargetItem(const BRepGraph_ItemId theSourceItem) const
{
  if (!theSourceItem.IsValid())
  {
    return BRepGraph_ItemId();
  }

  if (myMappingKind == MappingKind::Identity)
  {
    // In identity mode, the source item id IS the target item id.
    return theSourceItem;
  }

  const BRepGraph_ItemId* aFound = myItemRemap->Seek(theSourceItem);
  return aFound != nullptr ? *aFound : BRepGraph_ItemId();
}

//=================================================================================================

BRepGraph_ItemId BRepGraph_CopyRemap::TargetItemOrInvalid(
  const BRepGraph_ItemId theSourceItem) const
{
  return TargetItem(theSourceItem);
}

//=================================================================================================

bool BRepGraph_CopyRemap::HasTargetItem(const BRepGraph_ItemId theSourceItem) const
{
  return TargetItem(theSourceItem).IsValid();
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::SourceUID(const BRepGraph_ItemId theSourceItem) const
{
  return mySourceGraph->UIDs().Of(theSourceItem);
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::TargetUID(const BRepGraph_ItemId theTargetItem) const
{
  return myTargetGraph->UIDs().Of(theTargetItem);
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::TargetUIDFromSource(
  const BRepGraph_ItemId theSourceItem) const
{
  return TargetUID(TargetItemOrInvalid(theSourceItem));
}

//=================================================================================================

bool BRepGraph_CopyRemap::HasSameOwnGeneration(const BRepGraph_ItemId theSourceItem) const
{
  if (mySourceGraph->UIDs().GraphGUID() != myTargetGraph->UIDs().GraphGUID()
      || mySourceGraph->UIDs().Generation() != myTargetGraph->UIDs().Generation())
  {
    return false;
  }
  const BRepGraph_ItemId aTargetItem = TargetItemOrInvalid(theSourceItem);
  if (!aTargetItem.IsValid())
  {
    return false;
  }
  const BRepGraph_ItemStamp aSourceStamp = mySourceGraph->UIDs().StampOf(theSourceItem);
  const BRepGraph_ItemStamp aTargetStamp = myTargetGraph->UIDs().StampOf(aTargetItem);
  if (!aSourceStamp.IsValid() || !aTargetStamp.IsValid()
      || aSourceStamp.myNodeUID != aTargetStamp.myNodeUID
      || aSourceStamp.myRefUID != aTargetStamp.myRefUID
      || aSourceStamp.myMutationGen != aTargetStamp.myMutationGen)
  {
    return false;
  }
  return theSourceItem.IsNode()
           ? BRepGraph_RevisionHash::Hasher::Node(*mySourceGraph, theSourceItem.NodeId())
               == BRepGraph_RevisionHash::Hasher::Node(*myTargetGraph, aTargetItem.NodeId())
           : BRepGraph_RevisionHash::Hasher::Reference(*mySourceGraph, theSourceItem.RefId())
               == BRepGraph_RevisionHash::Hasher::Reference(*myTargetGraph, aTargetItem.RefId());
}

//=================================================================================================

bool BRepGraph_CopyRemap::HasSameSubtreeGeneration(const BRepGraph_NodeId theSourceNode) const
{
  if (mySourceGraph->UIDs().GraphGUID() != myTargetGraph->UIDs().GraphGUID()
      || mySourceGraph->UIDs().Generation() != myTargetGraph->UIDs().Generation())
  {
    return false;
  }
  const BRepGraph_ItemId aTargetItem = TargetItemOrInvalid(BRepGraph_ItemId(theSourceNode));
  if (!aTargetItem.IsNode() || aTargetItem.NodeId().NodeKind != theSourceNode.NodeKind)
  {
    return false;
  }
  const BRepGraphInc::BaseDef* aSourceDef = mySourceGraph->Topo().Gen().TopoEntity(theSourceNode);
  const BRepGraphInc::BaseDef* aTargetDef =
    myTargetGraph->Topo().Gen().TopoEntity(aTargetItem.NodeId());
  const BRepGraph_ItemStamp aSourceStamp =
    mySourceGraph->UIDs().StampOf(BRepGraph_ItemId(theSourceNode));
  const BRepGraph_ItemStamp aTargetStamp = myTargetGraph->UIDs().StampOf(aTargetItem);
  return aSourceDef != nullptr && aTargetDef != nullptr && aSourceStamp.IsValid()
         && aTargetStamp.IsValid() && aSourceStamp.myNodeUID == aTargetStamp.myNodeUID
         && aSourceDef->SubtreeGen == aTargetDef->SubtreeGen
         && BRepGraph_RevisionHash::Hasher::Node(*mySourceGraph, theSourceNode)
              == BRepGraph_RevisionHash::Hasher::Node(*myTargetGraph, aTargetItem.NodeId());
}

//=================================================================================================

bool BRepGraph_CopyRemap::IsOwnGenerationCompatible(const BRepGraph_ItemId theSourceItem) const
{
  return HasTargetItem(theSourceItem)
         && (myFreshnessPolicy == FreshnessPolicy::SourceOnly
             || HasSameOwnGeneration(theSourceItem));
}

//=================================================================================================

bool BRepGraph_CopyRemap::IsSubtreeGenerationCompatible(const BRepGraph_NodeId theSourceNode) const
{
  const BRepGraph_ItemId aTargetItem = TargetItemOrInvalid(BRepGraph_ItemId(theSourceNode));
  return aTargetItem.IsNode() && aTargetItem.NodeId().NodeKind == theSourceNode.NodeKind
         && (myFreshnessPolicy == FreshnessPolicy::SourceOnly
             || HasSameSubtreeGeneration(theSourceNode));
}
