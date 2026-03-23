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

#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>

//! Namespace grouping all topology node types for BRepGraph.
//!
//! Two-layer architecture: Definitions (intrinsic data per unique TShape)
//! and Usages (context-specific data per occurrence in the containment hierarchy).
namespace BRepGraph_TopoNode
{

// =========================================================================
// Layer 1: Definitions — one per unique TShape
// =========================================================================

//! Fields shared by every topology definition.
struct BaseDef
{
  BRepGraph_NodeId Id;  //!< Typed address: (kind + per-kind index)

  //! Lazily-computed derived quantities + extensible user attributes.
  BRepGraph_NodeCache Cache;

  //! True when this definition or a descendant has been mutated since Build().
  bool IsModified = false;

  //! All usages of this definition in the containment tree.
  NCollection_Vector<BRepGraph_UsageId> Usages;
};

struct SolidDef : public BaseDef {};

struct ShellDef : public BaseDef {};

struct FaceDef : public BaseDef
{
  //! Geometry: the surface node that realizes this face.
  BRepGraph_NodeId SurfNodeId;

  //! Tolerance from BRep_TFace.
  double Tolerance = 0.0;
};

//! Wire definition holding edge winding order and closure status.
struct WireDef : public BaseDef
{
  //! One entry per edge, in topological winding order.
  struct EdgeEntry
  {
    BRepGraph_NodeId   EdgeDefId;
    TopAbs_Orientation OrientationInWire = TopAbs_FORWARD;
  };

  NCollection_Array1<EdgeEntry> OrderedEdges;

  //! True if first edge's start vertex == last edge's end vertex.
  bool IsClosed = false;
};

struct EdgeDef : public BaseDef
{
  //! 3D curve geometry (may be invalid for degenerate edges).
  BRepGraph_NodeId CurveNodeId;

  //! PCurve entries, one per (edge, face) context.
  struct PCurveEntry
  {
    BRepGraph_NodeId PCurveNodeId;
    BRepGraph_NodeId FaceDefId;
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
};

struct VertexDef : public BaseDef
{
  //! 3D point in TShape-local coordinates (NOT pre-transformed).
  gp_Pnt Point;

  //! Tolerance from BRep_TVertex.
  double Tolerance = 0.0;
};

// =========================================================================
// Layer 2: Usages — one per occurrence in the containment hierarchy
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

struct FaceUsage : public BaseUsage
{
  BRepGraph_UsageId OuterWireUsage;
  NCollection_Vector<BRepGraph_UsageId> InnerWireUsages;
};

struct WireUsage : public BaseUsage
{
  BRepGraph_UsageId OwnerFaceUsage;
  NCollection_Vector<BRepGraph_UsageId> EdgeUsages;  //!< Parallel to WireDef::OrderedEdges
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
