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

//! Helper: reinitialize a vector member with the given allocator and block size.
template <typename T>
inline void BRepGraphInc_InitVec(NCollection_Vector<T>& theVec,
                                 const occ::handle<NCollection_BaseAllocator>& theAlloc,
                                 int theBlockSize = 4)
{
  theVec = NCollection_Vector<T>(theBlockSize, theAlloc);
}

//! Entity structs for the incidence-table model.
//!
//! Each entity holds its intrinsic geometry plus forward-direction children
//! (incidence refs).  Locations are stored only at Shell/Solid/Compound level
//! where they can be non-identity.  No separate Usage objects are needed.
namespace BRepGraphInc
{

//! Fields shared by every entity.
struct BaseEntity
{
  BRepGraph_NodeId   Id;            //!< Typed address (kind + per-kind index)
  BRepGraph_NodeCache Cache;        //!< Lazily-computed derived quantities + user attributes
  uint32_t           MutationGen = 0;    //!< Per-node mutation counter, incremented by markModified().
                                        //!< Wraps on overflow; callers compare via difference, not absolute value.
  bool               IsModified = false; //!< True when mutated since Build()
  bool               IsRemoved = false;  //!< Soft-removal flag
};

//! Fields shared by every representation entity.
struct BaseRep
{
  BRepGraph_RepId Id;            //!< Typed address (RepKind + per-kind index)
  uint32_t        MutationGen = 0;    //!< Per-rep mutation counter
  bool            IsRemoved = false;  //!< Soft-removal flag
};

//! Surface geometry representation for faces.
struct SurfaceRep : public BaseRep
{
  occ::handle<Geom_Surface> Surface;  //!< The geometric surface
};

//! 3D curve geometry representation for edges.
struct Curve3DRep : public BaseRep
{
  occ::handle<Geom_Curve> Curve;  //!< The 3D curve geometry
};

//! 2D parametric curve (PCurve) representation for coedges.
struct Curve2DRep : public BaseRep
{
  occ::handle<Geom2d_Curve> Curve;  //!< The 2D parametric curve
};

//! Triangulation mesh representation for faces.
struct TriangulationRep : public BaseRep
{
  occ::handle<Poly_Triangulation> Triangulation;  //!< The mesh
};

//! 3D polygon discretization for edges.
struct Polygon3DRep : public BaseRep
{
  occ::handle<Poly_Polygon3D> Polygon;  //!< The 3D polygon
};

//! 2D polygon-on-surface discretization for coedges.
struct Polygon2DRep : public BaseRep
{
  occ::handle<Poly_Polygon2D> Polygon;  //!< The 2D polygon on surface parametric space
};

//! Polygon-on-triangulation for coedges.
//! Links a polygon to a specific triangulation rep (global index, not face-local).
struct PolygonOnTriRep : public BaseRep
{
  occ::handle<Poly_PolygonOnTriangulation> Polygon;  //!< Polygon indices into triangulation
  int TriangulationRepIdx = -1;  //!< Global index into myTriangulationsRep
};

//! Vertex entity: 3D point + tolerance.
struct VertexEntity : public BaseEntity
{
  //! 3D point in global coordinates (from BRep_Tool::Pnt, which applies vertex location).
  gp_Pnt Point;

  //! Tolerance from BRep_TVertex.
  double Tolerance = 0.0;

  //! Vertex parameter on a 3D curve.
  struct PointOnCurveEntry
  {
    double           Parameter = 0.0;
    BRepGraph_NodeId EdgeDefId; //!< Edge definition owning the curve
  };
  NCollection_Vector<PointOnCurveEntry> PointsOnCurve;

  //! Vertex parameter on a surface (U, V).
  struct PointOnSurfaceEntry
  {
    double           ParameterU = 0.0;
    double           ParameterV = 0.0;
    BRepGraph_NodeId FaceDefId; //!< Face definition owning the surface
  };
  NCollection_Vector<PointOnSurfaceEntry> PointsOnSurface;

  //! Vertex parameter on a PCurve on a surface.
  struct PointOnPCurveEntry
  {
    double           Parameter = 0.0;
    BRepGraph_NodeId FaceDefId; //!< Face definition owning the surface
  };
  NCollection_Vector<PointOnPCurveEntry> PointsOnPCurve;

