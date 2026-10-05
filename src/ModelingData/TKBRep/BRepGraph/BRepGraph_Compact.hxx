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

#ifndef _BRepGraph_Compact_HeaderFile
#define _BRepGraph_Compact_HeaderFile

#include <BRepGraph.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Graph compaction algorithm that reclaims removed node slots.
//!
//! After deduplication or other operations that mark nodes as removed,
//! this algorithm rebuilds the graph with dense index arrays, eliminating
//! all removed nodes and reassigning indices to be contiguous.
//!
//! The implementation may either compact storage in place or rebuild-and-swap;
//! the public contract is that active nodes and references become dense.
class BRepGraph_Compact
{
public:
  DEFINE_STANDARD_ALLOC

  //! Configuration for compaction.
  struct Options
  {
    enum class ValidationPolicy
    {
      Lightweight, //!< Run normal graph-integrity validation before swapping rebuilt storage.
      Audit        //!< Include audit-only graph checks such as assembly cycle detection.
    };

    ValidationPolicy ValidationMode = ValidationPolicy::Lightweight; //!< Pre-swap validation mode.
  };

  enum class Status
  {
    Done,
    NoChange,
    InvalidGraph,
    CandidateValidationFailed,
    MigrationFailed
  };

  //! Result counters for diagnostics.
  struct Result
  {
    Status   StatusCode           = Status::InvalidGraph;
    uint32_t NbRemovedVertices    = 0;
    uint32_t NbRemovedEdges       = 0;
    uint32_t NbRemovedCoEdges     = 0;
    uint32_t NbRemovedWires       = 0;
    uint32_t NbRemovedFaces       = 0;
    uint32_t NbRemovedShells      = 0;
    uint32_t NbRemovedSolids      = 0;
    uint32_t NbRemovedCompounds   = 0;
    uint32_t NbRemovedCompSolids  = 0;
    uint32_t NbRemovedProducts    = 0;
    uint32_t NbRemovedOccurrences = 0;
    uint32_t NbRemovedRefs        = 0;
    uint32_t NbRemovedReps        = 0;
    uint32_t NbRemovedSurfaces    = 0;
    uint32_t NbRemovedCurves      = 0;
    uint32_t NbNodesBefore        = 0;
    uint32_t NbNodesAfter         = 0;
    uint32_t NbUnmappedActiveDefs =
      0; //!< Active defs not present in any remap (orphans + drop-outs).

    [[nodiscard]] bool IsDone() const noexcept
    {
      return StatusCode == Status::Done || StatusCode == Status::NoChange;
    }
  };

  //! Run compaction with default options.
  //! @param[in,out] theGraph graph to compact
  //! @return compaction statistics
  [[nodiscard]] Standard_EXPORT static Result Perform(BRepGraph& theGraph);

  //! Run compaction with specified options.
  //! @param[in,out] theGraph graph to compact
  //! @param[in] theOptions compaction configuration
  //! @return compaction statistics
  [[nodiscard]] Standard_EXPORT static Result Perform(BRepGraph&     theGraph,
                                                      const Options& theOptions);

  BRepGraph_Compact() = delete;
};

#endif // _BRepGraph_Compact_HeaderFile
