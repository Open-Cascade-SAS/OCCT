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

#ifndef _BRepGraph_LayerRegularity_HeaderFile
#define _BRepGraph_LayerRegularity_HeaderFile

#include <BRepGraph_Layer.hxx>

#include <GeomAbs_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>

//! @brief Persistent edge-continuity store, keyed by (edge, F1, F2).
//!
//! Each entry holds the geometric continuity (C^k / G^k) across the face pair
//! at a given edge. F1 == F2 represents seam continuity across a closed
//! surface's seam line; F1 != F2 represents inter-face regularity. The schema
//! mirrors classical BRep_Tool::Continuity(edge, F1, F2).
//!
//! ## Lifetime policy
//! The layer is **persistent metadata**: stored values survive arbitrary
//! mutations to the referenced edges and faces. Only the following events
//! discard data:
//!   - OnNodeRemoved(edge|face) - the referenced node is gone; entries naming
//!     it are dropped (or migrated when a replacement is provided).
//!   - OnCompact - ids are remapped; entries pointing to removed nodes drop.
//!   - InvalidateAll() / Clear() - explicit caller request.
//! In particular, this layer does NOT subscribe to OnNodeModified: a tolerance
//! bump or NaturalRestriction toggle leaves stored continuity intact. Callers
//! that change the underlying geometry are responsible for refreshing affected
//! entries (typically via SetRegularity, removeRegularity, or InvalidateAll).
class BRepGraph_LayerRegularity : public BRepGraph_Layer
{
public:
  //! Return fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  struct RegularityEntry
  {
    BRepGraph_FaceId FaceEntity1;
    BRepGraph_FaceId FaceEntity2;
    GeomAbs_Shape    Continuity = GeomAbs_C0;
  };

  struct EdgeRegularities
  {
    NCollection_DynamicArray<RegularityEntry> Entries;

    [[nodiscard]] bool IsEmpty() const { return Entries.IsEmpty(); }
  };

  Standard_EXPORT const EdgeRegularities* FindEdgeRegularities(
    const BRepGraph_EdgeId theEdge) const;

  Standard_EXPORT bool FindContinuity(const BRepGraph_EdgeId theEdge,
                                      const BRepGraph_FaceId theFace1,
                                      const BRepGraph_FaceId theFace2,
                                      GeomAbs_Shape* const   theContinuity = nullptr) const;

  Standard_EXPORT uint32_t      NbRegularities(const BRepGraph_EdgeId theEdge) const;
  Standard_EXPORT GeomAbs_Shape MaxContinuity(const BRepGraph_EdgeId theEdge) const;

  [[nodiscard]] bool HasBindings() const { return myEdgeRegularities.Extent() != 0; }

  Standard_EXPORT void SetRegularity(const BRepGraph_EdgeId theEdge,
                                     const BRepGraph_FaceId theFace1,
                                     const BRepGraph_FaceId theFace2,
                                     const GeomAbs_Shape    theContinuity);

  //! Copy all regularity entries from one edge to another.
  Standard_EXPORT void CopyRegularities(const BRepGraph_EdgeId theSourceEdge,
                                        const BRepGraph_EdgeId theTargetEdge);

  //! Remove all regularity entries bound to the edge.
  Standard_EXPORT void RemoveRegularities(const BRepGraph_EdgeId theEdge) noexcept;

  Standard_EXPORT const TCollection_AsciiString& Name() const override;
  Standard_EXPORT void                           OnNodeRemoved(const BRepGraph_NodeId theNode,
                                                               const BRepGraph_NodeId theReplacement) noexcept override;
  Standard_EXPORT void                           OnCompact(
                              const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerRegularity, BRepGraph_Layer)

private:
  void normalizeFacePair(BRepGraph_FaceId& theFace1, BRepGraph_FaceId& theFace2) const noexcept;
  EdgeRegularities& changeEdgeRegularities(const BRepGraph_EdgeId theEdge);
  void              bindFaceToEdge(const BRepGraph_FaceId theFace, const BRepGraph_EdgeId theEdge);
  void unbindFaceFromEdge(const BRepGraph_FaceId theFace, const BRepGraph_EdgeId theEdge) noexcept;
  void removeRegularity(const BRepGraph_EdgeId theEdge,
                        const BRepGraph_FaceId theFace1,
                        const BRepGraph_FaceId theFace2) noexcept;
  void removeEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept;
  void invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept;
  void migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                           const BRepGraph_EdgeId theNewEdge) noexcept;
  void migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                           const BRepGraph_FaceId theNewFace) noexcept;

private:
  NCollection_DataMap<BRepGraph_EdgeId, EdgeRegularities> myEdgeRegularities;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_DynamicArray<BRepGraph_EdgeId>> myFaceToEdges;
};

#endif // _BRepGraph_LayerRegularity_HeaderFile
