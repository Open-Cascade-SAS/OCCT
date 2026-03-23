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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <NCollection_Vector.hxx>

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
  bool               IsModified = false; //!< True when mutated since Build()
  bool               IsRemoved = false;  //!< Soft-removal flag
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
};

//! Edge entity: 3D curve, parameter range, boundary vertices, flags, polygon.
//! PCurve, polygon-on-surface/triangulation, and regularity entries stored inline.
struct EdgeEntity : public BaseEntity
{
  //! 3D curve geometry (direct handle, null for degenerate edges).
  Handle(Geom_Curve) Curve3d;

  //! PCurve entries, one per (edge, face) context.
  //! For seam edges there are two entries with the same FaceDefId,
  //! distinguished by EdgeOrientation (FORWARD vs REVERSED).
  struct PCurveEntry
  {
    Handle(Geom2d_Curve) Curve2d;          //!< 2D parametric curve on the face surface
    BRepGraph_NodeId     FaceDefId;
    double               ParamFirst = 0.0;
    double               ParamLast  = 0.0;
    GeomAbs_Shape        Continuity = GeomAbs_C0; //!< Geometric continuity across face pairs
    gp_Pnt2d             UV1;              //!< UV at ParamFirst
    gp_Pnt2d             UV2;              //!< UV at ParamLast
    TopAbs_Orientation   EdgeOrientation = TopAbs_FORWARD; //!< Edge orientation when this PCurve was extracted.
  };
  NCollection_Vector<PCurveEntry> PCurves;

  //! Boundary vertex definitions.  For closed edges, Start == End.
  BRepGraph_NodeId StartVertexDefId;
  BRepGraph_NodeId EndVertexDefId;

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

  //! Index into VertexEntity vector (internal incidence model use).
  int StartVertexIdx = -1;
  int EndVertexIdx   = -1;

  //! Optional 3D polygon discretization (stored inline, not as a graph node).
  Handle(Poly_Polygon3D) Polygon3D;

  //! Polygon-on-surface entries, one per (edge, face) context.
  struct PolyOnSurfEntry
  {
    Handle(Poly_Polygon2D) Polygon2D;
    BRepGraph_NodeId       FaceDefId;
    TopAbs_Orientation     EdgeOrientation = TopAbs_FORWARD;
  };
  NCollection_Vector<PolyOnSurfEntry> PolygonsOnSurf;

  //! Polygon-on-triangulation entries, one per (edge, face, triangulation) context.
  struct PolyOnTriEntry
  {
    Handle(Poly_PolygonOnTriangulation) Polygon;
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

  //! Return the start vertex adjusted for orientation in wire context.
  //! FORWARD -> StartVertexDefId, REVERSED -> EndVertexDefId, other -> invalid.
  BRepGraph_NodeId OrientedStartVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return StartVertexDefId;
    if (theOri == TopAbs_REVERSED) return EndVertexDefId;
    return BRepGraph_NodeId();
  }

  //! Return the end vertex adjusted for orientation in wire context.
  //! FORWARD -> EndVertexDefId, REVERSED -> StartVertexDefId, other -> invalid.
  BRepGraph_NodeId OrientedEndVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return EndVertexDefId;
    if (theOri == TopAbs_REVERSED) return StartVertexDefId;
    return BRepGraph_NodeId();
  }
};

//! Wire entity: ordered edges with orientations, closure flag.
struct WireEntity : public BaseEntity
{
  bool IsClosed = false;
  NCollection_Vector<EdgeRef> EdgeRefs;  //!< Ordered edge references
};

//! Face entity: surface, triangulations, wires.
struct FaceEntity : public BaseEntity
{
  Handle(Geom_Surface) Surface;
  NCollection_Vector<Handle(Poly_Triangulation)> Triangulations;
  int    ActiveTriangulationIndex = -1;
  double Tolerance = 0.0;
  bool   NaturalRestriction = false;

  //! Convenience: return the active triangulation handle, or null if none.
  Handle(Poly_Triangulation) ActiveTriangulation() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < Triangulations.Length())
      return Triangulations.Value(ActiveTriangulationIndex);
    return Handle(Poly_Triangulation)();
  }

  //! Wire references: outer wire first (if present), then inner wires.
  NCollection_Vector<WireRef> WireRefs;

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
};

//! Solid entity: ordered shell references with local locations.
struct SolidEntity : public BaseEntity
{
  NCollection_Vector<ShellRef> ShellRefs;
};

//! Compound entity: heterogeneous child references.
struct CompoundEntity : public BaseEntity
{
  NCollection_Vector<ChildRef> ChildRefs;

  //! Child definition NodeIds (Compound, CompSolid, Solid, Shell, or Face kinds).
  //! Derived from ChildRefs; populated during legacy derivation.
  NCollection_Vector<BRepGraph_NodeId> ChildDefIds;
};

//! Comp-solid entity: ordered solid references.
struct CompSolidEntity : public BaseEntity
{
  NCollection_Vector<SolidRef> SolidRefs;

  //! Child solid definition NodeIds.
  //! Derived from SolidRefs; populated during legacy derivation.
  NCollection_Vector<BRepGraph_NodeId> SolidDefIds;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Entity_HeaderFile