  //! Reinitialize inner vectors with the given allocator.
  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(PointsOnCurve, theAlloc, 2);    // typically 1-2 per vertex
    BRepGraphInc_InitVec(PointsOnSurface, theAlloc, 2);
    BRepGraphInc_InitVec(PointsOnPCurve, theAlloc, 2);
  }
};

//! Edge entity: 3D curve, parameter range, boundary vertices, flags, polygon.
//! PCurve, polygon-on-surface/triangulation, and regularity entries stored inline.
struct EdgeEntity : public BaseEntity
{
  //! 3D curve geometry (direct handle, null for degenerate edges).
  occ::handle<Geom_Curve> Curve3d;

  //! Representation index into Storage::myCurves3D (-1 if no 3D curve).
  int Curve3DRepIdx = -1;

  //! PCurve entries, one per (edge, face) context.
  //! For seam edges there are two entries with the same FaceDefId,
  //! distinguished by EdgeOrientation (FORWARD vs REVERSED).
  struct PCurveEntry
  {
    occ::handle<Geom2d_Curve> Curve2d;          //!< 2D parametric curve on the face surface
    BRepGraph_NodeId     FaceDefId;
    double               ParamFirst = 0.0;
    double               ParamLast  = 0.0;
    GeomAbs_Shape        Continuity = GeomAbs_C0; //!< Geometric continuity across face pairs
    gp_Pnt2d             UV1;              //!< UV at ParamFirst
    gp_Pnt2d             UV2;              //!< UV at ParamLast
    TopAbs_Orientation   EdgeOrientation = TopAbs_FORWARD; //!< Edge orientation when this PCurve was extracted.
    GeomAbs_Shape        SeamContinuity = GeomAbs_C0; //!< Continuity between seam PCurve pair (for REVERSED entry)
  };
  NCollection_Vector<PCurveEntry> PCurves;

  //! Curve parameter range.
  double ParamFirst = 0.0;
  double ParamLast  = 0.0;

  //! Tolerance from BRep_TEdge.
  double Tolerance = 0.0;

  //! True if this edge collapses to a point on the surface.
  bool IsDegenerate  = false;

  //! True if all PCurves are reparametrized to the same range as the 3D curve.
  bool SameParameter = false;

  //! True if the PCurve parameter range equals the 3D curve parameter range.
  bool SameRange     = false;

  //! Index into VertexEntity vector (boundary vertices).
  //! For closed edges, StartVertexIdx == EndVertexIdx.
  int StartVertexIdx = -1;
  int EndVertexIdx   = -1;

  //! Additional vertices with INTERNAL or EXTERNAL orientation.
  //! Edges with only FORWARD/REVERSED boundary vertices leave this empty.
  NCollection_Vector<VertexRef> InternalVertices;

  //! Convenience: start vertex NodeId from index.
  BRepGraph_NodeId StartVertexDefId() const
  {
    return StartVertexIdx >= 0 ? BRepGraph_NodeId::Vertex(StartVertexIdx) : BRepGraph_NodeId();
  }

  //! Convenience: end vertex NodeId from index.
  BRepGraph_NodeId EndVertexDefId() const
  {
    return EndVertexIdx >= 0 ? BRepGraph_NodeId::Vertex(EndVertexIdx) : BRepGraph_NodeId();
  }

  //! Optional 3D polygon discretization (stored inline, not as a graph node).
  occ::handle<Poly_Polygon3D> Polygon3D;

  //! Representation index into Storage::myPolygons3D (-1 if no polygon).
  int Polygon3DRepIdx = -1;

  //! Polygon-on-surface entries, one per (edge, face) context.
  struct PolyOnSurfEntry
  {
    occ::handle<Poly_Polygon2D> Polygon2D;
    BRepGraph_NodeId       FaceDefId;
    TopAbs_Orientation     EdgeOrientation = TopAbs_FORWARD;
  };
  NCollection_Vector<PolyOnSurfEntry> PolygonsOnSurf;

  //! Polygon-on-triangulation entries, one per (edge, face, triangulation) context.
  struct PolyOnTriEntry
  {
    occ::handle<Poly_PolygonOnTriangulation> Polygon;
    BRepGraph_NodeId                    FaceDefId;
    int                                 TriangulationIndex = 0;
    TopAbs_Orientation                  EdgeOrientation = TopAbs_FORWARD;
  };
  NCollection_Vector<PolyOnTriEntry> PolygonsOnTri;

