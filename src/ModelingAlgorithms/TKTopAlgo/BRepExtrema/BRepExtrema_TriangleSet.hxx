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

#include <BVH_PrimitiveSet3d.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! List of shapes and their IDs for collision detection.

//! Triangle set corresponding to specific face.
class BRepExtrema_TriangleSet : public BVH_PrimitiveSet3d
{
public:
  //! Creates empty triangle set.
  Standard_EXPORT BRepExtrema_TriangleSet();

  //! Creates triangle set from the given face.
  Standard_EXPORT BRepExtrema_TriangleSet(const NCollection_Vector<TopoDS_Shape>& theFaces);

  //! Releases resources of triangle set.
  Standard_EXPORT ~BRepExtrema_TriangleSet() override;

public: //! @name methods implementing BVH set interface
  //! Returns total number of triangles.
  Standard_EXPORT int Size() const override;

  //! Returns AABB of the given triangle.
  Standard_EXPORT BVH_Box<double, 3> Box(const int theIndex) const override;

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_PrimitiveSet3d::Box;

  //! Returns centroid position along specified axis.
  Standard_EXPORT double Center(const int theIndex, const int theAxis) const override;

  //! Swaps indices of two specified triangles.
  Standard_EXPORT void Swap(const int theIndex1, const int theIndex2) override;

public:
  //! Clears triangle set data.
  Standard_EXPORT void Clear();

  //! Initializes triangle set.
  Standard_EXPORT bool Init(const NCollection_Vector<TopoDS_Shape>& theShapes);

  //! Returns all vertices.
  Standard_EXPORT const BVH_Array3d& GetVertices() const { return myVertexArray; }

  //! Returns vertices of the given triangle.
  Standard_EXPORT void GetVertices(const int  theIndex,
                                   BVH_Vec3d& theVertex1,
                                   BVH_Vec3d& theVertex2,
                                   BVH_Vec3d& theVertex3) const;

  //! Returns vertex indices of the given triangle.
  Standard_EXPORT void GetVtxIndices(const int                theIndex,
                                     NCollection_Array1<int>& theVtxIndices) const;

  //! Returns face ID of the given triangle.
  Standard_EXPORT int GetFaceID(const int theIndex) const;

  //! Returns shape ID of the given vertex index.
  Standard_EXPORT int GetShapeIDOfVtx(const int theIndex) const;

  //! Returns vertex index in tringulation of the shape, which vertex belongs,
  //! with the given vtx ID in whole set.
  Standard_EXPORT int GetVtxIdxInShape(const int theIndex) const;

  //! Returns triangle index (before swapping) in tringulation of the shape, which triangle belongs,
  //! with the given trg ID in whole set (after swapping).
  Standard_EXPORT int GetTrgIdxInShape(const int theIndex) const;

private:
  //! Initializes triangle set from the face
  bool initFace(const TopoDS_Face& theFace, const int theIndex);

  //! Initializes polygon from the edge
  bool initEdge(const TopoDS_Edge& theEdge, const int theIndex);

  //! Initializes nodes
  void initNodes(const NCollection_Array1<gp_Pnt>& theNodes,
                 const gp_Trsf&                    theTrsf,
                 const int                         theIndex);

protected:
  //! Array of vertex indices.
  BVH_Array4i myTriangles;

  //! Array of vertex coordinates.
  BVH_Array3d myVertexArray;

  //! Vector of shapes' indices where index of item corresponds to index of vertex,
  //! belonging to this shape.
  NCollection_Vector<int> myShapeIdxOfVtxVec;

  //! Vector of vertexes' number belonging to shape which index corresponds item's index.
  NCollection_Vector<int> myNumVtxInShapeVec;

  //! Vector of triangles' number belonging to shape which index corresponds item's index.
  NCollection_Vector<int> myNumTrgInShapeVec;

  //! Map of triangles' indices after (key) and before (value) swapping.
  NCollection_DataMap<int, int> myTrgIdxMap;

public:
  DEFINE_STANDARD_RTTIEXT(BRepExtrema_TriangleSet, BVH_PrimitiveSet3d)
};

#endif // _BRepExtrema_TriangleSet_HeaderFile
