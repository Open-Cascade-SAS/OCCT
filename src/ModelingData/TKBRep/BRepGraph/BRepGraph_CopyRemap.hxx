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

#ifndef _BRepGraph_CopyRemap_HeaderFile
#define _BRepGraph_CopyRemap_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_ItemUID.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cstdint>

class BRepGraph;

//! Immutable context passed to layer copy callbacks.
//!
//! The structural copy algorithm owns remap construction. Layers receive this
//! context and decide how to copy their own representation without exposing layer
//! details back to BRepGraph_Copy.
class BRepGraph_CopyRemap
{
public:
  DEFINE_STANDARD_ALLOC

  //! Distinguishes copy vs. compact migration semantics.
  enum class Mode : std::uint8_t
  {
    Copy    = 0, //!< Full graph copy: source and target are distinct graphs.
    Compact = 1  //!< In-place compaction: layers migrate into the same (rebuilt) graph.
  };

  //! Distinguishes explicit item map vs. identity mapping.
  enum class MappingKind : std::uint8_t
  {
    Explicit = 0, //!< Use theItemRemap for source->target resolution.
    Identity = 1  //!< Source and target ids are identical (full identity copy).
  };

  using ItemMap = NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>;

  BRepGraph_CopyRemap(const BRepGraph& theSourceGraph,
                      BRepGraph&       theTargetGraph,
                      const ItemMap&   theItemRemap,
                      const Mode       theMode) noexcept;

  //! Identity-mapping constructor for full identity copy into an empty target.
  //! Source item ids are returned directly as target item ids after validation.
  BRepGraph_CopyRemap(const BRepGraph& theSourceGraph,
                      BRepGraph&       theTargetGraph,
                      MappingKind      theMappingKind,
                      Mode             theMode) noexcept;

  //! Migration mode of this context.
  [[nodiscard]] Mode CopyMode() const noexcept { return myMode; }

  //! True if this is a compaction migration (not a full copy).
  [[nodiscard]] bool IsCompact() const noexcept { return myMode == Mode::Compact; }

  //! Source graph the copied layer is attached to.
  [[nodiscard]] const BRepGraph& SourceGraph() const noexcept { return *mySourceGraph; }

  //! Target graph whose structural contents have already been copied.
  [[nodiscard]] BRepGraph& TargetGraph() const noexcept { return *myTargetGraph; }

  //! Target graph as const.
  [[nodiscard]] const BRepGraph& TargetGraphConst() const noexcept { return *myTargetGraph; }

  //! Source item id -> target item id map for copied definitions, refs, and reps.
  [[nodiscard]] const ItemMap& Items() const noexcept { return *myItemRemap; }

  //! Return the target item for a source item, or an invalid item if not copied.
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemId
    TargetItem(const BRepGraph_ItemId theSourceItem) const;

  //! Return the target item for a source item, or an invalid item id.
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemId
    TargetItemOrInvalid(const BRepGraph_ItemId theSourceItem) const;

  //! Return true if the source item has a valid copied target item.
  [[nodiscard]] Standard_EXPORT bool HasTargetItem(const BRepGraph_ItemId theSourceItem) const;

  //! Return source UID for a source item.
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemUID
    SourceUID(const BRepGraph_ItemId theSourceItem) const;

  //! Return target UID for a target item.
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemUID
    TargetUID(const BRepGraph_ItemId theTargetItem) const;

  //! Return target UID for a source item by source->target remap.
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemUID
    TargetUIDFromSource(const BRepGraph_ItemId theSourceItem) const;

private:
  const BRepGraph* mySourceGraph = nullptr;
  BRepGraph*       myTargetGraph = nullptr;
  const ItemMap*   myItemRemap   = nullptr;
  Mode             myMode        = Mode::Copy;
  MappingKind      myMappingKind = MappingKind::Explicit;
};

#endif // _BRepGraph_CopyRemap_HeaderFile