  //! Edge regularity (continuity) between adjacent face pairs.
  struct RegularityEntry
  {
    BRepGraph_NodeId FaceDef1;
    BRepGraph_NodeId FaceDef2;
    GeomAbs_Shape    Continuity = GeomAbs_C0;
  };
  NCollection_Vector<RegularityEntry> Regularities;

  //! Reinitialize inner vectors with the given allocator.
  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(PCurves, theAlloc, 4);           // typically 1-4 (seam edges have 2)
    BRepGraphInc_InitVec(PolygonsOnSurf, theAlloc, 2);   // typically 0-2
    BRepGraphInc_InitVec(PolygonsOnTri, theAlloc, 2);    // typically 0-2
    BRepGraphInc_InitVec(Regularities, theAlloc, 2);     // typically 0-2
    BRepGraphInc_InitVec(InternalVertices, theAlloc, 2); // typically 0
  }

  //! Return the start vertex adjusted for orientation in wire context.
  //! FORWARD -> StartVertexDefId(), REVERSED -> EndVertexDefId(), other -> invalid.
  BRepGraph_NodeId OrientedStartVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return StartVertexDefId();
    if (theOri == TopAbs_REVERSED) return EndVertexDefId();
    return BRepGraph_NodeId();
  }

  //! Return the end vertex adjusted for orientation in wire context.
  //! FORWARD -> EndVertexDefId(), REVERSED -> StartVertexDefId(), other -> invalid.
  BRepGraph_NodeId OrientedEndVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return EndVertexDefId();
    if (theOri == TopAbs_REVERSED) return StartVertexDefId();
    return BRepGraph_NodeId();
  }
};

//! CoEdge entity: use of an edge on a specific face, owns PCurve data.
//!
//! Follows the Parasolid COEDGE pattern (Weiler half-edge literature).
//! Each coedge represents one edge-face binding with its parametric curve.
//! Wires reference coedges rather than edges directly.
//! For seam edges, two coedges exist on the same face with opposite Sense,
//! linked by SeamPairIdx.
struct CoEdgeEntity : public BaseEntity
{
  int                EdgeIdx = -1;           //!< Parent edge index
  BRepGraph_NodeId   FaceDefId;              //!< Face this coedge belongs to (invalid for free wires)
  TopAbs_Orientation Sense = TopAbs_FORWARD; //!< Orientation relative to parent edge

  //! PCurve data (null for free-wire coedges without face context).
  occ::handle<Geom2d_Curve> Curve2d;

  //! Representation index into Storage::myCurves2D (-1 if no PCurve).
  int Curve2DRepIdx = -1;
  double               ParamFirst = 0.0;
  double               ParamLast  = 0.0;
  GeomAbs_Shape        Continuity = GeomAbs_C0; //!< Geometric continuity across face pairs
  gp_Pnt2d             UV1;                     //!< UV at ParamFirst
  gp_Pnt2d             UV2;                     //!< UV at ParamLast

  //! Seam pairing: index of the paired coedge (-1 if non-seam).
  int                  SeamPairIdx = -1;
  GeomAbs_Shape        SeamContinuity = GeomAbs_C0; //!< Continuity between seam pair

  //! Polygon-on-surface for this face context.
  occ::handle<Poly_Polygon2D> PolygonOnSurf;

  //! Representation index into Storage::myPolygons2D (-1 if no polygon-on-surface).
  int Polygon2DRepIdx = -1;

  //! Polygon-on-triangulation entries for this face context.
  struct PolyOnTriEntry
  {
    occ::handle<Poly_PolygonOnTriangulation> Polygon;
    int                                 TriangulationIndex = 0;
  };
  NCollection_Vector<PolyOnTriEntry> PolygonsOnTri;

  //! Representation indices into Storage::myPolygonsOnTri.
  NCollection_Vector<int> PolygonOnTriRepIdxs;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(PolygonsOnTri, theAlloc, 2);
    BRepGraphInc_InitVec(PolygonOnTriRepIdxs, theAlloc, 2);
  }
};

//! Wire entity: ordered coedge references with closure flag.
struct WireEntity : public BaseEntity
{
  bool IsClosed = false;
  NCollection_Vector<CoEdgeRef> CoEdgeRefs;  //!< Ordered coedge references

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(CoEdgeRefs, theAlloc, 8);      // typically 3-8 coedges per wire
  }
};

//! Face entity: surface, triangulations, wires.
struct FaceEntity : public BaseEntity
{
  occ::handle<Geom_Surface> Surface;
  NCollection_Vector<occ::handle<Poly_Triangulation>> Triangulations;
  int    ActiveTriangulationIndex = -1;

