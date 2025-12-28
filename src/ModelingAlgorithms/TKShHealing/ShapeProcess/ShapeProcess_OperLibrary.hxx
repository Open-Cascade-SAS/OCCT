// Created on: 2000-08-31
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeProcess_OperLibrary_HeaderFile
#define _ShapeProcess_OperLibrary_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopoDS_Shape;
class ShapeProcess_ShapeContext;
class BRepTools_Modification;
class ShapeExtend_MsgRegistrator;

//! Provides a set of following operators
//!
//! DirectFaces
//! FixShape
//! SameParameter
//! SetTolerance
//! SplitAngle
//! BSplineRestriction
//! ElementaryToRevolution
//! SurfaceToBSpline
//! ToBezier
//! SplitContinuity
//! SplitClosedFaces
//! FixWireGaps
//! FixFaceSize
//! DropSmallEdges
//! FixShape
//! SplitClosedEdges
class ShapeProcess_OperLibrary
{
public:
  DEFINE_STANDARD_ALLOC

  //! Registers all the operators
  Standard_EXPORT static void Init();

  //! Applies BRepTools_Modification to a shape,
  //! taking into account sharing of components of compounds.
  //! if theMutableInput vat is set to true then input shape S
  //! can be modified during the modification process.
  Standard_EXPORT static TopoDS_Shape ApplyModifier(
    const TopoDS_Shape&                                                       S,
    const occ::handle<ShapeProcess_ShapeContext>&                             context,
    const occ::handle<BRepTools_Modification>&                                M,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& map,
    const occ::handle<ShapeExtend_MsgRegistrator>&                            msg = 0,
    bool theMutableInput                                                          = false);
};

#endif // _ShapeProcess_OperLibrary_HeaderFile
