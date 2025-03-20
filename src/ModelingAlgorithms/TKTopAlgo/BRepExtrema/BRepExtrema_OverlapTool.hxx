// Created on: 2015-04-26
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BRepExtrema_OverlapTool_HeaderFile
#define _BRepExtrema_OverlapTool_HeaderFile

#include <BRepExtrema_TriangleSet.hxx>
#include <BRepExtrema_ElementFilter.hxx>
#include <BRepExtrema_MapOfIntegerPackedMapOfInteger.hxx>
#include <BVH_Traverse.hxx>

//! Enables storing of individual overlapped triangles (useful for debug).
// #define OVERLAP_TOOL_OUTPUT_TRIANGLES

//! Tool class for for detection of overlapping of two BVH primitive sets.
//! This tool is not intended to be used independently, and is integrated
//! in other classes, implementing algorithms based on shape tessellation
//! (BRepExtrema_ShapeProximity and BRepExtrema_SelfIntersection).
//!
//! Note that input element sets may correspond to different shapes or to
//! the same shape. In first case, tessellations of two given shapes will
//! be tested for intersection (or overlapping, if tolerance is not zero).
//! In second case, tessellation of single shape will be tested for self-
//! intersections. Please note that algorithm results are approximate and
//! depend greatly on the quality of input tessellation(s).
class BRepExtrema_OverlapTool : public BVH_PairTraverse<Standard_Real, 3>
{
public:
  //! Creates new uninitialized overlap tool.
  BRepExtrema_OverlapTool();

  //! Creates new overlap tool for the given element sets.
  BRepExtrema_OverlapTool(const Handle(BRepExtrema_TriangleSet)& theSet1,
                          const Handle(BRepExtrema_TriangleSet)& theSet2);

public:
  //! Loads the given element sets into the overlap tool.
  void LoadTriangleSets(const Handle(BRepExtrema_TriangleSet)& theSet1,
                        const Handle(BRepExtrema_TriangleSet)& theSet2);

  //! Performs searching of overlapped mesh elements.
  void Perform(const Standard_Real theTolerance = 0.0);

  //! Is overlap test completed?
  Standard_Boolean IsDone() const { return myIsDone; }

  //! Marks test results as outdated.
  void MarkDirty() { myIsDone = Standard_False; }

  //! Returns set of overlapped sub-shapes of 1st shape (currently only faces are detected).
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& OverlapSubShapes1() const
  {
    return myOverlapSubShapes1;
  }

  //! Returns set of overlapped sub-shapes of 2nd shape (currently only faces are detected).
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& OverlapSubShapes2() const
  {
    return myOverlapSubShapes2;
  }

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
  //! Returns set of overlapped triangles from the 1st shape (for debug).
  const TColStd_PackedMapOfInteger& OverlapTriangles1() const { return myOverlapTriangles1; }

  //! Returns set of overlapped triangles from the 2nd shape (for debug).
  const TColStd_PackedMapOfInteger& OverlapTriangles2() const { return myOverlapTriangles2; }
#endif

  //! Sets filtering tool for preliminary checking pairs of mesh elements.
  void SetElementFilter(BRepExtrema_ElementFilter* theFilter) { myFilter = theFilter; }

public: //! @name Reject/Accept implementations
  //! Defines the rules for node rejection by bounding box
  Standard_EXPORT virtual Standard_Boolean RejectNode(const BVH_Vec3d& theCornerMin1,
                                                      const BVH_Vec3d& theCornerMax1,
                                                      const BVH_Vec3d& theCornerMin2,
                                                      const BVH_Vec3d& theCornerMax2,
                                                      Standard_Real&) const Standard_OVERRIDE;
  //! Defines the rules for leaf acceptance
  Standard_EXPORT virtual Standard_Boolean Accept(const Standard_Integer theLeaf1,
                                                  const Standard_Integer theLeaf2)
    Standard_OVERRIDE;

protected:
  //! Performs narrow-phase of overlap test (exact intersection).
  void intersectTrianglesExact(const Standard_Integer theTrgIdx1,
                               const Standard_Integer theTrgIdx2);

  //! Performs narrow-phase of overlap test (intersection with non-zero tolerance).
  void intersectTrianglesToler(const Standard_Integer theTrgIdx1,
                               const Standard_Integer theTrgIdx2,
                               const Standard_Real    theToler);

private:
  //! Set of all mesh elements (triangles) of the 1st shape.
  Handle(BRepExtrema_TriangleSet) mySet1;
  //! Set of all mesh elements (triangles) of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) mySet2;

  //! Filter for preliminary checking pairs of mesh elements.
  BRepExtrema_ElementFilter* myFilter;

  //! Resulted set of overlapped sub-shapes of 1st shape (only faces).
  BRepExtrema_MapOfIntegerPackedMapOfInteger myOverlapSubShapes1;
  //! Resulted set of overlapped sub-shapes of 2nd shape (only faces).
  BRepExtrema_MapOfIntegerPackedMapOfInteger myOverlapSubShapes2;

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
  //! Set of overlapped elements from the 1st shape (only triangles).
  TColStd_PackedMapOfInteger myOverlapTriangles1;
  //! Set of overlapped elements from the 2nd shape (only triangles).
  TColStd_PackedMapOfInteger myOverlapTriangles2;
#endif

  //! Is overlap test test completed?
  Standard_Boolean myIsDone;

  Standard_Real myTolerance;
};

#endif // _BRepExtrema_OverlapTool_HeaderFile
