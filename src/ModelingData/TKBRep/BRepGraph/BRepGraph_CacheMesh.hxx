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

#ifndef _BRepGraph_CacheMesh_HeaderFile
#define _BRepGraph_CacheMesh_HeaderFile

#include <BRepGraph_Cache.hxx>
#include <BRepGraph_NodeId.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

#include <cstdint>

class BRepGraph;

//! @brief Registry-owned runtime mesh cache for BRepGraph.
//!
//! CacheMesh stores transient triangulations and polygons produced by meshing drivers.
//! It is not copied, transformed, or serialized. Persistent mesh representations remain
//! in topology definitions and are filled only by import, authored primitive creation,
//! or explicit promotion.
class BRepGraph_CacheMesh : public BRepGraph_Cache
{
public:
  using SlotId = uint32_t;

  static constexpr SlotId DefaultDisplaySlot = 0;

  //! Entry stamp against the slot recipe and cache-local generation.
  struct EntryStamp
  {
    uint64_t RecipeHash     = 0;
    uint32_t SlotGeneration = 0;

    void Reset() noexcept
    {
      RecipeHash     = 0;
      SlotGeneration = 0;
    }
  };

  //! Cached mesh entry for a face: single triangulation.
  struct FaceMeshEntry : public NodeEntry
  {
    occ::handle<Poly_Triangulation> Triangulation;
    EntryStamp                      Stamp;
    uint32_t                        MeshGeneration = 0;

    [[nodiscard]] bool IsPresent() const { return !Triangulation.IsNull(); }

    //! Clear triangulation representation. Does NOT bump MeshGeneration -
    //! callers must call BRepGraph_CacheMesh::BumpFaceMeshGeneration() separately.
    void ClearRepresentation() noexcept { Triangulation.Nullify(); }

    void Reset() noexcept
    {
      NodeEntry::Reset();
      Triangulation.Nullify();
      Stamp.Reset();
      MeshGeneration = 0;
    }
  };

  //! Cached mesh entry for a coedge: polygon-in-parametric-space and polygon-on-triangulation.
  //!
  //! CoEdgeMeshEntry composes two NodeEntry fields instead of inheriting from one.
  //! This breaks the inheritance pattern used by other entries because the coedge
  //! has two independent freshness dimensions: coedge topology and face mesh content.
  struct CoEdgeMeshEntry
  {
    //! Coedge topology freshness (for Polygon2D).
    NodeEntry CoEdgeStamp;

    //! Face topology freshness (for PolygonsOnTri).
    NodeEntry FaceTopologyStamp;

    //! Face mesh content freshness (for PolygonsOnTri).
    uint32_t FaceMeshGeneration = 0;
    //! Face whose MeshGeneration we track.
    BRepGraph_FaceId BoundFaceId;

    //! Slot recipe freshness.
    EntryStamp SlotStamp;

    //! Cached polygon-on-surface.
    occ::handle<Poly_Polygon2D> Polygon2D;
    //! Cached polygons-on-triangulation.
    NCollection_LinearVector<occ::handle<Poly_PolygonOnTriangulation>> PolygonsOnTri;

    [[nodiscard]] bool IsPresent() const { return !Polygon2D.IsNull() || !PolygonsOnTri.IsEmpty(); }

    void Reset() noexcept
    {
      CoEdgeStamp.Reset();
      FaceTopologyStamp.Reset();
      FaceMeshGeneration = 0;
      BoundFaceId        = BRepGraph_FaceId();
      SlotStamp.Reset();
      Polygon2D.Nullify();
      PolygonsOnTri.Clear();
    }
  };

  //! Cached mesh entry for an edge: polygon-3D.
  struct EdgeMeshEntry : public NodeEntry
  {
    occ::handle<Poly_Polygon3D> Polygon3D;
    EntryStamp                  Stamp;

    [[nodiscard]] bool IsPresent() const { return !Polygon3D.IsNull(); }

    void Reset() noexcept
    {
      NodeEntry::Reset();
      Polygon3D.Nullify();
      Stamp.Reset();
    }
  };

