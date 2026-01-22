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

#ifndef _TopoDS_TCompSolid_HeaderFile
#define _TopoDS_TCompSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_Shape.hxx>

//! A set of solids connected by their faces.
//!
//! CompSolids typically have 2-8 solids,
//! so the default bucket size is 8.
class TopoDS_TCompSolid : public TopoDS_TShape
{
public:
  //! Default bucket size for composite solids.
  //! Typically 2-8 solids.
  static constexpr int DefaultBucketSize = 8;

  //! Creates an empty TCompSolid with default bucket size.
  TopoDS_TCompSolid()
      : TopoDS_TShape(TopAbs_COMPSOLID),
        mySubShapes(DefaultBucketSize)
  {
  }

  //! Creates an empty TCompSolid with specified bucket size.
  //! @param theBucketSize the bucket size for internal storage
  explicit TopoDS_TCompSolid(const size_t theBucketSize)
      : TopoDS_TShape(TopAbs_COMPSOLID),
        mySubShapes(theBucketSize > 0 ? static_cast<int>(theBucketSize) : DefaultBucketSize)
  {
  }

  //! Returns the number of direct sub-shapes (children).
  int NbChildren() const final { return mySubShapes.Size(); }

  //! Returns the child shape at the given index (0-based).
  //! @param theIndex the 0-based index of the child
  //! @return the child shape at the given index
  const TopoDS_Shape& GetChild(size_t theIndex) const final { return mySubShapes.Value(static_cast<int>(theIndex)); }

  //! Returns an empty TCompSolid.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TCompSolid, TopoDS_TShape)

private:
  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  NCollection_DynamicArray<TopoDS_Shape> mySubShapes; //!< Child shapes (solids)
};

#endif // _TopoDS_TCompSolid_HeaderFile
