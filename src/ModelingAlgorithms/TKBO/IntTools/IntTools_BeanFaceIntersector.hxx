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

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard_Real.hxx>
#include <Extrema_ExtCS.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <IntTools_MarkedRangeSet.hxx>
#include <IntTools_Range.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <IntTools_CurveRangeSample.hxx>
#include <NCollection_List.hxx>
#include <IntTools_SurfaceRangeSample.hxx>
class Geom_Surface;
class IntTools_Context;
class TopoDS_Edge;
class TopoDS_Face;
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
  Standard_EXPORT IntTools_BeanFaceIntersector(const TopoDS_Edge& theEdge,
                                               const TopoDS_Face& theFace);

  //! Initializes the algorithm
  Standard_EXPORT IntTools_BeanFaceIntersector(const BRepAdaptor_Curve&   theCurve,
                                               const BRepAdaptor_Surface& theSurface,
                                               const double               theBeanTolerance,
                                               const double               theFaceTolerance);

  //! Initializes the algorithm
  //! theUMinParameter, ... are used for
  //! optimization purposes
  Standard_EXPORT IntTools_BeanFaceIntersector(const BRepAdaptor_Curve&   theCurve,
                                               const BRepAdaptor_Surface& theSurface,
                                               const double               theFirstParOnCurve,
                                               const double               theLastParOnCurve,
                                               const double               theUMinParameter,
                                               const double               theUMaxParameter,
                                               const double               theVMinParameter,
                                               const double               theVMaxParameter,
                                               const double               theBeanTolerance,
                                               const double               theFaceTolerance);

  //! Initializes the algorithm
  //!
  //! Warning:
  //! The parts of the edge which are on
  //! the surface of the face and belong to
  //! the whole in the face (if there is)
  //! is considered as result
  Standard_EXPORT void Init(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);

  //! Initializes the algorithm
  Standard_EXPORT void Init(const BRepAdaptor_Curve&   theCurve,
                            const BRepAdaptor_Surface& theSurface,
                            const double               theBeanTolerance,
                            const double               theFaceTolerance);

  //! Initializes the algorithm
  //! theUMinParameter, ... are used for
  //! optimization purposes
  Standard_EXPORT void Init(const BRepAdaptor_Curve&   theCurve,
                            const BRepAdaptor_Surface& theSurface,
                            const double               theFirstParOnCurve,
                            const double               theLastParOnCurve,
                            const double               theUMinParameter,
                            const double               theUMaxParameter,
                            const double               theVMinParameter,
                            const double               theVMaxParameter,
                            const double               theBeanTolerance,
                            const double               theFaceTolerance);

  //! Sets the intersection context
  Standard_EXPORT void SetContext(const occ::handle<IntTools_Context>& theContext);

  //! Gets the intersection context
  Standard_EXPORT const occ::handle<IntTools_Context>& Context() const;

  //! Set restrictions for curve
  Standard_EXPORT void SetBeanParameters(const double theFirstParOnCurve,
                                         const double theLastParOnCurve);

  //! Set restrictions for surface
  Standard_EXPORT void SetSurfaceParameters(const double theUMinParameter,
                                            const double theUMaxParameter,
                                            const double theVMinParameter,
                                            const double theVMaxParameter);

  //! Launches the algorithm
  Standard_EXPORT void Perform();

  //! Returns Done/NotDone state of the algorithm.
  bool IsDone() const { return myIsDone; }

  Standard_EXPORT const NCollection_Sequence<IntTools_Range>& Result() const;

  Standard_EXPORT void Result(NCollection_Sequence<IntTools_Range>& theResults) const;

  //! Returns the minimal distance found between edge and face
  double MinimalSquareDistance() const { return myMinSqDistance; }

private:
  Standard_EXPORT void ComputeAroundExactIntersection();

  Standard_EXPORT void ComputeLinePlane();

  //! Fast check on coincidence of the edge with face for the cases when both shapes are
  //! based on analytic geometries. The method also computes if the intersection
  //! between shapes is possible.
  //! The method returns TRUE if the computation was successful and further computation is
  //! unnecessary. Otherwise it returns FALSE and computation continues.
  Standard_EXPORT bool FastComputeAnalytic();

  Standard_EXPORT void ComputeUsingExtremum();

  Standard_EXPORT void ComputeNearRangeBoundaries();

  Standard_EXPORT bool ComputeLocalized();

  Standard_EXPORT void ComputeRangeFromStartPoint(const bool   ToIncreaseParameter,
                                                  const double theParameter,
                                                  const double theUParameter,
                                                  const double theVParameter);

  Standard_EXPORT void ComputeRangeFromStartPoint(const bool   ToIncreaseParameter,
                                                  const double theParameter,
                                                  const double theUParameter,
                                                  const double theVParameter,
                                                  const int    theIndex);

  Standard_EXPORT double Distance(const double theArg,
                                  double&      theUParameter,
                                  double&      theVParameter);

  Standard_EXPORT double Distance(const double theArg);

  Standard_EXPORT bool LocalizeSolutions(
    const IntTools_CurveRangeSample&               theCurveRange,
    const Bnd_Box&                                 theBoxCurve,
    const IntTools_SurfaceRangeSample&             theSurfaceRange,
    const Bnd_Box&                                 theBoxSurface,
    IntTools_CurveRangeLocalizeData&               theCurveData,
    IntTools_SurfaceRangeLocalizeData&             theSurfaceData,
    NCollection_List<IntTools_CurveRangeSample>&   theListCurveRange,
    NCollection_List<IntTools_SurfaceRangeSample>& theListSurfaceRange);

  Standard_EXPORT bool TestComputeCoinside();

  BRepAdaptor_Curve                    myCurve;
  BRepAdaptor_Surface                  mySurface;
  occ::handle<Geom_Surface>            myTrsfSurface;
  double                               myFirstParameter;
  double                               myLastParameter;
  double                               myUMinParameter;
  double                               myUMaxParameter;
  double                               myVMinParameter;
  double                               myVMaxParameter;
  double                               myBeanTolerance;
  double                               myFaceTolerance;
  double                               myCurveResolution;
  double                               myCriteria;
  GeomAPI_ProjectPointOnSurf           myProjector;
  IntTools_MarkedRangeSet              myRangeManager;
  occ::handle<IntTools_Context>        myContext;
  NCollection_Sequence<IntTools_Range> myResults;
  bool                                 myIsDone;
  double                               myMinSqDistance;
};

#endif // _IntTools_BeanFaceIntersector_HeaderFile
