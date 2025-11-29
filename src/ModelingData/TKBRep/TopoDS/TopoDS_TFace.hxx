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
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TFace;
DEFINE_STANDARD_HANDLE(TopoDS_TFace, TopoDS_TShape)

//! A topological part of a surface or of the 2D
//! space. The boundary is a set of wires and vertices.
//!
//! A face typically has 1-4 wires (outer boundary + holes).
//! Uses local storage for up to 2 wires, with dynamic
//! overflow array (bucket size 4) for faces with many holes.
class TopoDS_TFace : public TopoDS_TShape
{
public:
  //! Local storage capacity for wires (optimized for simple faces)
  static constexpr size_t LocalCapacity = 2;

  //! Bucket size for dynamic array overflow
  static constexpr int BucketSize = 8;

  //! Creates an empty TFace.
  TopoDS_TFace()
      : TopoDS_TShape(TopAbs_FACE),
        myWires()
  {
  }

  //! Returns an empty TFace.
  Standard_EXPORT virtual Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of wire/vertex sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the wire/vertex at the given index (0-based).
  //! @param theIndex index of the sub-shape (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the wire/vertex at the given index for modification.
  //! @param theIndex index of the sub-shape (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds a wire or vertex to this face.
  //! @param theShape the wire or vertex to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the sub-shape at the given index.
  //! @param theIndex index of the sub-shape to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return myWires.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return myWires.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return myWires.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { myWires.Append(theShape); }

  void storageRemove(int theIndex) { myWires.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TFace, TopoDS_TShape)

private:
  //! Storage for wire/vertex sub-shapes.
  //! Uses local storage for up to 2 wires (common case),
  //! switches to dynamic array for overflow.
  TopoDS_VariantShapeStorage<LocalCapacity, BucketSize> myWires;
};

#endif // _TopoDS_TFace_HeaderFile
