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

#ifndef _TopoDS_TSolid_HeaderFile
#define _TopoDS_TSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TSolid;
DEFINE_STANDARD_HANDLE(TopoDS_TSolid, TopoDS_TShape)

//! A Topological part of 3D space, bounded by shells,
//! edges and vertices.
//!
//! A solid typically has 1 shell (outer boundary).
//! Uses local storage for 1 shell (common case), with dynamic
//! overflow array (bucket size 4) for solids with voids.
class TopoDS_TSolid : public TopoDS_TShape
{
public:
  //! Local storage capacity for shells (optimized for simple solids)
  static constexpr size_t LocalCapacity = 2;

  //! Bucket size for dynamic array overflow
  static constexpr int BucketSize = 8;

  //! Creates an empty TSolid.
  TopoDS_TSolid()
      : TopoDS_TShape(TopAbs_SOLID),
        myShells()
  {
    Orientable(false);
  }

  //! Returns an empty TSolid.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of shell/edge/vertex sub-shapes.
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

  //! Adds a shell, edge, or vertex to this solid.
  //! @param theShape the sub-shape to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the sub-shape at the given index.
  //! @param theIndex index of the sub-shape to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return myShells.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return myShells.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return myShells.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { myShells.Append(theShape); }

  void storageRemove(int theIndex) { myShells.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TSolid, TopoDS_TShape)

private:
  //! Storage for shell/edge/vertex sub-shapes.
  //! Uses local storage for 1 shell (common case),
  //! switches to dynamic array for overflow.
  TopoDS_VariantShapeStorage<LocalCapacity, BucketSize> myShells;
};

#endif // _TopoDS_TSolid_HeaderFile
