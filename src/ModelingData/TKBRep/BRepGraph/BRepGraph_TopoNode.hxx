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
#include <BRepGraphInc_Entity.hxx>

#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>

#include <NCollection_Vector.hxx>

//! Namespace grouping all topology node types for BRepGraph.
//!
//! Two-layer architecture: Definitions (intrinsic data per unique TShape)
//! and Usages (context-specific data per occurrence in the containment hierarchy).
//!
//! Definition types are aliases to BRepGraphInc entity types (Phase A migration).
//! Usage types remain as separate structs (will be removed in Phase D).
namespace BRepGraph_TopoNode
{

// =========================================================================
// Layer 1: Definitions -- aliases to incidence entity types
// =========================================================================

using BaseDef      = BRepGraphInc::BaseEntity;
using VertexDef    = BRepGraphInc::VertexEntity;
using EdgeDef      = BRepGraphInc::EdgeEntity;
using WireDef      = BRepGraphInc::WireEntity;
using FaceDef      = BRepGraphInc::FaceEntity;
using ShellDef     = BRepGraphInc::ShellEntity;
using SolidDef     = BRepGraphInc::SolidEntity;
using CompoundDef  = BRepGraphInc::CompoundEntity;
using CompSolidDef = BRepGraphInc::CompSolidEntity;

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
  BRepGraph_UsageId OwnerFaceUsage;  //!< FaceUsage that contains this wire occurrence
  NCollection_Vector<BRepGraph_UsageId> EdgeUsages;  //!< Ordered edge usages in topological winding order

  //! True if first edge's start vertex == last edge's end vertex.
  bool IsClosed = false;
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
