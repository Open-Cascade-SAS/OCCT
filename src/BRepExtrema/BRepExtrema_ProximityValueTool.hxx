// Created on: 2022-08-08
// Created by: Kseniya NOSULKO
// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _BRepExtrema_ProximityValueTool_HeaderFile
#define _BRepExtrema_ProximityValueTool_HeaderFile

#include <BRepExtrema_ProximityDistTool.hxx>
#include <BRepExtrema_TriangleSet.hxx>

//! Tool class for computation of the proximity value from one BVH
//! primitive set to another, solving max(min) problem.
//! This tool is not intended to be used independently, and is integrated
//! in other classes, implementing algorithms based on shape tessellation
//! (BRepExtrema_ShapeProximity and BRepExtrema_SelfIntersection).
//! 
//! Please note that algorithm results are approximate and depend greatly
//! on the quality of input tessellation(s).
class BRepExtrema_ProximityValueTool
{
public:
  typedef typename BRepExtrema_ProximityDistTool::ProxPnt_Status ProxPnt_Status;

public:

  //! Creates new unitialized proximity tool.
  Standard_EXPORT BRepExtrema_ProximityValueTool();

  //! Creates new proximity tool for the given element sets.
  Standard_EXPORT BRepExtrema_ProximityValueTool (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                  const Handle(BRepExtrema_TriangleSet)& theSet2,
                                                  const BRepExtrema_ShapeList& theShapeList1,
                                                  const BRepExtrema_ShapeList& theShapeList2);

public:

  //! Loads the given element sets into the proximity tool.
  Standard_EXPORT void LoadTriangleSets (const Handle (BRepExtrema_TriangleSet)& theSet1,
                                         const Handle (BRepExtrema_TriangleSet)& theSet2);

  //! Loads the given list of subshapes into the proximity tool.
  Standard_EXPORT void LoadShapeLists (const BRepExtrema_ShapeList& theShapeList1,
                                       const BRepExtrema_ShapeList& theShapeList2);

  //! Sets number of sample points used for proximity calculation for each shape.
  //! If number is less or equal zero, all triangulation nodes are used.
  Standard_EXPORT void SetNbSamplePoints (const Standard_Integer theSamples1 = 0,
                                          const Standard_Integer theSamples2 = 0);

  //! Performs the computation of the proximity value.
  Standard_EXPORT void Perform (Standard_Real& theTolerance);

  //! Is proximity test completed?
  Standard_Boolean IsDone() const { return myIsDone; }

  //! Marks test results as outdated.
  void MarkDirty() { myIsDone = Standard_False; }

  //! Returns the computed distance.
  Standard_Real Distance() const { return myDistance; }

  //! Returns points on triangles sets, which provide the proximity distance.
  void ProximityPoints(gp_Pnt& thePoint1, gp_Pnt& thePoint2) const
  {
    thePoint1 = myPnt1;
    thePoint2 = myPnt2;
  }

  //! Returns status of points on triangles sets, which provide the proximity distance.
  void ProximityPointsStatus (ProxPnt_Status& thePointStatus1, ProxPnt_Status& thePointStatus2) const
  {
    thePointStatus1 = myPntStatus1;
    thePointStatus2 = myPntStatus2;
  }

private:

  //! Returns the computed proximity value from first BVH to another one.
  Standard_Real computeProximityDist (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                      const Standard_Integer theNbSamples1,
                                      const Handle(BRepExtrema_TriangleSet)& theSet2,
                                      const BRepExtrema_ShapeList& theShapeList1,
                                      const BRepExtrema_ShapeList& theShapeList2,
                                      BVH_Vec3d& thePoint1,
                                      BVH_Vec3d& thePoint2,
                                      ProxPnt_Status& thePointStatus1,
                                      ProxPnt_Status& thePointStatus2) const;

private:

  //! Set of all mesh primitives of the 1st shape.
  Handle(BRepExtrema_TriangleSet) mySet1;
  //! Set of all mesh primitives of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) mySet2;

  //! List of subshapes of the 1st shape.
  BRepExtrema_ShapeList myShapeList1;
  //! List of subshapes of the 2nd shape.
  BRepExtrema_ShapeList myShapeList2;

  Standard_Real myDistance;  //!< Distance
  Standard_Boolean myIsDone; //!< State of the algorithm

  Standard_Integer myNbSamples1; //!< Number of samples points on the first shape
  Standard_Integer myNbSamples2; //!< Number of samples points on the second shape

  //! Proximity points
  gp_Pnt myPnt1, myPnt2;

  //! Proximity points' status
  ProxPnt_Status myPntStatus1, myPntStatus2;

};

#endif // _BRepExtrema_ProximityValueTool_HeaderFile
