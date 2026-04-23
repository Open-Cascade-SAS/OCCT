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

#include <BRepGraph_History.hxx>

#include <NCollection_Map.hxx>

namespace
{
constexpr int THE_HISTORY_RECORD_BLOCK_SIZE      = 32;
constexpr int THE_HISTORY_REPLACEMENT_BLOCK_SIZE = 4;
constexpr int THE_HISTORY_FILTERED_BLOCK_SIZE    = 4;
constexpr int THE_HISTORY_DERIVED_BLOCK_SIZE     = 8;
constexpr int THE_HISTORY_QUEUE_BLOCK_SIZE       = 32;
} // namespace

//=================================================================================================

void BRepGraph_History::SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc)
{
  Standard_ASSERT_VOID(myRecords.IsEmpty(),
                       "SetAllocator: must be called before any records are added");
  myAllocator = theAlloc;
  // Reconstruct internal containers with the new allocator.
  myRecords =
    NCollection_Vector<BRepGraph_HistoryRecord>(THE_HISTORY_RECORD_BLOCK_SIZE, myAllocator);
  myDerivedToOriginal = NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>(1, myAllocator);
  myOriginalToDerived =
    NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>(1, myAllocator);
}

//=================================================================================================

void BRepGraph_History::Record(const TCollection_AsciiString&              theOpLabel,
                               const BRepGraph_NodeId                      theOriginal,
                               const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  if (!myEnabled)
  {
    return;
  }

  // Append a new history record.
  BRepGraph_HistoryRecord aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  if (!myAllocator.IsNull())
  {
    aRecord.Mapping =
      NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>(1, myAllocator);
  }
  aRecord.Mapping.Bind(theOriginal, theReplacements);
  myRecords.Append(std::move(aRecord));

  // Populate the bidirectional lookup maps.
  // Skip self-referencing entries (aDerived == theOriginal) to avoid overwriting
  // prior chain links in the reverse map.
  NCollection_Vector<BRepGraph_NodeId> aFilteredReplacements(THE_HISTORY_FILTERED_BLOCK_SIZE,
                                                             myAllocator);
  for (const BRepGraph_NodeId& aDerived : theReplacements)
  {
    if (aDerived != theOriginal)
    {
      myDerivedToOriginal.Bind(aDerived, theOriginal);
      aFilteredReplacements.Append(aDerived);
    }
  }

  if (aFilteredReplacements.IsEmpty())
    return;

  if (myOriginalToDerived.IsBound(theOriginal))
  {
    NCollection_Vector<BRepGraph_NodeId>& aDerivedVec = myOriginalToDerived.ChangeFind(theOriginal);
    for (const BRepGraph_NodeId& aDerived : aFilteredReplacements)
    {
      aDerivedVec.Append(aDerived);
    }
  }
  else
  {
    myOriginalToDerived.Bind(theOriginal, std::move(aFilteredReplacements));
  }
}

//=================================================================================================