  //! Dirty topology set requested from a cache driver.
  struct DirtySet
  {
    NCollection_LinearVector<BRepGraph_FaceId> Faces;
    NCollection_LinearVector<BRepGraph_EdgeId> FreeEdges;

    [[nodiscard]] bool IsEmpty() const { return Faces.IsEmpty() && FreeEdges.IsEmpty(); }

    void Clear()
    {
      Faces.Clear();
      FreeEdges.Clear();
    }
  };

  //! Runtime state of a cache slot.
  struct SlotState
  {
    SlotId   Slot       = DefaultDisplaySlot;
    uint64_t RecipeHash = 0;
    uint32_t Generation = 1;
    bool     HasDriver  = false;
  };

  //! Mesh recomputation driver registered by a meshing toolkit.
  class Driver : public Standard_Transient
  {
  public:
    [[nodiscard]] virtual const Standard_GUID& ID() const         = 0;
    [[nodiscard]] virtual uint64_t             RecipeHash() const = 0;

    [[nodiscard]] virtual bool Fill(BRepGraph&                   theGraph,
                                    SlotId                       theSlot,
                                    const DirtySet&              theDirtySet,
                                    const Message_ProgressRange& theRange) = 0;

    DEFINE_STANDARD_RTTIEXT(Driver, Standard_Transient)
  };

  Standard_EXPORT BRepGraph_CacheMesh();

  BRepGraph_CacheMesh(const BRepGraph_CacheMesh&)            = delete;
  BRepGraph_CacheMesh& operator=(const BRepGraph_CacheMesh&) = delete;

  //! Returns the unique cache service GUID.
  [[nodiscard]] static Standard_EXPORT const Standard_GUID& GetID();

  //! Returns the unique cache service GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Returns the cache service display name.
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Clears all cache slots and registered drivers.
  Standard_EXPORT void Clear() noexcept override;

  //! Copy fresh, remappable mesh cache entries into the target graph.
  Standard_EXPORT void CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! Get/set the currently active display slot.
  //! EffectiveView reads from this slot.
  [[nodiscard]] SlotId ActiveDisplaySlot() const { return myActiveSlot; }

  void SetActiveDisplaySlot(SlotId theSlot) { myActiveSlot = theSlot; }

  //! Register or replace a meshing driver for a cache slot.
  Standard_EXPORT void RegisterDriver(SlotId theSlot, const occ::handle<Driver>& theDriver);

  //! Remove a meshing driver from a cache slot and invalidate the slot.
  Standard_EXPORT void UnregisterDriver(SlotId theSlot);

  //! Return driver registered for a slot, or null.
  [[nodiscard]] Standard_EXPORT const occ::handle<Driver>& DriverOf(SlotId theSlot) const;

  //! Return current state of a cache slot.
  [[nodiscard]] Standard_EXPORT SlotState State(SlotId theSlot = DefaultDisplaySlot) const;

