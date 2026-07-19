// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeExtend_Explorer_HeaderFile
#define _ShapeExtend_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;

//! This class is intended to
//! explore shapes and convert different representations
//! (list, sequence, compound) of complex shapes. It provides tools for:
//! - obtaining type of the shapes in context of TopoDS_Compound,
//! - exploring shapes in context of TopoDS_Compound,
//! - converting different representations of shapes (list, sequence, compound).
class ShapeExtend_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an object Explorer
  Standard_EXPORT ShapeExtend_Explorer();

  //! Converts a sequence of Shapes to a Compound
  Standard_EXPORT TopoDS_Shape
    CompoundFromSeq(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqval) const;

  //! Converts a Compound to a list of Shapes
  //! if <comp> is not a compound, the list contains only <comp>
  //! if <comp> is Null, the list is empty
  //! if <comp> is a Compound, its sub-shapes are put into the list
  //! then if <expcomp> is True, if a sub-shape is a Compound, it
  //! is not put to the list but its sub-shapes are (recursive)
  Standard_EXPORT occ::handle<NCollection_HSequence<TopoDS_Shape>> SeqFromCompound(
    const TopoDS_Shape& comp,
    const bool          expcomp) const;

  //! Converts a Sequence of Shapes to a List of Shapes
  //! <clear> if True (D), commands the list to start from scratch
  //! else, the list is cumulated
  Standard_EXPORT void ListFromSeq(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqval,
                                   NCollection_List<TopoDS_Shape>&                         lisval,
                                   const bool clear = true) const;

  //! Converts a List of Shapes to a Sequence of Shapes
  Standard_EXPORT occ::handle<NCollection_HSequence<TopoDS_Shape>> SeqFromList(
    const NCollection_List<TopoDS_Shape>& lisval) const;

  //! Returns the type of a Shape: true type if <compound> is False
  //! If <compound> is True and <shape> is a Compound, iterates on
  //! its items. If all are of the same type, returns this type.
  //! Else, returns COMPOUND. If it is empty, returns SHAPE
  //! For a Null Shape, returns SHAPE
  Standard_EXPORT TopAbs_ShapeEnum ShapeType(const TopoDS_Shape& shape, const bool compound) const;

  //! Builds a COMPOUND from the given shape.
  //! It explores the shape level by level, according to the
  //! <explore> argument. If <explore> is False, only COMPOUND
  //! items are explored, else all items are.
  //! The following shapes are added to resulting compound:
  //! - shapes which comply to <type>
  //! - if <type> is WIRE, considers also free edges (and makes wires)
  //! - if <type> is SHELL, considers also free faces (and makes shells)
  //! If <compound> is True, gathers items in compounds which
  //! correspond to starting COMPOUND,SOLID or SHELL containers, or
  //! items directly contained in a Compound
  Standard_EXPORT TopoDS_Shape SortedCompound(const TopoDS_Shape&    shape,
                                              const TopAbs_ShapeEnum type,
                                              const bool             explore,
                                              const bool             compound) const;

  //! Dispatches starting list of shapes according to their type,
  //! to the appropriate resulting lists
  //! For each of these lists, if it is null, it is firstly created
  //! else, new items are appended to the already existing ones
  Standard_EXPORT void DispatchList(
    const occ::handle<NCollection_HSequence<TopoDS_Shape>>& list,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       vertices,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       edges,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       wires,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       faces,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       shells,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       solids,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       compsols,
    occ::handle<NCollection_HSequence<TopoDS_Shape>>&       compounds) const;
};

#endif // _ShapeExtend_Explorer_HeaderFile
