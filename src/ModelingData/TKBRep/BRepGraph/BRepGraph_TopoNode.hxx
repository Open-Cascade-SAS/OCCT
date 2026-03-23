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

#ifndef _BRepGraph_TopoNode_HeaderFile
#define _BRepGraph_TopoNode_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_UsageId.hxx>
#include <BRepGraph_NodeCache.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <NCollection_Vector.hxx>

//! Namespace grouping all topology node types for BRepGraph.
//!
//! Two-layer architecture: Definitions (intrinsic data per unique TShape)
//! and Usages (context-specific data per occurrence in the containment hierarchy).
namespace BRepGraph_TopoNode
{

// =========================================================================
// Layer 1: Definitions -- one per unique TShape
// =========================================================================

//! Fields shared by every topology definition.
struct BaseDef
{
  BRepGraph_NodeId Id;  //!< Typed address: (kind + per-kind index)

  //! Lazily-computed derived quantities + extensible user attributes.
  BRepGraph_NodeCache Cache;

  //! True when this definition or a descendant has been mutated since Build().
  bool IsModified = false;

  //! True when this definition has been soft-removed via RemoveNode().
  //! Removed nodes remain in storage but are skipped by iterators and queries.
  bool IsRemoved = false;

  //! All usages of this definition in the containment tree.
  NCollection_Vector<BRepGraph_UsageId> Usages;
};

struct SolidDef : public BaseDef {};

struct ShellDef : public BaseDef {};

struct CompoundDef : public BaseDef
{
  //! Child definition NodeIds (Compound, CompSolid, Solid, Shell, or Face kinds).
  NCollection_Vector<BRepGraph_NodeId> ChildDefIds;
};

struct CompSolidDef : public BaseDef
{
  //! Child solid definition NodeIds.
  NCollection_Vector<BRepGraph_NodeId> SolidDefIds;
};

struct FaceDef : public BaseDef
{
  //! Surface geometry (direct handle, no separate geometry node).
  Handle(Geom_Surface) Surface;

  //! All triangulations attached to this face (may be empty).
  NCollection_Vector<Handle(Poly_Triangulation)> Triangulations;
  int ActiveTriangulationIndex = -1; //!< Index into Triangulations, -1 = none active

  //! Convenience: return the active triangulation handle, or null if none.
  Handle(Poly_Triangulation) ActiveTriangulation() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < Triangulations.Length())
      return Triangulations.Value(ActiveTriangulationIndex);
    return Handle(Poly_Triangulation)();
  }

  //! Tolerance from BRep_TFace.
  double Tolerance = 0.0;

  //! True if face boundary coincides with the surface's natural parametric
  //! domain.  Wire topology is still stored.
  bool NaturalRestriction = false;
};

//! Wire definition holding closure status.
//! Edge ordering and orientation are stored on WireUsage::EdgeUsages.
struct WireDef : public BaseDef
{
  //! True if first edge's start vertex == last edge's end vertex.
  bool IsClosed = false;
};

struct EdgeDef : public BaseDef
{
  //! 3D curve geometry (direct handle, null for degenerate edges).
  Handle(Geom_Curve) Curve3d;

  //! PCurve entries, one per (edge, face) context.
  //! For seam edges there are two entries with the same FaceDefId,
  //! distinguished by EdgeOrientation (FORWARD vs REVERSED).
  //! Self-contained: all PCurve geometry is stored inline (no separate PCurve graph node).
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
  bool IsDegenerate = false;

  //! True if all PCurves are reparametrized to the same range as the 3D curve.
  //! Required by BRepCheck_Analyzer and downstream algorithms.
  bool SameParameter = false;

  //! True if the PCurve parameter range equals the 3D curve parameter range.
  bool SameRange = false;

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

struct VertexDef : public BaseDef
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

// =========================================================================
// Layer 2: Usages -- one per occurrence in the containment hierarchy
// =========================================================================

//! Fields shared by every topology usage.
struct BaseUsage
{
  BRepGraph_UsageId  UsageId;
  BRepGraph_NodeId   DefId;          //!< Points to definition
  TopLoc_Location    LocalLocation;
  TopLoc_Location    GlobalLocation;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  BRepGraph_UsageId  ParentUsage;    //!< Parent in usage tree (invalid for roots)
};

struct SolidUsage : public BaseUsage
{
  NCollection_Vector<BRepGraph_UsageId> ShellUsages;
};

struct ShellUsage : public BaseUsage
{
  NCollection_Vector<BRepGraph_UsageId> FaceUsages;
};

struct CompoundUsage : public BaseUsage
{
  NCollection_Vector<BRepGraph_UsageId> ChildUsages;
};

struct CompSolidUsage : public BaseUsage
{
  NCollection_Vector<BRepGraph_UsageId> SolidUsages;
};

struct FaceUsage : public BaseUsage
{
  BRepGraph_UsageId OuterWireUsage;
  NCollection_Vector<BRepGraph_UsageId> InnerWireUsages;

  //! Total number of wire usages (outer + inner).
  int NbWireUsages() const
  {
    return (OuterWireUsage.IsValid() ? 1 : 0) + InnerWireUsages.Length();
  }

  //! Access wire usage by unified index (outer wire first, then inner wires).
  //! @param[in] theIdx zero-based index in [0, NbWireUsages())
  BRepGraph_UsageId WireUsage(int theIdx) const
  {
    if (OuterWireUsage.IsValid())
    {
      if (theIdx == 0) return OuterWireUsage;
      return InnerWireUsages.Value(theIdx - 1);
    }
    return InnerWireUsages.Value(theIdx);
  }
};

struct WireUsage : public BaseUsage
{
  BRepGraph_UsageId OwnerFaceUsage;
  NCollection_Vector<BRepGraph_UsageId> EdgeUsages;  //!< Ordered edge usages in topological winding order
};

struct EdgeUsage : public BaseUsage
{
  BRepGraph_UsageId StartVertexUsage;
  BRepGraph_UsageId EndVertexUsage;
};

struct VertexUsage : public BaseUsage
{
  gp_Pnt TransformedPoint;  //!< Point in global coordinates
};

} // namespace BRepGraph_TopoNode

#endif // _BRepGraph_TopoNode_HeaderFile
