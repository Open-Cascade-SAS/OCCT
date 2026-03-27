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

#ifndef _BRepGraphInc_Entity_HeaderFile
#define _BRepGraphInc_Entity_HeaderFile

#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RepId.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_Vector.hxx>

//! @brief Entity structs for the incidence-table topology model.
//!
//! Each entity holds its intrinsic geometry plus forward-direction children
//! (incidence refs). The incidence model stores topology as flat vectors
//! of entities (one per kind) with integer cross-references, enabling
//! cache-friendly traversal and parallel geometry extraction.
//! Locations are stored on refs (VertexRef, CoEdgeRef, WireRef, FaceRef,
//! ShellRef, SolidRef) where they can differ per usage context.
namespace BRepGraphInc
{

//! Helper: reinitialize a vector member with the given allocator and block size.
template <typename T>
inline void InitVec(NCollection_Vector<T>&                        theVec,
                    const occ::handle<NCollection_BaseAllocator>& theAlloc,
                    const int                                     theBlockSize = 4)
{
  theVec = NCollection_Vector<T>(theBlockSize, theAlloc);
}

//! Fields shared by every entity.
struct BaseEntity
{
  BRepGraph_NodeId    Id;    //!< Typed address (kind + per-kind index)
  BRepGraph_NodeCache Cache; //!< Lazily-computed derived quantities + user attributes
  uint32_t            MutationGen =
    0; //!< Per-node mutation counter, incremented by markModified().
       //!< Wraps on overflow; callers compare via difference, not absolute value.
  bool IsModified = false; //!< True when mutated since Build(). Monotonic within a Build() cycle.
                           //!< Never cleared during mutation. Use MutationGen for granular checks;
                           //!< cache consumers should invalidate when true.
  bool IsRemoved = false;  //!< Soft-removal flag
};

//! Fields shared by every reference entry.
struct BaseRef
{
  BRepGraph_RefId  RefId;       //!< Typed address (kind + per-kind index)
  BRepGraph_NodeId ParentId;    //!< Parent topology node owning this reference usage
  uint32_t         MutationGen = 0; //!< Per-reference mutation counter
  bool             IsRemoved   = false; //!< Soft-removal flag
};

//! Fields shared by every representation entity.
struct BaseRep
{
  BRepGraph_RepId Id;                  //!< Typed address (Kind + per-kind index)
  uint32_t        MutationGen = 0;     //!< Per-rep mutation counter
  bool            IsRemoved   = false; //!< Soft-removal flag
};

//! Transitional shell reference storage entry.
struct ShellRefEntry : public BaseRef
{
  BRepGraph_ShellId  ShellEntityId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Transitional face reference storage entry.
struct FaceRefEntry : public BaseRef
{
  BRepGraph_FaceId   FaceEntityId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Transitional wire reference storage entry.
struct WireRefEntry : public BaseRef
{
  BRepGraph_WireId   WireEntityId;
  bool               IsOuter     = false;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Transitional coedge reference storage entry.
struct CoEdgeRefEntry : public BaseRef
{
  BRepGraph_CoEdgeId CoEdgeEntityId;
  TopLoc_Location    LocalLocation;
};

//! Transitional vertex reference storage entry.
struct VertexRefEntry : public BaseRef
{
  BRepGraph_VertexId VertexEntityId;
  TopAbs_Orientation Orientation = TopAbs_INTERNAL;
  TopLoc_Location    LocalLocation;
};

//! Transitional solid reference storage entry.
struct SolidRefEntry : public BaseRef
{
  BRepGraph_SolidId  SolidEntityId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Transitional child reference storage entry.
struct ChildRefEntry : public BaseRef
{
  BRepGraph_NodeId   ChildEntityId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Occurrence reference storage entry.
struct OccurrenceRefEntry : public BaseRef
{
  BRepGraph_OccurrenceId OccurrenceEntityId;
};

//! Surface geometry representation for faces.
struct SurfaceRep : public BaseRep
{
  occ::handle<Geom_Surface> Surface; //!< The geometric surface
};

//! 3D curve geometry representation for edges.
struct Curve3DRep : public BaseRep
{
  occ::handle<Geom_Curve> Curve; //!< The 3D curve geometry
};

//! 2D parametric curve (PCurve) representation for coedges.
struct Curve2DRep : public BaseRep
{
  occ::handle<Geom2d_Curve> Curve; //!< The 2D parametric curve
};

//! Triangulation mesh representation for faces.
struct TriangulationRep : public BaseRep
{
  occ::handle<Poly_Triangulation> Triangulation; //!< The mesh
};

//! 3D polygon discretization for edges.
struct Polygon3DRep : public BaseRep
{
  occ::handle<Poly_Polygon3D> Polygon; //!< The 3D polygon
};

//! 2D polygon-on-surface discretization for coedges.
struct Polygon2DRep : public BaseRep
{
  occ::handle<Poly_Polygon2D> Polygon; //!< The 2D polygon on surface parametric space
};

//! Polygon-on-triangulation for coedges.
//! Links a polygon to a specific triangulation rep (global index, not face-local).
struct PolygonOnTriRep : public BaseRep
{
  occ::handle<Poly_PolygonOnTriangulation> Polygon; //!< Polygon indices into triangulation
  BRepGraph_TriangulationRepId TriangulationRepId;  //!< Typed id into myTriangulationsRep
};

//! Vertex entity: 3D point + tolerance.
struct VertexEntity : public BaseEntity
{
  //! 3D point in definition frame (raw BRep_TVertex::Pnt, without vertex-in-edge Location).
  gp_Pnt Point;

