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

#ifndef _TopoDS_TEdge_HeaderFile
#define _TopoDS_TEdge_HeaderFile

#include <Standard.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ShapeStorage.hxx>
#include <TopoDS_TShape.hxx>

class TopoDS_TEdge;
DEFINE_STANDARD_HANDLE(TopoDS_TEdge, TopoDS_TShape)

//! A topological part of a curve in 2D or 3D, the
//! boundary is a set of oriented Vertices.
//!
//! An edge typically has 2 vertices (start and end).
//! Uses local storage for up to 2 vertices, with dynamic
//! overflow array (bucket size 4) for degenerate cases.
class TopoDS_TEdge : public TopoDS_TShape
{
public:
  //! Local storage capacity for vertices (optimized for common case of 2 vertices)
  static constexpr size_t LocalCapacity = 2;

  //! Bucket size for dynamic array overflow
  static constexpr int BucketSize = 8;

  //! Returns the number of vertex sub-shapes.
  int NbChildren() const Standard_OVERRIDE { return storageSize(); }

  //! Returns the vertex at the given index (0-based).
  //! @param theIndex index of the vertex (0 <= theIndex < NbChildren())
  const TopoDS_Shape& GetChild(int theIndex) const Standard_OVERRIDE
  {
    return storageValue(theIndex);
  }

  //! Returns the vertex at the given index for modification.
  //! @param theIndex index of the vertex (0 <= theIndex < NbChildren())
  TopoDS_Shape& ChangeChild(int theIndex) Standard_OVERRIDE { return storageChangeValue(theIndex); }

  //! Adds a vertex to this edge.
  //! @param theShape the vertex to add
  void AddChild(const TopoDS_Shape& theShape) Standard_OVERRIDE { storageAppend(theShape); }

  //! Removes the vertex at the given index.
  //! @param theIndex index of the vertex to remove (0 <= theIndex < NbChildren())
  void RemoveChild(int theIndex) Standard_OVERRIDE { storageRemove(theIndex); }

  // Non-virtual direct storage access for performance-critical code (Iterator, Builder)
  int storageSize() const { return myVertices.Size(); }

  const TopoDS_Shape& storageValue(int theIndex) const { return myVertices.Value(theIndex); }

  TopoDS_Shape& storageChangeValue(int theIndex) { return myVertices.ChangeValue(theIndex); }

  void storageAppend(const TopoDS_Shape& theShape) { myVertices.Append(theShape); }

  void storageRemove(int theIndex) { myVertices.Remove(theIndex); }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TEdge, TopoDS_TShape)

protected:
  //! Construct an edge with empty vertex storage.
  TopoDS_TEdge()
      : TopoDS_TShape(TopAbs_EDGE),
        myVertices()
  {
  }

private:
  //! Storage for vertex sub-shapes.
  //! Uses local storage for up to 2 vertices (common case),
  //! switches to dynamic array for overflow.
  TopoDS_VariantShapeStorage<LocalCapacity, BucketSize> myVertices;
};

#endif // _TopoDS_TEdge_HeaderFile
