// Created on: 1996-02-13
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepOffsetAPI_MakeThickSolid_HeaderFile
#define _BRepOffsetAPI_MakeThickSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffset_Mode.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_JoinType.hxx>
class TopoDS_Shape;

//! Describes functions to build hollowed solids.
//! A hollowed solid is built from an initial solid and a set of
//! faces on this solid, which are to be removed. The
//! remaining faces of the solid become the walls of the
//! hollowed solid, their thickness defined at the time of construction.
//! the solid is built from an initial
//! solid  <S> and a  set of  faces {Fi} from  <S>,
//! builds a   solid  composed  by two shells closed  by
//! the {Fi}. First shell <SS>   is composed by all
//! the faces of <S> expected {Fi}.  Second shell is
//! the offset shell of <SS>.
//! A MakeThickSolid object provides a framework for:
//! - defining the cross-section of a hollowed solid,
//! - implementing the construction algorithm, and
//! - consulting the result.
class BRepOffsetAPI_MakeThickSolid : public BRepOffsetAPI_MakeOffsetShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor does nothing.
  Standard_EXPORT BRepOffsetAPI_MakeThickSolid();

  //! Constructs solid using simple algorithm.
  //! According to its nature it is not possible to set list of the closing faces.
  //! This algorithm does not support faces removing. It is caused by fact that
  //! intersections are not computed during offset creation.
  //! Non-closed shell or face is expected as input.
  Standard_EXPORT void MakeThickSolidBySimple(const TopoDS_Shape& theS,
                                              const Standard_Real theOffsetValue);

  //! Constructs a hollowed solid from
  //! the solid S by removing the set of faces ClosingFaces from S, where:
  //! Offset defines the thickness of the walls. Its sign indicates
  //! which side of the surface of the solid the hollowed shape is built on;
  //! - Tol defines the tolerance criterion for coincidence in generated shapes;
  //! - Mode defines the construction type of parallels applied to free
  //! edges of shape S. Currently, only one construction type is
  //! implemented, namely the one where the free edges do not generate
  //! parallels; this corresponds to the default value BRepOffset_Skin;
  //! Intersection specifies how the algorithm must work in order to
  //! limit the parallels to two adjacent shapes:
  //! - if Intersection is false (default value), the intersection
  //! is calculated with the parallels to the two adjacent shapes,
  //! -     if Intersection is true, the intersection is calculated by
  //! taking account of all parallels generated; this computation
  //! method is more general as it avoids self-intersections
  //! generated in the offset shape from features of small dimensions
  //! on shape S, however this method has not been completely
  //! implemented and therefore is not recommended for use;
  //! -     SelfInter tells the algorithm whether a computation to
  //! eliminate self-intersections needs to be applied to the
  //! resulting shape. However, as this functionality is not yet
  //! implemented, you should use the default value (false);
  //! - Join defines how to fill the holes that may appear between
  //! parallels to the two adjacent faces. It may take values
  //! GeomAbs_Arc or GeomAbs_Intersection:
  //! - if Join is equal to GeomAbs_Arc, then pipes are generated
  //! between two free edges of two adjacent parallels,
  //! and spheres are generated on "images" of vertices;
  //! it is the default value,
  //! - if Join is equal to GeomAbs_Intersection,
  //! then the parallels to the two adjacent faces are
  //! enlarged and intersected, so that there are no free
  //! edges on parallels to faces.
  //! RemoveIntEdges flag defines whether to remove the INTERNAL edges
  //! from the result or not.
  //! Warnings
  //! Since the algorithm of MakeThickSolid is based on
  //! MakeOffsetShape algorithm, the warnings are the same as for
  //! MakeOffsetShape.
  Standard_EXPORT void MakeThickSolidByJoin(
    const TopoDS_Shape&          S,
    const TopTools_ListOfShape&  ClosingFaces,
    const Standard_Real          Offset,
    const Standard_Real          Tol,
    const BRepOffset_Mode        Mode           = BRepOffset_Skin,
    const Standard_Boolean       Intersection   = Standard_False,
    const Standard_Boolean       SelfInter      = Standard_False,
    const GeomAbs_JoinType       Join           = GeomAbs_Arc,
    const Standard_Boolean       RemoveIntEdges = Standard_False,
    const Message_ProgressRange& theRange       = Message_ProgressRange());

  // Does nothing.
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified(const TopoDS_Shape& S)
    Standard_OVERRIDE;
};

#endif // _BRepOffsetAPI_MakeThickSolid_HeaderFile
