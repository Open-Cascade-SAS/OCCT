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
#include <NCollection_DynamicArray.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

#include <atomic>
#include <mutex>
#include <shared_mutex>

class BRepGraphInc_Storage;

//! @brief Cache for derived edge, wire, and shell properties.
//!
//! Each query is independent and caches only its own result.
//! Callers request specific values (IsDegenerated, SameParameter, etc.)
//! and the cache computes + stores only what is needed.
class BRepGraph_CacheDerivedState : public BRepGraph_Cache
{
public:
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

  //! @brief Test if an edge is degenerate (no 3D curve and vertex collapse).
  //! Computes and caches only Status - does NOT compute SameParameter/SameRange.
  //! @param[in] theEdge edge definition identifier
  //! @return true if the edge is degenerate
  [[nodiscard]] Standard_EXPORT bool IsDegenerated(const BRepGraph_EdgeId theEdge);

  //! @brief Test if a single coedge has SameParameter.
  //! @param[in] theCoEdge coedge definition identifier
  //! @return true if the coedge has SameParameter
  [[nodiscard]] Standard_EXPORT bool SameParameter(const BRepGraph_CoEdgeId theCoEdge);

  //! @brief Test if a single coedge has SameRange.
  //! @param[in] theCoEdge coedge definition identifier
  //! @return true if the coedge has SameRange
  [[nodiscard]] Standard_EXPORT bool SameRange(const BRepGraph_CoEdgeId theCoEdge);

  //! @brief Test if an edge is closed (start vertex == end vertex).
  //! Computes and caches only IsClosed.
  //! @param[in] theEdge edge definition identifier
  //! @return true if the edge is closed
  [[nodiscard]] Standard_EXPORT bool IsClosed(const BRepGraph_EdgeId theEdge);

  //! @brief Return wire closure, computing and storing a fresh entry.
  //! @param[in]  theWire   wire definition identifier
  //! @param[out] theClosed filled with the fresh derived value
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT bool GetWireIsClosed(const BRepGraph_WireId theWire,
                                                     bool&                  theClosed);

  //! @brief Store a pre-computed wire closure value.
  //! @param[in] theWire   wire definition identifier
  //! @param[in] theClosed pre-computed closure value
  Standard_EXPORT void SetWireIsClosed(const BRepGraph_WireId theWire, const bool theClosed);

  //! @brief Return number of distinct edge definitions used by a wire.
  //! Seam wires can contain two coedges referencing the same edge; this query counts
  //! such seam halves once.
  //! @param[in] theWire wire definition identifier
  //! @return number of distinct active edge definitions used by active coedges in the wire
  [[nodiscard]] Standard_EXPORT uint32_t NbDistinctEdges(const BRepGraph_WireId theWire);

  //! @brief Return the outer wire reference of a face.
  //! Computes the wire whose UV bounds contain all other face wire UV boxes.
  //! @param[in] theFace face definition identifier
  //! @return outer wire reference id, or invalid id when no valid wire bounds exist
  [[nodiscard]] Standard_EXPORT BRepGraph_WireRefId OuterWireRef(const BRepGraph_FaceId theFace);

  //! @brief Return the outer wire of a face.
  //! @param[in] theFace face definition identifier
  //! @return child wire id of OuterWireRef(), or invalid id when no valid wire bounds exist
  [[nodiscard]] Standard_EXPORT BRepGraph_WireId OuterWire(const BRepGraph_FaceId theFace);

  //! @brief Test if a shell is closed.
  //! @param[in] theShell shell definition identifier
  //! @return true if the shell is closed
  [[nodiscard]] Standard_EXPORT bool IsShellClosed(const BRepGraph_ShellId theShell);

  //! Compute edge-own derived state (Status, IsClosed).
  //! SameRange/SameParameter are per-CoEdge - use the per-CoEdge cache directly.
  //! @param[in]  theGraph source graph
  //! @param[in]  theEdge  edge definition identifier
  //! @param[out] theIsDegenerated true if edge is degenerate
  //! @param[out] theIsClosed      true if edge is closed
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT static bool ComputeEdgeProperties(const BRepGraph&       theGraph,
                                                                  const BRepGraph_EdgeId theEdge,
                                                                  bool& theIsDegenerated,
                                                                  bool& theIsClosed);

  //! Compute shell closure directly from a BRepGraph without caching.
  //! @param[in] theGraph source graph
  //! @param[in] theShell shell definition identifier
  //! @return true if the shell is closed
  [[nodiscard]] Standard_EXPORT static bool ComputeShellIsClosed(const BRepGraph&        theGraph,
                                                                 const BRepGraph_ShellId theShell);

  //! Compute wire closure directly from a BRepGraph without caching.
  //! @param[in] theGraph source graph
  //! @param[in] theWire  wire definition identifier
  //! @return true if the wire is closed
  [[nodiscard]] Standard_EXPORT static bool ComputeWireIsClosed(const BRepGraph&       theGraph,
                                                                const BRepGraph_WireId theWire);

