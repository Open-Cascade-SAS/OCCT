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
#include <NCollection_CellFilter.hxx>
#include <Precision.hxx>

typedef NCollection_Vector<gp_XYZ> VectorOfPoint;

//! Class BRepExtrema_VertexInspector
//!   derived from NCollection_CellFilter_InspectorXYZ
//!   This class define the Inspector interface for CellFilter algorithm,
//!   working with gp_XYZ points in 3d space.
//!   Used in search of coincidence points with a certain tolerance.
class BRepExtrema_VertexInspector : public NCollection_CellFilter_InspectorXYZ
{
public:
  typedef Standard_Integer Target;

  //! Constructor; remembers the tolerance
  BRepExtrema_VertexInspector()
  : myTol (Precision::SquareConfusion()),
    myIsNeedAdd (Standard_True)
  {}

  //! Keep the points used for comparison
  void Add (const gp_XYZ& thePnt)
  {
    myPoints.Append (thePnt);
  }

  //! Set tolerance for comparison of point coordinates
  void SetTol (const Standard_Real theTol)
  {
    myTol = theTol;
  }

  //! Set current point to search for coincidence
  void SetCurrent (const gp_XYZ& theCurPnt)
  {
    myCurrent = theCurPnt;
    myIsNeedAdd = Standard_True;
  }

  Standard_Boolean IsNeedAdd()
  {
    return myIsNeedAdd;
  }

  //! Implementation of inspection method
  Standard_EXPORT NCollection_CellFilter_Action Inspect (const Standard_Integer theTarget);

private:
  Standard_Real myTol;
  Standard_Boolean myIsNeedAdd;
  VectorOfPoint myPoints;
  gp_XYZ myCurrent;
};

typedef NCollection_CellFilter<BRepExtrema_VertexInspector> BRepExtrema_CellFilter;
typedef typename BRepExtrema_ProximityDistTool::ProxPnt_Status ProxPnt_Status;

//! Tool class for computation of the proximity value from one BVH
//! primitive set to another, solving max(min) problem.
//! Handles only edge/edge or face/face cases.
//! This tool is not intended to be used independently, and is integrated
//! in other classes, implementing algorithms based on shape tessellation
//! (BRepExtrema_ShapeProximity and BRepExtrema_SelfIntersection).
//! 
//! Please note that algorithm results are approximate and depend greatly
//! on the quality of input tessellation(s).
class BRepExtrema_ProximityValueTool
{

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

  //! Gets shape data for further refinement.
  Standard_Boolean getInfoForRefinement (const TopoDS_Shape& theShapes,
                                         TopAbs_ShapeEnum& theShapeType,
                                         Standard_Integer& theNbNodes,
                                         Standard_Real& theStep);

  //! Returns the computed proximity value from first BVH to another one.
  Standard_Real computeProximityDist (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                      const Standard_Integer theNbSamples1,
                                      const BVH_Array3d& theAddVertices1,
                                      const NCollection_Vector<ProxPnt_Status>& theAddStatus1,
                                      const Handle(BRepExtrema_TriangleSet)& theSet2,
                                      const BRepExtrema_ShapeList& theShapeList1,
                                      const BRepExtrema_ShapeList& theShapeList2,
                                      BVH_Vec3d& thePoint1,
                                      BVH_Vec3d& thePoint2,
                                      ProxPnt_Status& thePointStatus1,
                                      ProxPnt_Status& thePointStatus2) const;

  //! Gets additional vertices on shapes with refining a coarser one if it's needed.
  Standard_Boolean getShapesAdditionalVertices();

  //! Gets additional vertices and their statuses on the edge with the input step.
  Standard_Boolean getEdgeAdditionalVertices (const TopoDS_Edge& theEdge,
                                              const Standard_Real theStep,
                                              BVH_Array3d& theAddVertices,
                                              NCollection_Vector<ProxPnt_Status>& theAddStatuses);

  //! Gets additional vertices and their statuses on the face with the input step (triangle square).
  Standard_Boolean getFaceAdditionalVertices (const TopoDS_Face& theFace,
                                              const Standard_Real theStep,
                                              BVH_Array3d& theAddVertices,
                                              NCollection_Vector<ProxPnt_Status>& theAddStatuses);

  //! Splits the triangle recursively, halving the longest side
  //! to the area of ​​the current triangle > input step
  void doRecurTrgSplit (const gp_Pnt (&theTrg)[3],
                        const ProxPnt_Status (&theEdgesStatus)[3],
                        const Standard_Real theTol,
                        const Standard_Real theStep,
                        BVH_Array3d& theAddVertices,
                        NCollection_Vector<ProxPnt_Status>& theAddStatuses);
private:

  //! Set of all mesh primitives of the 1st shape.
  Handle(BRepExtrema_TriangleSet) mySet1;
  //! Set of all mesh primitives of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) mySet2;

  //! List of subshapes of the 1st shape.
  BRepExtrema_ShapeList myShapeList1;
  //! List of subshapes of the 2nd shape.
  BRepExtrema_ShapeList myShapeList2;

  //! The 1st shape.
  TopoDS_Shape myShape1;
  //! The 2nd shape.
  TopoDS_Shape myShape2;

  BVH_Array3d myAddVertices1; //!< Additional vertices on the 1st shape if its mesh is coarser.
  BVH_Array3d myAddVertices2; //!< Additional vertices on the 2nd shape if its mesh is coarser.

  NCollection_Vector<ProxPnt_Status> myAddStatus1; //!< Status of additional vertices on the 1st shape.
  NCollection_Vector<ProxPnt_Status> myAddStatus2; //!< Status of additional vertices on the 2nd shape.

  Standard_Boolean myIsInitS1; //!< Is the 1st shape initialized?
  Standard_Boolean myIsInitS2; //!< Is the 2nd shape initialized?

  Standard_Boolean myIsRefinementRequired1; //!< Flag about the need to refine the 1st shape.
  Standard_Boolean myIsRefinementRequired2; //!< Flag about the need to refine the 2nd shape.

  Standard_Integer myNbNodes1; //!< Number of nodes in triangulation of the 1st shape.
  Standard_Integer myNbNodes2; //!< Number of nodes in triangulation of the 2nd shape.

  Standard_Real myStep1; //!< Step for getting vertices on the 1st shape.
  Standard_Real myStep2; //!< Step for getting vertices on the 2nd shape.

  BRepExtrema_CellFilter myCells;
  BRepExtrema_VertexInspector myInspector;

  TopAbs_ShapeEnum myShapeType1; //!< 1st shape type.
  TopAbs_ShapeEnum myShapeType2; //!< 2nd shape type.

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
