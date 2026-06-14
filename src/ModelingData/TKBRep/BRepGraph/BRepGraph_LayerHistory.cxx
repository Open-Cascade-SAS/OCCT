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

#include <BRepGraph_LayerHistory.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepTools_History.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerHistory, BRepGraph_Layer)

namespace
{
constexpr int THE_HISTORY_REPLACEMENT_BLOCK_SIZE = 4;
constexpr int THE_HISTORY_FILTERED_BLOCK_SIZE    = 4;
constexpr int THE_HISTORY_DERIVED_BLOCK_SIZE     = 8;
constexpr int THE_HISTORY_QUEUE_BLOCK_SIZE       = 32;

//! Pick the forward map that matches a record kind.  Deletion records do
//! not populate a forward map; the caller short-circuits for that kind.
inline NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>&
  selectForwardMap(
    const BRepGraph_LayerHistory::Kind                                                 theKind,
    NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& theModified,
    NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& theGenerated)
{
  return theKind == BRepGraph_LayerHistory::Kind::Generated ? theGenerated : theModified;
}

inline NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>&
  selectUidForwardMap(
    const BRepGraph_LayerHistory::Kind                                           theKind,
    NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>& theModified,
    NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>& theGenerated)
{
  return theKind == BRepGraph_LayerHistory::Kind::Generated ? theGenerated : theModified;
}

inline NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>&
  selectItemUidForwardMap(
    const BRepGraph_LayerHistory::Kind theKind,
    NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>&
      theModified,
    NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>&
      theGenerated)
{
  return theKind == BRepGraph_LayerHistory::Kind::Generated ? theGenerated : theModified;
}

inline void appendUniqueUid(NCollection_LinearVector<BRepGraph_UID>& theUids,
                            const BRepGraph_UID&                     theUid)
{
  for (const BRepGraph_UID& aSeen : theUids)
  {
    if (aSeen == theUid)
    {
      return;
    }
  }
  theUids.Append(theUid);
}

inline void appendUniqueItemUid(NCollection_LinearVector<BRepGraph_ItemUID>& theUids,
                                const BRepGraph_ItemUID&                     theUid)
{
  for (const BRepGraph_ItemUID& aSeen : theUids)
  {
    if (aSeen == theUid)
    {
      return;
    }
  }
  theUids.Append(theUid);
}

inline void appendUniqueNode(NCollection_LinearVector<BRepGraph_NodeId>& theNodes,
                             const BRepGraph_NodeId                      theNode)
{
  for (const BRepGraph_NodeId& aSeen : theNodes)
  {
    if (aSeen == theNode)
    {
      return;
    }
  }
  theNodes.Append(theNode);
}

inline void appendDerivedOrigin(
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& theMap,
  const BRepGraph_NodeId                                                             theDerived,
  const BRepGraph_NodeId                                                             theOriginal)
{
  if (theMap.IsBound(theDerived))
  {
    appendUniqueNode(theMap.ChangeFind(theDerived), theOriginal);
    return;
  }

  NCollection_LinearVector<BRepGraph_NodeId> anOrigins(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
  anOrigins.Append(theOriginal);
  theMap.Bind(theDerived, std::move(anOrigins));
}

template <class ContainerT>
inline NCollection_LinearVector<BRepGraph_NodeId> copyNodes(
  const ContainerT& theSource,
  const int         theBlockSize = THE_HISTORY_REPLACEMENT_BLOCK_SIZE)
{
  NCollection_LinearVector<BRepGraph_NodeId> aCopy(theBlockSize);
  for (const BRepGraph_NodeId& aNode : theSource)
  {
    aCopy.Append(aNode);
  }
  return aCopy;
}

template <class ContainerT>
inline NCollection_LinearVector<BRepGraph_UID> copyUids(
  const ContainerT& theSource,
  const int         theBlockSize = THE_HISTORY_REPLACEMENT_BLOCK_SIZE)
{
  NCollection_LinearVector<BRepGraph_UID> aCopy(theBlockSize);
  for (const BRepGraph_UID& aUid : theSource)
  {
    aCopy.Append(aUid);
  }
  return aCopy;
}

template <class ContainerT>
inline NCollection_LinearVector<BRepGraph_ItemUID> copyItemUids(
  const ContainerT& theSource,
  const int         theBlockSize = THE_HISTORY_REPLACEMENT_BLOCK_SIZE)
{
  NCollection_LinearVector<BRepGraph_ItemUID> aCopy(theBlockSize);
  for (const BRepGraph_ItemUID& aUid : theSource)
  {
    aCopy.Append(aUid);
  }
  return aCopy;
}

inline BRepGraph_ItemUID itemUidOf(const BRepGraph* theGraph, const BRepGraph_ItemId theItem)
{
  return theGraph != nullptr ? theGraph->UIDs().Of(theItem) : BRepGraph_ItemUID();
}

inline NCollection_LinearVector<BRepGraph_ItemUID> itemUidsOfNodes(
  const BRepGraph*                            theGraph,
  const NCollection_Array1<BRepGraph_NodeId>& theNodes)
{
  NCollection_LinearVector<BRepGraph_ItemUID> aResult(THE_HISTORY_FILTERED_BLOCK_SIZE);
  if (theGraph == nullptr)
  {
    return aResult;
  }

  for (const BRepGraph_NodeId& aNode : theNodes)
  {
    const BRepGraph_ItemUID aUid = theGraph->UIDs().Of(BRepGraph_ItemId(aNode));
    if (aUid.IsValid())
    {
      appendUniqueItemUid(aResult, aUid);
    }
  }
  return aResult;
}

inline void appendItemUidForward(
  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& theMap,
  const BRepGraph_ItemUID&                                                             theOriginal,
  const BRepGraph_ItemUID&                                                             theDerived)
{
  if (theMap.IsBound(theOriginal))
  {
    appendUniqueItemUid(theMap.ChangeFind(theOriginal), theDerived);
    return;
  }

  NCollection_LinearVector<BRepGraph_ItemUID> aFresh(THE_HISTORY_FILTERED_BLOCK_SIZE);
  aFresh.Append(theDerived);
  theMap.Bind(theOriginal, std::move(aFresh));
}
} // namespace

//=================================================================================================

BRepGraph_LayerHistory::BRepGraph_LayerHistory()
    : myRecords(32),
      myDerivedToOriginals(1),
      myOriginalToModified(1),
      myOriginalToGenerated(1),
      myDeleted(1),
      myUidOriginalToModified(1),
      myUidOriginalToGenerated(1),
      myUidKnownInputs(1),
      myUidDeleted(1),
      myItemUidOriginalToModified(1),
      myItemUidOriginalToGenerated(1),
      myItemUidKnownInputs(1),
      myItemUidDeleted(1)
{
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerHistory::GetID()
{
  static const Standard_GUID THE_ID("7b6756a1-0f98-48b0-9985-c0ab41e38012");
  return THE_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerHistory::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerHistory::Name() const
{
  static const TCollection_AsciiString THE_NAME("History");
  return THE_NAME;
}

//=================================================================================================

void BRepGraph_LayerHistory::OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept
{
  if (!myEnabled || !theNode.IsValid())
  {
    return;
  }

  try
  {
    NCollection_LinearVector<BRepGraph_NodeId> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
    aDeleted.Append(theNode);
    RecordDeleted(TCollection_AsciiString("Graph:RemoveNode"), aDeleted.ToArray1());
  }
  catch (...)
  {
  }
}

//=================================================================================================

void BRepGraph_LayerHistory::rebuildCaches()
{
  myDerivedToOriginals.Clear();
  myOriginalToModified.Clear();
  myOriginalToGenerated.Clear();
  myDeleted.Clear();
  myUidOriginalToModified.Clear();
  myUidOriginalToGenerated.Clear();
  myUidKnownInputs.Clear();
  myUidDeleted.Clear();
  myItemUidOriginalToModified.Clear();
  myItemUidOriginalToGenerated.Clear();
  myItemUidKnownInputs.Clear();
  myItemUidDeleted.Clear();

  for (const Event& aRecord : myRecords)
  {
    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>::Iterator
           anIt(aRecord.Mapping);
         anIt.More();
         anIt.Next())
    {
      const BRepGraph_NodeId&                           anOriginal    = anIt.Key();
      const NCollection_LinearVector<BRepGraph_NodeId>& aReplacements = anIt.Value();
      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Deleted || aReplacements.IsEmpty())
      {
        myDeleted.Add(anOriginal);
        continue;
      }

      NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& aFwd =
        selectForwardMap(aRecord.RecordKind, myOriginalToModified, myOriginalToGenerated);
      for (const BRepGraph_NodeId& aDerived : aReplacements)
      {
        if (aDerived == anOriginal)
        {
          continue;
        }
        appendDerivedOrigin(myDerivedToOriginals, aDerived, anOriginal);
        if (aFwd.IsBound(anOriginal))
        {
          appendUniqueNode(aFwd.ChangeFind(anOriginal), aDerived);
        }
        else
        {
          NCollection_LinearVector<BRepGraph_NodeId> aFresh(THE_HISTORY_FILTERED_BLOCK_SIZE);
          aFresh.Append(aDerived);
          aFwd.Bind(anOriginal, std::move(aFresh));
        }
      }

      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Replaced)
      {
        myDeleted.Add(anOriginal);
      }
    }

    for (NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>::Iterator anIt(
           aRecord.UidMapping);
         anIt.More();
         anIt.Next())
    {
      const BRepGraph_UID& anOriginal = anIt.Key();
      myUidKnownInputs.Add(anOriginal);
      const NCollection_LinearVector<BRepGraph_UID>& aReplacements = anIt.Value();
      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Deleted || aReplacements.IsEmpty())
      {
        myUidDeleted.Add(anOriginal);
        continue;
      }

      NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>& aFwd =
        selectUidForwardMap(aRecord.RecordKind, myUidOriginalToModified, myUidOriginalToGenerated);
      for (const BRepGraph_UID& aDerived : aReplacements)
      {
        if (aDerived != anOriginal)
        {
          if (aFwd.IsBound(anOriginal))
          {
            appendUniqueUid(aFwd.ChangeFind(anOriginal), aDerived);
          }
          else
          {
            NCollection_LinearVector<BRepGraph_UID> aFresh(THE_HISTORY_FILTERED_BLOCK_SIZE);
            aFresh.Append(aDerived);
            aFwd.Bind(anOriginal, std::move(aFresh));
          }
        }
      }

      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Replaced)
      {
        myUidDeleted.Add(anOriginal);
      }
    }

    for (NCollection_DataMap<BRepGraph_ItemUID,
                             NCollection_LinearVector<BRepGraph_ItemUID>>::Iterator
           anIt(aRecord.ItemUidMapping);
         anIt.More();
         anIt.Next())
    {
      const BRepGraph_ItemUID& anOriginal = anIt.Key();
      myItemUidKnownInputs.Add(anOriginal);
      const NCollection_LinearVector<BRepGraph_ItemUID>& aReplacements = anIt.Value();
      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Deleted || aReplacements.IsEmpty())
      {
        myItemUidDeleted.Add(anOriginal);
        continue;
      }

      NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& aFwd =
        selectItemUidForwardMap(aRecord.RecordKind,
                                myItemUidOriginalToModified,
                                myItemUidOriginalToGenerated);
      for (const BRepGraph_ItemUID& aDerived : aReplacements)
      {
        if (aDerived != anOriginal)
        {
          appendItemUidForward(aFwd, anOriginal, aDerived);
        }
      }

      if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Replaced)
      {
        myItemUidDeleted.Add(anOriginal);
      }
    }
  }

  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::CopyTo(const BRepGraph_CopyRemap& theCopy) const
{
  if (myRecords.IsEmpty())
  {
    return;
  }

  if (theCopy.IsCompact())
  {
    // Compact mode: remap node-domain keys through ItemId map, preserve durable UID/ItemUID,
    // skip previous Compact:Remap records, drop stale node-only entries.
    occ::handle<BRepGraph_LayerHistory> aTarget =
      theCopy.TargetGraph().LayerRegistry().Ensure<BRepGraph_LayerHistory>();
    aTarget->SetEnabled(true);

    static const TCollection_AsciiString THE_COMPACT_REMAP_LABEL("Compact:Remap");
    NCollection_DynamicArray<Event>      aNewRecords(32);

    for (const Event& aRecord : myRecords)
    {
      if (aRecord.OperationName == THE_COMPACT_REMAP_LABEL)
      {
        continue;
      }

      Event aNewRecord;
      aNewRecord.OperationName  = aRecord.OperationName;
      aNewRecord.SequenceNumber = aNewRecords.Size();
      aNewRecord.RecordKind     = aRecord.RecordKind;
      aNewRecord.ExtraInfo      = aRecord.ExtraInfo;

      // Preserve durable UID mappings as-is.
      for (NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>::Iterator
             anIt(aRecord.UidMapping);
           anIt.More();
           anIt.Next())
      {
        aNewRecord.UidMapping.Bind(anIt.Key(), copyUids(anIt.Value()));
      }
      for (NCollection_DataMap<BRepGraph_ItemUID,
                               NCollection_LinearVector<BRepGraph_ItemUID>>::Iterator
             anIt(aRecord.ItemUidMapping);
           anIt.More();
           anIt.Next())
      {
        aNewRecord.ItemUidMapping.Bind(anIt.Key(), copyItemUids(anIt.Value()));
      }

      // Remap NodeId-based entries through the ItemId map.
      for (NCollection_DataMap<BRepGraph_NodeId,
                               NCollection_LinearVector<BRepGraph_NodeId>>::Iterator
             anIt(aRecord.Mapping);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_ItemId aNewOriginalItem = theCopy.TargetItem(BRepGraph_ItemId(anIt.Key()));
        if (!aNewOriginalItem.IsNode())
        {
          continue;
        }

        NCollection_LinearVector<BRepGraph_NodeId> aNewImages(THE_HISTORY_FILTERED_BLOCK_SIZE);
        for (const BRepGraph_NodeId& anOldImage : anIt.Value())
        {
          const BRepGraph_ItemId aNewImageItem = theCopy.TargetItem(BRepGraph_ItemId(anOldImage));
          if (aNewImageItem.IsNode())
          {
            appendUniqueNode(aNewImages, aNewImageItem.NodeId());
          }
        }
        aNewRecord.Mapping.Bind(aNewOriginalItem.NodeId(), std::move(aNewImages));
      }

      if (!aNewRecord.Mapping.IsEmpty() || !aNewRecord.UidMapping.IsEmpty()
          || !aNewRecord.ItemUidMapping.IsEmpty())
      {
        aNewRecords.Append(std::move(aNewRecord));
      }
    }

    aTarget->myRecords = std::move(aNewRecords);
    aTarget->rebuildCaches();
    aTarget->SetEnabled(myEnabled);
    return;
  }

  occ::handle<BRepGraph_LayerHistory> aTarget =
    theCopy.TargetGraph().LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  aTarget->SetEnabled(true);
  bool hasRawRecordsAppended = false;

  auto toSourceItem = [&](const BRepGraph_ItemUID& theUID) {
    return theCopy.SourceGraph().UIDs().ItemIdFrom(theUID);
  };

  auto replayItems = [&](const TCollection_AsciiString&                    theOperationName,
                         const BRepGraph_LayerHistory::Kind                theKind,
                         const BRepGraph_ItemId                            theSourceOriginal,
                         const BRepGraph_ItemUID&                          theDurableOriginalUID,
                         const NCollection_LinearVector<BRepGraph_ItemId>& theSourceImages) {
    const BRepGraph_ItemId aTargetOriginal = theCopy.TargetItem(theSourceOriginal);
    if (!aTargetOriginal.IsValid())
    {
      if (theKind == BRepGraph_LayerHistory::Kind::Deleted)
      {
        if (theSourceOriginal.IsNode()
            && theCopy.TargetGraphConst().Topo().Gen().TopoEntity(theSourceOriginal.NodeId())
                 != nullptr)
        {
          NCollection_LinearVector<BRepGraph_NodeId> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
          aDeleted.Append(theSourceOriginal.NodeId());
          aTarget->RecordDeleted(theOperationName, aDeleted.ToArray1());
        }

        if (theDurableOriginalUID.IsValid())
        {
          NCollection_LinearVector<BRepGraph_ItemUID> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
          aDeleted.Append(theDurableOriginalUID);
          aTarget->RecordDeletedItemUid(theOperationName, aDeleted.ToArray1());
        }
      }
      return;
    }

    NCollection_LinearVector<BRepGraph_ItemUID> aTargetImageUIDs(THE_HISTORY_FILTERED_BLOCK_SIZE);
    NCollection_LinearVector<BRepGraph_NodeId>  aTargetImageNodes(THE_HISTORY_FILTERED_BLOCK_SIZE);
    bool                                        areAllItemsNodes = aTargetOriginal.IsNode();
    for (const BRepGraph_ItemId& aSourceImage : theSourceImages)
    {
      const BRepGraph_ItemId aTargetImage = theCopy.TargetItem(aSourceImage);
      if (!aTargetImage.IsValid())
      {
        continue;
      }

      const BRepGraph_ItemUID aTargetUID = theCopy.TargetUID(aTargetImage);
      if (aTargetUID.IsValid())
      {
        appendUniqueItemUid(aTargetImageUIDs, aTargetUID);
      }

      if (areAllItemsNodes && aTargetImage.IsNode())
      {
        appendUniqueNode(aTargetImageNodes, aTargetImage.NodeId());
      }
      else
      {
        areAllItemsNodes = false;
      }
    }

    if (theKind == BRepGraph_LayerHistory::Kind::Deleted)
    {
      if (aTargetOriginal.IsNode())
      {
        NCollection_LinearVector<BRepGraph_NodeId> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
        aDeleted.Append(aTargetOriginal.NodeId());
        aTarget->RecordDeleted(theOperationName, aDeleted.ToArray1());
      }
      if (theDurableOriginalUID.IsValid())
      {
        NCollection_LinearVector<BRepGraph_ItemUID> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
        aDeleted.Append(theDurableOriginalUID);
        aTarget->RecordDeletedItemUid(theOperationName, aDeleted.ToArray1());
      }
      return;
    }

    if (aTargetImageUIDs.IsEmpty())
    {
      return;
    }

    if (areAllItemsNodes)
    {
      aTarget->Record(theOperationName,
                      aTargetOriginal.NodeId(),
                      aTargetImageNodes.ToArray1(),
                      theKind);
      return;
    }

    const BRepGraph_ItemUID aTargetOriginalUID = theCopy.TargetUID(aTargetOriginal);
    if (aTargetOriginalUID.IsValid())
    {
      aTarget->RecordItemUid(theOperationName,
                             aTargetOriginalUID,
                             aTargetImageUIDs.ToArray1(),
                             theKind);
    }
  };

  for (const Event& aRecord : myRecords)
  {
    if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Deleted && !aRecord.Mapping.IsEmpty())
    {
      Event aNewRecord;
      aNewRecord.OperationName  = aRecord.OperationName;
      aNewRecord.SequenceNumber = aTarget->myRecords.Size();
      aNewRecord.RecordKind     = aRecord.RecordKind;
      aNewRecord.ExtraInfo      = aRecord.ExtraInfo;

      for (NCollection_DataMap<BRepGraph_NodeId,
                               NCollection_LinearVector<BRepGraph_NodeId>>::Iterator
             anIt(aRecord.Mapping);
           anIt.More();
           anIt.Next())
      {
        BRepGraph_NodeId       aTargetNode;
        const BRepGraph_ItemId aTargetOriginal = theCopy.TargetItem(BRepGraph_ItemId(anIt.Key()));
        if (aTargetOriginal.IsNode())
        {
          aTargetNode = aTargetOriginal.NodeId();
        }
        else if (theCopy.TargetGraphConst().Topo().Gen().TopoEntity(anIt.Key()) != nullptr)
        {
          aTargetNode = anIt.Key();
        }
        if (!aTargetNode.IsValid())
        {
          continue;
        }

        NCollection_LinearVector<BRepGraph_NodeId> aTargetImages(THE_HISTORY_FILTERED_BLOCK_SIZE);
        for (const BRepGraph_NodeId& aNode : anIt.Value())
        {
          const BRepGraph_ItemId aTargetImage = theCopy.TargetItem(BRepGraph_ItemId(aNode));
          if (aTargetImage.IsNode())
          {
            appendUniqueNode(aTargetImages, aTargetImage.NodeId());
          }
        }
        aNewRecord.Mapping.Bind(aTargetNode, std::move(aTargetImages));
      }

      if (!aRecord.ItemUidMapping.IsEmpty())
      {
        for (NCollection_DataMap<BRepGraph_ItemUID,
                                 NCollection_LinearVector<BRepGraph_ItemUID>>::Iterator
               anIt(aRecord.ItemUidMapping);
             anIt.More();
             anIt.Next())
        {
          aNewRecord.ItemUidMapping.Bind(anIt.Key(), copyItemUids(anIt.Value()));
        }
      }

      if (!aNewRecord.Mapping.IsEmpty() || !aNewRecord.ItemUidMapping.IsEmpty())
      {
        aTarget->myRecords.Append(std::move(aNewRecord));
        hasRawRecordsAppended = true;
      }
      continue;
    }

    if (!aRecord.ItemUidMapping.IsEmpty())
    {
      for (NCollection_DataMap<BRepGraph_ItemUID,
                               NCollection_LinearVector<BRepGraph_ItemUID>>::Iterator
             anIt(aRecord.ItemUidMapping);
           anIt.More();
           anIt.Next())
      {
        NCollection_LinearVector<BRepGraph_ItemId> aSourceImages(THE_HISTORY_FILTERED_BLOCK_SIZE);
        for (const BRepGraph_ItemUID& aUid : anIt.Value())
        {
          const BRepGraph_ItemId aSourceImage = toSourceItem(aUid);
          if (aSourceImage.IsValid())
          {
            aSourceImages.Append(aSourceImage);
          }
        }
        replayItems(aRecord.OperationName,
                    aRecord.RecordKind,
                    toSourceItem(anIt.Key()),
                    anIt.Key(),
                    aSourceImages);
      }
      continue;
    }

    if (!aRecord.UidMapping.IsEmpty())
    {
      for (NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>::Iterator
             anIt(aRecord.UidMapping);
           anIt.More();
           anIt.Next())
      {
        if (aRecord.RecordKind == BRepGraph_LayerHistory::Kind::Deleted)
        {
          NCollection_LinearVector<BRepGraph_UID> aDeleted(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
          aDeleted.Append(anIt.Key());
          aTarget->RecordDeletedUid(aRecord.OperationName, aDeleted.ToArray1());
          continue;
        }

        NCollection_LinearVector<BRepGraph_ItemId> aSourceImages(THE_HISTORY_FILTERED_BLOCK_SIZE);
        NCollection_LinearVector<BRepGraph_UID> aTargetImageUIDs(THE_HISTORY_FILTERED_BLOCK_SIZE);
        for (const BRepGraph_UID& aUid : anIt.Value())
        {
          const BRepGraph_ItemId aSourceImage =
            toSourceItem(BRepGraph_ItemUID::Node(aUid.Kind, aUid.Counter));
          if (aSourceImage.IsValid())
          {
            aSourceImages.Append(aSourceImage);
            const BRepGraph_ItemId aTargetImage = theCopy.TargetItem(aSourceImage);
            if (aTargetImage.IsNode())
            {
              const BRepGraph_UID aTargetUID =
                theCopy.TargetGraphConst().UIDs().Of(aTargetImage.NodeId());
              if (aTargetUID.IsValid())
              {
                appendUniqueUid(aTargetImageUIDs, aTargetUID);
              }
            }
          }
        }
        if (!aTargetImageUIDs.IsEmpty())
        {
          aTarget->RecordUid(aRecord.OperationName,
                             anIt.Key(),
                             aTargetImageUIDs.ToArray1(),
                             aRecord.RecordKind);
        }
      }
      continue;
    }

    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>::Iterator
           anIt(aRecord.Mapping);
         anIt.More();
         anIt.Next())
    {
      NCollection_LinearVector<BRepGraph_ItemId> aSourceImages(THE_HISTORY_FILTERED_BLOCK_SIZE);
      for (const BRepGraph_NodeId& aNode : anIt.Value())
      {
        aSourceImages.Append(BRepGraph_ItemId(aNode));
      }
      replayItems(aRecord.OperationName,
                  aRecord.RecordKind,
                  BRepGraph_ItemId(anIt.Key()),
                  BRepGraph_ItemUID(),
                  aSourceImages);
    }
  }

  if (hasRawRecordsAppended)
  {
    aTarget->rebuildCaches();
  }
  aTarget->SetEnabled(myEnabled);
}

