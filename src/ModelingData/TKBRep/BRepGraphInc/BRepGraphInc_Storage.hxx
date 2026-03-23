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

#ifndef _BRepGraphInc_Storage_HeaderFile
#define _BRepGraphInc_Storage_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraphInc_Entity.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

//! @brief Central storage for the incidence-table model.
//!
//! Holds all entity vectors, reverse indices, and TShape deduplication maps.
//! This struct is the physical backing for BRepGraphInc_Populate and
//! BRepGraphInc_Reconstruct.
struct BRepGraphInc_Storage
{
  DEFINE_STANDARD_ALLOC

  // ------ Entity tables (8 kinds) ------

  NCollection_Vector<BRepGraphInc::VertexEntity>    Vertices;
  NCollection_Vector<BRepGraphInc::EdgeEntity>      Edges;
  NCollection_Vector<BRepGraphInc::WireEntity>      Wires;
  NCollection_Vector<BRepGraphInc::FaceEntity>      Faces;
  NCollection_Vector<BRepGraphInc::ShellEntity>     Shells;
  NCollection_Vector<BRepGraphInc::SolidEntity>     Solids;
  NCollection_Vector<BRepGraphInc::CompoundEntity>  Compounds;
  NCollection_Vector<BRepGraphInc::CompSolidEntity> CompSolids;

  // ------ Reverse indices ------

  BRepGraphInc_ReverseIndex ReverseIdx;

  // ------ Unified TShape → NodeId map ------

  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> TShapeToNodeId;

  // ------ Original shapes from Build() input ------

  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> OriginalShapes;

  // ------ Population status ------

  bool IsDone = false;

  //! Clear all storage.
  Standard_EXPORT void Clear();

  //! Build reverse indices from entity and relationship tables.
  //! Call after population is complete.
  Standard_EXPORT void BuildReverseIndex();
};

#endif // _BRepGraphInc_Storage_HeaderFile
