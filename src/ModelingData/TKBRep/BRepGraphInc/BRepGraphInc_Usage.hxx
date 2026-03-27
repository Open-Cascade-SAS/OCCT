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

#ifndef _BRepGraphInc_Usage_HeaderFile
#define _BRepGraphInc_Usage_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! Lightweight usage structs for incidence-table topology.
//!
//! Each struct encodes how a definition is used at a particular site,
//! carrying the orientation and (where applicable) the local location
//! of the child in the parent context.
namespace BRepGraphInc
{

//! Reference to a vertex carrying Orientation and Location.
//! Used for boundary vertices (FORWARD/REVERSED) on edges and
//! direct INTERNAL/EXTERNAL vertex children on edges and faces.
struct VertexUsage
{
  BRepGraph_VertexId VertexDefId;
  TopAbs_Orientation Orientation = TopAbs_INTERNAL;
  TopLoc_Location    LocalLocation;
};

//! Reference from a wire to one of its coedges.
//!
//! Unlike other Ref types, CoEdgeUsage intentionally carries no Orientation field.
//! Orientation (Sense) lives on CoEdgeDef because it is definitional, not
//! referential: Sense is intrinsically bound to the CoEdge's PCurve, parametric
//! range, and UV endpoints.  Seam edges rely on two CoEdgeDefs with opposite
//! Sense sharing a SeamPairIdx, each owning its own PCurve - moving Sense here
//! would break that coupling.
struct CoEdgeUsage
{
  BRepGraph_CoEdgeId CoEdgeDefId;
  TopLoc_Location    LocalLocation;
};

//! Reference from a face to one of its wires.
struct WireUsage
{
  BRepGraph_WireId   WireDefId;
  bool               IsOuter     = false;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a shell to one of its faces.
struct FaceUsage
{
  BRepGraph_FaceId   FaceDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a solid to one of its shells.
struct ShellUsage
{
  BRepGraph_ShellId  ShellDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a comp-solid to one of its solids.
struct SolidUsage
{
  BRepGraph_SolidId  SolidDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a compound to a child of any kind.
struct ChildUsage
{
  BRepGraph_NodeId   ChildDefId; //!< Typed child entity id.
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a product to one of its child occurrences.
struct OccurrenceUsage
{
  BRepGraph_OccurrenceId OccurrenceDefId;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Usage_HeaderFile