//=================================================================================================

void BRepGraph_LayerHistory::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerHistory::Record(const TCollection_AsciiString&              theOpLabel,
                                    const BRepGraph_NodeId                      theOriginal,
                                    const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
                                    const BRepGraph_LayerHistory::Kind          theKind)
{
  if (!myEnabled)
  {
    return;
  }

  // Append a new history record.  Empty replacements collapse to a Deleted
  // record regardless of the caller-supplied kind.
  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind = theReplacements.IsEmpty() ? BRepGraph_LayerHistory::Kind::Deleted : theKind;
  aRecord.Mapping.Bind(theOriginal, copyNodes(theReplacements));
  const BRepGraph*        aGraph            = AttachedGraph();
  const BRepGraph_ItemUID anOriginalItemUID = itemUidOf(aGraph, BRepGraph_ItemId(theOriginal));
  const NCollection_LinearVector<BRepGraph_ItemUID> anItemReplacements =
    itemUidsOfNodes(aGraph, theReplacements);
  if (anOriginalItemUID.IsValid())
  {
    myItemUidKnownInputs.Add(anOriginalItemUID);
    aRecord.ItemUidMapping.Bind(anOriginalItemUID, copyItemUids(anItemReplacements));
  }
  myRecords.Append(std::move(aRecord));

  if (theReplacements.IsEmpty())
  {
    myDeleted.Add(theOriginal);
    if (anOriginalItemUID.IsValid())
    {
      myItemUidDeleted.Add(anOriginalItemUID);
    }
    touch();
    return;
  }

  Standard_ASSERT_VOID(theKind != BRepGraph_LayerHistory::Kind::Deleted,
                       "Record: use RecordDeleted() for deletions");

  // Populate the per-kind forward map and the reverse map.  Skip
  // self-referencing entries (aDerived == theOriginal) to avoid overwriting
  // prior chain links in the reverse map.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& aFwd =
    selectForwardMap(theKind, myOriginalToModified, myOriginalToGenerated);
  NCollection_LinearVector<BRepGraph_NodeId>* aDerivedVec =
    aFwd.IsBound(theOriginal) ? &aFwd.ChangeFind(theOriginal) : nullptr;

  for (const BRepGraph_NodeId& aDerived : theReplacements)
  {
    if (aDerived == theOriginal)
    {
      continue;
    }
    appendDerivedOrigin(myDerivedToOriginals, aDerived, theOriginal);
    if (aDerivedVec == nullptr)
    {
      NCollection_LinearVector<BRepGraph_NodeId> aFresh(THE_HISTORY_FILTERED_BLOCK_SIZE);
      aFresh.Append(aDerived);
      aDerivedVec = aFwd.Bound(theOriginal, std::move(aFresh));
    }
    else
    {
      // De-dup: avoid appending the same derived id twice if the caller
      // records the same edge across multiple Record() calls.
      bool aSeen = false;
      for (const BRepGraph_NodeId& aExisting : *aDerivedVec)
      {
        if (aExisting == aDerived)
        {
          aSeen = true;
          break;
        }
      }
      if (!aSeen)
      {
        aDerivedVec->Append(aDerived);
      }
    }
  }

  if (anOriginalItemUID.IsValid())
  {
    NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& anItemFwd =
      selectItemUidForwardMap(theKind, myItemUidOriginalToModified, myItemUidOriginalToGenerated);
    for (const BRepGraph_ItemUID& aDerivedUID : anItemReplacements)
    {
      if (aDerivedUID != anOriginalItemUID)
      {
        appendItemUidForward(anItemFwd, anOriginalItemUID, aDerivedUID);
      }
    }
  }

  if (theKind == BRepGraph_LayerHistory::Kind::Replaced)
  {
    myDeleted.Add(theOriginal);
    if (anOriginalItemUID.IsValid())
    {
      myItemUidDeleted.Add(anOriginalItemUID);
    }
  }
  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordBatch(
  const TCollection_AsciiString&              theOpLabel,
  const NCollection_Array1<BRepGraph_NodeId>& theOriginals,
  const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
  const TCollection_AsciiString&              theExtraInfo,
  const BRepGraph_LayerHistory::Kind          theKind)
{
  Standard_ASSERT_VOID(theOriginals.Size() == theReplacements.Size(),
                       "RecordBatch: mismatched array lengths");
  Standard_ASSERT_VOID(theKind != BRepGraph_LayerHistory::Kind::Deleted,
                       "RecordBatch: use RecordDeleted() for deletions");
  if (!myEnabled || theOriginals.IsEmpty())
  {
    return;
  }

  const size_t aNbPairs = theOriginals.Size();

  // Create a single history record with all mappings.
  // Pre-size the Mapping to avoid DataMap rehashing.
  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = theKind;
  aRecord.Mapping.ReSize(aNbPairs);

  aRecord.ExtraInfo = theExtraInfo;

  // Build mapping: each pair creates a 1-element replacement vector.
  const BRepGraph* aGraph = AttachedGraph();
  {
    for (size_t aPairIdx = 0; aPairIdx < aNbPairs; ++aPairIdx)
    {
      const BRepGraph_NodeId& anOriginal   = theOriginals.At(aPairIdx);
      const BRepGraph_NodeId& aReplacement = theReplacements.At(aPairIdx);
      Standard_ASSERT_VOID(!aRecord.Mapping.IsBound(anOriginal),
                           "RecordBatch: duplicate original node");
      NCollection_LinearVector<BRepGraph_NodeId> aRepVec(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
      aRepVec.Append(aReplacement);
      aRecord.Mapping.Bind(anOriginal, std::move(aRepVec));

      const BRepGraph_ItemUID anOriginalUID   = itemUidOf(aGraph, BRepGraph_ItemId(anOriginal));
      const BRepGraph_ItemUID aReplacementUID = itemUidOf(aGraph, BRepGraph_ItemId(aReplacement));
      if (anOriginalUID.IsValid())
      {
        myItemUidKnownInputs.Add(anOriginalUID);
        NCollection_LinearVector<BRepGraph_ItemUID> anItemRepVec(
          THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
        if (aReplacementUID.IsValid())
        {
          anItemRepVec.Append(aReplacementUID);
        }
        aRecord.ItemUidMapping.Bind(anOriginalUID, std::move(anItemRepVec));
      }
    }
  }
  myRecords.Append(std::move(aRecord));

  // Update per-kind forward map and reverse map in bulk.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>& aFwd =
    selectForwardMap(theKind, myOriginalToModified, myOriginalToGenerated);
  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& anItemFwd =
    selectItemUidForwardMap(theKind, myItemUidOriginalToModified, myItemUidOriginalToGenerated);
  aFwd.ReSize(aFwd.Extent() + aNbPairs);

  {
    for (size_t aPairIdx = 0; aPairIdx < aNbPairs; ++aPairIdx)
    {
      const BRepGraph_NodeId& anOriginal      = theOriginals.At(aPairIdx);
      const BRepGraph_NodeId& aReplacement    = theReplacements.At(aPairIdx);
      const BRepGraph_ItemUID anOriginalUID   = itemUidOf(aGraph, BRepGraph_ItemId(anOriginal));
      const BRepGraph_ItemUID aReplacementUID = itemUidOf(aGraph, BRepGraph_ItemId(aReplacement));
      if (aReplacement == anOriginal)
      {
        continue;
      }

      appendDerivedOrigin(myDerivedToOriginals, aReplacement, anOriginal);

      if (aFwd.IsBound(anOriginal))
      {
        NCollection_LinearVector<BRepGraph_NodeId>& aDerVec = aFwd.ChangeFind(anOriginal);
        bool                                        aSeen   = false;
        for (const BRepGraph_NodeId& aExisting : aDerVec)
        {
          if (aExisting == aReplacement)
          {
            aSeen = true;
            break;
          }
        }
        if (!aSeen)
        {
          aDerVec.Append(aReplacement);
        }
      }
      else
      {
        NCollection_LinearVector<BRepGraph_NodeId> aDerVec(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
        aDerVec.Append(aReplacement);
        aFwd.Bind(anOriginal, std::move(aDerVec));
      }

      if (theKind == BRepGraph_LayerHistory::Kind::Replaced)
      {
        myDeleted.Add(anOriginal);
      }

      if (anOriginalUID.IsValid() && aReplacementUID.IsValid() && aReplacementUID != anOriginalUID)
      {
        appendItemUidForward(anItemFwd, anOriginalUID, aReplacementUID);
      }

      if (theKind == BRepGraph_LayerHistory::Kind::Replaced && anOriginalUID.IsValid())
      {
        myItemUidDeleted.Add(anOriginalUID);
      }
    }
  }
  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordDeleted(const TCollection_AsciiString&              theOpLabel,
                                           const NCollection_Array1<BRepGraph_NodeId>& theDeleted)
{
  if (!myEnabled || theDeleted.IsEmpty())
  {
    return;
  }

  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = BRepGraph_LayerHistory::Kind::Deleted;
  aRecord.Mapping.ReSize(theDeleted.Size());
  aRecord.ItemUidMapping.ReSize(theDeleted.Size());
  const BRepGraph* aGraph = AttachedGraph();
  for (const BRepGraph_NodeId& aNode : theDeleted)
  {
    if (!aRecord.Mapping.IsBound(aNode))
    {
      aRecord.Mapping.Bind(
        aNode,
        NCollection_LinearVector<BRepGraph_NodeId>(THE_HISTORY_REPLACEMENT_BLOCK_SIZE));
    }
    myDeleted.Add(aNode);

    const BRepGraph_ItemUID aUID = itemUidOf(aGraph, BRepGraph_ItemId(aNode));
    if (aUID.IsValid())
    {
      myItemUidKnownInputs.Add(aUID);
      myItemUidDeleted.Add(aUID);
      if (!aRecord.ItemUidMapping.IsBound(aUID))
      {
        aRecord.ItemUidMapping.Bind(
          aUID,
          NCollection_LinearVector<BRepGraph_ItemUID>(THE_HISTORY_REPLACEMENT_BLOCK_SIZE));
      }
    }
  }
  myRecords.Append(std::move(aRecord));
  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordReplaced(const TCollection_AsciiString& theOpLabel,
                                            const BRepGraph_NodeId         theOriginal,
                                            const BRepGraph_NodeId         theReplacement)
{
  if (!theOriginal.IsValid() || !theReplacement.IsValid())
  {
    return;
  }

  NCollection_LinearVector<BRepGraph_NodeId> aReplacements(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
  aReplacements.Append(theReplacement);
  Record(theOpLabel, theOriginal, aReplacements.ToArray1(), BRepGraph_LayerHistory::Kind::Replaced);
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordReplacedBatch(
  const TCollection_AsciiString&              theOpLabel,
  const NCollection_Array1<BRepGraph_NodeId>& theOriginals,
  const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
  const TCollection_AsciiString&              theExtraInfo)
{
  RecordBatch(theOpLabel,
              theOriginals,
              theReplacements,
              theExtraInfo,
              BRepGraph_LayerHistory::Kind::Replaced);
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordUid(const TCollection_AsciiString&           theOpLabel,
                                       const BRepGraph_UID&                     theOriginal,
                                       const NCollection_Array1<BRepGraph_UID>& theReplacements,
                                       const BRepGraph_LayerHistory::Kind       theKind)
{
  if (!myEnabled || !theOriginal.IsValid())
  {
    return;
  }

  myUidKnownInputs.Add(theOriginal);
  myItemUidKnownInputs.Add(BRepGraph_ItemUID::Node(theOriginal.Kind, theOriginal.Counter));

  if (theReplacements.IsEmpty())
  {
    NCollection_LinearVector<BRepGraph_UID> aDeleted(THE_HISTORY_DERIVED_BLOCK_SIZE);
    aDeleted.Append(theOriginal);
    RecordDeletedUid(theOpLabel, aDeleted.ToArray1());
    return;
  }

  Standard_ASSERT_VOID(theKind != BRepGraph_LayerHistory::Kind::Deleted,
                       "RecordUid: use RecordDeletedUid() for deletions");

  NCollection_LinearVector<BRepGraph_UID>     aValidReplacements(THE_HISTORY_FILTERED_BLOCK_SIZE);
  NCollection_LinearVector<BRepGraph_ItemUID> aValidItemReplacements(
    THE_HISTORY_FILTERED_BLOCK_SIZE);
  for (const BRepGraph_UID& aDerived : theReplacements)
  {
    if (aDerived.IsValid())
    {
      appendUniqueUid(aValidReplacements, aDerived);
      appendUniqueItemUid(aValidItemReplacements,
                          BRepGraph_ItemUID::Node(aDerived.Kind, aDerived.Counter));
    }
  }
  if (aValidReplacements.IsEmpty())
  {
    return;
  }

  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = theKind;
  aRecord.UidMapping.Bind(theOriginal, copyUids(aValidReplacements));
  aRecord.ItemUidMapping.Bind(BRepGraph_ItemUID::Node(theOriginal.Kind, theOriginal.Counter),
                              copyItemUids(aValidItemReplacements));
  myRecords.Append(std::move(aRecord));

  NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>& aFwd =
    selectUidForwardMap(theKind, myUidOriginalToModified, myUidOriginalToGenerated);
  NCollection_LinearVector<BRepGraph_UID>* aDerivedVec =
    aFwd.IsBound(theOriginal) ? &aFwd.ChangeFind(theOriginal) : nullptr;

  for (const BRepGraph_UID& aDerived : aValidReplacements)
  {
    if (aDerivedVec == nullptr)
    {
      NCollection_LinearVector<BRepGraph_UID> aFresh(THE_HISTORY_FILTERED_BLOCK_SIZE);
      aFresh.Append(aDerived);
      aDerivedVec = aFwd.Bound(theOriginal, std::move(aFresh));
    }
    else
    {
      appendUniqueUid(*aDerivedVec, aDerived);
    }
  }

  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& anItemFwd =
    selectItemUidForwardMap(theKind, myItemUidOriginalToModified, myItemUidOriginalToGenerated);
  for (const BRepGraph_ItemUID& aDerived : aValidItemReplacements)
  {
    appendItemUidForward(anItemFwd,
                         BRepGraph_ItemUID::Node(theOriginal.Kind, theOriginal.Counter),
                         aDerived);
  }

  if (theKind == BRepGraph_LayerHistory::Kind::Replaced)
  {
    myUidDeleted.Add(theOriginal);
    myItemUidDeleted.Add(BRepGraph_ItemUID::Node(theOriginal.Kind, theOriginal.Counter));
  }
  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordDeletedUid(const TCollection_AsciiString&           theOpLabel,
                                              const NCollection_Array1<BRepGraph_UID>& theDeleted)
{
  if (!myEnabled || theDeleted.IsEmpty())
  {
    return;
  }

  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = BRepGraph_LayerHistory::Kind::Deleted;
  aRecord.UidMapping.ReSize(theDeleted.Size());
  aRecord.ItemUidMapping.ReSize(theDeleted.Size());

  for (const BRepGraph_UID& aUid : theDeleted)
  {
    if (!aUid.IsValid())
    {
      continue;
    }
    myUidKnownInputs.Add(aUid);
    myUidDeleted.Add(aUid);
    myItemUidKnownInputs.Add(BRepGraph_ItemUID::Node(aUid.Kind, aUid.Counter));
    myItemUidDeleted.Add(BRepGraph_ItemUID::Node(aUid.Kind, aUid.Counter));
    if (!aRecord.UidMapping.IsBound(aUid))
    {
      aRecord.UidMapping.Bind(
        aUid,
        NCollection_LinearVector<BRepGraph_UID>(THE_HISTORY_REPLACEMENT_BLOCK_SIZE));
    }
    if (!aRecord.ItemUidMapping.IsBound(BRepGraph_ItemUID::Node(aUid.Kind, aUid.Counter)))
    {
      aRecord.ItemUidMapping.Bind(
        BRepGraph_ItemUID::Node(aUid.Kind, aUid.Counter),
        NCollection_LinearVector<BRepGraph_ItemUID>(THE_HISTORY_REPLACEMENT_BLOCK_SIZE));
    }
  }

  if (!aRecord.UidMapping.IsEmpty() || !aRecord.ItemUidMapping.IsEmpty())
  {
    myRecords.Append(std::move(aRecord));
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordItemUid(
  const TCollection_AsciiString&               theOpLabel,
  const BRepGraph_ItemUID&                     theOriginal,
  const NCollection_Array1<BRepGraph_ItemUID>& theReplacements,
  const BRepGraph_LayerHistory::Kind           theKind)
{
  if (!myEnabled || !theOriginal.IsValid())
  {
    return;
  }

  myItemUidKnownInputs.Add(theOriginal);

  if (theReplacements.IsEmpty())
  {
    NCollection_LinearVector<BRepGraph_ItemUID> aDeleted(THE_HISTORY_DERIVED_BLOCK_SIZE);
    aDeleted.Append(theOriginal);
    RecordDeletedItemUid(theOpLabel, aDeleted.ToArray1());
    return;
  }

  Standard_ASSERT_VOID(theKind != BRepGraph_LayerHistory::Kind::Deleted,
                       "RecordItemUid: use RecordDeletedItemUid() for deletions");

  NCollection_LinearVector<BRepGraph_ItemUID> aValidReplacements(THE_HISTORY_FILTERED_BLOCK_SIZE);
  for (const BRepGraph_ItemUID& aDerived : theReplacements)
  {
    if (aDerived.IsValid())
    {
      appendUniqueItemUid(aValidReplacements, aDerived);
    }
  }
  if (aValidReplacements.IsEmpty())
  {
    return;
  }

  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = theKind;
  aRecord.ItemUidMapping.Bind(theOriginal, copyItemUids(aValidReplacements));
  myRecords.Append(std::move(aRecord));

  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>& aFwd =
    selectItemUidForwardMap(theKind, myItemUidOriginalToModified, myItemUidOriginalToGenerated);
  for (const BRepGraph_ItemUID& aDerived : aValidReplacements)
  {
    appendItemUidForward(aFwd, theOriginal, aDerived);
  }

  if (theKind == BRepGraph_LayerHistory::Kind::Replaced)
  {
    myItemUidDeleted.Add(theOriginal);
  }
  touch();
}

//=================================================================================================

void BRepGraph_LayerHistory::RecordDeletedItemUid(
  const TCollection_AsciiString&               theOpLabel,
  const NCollection_Array1<BRepGraph_ItemUID>& theDeleted)
{
  if (!myEnabled || theDeleted.IsEmpty())
  {
    return;
  }

  BRepGraph_LayerHistory::Event aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  aRecord.RecordKind     = BRepGraph_LayerHistory::Kind::Deleted;
  aRecord.ItemUidMapping.ReSize(theDeleted.Size());

  for (const BRepGraph_ItemUID& aUid : theDeleted)
  {
    if (!aUid.IsValid())
    {
      continue;
    }
    myItemUidKnownInputs.Add(aUid);
    myItemUidDeleted.Add(aUid);
    if (!aRecord.ItemUidMapping.IsBound(aUid))
    {
      aRecord.ItemUidMapping.Bind(
        aUid,
        NCollection_LinearVector<BRepGraph_ItemUID>(THE_HISTORY_REPLACEMENT_BLOCK_SIZE));
    }
  }

  if (!aRecord.ItemUidMapping.IsEmpty())
  {
    myRecords.Append(std::move(aRecord));
    touch();
  }
}

namespace
{
//! Translate an NCollection_List<TopoDS_Shape> into a NodeId vector by
//! looking up each shape in @p theOutputs.  Missing entries are silently
//! dropped (see Absorb's contract for why).
inline NCollection_LinearVector<BRepGraph_NodeId> resolveImages(
  const NCollection_List<TopoDS_Shape>&                                               theImages,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs)
{
  NCollection_LinearVector<BRepGraph_NodeId> aResult(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
  if (theImages.IsEmpty())
  {
    return aResult;
  }
  for (NCollection_List<TopoDS_Shape>::Iterator anIt(theImages); anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& anImage = anIt.Value();
    if (theOutputs.IsBound(anImage))
    {
      appendUniqueNode(aResult, theOutputs.Find(anImage));
    }
  }
  return aResult;
}

inline NCollection_LinearVector<BRepGraph_UID> resolveUidImages(
  const BRepGraph&                      theOutputGraph,
  const NCollection_List<TopoDS_Shape>& theImages,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs)
{
  NCollection_LinearVector<BRepGraph_UID> aResult(THE_HISTORY_REPLACEMENT_BLOCK_SIZE);
  if (theImages.IsEmpty())
  {
    return aResult;
  }
  for (NCollection_List<TopoDS_Shape>::Iterator anIt(theImages); anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& anImage = anIt.Value();
    if (const BRepGraph_NodeId* aNode = theOutputs.Seek(anImage))
    {
      const BRepGraph_UID aUID = theOutputGraph.UIDs().Of(*aNode);
      if (aUID.IsValid())
      {
        appendUniqueUid(aResult, aUID);
      }
    }
  }
  return aResult;
}
} // namespace

//=================================================================================================

void BRepGraph_LayerHistory::Absorb(
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs,
  const occ::handle<BRepTools_History>&                                               theSource,
  const TCollection_AsciiString&                                                      theOpLabel)
{
  if (theSource.IsNull() || theInputs.IsEmpty())
  {
    return;
  }

  NCollection_LinearVector<BRepGraph_NodeId> aDeletedNodes(THE_HISTORY_DERIVED_BLOCK_SIZE);

  for (NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>::Iterator anIt(
         theInputs);
       anIt.More();
       anIt.Next())
  {
    const TopoDS_Shape&    anInputShape = anIt.Key();
    const BRepGraph_NodeId anInputNode  = anIt.Value();

    // Deleted has precedence: an input that was both removed *and* (per OCCT
    // bug) listed as Modified/Generated is still a deletion event.
    if (theSource->IsRemoved(anInputShape))
    {
      aDeletedNodes.Append(anInputNode);
      continue;
    }

    {
      const NCollection_List<TopoDS_Shape>&      aModified = theSource->Modified(anInputShape);
      NCollection_LinearVector<BRepGraph_NodeId> aReplacements =
        resolveImages(aModified, theOutputs);
      if (!aReplacements.IsEmpty())
      {
        Record(theOpLabel,
               anInputNode,
               aReplacements.ToArray1(),
               BRepGraph_LayerHistory::Kind::Modified);
      }
    }

    {
      const NCollection_List<TopoDS_Shape>&      aGenerated = theSource->Generated(anInputShape);
      NCollection_LinearVector<BRepGraph_NodeId> aReplacements =
        resolveImages(aGenerated, theOutputs);
      if (!aReplacements.IsEmpty())
      {
        Record(theOpLabel,
               anInputNode,
               aReplacements.ToArray1(),
               BRepGraph_LayerHistory::Kind::Generated);
      }
    }
  }

  if (!aDeletedNodes.IsEmpty())
  {
    RecordDeleted(theOpLabel, aDeletedNodes.ToArray1());
  }
}

//=================================================================================================

void BRepGraph_LayerHistory::Absorb(
  const BRepGraph& theInputGraph,
  const BRepGraph& theOutputGraph,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs,
  const occ::handle<BRepTools_History>&                                               theSource,
  const TCollection_AsciiString&                                                      theOpLabel)
{
  if (theSource.IsNull() || theInputs.IsEmpty())
  {
    return;
  }

  NCollection_LinearVector<BRepGraph_UID> aDeletedUids(THE_HISTORY_DERIVED_BLOCK_SIZE);

  for (NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>::Iterator anIt(
         theInputs);
       anIt.More();
       anIt.Next())
  {
    const TopoDS_Shape& anInputShape = anIt.Key();
    const BRepGraph_UID anInputUid   = theInputGraph.UIDs().Of(anIt.Value());
    if (!anInputUid.IsValid())
    {
      continue;
    }

    myUidKnownInputs.Add(anInputUid);

    if (theSource->IsRemoved(anInputShape))
    {
      aDeletedUids.Append(anInputUid);
      continue;
    }

    {
      const NCollection_List<TopoDS_Shape>&   aModified = theSource->Modified(anInputShape);
      NCollection_LinearVector<BRepGraph_UID> aReplacements =
        resolveUidImages(theOutputGraph, aModified, theOutputs);
      if (!aReplacements.IsEmpty())
      {
        RecordUid(theOpLabel,
                  anInputUid,
                  aReplacements.ToArray1(),
                  BRepGraph_LayerHistory::Kind::Modified);
      }
    }

    {
      const NCollection_List<TopoDS_Shape>&   aGenerated = theSource->Generated(anInputShape);
      NCollection_LinearVector<BRepGraph_UID> aReplacements =
        resolveUidImages(theOutputGraph, aGenerated, theOutputs);
      if (!aReplacements.IsEmpty())
      {
        RecordUid(theOpLabel,
                  anInputUid,
                  aReplacements.ToArray1(),
                  BRepGraph_LayerHistory::Kind::Generated);
      }
    }
  }

  if (!aDeletedUids.IsEmpty())
  {
    RecordDeletedUid(theOpLabel, aDeletedUids.ToArray1());
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_LayerHistory::FindOriginal(const BRepGraph_NodeId theModified) const
{
  // Walk the reverse map iteratively until a root node is reached.
  // Limit iterations to the map extent to protect against cycles.
  BRepGraph_NodeId aCurrent = theModified;
  size_t           aMaxIter = static_cast<size_t>(myDerivedToOriginals.Extent());
  while (myDerivedToOriginals.IsBound(aCurrent) && aMaxIter > 0)
  {
    const NCollection_LinearVector<BRepGraph_NodeId>& anOrigins =
      myDerivedToOriginals.Find(aCurrent);
    if (anOrigins.IsEmpty())
    {
      break;
    }
    const BRepGraph_NodeId& anOriginal = anOrigins.First();
    if (anOriginal == aCurrent)
    {
      break;
    }
    aCurrent = anOriginal;
    --aMaxIter;
  }
  return aCurrent;
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_NodeId> BRepGraph_LayerHistory::FindDerived(
  const BRepGraph_NodeId theOriginal) const
{
  // Collect every transitively derived node (Modified U Generated) in
  // breadth-first order.  A visited set guards against infinite loops if
  // cycles exist in the forward maps.  @p theOriginal itself is excluded
  // from the result.
  NCollection_LinearVector<BRepGraph_NodeId> aResult(THE_HISTORY_DERIVED_BLOCK_SIZE);
  NCollection_LinearVector<BRepGraph_NodeId> aQueue(THE_HISTORY_QUEUE_BLOCK_SIZE);
  NCollection_FlatMap<BRepGraph_NodeId>      aVisited;

  aQueue.Append(theOriginal);
  aVisited.Add(theOriginal);

  size_t aFront = 0;
  while (aFront < aQueue.Size())
  {
    const BRepGraph_NodeId aNode = aQueue.Value(aFront++);
    if (aNode != theOriginal)
    {
      aResult.Append(aNode);
    }

    auto enqueue = [&](const NCollection_LinearVector<BRepGraph_NodeId>& theArr) {
      for (const BRepGraph_NodeId& aDerived : theArr)
      {
        if (aVisited.Add(aDerived))
        {
          aQueue.Append(aDerived);
        }
      }
    };
    if (myOriginalToModified.IsBound(aNode))
    {
      enqueue(myOriginalToModified.Find(aNode));
    }
    if (myOriginalToGenerated.IsBound(aNode))
    {
      enqueue(myOriginalToGenerated.Find(aNode));
    }
  }

  return aResult;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_NodeId>* BRepGraph_LayerHistory::FindModified(
  const BRepGraph_NodeId theOriginal) const
{
  return myOriginalToModified.IsBound(theOriginal) ? &myOriginalToModified.Find(theOriginal)
                                                   : nullptr;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_NodeId>* BRepGraph_LayerHistory::FindGenerated(
  const BRepGraph_NodeId theOriginal) const
{
  return myOriginalToGenerated.IsBound(theOriginal) ? &myOriginalToGenerated.Find(theOriginal)
                                                    : nullptr;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_UID>* BRepGraph_LayerHistory::FindModified(
  const BRepGraph_UID& theUID) const
{
  return myUidOriginalToModified.IsBound(theUID) ? &myUidOriginalToModified.Find(theUID) : nullptr;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_NodeId>* BRepGraph_LayerHistory::FindOriginals(
  const BRepGraph_NodeId theDerived) const
{
  return myDerivedToOriginals.IsBound(theDerived) ? &myDerivedToOriginals.Find(theDerived)
                                                  : nullptr;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_UID>* BRepGraph_LayerHistory::FindGenerated(
  const BRepGraph_UID& theUID) const
{
  return myUidOriginalToGenerated.IsBound(theUID) ? &myUidOriginalToGenerated.Find(theUID)
                                                  : nullptr;
}

//=================================================================================================

bool BRepGraph_LayerHistory::IsDeleted(const BRepGraph_UID& theUID) const
{
  return myUidDeleted.Contains(theUID);
}

//=================================================================================================

const NCollection_FlatMap<BRepGraph_UID>& BRepGraph_LayerHistory::DeletedUids() const
{
  return myUidDeleted;
}

//=================================================================================================

bool BRepGraph_LayerHistory::HasKnownInput(const BRepGraph_UID& theUID) const
{
  return myUidKnownInputs.Contains(theUID);
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ItemUID>* BRepGraph_LayerHistory::FindModified(
  const BRepGraph_ItemUID& theUID) const
{
  return myItemUidOriginalToModified.IsBound(theUID) ? &myItemUidOriginalToModified.Find(theUID)
                                                     : nullptr;
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ItemUID>* BRepGraph_LayerHistory::FindGenerated(
  const BRepGraph_ItemUID& theUID) const
{
  return myItemUidOriginalToGenerated.IsBound(theUID) ? &myItemUidOriginalToGenerated.Find(theUID)
                                                      : nullptr;
}

//=================================================================================================

bool BRepGraph_LayerHistory::IsDeleted(const BRepGraph_ItemUID& theUID) const
{
  return myItemUidDeleted.Contains(theUID);
}

//=================================================================================================

const NCollection_FlatMap<BRepGraph_ItemUID>& BRepGraph_LayerHistory::DeletedItemUids() const
{
  return myItemUidDeleted;
}

//=================================================================================================

bool BRepGraph_LayerHistory::HasKnownInput(const BRepGraph_ItemUID& theUID) const
{
  return myItemUidKnownInputs.Contains(theUID);
}

//=================================================================================================

bool BRepGraph_LayerHistory::IsDeleted(const BRepGraph_NodeId theOriginal) const
{
  return myDeleted.Contains(theOriginal);
}

//=================================================================================================

const NCollection_FlatMap<BRepGraph_NodeId>& BRepGraph_LayerHistory::DeletedNodes() const
{
  return myDeleted;
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_UID> BRepGraph_LayerHistory::FindModified(
  const BRepGraph&     theGraph,
  const BRepGraph_UID& theUID) const
{
  NCollection_LinearVector<BRepGraph_UID> aResult(THE_HISTORY_DERIVED_BLOCK_SIZE);
  if (const NCollection_LinearVector<BRepGraph_UID>* aUidVec = FindModified(theUID))
  {
    return *aUidVec;
  }
  const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(theUID);
  if (!aNode.IsValid())
  {
    return aResult;
  }
  if (const NCollection_LinearVector<BRepGraph_NodeId>* aVec = FindModified(aNode))
  {
    for (const BRepGraph_NodeId& aDer : *aVec)
    {
      const BRepGraph_UID aUID = theGraph.UIDs().Of(aDer);
      if (aUID.IsValid())
      {
        aResult.Append(aUID);
      }
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_UID> BRepGraph_LayerHistory::FindGenerated(
  const BRepGraph&     theGraph,
  const BRepGraph_UID& theUID) const
{
  NCollection_LinearVector<BRepGraph_UID> aResult(THE_HISTORY_DERIVED_BLOCK_SIZE);
  if (const NCollection_LinearVector<BRepGraph_UID>* aUidVec = FindGenerated(theUID))
  {
    return *aUidVec;
  }
  const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(theUID);
  if (!aNode.IsValid())
  {
    return aResult;
  }
  if (const NCollection_LinearVector<BRepGraph_NodeId>* aVec = FindGenerated(aNode))
  {
    for (const BRepGraph_NodeId& aDer : *aVec)
    {
      const BRepGraph_UID aUID = theGraph.UIDs().Of(aDer);
      if (aUID.IsValid())
      {
        aResult.Append(aUID);
      }
    }
  }
  return aResult;
}

//=================================================================================================

bool BRepGraph_LayerHistory::IsDeleted(const BRepGraph& theGraph, const BRepGraph_UID& theUID) const
{
  if (IsDeleted(theUID))
  {
    return true;
  }
  const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(theUID);
  if (!aNode.IsValid())
  {
    return false;
  }
  return myDeleted.Contains(aNode);
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_UID> BRepGraph_LayerHistory::DeletedUids(
  const BRepGraph& theGraph) const
{
  NCollection_LinearVector<BRepGraph_UID> aResult(myDeleted.Size() + myUidDeleted.Size());
  for (NCollection_FlatMap<BRepGraph_UID>::Iterator anIt(myUidDeleted); anIt.More(); anIt.Next())
  {
    appendUniqueUid(aResult, anIt.Value());
  }
  for (NCollection_FlatMap<BRepGraph_NodeId>::Iterator anIt(myDeleted); anIt.More(); anIt.Next())
  {
    const BRepGraph_UID aUID = theGraph.UIDs().Of(anIt.Value());
    if (aUID.IsValid())
    {
      appendUniqueUid(aResult, aUID);
    }
  }
  return aResult;
}

//=================================================================================================

size_t BRepGraph_LayerHistory::NbRecords() const
{
  return myRecords.Size();
}

//=================================================================================================

const BRepGraph_LayerHistory::Event& BRepGraph_LayerHistory::Record(const size_t theRecordIdx) const
{
  return myRecords.Value(theRecordIdx);
}

//=================================================================================================

void BRepGraph_LayerHistory::SetEnabled(const bool theVal)
{
  if (myEnabled == theVal)
  {
    return;
  }
  myEnabled = theVal;
  touch();
}

//=================================================================================================

bool BRepGraph_LayerHistory::IsEnabled() const
{
  return myEnabled;
}

//=================================================================================================

void BRepGraph_LayerHistory::Clear() noexcept
{
  myRecords.Clear(true);
  myDerivedToOriginals.Clear();
  myOriginalToModified.Clear();
  myOriginalToGenerated.Clear();
  myDeleted.Clear();
  myUidOriginalToModified.Clear();
  myUidOriginalToGenerated.Clear();
  myUidKnownInputs.Clear();
  myUidDeleted.Clear();
  myItemUidOriginalToModified.Clear();
  myItemUidOriginalToGenerated.Clear();
  myItemUidKnownInputs.Clear();
  myItemUidDeleted.Clear();
  touch();
}