void BRepGraph_History::RecordBatch(const TCollection_AsciiString&              theOpLabel,
                                    const NCollection_Vector<BRepGraph_NodeId>& theOriginals,
                                    const NCollection_Vector<BRepGraph_NodeId>& theReplacements,
                                    const TCollection_AsciiString&              theExtraInfo)
{
  Standard_ASSERT_VOID(theOriginals.Size() == theReplacements.Size(),
                       "RecordBatch: mismatched vector lengths");
  if (!myEnabled || theOriginals.IsEmpty())
  {
    return;
  }

  const size_t aNbPairs = theOriginals.Size();

  // Create a single history record with all mappings.
  // Pre-size the Mapping to avoid DataMap rehashing.
  BRepGraph_HistoryRecord aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Size();
  if (!myAllocator.IsNull())
  {
    aRecord.Mapping =
      NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>(aNbPairs,
                                                                                  myAllocator);
  }
  else
  {
    aRecord.Mapping.ReSize(aNbPairs);
  }
  aRecord.ExtraInfo = theExtraInfo;

  // Build mapping: each pair creates a 1-element replacement vector.
  {
    NCollection_Vector<BRepGraph_NodeId>::Iterator anOrigIt(theOriginals);
    NCollection_Vector<BRepGraph_NodeId>::Iterator aReplIt(theReplacements);
    for (; anOrigIt.More(); anOrigIt.Next(), aReplIt.Next())
    {
      const BRepGraph_NodeId& anOriginal   = anOrigIt.Value();
      const BRepGraph_NodeId& aReplacement = aReplIt.Value();
      Standard_ASSERT_VOID(!aRecord.Mapping.IsBound(anOriginal),
                           "RecordBatch: duplicate original node");
      NCollection_Vector<BRepGraph_NodeId> aRepVec(THE_HISTORY_REPLACEMENT_BLOCK_SIZE,
                                                   myAllocator);
      aRepVec.Append(aReplacement);
      aRecord.Mapping.Bind(anOriginal, std::move(aRepVec));
    }
  }
  myRecords.Append(std::move(aRecord));

  // Update bidirectional lookup maps in bulk.
  // Pre-size to avoid rehashing during batch insert.
  myDerivedToOriginal.ReSize(myDerivedToOriginal.Extent() + aNbPairs);
  myOriginalToDerived.ReSize(myOriginalToDerived.Extent() + aNbPairs);

  {
    NCollection_Vector<BRepGraph_NodeId>::Iterator anOrigIt(theOriginals);
    NCollection_Vector<BRepGraph_NodeId>::Iterator aReplIt(theReplacements);
    for (; anOrigIt.More(); anOrigIt.Next(), aReplIt.Next())
    {
      const BRepGraph_NodeId& anOriginal   = anOrigIt.Value();
      const BRepGraph_NodeId& aReplacement = aReplIt.Value();
      if (aReplacement == anOriginal)
      {
        continue;
      }

      myDerivedToOriginal.Bind(aReplacement, anOriginal);

      if (myOriginalToDerived.IsBound(anOriginal))
      {
        myOriginalToDerived.ChangeFind(anOriginal).Append(aReplacement);
      }
      else
      {
        NCollection_Vector<BRepGraph_NodeId> aDerVec(THE_HISTORY_REPLACEMENT_BLOCK_SIZE,
                                                     myAllocator);
        aDerVec.Append(aReplacement);
        myOriginalToDerived.Bind(anOriginal, std::move(aDerVec));
      }
    }
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_History::FindOriginal(const BRepGraph_NodeId theModified) const
{
  // Walk the reverse map iteratively until a root node is reached.
  // Limit iterations to the map extent to protect against cycles.
  BRepGraph_NodeId aCurrent = theModified;
  int              aMaxIter = myDerivedToOriginal.Extent();
  while (myDerivedToOriginal.IsBound(aCurrent) && aMaxIter-- > 0)
  {
    const BRepGraph_NodeId& anOriginal = myDerivedToOriginal.Find(aCurrent);
    if (anOriginal == aCurrent)
    {
      break;
    }
    aCurrent = anOriginal;
  }
  return aCurrent;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_History::FindDerived(
  const BRepGraph_NodeId theOriginal) const
{
  // Collect all transitively derived nodes using iterative BFS.
  // A visited set guards against infinite loops if cycles exist in the forward map.
  NCollection_Vector<BRepGraph_NodeId> aResult(THE_HISTORY_DERIVED_BLOCK_SIZE);
  NCollection_Vector<BRepGraph_NodeId> aQueue(THE_HISTORY_QUEUE_BLOCK_SIZE);
  NCollection_Map<BRepGraph_NodeId>    aVisited;

  aQueue.Append(theOriginal);
  aVisited.Add(theOriginal);

  size_t aFront = 0;
  while (aFront < aQueue.Size())
  {
    const BRepGraph_NodeId aNode = aQueue.Value(aFront++);
    if (!myOriginalToDerived.IsBound(aNode))
    {
      // Leaf node: only add if it is not the initial query node itself.
      if (aNode != theOriginal)
      {
        aResult.Append(aNode);
      }
      continue;
    }

    const NCollection_Vector<BRepGraph_NodeId>& aDirectDerived = myOriginalToDerived.Find(aNode);
    for (const BRepGraph_NodeId& aDerived : aDirectDerived)
    {
      if (aVisited.Add(aDerived))
      {
        aQueue.Append(aDerived);
      }
    }
  }

  // If there were no transitive leaves but there are direct derived nodes,
  // return the direct derived for the non-recursive case.
  if (aResult.IsEmpty() && myOriginalToDerived.IsBound(theOriginal))
  {
    const NCollection_Vector<BRepGraph_NodeId>& aDirectDerived =
      myOriginalToDerived.Find(theOriginal);
    for (const BRepGraph_NodeId& aDerived : aDirectDerived)
    {
      aResult.Append(aDerived);
    }
  }

  return aResult;
}

//=================================================================================================

size_t BRepGraph_History::NbRecords() const
{
  return myRecords.Size();
}

//=================================================================================================

const BRepGraph_HistoryRecord& BRepGraph_History::Record(const size_t theRecordIdx) const
{
  return myRecords.Value(theRecordIdx);
}

//=================================================================================================

void BRepGraph_History::SetEnabled(const bool theVal)
{
  myEnabled = theVal;
}

//=================================================================================================

bool BRepGraph_History::IsEnabled() const
{
  return myEnabled;
}

//=================================================================================================

void BRepGraph_History::Clear()
{
  myRecords.Clear();
  myDerivedToOriginal.Clear();
  myOriginalToDerived.Clear();
}
