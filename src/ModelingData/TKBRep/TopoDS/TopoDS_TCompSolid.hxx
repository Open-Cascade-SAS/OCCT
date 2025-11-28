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
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TCompSolid;
DEFINE_STANDARD_HANDLE(TopoDS_TCompSolid, TopoDS_TShape)

//! A set of solids connected by their faces.
//!
//! A compsolid contains multiple solids.
//! Uses dynamic array storage with bucket size 4.
class TopoDS_TCompSolid : public TopoDS_TShape
{
public:
  //! Bucket size for dynamic array
  static constexpr int BucketSize = 4;

  //! Creates an empty TCompSolid.
  TopoDS_TCompSolid()
      : TopoDS_TShape(TopAbs_COMPSOLID),
        mySolids(BucketSize)
  {
  }

  //! Returns an empty TCompSolid.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of solid sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the solid at the given index (0-based).
  //! @param theIndex index of the solid (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the solid at the given index for modification.
  //! @param theIndex index of the solid (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds a solid to this compsolid.
  //! @param theShape the solid to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the solid at the given index.
  //! @param theIndex index of the solid to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return mySolids.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return mySolids.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return mySolids.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { mySolids.Append(theShape); }

  void storageRemove(int theIndex) { mySolids.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TCompSolid, TopoDS_TShape)

private:
  //! Storage for solid sub-shapes using dynamic array.
  TopoDS_DynamicShapeStorage<BucketSize> mySolids;
};

#endif // _TopoDS_TCompSolid_HeaderFile