  DEFINE_STANDARD_RTTIEXT(BRepGraph_CacheDerivedState, BRepGraph_Cache)

private:
  //! Edge-own derived state: Status (HasCurve3D/Degenerate/Missing), IsClosed.
  //! Depends only on Edge OwnGen (vertices, 3D curve).
  //! Packed into a single atomic byte for lock-free reads.
  struct EdgeEntry : public NodeEntry
  {
    enum class GeomStatus : uint8_t
    {
      HasCurve3D,
      DegenerateOnSurface,
      MissingCurve3D,
      Invalid
    };

    enum Flags : uint8_t
    {
      FlagNone     = 0,
      StatusMask   = 0x07,
      FlagClosed   = 1 << 3,
      FlagComputed = 1 << 4,
    };

    std::atomic<uint8_t> Packed{FlagNone};

    EdgeEntry() = default;

    EdgeEntry(const EdgeEntry& theOther)
        : NodeEntry(theOther),
          Packed(theOther.Packed.load(std::memory_order_relaxed))
    {
    }

    EdgeEntry& operator=(const EdgeEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Packed.store(theOther.Packed.load(std::memory_order_relaxed), std::memory_order_relaxed);
      return *this;
    }

    [[nodiscard]] GeomStatus GetStatus() const
    {
      return static_cast<GeomStatus>(Packed.load(std::memory_order_acquire) & StatusMask);
    }

