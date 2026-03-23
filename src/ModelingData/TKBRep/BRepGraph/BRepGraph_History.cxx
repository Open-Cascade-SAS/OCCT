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

//=================================================================================================

void BRepGraph_History::Record(const TCollection_AsciiString&              theOpLabel,
                               BRepGraph_NodeId                            theOriginal,
                               const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  if (!myEnabled)
  {
    return;
  }

  // Append a new history record.
  BRepGraph_HistoryRecord aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Length();
  aRecord.Mapping.Bind(theOriginal, theReplacements);
  myRecords.Append(aRecord);

  // Populate the bidirectional lookup maps.
  // Skip self-referencing entries (aDerived == theOriginal) to avoid overwriting
  // prior chain links in the reverse map.
  NCollection_Vector<BRepGraph_NodeId> aFilteredReplacements;
  for (int anIdx = 0; anIdx < theReplacements.Length(); ++anIdx)
  {
    const BRepGraph_NodeId& aDerived = theReplacements.Value(anIdx);
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
    for (int anIdx = 0; anIdx < aFilteredReplacements.Length(); ++anIdx)
    {
      aDerivedVec.Append(aFilteredReplacements.Value(anIdx));
    }
  }
  else
  {
    myOriginalToDerived.Bind(theOriginal, aFilteredReplacements);
  }
}

//=================================================================================================

void BRepGraph_History::RecordBatch(const TCollection_AsciiString&              theOpLabel,
                                    const NCollection_Vector<BRepGraph_NodeId>& theOriginals,
                                    const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  Standard_ASSERT_VOID(theOriginals.Length() == theReplacements.Length(),
                       "RecordBatch: mismatched vector lengths");
  if (!myEnabled || theOriginals.IsEmpty())
  {
    return;
  }

  // Create a single history record with all mappings.
  BRepGraph_HistoryRecord aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myRecords.Length();

  const int aNbPairs = theOriginals.Length();
  for (int i = 0; i < aNbPairs; ++i)
  {
    const BRepGraph_NodeId& anOriginal   = theOriginals.Value(i);
    const BRepGraph_NodeId& aReplacement = theReplacements.Value(i);
    Standard_ASSERT_VOID(!aRecord.Mapping.IsBound(anOriginal),
                         "RecordBatch: duplicate original node");
    NCollection_Vector<BRepGraph_NodeId> aRepVec;
    aRepVec.Append(aReplacement);
    aRecord.Mapping.Bind(anOriginal, aRepVec);
  }
  myRecords.Append(aRecord);

  // Update bidirectional lookup maps in bulk.
  for (int i = 0; i < aNbPairs; ++i)
  {
    const BRepGraph_NodeId& anOriginal   = theOriginals.Value(i);
    const BRepGraph_NodeId& aReplacement = theReplacements.Value(i);
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
      NCollection_Vector<BRepGraph_NodeId> aDerVec;
      aDerVec.Append(aReplacement);
      myOriginalToDerived.Bind(anOriginal, aDerVec);
    }
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_History::FindOriginal(BRepGraph_NodeId theModified) const
{
  // Walk the reverse map recursively until a root node is reached.
  BRepGraph_NodeId aCurrent = theModified;
  while (myDerivedToOriginal.IsBound(aCurrent))
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph_History::FindDerived(BRepGraph_NodeId theOriginal) const
{
  // Collect all transitively derived nodes using iterative BFS.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_Vector<BRepGraph_NodeId> aQueue;
  aQueue.Append(theOriginal);

  for (int aQueueIdx = 0; aQueueIdx < aQueue.Length(); ++aQueueIdx)
  {
    const BRepGraph_NodeId aNode = aQueue.Value(aQueueIdx);
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
    for (int anIdx = 0; anIdx < aDirectDerived.Length(); ++anIdx)
    {
      aQueue.Append(aDirectDerived.Value(anIdx));
    }
  }

  // If there were no transitive leaves but there are direct derived nodes,
  // return the direct derived for the non-recursive case.
  if (aResult.IsEmpty() && myOriginalToDerived.IsBound(theOriginal))
  {
    return myOriginalToDerived.Find(theOriginal);
  }

  return aResult;
}

//=================================================================================================

int BRepGraph_History::NbRecords() const
{
  return myRecords.Length();
}

//=================================================================================================

const BRepGraph_HistoryRecord& BRepGraph_History::Record(int theIdx) const
{
  return myRecords.Value(theIdx);
}

//=================================================================================================

void BRepGraph_History::SetEnabled(bool theVal)
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
