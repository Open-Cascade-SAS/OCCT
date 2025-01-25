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

#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;

//! This class is intended to
//! explore shapes and convert different representations
//! (list, sequence, compound) of complex shapes. It provides tools for:
//! - obtaining type of the shapes in context of TopoDS_Compound,
//! - exploring shapes in context of  TopoDS_Compound,
//! - converting different representations of shapes (list, sequence, compound).
class ShapeExtend_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an object Explorer
  Standard_EXPORT ShapeExtend_Explorer();

  //! Converts a sequence of Shapes to a Compound
  Standard_EXPORT TopoDS_Shape
    CompoundFromSeq(const Handle(TopTools_HSequenceOfShape)& seqval) const;

  //! Converts a Compound to a list of Shapes
  //! if <comp> is not a compound, the list contains only <comp>
  //! if <comp> is Null, the list is empty
  //! if <comp> is a Compound, its sub-shapes are put into the list
  //! then if <expcomp> is True, if a sub-shape is a Compound, it
  //! is not put to the list but its sub-shapes are (recursive)
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) SeqFromCompound(
    const TopoDS_Shape&    comp,
    const Standard_Boolean expcomp) const;

  //! Converts a Sequence of Shapes to a List of Shapes
  //! <clear> if True (D), commands the list to start from scratch
  //! else, the list is cumulated
  Standard_EXPORT void ListFromSeq(const Handle(TopTools_HSequenceOfShape)& seqval,
                                   TopTools_ListOfShape&                    lisval,
                                   const Standard_Boolean clear = Standard_True) const;

  //! Converts a List of Shapes to a Sequence of Shapes
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) SeqFromList(
    const TopTools_ListOfShape& lisval) const;

  //! Returns the type of a Shape: true type if <compound> is False
  //! If <compound> is True and <shape> is a Compound, iterates on
  //! its items. If all are of the same type, returns this type.
  //! Else, returns COMPOUND. If it is empty, returns SHAPE
  //! For a Null Shape, returns SHAPE
  Standard_EXPORT TopAbs_ShapeEnum ShapeType(const TopoDS_Shape&    shape,
                                             const Standard_Boolean compound) const;

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
                                              const Standard_Boolean explore,
                                              const Standard_Boolean compound) const;

  //! Dispatches starting list of shapes according to their type,
  //! to the appropriate resulting lists
  //! For each of these lists, if it is null, it is firstly created
  //! else, new items are appended to the already existing ones
  Standard_EXPORT void DispatchList(const Handle(TopTools_HSequenceOfShape)& list,
                                    Handle(TopTools_HSequenceOfShape)&       vertices,
                                    Handle(TopTools_HSequenceOfShape)&       edges,
                                    Handle(TopTools_HSequenceOfShape)&       wires,
                                    Handle(TopTools_HSequenceOfShape)&       faces,
                                    Handle(TopTools_HSequenceOfShape)&       shells,
                                    Handle(TopTools_HSequenceOfShape)&       solids,
                                    Handle(TopTools_HSequenceOfShape)&       compsols,
                                    Handle(TopTools_HSequenceOfShape)&       compounds) const;

protected:
private:
};

#endif // _ShapeExtend_Explorer_HeaderFile
