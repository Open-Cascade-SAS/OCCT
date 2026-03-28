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

#ifndef _BRepGraph_RegularityLayer_HeaderFile
#define _BRepGraph_RegularityLayer_HeaderFile

#include <BRepGraph_Layer.hxx>

#include <GeomAbs_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>

//! @brief Stores edge continuity records between adjacent face pairs.
class BRepGraph_RegularityLayer : public BRepGraph_Layer
{
public:
  struct RegularityEntry
  {
    BRepGraph_FaceId FaceEntity1;
    BRepGraph_FaceId FaceEntity2;
    GeomAbs_Shape    Continuity = GeomAbs_C0;
  };

  struct EdgeRegularities
  {
    NCollection_Vector<RegularityEntry> Entries;

    [[nodiscard]] bool IsEmpty() const { return Entries.IsEmpty(); }
  };

  Standard_EXPORT const EdgeRegularities* FindEdgeRegularities(const BRepGraph_EdgeId theEdge) const;

  Standard_EXPORT bool FindContinuity(const BRepGraph_EdgeId theEdge,
                                      const BRepGraph_FaceId theFace1,
                                      const BRepGraph_FaceId theFace2,
                                      GeomAbs_Shape* const   theContinuity = nullptr) const;

  Standard_EXPORT int NbRegularities(const BRepGraph_EdgeId theEdge) const;
  Standard_EXPORT GeomAbs_Shape MaxContinuity(const BRepGraph_EdgeId theEdge) const;

  [[nodiscard]] bool HasBindings() const { return myEdgeRegularities.Extent() != 0; }

  Standard_EXPORT void SetRegularity(const BRepGraph_EdgeId theEdge,
                                     const BRepGraph_FaceId theFace1,
                                     const BRepGraph_FaceId theFace2,
                                     const GeomAbs_Shape    theContinuity);

  Standard_EXPORT const TCollection_AsciiString& Name() const override;
  [[nodiscard]] Standard_EXPORT int SubscribedKinds() const override;
  Standard_EXPORT void OnNodeModified(const BRepGraph_NodeId theNode) noexcept override;
  Standard_EXPORT void OnNodesModified(
    const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes) noexcept override;
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode,
                                     const BRepGraph_NodeId theReplacement) noexcept override;
  Standard_EXPORT void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_RegularityLayer, BRepGraph_Layer)

private:
  Standard_EXPORT void normalizeFacePair(BRepGraph_FaceId& theFace1,
                                         BRepGraph_FaceId& theFace2) const noexcept;
  Standard_EXPORT EdgeRegularities& changeEdgeRegularities(const BRepGraph_EdgeId theEdge);
  Standard_EXPORT void bindFaceToEdge(const BRepGraph_FaceId theFace, const BRepGraph_EdgeId theEdge);
  Standard_EXPORT void unbindFaceFromEdge(const BRepGraph_FaceId theFace,
                                          const BRepGraph_EdgeId theEdge) noexcept;
  Standard_EXPORT void removeRegularity(const BRepGraph_EdgeId theEdge,
                                        const BRepGraph_FaceId theFace1,
                                        const BRepGraph_FaceId theFace2) noexcept;
  Standard_EXPORT void removeEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept;
  Standard_EXPORT void invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept;
  Standard_EXPORT void migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                                           const BRepGraph_EdgeId theNewEdge) noexcept;
  Standard_EXPORT void migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                                           const BRepGraph_FaceId theNewFace) noexcept;

private:
  NCollection_DataMap<BRepGraph_EdgeId, EdgeRegularities>                 myEdgeRegularities;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_EdgeId>> myFaceToEdges;
};

#endif // _BRepGraph_RegularityLayer_HeaderFile
