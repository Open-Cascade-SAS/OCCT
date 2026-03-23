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

#ifndef _BRepGraphInc_IncidenceRef_HeaderFile
#define _BRepGraphInc_IncidenceRef_HeaderFile

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! Lightweight reference structs for incidence-table topology.
//!
//! Each struct encodes a forward-direction containment relationship
//! between a parent entity and a child entity, carrying the orientation
//! and (where applicable) the local location of the child in the parent context.
namespace BRepGraphInc
{

//! Reference from an edge or face to a vertex with INTERNAL/EXTERNAL orientation.
struct VertexRef
{
  int                VertexIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_INTERNAL;
};

//! Reference from a wire to one of its edges.
struct EdgeRef
{
  int                EdgeIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
};

//! Reference from a face to one of its wires.
struct WireRef
{
  int                WireIdx = -1;
  bool               IsOuter = false;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
};

//! Reference from a shell to one of its faces.
struct FaceRef
{
  int                FaceIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a solid to one of its shells.
struct ShellRef
{
  int                ShellIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a comp-solid to one of its solids.
struct SolidRef
{
  int                SolidIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

//! Reference from a compound to a child of any kind.
struct ChildRef
{
  int                Kind = -1;     //!< BRepGraph_NodeId::Kind as int
  int                ChildIdx = -1;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_IncidenceRef_HeaderFile
