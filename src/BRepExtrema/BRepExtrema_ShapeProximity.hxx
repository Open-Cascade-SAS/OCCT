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

#ifndef _BRepExtrema_ShapeProximity_HeaderFile
#define _BRepExtrema_ShapeProximity_HeaderFile

#include <BVH_Geometry.hxx>
#include <BRepExtrema_TriangleSet.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_DataMap.hxx>

//! Set of overlapped sub-shapes.
typedef NCollection_DataMap<Standard_Integer, TColStd_PackedMapOfInteger > BRepExtrema_OverlappedSubShapes;

//! Tool class for shape proximity detection.
//! For two given shapes and given tolerance (offset from the mesh) the algorithm allows
//! to determine whether or not they are overlapped. The algorithm input consists of any
//! shapes which can be decomposed into individual faces (used as basic shape elements).
//! High performance is achieved through the use of existing triangulation of faces. So
//! poly triangulation (with the desired deflection) should already be built. Note that
//! solution is approximate (and corresponds to the deflection used for triangulation).
//!
//! The algorithm can be run in two modes. If tolerance is set to zero, the algorithm
//! will detect only intersecting faces (containing triangles with common points). If
//! tolerance is set to positive value, the algorithm will also detect faces located
//! on distance less than the given tolerance from each other.
class BRepExtrema_ShapeProximity
{
 public:

  //! Creates empty proximity tool.
  Standard_EXPORT BRepExtrema_ShapeProximity (const Standard_Real theTolerance = 0.0);

  //! Creates proximity tool for the given two shapes.
  Standard_EXPORT BRepExtrema_ShapeProximity (const TopoDS_Shape& theShape1,
                                              const TopoDS_Shape& theShape2,
                                              const Standard_Real theTolerance = 0.0);

public:

  //! Returns tolerance value for overlap test (distance between shapes).
  Standard_Real Tolerance() const
  {
    return myTolerance;
  }

  //! Sets tolerance value for overlap test (distance between shapes).
  void SetTolerance (const Standard_Real theTolerance)
  {
    myTolerance = theTolerance;
  }

  //! Loads 1st shape into proximity tool.
  Standard_EXPORT Standard_Boolean LoadShape1 (const TopoDS_Shape& theShape1);

  //! Loads 2nd shape into proximity tool.
  Standard_EXPORT Standard_Boolean LoadShape2 (const TopoDS_Shape& theShape2);

  //! Performs search for overlapped faces.
  Standard_EXPORT void Perform();

  //! True if the search is completed.
  Standard_Boolean IsDone() const
  { 
    return myIsDone;
  }

  //! Returns set of all the face triangles of the 1st shape.
  const NCollection_Handle<BRepExtrema_TriangleSet>& PrimitiveSet1() const
  {
    return myPrimitiveSet1;
  }

  //! Returns set of all the face triangles of the 2nd shape.
  const NCollection_Handle<BRepExtrema_TriangleSet>& PrimitiveSet2() const
  {
    return myPrimitiveSet2;
  }

  //! Returns set of IDs of overlapped faces of 1st shape.
  const BRepExtrema_OverlappedSubShapes& OverlapSubShapes1() const
  {
    return myOverlapSubShapes1;
  }

  //! Returns set of IDs of overlapped faces of 2nd shape.
  const BRepExtrema_OverlappedSubShapes& OverlapSubShapes2() const
  {
    return myOverlapSubShapes2;
  }

  //! Returns sub-shape from 1st shape with the given index.
  const TopoDS_Face& GetSubShape1 (const Standard_Integer theID) const
  {
    return myFaceList1.Value (theID);
  }

  //! Returns sub-shape from 1st shape with the given index.
  const TopoDS_Face& GetSubShape2 (const Standard_Integer theID) const
  {
    return myFaceList2.Value (theID);
  }

protected:

  //! Performs narrow-phase of overlap test (exact intersection).
  void IntersectLeavesExact (const BVH_Vec4i& theLeaf1, const BVH_Vec4i& theLeaf2);

  //! Performs narrow-phase of overlap test (intersection with non-zero tolerance).
  void IntersectLeavesToler (const BVH_Vec4i& theLeaf1, const BVH_Vec4i& theLeaf2);

private:

  //! Maximum overlapping distance.
  Standard_Real myTolerance;

  //! Is the 1st shape initialized?
  Standard_Boolean myIsInitS1;
  //! Is the 2nd shape initialized?
  Standard_Boolean myIsInitS2;

  //! List of faces of the 1st shape.
  BRepExtrema_ShapeList myFaceList1;
  //! List of faces of the 2nd shape.
  BRepExtrema_ShapeList myFaceList2;

  //! Set of all the face triangles of the 1st shape.
  NCollection_Handle<BRepExtrema_TriangleSet> myPrimitiveSet1;
  //! Set of all the face triangles of the 2nd shape.
  NCollection_Handle<BRepExtrema_TriangleSet> myPrimitiveSet2;

  //! Set of overlapped faces of 1st shape.
  BRepExtrema_OverlappedSubShapes myOverlapSubShapes1;
  //! Set of overlapped faces of 2nd shape.
  BRepExtrema_OverlappedSubShapes myOverlapSubShapes2;

  //! Is overlap test completed?
  Standard_Boolean myIsDone;

};

#endif // _BRepExtrema_ShapeProximity_HeaderFile