  //! Recompute stale data in a slot using its registered driver.
  [[nodiscard]] Standard_EXPORT bool Ensure(
    BRepGraph&                   theGraph,
    SlotId                       theSlot  = DefaultDisplaySlot,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Recompute stale data below a topology node using the slot driver.
  [[nodiscard]] Standard_EXPORT bool Ensure(
    BRepGraph&                   theGraph,
    const BRepGraph_NodeId       theRoot,
    SlotId                       theSlot  = DefaultDisplaySlot,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Recompute stale data for requested topology nodes using the slot driver.
  [[nodiscard]] Standard_EXPORT bool Ensure(
    BRepGraph&                                  theGraph,
    const NCollection_Array1<BRepGraph_NodeId>& theNodes,
    SlotId                                      theSlot  = DefaultDisplaySlot,
    const Message_ProgressRange&                theRange = Message_ProgressRange());

  //! Return true when a cache slot has stale or missing mesh below the topology node.
  //! Uses the same actualness rules as Ensure() but does not invoke the driver.
  [[nodiscard]] Standard_EXPORT bool Needs(BRepGraph&       theGraph,
                                           BRepGraph_NodeId theRoot,
                                           SlotId           theSlot = DefaultDisplaySlot) const;

  [[nodiscard]] Standard_EXPORT bool                 HasFaceMesh(BRepGraph_FaceId theFace) const;
  [[nodiscard]] Standard_EXPORT const FaceMeshEntry* FindFaceMesh(BRepGraph_FaceId theFace) const;
  [[nodiscard]] Standard_EXPORT FaceMeshEntry&       ChangeFaceMesh(BRepGraph_FaceId theFace);
  Standard_EXPORT void                               ClearFaceMesh(BRepGraph_FaceId theFace);

  [[nodiscard]] Standard_EXPORT bool             HasCoEdgeMesh(BRepGraph_CoEdgeId theCoEdge) const;
  [[nodiscard]] Standard_EXPORT CoEdgeMeshEntry& ChangeCoEdgeMesh(BRepGraph_CoEdgeId theCoEdge);
  Standard_EXPORT void                           ClearCoEdgeMesh(BRepGraph_CoEdgeId theCoEdge);

  [[nodiscard]] Standard_EXPORT bool                 HasEdgeMesh(BRepGraph_EdgeId theEdge) const;
  [[nodiscard]] Standard_EXPORT const EdgeMeshEntry* FindEdgeMesh(BRepGraph_EdgeId theEdge) const;
  [[nodiscard]] Standard_EXPORT EdgeMeshEntry&       ChangeEdgeMesh(BRepGraph_EdgeId theEdge);
  Standard_EXPORT void                               ClearEdgeMesh(BRepGraph_EdgeId theEdge);

  [[nodiscard]] Standard_EXPORT bool HasFaceMesh(SlotId theSlot, BRepGraph_FaceId theFace) const;
  [[nodiscard]] Standard_EXPORT const FaceMeshEntry* FindFaceMesh(SlotId           theSlot,
                                                                  BRepGraph_FaceId theFace) const;
  [[nodiscard]] Standard_EXPORT FaceMeshEntry&       ChangeFaceMesh(SlotId           theSlot,
                                                                    BRepGraph_FaceId theFace);
  Standard_EXPORT void ClearFaceMesh(SlotId theSlot, BRepGraph_FaceId theFace);

  [[nodiscard]] Standard_EXPORT bool             HasCoEdgeMesh(SlotId             theSlot,
                                                               BRepGraph_CoEdgeId theCoEdge) const;
  [[nodiscard]] Standard_EXPORT CoEdgeMeshEntry& ChangeCoEdgeMesh(SlotId             theSlot,
                                                                  BRepGraph_CoEdgeId theCoEdge);
  Standard_EXPORT void ClearCoEdgeMesh(SlotId theSlot, BRepGraph_CoEdgeId theCoEdge);

  [[nodiscard]] Standard_EXPORT bool HasEdgeMesh(SlotId theSlot, BRepGraph_EdgeId theEdge) const;
  [[nodiscard]] Standard_EXPORT const EdgeMeshEntry* FindEdgeMesh(SlotId           theSlot,
                                                                  BRepGraph_EdgeId theEdge) const;
  [[nodiscard]] Standard_EXPORT EdgeMeshEntry&       ChangeEdgeMesh(SlotId           theSlot,
                                                                    BRepGraph_EdgeId theEdge);
  Standard_EXPORT void ClearEdgeMesh(SlotId theSlot, BRepGraph_EdgeId theEdge);

  //! Stamp a freshly written default-slot entry.
  Standard_EXPORT void BindFresh(FaceMeshEntry& theEntry, BRepGraph_FaceId theFace) const;
  Standard_EXPORT void BindFresh(CoEdgeMeshEntry& theEntry, BRepGraph_CoEdgeId theCoEdge) const;
  Standard_EXPORT void BindFresh(EdgeMeshEntry& theEntry, BRepGraph_EdgeId theEdge) const;

  //! Bump face mesh generation after cached content changed.
  //! This is the ONLY mutator for MeshGeneration. ClearRepresentation() does not bump.
  //! Creates the face entry if it doesn't exist yet (via ensureSize).
  Standard_EXPORT void BumpFaceMeshGeneration(BRepGraph_FaceId theFace,
                                              SlotId           theSlot = DefaultDisplaySlot);

  //! Raw face entry access (no freshness filtering). For internal use.
  [[nodiscard]] Standard_EXPORT const FaceMeshEntry* findFaceEntryRaw(
    SlotId           theSlot,
    BRepGraph_FaceId theFace) const;

  //! Raw coedge entry access (no freshness/generation filtering).
  //! Returns nullptr if the slot is absent or the entry has no representation.
  //! For internal use.
  [[nodiscard]] Standard_EXPORT const CoEdgeMeshEntry* findCoEdgeEntryRaw(
    SlotId             theSlot,
    BRepGraph_CoEdgeId theCoEdge) const;

  //! Return coedge entry if Polygon2D is fresh, nullptr otherwise.
  [[nodiscard]] Standard_EXPORT const CoEdgeMeshEntry* FindCoEdgePolygon2D(
    SlotId             theSlot,
    BRepGraph_CoEdgeId theCoEdge) const;

  //! Return coedge entry if PolygonsOnTri is fresh, nullptr otherwise.
  [[nodiscard]] Standard_EXPORT const CoEdgeMeshEntry* FindCoEdgePolygonOnTri(
    SlotId             theSlot,
    BRepGraph_CoEdgeId theCoEdge) const;

  //! Return coedge entry if Polygon2D is fresh (default slot), nullptr otherwise.
  [[nodiscard]] Standard_EXPORT const CoEdgeMeshEntry* FindCoEdgePolygon2D(
    BRepGraph_CoEdgeId theCoEdge) const;

  //! Return coedge entry if PolygonsOnTri is fresh (default slot), nullptr otherwise.
  [[nodiscard]] Standard_EXPORT const CoEdgeMeshEntry* FindCoEdgePolygonOnTri(
    BRepGraph_CoEdgeId theCoEdge) const;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_CacheMesh, BRepGraph_Cache)

private:
  struct Slot;

  template <typename T>
  static void ensureSize(NCollection_DynamicArray<T>& theVec, size_t theIndex);

  [[nodiscard]] Slot&       changeSlot(SlotId theSlot);
  [[nodiscard]] const Slot* findSlot(SlotId theSlot) const;

  [[nodiscard]] bool isSlotActual(const Slot& theSlot, const EntryStamp& theStamp) const noexcept;
  void bindEntry(FaceMeshEntry& theEntry, BRepGraph_FaceId theFace, const Slot& theSlot) const;
  void bindEntry(CoEdgeMeshEntry&   theEntry,
                 BRepGraph_CoEdgeId theCoEdge,
                 const Slot&        theSlot) const;
  void bindEntry(EdgeMeshEntry& theEntry, BRepGraph_EdgeId theEdge, const Slot& theSlot) const;

  [[nodiscard]] bool isCoEdgePolygon2DFresh(const CoEdgeMeshEntry& theEntry,
                                            const Slot&            theSlot) const noexcept;
  [[nodiscard]] bool isCoEdgePolygonOnTriFresh(const CoEdgeMeshEntry& theEntry,
                                               const Slot&            theSlot) const noexcept;
  [[nodiscard]] bool isFaceMeshFresh(const CoEdgeMeshEntry& theEntry,
                                     const Slot&            theSlot) const noexcept;

  [[nodiscard]] const CoEdgeMeshEntry* findCoEdgeMesh(SlotId             theSlot,
                                                      BRepGraph_CoEdgeId theCoEdge) const;

  [[nodiscard]] DirtySet collectDirty(BRepGraph& theGraph, const Slot& theSlot) const;
  [[nodiscard]] DirtySet collectDirty(BRepGraph&             theGraph,
                                      const BRepGraph_NodeId theRoot,
                                      const Slot&            theSlot) const;
  [[nodiscard]] DirtySet collectDirty(BRepGraph&                                  theGraph,
                                      const NCollection_Array1<BRepGraph_NodeId>& theNodes,
                                      const Slot&                                 theSlot) const;

  NCollection_LinearVector<Slot> mySlots;
  SlotId                         myActiveSlot = DefaultDisplaySlot;
};

#endif // _BRepGraph_CacheMesh_HeaderFile
