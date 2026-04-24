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

#ifndef _BRepGraph_LayerParam_HeaderFile
#define _BRepGraph_LayerParam_HeaderFile

#include <BRepGraph_Layer.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <gp_Pnt2d.hxx>

//! @brief Stores vertex-on-curve, vertex-on-surface, and vertex-on-PCurve bindings.
class BRepGraph_LayerParam : public BRepGraph_Layer
{
public:
  //! Return fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  struct PointOnCurveEntry
  {
    double           Parameter = 0.0;
    BRepGraph_EdgeId EdgeDefId;
  };

  struct PointOnSurfaceEntry
  {
    double           ParameterU = 0.0;
    double           ParameterV = 0.0;
    BRepGraph_FaceId FaceDefId;
  };

  struct PointOnPCurveEntry
  {
    double             Parameter = 0.0;
    BRepGraph_CoEdgeId CoEdgeDefId;
  };

  struct VertexParams
  {
    NCollection_DynamicArray<PointOnCurveEntry>   PointsOnCurve;
    NCollection_DynamicArray<PointOnSurfaceEntry> PointsOnSurface;
    NCollection_DynamicArray<PointOnPCurveEntry>  PointsOnPCurve;

    [[nodiscard]] bool IsEmpty() const
    {
      return PointsOnCurve.IsEmpty() && PointsOnSurface.IsEmpty() && PointsOnPCurve.IsEmpty();
    }
  };

  Standard_EXPORT const VertexParams* FindVertexParams(const BRepGraph_VertexId theVertex) const;

  Standard_EXPORT bool FindPointOnCurve(const BRepGraph_VertexId theVertex,
                                        const BRepGraph_EdgeId   theEdge,
                                        double* const            theParameter = nullptr) const;

  Standard_EXPORT bool FindPointOnSurface(const BRepGraph_VertexId theVertex,
                                          const BRepGraph_FaceId   theFace,
                                          gp_Pnt2d* const          theUV = nullptr) const;

  Standard_EXPORT bool FindPointOnPCurve(const BRepGraph_VertexId theVertex,
                                         const BRepGraph_CoEdgeId theCoEdge,
                                         double* const            theParameter = nullptr) const;

  Standard_EXPORT uint32_t NbPointsOnCurve(const BRepGraph_VertexId theVertex) const;
  Standard_EXPORT uint32_t NbPointsOnSurface(const BRepGraph_VertexId theVertex) const;
  Standard_EXPORT uint32_t NbPointsOnPCurve(const BRepGraph_VertexId theVertex) const;

  [[nodiscard]] bool HasBindings() const { return myVertexParams.Extent() != 0; }

  Standard_EXPORT void SetPointOnCurve(const BRepGraph_VertexId theVertex,
                                       const BRepGraph_EdgeId   theEdge,
                                       const double             theParameter);

  Standard_EXPORT void SetPointOnSurface(const BRepGraph_VertexId theVertex,
                                         const BRepGraph_FaceId   theFace,
                                         const double             theParameterU,
                                         const double             theParameterV);

  Standard_EXPORT void SetPointOnPCurve(const BRepGraph_VertexId theVertex,
                                        const BRepGraph_CoEdgeId theCoEdge,
                                        const double             theParameter);

  Standard_EXPORT const TCollection_AsciiString& Name() const override;
  [[nodiscard]] Standard_EXPORT int              SubscribedKinds() const override;
  Standard_EXPORT void OnNodeModified(const BRepGraph_NodeId theNode) noexcept override;
  Standard_EXPORT void OnNodesModified(
    const NCollection_DynamicArray<BRepGraph_NodeId>& theModifiedNodes) noexcept override;
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode,
                                     const BRepGraph_NodeId theReplacement) noexcept override;
  Standard_EXPORT void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerParam, BRepGraph_Layer)

private:
  void removeVertexBindings(const BRepGraph_VertexId theVertex) noexcept;
  void invalidateEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept;
  void invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept;
  void invalidateCoEdgeBindings(const BRepGraph_CoEdgeId theCoEdge) noexcept;
  void migrateVertexBindings(const BRepGraph_VertexId theOldVertex,
                             const BRepGraph_VertexId theNewVertex) noexcept;
  void migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                           const BRepGraph_EdgeId theNewEdge) noexcept;
  void migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                           const BRepGraph_FaceId theNewFace) noexcept;
  void migrateCoEdgeBindings(const BRepGraph_CoEdgeId theOldCoEdge,
                             const BRepGraph_CoEdgeId theNewCoEdge) noexcept;

  VertexParams& changeVertexParams(const BRepGraph_VertexId theVertex);
  void bindEdgeToVertex(const BRepGraph_EdgeId theEdge, const BRepGraph_VertexId theVertex);
  void bindFaceToVertex(const BRepGraph_FaceId theFace, const BRepGraph_VertexId theVertex);
  void bindCoEdgeToVertex(const BRepGraph_CoEdgeId theCoEdge, const BRepGraph_VertexId theVertex);
  void unbindEdgeFromVertex(const BRepGraph_EdgeId   theEdge,
                            const BRepGraph_VertexId theVertex) noexcept;
  void unbindFaceFromVertex(const BRepGraph_FaceId   theFace,
                            const BRepGraph_VertexId theVertex) noexcept;
  void unbindCoEdgeFromVertex(const BRepGraph_CoEdgeId theCoEdge,
                              const BRepGraph_VertexId theVertex) noexcept;
  void removePointOnCurve(const BRepGraph_VertexId theVertex,
                          const BRepGraph_EdgeId   theEdge) noexcept;
  void removePointOnSurface(const BRepGraph_VertexId theVertex,
                            const BRepGraph_FaceId   theFace) noexcept;
  void removePointOnPCurve(const BRepGraph_VertexId theVertex,
                           const BRepGraph_CoEdgeId theCoEdge) noexcept;

private:
  NCollection_DataMap<BRepGraph_VertexId, VertexParams>                         myVertexParams;
  NCollection_DataMap<BRepGraph_EdgeId, NCollection_DynamicArray<BRepGraph_VertexId>> myEdgeToVertices;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_DynamicArray<BRepGraph_VertexId>> myFaceToVertices;
  NCollection_DataMap<BRepGraph_CoEdgeId, NCollection_DynamicArray<BRepGraph_VertexId>>
    myCoEdgeToVertices;
};

#endif // _BRepGraph_LayerParam_HeaderFile
