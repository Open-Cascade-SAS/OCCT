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

#ifndef _BRepGraph_Replace_HeaderFile
#define _BRepGraph_Replace_HeaderFile

#include <BRepGraph_CopyRemap.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cstdint>

class BRepGraph;

//! All-or-nothing replacement of persistent graph data with runtime-service migration.
//!
//! The target BRepGraph wrapper remains at the same address. Persistent topology,
//! representations, supplemental data, and persistent layers come from the replacement.
//! Runtime and derived layers plus fresh graph caches migrate from the active graph through
//! durable UID remapping.
class BRepGraph_Replace
{
public:
  DEFINE_STANDARD_ALLOC

  enum class Status : std::uint8_t
  {
    Done,
    InvalidGraph,
    DifferentGraphIdentity,
    MigrationFailed
  };

  struct Result
  {
    Status   StatusCode    = Status::InvalidGraph;
    uint32_t NbMappedNodes = 0;
    uint32_t NbMappedRefs  = 0;

    [[nodiscard]] bool IsDone() const noexcept { return StatusCode == Status::Done; }
  };

  //! Replace persistent graph data and migrate valid runtime data from the active graph.
  //! @param[in,out] theGraph stable graph wrapper whose persistent data is replaced
  //! @param[in,out] theReplacement complete replacement graph
  //! @return replacement status and durable remap statistics
  [[nodiscard]] Standard_EXPORT static Result Perform(BRepGraph&  theGraph,
                                                      BRepGraph&& theReplacement);

  //! Install compacted core storage and migrate graph-owned components through their contracts.
  [[nodiscard]] Standard_EXPORT static Result PerformCompaction(
    BRepGraph&                          theGraph,
    BRepGraph&&                         theCompactedCore,
    const BRepGraph_CopyRemap::ItemMap& theItemRemap);
};

#endif // _BRepGraph_Replace_HeaderFile