  //! Representation indices into Storage rep vectors.
  int SurfaceRepIdx = -1;                       //!< Index into mySurfaces
  NCollection_Vector<int> TriangulationRepIdxs;  //!< Indices into myTriangulations

  //! Convenience: active triangulation rep index, or -1.
  int ActiveTriangulationRepIdx() const
  {
    if (ActiveTriangulationIndex >= 0
        && ActiveTriangulationIndex < TriangulationRepIdxs.Length())
      return TriangulationRepIdxs.Value(ActiveTriangulationIndex);
    return -1;
  }
  double Tolerance = 0.0;
  bool   NaturalRestriction = false;

  //! Convenience: return the active triangulation handle, or null if none.
  occ::handle<Poly_Triangulation> ActiveTriangulation() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < Triangulations.Length())
      return Triangulations.Value(ActiveTriangulationIndex);
    return occ::handle<Poly_Triangulation>();
  }

  //! Wire references: outer wire first (if present), then inner wires.
  NCollection_Vector<WireRef> WireRefs;

  //! Direct INTERNAL/EXTERNAL vertex children (not inside wires).
  NCollection_Vector<VertexRef> VertexRefs;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(Triangulations, theAlloc, 2);        // typically 1
    BRepGraphInc_InitVec(TriangulationRepIdxs, theAlloc, 2);  // typically 1
    BRepGraphInc_InitVec(WireRefs, theAlloc, 2);              // typically 1-2 (outer + holes)
    BRepGraphInc_InitVec(VertexRefs, theAlloc, 2);            // typically 0
  }

  //! Return index of the outer wire, or -1 if none.
  int OuterWireIdx() const
  {
    for (int i = 0; i < WireRefs.Length(); ++i)
    {
      if (WireRefs.Value(i).IsOuter)
        return WireRefs.Value(i).WireIdx;
    }
    return -1;
  }
};

//! Shell entity: ordered face references with local locations.
struct ShellEntity : public BaseEntity
{
  NCollection_Vector<FaceRef> FaceRefs;
  NCollection_Vector<ChildRef> FreeChildRefs; //!< Non-face children (wires, edges)

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(FaceRefs, theAlloc, 8);        // typically 4-8 faces per shell
  }
};

//! Solid entity: ordered shell references with local locations.
struct SolidEntity : public BaseEntity
{
  NCollection_Vector<ShellRef> ShellRefs;
  NCollection_Vector<ChildRef> FreeChildRefs; //!< Non-shell children (edges, vertices)

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(ShellRefs, theAlloc, 2);       // typically 1
  }
};

//! Compound entity: heterogeneous child references.
struct CompoundEntity : public BaseEntity
{
  NCollection_Vector<ChildRef> ChildRefs;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(ChildRefs, theAlloc, 4);       // varies
  }
};

//! Comp-solid entity: ordered solid references.
struct CompSolidEntity : public BaseEntity
{
  NCollection_Vector<SolidRef> SolidRefs;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(SolidRefs, theAlloc, 2);       // typically 1-2
  }
};

//! Reference from a product to one of its child occurrences.
struct OccurrenceRef
{
  int OccurrenceIdx = -1;
};

//! Product entity: reusable shape definition (part or assembly).
//! A part has a valid ShapeRootId pointing to the root topology node.
//! An assembly has an invalid ShapeRootId and owns child occurrences.
struct ProductEntity : public BaseEntity
{
  BRepGraph_NodeId ShapeRootId;  //!< Root topology for parts; invalid for assemblies
  NCollection_Vector<OccurrenceRef> OccurrenceRefs;

  void InitVectors(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    BRepGraphInc_InitVec(OccurrenceRefs, theAlloc, 4);
  }
};

//! Occurrence entity: placed instance of a product within a parent product.
//! ParentOccurrenceIdx forms a tree-structured placement chain for
//! unambiguous GlobalPlacement computation even when products are shared (DAG).
struct OccurrenceEntity : public BaseEntity
{
  int             ProductIdx         = -1;  //!< Referenced product index
  int             ParentProductIdx   = -1;  //!< Parent assembly product index
  int             ParentOccurrenceIdx = -1; //!< Parent occurrence index (-1 for top-level)
  TopLoc_Location Placement;                //!< Local placement relative to parent
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Entity_HeaderFile