  //! Tolerance from BRep_TVertex.
  double Tolerance = 0.0;

  //! Vertex parameter on a 3D curve.
  struct PointOnCurveEntry
  {
    double           Parameter = 0.0;
    BRepGraph_EdgeId EdgeEntityId; //!< Edge definition owning the curve
  };

  NCollection_Vector<PointOnCurveEntry> PointsOnCurve;

  //! Vertex parameter on a surface (U, V).
  struct PointOnSurfaceEntry
  {
    double           ParameterU = 0.0;
    double           ParameterV = 0.0;
    BRepGraph_FaceId FaceEntityId; //!< Face definition owning the surface
  };

  NCollection_Vector<PointOnSurfaceEntry> PointsOnSurface;

  //! Vertex parameter on a PCurve on a surface.
  struct PointOnPCurveEntry
  {
    double           Parameter = 0.0;
    BRepGraph_FaceId FaceEntityId; //!< Face definition owning the surface
  };

  NCollection_Vector<PointOnPCurveEntry> PointsOnPCurve;

  //! Reinitialize inner vectors with the given allocator.
  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(PointsOnCurve, theAlloc, 2); // typically 1-2 per vertex
    InitVec(PointsOnSurface, theAlloc, 2);
    InitVec(PointsOnPCurve, theAlloc, 2);
  }
};

//! Edge entity: parameter range, boundary vertices, flags.
//! Geometry (curve, polygon) accessed via rep indices into Storage vectors.
struct EdgeEntity : public BaseEntity
{
  //! Typed representation id into Storage::myCurves3D (invalid for degenerate edges).
  BRepGraph_Curve3DRepId Curve3DRepId;

  //! Curve parameter range.
  double ParamFirst = 0.0;
  double ParamLast  = 0.0;

  //! Tolerance from BRep_TEdge.
  double Tolerance = 0.0;

  //! True if this edge collapses to a point on the surface.
  bool IsDegenerate = false;

  //! True if all PCurves are reparametrized to the same range as the 3D curve.
  bool SameParameter = false;

  //! True if the PCurve parameter range equals the 3D curve parameter range.
  bool SameRange = false;

  //! True if StartVertex == EndVertex (topological loop, e.g. circle edge).
  bool IsClosed = false;

