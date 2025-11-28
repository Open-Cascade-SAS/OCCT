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

#ifndef _TopoDS_TCompound_HeaderFile
#define _TopoDS_TCompound_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TCompound;
DEFINE_STANDARD_HANDLE(TopoDS_TCompound, TopoDS_TShape)

//! A TCompound is an all-purpose set of Shapes.
//!
//! A compound can contain any type of shape.
//! Uses dynamic array storage with bucket size 8.
class TopoDS_TCompound : public TopoDS_TShape
{
public:
  //! Bucket size for dynamic array
  static constexpr int BucketSize = 8;

  //! Creates an empty TCompound.
  TopoDS_TCompound()
      : TopoDS_TShape(TopAbs_COMPOUND),
        myShapes(BucketSize)
  {
    Orientable(false);
  }

  //! Returns an empty TCompound.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the sub-shape at the given index (0-based).
  //! @param theIndex index of the sub-shape (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the sub-shape at the given index for modification.
  //! @param theIndex index of the sub-shape (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds a shape to this compound.
  //! @param theShape the shape to add (can be any type)
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the sub-shape at the given index.
  //! @param theIndex index of the sub-shape to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int                 storageSize() const { return myShapes.Size(); }
  const TopoDS_Shape& storageValue(int theIndex) const { return myShapes.Value(theIndex); }
  TopoDS_Shape&       storageChangeValue(int theIndex) { return myShapes.ChangeValue(theIndex); }
  void                storageAppend(const TopoDS_Shape& theShape) { myShapes.Append(theShape); }
  void                storageRemove(int theIndex) { myShapes.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TCompound, TopoDS_TShape)

private:
  //! Storage for all sub-shapes using dynamic array.
  TopoDS_DynamicShapeStorage<BucketSize> myShapes;
};

#endif // _TopoDS_TCompound_HeaderFile
