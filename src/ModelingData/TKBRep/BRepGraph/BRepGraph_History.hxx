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

#ifndef _BRepGraph_History_HeaderFile
#define _BRepGraph_History_HeaderFile

#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TCollection_AsciiString.hxx>

class BRepGraph;

//! Extracted history subsystem for BRepGraph.
//!
//! BRepGraph_History maintains an append-only log of modification events
//! and bidirectional lookup maps (original <-> derived) for efficient
//! history queries.  Recording can be toggled on/off at runtime.
class BRepGraph_History
{
  friend class BRepGraph;

public:

  DEFINE_STANDARD_ALLOC

  //! Record a modification: theOriginal was replaced by theReplacements.
  //! @param[in] theOpLabel      human-readable operation name
  //! @param[in] theOriginal     node id before the operation
  //! @param[in] theReplacements node ids after the operation
  Standard_EXPORT void Record(const TCollection_AsciiString&              theOpLabel,
                              BRepGraph_NodeId                            theOriginal,
                              const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  //! Record a batch of 1-to-1 modifications in a single history event.
  //! theOriginals[i] was replaced by theReplacements[i].
  //! More efficient than calling Record() in a loop: creates one HistoryRecord
  //! and updates the bidirectional maps with minimal overhead.
  //! @param[in] theOpLabel      human-readable operation name
  //! @param[in] theOriginals    node ids before the operation
  //! @param[in] theReplacements node ids after the operation (same length)
  Standard_EXPORT void RecordBatch(const TCollection_AsciiString&             theOpLabel,
                                   const NCollection_Vector<BRepGraph_NodeId>& theOriginals,
                                   const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  //! Walk backwards from a modified node to its original.
  //! Follows the reverse map recursively until a root is reached.
  //! @param[in] theModified node id to trace back
  //! @return the root original node id, or theModified itself if not found
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;

  //! Walk forwards from an original node to all derived nodes.
  //! Follows the forward map recursively, collecting all leaves.
  //! @param[in] theOriginal node id to trace forward
  //! @return all transitively derived node ids
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(BRepGraph_NodeId theOriginal) const;

  //! Number of recorded history events.
  //! @return record count
  Standard_EXPORT int NbRecords() const;

  //! Access a record by index (0-based).
  //! @param[in] theIdx index into the records vector
  //! @return the history record at theIdx
  Standard_EXPORT const BRepGraph_HistoryRecord& Record(int theIdx) const;

  //! Enable or disable history recording.
  //! @param[in] theVal true to enable, false to disable
  Standard_EXPORT void SetEnabled(bool theVal);

  //! Query whether history recording is enabled.
  //! @return true if recording is active
  Standard_EXPORT bool IsEnabled() const;

  //! Clear all records and lookup maps.
  Standard_EXPORT void Clear();

private:

  NCollection_Vector<BRepGraph_HistoryRecord> myRecords;

  //! Reverse map: derived node -> original node.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> myDerivedToOriginal;

  //! Forward map: original node -> vector of derived nodes.
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_NodeId>> myOriginalToDerived;

  bool myEnabled = true;
};

#endif // _BRepGraph_History_HeaderFile
