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

#ifndef _BRepGraph_CacheDerivedState_HeaderFile
#define _BRepGraph_CacheDerivedState_HeaderFile

#include <BRepGraph_Cache.hxx>
#include <BRepGraph_NodeId.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

#include <shared_mutex>

class BRepGraphInc_Storage;

//! @brief Cache for derived edge, wire, and shell properties.
//!
//! Stores degeneracy, closure, SameRange, and SameParameter results derived from
//! current BRepGraph topology and geometry. Cache getters compute on read,
//! refresh the stored entries, and expose only fresh values to callers.
class BRepGraph_CacheDerivedState : public BRepGraph_Cache
{
public:
  //! @brief Geometry status of an edge.
  //!
  enum class EdgeGeometryStatus
  {
    HasCurve3D,          //!< Edge has a valid 3D curve
    DegenerateOnSurface, //!< Edge collapses to a point on a face surface
    MissingCurve3D,      //!< Edge has no 3D curve and no valid degenerate evidence
    Invalid              //!< Edge id is invalid or removed
  };

  //! @brief Shell closure status.
  //!
  enum class ShellClosureStatus
  {
    Empty,       //!< Shell has no active faces
    Open,        //!< At least one non-degenerate boundary edge has exactly one use
    Closed,      //!< All non-degenerate boundary edges have exactly two uses
    NonManifold, //!< At least one non-degenerate boundary edge has more than two uses
    Invalid      //!< Shell id is invalid or removed
  };

  //! @brief Cached derived-state entry for an edge.
  //!
  //! Inherits generation-based freshness tracking from NodeEntry and adds
  //! edge-specific derived properties.
  struct EdgeEntry : public NodeEntry
  {
    EdgeGeometryStatus Status    = EdgeGeometryStatus::Invalid; //!< Edge geometry status
    bool               IsClosed  = false; //!< True if start vertex == end vertex
    bool               SameRange = false; //!< True if all PCurve ranges equal 3D curve range
    bool SameParameter           = false; //!< True if PCurves evaluate to the 3D curve on surfaces
  };

  //! @brief Cached derived-state entry for a wire.
  //!
  //! Inherits generation-based freshness tracking from NodeEntry and adds
  //! wire-specific derived properties.
  struct WireEntry : public NodeEntry
  {
    bool IsClosed = false; //!< True if the wire forms a topological loop
  };

  //! @brief Cached derived-state entry for a shell.
  //!
  //! Inherits generation-based freshness tracking from NodeEntry and adds
  //! shell-specific derived properties.
  struct ShellEntry : public NodeEntry
  {
    ShellClosureStatus Status = ShellClosureStatus::Invalid; //!< Shell closure status
  };

  //! Returns the unique cache service GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Returns the unique cache service GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Returns the cache service display name.
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Clears all cached entries.
  Standard_EXPORT void Clear() noexcept override;

  //! Copy fresh, remappable derived-state entries into the target graph.
  Standard_EXPORT void CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! @brief Return edge status, computing and storing a fresh entry.
  //! @param[in]  theEdge  edge definition identifier
  //! @param[out] theEntry filled with a fresh derived entry
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT bool GetEdgeStatus(BRepGraph_EdgeId theEdge, EdgeEntry& theEntry);

  //! @brief Return wire closure, computing and storing a fresh entry.
  //! @param[in]  theWire   wire definition identifier
  //! @param[out] theClosed filled with the fresh derived value
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT bool GetWireIsClosed(BRepGraph_WireId theWire, bool& theClosed);

  //! @brief Return shell status, computing and storing a fresh entry.
  //! @param[in]  theShell shell definition identifier
  //! @param[out] theEntry filled with a fresh derived entry
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT bool GetShellStatus(BRepGraph_ShellId theShell,
                                                    ShellEntry&       theEntry);

  //! @brief Store a pre-computed edge entry (warm-start from import or after computation).
  //! @param[in] theEdge  edge definition identifier
  //! @param[in] theEntry pre-computed entry to store
  Standard_EXPORT void SetEdgeStatus(BRepGraph_EdgeId theEdge, const EdgeEntry& theEntry);

  //! @brief Store a pre-computed wire closure value.
  //! @param[in] theWire   wire definition identifier
  //! @param[in] theClosed pre-computed closure value
  Standard_EXPORT void SetWireIsClosed(BRepGraph_WireId theWire, bool theClosed);

  //! @brief Store a pre-computed shell entry (warm-start from import or after computation).
  //! @param[in] theShell shell definition identifier
  //! @param[in] theEntry pre-computed entry to store
  Standard_EXPORT void SetShellStatus(BRepGraph_ShellId theShell, const ShellEntry& theEntry);

  //! Compute edge derived state directly from a BRepGraph without caching.
  //! Used by callers like Reconstruct that need one-shot computation.
  //! @param[in]  theGraph source graph
  //! @param[in]  theEdge  edge definition identifier
  //! @param[out] theEntry filled with computed entry
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT static bool ComputeEdgeStatus(const BRepGraph& theGraph,
                                                              BRepGraph_EdgeId theEdge,
                                                              EdgeEntry&       theEntry);

  //! Compute wire closure directly from a BRepGraph without caching.
  //! @param[in] theGraph source graph
  //! @param[in] theWire  wire definition identifier
  //! @return true if the wire is closed
  [[nodiscard]] Standard_EXPORT static bool ComputeWireIsClosed(const BRepGraph& theGraph,
                                                                BRepGraph_WireId theWire);

  //! Compute shell closure status directly from a BRepGraph without caching.
  //! @param[in]  theGraph source graph
  //! @param[in]  theShell shell definition identifier
  //! @param[out] theEntry filled with computed entry
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT static bool ComputeShellStatus(const BRepGraph&  theGraph,
                                                               BRepGraph_ShellId theShell,
                                                               ShellEntry&       theEntry);

  DEFINE_STANDARD_RTTIEXT(BRepGraph_CacheDerivedState, BRepGraph_Cache)

private:
  mutable std::shared_mutex myMutex;

  NCollection_DataMap<BRepGraph_EdgeId, EdgeEntry>   myEdgeEntries;
  NCollection_DataMap<BRepGraph_WireId, WireEntry>   myWireEntries;
  NCollection_DataMap<BRepGraph_ShellId, ShellEntry> myShellEntries;
};

#endif // _BRepGraph_CacheDerivedState_HeaderFile
