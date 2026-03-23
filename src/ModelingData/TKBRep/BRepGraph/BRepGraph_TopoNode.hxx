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
#include <BRepGraph_UID.hxx>
#include <BRepGraph_NodeCache.hxx>

#include <TopoDS_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>

//! Namespace grouping all topology node types for BRepGraph.
//!
//! Each concrete node type is stored in its own typed vector inside
//! BRepGraph.  Inheritance from Base is purely for code reuse of common
//! fields; there is no polymorphic dispatch.
namespace BRepGraph_TopoNode
{

//! Fields shared by every topology node.
struct Base
{
  BRepGraph_NodeId Id;  //!< Typed address: (kind + per-kind index)
  BRepGraph_UID    UID; //!< Graph-wide unique identifier

  TopoDS_Shape    OriginalShape;  //!< The TopoDS_Shape this node was built from
  TopLoc_Location LocalLocation;  //!< Location stored on this specific shape
  TopLoc_Location GlobalLocation; //!< Accumulated transform from root to here

  //! Parent node in the containment hierarchy.
  //! Invalid for root Solid / CompSolid / free Shell.
  BRepGraph_NodeId Parent;

  //! Outgoing relationship edge indices (into BRepGraph::myRelEdges).
  NCollection_Vector<int> OutRelEdgeIndices;

  //! Incoming relationship edge indices (into BRepGraph::myRelEdges).
  NCollection_Vector<int> InRelEdgeIndices;

  //! Lazily-computed derived quantities + extensible user attributes.
  BRepGraph_NodeCache Cache;
};

struct Solid : public Base
{
  //! Child Shell ids, in containment order.
  NCollection_Vector<BRepGraph_NodeId> Shells;
};

struct Shell : public Base
{
  //! Child Face ids, in containment order.
  NCollection_Vector<BRepGraph_NodeId> Faces;
};

struct Face : public Base
{
  //! Geometry: the surface node that realizes this face.
  BRepGraph_NodeId SurfNodeId;

  //! The single outer WireNode.
  BRepGraph_NodeId OuterWireId;

  //! Zero or more inner WireNodes (holes).
  NCollection_Vector<BRepGraph_NodeId> InnerWireIds;

  //! Face orientation relative to its surface normal.
  TopAbs_Orientation Orientation = TopAbs_FORWARD;

  //! Tolerance from BRep_TFace.
  double Tolerance = 0.0;
};

//! Wire node holding edge winding order and closure status.
struct Wire : public Base
{
  //! The Face that owns this wire.
  BRepGraph_NodeId OwnerFaceId;

  //! One entry per edge, in topological winding order.
  struct EdgeEntry
  {
    BRepGraph_NodeId   EdgeId;
    TopAbs_Orientation OrientationInWire = TopAbs_FORWARD;
  };

  NCollection_Array1<EdgeEntry> OrderedEdges;

  //! True if first edge's start vertex == last edge's end vertex.
  bool IsClosed = false;
};

struct Edge : public Base
{
  //! 3D curve geometry (may be invalid for degenerate edges).
  BRepGraph_NodeId CurveNodeId;

  //! PCurve entries, one per (edge, face) context.
  struct PCurveEntry
  {
    BRepGraph_NodeId PCurveNodeId;
    BRepGraph_NodeId FaceNodeId;
  };

  NCollection_Vector<PCurveEntry> PCurves;

  //! Boundary vertices.  For closed edges, Start == End.
  BRepGraph_NodeId StartVertexId;
  BRepGraph_NodeId EndVertexId;

  //! Curve parameter range.
  double ParamFirst = 0.0;
  double ParamLast  = 0.0;

  //! Tolerance from BRep_TEdge.
  double Tolerance = 0.0;

  //! True if this edge collapses to a point on the surface.
  bool IsDegenerate = false;
};

struct Vertex : public Base
{
  //! 3D point, already transformed by GlobalLocation.
  gp_Pnt Point;

  //! Tolerance from BRep_TVertex.
  double Tolerance = 0.0;
};

} // namespace BRepGraph_TopoNode

#endif // _BRepGraph_TopoNode_HeaderFile
