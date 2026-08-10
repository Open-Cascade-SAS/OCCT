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

#include <BRepGraphInc_ParityOrientation.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <TopLoc_Location.hxx>

//! @brief Managed reference entry structs for the incidence-table storage.
//!
//! Each reference entry extends BaseRef with representation fields describing
//! how a child definition is used by its parent.
//! Reference entries are stored in flat per-kind vectors in BRepGraphInc_Storage
//! and support mutation tracking and soft-removal.
//! Not every definition kind has a dedicated Ref kind by design:
//! - CoEdge usage is stored directly on CoEdgeDef and ordered through WireRelations
//! - Compound children use ChildRef (heterogeneous NodeId target)
//! - Product children use OccurrenceRef (placement owned by OccurrenceDef)
//! - CompSolid children use SolidRef
//!
//! For lightweight read-only projections without lifecycle fields, see
//! BRepGraphInc_Instance.hxx (Instance structs carry only DefId + Orientation + Location).
namespace BRepGraphInc
{

//! Fields shared by every reference entry.
struct BaseRef
{
  using TypeId = BRepGraph_RefId;

  //! Persistent per-kind UID counter value.
  //! 0 = invalid sentinel (not yet allocated). Valid UIDs start at 1.
  //! Kind is implicit from the concrete struct type (ShellRef, FaceRef, etc.).
  uint32_t UID = 0;
};

//! Shell reference storage entry.
struct ShellRef : public BaseRef
{
  using TypeId = BRepGraph_ShellRefId;

  BRepGraph_SolidId ParentSolidId;                //!< Parent solid identifier
  BRepGraph_ShellId ChildShellId;                 //!< Child shell identifier
  ParityOrientation Orientation = TopAbs_FORWARD; //!< Orientation within parent
};

//! Face reference storage entry.
struct FaceRef : public BaseRef
{
  using TypeId = BRepGraph_FaceRefId;

  BRepGraph_ShellId ParentShellId;                //!< Parent shell identifier
  BRepGraph_FaceId  ChildFaceId;                  //!< Child face identifier
  ParityOrientation Orientation = TopAbs_FORWARD; //!< Orientation within parent
};

//! Wire reference storage entry.
struct WireRef : public BaseRef
{
  using TypeId = BRepGraph_WireRefId;

  BRepGraph_FaceId  ParentFaceId;                 //!< Parent face identifier
  BRepGraph_WireId  ChildWireId;                  //!< Child wire identifier
  ParityOrientation Orientation = TopAbs_FORWARD; //!< Orientation within parent
};

//! Vertex reference storage entry.
struct VertexRef : public BaseRef
{
  using TypeId = BRepGraph_VertexRefId;

  BRepGraph_VertexId ChildVertexId;                //!< Child vertex identifier
  BRepGraph_EdgeId   ParentEdgeId;                 //!< Edge that owns this vertex reference
  ParityOrientation  Orientation = TopAbs_FORWARD; //!< Orientation within parent
};

//! Solid reference storage entry.
struct SolidRef : public BaseRef
{
  using TypeId = BRepGraph_SolidRefId;

  BRepGraph_CompSolidId ParentCompSolidId;            //!< Parent compsolid identifier
  BRepGraph_SolidId     ChildSolidId;                 //!< Child solid identifier
  ParityOrientation     Orientation = TopAbs_FORWARD; //!< Orientation within parent
};

//! Child reference storage entry.
struct ChildRef : public BaseRef
{
  using TypeId = BRepGraph_ChildRefId;

  BRepGraph_CompoundId ParentCompoundId;             //!< Parent compound identifier
  BRepGraph_NodeId     ChildNodeId;                  //!< Child node identifier (heterogeneous)
  ParityOrientation    Orientation = TopAbs_FORWARD; //!< Orientation within parent
  TopLoc_Location      LocalLocation;                //!< Location relative to parent
};

//! Occurrence reference storage entry.
//! Like ChildRef but without Orientation - placement is a reference property.
//! Structurally parallel to other ref types: definitions carry no location.
struct OccurrenceRef : public BaseRef
{
  using TypeId = BRepGraph_OccurrenceRefId;

  BRepGraph_ProductId    ParentProductId;
  BRepGraph_OccurrenceId ChildOccurrenceId;
  TopLoc_Location        LocalLocation; //!< Placement relative to parent product
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Reference_HeaderFile
