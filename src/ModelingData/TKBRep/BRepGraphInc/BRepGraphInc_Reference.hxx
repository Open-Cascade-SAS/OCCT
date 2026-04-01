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

#ifndef _BRepGraphInc_Reference_HeaderFile
#define _BRepGraphInc_Reference_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Managed reference entry structs for the incidence-table storage.
//!
//! Each reference entry extends BaseRef with payload fields describing
//! how a child definition is used by its parent (orientation, location).
//! Reference entries are stored in flat per-kind vectors in BRepGraphInc_Storage
//! and support mutation tracking and soft-removal.
//! Not every definition kind has a dedicated Ref kind by design:
//! - Edge usage is represented by CoEdgeRef -> CoEdgeDef (which then targets EdgeDef)
//! - Compound children use ChildRef (heterogeneous NodeId target)
//! - Product children use OccurrenceRef (placement owned by OccurrenceDef)
//! - CompSolid children use SolidRef
//!
//! For lightweight read-only projections without lifecycle fields, see
//! BRepGraphInc_Usage.hxx (Usage structs carry only DefId + Orientation + Location).
namespace BRepGraphInc
{

//! Fields shared by every reference entry.
struct BaseRef
{
  BRepGraph_RefId  RefId;             //!< Typed address (kind + per-kind index)
  BRepGraph_NodeId ParentId;          //!< Parent topology node owning this reference usage
  uint32_t         OwnGen    = 0;     //!< Per-reference mutation counter
  bool             IsRemoved = false; //!< Soft-removal flag
};

//! Shell reference storage entry.
struct ShellRef : public BaseRef
{
  BRepGraph_ShellId  ShellDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Face reference storage entry.
struct FaceRef : public BaseRef
{
  BRepGraph_FaceId   FaceDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Wire reference storage entry.
struct WireRef : public BaseRef
{
  BRepGraph_WireId   WireDefId;
  bool               IsOuter     = false;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! CoEdge reference storage entry.
//! No Orientation field: CoEdgeDef::Sense already owns the edge-on-face sense,
//! coupled with PCurve parametrization, so duplicating orientation here would
//! create a second competing source of truth.
struct CoEdgeRef : public BaseRef
{
  BRepGraph_CoEdgeId CoEdgeDefId;
  TopLoc_Location    LocalLocation;
};

//! Vertex reference storage entry.
struct VertexRef : public BaseRef
{
  BRepGraph_VertexId VertexDefId;
  TopAbs_Orientation Orientation =
    TopAbs_INTERNAL; //!< INTERNAL: B-Rep vertex classification convention
  TopLoc_Location LocalLocation;
};

//! Solid reference storage entry.
struct SolidRef : public BaseRef
{
  BRepGraph_SolidId  SolidDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Child reference storage entry.
struct ChildRef : public BaseRef
{
  BRepGraph_NodeId   ChildDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Occurrence reference storage entry.
//! Unlike other ref types, OccurrenceRef omits Orientation and LocalLocation.
//! Placement is owned by OccurrenceDef::Placement; this ref is a lightweight
//! pointer from the parent Product's OccurrenceRefIds to the OccurrenceDef slot.
struct OccurrenceRef : public BaseRef
{
  BRepGraph_OccurrenceId OccurrenceDefId;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Reference_HeaderFile
