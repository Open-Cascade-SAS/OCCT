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

#include <NCollection_DataMap.hxx>
#include <Precision.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#include <BRepExtrema_ProximityValueTool.hxx>
#include <BRepExtrema_TriangleSet.hxx>
#include <BRepExtrema_OverlapTool.hxx>

//! @brief Tool class for shape proximity detection.
//!
//! First approach:
//! For two given shapes and given tolerance (offset from the mesh) the algorithm allows
//! to determine whether or not they are overlapped. The algorithm input consists of any
//! shapes which can be decomposed into individual faces (used as basic shape elements).
//!
//! The algorithm can be run in two modes. If tolerance is set to zero, the algorithm
//! will detect only intersecting faces (containing triangles with common points). If
//! tolerance is set to positive value, the algorithm will also detect faces located
//! on distance less than the given tolerance from each other.
//!
//! Second approach:
//! Compute the proximity value between two shapes (handles only edge/edge or face/face cases)
//! if the tolerance is not defined (Precision::Infinite()).
//! In this case the proximity value is a minimal thickness of a layer containing both shapes.
//!
//! For the both approaches the high performance is achieved through the use of existing
//! triangulation of faces. So, poly triangulation (with the desired deflection) should already
//! be built. Note that solution is approximate (and corresponds to the deflection used for
//! triangulation).
class BRepExtrema_ShapeProximity
{

public:

  //! Creates empty proximity tool.
  Standard_EXPORT BRepExtrema_ShapeProximity (const Standard_Real theTolerance = Precision::Infinite());

  //! Creates proximity tool for the given two shapes.
  Standard_EXPORT BRepExtrema_ShapeProximity (const TopoDS_Shape& theShape1,
                                              const TopoDS_Shape& theShape2,
                                              const Standard_Real theTolerance = Precision::Infinite());

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

  //! Returns proximity value calculated for the whole input shapes.
  Standard_Real Proximity() const
  {
    return Tolerance();
  }

  //! Loads 1st shape into proximity tool.
  Standard_EXPORT Standard_Boolean LoadShape1 (const TopoDS_Shape& theShape1);

  //! Loads 2nd shape into proximity tool.
  Standard_EXPORT Standard_Boolean LoadShape2 (const TopoDS_Shape& theShape2);

  //! Set number of sample points on the 1st shape used to compute the proximity value.
  //! In case of 0, all triangulation nodes will be used.
  void SetNbSamples1(const Standard_Integer theNbSamples) { myNbSamples1 = theNbSamples; }

  //! Set number of sample points on the 2nd shape used to compute the proximity value.
  //! In case of 0, all triangulation nodes will be used.
  void SetNbSamples2(const Standard_Integer theNbSamples) { myNbSamples2 = theNbSamples; }

  //! Performs search of overlapped faces.
  Standard_EXPORT void Perform();

  //! True if the search is completed.
  Standard_Boolean IsDone() const
  { 
    return myOverlapTool.IsDone() || myProxValTool.IsDone();
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
  const TopoDS_Shape& GetSubShape1 (const Standard_Integer theID) const
  {
    return myShapeList1.Value (theID);
  }

  //! Returns sub-shape from 1st shape with the given index (started from 0).
  const TopoDS_Shape& GetSubShape2 (const Standard_Integer theID) const
  {
    return myShapeList2.Value (theID);
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

  //! Returns the point on the 1st shape, which could be used as a reference point
  //! for the value of the proximity.
  const gp_Pnt& ProximityPoint1() const
  {
    return myProxPoint1;
  }

  //! Returns the point on the 2nd shape, which could be used as a reference point
  //! for the value of the proximity.
  const gp_Pnt& ProximityPoint2() const
  {
    return myProxPoint2;
  }

  //! Returns the status of point on the 1st shape, which could be used as a reference point
  //! for the value of the proximity.
  const ProxPnt_Status& ProxPntStatus1() const
  {
    return myProxPntStatus1;
  }

  //! Returns the status of point on the 2nd shape, which could be used as a reference point
  //! for the value of the proximity.
  const ProxPnt_Status& ProxPntStatus2() const
  {
    return myProxPntStatus2;
  }

private:

  //! Maximum overlapping distance.
  Standard_Real myTolerance;

  //! Is the 1st shape initialized?
  Standard_Boolean myIsInitS1;
  //! Is the 2nd shape initialized?
  Standard_Boolean myIsInitS2;

  //! List of subshapes of the 1st shape.
  BRepExtrema_ShapeList myShapeList1;
  //! List of subshapes of the 2nd shape.
  BRepExtrema_ShapeList myShapeList2;

  //! Set of all the face triangles of the 1st shape.
  Handle(BRepExtrema_TriangleSet) myElementSet1;
  //! Set of all the face triangles of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) myElementSet2;

  //! Number of sample points on the 1st shape used to compute the proximity value
  //! (if zero (default), all triangulation nodes will be used).
  Standard_Integer myNbSamples1;
  //! Number of sample points on the 2nd shape used to compute the proximity value
  //! (if zero (default), all triangulation nodes will be used).
  Standard_Integer myNbSamples2;

  //! Reference point of the proximity value on the 1st shape.
  gp_Pnt myProxPoint1;
  //! Reference point of the proximity value on the 2st shape.
  gp_Pnt myProxPoint2;

  //! Status of reference points of the proximity value.
  ProxPnt_Status myProxPntStatus1, myProxPntStatus2;

  //! Overlap tool used for intersection/overlap test.
  BRepExtrema_OverlapTool myOverlapTool;

  //! Shape-shape proximity tool used for computation of
  //! the minimal diameter of a tube containing both edges or
  //! the minimal thickness of a shell containing both faces.
  BRepExtrema_ProximityValueTool myProxValTool;

};

#endif // _BRepExtrema_ShapeProximity_HeaderFile
