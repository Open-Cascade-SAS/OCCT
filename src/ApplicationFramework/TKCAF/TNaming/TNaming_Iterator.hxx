// Created on: 1996-12-16
// Created by: Remi Lequette
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

#ifndef _TNaming_Iterator_HeaderFile
#define _TNaming_Iterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TNaming_PtrNode.hxx>
#include <Standard_Integer.hxx>
#include <TNaming_Evolution.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class TNaming_NewShapeIterator;
class TNaming_OldShapeIterator;
class TNaming_NamedShape;
class TDF_Label;
class TopoDS_Shape;

//! A tool to visit the contents of a named shape attribute.
//! Pairs of shapes in the attribute are iterated, one
//! being the pre-modification or the old shape, and
//! the other the post-modification or the new shape.
//! This allows you to have a full access to all
//! contents of an attribute. If, on the other hand, you
//! are only interested in topological entities stored
//! in the attribute, you can use the functions
//! GetShape and CurrentShape in TNaming_Tool.
class TNaming_Iterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Iterates on all  the history records in
  //! <anAtt>.
  Standard_EXPORT TNaming_Iterator(const Handle(TNaming_NamedShape)& anAtt);

  //! Iterates on all  the history records in
  //! the current transaction
  Standard_EXPORT TNaming_Iterator(const TDF_Label& aLabel);

  //! Iterates on all  the history records in
  //! the transaction <aTrans>
  Standard_EXPORT TNaming_Iterator(const TDF_Label& aLabel, const Standard_Integer aTrans);

  //! Returns True if there is a current Item in
  //! the iteration.
  Standard_Boolean More() const;

  //! Moves the iteration to the next Item
  Standard_EXPORT void Next();

  //! Returns the old shape in this iterator object.
  //! This shape can be a null one.
  Standard_EXPORT const TopoDS_Shape& OldShape() const;

  //! Returns the new shape in this iterator object.
  Standard_EXPORT const TopoDS_Shape& NewShape() const;

  //! Returns true if the  new  shape is a modification  (split,
  //! fuse,etc...) of the old shape.
  Standard_EXPORT Standard_Boolean IsModification() const;

  Standard_EXPORT TNaming_Evolution Evolution() const;

  friend class TNaming_NewShapeIterator;
  friend class TNaming_OldShapeIterator;

protected:
private:
  TNaming_PtrNode  myNode;
  Standard_Integer myTrans;
};

#include <TNaming_Iterator.lxx>

#endif // _TNaming_Iterator_HeaderFile
