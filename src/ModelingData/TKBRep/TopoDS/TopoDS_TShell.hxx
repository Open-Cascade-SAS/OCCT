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
#include <Standard_Type.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_Shape.hxx>

//! A set of faces connected by their edges.
//!
//! Shells typically have 4-12 faces (tetrahedron=4, box=6),
//! so the default bucket size is 6.
class TopoDS_TShell : public TopoDS_TShape
{
public:
  //! Default bucket size for shells.
  //! Tetrahedron=4, box=6, typical shells 4-12 faces.
  static constexpr int DefaultBucketSize = 6;

  //! Creates an empty TShell with default bucket size.
  TopoDS_TShell()
      : TopoDS_TShape(TopAbs_SHELL),
        mySubShapes(DefaultBucketSize)
  {
  }

  //! Creates an empty TShell with specified bucket size.
  //! @param theBucketSize the bucket size for internal storage
  explicit TopoDS_TShell(const size_t theBucketSize)
      : TopoDS_TShape(TopAbs_SHELL),
        mySubShapes(theBucketSize > 0 ? static_cast<int>(theBucketSize) : DefaultBucketSize)
  {
  }

  //! Returns the number of direct sub-shapes (children).
  int NbChildren() const final { return mySubShapes.Size(); }

  //! Returns the child shape at the given index (0-based).
  //! @param theIndex the 0-based index of the child
  //! @return the child shape at the given index
  const TopoDS_Shape& GetChild(size_t theIndex) const final
  {
    return mySubShapes.Value(static_cast<int>(theIndex));
  }

  //! Returns an empty TShell.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShell, TopoDS_TShape)

private:
  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  NCollection_DynamicArray<TopoDS_Shape> mySubShapes; //!< Child shapes (faces)
};

#endif // _TopoDS_TShell_HeaderFile
