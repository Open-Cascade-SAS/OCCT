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

#include <BRepGraphInc_Entity.hxx>

//! Namespace grouping topology definition types for BRepGraph.
//!
//! All definition types are aliases to BRepGraphInc entity types.
//! The legacy Usage layer has been removed — per-occurrence data
//! (orientation, location) lives on incidence refs (FaceRef, ShellRef, etc.).
namespace BRepGraph_TopoNode
{

using BaseDef      = BRepGraphInc::BaseEntity;
using VertexDef    = BRepGraphInc::VertexEntity;
using EdgeDef      = BRepGraphInc::EdgeEntity;
using CoEdgeDef    = BRepGraphInc::CoEdgeEntity;
using WireDef      = BRepGraphInc::WireEntity;
using FaceDef      = BRepGraphInc::FaceEntity;
using ShellDef     = BRepGraphInc::ShellEntity;
using SolidDef     = BRepGraphInc::SolidEntity;
using CompoundDef   = BRepGraphInc::CompoundEntity;
using CompSolidDef  = BRepGraphInc::CompSolidEntity;
using ProductDef    = BRepGraphInc::ProductEntity;
using OccurrenceDef = BRepGraphInc::OccurrenceEntity;

} // namespace BRepGraph_TopoNode

#endif // _BRepGraph_TopoNode_HeaderFile
