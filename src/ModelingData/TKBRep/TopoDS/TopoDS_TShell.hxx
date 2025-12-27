// Created on: 1990-12-17
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_TShell_HeaderFile
#define _TopoDS_TShell_HeaderFile

#include <Standard.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TShell;
DEFINE_STANDARD_HANDLE(TopoDS_TShell, TopoDS_TShape)

//! A set of faces connected by their edges.
//!
//! A shell can have many faces (tetrahedron=4, box=6, complex=many).
//! Uses dynamic array storage with bucket size 8.
class TopoDS_TShell : public TopoDS_TShape
{
public:
  //! Bucket size for dynamic array (tetrahedron=4, box=6 needs 2 buckets)
  static constexpr int BucketSize = 6;

  //! Creates an empty TShell.
  TopoDS_TShell()
      : TopoDS_TShape(TopAbs_SHELL),
        mySubShapes(BucketSize)
  {
  }

  //! Returns an empty TShell.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return mySubShapes.Size(); }

  //! Returns the sub-shape at the given index (0-based).
  //! @param theIndex index of the sub-shape (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE { return mySubShapes.Value(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShell, TopoDS_TShape)

private:
  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  //! Storage for sub-shapes.
  NCollection_DynamicArray<TopoDS_Shape> mySubShapes;
};

#endif // _TopoDS_TShell_HeaderFile
