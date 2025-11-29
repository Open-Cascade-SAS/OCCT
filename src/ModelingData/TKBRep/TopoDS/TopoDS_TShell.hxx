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
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TShell;
DEFINE_STANDARD_HANDLE(TopoDS_TShell, TopoDS_TShape)

//! A set of faces connected by their edges.
//!
//! A shell can have many faces (tetrahedron=4, box=6, complex=many).
//! Uses local storage for up to 4 faces, with dynamic overflow (bucket size 8).
class TopoDS_TShell : public TopoDS_TShape
{
public:
  //! Local storage capacity for faces (covers tetrahedron=4, prism base=5).
  //! Box with 6 faces uses 4 local + 2 in first bucket (minor overhead).
  //! Memory: 4×24=96 bytes vs 6×24=144 bytes saves 48 bytes per shell.
  static constexpr size_t LocalCapacity = 4;

  //! Bucket size for dynamic array overflow (shells grow moderately)
  static constexpr int BucketSize = 8;

  //! Creates an empty TShell.
  TopoDS_TShell()
      : TopoDS_TShape(TopAbs_SHELL),
        myFaces()
  {
  }

  //! Returns an empty TShell.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of face sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the face at the given index (0-based).
  //! @param theIndex index of the face (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the face at the given index for modification.
  //! @param theIndex index of the face (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds a face to this shell.
  //! @param theShape the face to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the face at the given index.
  //! @param theIndex index of the face to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return myFaces.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return myFaces.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return myFaces.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { myFaces.Append(theShape); }

  void storageRemove(int theIndex) { myFaces.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShell, TopoDS_TShape)

private:
  //! Storage for face sub-shapes.
  //! Uses local storage for up to 4 faces (tetrahedron, prism base),
  //! switches to dynamic array for overflow.
  TopoDS_VariantShapeStorage<LocalCapacity, BucketSize> myFaces;
};

#endif // _TopoDS_TShell_HeaderFile
