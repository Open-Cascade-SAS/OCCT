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
  [[nodiscard]] Standard_EXPORT bool IsDegenerated(BRepGraph_EdgeId theEdge);

  //! @brief Test if a single coedge has SameParameter.
  //! @param[in] theCoEdge coedge definition identifier
  //! @return true if the coedge has SameParameter
  [[nodiscard]] Standard_EXPORT bool SameParameter(BRepGraph_CoEdgeId theCoEdge);

  //! @brief Test if a single coedge has SameRange.
  //! @param[in] theCoEdge coedge definition identifier
  //! @return true if the coedge has SameRange
  [[nodiscard]] Standard_EXPORT bool SameRange(BRepGraph_CoEdgeId theCoEdge);

  //! @brief Test if an edge is closed (start vertex == end vertex).
  //! Computes and caches only IsClosed.
  //! @param[in] theEdge edge definition identifier
  //! @return true if the edge is closed
  [[nodiscard]] Standard_EXPORT bool IsClosed(BRepGraph_EdgeId theEdge);

  //! @brief Return wire closure, computing and storing a fresh entry.
  //! @param[in]  theWire   wire definition identifier
  //! @param[out] theClosed filled with the fresh derived value
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT bool GetWireIsClosed(BRepGraph_WireId theWire, bool& theClosed);

  //! @brief Store a pre-computed wire closure value.
  //! @param[in] theWire   wire definition identifier
  //! @param[in] theClosed pre-computed closure value
  Standard_EXPORT void SetWireIsClosed(BRepGraph_WireId theWire, bool theClosed);

  //! @brief Test if a shell is closed.
  //! @param[in] theShell shell definition identifier
  //! @return true if the shell is closed
  [[nodiscard]] Standard_EXPORT bool IsShellClosed(BRepGraph_ShellId theShell);

  //! Compute edge-own derived state (Status, IsClosed).
  //! SameRange/SameParameter are per-CoEdge - use the per-CoEdge cache directly.
  //! @param[in]  theGraph source graph
  //! @param[in]  theEdge  edge definition identifier
  //! @param[out] theIsDegenerated true if edge is degenerate
  //! @param[out] theIsClosed      true if edge is closed
  //! @return true if computation succeeded
  [[nodiscard]] Standard_EXPORT static bool ComputeEdgeProperties(const BRepGraph& theGraph,
                                                                  BRepGraph_EdgeId theEdge,
                                                                  bool&            theIsDegenerated,
                                                                  bool&            theIsClosed);

  //! Compute shell closure directly from a BRepGraph without caching.
  //! @param[in] theGraph source graph
  //! @param[in] theShell shell definition identifier
  //! @return true if the shell is closed
  [[nodiscard]] Standard_EXPORT static bool ComputeShellIsClosed(const BRepGraph&  theGraph,
                                                                 BRepGraph_ShellId theShell);

  //! Compute wire closure directly from a BRepGraph without caching.
  //! @param[in] theGraph source graph
  //! @param[in] theWire  wire definition identifier
  //! @return true if the wire is closed
  [[nodiscard]] Standard_EXPORT static bool ComputeWireIsClosed(const BRepGraph& theGraph,
                                                                BRepGraph_WireId theWire);

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

    void Set(GeomStatus theStatus, bool theClosed)
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

    void SetSameRange(bool theVal)
    {
      uint8_t aFlags = ComputedSameRange;
      if (theVal)
      {
        aFlags |= FlagSameRange;
      }
      Packed.fetch_or(aFlags, std::memory_order_release);
    }

    void SetSameParameter(bool theVal)
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
      FlagNone     = 0,
      FlagClosed   = 1 << 0,
      FlagComputed = 1 << 1,
    };

    std::atomic<uint8_t> Packed{FlagNone};

    WireEntry() = default;

    WireEntry(const WireEntry& theOther)
        : NodeEntry(theOther),
          Packed(theOther.Packed.load(std::memory_order_relaxed))
    {
    }

    WireEntry& operator=(const WireEntry& theOther)
    {
      NodeEntry::operator=(theOther);
      Packed.store(theOther.Packed.load(std::memory_order_relaxed), std::memory_order_relaxed);
      return *this;
    }

    [[nodiscard]] bool IsClosed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagClosed) != 0;
    }

    [[nodiscard]] bool IsComputed() const
    {
      return (Packed.load(std::memory_order_acquire) & FlagComputed) != 0;
    }

    void SetClosed(bool theVal)
    {
      uint8_t aFlags = FlagComputed;
      if (theVal)
      {
        aFlags |= FlagClosed;
      }
      Packed.store(aFlags, std::memory_order_release);
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

  //! Ensure edge-own entry (Status, IsClosed) is fresh. Uses OwnGen only.
  bool ensureEdgeEntry(BRepGraph_EdgeId theEdge, EdgeEntry& theEntry);

  //! Ensure per-CoEdge entry is fresh. Bound to CoEdge OwnGen.
  bool ensureCoEdgeSameRangeEntry(BRepGraph_CoEdgeId    theCoEdge,
                                  uint8_t               theRequiredFlags,
                                  CoEdgeSameRangeEntry& theEntry);

  static void computeStatusOnly(const BRepGraph& theGraph,
                                BRepGraph_EdgeId theEdge,
                                EdgeEntry&       theEntry);

  static void computeSameRange(const BRepGraph&      theGraph,
                               BRepGraph_CoEdgeId    theCoEdge,
                               CoEdgeSameRangeEntry& theEntry);

  static void computeSameParameter(const BRepGraph&      theGraph,
                                   BRepGraph_CoEdgeId    theCoEdge,
                                   CoEdgeSameRangeEntry& theEntry);

  static ShellEntry::ClosureStatus computeShellClosure(const BRepGraph&  theGraph,
                                                       BRepGraph_ShellId theShell);

  mutable std::mutex myMutex;

  NCollection_DynamicArray<EdgeEntry>            myEdgeEntries;
  NCollection_DynamicArray<CoEdgeSameRangeEntry> myCoEdgeSameRangeEntries;
  NCollection_DynamicArray<WireEntry>            myWireEntries;
  NCollection_DynamicArray<ShellEntry>           myShellEntries;
};

#endif // _BRepGraph_CacheDerivedState_HeaderFile
