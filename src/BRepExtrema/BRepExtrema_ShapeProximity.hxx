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
#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#include <BRepExtrema_TriangleSet.hxx>
#include <BRepExtrema_OverlapTool.hxx>

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

  //! Performs search of overlapped faces.
  Standard_EXPORT void Perform();

  //! True if the search is completed.
  Standard_Boolean IsDone() const
  { 
    return myOverlapTool.IsDone();
  }

  //! Returns set of IDs of overlapped faces of 1st shape (started from 0).
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& OverlapSubShapes1() const
  {
    return myOverlapTool.OverlapSubShapes1();
  }

  //! Returns set of IDs of overlapped faces of 2nd shape (started from 0).
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& OverlapSubShapes2() const
  {
    return myOverlapTool.OverlapSubShapes2();
  }

  //! Returns sub-shape from 1st shape with the given index (started from 0).
  const TopoDS_Face& GetSubShape1 (const Standard_Integer theID) const
  {
    return myFaceList1.Value (theID);
  }

  //! Returns sub-shape from 1st shape with the given index (started from 0).
  const TopoDS_Face& GetSubShape2 (const Standard_Integer theID) const
  {
    return myFaceList2.Value (theID);
  }

  //! Returns set of all the face triangles of the 1st shape.
  const Handle(BRepExtrema_TriangleSet)& ElementSet1() const
  {
    return myElementSet1;
  }

  //! Returns set of all the face triangles of the 2nd shape.
  const Handle(BRepExtrema_TriangleSet)& ElementSet2() const
  {
    return myElementSet2;
  }

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
  Handle(BRepExtrema_TriangleSet) myElementSet1;
  //! Set of all the face triangles of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) myElementSet2;

  //! Overlap tool used for intersection/overlap test.
  BRepExtrema_OverlapTool myOverlapTool;

};

#endif // _BRepExtrema_ShapeProximity_HeaderFile
