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

#ifndef _BRepExtrema_SelfIntersection_HeaderFile
#define _BRepExtrema_SelfIntersection_HeaderFile

#include <BRepExtrema_OverlapTool.hxx>

#include <TopoDS.hxx>

//! Tool class for detection of self-sections in the given shape.
//! This class is based on BRepExtrema_OverlapTool and thus uses
//! shape tessellation to detect incorrect mesh fragments (pairs
//! of overlapped triangles belonging to different faces). Thus,
//! a result depends critically on the quality of mesh generator
//! (e.g., BREP mesh is not always a good choice, because it can
//! contain gaps between adjacent face triangulations, which may
//! not share vertices on common edge; thus false overlap can be
//! detected). As a result, this tool can be used for relatively
//! fast approximated test which provides sub-set of potentially
//! overlapped faces.
class BRepExtrema_SelfIntersection : public BRepExtrema_ElementFilter
{
  friend class BRepExtrema_OverlapTool;

public:

  //! Creates uninitialized self-intersection tool.
  Standard_EXPORT BRepExtrema_SelfIntersection (const Standard_Real theTolerance = 0.0);

  //! Creates self-intersection tool for the given shape.
  Standard_EXPORT BRepExtrema_SelfIntersection (const TopoDS_Shape& theShape, const Standard_Real theTolerance = 0.0);

public:

  //! Returns tolerance value used for self-intersection test.
  Standard_Real Tolerance() const
  {
    return myTolerance;
  }

  //! Sets tolerance value used for self-intersection test.
  void SetTolerance (const Standard_Real theTolerance)
  {
    myTolerance = theTolerance;
  }

  //! Loads shape for detection of self-intersections.
  Standard_EXPORT Standard_Boolean LoadShape (const TopoDS_Shape& theShape);

  //! Performs detection of self-intersections.
  Standard_EXPORT void Perform();

  //! True if the detection is completed.
  Standard_Boolean IsDone() const
  { 
    return myOverlapTool.IsDone();
  }

  //! Returns set of IDs of overlapped sub-shapes (started from 0).
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& OverlapElements() const
  {
    return myOverlapTool.OverlapSubShapes1();
  }

  //! Returns sub-shape from the shape for the given index (started from 0).
  const TopoDS_Face& GetSubShape (const Standard_Integer theID) const
  {
    return TopoDS::Face(myFaceList.Value(theID));
  }

  //! Returns set of all the face triangles of the shape.
  const Handle(BRepExtrema_TriangleSet)& ElementSet() const
  {
    return myElementSet;
  }

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
  //! Returns set of overlapped mesh elements (only triangles).
  const TColStd_PackedMapOfInteger& OverlapTriangles() const
  {
    return myOverlapTool.OverlapTriangles1();
  }
#endif

protected:

  //! Filter out correct adjacent mesh elements.
  Standard_EXPORT virtual BRepExtrema_ElementFilter::FilterResult PreCheckElements (const Standard_Integer theIndex1,
                                                                                    const Standard_Integer theIndex2);

  //! Checks if the given triangles have only single common vertex.
  Standard_EXPORT BRepExtrema_ElementFilter::FilterResult isRegularSharedVertex (const BVH_Vec3d& theSharedVert,
                                                                                 const BVH_Vec3d& theTrng1Vtxs1,
                                                                                 const BVH_Vec3d& theTrng1Vtxs2,
                                                                                 const BVH_Vec3d& theTrng2Vtxs1,
                                                                                 const BVH_Vec3d& theTrng2Vtxs2);

  //! Checks if the given triangles have only single common edge.
  Standard_EXPORT BRepExtrema_ElementFilter::FilterResult isRegularSharedEdge (const BVH_Vec3d& theTrng1Vtxs0,
                                                                               const BVH_Vec3d& theTrng1Vtxs1,
                                                                               const BVH_Vec3d& theTrng1Vtxs2,
                                                                               const BVH_Vec3d& theTrng2Vtxs2);

private:

  //! Self-intersection tolerance.
  Standard_Real myTolerance;

  //! Is the input shape inited?
  Standard_Boolean myIsInit;

  //! List of triangulated faces of the shape.
  BRepExtrema_ShapeList myFaceList;

  //! Set of all the face triangles of the shape.
  Handle(BRepExtrema_TriangleSet) myElementSet;

  //! Overlap tool used for self-intersection test.
  BRepExtrema_OverlapTool myOverlapTool;

};

#endif // _BRepExtrema_SelfIntersection_HeaderFile