  //! Boundary vertex reference ids (indices into VertexRefEntry table).
  //! For closed edges, the start and end ref entries point to the same VertexEntityId.
  BRepGraph_VertexRefId StartVertexRefId;
  BRepGraph_VertexRefId EndVertexRefId;

  //! Additional vertex reference ids with INTERNAL or EXTERNAL orientation.
  //! Edges with only FORWARD/REVERSED boundary vertices leave this empty.
  NCollection_Vector<BRepGraph_VertexRefId> InternalVertexRefIds;

  //! Typed representation id into Storage::myPolygons3D (invalid if no polygon).
  BRepGraph_Polygon3DRepId Polygon3DRepId;

  //! Edge regularity (continuity) between adjacent face pairs.
  struct RegularityEntry
  {
    BRepGraph_FaceId FaceEntity1;
    BRepGraph_FaceId FaceEntity2;
    GeomAbs_Shape    Continuity = GeomAbs_C0;
  };

  NCollection_Vector<RegularityEntry> Regularities;

  //! Reinitialize inner vectors with the given allocator.
  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(Regularities, theAlloc, 2);         // typically 0-2
    InitVec(InternalVertexRefIds, theAlloc, 2); // typically 0
  }
};

//! CoEdge entity: use of an edge on a specific face, owns PCurve data.
//!
//! Each coedge represents one edge-face binding with its parametric curve.
//! Wires reference coedges rather than edges directly.
//! For seam edges, two coedges exist on the same face with opposite Sense,
//! linked by SeamPairIdx.
struct CoEdgeEntity : public BaseEntity
{
  BRepGraph_EdgeId   EdgeEntityId; //!< Parent edge definition id
  BRepGraph_FaceId   FaceEntityId; //!< Face this coedge belongs to (invalid for free wires)
  TopAbs_Orientation Sense = TopAbs_FORWARD; //!< Orientation relative to parent edge

  //! Typed representation id into Storage::myCurves2D (invalid for free-wire coedges).
  BRepGraph_Curve2DRepId Curve2DRepId;
  double                 ParamFirst = 0.0;
  double                 ParamLast  = 0.0;
  GeomAbs_Shape          Continuity = GeomAbs_C0; //!< Geometric continuity across face pairs
  gp_Pnt2d               UV1;                     //!< UV at ParamFirst
  gp_Pnt2d               UV2;                     //!< UV at ParamLast

  //! Seam pairing: typed id of the paired coedge (invalid if non-seam).
  BRepGraph_CoEdgeId SeamPairId;
  GeomAbs_Shape      SeamContinuity = GeomAbs_C0; //!< Continuity between seam pair

  //! Typed representation id into Storage::myPolygons2D (invalid if no polygon-on-surface).
  BRepGraph_Polygon2DRepId Polygon2DRepId;

  //! Typed representation ids into Storage::myPolygonsOnTri.
  NCollection_Vector<BRepGraph_PolygonOnTriRepId> PolygonOnTriRepIds;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(PolygonOnTriRepIds, theAlloc, 2);
  }
};

//! Wire entity: ordered coedge references with closure flag.
struct WireEntity : public BaseEntity
{
  bool IsClosed = false;
  NCollection_Vector<BRepGraph_CoEdgeRefId> CoEdgeRefIds; //!< Ordered coedge ref indices

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(CoEdgeRefIds, theAlloc, 8); // typically 3-8 coedges per wire
  }
};

//! Face entity: surface, triangulations, wires.
struct FaceEntity : public BaseEntity
{
  BRepGraph_SurfaceRepId SurfaceRepId; //!< Typed id into mySurfaces
  NCollection_Vector<BRepGraph_TriangulationRepId>
      TriangulationRepIds;           //!< Typed ids into myTriangulations
  int ActiveTriangulationIndex = -1; //!< Index into TriangulationRepIds array (NOT a rep id)

  //! Convenience: active triangulation rep id, or invalid.
  [[nodiscard]] BRepGraph_TriangulationRepId ActiveTriangulationRepId() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < TriangulationRepIds.Length())
      return TriangulationRepIds.Value(ActiveTriangulationIndex);
    return BRepGraph_TriangulationRepId();
  }

