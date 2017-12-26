// Created on: 2001-06-29
// Created by: Michael KLOKOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_BeanFaceIntersector_HeaderFile
#define _IntTools_BeanFaceIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard_Real.hxx>
#include <Extrema_ExtCS.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <IntTools_MarkedRangeSet.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <IntTools_ListOfCurveRangeSample.hxx>
#include <IntTools_ListOfSurfaceRangeSample.hxx>
class Geom_Surface;
class IntTools_Context;
class TopoDS_Edge;
class TopoDS_Face;
class BRepAdaptor_Curve;
class BRepAdaptor_Surface;
class IntTools_CurveRangeSample;
class Bnd_Box;
class IntTools_SurfaceRangeSample;
class IntTools_CurveRangeLocalizeData;
class IntTools_SurfaceRangeLocalizeData;


//! The class BeanFaceIntersector computes ranges of parameters on
//! the curve of a bean(part of edge) that bound the parts of bean which
//! are on the surface of a face according to edge and face tolerances.
//! Warning: The real boundaries of the face are not taken into account,
//! Most of the result parts of the bean lays only inside the region of the surface,
//! which includes the inside of the face. And the parts which are out of this region can be
//! excluded from the result.
class IntTools_BeanFaceIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntTools_BeanFaceIntersector();
  

  //! Initializes the algorithm
  //!
  //! Warning:
  //! The parts of the edge which are on
  //! the surface of the face and belong to
  //! the whole in the face (if there is)
  //! is considered as result
  Standard_EXPORT IntTools_BeanFaceIntersector(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);
  

  //! Initializes the algorithm
  Standard_EXPORT IntTools_BeanFaceIntersector(const BRepAdaptor_Curve& theCurve, const BRepAdaptor_Surface& theSurface, const Standard_Real theBeanTolerance, const Standard_Real theFaceTolerance);
  

  //! Initializes the algorithm
  //! theUMinParameter, ... are used for
  //! optimization purposes
  Standard_EXPORT IntTools_BeanFaceIntersector(const BRepAdaptor_Curve& theCurve, const BRepAdaptor_Surface& theSurface, const Standard_Real theFirstParOnCurve, const Standard_Real theLastParOnCurve, const Standard_Real theUMinParameter, const Standard_Real theUMaxParameter, const Standard_Real theVMinParameter, const Standard_Real theVMaxParameter, const Standard_Real theBeanTolerance, const Standard_Real theFaceTolerance);
  

  //! Initializes the algorithm
  //!
  //! Warning:
  //! The parts of the edge which are on
  //! the surface of the face and belong to
  //! the whole in the face (if there is)
  //! is considered as result
  Standard_EXPORT void Init (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);
  

  //! Initializes the algorithm
  Standard_EXPORT void Init (const BRepAdaptor_Curve& theCurve, const BRepAdaptor_Surface& theSurface, const Standard_Real theBeanTolerance, const Standard_Real theFaceTolerance);
  

  //! Initializes the algorithm
  //! theUMinParameter, ... are used for
  //! optimization purposes
  Standard_EXPORT void Init (const BRepAdaptor_Curve& theCurve, const BRepAdaptor_Surface& theSurface, const Standard_Real theFirstParOnCurve, const Standard_Real theLastParOnCurve, const Standard_Real theUMinParameter, const Standard_Real theUMaxParameter, const Standard_Real theVMinParameter, const Standard_Real theVMaxParameter, const Standard_Real theBeanTolerance, const Standard_Real theFaceTolerance);
  

  //! Sets the intersecton context
  Standard_EXPORT void SetContext (const Handle(IntTools_Context)& theContext);
  

  //! Gets the intersecton context
  Standard_EXPORT const Handle(IntTools_Context)& Context() const;
  

  //! Set restrictions for curve
  Standard_EXPORT void SetBeanParameters (const Standard_Real theFirstParOnCurve, const Standard_Real theLastParOnCurve);
  

  //! Set restrictions for surface
  Standard_EXPORT void SetSurfaceParameters (const Standard_Real theUMinParameter, const Standard_Real theUMaxParameter, const Standard_Real theVMinParameter, const Standard_Real theVMaxParameter);
  

  //! Launches the algorithm
  Standard_EXPORT void Perform();
  
  //! Returns Done/NotDone state of the algorithm.
  Standard_Boolean IsDone() const
  {
    return myIsDone;
  }
  
  Standard_EXPORT const IntTools_SequenceOfRanges& Result() const;
  
  Standard_EXPORT void Result (IntTools_SequenceOfRanges& theResults) const;




protected:





private:

  
  Standard_EXPORT void ComputeAroundExactIntersection();
  
  Standard_EXPORT void ComputeLinePlane();
  
  //! Fast check on coincidence of the edge with face for the cases when both shapes are
  //! based on analytic geometries. The method also computes if the intersection
  //! between shapes is possible.
  //! The method returns TRUE if the computation was successful and further computation is unnecessary.
  //! Otherwise it returns FALSE and computation continues.
  Standard_EXPORT Standard_Boolean FastComputeAnalytic();
  
  Standard_EXPORT void ComputeUsingExtremum();
  
  Standard_EXPORT void ComputeNearRangeBoundaries();
  
  Standard_EXPORT Standard_Boolean ComputeLocalized();
  
  Standard_EXPORT void ComputeRangeFromStartPoint (const Standard_Boolean ToIncreaseParameter, const Standard_Real theParameter, const Standard_Real theUParameter, const Standard_Real theVParameter);
  
  Standard_EXPORT void ComputeRangeFromStartPoint (const Standard_Boolean ToIncreaseParameter, const Standard_Real theParameter, const Standard_Real theUParameter, const Standard_Real theVParameter, const Standard_Integer theIndex);
  
  Standard_EXPORT Standard_Real Distance (const Standard_Real theArg, Standard_Real& theUParameter, Standard_Real& theVParameter);
  
  Standard_EXPORT Standard_Real Distance (const Standard_Real theArg);
  
  Standard_EXPORT Standard_Boolean LocalizeSolutions (const IntTools_CurveRangeSample& theCurveRange, const Bnd_Box& theBoxCurve, const IntTools_SurfaceRangeSample& theSurfaceRange, const Bnd_Box& theBoxSurface, IntTools_CurveRangeLocalizeData& theCurveData, IntTools_SurfaceRangeLocalizeData& theSurfaceData, IntTools_ListOfCurveRangeSample& theListCurveRange, IntTools_ListOfSurfaceRangeSample& theListSurfaceRange);
  
  Standard_EXPORT Standard_Boolean TestComputeCoinside();


  BRepAdaptor_Curve myCurve;
  BRepAdaptor_Surface mySurface;
  Handle(Geom_Surface) myTrsfSurface;
  Standard_Real myFirstParameter;
  Standard_Real myLastParameter;
  Standard_Real myUMinParameter;
  Standard_Real myUMaxParameter;
  Standard_Real myVMinParameter;
  Standard_Real myVMaxParameter;
  Standard_Real myBeanTolerance;
  Standard_Real myFaceTolerance;
  Standard_Real myCurveResolution;
  Standard_Real myCriteria;
  Extrema_ExtCS myExtrema;
  GeomAPI_ProjectPointOnSurf myProjector;
  IntTools_MarkedRangeSet myRangeManager;
  Handle(IntTools_Context) myContext;
  IntTools_SequenceOfRanges myResults;
  Standard_Boolean myIsDone;

};

#endif // _IntTools_BeanFaceIntersector_HeaderFile
