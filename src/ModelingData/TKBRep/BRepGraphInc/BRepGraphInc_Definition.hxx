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

#ifndef _BRepGraphInc_Definition_HeaderFile
#define _BRepGraphInc_Definition_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraphInc_ParityOrientation.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraphInc_Instance.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

//! @brief Definition structs for the incidence-table topology model.
//!
//! Each definition holds intrinsic geometry properties. Ordered child
//! incidence lives in BRepGraphInc_Relations and reusable parent/child usage
//! edges live in reference records.
namespace BRepGraphInc
{

//! Fields shared by every entity.
struct BaseDef
{
  using TypeId = BRepGraph_NodeId;

  //! Persistent per-kind UID counter value.
  //! 0 = invalid sentinel (not yet allocated). Valid UIDs start at 1.
  //! Kind is implicit from the concrete struct type (VertexDef, EdgeDef, etc.).
  uint32_t UID = 0;

  //! Own-data mutation counter, incremented ONLY when the entity's own
  //! definition fields change (tolerance, point, flags, etc.).
  //! NOT incremented by descendant changes.
  //! Used by VersionStamp for persistent identity staleness detection.
  uint32_t OwnGen = 0;

  //! Subtree mutation counter, incremented when own data OR any descendant
  //! data changes. Propagated upward via markParentSubtreeGen().
  //! Used by TransientCache and shape cache for hierarchical freshness.
  uint32_t SubtreeGen = 0;

  //! Wave counter from the last propagation that visited this node.
  //! Used as a re-visit guard in markParentSubtreeGen() to prevent
  //! exponential blowup on diamond topologies. Compared against
  //! BRepGraphInc_Storage::myPropagationWave.
  uint32_t LastPropWave = 0;
};

//! Vertex definition: 3D point + tolerance.
struct VertexDef : public BaseDef
{
  using TypeId = BRepGraph_VertexId;

  //! 3D point in definition frame (raw BRep_TVertex::Pnt, without vertex-in-edge Location).
  gp_Pnt Point;

  //! Tolerance from BRep_TVertex.
  double Tolerance = 0.0;
};

//! Edge entity: parameter range, boundary vertices.
//! Geometry (curve, polygon) accessed via owned use records.
//! Degeneracy, closure, SameRange, and SameParameter are derived from
//! current topology and geometry via BRepGraph_CacheDerivedState.
struct EdgeDef : public BaseDef
{
  using TypeId = BRepGraph_EdgeId;

  BRepGraph_EdgeCurve3DRepId Curve3DRepId; //!< Owned 3D curve use id (invalid for degenerate edges)

  double Tolerance = 0.0; //!< Tolerance from BRep_TEdge

  BRepGraph_VertexRefId StartVertexRefId; //!< Start vertex reference
  BRepGraph_VertexRefId EndVertexRefId;   //!< End vertex reference

  BRepGraph_EdgePolygon3DRepId Polygon3DRepId; //!< Owned 3D polygon use id
};

//! CoEdge entity: use of an edge on a specific face, owns PCurve data.
//!
//! Each coedge represents one edge-face binding with its parametric curve.
//! Wires reference coedges rather than edges directly.
//! Seam edges produce two coedges on the same face with opposite Orientation;
//! the seam relation is queryable via BRepGraph_Tool::CoEdge::SeamPair.
struct CoEdgeDef : public BaseDef
{
  using TypeId = BRepGraph_CoEdgeId;

  BRepGraph_WireId  ParentWireId; //!< Ordered owner wire
  BRepGraph_EdgeId  ChildEdgeId;  //!< Connected reusable edge definition
  BRepGraph_FaceId  FaceId;       //!< Face this coedge belongs to (invalid for free wires)
  ParityOrientation Orientation = TopAbs_FORWARD; //!< Orientation relative to parent edge

  BRepGraph_CoEdgeCurve2DRepId      Curve2DRepId;      //!< Owned 2D curve use id
  BRepGraph_CoEdgePolygon2DRepId    Polygon2DRepId;    //!< Owned 2D polygon use id
  BRepGraph_CoEdgePolygonOnTriRepId PolygonOnTriRepId; //!< Owned polygon-on-triangulation use id
};

//! Wire entity: ordered coedge sequence.
//! Wire closure is derived from the ordered coedge chain via BRepGraph_CacheDerivedState.
struct WireDef : public BaseDef
{
  using TypeId = BRepGraph_WireId;
};

//! Face entity: surface, triangulations, wires.
struct FaceDef : public BaseDef
{
  using TypeId = BRepGraph_FaceId;

  BRepGraph_FaceSurfaceRepId SurfaceRepId; //!< Owned surface use id
  BRepGraph_FaceTriangulationRepId
    TriangulationRepId; //!< Owned triangulation use id (persistent/imported)

  double Tolerance = 0.0; //!< Face tolerance
};

//! Shell entity.
//! Shell closure is derived from face-boundary edge incidence via BRepGraph_CacheDerivedState.
struct ShellDef : public BaseDef
{
  using TypeId = BRepGraph_ShellId;
};

//! Solid entity.
struct SolidDef : public BaseDef
{
  using TypeId = BRepGraph_SolidId;
};

//! Compound entity.
struct CompoundDef : public BaseDef
{
  using TypeId = BRepGraph_CompoundId;
};

//! Comp-solid entity.
struct CompSolidDef : public BaseDef
{
  using TypeId = BRepGraph_CompSolidId;
};

//! Product entity: reusable shape definition (part or assembly).
//! Children are managed uniformly via ProductRelations::OccurrenceRefIds:
//! - A part product has one occurrence whose ChildNodeId is a topology root node.
//! - An assembly product has occurrences whose ChildNodeId values are other products.
//! Products carry no location or orientation - those live on references.
struct ProductDef : public BaseDef
{
  using TypeId = BRepGraph_ProductId;
};

//! Occurrence entity: reference to a child node (topology root or product).
//! Parent products are determined from ProductRelations owner arrays.
//! Placement lives on OccurrenceRef::LocalLocation (definitions never carry location).
//! Path-based traversal (BRepGraph_UsagePath) resolves DAG paths without stored
//! parent-occurrence pointers.
struct OccurrenceDef : public BaseDef
{
  using TypeId = BRepGraph_OccurrenceId;

  BRepGraph_NodeId ChildNodeId; //!< Referenced child node (topology root or product)
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Definition_HeaderFile