    [[nodiscard]] bool IsClosed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagClosed) != 0;
    }

    [[nodiscard]] bool IsComputed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagComputed) != 0;
    }

    void Set(const GeomStatus theStatus, const bool theClosed)
    {
      uint8_t aFlags = FlagComputed | static_cast<uint8_t>(theStatus);
      if (theClosed)
      {
        aFlags |= FlagClosed;
      }
      Packed.store(aFlags, std::memory_order_release);
    }
  };

  //! Per-CoEdge entry for SameRange/SameParameter.
  //! Bound to CoEdge OwnGen - invalidates automatically when PCurve changes.
  //! Packed into a single atomic byte for lock-free reads.
  struct CoEdgeSameRangeEntry : public NodeEntry
  {
    enum Flags : uint8_t
    {
      FlagNone          = 0,
      ComputedSameRange = 1 << 0,
      ComputedSameParam = 1 << 1,
      FlagSameRange     = 1 << 2,
      FlagSameParameter = 1 << 3,
    };

    std::atomic<uint8_t> Packed{FlagNone};

    CoEdgeSameRangeEntry() = default;

    CoEdgeSameRangeEntry(const CoEdgeSameRangeEntry& theOther)
        : NodeEntry(theOther),
          Packed(theOther.Packed.load(std::memory_order_relaxed))
    {
    }

    CoEdgeSameRangeEntry& operator=(const CoEdgeSameRangeEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Packed.store(theOther.Packed.load(std::memory_order_relaxed), std::memory_order_relaxed);
      return *this;
    }

    [[nodiscard]] bool SameRange() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagSameRange) != 0;
    }

    [[nodiscard]] bool SameParameter() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagSameParameter) != 0;
    }

    [[nodiscard]] uint8_t Computed() const { return Packed.load(std::memory_order_acquire); }

    void SetSameRange(const bool theVal)
    {
      uint8_t aFlags = ComputedSameRange;
      if (theVal)
      {
        aFlags |= FlagSameRange;
      }
      Packed.fetch_or(aFlags, std::memory_order_release);
    }

    void SetSameParameter(const bool theVal)
    {
      uint8_t aFlags = ComputedSameParam;
      if (theVal)
      {
        aFlags |= FlagSameParameter;
      }
      Packed.fetch_or(aFlags, std::memory_order_release);
    }
  };

  struct WireEntry : public NodeEntry
  {
    enum Flags : uint8_t
    {
      FlagNone             = 0,
      FlagClosed           = 1 << 0,
      FlagClosureComputed  = 1 << 1,
      FlagDistinctComputed = 1 << 2,
    };

    std::atomic<uint8_t> Packed{FlagNone};
    std::atomic<uint32_t> DistinctEdges{0};

    WireEntry() = default;

    WireEntry(const WireEntry& theOther)
        : NodeEntry(theOther),
          Packed(theOther.Packed.load(std::memory_order_relaxed)),
          DistinctEdges(theOther.DistinctEdges.load(std::memory_order_relaxed))
    {
    }

    WireEntry& operator=(const WireEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Packed.store(theOther.Packed.load(std::memory_order_relaxed), std::memory_order_relaxed);
      DistinctEdges.store(theOther.DistinctEdges.load(std::memory_order_relaxed),
                          std::memory_order_relaxed);
      return *this;
    }

    [[nodiscard]] bool IsClosed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagClosed) != 0;
    }

    [[nodiscard]] bool IsComputed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagClosureComputed) != 0;
    }

    [[nodiscard]] bool HasDistinctEdges() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagDistinctComputed) != 0;
    }

    [[nodiscard]] uint32_t NbDistinctEdges() const
    {
      return DistinctEdges.load(std::memory_order_acquire);
    }

    void SetClosed(const bool theVal)
    {
      uint8_t aFlags = Packed.load(std::memory_order_acquire)
                     & ~(FlagClosureComputed | FlagClosed);
      aFlags |= FlagClosureComputed;
      if (theVal)
      {
        aFlags |= FlagClosed;
      }
      Packed.store(aFlags, std::memory_order_release);
    }

    void SetDistinctEdges(const uint32_t theCount)
    {
      DistinctEdges.store(theCount, std::memory_order_release);
      Packed.fetch_or(FlagDistinctComputed, std::memory_order_release);
    }
  };

  struct ShellEntry : public NodeEntry
  {
    enum class ClosureStatus : uint8_t
    {
      Empty,
      Open,
      Closed,
      NonManifold,
      Invalid
    };

    std::atomic<ClosureStatus> Status{ClosureStatus::Invalid};

    ShellEntry() = default;

    ShellEntry(const ShellEntry& theOther)
        : NodeEntry(theOther),
          Status(theOther.Status.load(std::memory_order_relaxed))
    {
    }

    ShellEntry& operator=(const ShellEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Status.store(theOther.Status.load(std::memory_order_relaxed), std::memory_order_relaxed);
      return *this;
    }
  };

  struct FaceEntry : public NodeEntry
  {
    enum Flags : uint8_t
    {
      FlagNone              = 0,
      FlagOuterWireComputed = 1 << 0,
    };

    std::atomic<uint8_t> Packed{FlagNone};
    BRepGraph_WireRefId  OuterWireRefId;

    FaceEntry() = default;

    FaceEntry(const FaceEntry& theOther)
        : NodeEntry(theOther),
          Packed(theOther.Packed.load(std::memory_order_relaxed)),
           OuterWireRefId(theOther.OuterWireRefId)
    {
    }

    FaceEntry& operator=(const FaceEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Packed.store(theOther.Packed.load(std::memory_order_relaxed), std::memory_order_relaxed);
      OuterWireRefId = theOther.OuterWireRefId;
      return *this;
    }

    [[nodiscard]] bool HasOuterWire() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagOuterWireComputed) != 0;
    }

    [[nodiscard]] BRepGraph_WireRefId OuterWireRef() const { return OuterWireRefId; }

    void SetOuterWireRef(const BRepGraph_WireRefId theWireRef)
    {
      OuterWireRefId = theWireRef;
      Packed.store(FlagOuterWireComputed, std::memory_order_release);
    }
  };

  //! Ensure edge-own entry (Status, IsClosed) is fresh. Uses OwnGen only.
  bool ensureEdgeEntry(const BRepGraph_EdgeId theEdge, EdgeEntry& theEntry);

  //! Ensure per-CoEdge entry is fresh. Bound to CoEdge OwnGen.
  bool ensureCoEdgeSameRangeEntry(const BRepGraph_CoEdgeId theCoEdge,
                                  const uint8_t            theRequiredFlags,
                                  CoEdgeSameRangeEntry&    theEntry);

  static void computeStatusOnly(const BRepGraph&       theGraph,
                                const BRepGraph_EdgeId theEdge,
                                EdgeEntry&             theEntry);

  static void computeSameRange(const BRepGraph&         theGraph,
                               const BRepGraph_CoEdgeId theCoEdge,
                               CoEdgeSameRangeEntry&    theEntry);

  static void computeSameParameter(const BRepGraph&         theGraph,
                                   const BRepGraph_CoEdgeId theCoEdge,
                                   CoEdgeSameRangeEntry&    theEntry);

  static ShellEntry::ClosureStatus computeShellClosure(const BRepGraph&        theGraph,
                                                       const BRepGraph_ShellId theShell);

  static uint32_t computeWireNbDistinctEdges(const BRepGraph&       theGraph,
                                             const BRepGraph_WireId theWire);

  static BRepGraph_WireRefId computeFaceOuterWireRef(const BRepGraph&       theGraph,
                                                     const BRepGraph_FaceId theFace);

  mutable std::shared_mutex myMutex;

  NCollection_DynamicArray<EdgeEntry>            myEdgeEntries;
  NCollection_DynamicArray<CoEdgeSameRangeEntry> myCoEdgeSameRangeEntries;
  NCollection_DynamicArray<WireEntry>            myWireEntries;
  NCollection_DynamicArray<ShellEntry>           myShellEntries;
  NCollection_DynamicArray<FaceEntry>            myFaceEntries;
};

#endif // _BRepGraph_CacheDerivedState_HeaderFile
