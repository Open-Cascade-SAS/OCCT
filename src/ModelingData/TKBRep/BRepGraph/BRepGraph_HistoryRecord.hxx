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

#ifndef _BRepGraph_HistoryRecord_HeaderFile
#define _BRepGraph_HistoryRecord_HeaderFile

#include <BRepGraph_NodeId.hxx>

#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>

//! One atomic modification event recorded in the graph's history log.
//!
//! A HistoryRecord captures what happened during a single call to
//! BRepGraph::ApplyModification():
//!   - OperationName identifies the algorithm ("Sewing", "FilletEdge", ...).
//!   - SequenceNumber provides total ordering of events.
//!   - Mapping records the topological fate of each affected node:
//!       original -> [replacement1, replacement2, ...]   (split)
//!       original -> [same_node]                         (modified in place)
//!       original -> []                                  (deleted)
//!
//! The history log is append-only within a graph's lifetime.
struct BRepGraph_HistoryRecord
{
  TCollection_AsciiString OperationName;
  size_t                  SequenceNumber = 0;

  //! Key: original node id before the operation.
  //! Value: sequence of replacement node ids after the operation.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>> Mapping;

  //! Optional extra info for diagnostic/debugging purposes.
  //! E.g., merge tolerance, canonical source index.
  TCollection_AsciiString ExtraInfo;
};

#endif // _BRepGraph_HistoryRecord_HeaderFile
