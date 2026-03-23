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

#ifndef _BRepGraphAlgo_Compact_HeaderFile
#define _BRepGraphAlgo_Compact_HeaderFile

#include <BRepGraph.hxx>

#include <Standard_DefineAlloc.hxx>

//! @brief Graph compaction algorithm that reclaims removed node slots.
//!
//! After deduplication or other operations that mark nodes as removed,
//! this algorithm rebuilds the graph with dense index arrays, eliminating
//! all removed nodes and reassigning indices to be contiguous.
//!
//! Strategy: rebuild-and-swap. A fresh BRepGraph is constructed from
//! non-removed nodes with remapped indices, then move-assigned into
//! the input graph.
class BRepGraphAlgo_Compact
{
public:
  DEFINE_STANDARD_ALLOC

  //! Configuration for compaction.
  struct Options
  {
    bool HistoryMode = true; //!< Record index remapping in history.
  };

  //! Result counters for diagnostics.
  struct Result
  {
    int NbRemovedVertices   = 0;
    int NbRemovedEdges      = 0;
    int NbRemovedWires      = 0;
    int NbRemovedFaces      = 0;
    int NbRemovedShells     = 0;
    int NbRemovedSolids     = 0;
    int NbRemovedCompounds  = 0;
    int NbRemovedCompSolids = 0;
    int NbRemovedSurfaces   = 0;
    int NbRemovedCurves     = 0;
    int NbRemovedPCurves    = 0;
    int NbNodesBefore       = 0;
    int NbNodesAfter        = 0;
  };

  //! Run compaction with default options.
  //! @param[in,out] theGraph graph to compact
  //! @return compaction statistics
  Standard_EXPORT static Result Perform(BRepGraph& theGraph);

  //! Run compaction with specified options.
  //! @param[in,out] theGraph graph to compact
  //! @param[in] theOptions compaction configuration
  //! @return compaction statistics
  Standard_EXPORT static Result Perform(BRepGraph& theGraph, const Options& theOptions);

private:
  BRepGraphAlgo_Compact() = delete;
};

#endif // _BRepGraphAlgo_Compact_HeaderFile
