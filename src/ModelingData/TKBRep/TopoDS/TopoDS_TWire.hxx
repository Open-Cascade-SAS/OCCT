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

#ifndef _TopoDS_TWire_HeaderFile
#define _TopoDS_TWire_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TWire;
DEFINE_STANDARD_HANDLE(TopoDS_TWire, TopoDS_TShape)

//! A set of edges connected by their vertices.
//!
//! A wire can have many edges (commonly 3-100+).
//! Uses local storage for up to 2 edges, with dynamic overflow (bucket size 8).
class TopoDS_TWire : public TopoDS_TShape
{
public:
  //! Local storage capacity for edges (covers simple polygons like rectangles)
  static constexpr size_t LocalCapacity = 4;

  //! Bucket size for dynamic array overflow (wires often have many edges)
  static constexpr int BucketSize = 16;

  //! Creates an empty TWire.
  TopoDS_TWire()
      : TopoDS_TShape(TopAbs_WIRE),
        myEdges()
  {
  }

  //! Returns an empty TWire.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Returns the number of edge sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the edge at the given index (0-based).
  //! @param theIndex index of the edge (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the edge at the given index for modification.
  //! @param theIndex index of the edge (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds an edge to this wire.
  //! @param theShape the edge to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the edge at the given index.
  //! @param theIndex index of the edge to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return myEdges.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return myEdges.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return myEdges.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { myEdges.Append(theShape); }

  void storageRemove(int theIndex) { myEdges.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TWire, TopoDS_TShape)

private:
  //! Storage for edge sub-shapes.
  //! Uses local storage for up to 2 edges (common case),
  //! switches to dynamic array for overflow.
  TopoDS_VariantShapeStorage<LocalCapacity, BucketSize> myEdges;
};

#endif // _TopoDS_TWire_HeaderFile
