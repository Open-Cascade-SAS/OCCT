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

#ifndef _TopoDS_TFace_HeaderFile
#define _TopoDS_TFace_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_Shape.hxx>

//! A topological part of a surface or of the 2D
//! space. The boundary is a set of wires and vertices.
class TopoDS_TFace : public TopoDS_TShape
{
public:
  //! Default bucket size for faces.
  static constexpr int DefaultBucketSize = 4;

  //! Creates an empty TFace with default bucket size.
  TopoDS_TFace()
      : TopoDS_TShape(TopAbs_FACE),
        mySubShapes(DefaultBucketSize)
  {
  }

  //! Creates an empty TFace with specified bucket size.
  //! @param theBucketSize the bucket size for internal storage
  explicit TopoDS_TFace(const size_t theBucketSize)
      : TopoDS_TShape(TopAbs_FACE),
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

  //! Returns an empty TFace.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TFace, TopoDS_TShape)

private:
  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  NCollection_DynamicArray<TopoDS_Shape> mySubShapes; //!< Child shapes (wires, vertices)
};

#endif // _TopoDS_TFace_HeaderFile
