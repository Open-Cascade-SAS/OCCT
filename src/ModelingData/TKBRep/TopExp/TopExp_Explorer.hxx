// Created on: 1993-01-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopExp_Explorer_HeaderFile
#define _TopExp_Explorer_HeaderFile

#include <NCollection_Vector.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//! An Explorer is a Tool to visit a Topological Data
//! Structure from the TopoDS package.
//!
//! An Explorer is built with:
//!
//! * The Shape to explore.
//!
//! * The type of Shapes to find: e.g VERTEX, EDGE.
//! This type cannot be SHAPE.
//!
//! * The type of Shapes to avoid. e.g SHELL, EDGE.
//! By default this type is SHAPE which means no
//! restriction on the exploration.
//!
//! The Explorer visits all the structure to find
//! shapes of the requested type which are not
//! contained in the type to avoid.
//!
//! Example to find all the Faces in the Shape S :
//!
//! TopExp_Explorer Ex;
//! for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
//! ProcessFace(Ex.Current());
//! }
//!
//! // an other way
//! TopExp_Explorer Ex(S,TopAbs_FACE);
//! while (Ex.More()) {
//! ProcessFace(Ex.Current());
//! Ex.Next();
//! }
//!
//! To find all the vertices which are not in an edge :
//!
//! for (Ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ...)
//!
//! To find all the faces in a SHELL, then all the
//! faces not in a SHELL :
//!
//! TopExp_Explorer Ex1, Ex2;
//!
//! for (Ex1.Init(S,TopAbs_SHELL),...) {
//! // visit all shells
//! for (Ex2.Init(Ex1.Current(),TopAbs_FACE),...) {
//! // visit all the faces of the current shell
//! }
//! }
//!
//! for (Ex1.Init(S,TopAbs_FACE,TopAbs_SHELL),...) {
//! // visit all faces not in a shell
//! }
//!
//! If the type to avoid is the same or is less
//! complex than the type to find it has no effect.
//!
//! For example searching edges not in a vertex does
//! not make a difference.
class TopExp_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty explorer, becomes useful after Init.
  Standard_EXPORT TopExp_Explorer() noexcept;

  //! Creates an Explorer on the Shape <S>.
  //!
  //! <ToFind> is the type of shapes to search.
  //! TopAbs_VERTEX, TopAbs_EDGE, ...
  //!
  //! <ToAvoid> is the type of shape to skip in the
  //! exploration. If <ToAvoid> is equal or less
  //! complex than <ToFind> or if <ToAVoid> is SHAPE it
  //! has no effect on the exploration.
  Standard_EXPORT TopExp_Explorer(const TopoDS_Shape&    S,
                                  const TopAbs_ShapeEnum ToFind,
                                  const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);

  //! Resets this explorer on the shape S. It is initialized to
  //! search the shape S, for shapes of type ToFind, that
  //! are not part of a shape ToAvoid.
  //! If the shape ToAvoid is equal to TopAbs_SHAPE, or
  //! if it is the same as, or less complex than, the shape
  //! ToFind it has no effect on the search.
  Standard_EXPORT void Init(const TopoDS_Shape&    S,
                            const TopAbs_ShapeEnum ToFind,
                            const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);

  //! Returns True if there are more shapes in the exploration.
  Standard_Boolean More() const noexcept { return hasMore; }

  //! Moves to the next Shape in the exploration.
  Standard_EXPORT void Next();

  //! Returns the current shape in the exploration.
  const TopoDS_Shape& Value() const noexcept { return Current(); }

  //! Returns the current shape in the exploration.
  Standard_EXPORT const TopoDS_Shape& Current() const noexcept;

  //! Reinitialize the exploration with the original arguments.
  void ReInit() { Init(myShape, toFind, toAvoid); }

  //! Return explored shape.
  const TopoDS_Shape& ExploredShape() const noexcept { return myShape; }

  //! Returns the current depth of the exploration. 0 is
  //! the shape to explore itself.
  Standard_EXPORT Standard_Integer Depth() const noexcept;

  //! Clears the content of the explorer.
  Standard_EXPORT void Clear();

  //! Destructor.
  Standard_EXPORT ~TopExp_Explorer();

private:
  NCollection_Vector<TopoDS_Iterator> myStack;
  TopoDS_Shape                        myShape;
  TopAbs_ShapeEnum                    toFind;
  TopAbs_ShapeEnum                    toAvoid;
  Standard_Boolean                    hasMore;
};

#endif // _TopExp_Explorer_HeaderFile