  double Tolerance          = 0.0;
  bool   NaturalRestriction = false;

  NCollection_Vector<BRepGraph_WireRefId> WireRefIds; //!< Wire ref indices (outer first)

  //! Direct INTERNAL/EXTERNAL vertex children (not inside wires).
  NCollection_Vector<BRepGraph_VertexRefId> VertexRefIds;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(TriangulationRepIds, theAlloc, 2); // typically 1
    InitVec(WireRefIds, theAlloc, 2);           // typically 1-2 (outer + holes)
    InitVec(VertexRefIds, theAlloc, 2);         // typically 0
  }
};

//! Shell entity: ordered face references with local locations.
struct ShellEntity : public BaseEntity
{
  bool IsClosed = false; //!< True if shell forms a watertight (closed) boundary.
  NCollection_Vector<BRepGraph_FaceRefId> FaceRefIds;     //!< Face ref indices
  NCollection_Vector<BRepGraph_ChildRefId> FreeChildRefIds; //!< Non-face children (wires, edges)

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(FaceRefIds, theAlloc, 8);        // typically 4-8 faces per shell
    InitVec(FreeChildRefIds, theAlloc, 2);   // typically 0
  }
};

//! Solid entity: ordered shell references with local locations.
struct SolidEntity : public BaseEntity
{
  NCollection_Vector<BRepGraph_ShellRefId> ShellRefIds;    //!< Shell ref indices
  NCollection_Vector<BRepGraph_ChildRefId> FreeChildRefIds; //!< Non-shell children (edges, vertices)

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(ShellRefIds, theAlloc, 2);       // typically 1
    InitVec(FreeChildRefIds, theAlloc, 2);   // typically 0
  }
};

//! Compound entity: heterogeneous child references.
struct CompoundEntity : public BaseEntity
{
  NCollection_Vector<BRepGraph_ChildRefId> ChildRefIds; //!< Child ref indices

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(ChildRefIds, theAlloc, 4);
  }
};

//! Comp-solid entity: ordered solid references.
struct CompSolidEntity : public BaseEntity
{
  NCollection_Vector<BRepGraph_SolidRefId> SolidRefIds; //!< Solid ref indices

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(SolidRefIds, theAlloc, 2);
  }
};

//! Product entity: reusable shape definition (part or assembly).
//! A part has a valid ShapeRootId pointing to the root topology node.
//! An assembly has an invalid ShapeRootId and owns child occurrences.
struct ProductEntity : public BaseEntity
{
  BRepGraph_NodeId   ShapeRootId; //!< Root topology for parts; invalid for assemblies
  TopAbs_Orientation RootOrientation = TopAbs_FORWARD; //!< Orientation of the root shape
  TopLoc_Location    RootLocation;                     //!< Location of the root shape
  NCollection_Vector<BRepGraph_OccurrenceRefId> OccurrenceRefIds; //!< Occurrence ref indices

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    InitVec(OccurrenceRefIds, theAlloc, 4);
  }
};

//! Occurrence entity: placed instance of a product within a parent product.
//! ParentOccurrenceIdx forms a tree-structured placement chain for
//! unambiguous GlobalPlacement computation even when products are shared (DAG).
struct OccurrenceEntity : public BaseEntity
{
  BRepGraph_ProductId    ProductEntityId;          //!< Referenced product definition id
  BRepGraph_ProductId    ParentProductEntityId;    //!< Parent assembly product definition id
  BRepGraph_OccurrenceId ParentOccurrenceEntityId; //!< Parent occurrence id (invalid for top-level)
  TopLoc_Location        Placement;             //!< Local placement relative to parent

  //! No-op: OccurrenceEntity has no inner vectors to reinitialize.
  //! Present for uniform EntityStore<T>::Append() logic.
  void InitVectors(const occ::handle<NCollection_BaseAllocator>&) {}
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Entity_HeaderFile
