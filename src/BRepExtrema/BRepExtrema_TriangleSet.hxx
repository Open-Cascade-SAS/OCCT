// Created on: 2014-10-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepExtrema_TriangleSet_HeaderFile
#define _BRepExtrema_TriangleSet_HeaderFile

#include <TopoDS_Face.hxx>
#include <BVH_PrimitiveSet.hxx>

//! List of shapes and their IDs for collision detection.
typedef NCollection_Vector<TopoDS_Face> BRepExtrema_ShapeList;

//! Triangle set corresponding to specific face.
class BRepExtrema_TriangleSet : public BVH_PrimitiveSet<Standard_Real, 3>, public Standard_Transient
{
public:

  //! Creates empty triangle set.
  Standard_EXPORT BRepExtrema_TriangleSet();

  //! Creates triangle set from the given face.
  Standard_EXPORT BRepExtrema_TriangleSet (const BRepExtrema_ShapeList& theFaces);

  //! Releases resources of triangle set.
  Standard_EXPORT ~BRepExtrema_TriangleSet();

public: //! @name methods implementing BVH set interface

  //! Returns total number of triangles.
  Standard_Integer Size() const;

  //! Returns AABB of the given triangle.
  BVH_Box<Standard_Real, 3> Box (const Standard_Integer theIndex) const;

  //! Returns centroid position along specified axis.
  Standard_Real Center (const Standard_Integer theIndex, const Standard_Integer theAxis) const;

  //! Swaps indices of two specified triangles.
  void Swap (const Standard_Integer theIndex1, const Standard_Integer theIndex2);

public:

  //! Clears triangle set data.
  Standard_EXPORT void Clear();

  //! Initializes triangle set.
  Standard_EXPORT Standard_Boolean Init (const BRepExtrema_ShapeList& theFaces);

  //! Returns vertices of the given triangle.
  Standard_EXPORT void GetVertices (const Standard_Integer theIndex,
                                    BVH_Vec3d&             theVertex1,
                                    BVH_Vec3d&             theVertex2,
                                    BVH_Vec3d&             theVertex3) const;

  //! Returns face ID of the given triangle.
  Standard_EXPORT Standard_Integer GetFaceID (const Standard_Integer theIndex) const;

protected:

  //! Array of vertex indices.
  BVH_Array4i myTriangles;

  //! Array of vertex UV params.
  BVH_Array2d myVertUVArray;

  //! Array of vertex coordinates.
  BVH_Array3d myVertexArray;

public:

  DEFINE_STANDARD_RTTI(BRepExtrema_TriangleSet)

};

DEFINE_STANDARD_HANDLE (BRepExtrema_TriangleSet, Standard_Transient)

#endif // _BRepExtrema_TriangleSet_HeaderFile
