// Created on: 1995-03-28
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepTools_Substitution_HeaderFile
#define _BRepTools_Substitution_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;

//! A tool to substitute subshapes by other shapes.
//!
//! The user use the method Substitute to define the
//! modifications.
//! A set of shapes is designated to replace a initial
//! shape.
//!
//! The method Build reconstructs a new Shape with the
//! modifications.The Shape and the new shape are
//! registered.
class BRepTools_Substitution
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepTools_Substitution();

  //! Reset all the fields.
  Standard_EXPORT void Clear();

  //! <Oldshape> will be replaced by <NewShapes>.
  //!
  //! <NewShapes> can be empty , in this case <OldShape>
  //! will disparate from its ancestors.
  //!
  //! if an item of <NewShapes> is oriented FORWARD.
  //! it will be oriented as <OldShape> in its ancestors.
  //! else it will be reversed.
  Standard_EXPORT void Substitute(const TopoDS_Shape&         OldShape,
                                  const TopTools_ListOfShape& NewShapes);

  //! Build NewShape from <S> if its subshapes has modified.
  //!
  //! The methods <IsCopied> and <Copy> allows you to keep
  //! the resul of <Build>
  Standard_EXPORT void Build(const TopoDS_Shape& S);

  //! Returns   True if <S> has   been  replaced .
  Standard_EXPORT Standard_Boolean IsCopied(const TopoDS_Shape& S) const;

  //! Returns the set of shapes substituted to <S>.
  Standard_EXPORT const TopTools_ListOfShape& Copy(const TopoDS_Shape& S) const;

protected:
private:
  TopTools_DataMapOfShapeListOfShape myMap;
};

#endif // _BRepTools_Substitution_HeaderFile
