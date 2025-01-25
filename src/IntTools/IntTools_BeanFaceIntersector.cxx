// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <IntTools_BeanFaceIntersector.hxx>

#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenExtCS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomInt.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_IntersectionSegment.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_CurveRangeLocalizeData.hxx>
#include <IntTools_CurveRangeSample.hxx>
#include <IntTools_ListOfBox.hxx>
#include <IntTools_ListOfCurveRangeSample.hxx>
#include <IntTools_ListOfSurfaceRangeSample.hxx>
#include <IntTools_SurfaceRangeLocalizeData.hxx>
#include <IntTools_SurfaceRangeSample.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <NCollection_IndexedMap.hxx>

static Standard_Boolean SetEmptyResultRange(const Standard_Real      theParameter,
                                            IntTools_MarkedRangeSet& theMarkedRange);

static Bnd_Box GetSurfaceBox(const Handle(Geom_BSplineSurface)& theSurf,
                             const Standard_Real                theFirstU,
                             const Standard_Real                theLastU,
                             const Standard_Real                theFirstV,
                             const Standard_Real                theLastV,
                             const Standard_Real                theTolerance,
                             IntTools_SurfaceRangeLocalizeData& theSurfaceData);

static void ComputeGridPoints(const Handle(Geom_BSplineSurface)& theSurf,
                              const Standard_Real                theFirstU,
                              const Standard_Real                theLastU,
                              const Standard_Real                theFirstV,
                              const Standard_Real                theLastV,
                              const Standard_Real                theTolerance,
                              IntTools_SurfaceRangeLocalizeData& theSurfaceData);

static void BuildBox(const Handle(Geom_BSplineSurface)& theSurf,
                     const Standard_Real                theFirstU,
                     const Standard_Real                theLastU,
                     const Standard_Real                theFirstV,
                     const Standard_Real                theLastV,
                     IntTools_SurfaceRangeLocalizeData& theSurfaceData,
                     Bnd_Box&                           theBox);

static void MergeSolutions(const IntTools_ListOfCurveRangeSample&   theListCurveRange,
                           const IntTools_ListOfSurfaceRangeSample& theListSurfaceRange,
                           IntTools_ListOfCurveRangeSample&         theListCurveRangeSort,
                           IntTools_ListOfSurfaceRangeSample&       theListSurfaceRangeSort);

static void CheckSampling(const IntTools_CurveRangeSample&         theCurveRange,
                          const IntTools_SurfaceRangeSample&       theSurfaceRange,
                          const IntTools_CurveRangeLocalizeData&   theCurveData,
                          const IntTools_SurfaceRangeLocalizeData& theSurfaceData,
                          const Standard_Real                      DiffC,
                          const Standard_Real                      DiffU,
                          const Standard_Real                      DiffV,
                          Standard_Boolean&                        bAllowSamplingC,
                          Standard_Boolean&                        bAllowSamplingU,
                          Standard_Boolean&                        bAllowSamplingV);

// ==================================================================================
// function: IntTools_BeanFaceIntersector
// purpose:
// ==================================================================================
IntTools_BeanFaceIntersector::IntTools_BeanFaceIntersector()
    : myFirstParameter(0.),
      myLastParameter(0.),
      myUMinParameter(0.),
      myUMaxParameter(0.),
      myVMinParameter(0.),
      myVMaxParameter(0.),
      myBeanTolerance(0.),
      myFaceTolerance(0.),
      myIsDone(Standard_False),
      myMinSqDistance(RealLast())
{
  myCriteria        = Precision::Confusion();
  myCurveResolution = Precision::PConfusion();
}

// ==================================================================================
// function: IntTools_BeanFaceIntersector
// purpose:
// ==================================================================================
IntTools_BeanFaceIntersector::IntTools_BeanFaceIntersector(const TopoDS_Edge& theEdge,
                                                           const TopoDS_Face& theFace)
    : myFirstParameter(0.),
      myLastParameter(0.),
      myUMinParameter(0.),
      myUMaxParameter(0.),
      myVMinParameter(0.),
      myVMaxParameter(0.),
      myBeanTolerance(0.),
      myFaceTolerance(0.),
      myIsDone(Standard_False),
      myMinSqDistance(RealLast())
{
  Init(theEdge, theFace);
}

// ==================================================================================
// function: IntTools_BeanFaceIntersector
// purpose:
// ==================================================================================
IntTools_BeanFaceIntersector::IntTools_BeanFaceIntersector(const BRepAdaptor_Curve&   theCurve,
                                                           const BRepAdaptor_Surface& theSurface,
                                                           const Standard_Real theBeanTolerance,
                                                           const Standard_Real theFaceTolerance)
    : myFirstParameter(0.),
      myLastParameter(0.),
      myUMinParameter(0.),
      myUMaxParameter(0.),
      myVMinParameter(0.),
      myVMaxParameter(0.),
      myIsDone(Standard_False),
      myMinSqDistance(RealLast())
{
  Init(theCurve, theSurface, theBeanTolerance, theFaceTolerance);
}

// ==================================================================================
// function: IntTools_BeanFaceIntersector
// purpose:
// ==================================================================================
IntTools_BeanFaceIntersector::IntTools_BeanFaceIntersector(const BRepAdaptor_Curve&   theCurve,
                                                           const BRepAdaptor_Surface& theSurface,
                                                           const Standard_Real theFirstParOnCurve,
                                                           const Standard_Real theLastParOnCurve,
                                                           const Standard_Real theUMinParameter,
                                                           const Standard_Real theUMaxParameter,
                                                           const Standard_Real theVMinParameter,
                                                           const Standard_Real theVMaxParameter,
                                                           const Standard_Real theBeanTolerance,
                                                           const Standard_Real theFaceTolerance)
    : myFirstParameter(theFirstParOnCurve),
      myLastParameter(theLastParOnCurve),
      myUMinParameter(theUMinParameter),
      myUMaxParameter(theUMaxParameter),
      myVMinParameter(theVMinParameter),
      myVMaxParameter(theVMaxParameter),
      myBeanTolerance(theBeanTolerance),
      myFaceTolerance(theFaceTolerance),
      myIsDone(Standard_False),
      myMinSqDistance(RealLast())
{
  myCurve = theCurve;

  myCriteria        = myBeanTolerance + myFaceTolerance;
  myCurveResolution = myCurve.Resolution(myCriteria);

  mySurface = theSurface;
  myTrsfSurface =
    Handle(Geom_Surface)::DownCast(mySurface.Surface().Surface()->Transformed(mySurface.Trsf()));
}

// ==================================================================================
// function: Init
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::Init(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace)
{
  if (myContext.IsNull())
  {
    myContext = new IntTools_Context;
  }
  //
  myCurve.Initialize(theEdge);
  mySurface = myContext->SurfaceAdaptor(theFace);
  myTrsfSurface =
    Handle(Geom_Surface)::DownCast(mySurface.Surface().Surface()->Transformed(mySurface.Trsf()));
  myBeanTolerance = BRep_Tool::Tolerance(theEdge);
  myFaceTolerance = BRep_Tool::Tolerance(theFace);

  myCriteria        = myBeanTolerance + myFaceTolerance + Precision::Confusion();
  myCurveResolution = myCurve.Resolution(myCriteria);

  SetSurfaceParameters(mySurface.FirstUParameter(),
                       mySurface.LastUParameter(),
                       mySurface.FirstVParameter(),
                       mySurface.LastVParameter());
  myResults.Clear();
}

// ==================================================================================
// function: Init
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::Init(const BRepAdaptor_Curve&   theCurve,
                                        const BRepAdaptor_Surface& theSurface,
                                        const Standard_Real        theBeanTolerance,
                                        const Standard_Real        theFaceTolerance)
{
  myCurve   = theCurve;
  mySurface = theSurface;
  myTrsfSurface =
    Handle(Geom_Surface)::DownCast(mySurface.Surface().Surface()->Transformed(mySurface.Trsf()));
  myBeanTolerance = theBeanTolerance;
  myFaceTolerance = theFaceTolerance;

  myCriteria        = myBeanTolerance + myFaceTolerance;
  myCurveResolution = myCurve.Resolution(myCriteria);

  SetSurfaceParameters(mySurface.FirstUParameter(),
                       mySurface.LastUParameter(),
                       mySurface.FirstVParameter(),
                       mySurface.LastVParameter());
  myResults.Clear();
}

// ==================================================================================
// function: Init
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::Init(const BRepAdaptor_Curve&   theCurve,
                                        const BRepAdaptor_Surface& theSurface,
                                        const Standard_Real        theFirstParOnCurve,
                                        const Standard_Real        theLastParOnCurve,
                                        const Standard_Real        theUMinParameter,
                                        const Standard_Real        theUMaxParameter,
                                        const Standard_Real        theVMinParameter,
                                        const Standard_Real        theVMaxParameter,
                                        const Standard_Real        theBeanTolerance,
                                        const Standard_Real        theFaceTolerance)
{
  Init(theCurve, theSurface, theBeanTolerance, theFaceTolerance);
  SetBeanParameters(theFirstParOnCurve, theLastParOnCurve);
  SetSurfaceParameters(theUMinParameter, theUMaxParameter, theVMinParameter, theVMaxParameter);
}

// ==================================================================================
// function: SetContext
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::SetContext(const Handle(IntTools_Context)& theContext)
{
  myContext = theContext;
}

// ==================================================================================
// function: Context
// purpose:
// ==================================================================================
const Handle(IntTools_Context)& IntTools_BeanFaceIntersector::Context() const
{
  return myContext;
}

// ==================================================================================
// function: SetBeanParameters
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::SetBeanParameters(const Standard_Real theFirstParOnCurve,
                                                     const Standard_Real theLastParOnCurve)
{
  myFirstParameter = theFirstParOnCurve;
  myLastParameter  = theLastParOnCurve;
}

// ==================================================================================
// function: SetSurfaceParameters
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::SetSurfaceParameters(const Standard_Real theUMinParameter,
                                                        const Standard_Real theUMaxParameter,
                                                        const Standard_Real theVMinParameter,
                                                        const Standard_Real theVMaxParameter)
{
  myUMinParameter = theUMinParameter;
  myUMaxParameter = theUMaxParameter;
  myVMinParameter = theVMinParameter;
  myVMaxParameter = theVMaxParameter;
}

// ==================================================================================
// function: Perform
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::Perform()
{
  myIsDone = Standard_False;
  myResults.Clear();

  if (myContext.IsNull())
  {
    myContext = new IntTools_Context;
  }

  // Fast computation of Line/Plane case
  if (myCurve.GetType() == GeomAbs_Line && mySurface.GetType() == GeomAbs_Plane)
  {
    ComputeLinePlane();
    return;
  }

  // Fast check on coincidence for analytic cases
  if (FastComputeAnalytic())
  {
    // no further computation is necessary
    myIsDone = Standard_True;
    return;
  }

  // Initialization of the range manager
  myRangeManager.SetBoundaries(myFirstParameter, myLastParameter, 0);

  // Check coincidence
  Standard_Boolean isCoincide = TestComputeCoinside();
  if (isCoincide)
  {
    myResults.Append(IntTools_Range(myFirstParameter, myLastParameter));
    myIsDone = Standard_True;
    return;
  }

  // Perform intersection

  // try to find localized solution
  Standard_Boolean bLocalize =
    (!Precision::IsInfinite(myUMinParameter) && !Precision::IsInfinite(myUMaxParameter)
     && !Precision::IsInfinite(myVMinParameter) && !Precision::IsInfinite(myVMaxParameter));
  bLocalize =
    bLocalize
    && (mySurface.GetType() == GeomAbs_BezierSurface || mySurface.GetType() == GeomAbs_OtherSurface
        || (mySurface.GetType() == GeomAbs_BSplineSurface
            && (mySurface.UDegree() > 2 || mySurface.VDegree() > 2)
            && (mySurface.NbUKnots() > 2 && mySurface.NbVKnots() > 2)));

  Standard_Boolean isLocalized = bLocalize && ComputeLocalized();

  // Perform real intersection
  if (!isLocalized)
  {
    ComputeAroundExactIntersection();

    ComputeUsingExtremum();

    ComputeNearRangeBoundaries();
  }

  myIsDone = Standard_True;

  // Treatment of the results
  for (Standard_Integer i = 1; i <= myRangeManager.Length(); i++)
  {
    if (myRangeManager.Flag(i) != 2)
      continue;

    IntTools_Range   aRange     = myRangeManager.Range(i);
    Standard_Integer iLastRange = myResults.Length();
    if (iLastRange > 0)
    {
      IntTools_Range& aLastRange = myResults.ChangeValue(iLastRange);
      if (Abs(aRange.First() - aLastRange.Last()) > Precision::PConfusion())
      {
        myResults.Append(aRange);
      }
      else
      {
        aLastRange.SetLast(aRange.Last());
      }
    }
    else
      myResults.Append(aRange);
  }
}

// ==================================================================================
// function: Result
// purpose:
// ==================================================================================
const IntTools_SequenceOfRanges& IntTools_BeanFaceIntersector::Result() const
{
  return myResults;
}

// ==================================================================================
// function: Result
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::Result(IntTools_SequenceOfRanges& theResults) const
{
  theResults = myResults;
}

// ==================================================================================
// function: Distance
// purpose:
// ==================================================================================
Standard_Real IntTools_BeanFaceIntersector::Distance(const Standard_Real theArg)
{
  gp_Pnt aPoint = myCurve.Value(theArg);

  GeomAPI_ProjectPointOnSurf& aProjector = myContext->ProjPS(mySurface.Face());
  aProjector.Perform(aPoint);

  if (aProjector.IsDone() && aProjector.NbPoints() > 0)
  {
    return aProjector.LowerDistance();
  }
  //
  Standard_Real aDistance = RealLast();

  for (Standard_Integer i = 0; i < 4; i++)
  {
    Standard_Real anIsoParameter =
      (i == 0) ? myUMinParameter
               : ((i == 1) ? myUMaxParameter : ((i == 2) ? myVMinParameter : myVMaxParameter));
    Standard_Real aMinParameter = (i < 2) ? myVMinParameter : myUMinParameter;
    Standard_Real aMaxParameter = (i < 2) ? myVMaxParameter : myUMaxParameter;
    Standard_Real aMidParameter = (aMinParameter + aMaxParameter) * 0.5;
    gp_Pnt        aPointMin     = (i < 2) ? mySurface.Value(anIsoParameter, aMinParameter)
                                          : mySurface.Value(aMinParameter, anIsoParameter);
    gp_Pnt        aPointMax     = (i < 2) ? mySurface.Value(anIsoParameter, aMaxParameter)
                                          : mySurface.Value(aMaxParameter, anIsoParameter);
    gp_Pnt        aPointMid     = (i < 2) ? mySurface.Value(anIsoParameter, aMidParameter)
                                          : mySurface.Value(aMidParameter, anIsoParameter);

    Standard_Boolean useMinMaxPoints = Standard_True;
    Standard_Boolean computeisoline  = Standard_True;

    if (aPointMin.IsEqual(aPointMax, myCriteria) && aPointMin.IsEqual(aPointMid, myCriteria)
        && aPointMax.IsEqual(aPointMid, myCriteria))
    {
      computeisoline = Standard_False;
    }

    if (computeisoline)
    {
      Handle(Geom_Curve) aCurve =
        (i < 2) ? myTrsfSurface->UIso(anIsoParameter) : myTrsfSurface->VIso(anIsoParameter);
      GeomAPI_ProjectPointOnCurve aProjectorOnCurve(aPoint, aCurve, aMinParameter, aMaxParameter);

      if (aProjectorOnCurve.NbPoints() > 0)
      {
        useMinMaxPoints = Standard_False;

        if (aDistance > aProjectorOnCurve.LowerDistance())
          aDistance = aProjectorOnCurve.LowerDistance();
      }
    }

    if (useMinMaxPoints)
    {
      Standard_Real aPPDistance = aPoint.Distance(aPointMin);
      aDistance                 = (aPPDistance < aDistance) ? aPPDistance : aDistance;
      aPPDistance               = aPoint.Distance(aPointMax);
      aDistance                 = (aPPDistance < aDistance) ? aPPDistance : aDistance;
    }
  }
  return aDistance;
}

// ==================================================================================
// function: Distance
// purpose:
// ==================================================================================
Standard_Real IntTools_BeanFaceIntersector::Distance(const Standard_Real theArg,
                                                     Standard_Real&      theUParameter,
                                                     Standard_Real&      theVParameter)
{
  gp_Pnt aPoint = myCurve.Value(theArg);

  theUParameter = myUMinParameter;
  theVParameter = myVMinParameter;
  //
  Standard_Real    aDistance       = RealLast();
  Standard_Boolean projectionfound = Standard_False;

  GeomAPI_ProjectPointOnSurf& aProjector = myContext->ProjPS(mySurface.Face());
  aProjector.Perform(aPoint);

  if (aProjector.IsDone() && aProjector.NbPoints() > 0)
  {
    aProjector.LowerDistanceParameters(theUParameter, theVParameter);
    aDistance       = aProjector.LowerDistance();
    projectionfound = Standard_True;
  }

  if (!projectionfound)
  {
    //
    for (Standard_Integer i = 0; i < 4; i++)
    {
      Standard_Real anIsoParameter =
        (i == 0) ? myUMinParameter
                 : ((i == 1) ? myUMaxParameter : ((i == 2) ? myVMinParameter : myVMaxParameter));
      Standard_Real aMinParameter = (i < 2) ? myVMinParameter : myUMinParameter;
      Standard_Real aMaxParameter = (i < 2) ? myVMaxParameter : myUMaxParameter;
      Standard_Real aMidParameter = (aMinParameter + aMaxParameter) * 0.5;
      gp_Pnt        aPointMin     = (i < 2) ? mySurface.Value(anIsoParameter, aMinParameter)
                                            : mySurface.Value(aMinParameter, anIsoParameter);
      gp_Pnt        aPointMax     = (i < 2) ? mySurface.Value(anIsoParameter, aMaxParameter)
                                            : mySurface.Value(aMaxParameter, anIsoParameter);
      gp_Pnt        aPointMid     = (i < 2) ? mySurface.Value(anIsoParameter, aMidParameter)
                                            : mySurface.Value(aMidParameter, anIsoParameter);

      Standard_Boolean useMinMaxPoints = Standard_True;
      Standard_Boolean computeisoline  = Standard_True;

      if (aPointMin.IsEqual(aPointMax, myCriteria) && aPointMin.IsEqual(aPointMid, myCriteria)
          && aPointMax.IsEqual(aPointMid, myCriteria))
      {
        computeisoline = Standard_False;
      }

      if (computeisoline)
      {
        Handle(Geom_Curve) aCurve =
          (i < 2) ? myTrsfSurface->UIso(anIsoParameter) : myTrsfSurface->VIso(anIsoParameter);
        GeomAPI_ProjectPointOnCurve aProjectorOnCurve(aPoint, aCurve, aMinParameter, aMaxParameter);

        if (aProjectorOnCurve.NbPoints() > 0)
        {
          useMinMaxPoints = Standard_False;

          if (aDistance > aProjectorOnCurve.LowerDistance())
          {
            theUParameter = (i <= 1) ? anIsoParameter : aProjectorOnCurve.LowerDistanceParameter();
            theVParameter = (i >= 2) ? anIsoParameter : aProjectorOnCurve.LowerDistanceParameter();
            aDistance     = aProjectorOnCurve.LowerDistance();
          }
        }
      }

      if (useMinMaxPoints)
      {
        Standard_Real aPPDistance = aPoint.Distance(aPointMin);

        if (aPPDistance < aDistance)
        {
          theUParameter = (i <= 1) ? anIsoParameter : aMinParameter;
          theVParameter = (i >= 2) ? anIsoParameter : aMinParameter;
          aDistance     = aPPDistance;
        }
        aPPDistance = aPoint.Distance(aPointMax);

        if (aPPDistance < aDistance)
        {
          theUParameter = (i <= 1) ? anIsoParameter : aMaxParameter;
          theVParameter = (i >= 2) ? anIsoParameter : aMaxParameter;
          aDistance     = aPPDistance;
        }
      }
    }
  }
  theUParameter = (myUMinParameter > theUParameter) ? myUMinParameter : theUParameter;
  theUParameter = (myUMaxParameter < theUParameter) ? myUMaxParameter : theUParameter;
  theVParameter = (myVMinParameter > theVParameter) ? myVMinParameter : theVParameter;
  theVParameter = (myVMaxParameter < theVParameter) ? myVMaxParameter : theVParameter;

  return aDistance;
}

// ==================================================================================
// function: ComputeAroundExactIntersection
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeAroundExactIntersection()
{
  IntCurveSurface_HInter anExactIntersector;

  Handle(BRepAdaptor_Curve)   aCurve   = new BRepAdaptor_Curve(myCurve);
  Handle(BRepAdaptor_Surface) aSurface = new BRepAdaptor_Surface(mySurface);

  anExactIntersector.Perform(aCurve, aSurface);

  if (anExactIntersector.IsDone())
  {
    Standard_Integer i = 0;

    if (anExactIntersector.NbPoints() > 1)
    {
      // To avoid unification of the intersection points in a single intersection
      // range, perform exact range search considering the lowest possible tolerance
      // for edge and face.
      myCriteria        = 3 * Precision::Confusion();
      myCurveResolution = myCurve.Resolution(myCriteria);
    }

    for (i = 1; i <= anExactIntersector.NbPoints(); i++)
    {
      const IntCurveSurface_IntersectionPoint& aPoint = anExactIntersector.Point(i);

      if ((aPoint.W() >= myFirstParameter) && (aPoint.W() <= myLastParameter))
      {
        Standard_Boolean UIsNotValid =
          ((myUMinParameter > aPoint.U()) || (aPoint.U() > myUMaxParameter));
        Standard_Boolean VIsNotValid =
          ((myVMinParameter > aPoint.V()) || (aPoint.V() > myVMaxParameter));
        Standard_Boolean solutionIsValid = !UIsNotValid && !VIsNotValid;
        Standard_Real    U               = aPoint.U();
        Standard_Real    V               = aPoint.V();

        if (UIsNotValid || VIsNotValid)
        {
          Standard_Boolean bUCorrected = Standard_True;

          if (UIsNotValid)
          {
            bUCorrected     = Standard_False;
            solutionIsValid = Standard_False;
            //
            if (mySurface.IsUPeriodic())
            {
              Standard_Real aNewU, aUPeriod, aEps, du;
              //
              aUPeriod = mySurface.UPeriod();
              aEps     = Epsilon(aUPeriod);
              //
              GeomInt::AdjustPeriodic(U,
                                      myUMinParameter,
                                      myUMaxParameter,
                                      aUPeriod,
                                      aNewU,
                                      du,
                                      aEps);
              solutionIsValid = Standard_True;
              bUCorrected     = Standard_True;
              U               = aNewU;
            }
          }
          //   if(solutionIsValid && VIsNotValid) {
          if (bUCorrected && VIsNotValid)
          {
            solutionIsValid = Standard_False;
            //
            if (mySurface.IsVPeriodic())
            {
              Standard_Real aNewV, aVPeriod, aEps, dv;
              //
              aVPeriod = mySurface.VPeriod();
              aEps     = Epsilon(aVPeriod);
              //
              GeomInt::AdjustPeriodic(V,
                                      myVMinParameter,
                                      myVMaxParameter,
                                      aVPeriod,
                                      aNewV,
                                      dv,
                                      aEps);
              solutionIsValid = Standard_True;
              V               = aNewV;
            }
          }
        }

        if (!solutionIsValid)
          continue;

        Standard_Integer aNbRanges = myRangeManager.Length();

        ComputeRangeFromStartPoint(Standard_False, aPoint.W(), U, V);
        ComputeRangeFromStartPoint(Standard_True, aPoint.W(), U, V);

        if (aNbRanges == myRangeManager.Length())
        {
          SetEmptyResultRange(aPoint.W(), myRangeManager);
        }
        else
        {
          myMinSqDistance = 0.0;
        }
      }
    }

    for (i = 1; i <= anExactIntersector.NbSegments(); i++)
    {
      const IntCurveSurface_IntersectionSegment& aSegment = anExactIntersector.Segment(i);
      IntCurveSurface_IntersectionPoint          aPoint1, aPoint2;
      aSegment.Values(aPoint1, aPoint2);

      Standard_Real aFirstParameter =
        (aPoint1.W() < myFirstParameter) ? myFirstParameter : aPoint1.W();
      Standard_Real aLastParameter =
        (myLastParameter < aPoint2.W()) ? myLastParameter : aPoint2.W();

      myRangeManager.InsertRange(aFirstParameter, aLastParameter, 2);

      ComputeRangeFromStartPoint(Standard_False, aPoint1.W(), aPoint1.U(), aPoint1.V());
      ComputeRangeFromStartPoint(Standard_True, aPoint2.W(), aPoint2.U(), aPoint2.V());
      myMinSqDistance = 0.0;
    }
  }
}

// ==================================================================================
// function: FastComputeExactIntersection
// purpose:
// ==================================================================================
Standard_Boolean IntTools_BeanFaceIntersector::FastComputeAnalytic()
{
  GeomAbs_CurveType aCT = myCurve.GetType();
  if (aCT == GeomAbs_BezierCurve || aCT == GeomAbs_BSplineCurve || aCT == GeomAbs_OffsetCurve
      || aCT == GeomAbs_OtherCurve)
  {
    // not supported type
    return Standard_False;
  }

  Standard_Boolean isCoincide      = Standard_False;
  Standard_Boolean hasIntersection = Standard_True;

  GeomAbs_SurfaceType aST = mySurface.GetType();

  // Plane - Circle/Ellipse/Hyperbola/Parabola
  if (aST == GeomAbs_Plane)
  {
    gp_Pln surfPlane = mySurface.Plane();

    gp_Dir aDir;
    gp_Pnt aPLoc;
    switch (aCT)
    {
      case GeomAbs_Circle: {
        aDir  = myCurve.Circle().Axis().Direction();
        aPLoc = myCurve.Circle().Location();
        break;
      }
      case GeomAbs_Ellipse: {
        aDir  = myCurve.Ellipse().Axis().Direction();
        aPLoc = myCurve.Ellipse().Location();
        break;
      }
      case GeomAbs_Hyperbola: {
        aDir  = myCurve.Hyperbola().Axis().Direction();
        aPLoc = myCurve.Hyperbola().Location();
        break;
      }
      case GeomAbs_Parabola: {
        aDir  = myCurve.Parabola().Axis().Direction();
        aPLoc = myCurve.Parabola().Location();
        break;
      }
      default:
        return Standard_False;
    }

    Standard_Real anAngle = aDir.Angle(surfPlane.Axis().Direction());
    if (anAngle > Precision::Angular())
      return Standard_False;

    hasIntersection = Standard_False;

    Standard_Real aDist = surfPlane.Distance(aPLoc);
    isCoincide          = aDist < myCriteria;
  }

  // Cylinder - Line/Circle
  else if (aST == GeomAbs_Cylinder)
  {
    gp_Cylinder   aCylinder  = mySurface.Cylinder();
    const gp_Ax1& aCylAxis   = aCylinder.Axis();
    const gp_Dir& aCylDir    = aCylAxis.Direction();
    Standard_Real aCylRadius = aCylinder.Radius();

    if (aCT == GeomAbs_Line)
    {
      gp_Lin aLin = myCurve.Line();
      if (!aLin.Direction().IsParallel(aCylDir, Precision::Angular()))
        return Standard_False;

      hasIntersection = Standard_False;

      Standard_Real aDist = Abs(aLin.Distance(aCylAxis.Location()) - aCylRadius);
      isCoincide          = (aDist < myCriteria);
    }

    else if (aCT == GeomAbs_Circle)
    {
      gp_Circ aCircle = myCurve.Circle();

      Standard_Real anAngle = aCylDir.Angle(aCircle.Axis().Direction());
      if (anAngle > Precision::Angular())
        return Standard_False;

      Standard_Real aDistLoc = gp_Lin(aCylAxis).Distance(aCircle.Location());
      Standard_Real aDist    = aDistLoc + Abs(aCircle.Radius() - aCylRadius);
      isCoincide             = (aDist < myCriteria);

      if (!isCoincide)
        hasIntersection = (aDistLoc - (aCircle.Radius() + aCylRadius)) < myCriteria
                          && (Abs(aCircle.Radius() - aCylRadius) - aDistLoc) < myCriteria;
    }
  }

  // Sphere - Line
  else if (aST == GeomAbs_Sphere)
  {
    gp_Sphere aSph    = mySurface.Sphere();
    gp_Pnt    aSphLoc = aSph.Location();
    if (aCT == GeomAbs_Line)
    {
      gp_Lin        aLin  = myCurve.Line();
      Standard_Real aDist = aLin.Distance(aSphLoc) - aSph.Radius();
      hasIntersection     = aDist < myCriteria;
    }
  }

  // Check intermediate point
  if (isCoincide)
  {
    myResults.Append(IntTools_Range(myFirstParameter, myLastParameter));
  }

  return isCoincide || !hasIntersection;
}

// ==================================================================================
// function: ComputeLinePlane
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeLinePlane()
{
  Standard_Real Tolang = 1.e-9;
  gp_Pln        P      = mySurface.Plane();
  gp_Lin        L      = myCurve.Line();

  myIsDone = Standard_True;

  Standard_Real A, B, C, D;
  Standard_Real Al, Bl, Cl;
  Standard_Real Dis, Direc;

  P.Coefficients(A, B, C, D);
  gp_Pnt Orig(L.Location());
  L.Direction().Coord(Al, Bl, Cl);

  Direc = A * Al + B * Bl + C * Cl;
  Dis   = A * Orig.X() + B * Orig.Y() + C * Orig.Z() + D;

  Standard_Boolean parallel = Standard_False, inplane = Standard_False;
  if (Abs(Direc) < Tolang)
  {
    parallel = Standard_True;
    if (Abs(Dis) < myCriteria)
    {
      inplane = Standard_True;
    }
    else
    {
      inplane = Standard_False;
    }
  }
  else
  {
    gp_Pnt        p1 = ElCLib::Value(myFirstParameter, L);
    gp_Pnt        p2 = ElCLib::Value(myLastParameter, L);
    Standard_Real d1 = A * p1.X() + B * p1.Y() + C * p1.Z() + D;
    if (d1 < 0)
      d1 = -d1;
    Standard_Real d2 = A * p2.X() + B * p2.Y() + C * p2.Z() + D;
    if (d2 < 0)
      d2 = -d2;
    if (d1 <= myCriteria && d2 <= myCriteria)
    {
      inplane = Standard_True;
    }
  }

  if (inplane)
  {
    IntTools_Range aRange(myFirstParameter, myLastParameter);
    myResults.Append(aRange);
    return;
  }

  if (parallel)
  {
    return;
  }

  Standard_Real t = -Dis / Direc;
  if (t < myFirstParameter || t > myLastParameter)
  {
    return;
  }

  gp_Pnt pint(Orig.X() + t * Al, Orig.Y() + t * Bl, Orig.Z() + t * Cl);

  Standard_Real u, v;
  ElSLib::Parameters(P, pint, u, v);
  if (myUMinParameter > u || u > myUMaxParameter || myVMinParameter > v || v > myVMaxParameter)
  {
    return;
  }
  //
  // compute correct range on the edge
  Standard_Real anAngle, aDt;
  gp_Dir        aDL, aDP;
  //
  aDL     = L.Position().Direction();
  aDP     = P.Position().Direction();
  anAngle = Abs(M_PI_2 - aDL.Angle(aDP));
  //
  aDt = IntTools_Tools::ComputeIntRange(myBeanTolerance, myFaceTolerance, anAngle);
  //
  Standard_Real  t1 = Max(myFirstParameter, t - aDt);
  Standard_Real  t2 = Min(myLastParameter, t + aDt);
  IntTools_Range aRange(t1, t2);
  myResults.Append(aRange);

  return;
}

// ==================================================================================
// function: ComputeUsingExtremum
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeUsingExtremum()
{
  Standard_Real Tol, af, al;
  Tol                        = Precision::PConfusion();
  Handle(Geom_Curve)  aCurve = BRep_Tool::Curve(myCurve.Edge(), af, al);
  GeomAdaptor_Surface aGASurface(myTrsfSurface,
                                 myUMinParameter,
                                 myUMaxParameter,
                                 myVMinParameter,
                                 myVMaxParameter);

  for (Standard_Integer i = 1; i <= myRangeManager.Length(); i++)
  {

    if (myRangeManager.Flag(i) > 0)
      continue;

    IntTools_Range aParamRange = myRangeManager.Range(i);
    Standard_Real  anarg1      = aParamRange.First();
    Standard_Real  anarg2      = aParamRange.Last();

    if (anarg2 - anarg1 < Precision::PConfusion())
    {

      if (((i > 1) && (myRangeManager.Flag(i - 1) == 2))
          || ((i < myRangeManager.Length()) && (myRangeManager.Flag(i + 1) == 2)))
      {
        myRangeManager.SetFlag(i, 1);
        continue;
      }
    }

    GeomAdaptor_Curve aGACurve(aCurve, anarg1, anarg2);
    Extrema_ExtCS     anExtCS;
    anExtCS.Initialize(aGASurface,
                       myUMinParameter,
                       myUMaxParameter,
                       myVMinParameter,
                       myVMaxParameter,
                       Tol,
                       Tol);
    Standard_Real first = aCurve->FirstParameter(), last = aCurve->LastParameter();
    if (aCurve->IsPeriodic()
        || (anarg1 >= first - Precision::PConfusion() && anarg2 <= last + Precision::PConfusion()))
    {
      // Extrema_ExtCS anExtCS (aGACurve, aGASurface, Tol, Tol);
      anExtCS.Perform(aGACurve, anarg1, anarg2);
    }

    if (anExtCS.IsDone() && (anExtCS.NbExt() || anExtCS.IsParallel()))
    {
      Standard_Integer anOldNbRanges = myRangeManager.Length();

      if (anExtCS.IsParallel())
      {
        const Standard_Real aSqDist = anExtCS.SquareDistance(1);
        myMinSqDistance             = Min(myMinSqDistance, aSqDist);
        if (aSqDist < myCriteria * myCriteria)
        {
          Standard_Real    U1, V1, U2, V2;
          Standard_Real    adistance1     = Distance(anarg1, U1, V1);
          Standard_Real    adistance2     = Distance(anarg2, U2, V2);
          Standard_Boolean validdistance1 = (adistance1 < myCriteria);
          Standard_Boolean validdistance2 = (adistance2 < myCriteria);

          if (validdistance1 && validdistance2)
          {
            myRangeManager.InsertRange(anarg1, anarg2, 2);
            continue;
          }
          else
          {
            if (validdistance1)
            {
              ComputeRangeFromStartPoint(Standard_True, anarg1, U1, V1);
            }
            else
            {
              if (validdistance2)
              {
                ComputeRangeFromStartPoint(Standard_False, anarg2, U2, V2);
              }
              else
              {
                Standard_Real    a         = anarg1;
                Standard_Real    b         = anarg2;
                Standard_Real    da        = adistance1;
                Standard_Real    db        = adistance2;
                Standard_Real    asolution = a;
                Standard_Boolean found     = Standard_False;

                while (((b - a) > myCurveResolution) && !found)
                {
                  asolution           = (a + b) * 0.5;
                  Standard_Real adist = Distance(asolution, U1, V1);

                  if (adist < myCriteria)
                  {
                    found = Standard_True;
                  }
                  else
                  {
                    if (da < db)
                    {
                      b  = asolution;
                      db = adist;
                    }
                    else
                    {
                      a  = asolution;
                      da = adist;
                    }
                  }
                } // end while

                if (found)
                {
                  ComputeRangeFromStartPoint(Standard_False, asolution, U1, V1);
                  ComputeRangeFromStartPoint(Standard_True, asolution, U1, V1);
                }
                else
                {
                  myRangeManager.SetFlag(i, 1);
                }
              }
            }
          }
        }
        else
        {
          myRangeManager.SetFlag(i, 1);
        }
      }
      else
      {
        Standard_Boolean solutionfound = Standard_False;

        for (Standard_Integer j = 1; j <= anExtCS.NbExt(); j++)
        {
          if (anExtCS.SquareDistance(j) < myCriteria * myCriteria)
          {
            Extrema_POnCurv p1;
            Extrema_POnSurf p2;
            anExtCS.Points(j, p1, p2);
            Standard_Real U, V;
            p2.Parameter(U, V);

            Standard_Integer aNbRanges = myRangeManager.Length();
            ComputeRangeFromStartPoint(Standard_False, p1.Parameter(), U, V);
            ComputeRangeFromStartPoint(Standard_True, p1.Parameter(), U, V);
            solutionfound = Standard_True;

            if (aNbRanges == myRangeManager.Length())
            {
              SetEmptyResultRange(p1.Parameter(), myRangeManager);
            }
          }

          myMinSqDistance = Min(myMinSqDistance, anExtCS.SquareDistance(j));
        }

        if (!solutionfound)
        {
          myRangeManager.SetFlag(i, 1);
        }
      }
      Standard_Integer adifference = myRangeManager.Length() - anOldNbRanges;

      if (adifference > 0)
      {
        i += adifference;
      }
    }
  }
}

// ==================================================================================
// function: ComputeNearRangeBoundaries
// purpose:
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeNearRangeBoundaries()
{
  Standard_Real U = myUMinParameter;
  Standard_Real V = myVMinParameter;

  for (Standard_Integer i = 1; i <= myRangeManager.Length(); i++)
  {

    if (myRangeManager.Flag(i) > 0)
      continue;

    if ((i > 1) && (myRangeManager.Flag(i - 1) > 0))
      continue;

    IntTools_Range aParamRange = myRangeManager.Range(i);

    if (Distance(aParamRange.First(), U, V) < myCriteria)
    {
      Standard_Integer aNbRanges = myRangeManager.Length();

      if (i > 1)
      {
        ComputeRangeFromStartPoint(Standard_False, aParamRange.First(), U, V, i - 1);
      }
      ComputeRangeFromStartPoint(Standard_True,
                                 aParamRange.First(),
                                 U,
                                 V,
                                 i + (myRangeManager.Length() - aNbRanges));

      if (aNbRanges == myRangeManager.Length())
      {
        SetEmptyResultRange(aParamRange.First(), myRangeManager);
      }
    }
  }

  if (myRangeManager.Flag(myRangeManager.Length()) == 0)
  {
    IntTools_Range aParamRange = myRangeManager.Range(myRangeManager.Length());

    if (Distance(aParamRange.Last(), U, V) < myCriteria)
    {
      Standard_Integer aNbRanges = myRangeManager.Length();

      ComputeRangeFromStartPoint(Standard_False, aParamRange.Last(), U, V, myRangeManager.Length());

      if (aNbRanges == myRangeManager.Length())
      {
        SetEmptyResultRange(aParamRange.Last(), myRangeManager);
      }
    }
  }
}

// ==================================================================================
// function: ComputeRangeFromStartPoint
// purpose:  Compute range using start point according to parameter theParameter,
//           increasing parameter on curve if ToIncreaseParameter == Standard_True or
//           decreasing parameter on curve if ToIncreaseParameter == Standard_False
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeRangeFromStartPoint(
  const Standard_Boolean ToIncreaseParameter,
  const Standard_Real    theParameter,
  const Standard_Real    theUParameter,
  const Standard_Real    theVParameter)
{
  Standard_Integer aFoundIndex = myRangeManager.GetIndex(theParameter, ToIncreaseParameter);

  if (aFoundIndex == 0)
  {
    return;
  }

  ComputeRangeFromStartPoint(ToIncreaseParameter,
                             theParameter,
                             theUParameter,
                             theVParameter,
                             aFoundIndex);
}

// ==================================================================================
// function: ComputeRangeFromStartPoint
// purpose:  Compute range using start point according to parameter theParameter,
//           increasing parameter on curve if ToIncreaseParameter == Standard_True or
//           decreasing parameter on curve if ToIncreaseParameter == Standard_False.
//           theIndex indicate that theParameter belong the range number theIndex.
// ==================================================================================
void IntTools_BeanFaceIntersector::ComputeRangeFromStartPoint(
  const Standard_Boolean ToIncreaseParameter,
  const Standard_Real    theParameter,
  const Standard_Real    theUParameter,
  const Standard_Real    theVParameter,
  const Standard_Integer theIndex)
{
  if (myRangeManager.Flag(theIndex) > 0)
    return;

  Standard_Integer aValidIndex = theIndex;

  Standard_Real aMinDelta        = myCurveResolution * 0.5;
  Standard_Real aDeltaRestrictor = 0.1 * (myLastParameter - myFirstParameter);

  if (aMinDelta > aDeltaRestrictor)
    aMinDelta = aDeltaRestrictor * 0.5;

  Standard_Real tenOfMinDelta = aMinDelta * 10.;
  Standard_Real aDelta        = myCurveResolution;

  Standard_Real aCurPar = (ToIncreaseParameter) ? (theParameter + aDelta) : (theParameter - aDelta);
  Standard_Real aPrevPar       = theParameter;
  IntTools_Range aCurrentRange = myRangeManager.Range(aValidIndex);

  Standard_Boolean BoundaryCondition =
    (ToIncreaseParameter) ? (aCurPar > aCurrentRange.Last()) : (aCurPar < aCurrentRange.First());

  if (BoundaryCondition)
  {
    aCurPar           = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
    BoundaryCondition = Standard_False;
  }

  Standard_Integer loopcounter          = 0; // necessary as infinite loop restricter
  Standard_Real    U                    = theUParameter;
  Standard_Real    V                    = theVParameter;
  Standard_Boolean anotherSolutionFound = Standard_False;

  Standard_Boolean isboundaryindex = Standard_False;
  Standard_Boolean isvalidindex    = Standard_True;

  while ((aDelta >= aMinDelta) && (loopcounter <= 10))
  {
    Standard_Boolean pointfound = Standard_False;

    //
    gp_Pnt                 aPoint = myCurve.Value(aCurPar);
    Extrema_GenLocateExtPS anExtrema(mySurface, 1.e-10, 1.e-10);
    anExtrema.Perform(aPoint, U, V);

    if (anExtrema.IsDone())
    {
      if (anExtrema.SquareDistance() < myCriteria * myCriteria)
      {
        Extrema_POnSurf aPOnSurf = anExtrema.Point();
        aPOnSurf.Parameter(U, V);
        pointfound = Standard_True;
      }
    }
    else
    {
      pointfound = (Distance(aCurPar) < myCriteria);
    }

    if (pointfound)
    {
      aPrevPar             = aCurPar;
      anotherSolutionFound = Standard_True;

      if (BoundaryCondition && (isboundaryindex || !isvalidindex))
        break;
    }
    else
    {
      aDeltaRestrictor = aDelta;
    }

    // if point found decide to increase aDelta using derivative of distance function
    //

    aDelta = (pointfound) ? (aDelta * 2.) : (aDelta * 0.5);
    aDelta = (aDelta < aDeltaRestrictor) ? aDelta : aDeltaRestrictor;

    aCurPar = (ToIncreaseParameter) ? (aPrevPar + aDelta) : (aPrevPar - aDelta);

    // prevent infinite loop when (aPrevPar +/- aDelta) == aPrevPar == 0.
    //

    if (aCurPar == aPrevPar)
      break;

    BoundaryCondition =
      (ToIncreaseParameter) ? (aCurPar > aCurrentRange.Last()) : (aCurPar < aCurrentRange.First());

    isboundaryindex = Standard_False;
    isvalidindex    = Standard_True;

    if (BoundaryCondition)
    {
      isboundaryindex = ((!ToIncreaseParameter && (aValidIndex == 1))
                         || (ToIncreaseParameter && (aValidIndex == myRangeManager.Length())));

      if (!isboundaryindex)
      {

        if (pointfound)
        {
          Standard_Integer aFlag = (ToIncreaseParameter) ? myRangeManager.Flag(aValidIndex + 1)
                                                         : myRangeManager.Flag(aValidIndex - 1);

          if (aFlag == 0)
          {
            aValidIndex   = (ToIncreaseParameter) ? (aValidIndex + 1) : (aValidIndex - 1);
            aCurrentRange = myRangeManager.Range(aValidIndex);

            if ((ToIncreaseParameter && (aCurPar > aCurrentRange.Last()))
                || (!ToIncreaseParameter && (aCurPar < aCurrentRange.First())))
            {
              aCurPar = (aCurrentRange.First() + aCurrentRange.Last()) * 0.5;
              aDelta *= 0.5;
            }
          }
          else
          {
            isvalidindex = Standard_False;
            aCurPar      = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
          }
        }
      }
      else
      {
        aCurPar = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
      }

      if (aDelta < tenOfMinDelta)
      {
        loopcounter++;
      }
      else
      {
        loopcounter = 0;
      }
    } // if(BoundaryCondition)
  }

  if (anotherSolutionFound)
  {
    if (ToIncreaseParameter)
      myRangeManager.InsertRange(theParameter, aPrevPar, 2);
    else
      myRangeManager.InsertRange(aPrevPar, theParameter, 2);
  }
}

// ---------------------------------------------------------------------------------
// static function: SetEmptyResultRange
// purpose:
// ---------------------------------------------------------------------------------
static Standard_Boolean SetEmptyResultRange(const Standard_Real      theParameter,
                                            IntTools_MarkedRangeSet& theMarkedRange)
{

  const TColStd_SequenceOfInteger& anIndices = theMarkedRange.GetIndices(theParameter);
  Standard_Boolean                 add       = (anIndices.Length() > 0);

  for (Standard_Integer k = 1; k <= anIndices.Length(); k++)
  {
    if (theMarkedRange.Flag(anIndices(k)) == 2)
    {
      add = Standard_False;
      break;
    }
  }

  if (add)
  {
    theMarkedRange.InsertRange(theParameter, theParameter, 2);
  }

  return add;
}

// ---------------------------------------------------------------------------------
// static function: TestCoinside
// purpose:
// ---------------------------------------------------------------------------------
// static Standard_Boolean TestClose(const Extrema_ExtPS & theExt,
//                                   const Standard_Real   theDist)
// {
//   Standard_Boolean close = Standard_False;
//   if(!theExt.IsDone() || theExt.NbExt() == 0)
//     return close;
//   else {
//     Standard_Integer ie;
//     for(ie = 1; ie <= theExt.NbExt(); ie++) {
//       Standard_Real dist = theExt.Value(ie);
//       if(dist <= theDist) {
//         close = Standard_True;
//         break;
//       }
//     }
//   }
//   return close;
// }

// Standard_Boolean TestCoinside(const BRepAdaptor_Curve&   theCurve,
//                               const BRepAdaptor_Surface& theSurface)
// {
//   Standard_Real cfp = theCurve.FirstParameter(), clp = theCurve.LastParameter();
//   Standard_Real cdp = fabs(clp - cfp) / 23.;

//   Standard_Integer i = 0;
//   Standard_Real tolE = theCurve.Tolerance(), tolF = theSurface.Tolerance();
//   Standard_Real tolT = tolE + tolF, tolU = 1.e-9, tolV = 1.e-9;
//   gp_Pnt aP;

//   theCurve.D0(cfp,aP);
//   Extrema_ExtPS eps(aP,theSurface,tolU,tolV);

//   if(!TestClose(eps,tolT))
//     return Standard_False;

//   theCurve.D0(clp,aP);
//   eps.Perform(aP);

//   if(!TestClose(eps,tolT))
//     return Standard_False;

//   Standard_Boolean close = Standard_True;

//   for(i = 1; i <= 22; i++) {
//     theCurve.D0((cfp+((Standard_Real)i)*cdp),aP);
//     eps.Perform(aP);
//     if(!TestClose(eps,tolT)) {
//       close = Standard_False;
//       break;
//     }
//   }
//   return close;
// }

// ======================================================================================================================
// function: LocalizeSolutions
// purpose:
// ======================================================================================================================
Standard_Boolean IntTools_BeanFaceIntersector::LocalizeSolutions(
  const IntTools_CurveRangeSample&   theCurveRange,
  const Bnd_Box&                     theBoxCurve,
  const IntTools_SurfaceRangeSample& theSurfaceRange,
  const Bnd_Box&                     theBoxSurface,
  IntTools_CurveRangeLocalizeData&   theCurveData,
  IntTools_SurfaceRangeLocalizeData& theSurfaceData,
  IntTools_ListOfCurveRangeSample&   theListCurveRange,
  IntTools_ListOfSurfaceRangeSample& theListSurfaceRange)
{
  Standard_Integer tIt = 0, uIt = 0, vIt = 0;

  //
  IntTools_CurveRangeSample aRootRangeC(0);
  aRootRangeC.SetDepth(0);
  IntTools_SurfaceRangeSample aRootRangeS(0, 0, 0, 0);

  Bnd_Box          aMainBoxC      = theBoxCurve;
  Bnd_Box          aMainBoxS      = theBoxSurface;
  Standard_Boolean bMainBoxFoundS = Standard_False;
  Standard_Boolean bMainBoxFoundC = Standard_False;
  //
  IntTools_ListOfCurveRangeSample   aListCurveRangeFound;
  IntTools_ListOfSurfaceRangeSample aListSurfaceRangeFound;

  IntTools_Range aRangeC =
    theCurveRange.GetRange(myFirstParameter, myLastParameter, theCurveData.GetNbSample());
  Standard_Real localdiffC = (aRangeC.Last() - aRangeC.First()) / theCurveData.GetNbSample();

  Standard_Real    aCurPar       = aRangeC.First();
  Standard_Real    aPrevPar      = aRangeC.First();
  Standard_Integer aCurIndexInit = theCurveRange.GetRangeIndexDeeper(theCurveData.GetNbSample());

  TColStd_ListOfInteger aListCToAvoid;
  Standard_Boolean      bGlobalCheckDone = Standard_False;
  //

  //

  Standard_Integer aCurIndexU = theSurfaceRange.GetRangeIndexUDeeper(theSurfaceData.GetNbSampleU());

  Standard_Integer aCurIndexVInit =
    theSurfaceRange.GetRangeIndexVDeeper(theSurfaceData.GetNbSampleV());
  IntTools_Range aRangeV =
    theSurfaceRange.GetRangeV(myVMinParameter, myVMaxParameter, theSurfaceData.GetNbSampleV());

  //
  IntTools_Range aRangeU =
    theSurfaceRange.GetRangeU(myUMinParameter, myUMaxParameter, theSurfaceData.GetNbSampleU());
  Standard_Real aCurParU    = aRangeU.First();
  Standard_Real aLocalDiffU = (aRangeU.Last() - aRangeU.First()) / theSurfaceData.GetNbSampleU();

  Standard_Real aPrevParU   = aCurParU;
  Standard_Real aLocalDiffV = (aRangeV.Last() - aRangeV.First()) / theSurfaceData.GetNbSampleV();

  // ranges check.begin
  Standard_Boolean bAllowSamplingC = Standard_True;
  Standard_Boolean bAllowSamplingU = Standard_True;
  Standard_Boolean bAllowSamplingV = Standard_True;

  // check
  CheckSampling(theCurveRange,
                theSurfaceRange,
                theCurveData,
                theSurfaceData,
                localdiffC,
                aLocalDiffU,
                aLocalDiffV,
                bAllowSamplingC,
                bAllowSamplingU,
                bAllowSamplingV);
  //

  if (!bAllowSamplingC && !bAllowSamplingU && !bAllowSamplingV)
  {
    theListCurveRange.Append(theCurveRange);
    theListSurfaceRange.Append(theSurfaceRange);
    return Standard_True;
  }
  // ranges check.end

  // init template. begin
  IntTools_CurveRangeSample aNewRangeCTemplate;

  if (!bAllowSamplingC)
  {
    aNewRangeCTemplate = theCurveRange;
    aCurIndexInit      = theCurveRange.GetRangeIndex();
    localdiffC         = (aRangeC.Last() - aRangeC.First());
  }
  else
  {
    aNewRangeCTemplate.SetDepth(theCurveRange.GetDepth() + 1);
    aNewRangeCTemplate.SetRangeIndex(aCurIndexInit);
  }

  IntTools_SurfaceRangeSample aNewRangeSTemplate = theSurfaceRange;

  if (bAllowSamplingU)
  {
    aNewRangeSTemplate.SetDepthU(theSurfaceRange.GetDepthU() + 1);
  }
  else
  {
    aCurIndexU  = aNewRangeSTemplate.GetIndexU();
    aLocalDiffU = aRangeU.Last() - aRangeU.First();
  }

  if (bAllowSamplingV)
  {
    aNewRangeSTemplate.SetDepthV(theSurfaceRange.GetDepthV() + 1);
  }
  else
  {
    aCurIndexVInit = theSurfaceRange.GetIndexV();
    aLocalDiffV    = aRangeV.Last() - aRangeV.First();
  }
  // init template. end

  Standard_Boolean       bHasOut = Standard_False;
  const Standard_Integer nbU     = (bAllowSamplingU) ? theSurfaceData.GetNbSampleU() : 1;
  const Standard_Integer nbV     = (bAllowSamplingV) ? theSurfaceData.GetNbSampleV() : 1;
  const Standard_Integer nbC     = (bAllowSamplingC) ? theCurveData.GetNbSample() : 1;

  for (uIt = 1; uIt <= nbU; uIt++, aCurIndexU++, aPrevParU = aCurParU)
  {
    aCurParU += aLocalDiffU;

    Standard_Real    aCurParV   = aRangeV.First();
    Standard_Real    aPrevParV  = aCurParV;
    Standard_Integer aCurIndexV = aCurIndexVInit;

    Standard_Boolean bHasOutV = Standard_False;

    // ///////
    for (vIt = 1; vIt <= nbV; vIt++, aCurIndexV++, aPrevParV = aCurParV)
    {

      aCurParV += aLocalDiffV;

      // //////////////
      //
      IntTools_SurfaceRangeSample aNewRangeS = aNewRangeSTemplate;

      if (bAllowSamplingU)
      {
        aNewRangeS.SetIndexU(aCurIndexU);
      }

      if (bAllowSamplingV)
      {
        aNewRangeS.SetIndexV(aCurIndexV);
      }

      if (theSurfaceData.IsRangeOut(aNewRangeS))
      {
        bHasOutV = Standard_True;
        continue;
      }

      // ///////

      Bnd_Box aBoxS;

      if (!theSurfaceData.FindBox(aNewRangeS, aBoxS))
      {

        if (mySurface.GetType() == GeomAbs_BSplineSurface)
        {
          // 	if(Standard_False ) {
          Handle(Geom_BSplineSurface) aSurfBspl =
            Handle(Geom_BSplineSurface)::DownCast(myTrsfSurface);
          aBoxS = GetSurfaceBox(aSurfBspl,
                                aPrevParU,
                                aCurParU,
                                aPrevParV,
                                aCurParV,
                                myCriteria,
                                theSurfaceData);
        }
        else
        {
          BndLib_AddSurface::Add(mySurface,
                                 aPrevParU,
                                 aCurParU,
                                 aPrevParV,
                                 aCurParV,
                                 myCriteria,
                                 aBoxS);
        }
        // 	Bnd_Box aMainBoxC;

        if (!bMainBoxFoundC && theCurveData.FindBox(aRootRangeC, aMainBoxC))
        {
          bMainBoxFoundC = Standard_True;
        }

        if (aBoxS.IsOut(aMainBoxC))
        {
          theSurfaceData.AddOutRange(aNewRangeS);
          bHasOutV = Standard_True;
          continue;
        }
        // 	}
        theSurfaceData.AddBox(aNewRangeS, aBoxS);
      }

      if (aBoxS.IsOut(theBoxCurve))
      {
        bHasOutV = Standard_True;
        continue;
      }

      IntTools_ListOfBox    aListOfBox;
      TColStd_ListOfInteger aListOfIndex;

      Standard_Boolean bHasOutC  = Standard_False;
      Standard_Integer aCurIndex = aCurIndexInit;

      // ////////////////////////////
      aCurPar                              = aRangeC.First();
      aPrevPar                             = aRangeC.First();
      IntTools_CurveRangeSample aCurRangeC = aNewRangeCTemplate;

      for (tIt = 1; tIt <= nbC; tIt++, aCurIndex++, aPrevPar = aCurPar)
      {

        aCurPar += localdiffC;

        // ignore already computed. begin
        Standard_Boolean                    bFound = Standard_False;
        TColStd_ListIteratorOfListOfInteger anItToAvoid(aListCToAvoid);

        for (; anItToAvoid.More(); anItToAvoid.Next())
        {
          if (tIt == anItToAvoid.Value())
          {
            bFound = Standard_True;
            break;
          }
        }

        if (!bFound)
        {
          if (bAllowSamplingC)
          {
            aCurRangeC.SetRangeIndex(aCurIndex);
          }
          bFound = theCurveData.IsRangeOut(aCurRangeC);
        }

        if (bFound)
        {
          bHasOutC = Standard_True;
          continue;
        }
        // ignore already computed. end

        // compute Box
        Bnd_Box aBoxC;

        if (!theCurveData.FindBox(aCurRangeC, aBoxC))
        {
          BndLib_Add3dCurve::Add(myCurve, aPrevPar, aCurPar, myCriteria, aBoxC);

          //   Bnd_Box aMainBoxS;

          if (!bMainBoxFoundS && theSurfaceData.FindBox(aRootRangeS, aMainBoxS))
          {
            bMainBoxFoundS = Standard_True;
          }
          if (aBoxC.IsOut(aMainBoxS))
          {
            theCurveData.AddOutRange(aCurRangeC);
            bHasOutC = Standard_True;
            continue;
          }
          //   }
          theCurveData.AddBox(aCurRangeC, aBoxC);
        }

        if (!bGlobalCheckDone && aBoxC.IsOut(theBoxSurface))
        {
          aListCToAvoid.Append(tIt);
          bHasOutC = Standard_True;
          continue;
        }

        if (aBoxC.IsOut(aBoxS))
        {
          bHasOutV = Standard_True;
          bHasOutC = Standard_True;
          continue;
        }
        //

        aListOfIndex.Append(tIt);
        aListOfBox.Append(aBoxC);
      } // end for(tIt...)

      bGlobalCheckDone = Standard_True;

      if (bHasOutC)
      {
        bHasOutV = Standard_True;
      }

      // //////////////
      //

      IntTools_CurveRangeSample aNewRangeC = aNewRangeCTemplate;

      aCurIndex = aCurIndexInit;
      TColStd_ListIteratorOfListOfInteger anItI(aListOfIndex);
      IntTools_ListIteratorOfListOfBox    anItBox(aListOfBox);
      Standard_Boolean                    bUseOldC   = Standard_False;
      Standard_Boolean                    bUseOldS   = Standard_False;
      Standard_Boolean                    bCheckSize = !bHasOutC;

      for (; anItI.More() && anItBox.More(); anItI.Next(), anItBox.Next())
      {
        aCurIndex = aCurIndexInit + anItI.Value() - 1;

        bUseOldS = Standard_False;

        if (bAllowSamplingC)
        {
          aNewRangeC.SetRangeIndex(aCurIndex);
        }

        if (bCheckSize)
        {

          if ((theCurveRange.GetDepth() == 0) || (theSurfaceRange.GetDepthU() == 0)
              || (theSurfaceRange.GetDepthV() == 0))
          {
            bHasOutC = Standard_True;
            bHasOutV = Standard_True;
          }
          else if ((theCurveRange.GetDepth() < 4) && (theSurfaceRange.GetDepthU() < 4)
                   && (theSurfaceRange.GetDepthV() < 4))
          {
            Bnd_Box aBoxC = anItBox.Value();

            if (!aBoxC.IsWhole() && !aBoxS.IsWhole())
            {
              Standard_Real aDiagC = aBoxC.SquareExtent();
              Standard_Real aDiagS = aBoxS.SquareExtent();

              if (aDiagC < aDiagS)
              {
                if ((aDiagC * 10.) < aDiagS)
                {
                  bUseOldC = Standard_True;
                  bHasOutC = Standard_True;
                  bHasOutV = Standard_True;
                  break;
                }
              }
              else
              {
                if ((aDiagS * 10.) < aDiagC)
                {
                  bUseOldS = Standard_True;
                  bHasOutC = Standard_True;
                  bHasOutV = Standard_True;
                }
              }
            }
          }
        }

        if (!bHasOutC)
        {
          aListCurveRangeFound.Append(aNewRangeC);
          aListSurfaceRangeFound.Append(aNewRangeS);
        }
        else
        {

          //   if(bUseOldS || bAllowSamplingU || bAllowSamplingV) {
          //     theSurfaceData.AddBox(aNewRangeS, aBoxS);
          //   }

          if (bUseOldS && aNewRangeC.IsEqual(theCurveRange))
          {
            return Standard_False;
          }

          if (!LocalizeSolutions(aNewRangeC,
                                 anItBox.Value(),
                                 ((bUseOldS) ? theSurfaceRange : aNewRangeS),
                                 ((bUseOldS) ? theBoxSurface : aBoxS),
                                 theCurveData,
                                 theSurfaceData,
                                 theListCurveRange,
                                 theListSurfaceRange))
            return Standard_False;
        }
      }
      // end (tIt...)
      aListOfIndex.Clear();
      aListOfBox.Clear();

      if (bHasOutV)
      {
        // 	theSurfaceData.AddBox(aNewRangeS, aBoxS);

        if (bUseOldC && bAllowSamplingC && (bAllowSamplingU || bAllowSamplingV))
        {
          if (!LocalizeSolutions(theCurveRange,
                                 theBoxCurve,
                                 aNewRangeS,
                                 aBoxS,
                                 theCurveData,
                                 theSurfaceData,
                                 theListCurveRange,
                                 theListSurfaceRange))
            return Standard_False;
        }
      }
    } // end for (vIt...)

    if (bHasOutV)
    {
      bHasOut = Standard_True;
    }
  }

  if (!bHasOut)
  {
    theListCurveRange.Append(theCurveRange);
    theListSurfaceRange.Append(theSurfaceRange);
  }
  else
  {
    IntTools_ListIteratorOfListOfCurveRangeSample   anIt1(aListCurveRangeFound);
    IntTools_ListIteratorOfListOfSurfaceRangeSample anIt2(aListSurfaceRangeFound);

    for (; anIt1.More() && anIt2.More(); anIt1.Next(), anIt2.Next())
    {
      theListCurveRange.Append(anIt1.Value());
      theListSurfaceRange.Append(anIt2.Value());
    }
  }
  return Standard_True;
}

// ======================================================================================================================
// function: ComputeLocalized
// purpose:
// ======================================================================================================================
Standard_Boolean IntTools_BeanFaceIntersector::ComputeLocalized()
{
  Standard_Real Tol = Precision::PConfusion();

  IntTools_SurfaceRangeSample        aSurfaceRange(0, 0, 0, 0);
  Standard_Real                      dMinU = 10. * Precision::PConfusion();
  Standard_Real                      dMinV = dMinU;
  IntTools_SurfaceRangeLocalizeData  aSurfaceDataInit(3, 3, dMinU, dMinV);
  IntTools_SurfaceRangeLocalizeData& aSurfaceData = myContext->SurfaceData(mySurface.Face());
  aSurfaceData.RemoveRangeOutAll();
  aSurfaceData.ClearGrid();

  Bnd_Box          FBox;
  Standard_Boolean bFBoxFound = aSurfaceData.FindBox(aSurfaceRange, FBox);

  if (mySurface.GetType() == GeomAbs_BSplineSurface)
  {
    Handle(Geom_BSplineSurface) aSurfBspl = Handle(Geom_BSplineSurface)::DownCast(myTrsfSurface);

    ComputeGridPoints(aSurfBspl,
                      myUMinParameter,
                      myUMaxParameter,
                      myVMinParameter,
                      myVMaxParameter,
                      myFaceTolerance,
                      aSurfaceData);

    if (!bFBoxFound)
    {
      FBox = GetSurfaceBox(aSurfBspl,
                           myUMinParameter,
                           myUMaxParameter,
                           myVMinParameter,
                           myVMaxParameter,
                           myCriteria,
                           aSurfaceData);
      aSurfaceData.AddBox(aSurfaceRange, FBox);
    }
  }
  else if (!bFBoxFound)
  {

    BndLib_AddSurface::Add(mySurface,
                           myUMinParameter,
                           myUMaxParameter,
                           myVMinParameter,
                           myVMaxParameter,
                           myFaceTolerance,
                           FBox);
    aSurfaceData.AddBox(aSurfaceRange, FBox);
  }

  Bnd_Box EBox;

  BndLib_Add3dCurve::Add(*myCurve.Trim(myFirstParameter, myLastParameter, Precision::PConfusion()),
                         myBeanTolerance,
                         EBox);

  if (EBox.IsOut(FBox))
  {
    for (Standard_Integer i = 1; i <= myRangeManager.Length(); i++)
    {
      myRangeManager.SetFlag(i, 1);
    }
    aSurfaceData.ClearGrid();

    return Standard_True;
  }

  IntTools_ListOfCurveRangeSample   aListCurveRange;
  IntTools_ListOfSurfaceRangeSample aListSurfaceRange;

  IntTools_CurveRangeSample aCurveRange(0);
  aCurveRange.SetDepth(0);
  Standard_Integer                nbSampleC = 3;
  Standard_Integer                nbSampleU = aSurfaceData.GetNbSampleU();
  Standard_Integer                nbSampleV = aSurfaceData.GetNbSampleV();
  Standard_Real                   dMinC     = 10. * myCurveResolution;
  IntTools_ListOfCurveRangeSample aListOut;

  // check
  Standard_Boolean                  bAllowSamplingC = Standard_True;
  Standard_Boolean                  bAllowSamplingU = Standard_True;
  Standard_Boolean                  bAllowSamplingV = Standard_True;
  IntTools_CurveRangeLocalizeData   aCurveDataTmp(nbSampleC, dMinC);
  IntTools_SurfaceRangeLocalizeData aSurfaceDataTmp(nbSampleU, nbSampleV, dMinU, dMinV);

  CheckSampling(aCurveRange,
                aSurfaceRange,
                aCurveDataTmp,
                aSurfaceDataTmp,
                myLastParameter - myFirstParameter,
                myUMaxParameter - myUMinParameter,
                myVMaxParameter - myVMinParameter,
                bAllowSamplingC,
                bAllowSamplingU,
                bAllowSamplingV);

  {
    IntTools_CurveRangeLocalizeData aCurveData(nbSampleC, dMinC);

    aCurveData.AddBox(aCurveRange, EBox);

    if (!LocalizeSolutions(aCurveRange,
                           EBox,
                           aSurfaceRange,
                           FBox,
                           aCurveData,
                           aSurfaceData,
                           aListCurveRange,
                           aListSurfaceRange))
    {
      aSurfaceData.ClearGrid();

      return Standard_False;
    }
    IntTools_ListOfCurveRangeSample   aListCurveRangeSort;
    IntTools_ListOfSurfaceRangeSample aListSurfaceRangeSort;

    MergeSolutions(aListCurveRange, aListSurfaceRange, aListCurveRangeSort, aListSurfaceRangeSort);

    IntTools_ListIteratorOfListOfCurveRangeSample   anItC(aListCurveRangeSort);
    IntTools_ListIteratorOfListOfSurfaceRangeSample anItS(aListSurfaceRangeSort);
    IntTools_SurfaceRangeSample                     aRangeSPrev;

    Extrema_GenExtCS anExtremaGen;

    for (; anItC.More() && anItS.More(); anItC.Next(), anItS.Next())
    {

      IntTools_Range aRangeC(myFirstParameter, myLastParameter);

      if (bAllowSamplingC)
        aRangeC = anItC.Value().GetRange(myFirstParameter, myLastParameter, nbSampleC);

      IntTools_Range aRangeU(myUMinParameter, myUMaxParameter);

      if (bAllowSamplingU)
        aRangeU = anItS.Value().GetRangeU(myUMinParameter, myUMaxParameter, nbSampleU);

      IntTools_Range aRangeV(myVMinParameter, myVMaxParameter);

      if (bAllowSamplingV)
        aRangeV = anItS.Value().GetRangeV(myVMinParameter, myVMaxParameter, nbSampleV);

      Standard_Real anarg1 = aRangeC.First(), anarg2 = aRangeC.Last();

      Standard_Boolean bFound = Standard_False;

      Standard_Integer                 nMinIndex = myRangeManager.Length();
      Standard_Integer                 nMaxIndex = -1;
      const TColStd_SequenceOfInteger& anInds1   = myRangeManager.GetIndices(anarg1);
      Standard_Integer                 indIt     = 1;

      for (indIt = 1; indIt <= anInds1.Length(); indIt++)
      {
        Standard_Integer nIndex = anInds1.Value(indIt);
        nMinIndex               = (nMinIndex > nIndex) ? nIndex : nMinIndex;
        nMaxIndex               = (nMaxIndex < nIndex) ? nIndex : nMaxIndex;
      }

      for (indIt = nMinIndex; indIt <= nMaxIndex; indIt++)
      {
        if (myRangeManager.Flag(indIt) == 2)
        {
          bFound = Standard_True;
          break;
        }
      }

      if (bFound)
        continue;
      nMinIndex                                = (nMaxIndex >= 0) ? nMaxIndex : nMinIndex;
      const TColStd_SequenceOfInteger& anInds2 = myRangeManager.GetIndices(anarg2);

      for (indIt = 1; indIt <= anInds2.Length(); indIt++)
      {
        Standard_Integer nIndex = anInds2.Value(indIt);
        nMinIndex               = (nMinIndex > nIndex) ? nIndex : nMinIndex;
        nMaxIndex               = (nMaxIndex < nIndex) ? nIndex : nMaxIndex;
      }

      for (indIt = nMinIndex; indIt <= nMaxIndex; indIt++)
      {
        if (myRangeManager.Flag(indIt) == 2)
        {
          bFound = Standard_True;
          break;
        }
      }

      if (bFound)
        continue;

      Standard_Real parUF = aRangeU.First(), parUL = aRangeU.Last();
      Standard_Real parVF = aRangeV.First(), parVL = aRangeV.Last();

      if (aRangeSPrev.IsEqual(anItS.Value()))
      {
        anExtremaGen.Perform(myCurve, 10, anarg1, anarg2, Tol);
      }
      else
      {
        anExtremaGen.Initialize(mySurface, 10, 10, parUF, parUL, parVF, parVL, Tol);
        anExtremaGen.Perform(myCurve, 10, anarg1, anarg2, Tol);
      }

      if (anExtremaGen.IsDone() && (anExtremaGen.NbExt() > 0))
      {

        for (Standard_Integer j = 1; j <= anExtremaGen.NbExt(); j++)
        {

          if (anExtremaGen.SquareDistance(j) < myCriteria * myCriteria)
          {

            Extrema_POnCurv p1;
            Extrema_POnSurf p2;
            p1 = anExtremaGen.PointOnCurve(j);
            p2 = anExtremaGen.PointOnSurface(j);
            Standard_Real U, V, T;
            T = p1.Parameter();
            p2.Parameter(U, V);

            if (myCurve.IsPeriodic())
              T = ElCLib::InPeriod(T, anarg1, anarg1 + myCurve.Period());
            if (mySurface.IsUPeriodic())
              U = ElCLib::InPeriod(U, parUF, parUF + mySurface.UPeriod());
            if (mySurface.IsVPeriodic())
              V = ElCLib::InPeriod(V, parVF, parVF + mySurface.VPeriod());

            // To avoid occasional going out of boundaries because of numerical
            // problem
            if (U < myUMinParameter)
              U = myUMinParameter;
            if (U > myUMaxParameter)
              U = myUMaxParameter;
            if (V < myVMinParameter)
              V = myVMinParameter;
            if (V > myVMaxParameter)
              V = myVMaxParameter;

            Standard_Integer aNbRanges = myRangeManager.Length();
            ComputeRangeFromStartPoint(Standard_False, T, U, V);
            ComputeRangeFromStartPoint(Standard_True, T, U, V);

            if (aNbRanges == myRangeManager.Length())
            {
              SetEmptyResultRange(T, myRangeManager);
            }
          }
        } // end for
      }
      else
      {
        myRangeManager.InsertRange(anarg1, anarg2, 0);
      }

      aRangeSPrev = anItS.Value();
    }

    //
    aCurveData.ListRangeOut(aListOut);
  }

  //
  if (bAllowSamplingC)
  {
    IntTools_ListIteratorOfListOfCurveRangeSample anItC(aListOut);

    for (; anItC.More(); anItC.Next())
    {
      IntTools_Range aRangeC = anItC.Value().GetRange(myFirstParameter, myLastParameter, nbSampleC);
      myRangeManager.InsertRange(aRangeC.First(), aRangeC.Last(), 1);
    }
  }
  ComputeNearRangeBoundaries();

  aSurfaceData.ClearGrid();

  return Standard_True;
}

// ======================================================================================================================
// function: TestComputeCoinside
// purpose:
// ======================================================================================================================
Standard_Boolean IntTools_BeanFaceIntersector::TestComputeCoinside()
{
  Standard_Real          cfp = myFirstParameter, clp = myLastParameter;
  const Standard_Integer nbSeg = 23;
  Standard_Real          cdp   = (clp - cfp) / (Standard_Real)nbSeg;

  Standard_Integer i = 0;

  Standard_Real U, V;

  if (Distance(cfp, U, V) > myCriteria)
    return Standard_False;

  //
  ComputeRangeFromStartPoint(Standard_True, cfp, U, V);
  //

  Standard_Integer aFoundIndex = myRangeManager.GetIndex(clp, Standard_False);

  if (aFoundIndex != 0)
  {
    if (myRangeManager.Flag(aFoundIndex) == 2)
      return Standard_True;
  }

  if (Distance(clp, U, V) > myCriteria)
    return Standard_False;

  //
  ComputeRangeFromStartPoint(Standard_False, clp, U, V);
  //

  for (i = 1; i < nbSeg; i++)
  {
    Standard_Real aPar = (cfp + ((Standard_Real)i) * cdp);

    if (Distance(aPar, U, V) > myCriteria)
      return Standard_False;

    Standard_Integer aNbRanges = myRangeManager.Length();
    ComputeRangeFromStartPoint(Standard_False, aPar, U, V);
    ComputeRangeFromStartPoint(Standard_True, aPar, U, V);

    if (aNbRanges == myRangeManager.Length())
    {
      SetEmptyResultRange(aPar, myRangeManager);
    }
  }

  return Standard_True;
}

//  Modified by skv - Wed Nov  2 15:21:11 2005 Optimization Begin
// ---------------------------------------------------------------------------------
// static function: GetSurfaceBox
// purpose:
// ---------------------------------------------------------------------------------
Bnd_Box GetSurfaceBox(const Handle(Geom_BSplineSurface)& theSurf,
                      const Standard_Real                theFirstU,
                      const Standard_Real                theLastU,
                      const Standard_Real                theFirstV,
                      const Standard_Real                theLastV,
                      const Standard_Real                theTolerance,
                      IntTools_SurfaceRangeLocalizeData& theSurfaceData)
{
  Bnd_Box aTotalBox;

  BuildBox(theSurf, theFirstU, theLastU, theFirstV, theLastV, theSurfaceData, aTotalBox);

  aTotalBox.Enlarge(theTolerance);
  return aTotalBox;
}

// ---------------------------------------------------------------------------------
// static function: ComputeGridPoints
// purpose:
// ---------------------------------------------------------------------------------
void ComputeGridPoints(const Handle(Geom_BSplineSurface)& theSurf,
                       const Standard_Real                theFirstU,
                       const Standard_Real                theLastU,
                       const Standard_Real                theFirstV,
                       const Standard_Real                theLastV,
                       const Standard_Real                theTolerance,
                       IntTools_SurfaceRangeLocalizeData& theSurfaceData)
{
  Standard_Integer     i;
  Standard_Integer     j;
  Standard_Integer     k;
  Standard_Integer     aNbSamples[2] = {theSurf->UDegree(), theSurf->VDegree()};
  Standard_Integer     aNbKnots[2]   = {theSurf->NbUKnots(), theSurf->NbVKnots()};
  TColStd_Array1OfReal aKnotsU(1, aNbKnots[0]);
  TColStd_Array1OfReal aKnotsV(1, aNbKnots[1]);

  theSurf->UKnots(aKnotsU);
  theSurf->VKnots(aKnotsV);

  Standard_Integer iLmI;
  Standard_Integer iMin[2] = {-1, -1};
  Standard_Integer iMax[2] = {-1, -1};
  Standard_Integer aNbGridPnts[2];
  Standard_Real    aFPar[2]  = {theFirstU, theFirstV};
  Standard_Real    aLPar[2]  = {theLastU, theLastV};
  Standard_Real    aFpTol[2] = {aFPar[0] + theTolerance, aFPar[1] + theTolerance};
  Standard_Real    aFmTol[2] = {aFPar[0] - theTolerance, aFPar[1] - theTolerance};
  Standard_Real    aLpTol[2] = {aLPar[0] + theTolerance, aLPar[1] + theTolerance};
  Standard_Real    aLmTol[2] = {aLPar[0] - theTolerance, aLPar[1] - theTolerance};

  // Compute number of U and V grid points.
  for (j = 0; j < 2; j++)
  {
    const TColStd_Array1OfReal& aKnots = (j == 0) ? aKnotsU : aKnotsV;

    for (i = 1; i <= aNbKnots[j] && (iMin[j] == -1 || iMax[j] == -1); i++)
    {
      if (iMin[j] == -1 && aFpTol[j] < aKnots.Value(i))
        iMin[j] = i - 1;

      iLmI = aNbKnots[j] - i + 1;

      if (iMax[j] == -1 && aLmTol[j] > aKnots.Value(iLmI))
        iMax[j] = iLmI + 1;
    }

    // If indices are not found, return.
    // if (iMin[j] == -1 || iMax[j] == -1)
    // return;
    if (iMin[j] == -1)
      iMin[j] = 1;

    if (iMax[j] == -1)
      iMax[j] = aNbKnots[j];

    if (iMin[j] == 0)
      iMin[j] = 1;

    if (iMax[j] > aNbKnots[j])
      iMax[j] = aNbKnots[j];

    if (iMax[j] < iMin[j])
      return;

    if (iMax[j] == iMin[j])
    {
      iMax[j]++;
      iMin[j]--;
      if (iMin[j] == 0)
        iMin[j] = 1;
      if (iMax[j] > aNbKnots[j])
        iMax[j] = aNbKnots[j];
    }

    aNbGridPnts[j] = (iMax[j] - iMin[j]) * aNbSamples[j] + 1;

    // Setting the number of grid points.
    if (j == 0)
      theSurfaceData.SetRangeUGrid(aNbGridPnts[j]);
    else // j == 1
      theSurfaceData.SetRangeVGrid(aNbGridPnts[j]);

    // Setting the first and last parameters.
    Standard_Integer iAbs = 1;
    Standard_Real    aMinPar;
    Standard_Real    aMaxPar = (j == 0) ? theLastU : theLastV;

    for (i = iMin[j]; i < iMax[j]; i++)
    {
      // Get the first parameter.
      if (i == iMin[j])
      {
        // The first knot.
        if (aFmTol[j] > aKnots.Value(iMin[j]))
          aMinPar = aFPar[j];
        else
          aMinPar = aKnots.Value(iMin[j]);
      }
      else
      {
        aMinPar = aKnots.Value(i);
      }

      // Get the last parameter.
      if (i == iMax[j] - 1)
      {
        // The last knot.
        if (aLpTol[j] < aKnots.Value(iMax[j]))
          aMaxPar = aLPar[j];
        else
          aMaxPar = aKnots.Value(iMax[j]);
      }
      else
      {
        aMaxPar = aKnots.Value(i + 1);
      }

      // Compute grid parameters.
      Standard_Real aDelta = (aMaxPar - aMinPar) / aNbSamples[j];

      for (k = 0; k < aNbSamples[j]; k++, aMinPar += aDelta)
      {
        if (j == 0)
          theSurfaceData.SetUParam(iAbs++, aMinPar);
        else
          theSurfaceData.SetVParam(iAbs++, aMinPar);
      }
    }

    // Add the last parameter
    if (j == 0)
      theSurfaceData.SetUParam(iAbs++, aMaxPar);
    else
      theSurfaceData.SetVParam(iAbs++, aMaxPar);
  }

  // Compute of grid points.
  gp_Pnt           aPnt;
  Standard_Real    aParU;
  Standard_Real    aParV;
  gp_Vec           aDU, aDV;
  Standard_Real    du = 0, dv = 0;
  Standard_Boolean isCalcDefl = aNbGridPnts[0] < 30 && aNbGridPnts[1] < 30;

  Bnd_Box aGridBox, anExtBox;

  for (i = 1; i <= aNbGridPnts[0]; i++)
  {
    aParU = theSurfaceData.GetUParam(i);

    if (isCalcDefl && i < aNbGridPnts[0])
    {
      du = 0.5 * (theSurfaceData.GetUParam(i + 1) - aParU);
    }

    for (j = 1; j <= aNbGridPnts[1]; j++)
    {
      aParV = theSurfaceData.GetVParam(j);

      if (isCalcDefl)
      {
        theSurf->D1(aParU, aParV, aPnt, aDU, aDV);
      }
      else
      {
        theSurf->D0(aParU, aParV, aPnt);
      }

      theSurfaceData.SetGridPoint(i, j, aPnt);
      //
      if (isCalcDefl)
      {
        aGridBox.Add(aPnt);
        if (i < aNbGridPnts[0] && j < aNbGridPnts[1])
        {
          dv            = 0.5 * (theSurfaceData.GetVParam(j + 1) - aParV);
          gp_Vec aShift = du * aDU + dv * aDV;
          aPnt.Translate(aShift);
          anExtBox.Add(aPnt);
        }
      }
    }
  }

  // Compute deflection.
  Standard_Real aDef = 0.;
  if (isCalcDefl)
  {
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    Standard_Real xmin1, ymin1, zmin1, xmax1, ymax1, zmax1;
    aGridBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    anExtBox.Get(xmin1, ymin1, zmin1, xmax1, ymax1, zmax1);
    Standard_Integer anExtCount = 0;
    if (xmin1 < xmin)
    {
      aDef = Max(xmin - xmin1, aDef);
      anExtCount++;
    }
    if (ymin1 < ymin)
    {
      aDef = Max(ymin - ymin1, aDef);
      anExtCount++;
    }
    if (zmin1 < zmin)
    {
      aDef = Max(zmin - zmin1, aDef);
      anExtCount++;
    }
    if (xmax1 > xmax)
    {
      aDef = Max(xmax1 - xmax, aDef);
      anExtCount++;
    }
    if (ymax1 > ymax)
    {
      aDef = Max(ymax1 - ymax, aDef);
      anExtCount++;
    }
    if (zmax1 > zmax)
    {
      aDef = Max(zmax1 - zmax, aDef);
      anExtCount++;
    }
    if (anExtCount < 3)
    {
      aDef /= 2.;
    }
  }

  if (theTolerance > aDef)
    aDef = 2. * theTolerance;

  theSurfaceData.SetGridDeflection(aDef);
}

// ---------------------------------------------------------------------------------
// static function: BuildBox
// purpose:  Compute bounding box.
// ---------------------------------------------------------------------------------
void BuildBox(const Handle(Geom_BSplineSurface)& theSurf,
              const Standard_Real                theFirstU,
              const Standard_Real                theLastU,
              const Standard_Real                theFirstV,
              const Standard_Real                theLastV,
              IntTools_SurfaceRangeLocalizeData& theSurfaceData,
              Bnd_Box&                           theBox)
{
  Standard_Integer i;
  Standard_Integer j;
  Standard_Integer aNbUPnts;
  Standard_Integer aNbVPnts;
  Standard_Real    aParam;
  gp_Pnt           aPnt;

  theSurfaceData.SetFrame(theFirstU, theLastU, theFirstV, theLastV);
  aNbUPnts = theSurfaceData.GetNBUPointsInFrame();
  aNbVPnts = theSurfaceData.GetNBVPointsInFrame();

  // Add corner points.
  theSurf->D0(theFirstU, theFirstV, aPnt);
  theBox.Add(aPnt);
  theSurf->D0(theLastU, theFirstV, aPnt);
  theBox.Add(aPnt);
  theSurf->D0(theFirstU, theLastV, aPnt);
  theBox.Add(aPnt);
  theSurf->D0(theLastU, theLastV, aPnt);
  theBox.Add(aPnt);

  for (i = 1; i <= aNbUPnts; i++)
  {
    // Add top and bottom points.
    aParam = theSurfaceData.GetUParamInFrame(i);
    theSurf->D0(aParam, theFirstV, aPnt);
    theBox.Add(aPnt);
    theSurf->D0(aParam, theLastV, aPnt);
    theBox.Add(aPnt);

    // Add internal points.
    for (j = 1; j <= aNbVPnts; j++)
    {
      const gp_Pnt& aGridPnt = theSurfaceData.GetPointInFrame(i, j);

      theBox.Add(aGridPnt);
    }
  }

  // Add left and right points.
  for (j = 1; j <= aNbVPnts; j++)
  {
    aParam = theSurfaceData.GetVParamInFrame(j);
    theSurf->D0(theFirstU, aParam, aPnt);
    theBox.Add(aPnt);
    theSurf->D0(theLastU, aParam, aPnt);
    theBox.Add(aPnt);
  }

  theBox.Enlarge(theSurfaceData.GetGridDeflection());
}

//  Modified by skv - Wed Nov  2 15:21:11 2005 Optimization End

// ---------------------------------------------------------------------------------
// static function: MergeSolutions
// purpose:
// ---------------------------------------------------------------------------------
static void MergeSolutions(const IntTools_ListOfCurveRangeSample&   theListCurveRange,
                           const IntTools_ListOfSurfaceRangeSample& theListSurfaceRange,
                           IntTools_ListOfCurveRangeSample&         theListCurveRangeSort,
                           IntTools_ListOfSurfaceRangeSample&       theListSurfaceRangeSort)
{
  NCollection_IndexedMap<IntTools_SurfaceRangeSample> aMapToAvoid;

  NCollection_DataMap<Standard_Integer, TColStd_ListOfInteger> aCurveIdMap;
  std::vector<IntTools_CurveRangeSample>                       aCurveRangeVector;
  aCurveRangeVector.reserve(theListCurveRange.Size());

  IntTools_ListIteratorOfListOfCurveRangeSample   anItC(theListCurveRange);
  IntTools_ListIteratorOfListOfSurfaceRangeSample anItS(theListSurfaceRange);

  Standard_Integer aCurveRangeId  = 0;
  Standard_Integer aSurfRangeSize = 0;
  for (; anItS.More() && anItC.More(); anItS.Next(), anItC.Next(), ++aCurveRangeId)
  {
    aCurveRangeVector.push_back(anItC.Value());
    Standard_Integer aSurfIndex = aMapToAvoid.Add(anItS.Value());
    if (aSurfIndex > aSurfRangeSize)
    {
      aCurveIdMap.Bound(aSurfIndex, TColStd_ListOfInteger())->Append(aCurveRangeId);
      ++aSurfRangeSize;
    }
    else
    {
      aCurveIdMap.ChangeFind(aSurfIndex).Append(aCurveRangeId);
    }
  }

  for (Standard_Integer i = 1; i <= aMapToAvoid.Size(); i++)
  {
    const IntTools_SurfaceRangeSample& aSurfRange      = aMapToAvoid(i);
    const TColStd_ListOfInteger&       aCurveRangeList = aCurveIdMap(i);
    for (TColStd_ListOfInteger::Iterator anIter(aCurveRangeList); anIter.More(); anIter.Next())
    {
      const IntTools_CurveRangeSample& aCurveRange = aCurveRangeVector[anIter.Value()];

      theListSurfaceRangeSort.Append(aSurfRange);
      theListCurveRangeSort.Append(aCurveRange);
    }
  }
}

// ---------------------------------------------------------------------------------
// static function: CheckSampling
// purpose:
// ---------------------------------------------------------------------------------
static void CheckSampling(const IntTools_CurveRangeSample&         theCurveRange,
                          const IntTools_SurfaceRangeSample&       theSurfaceRange,
                          const IntTools_CurveRangeLocalizeData&   theCurveData,
                          const IntTools_SurfaceRangeLocalizeData& theSurfaceData,
                          const Standard_Real                      DiffC,
                          const Standard_Real                      DiffU,
                          const Standard_Real                      DiffV,
                          Standard_Boolean&                        bAllowSamplingC,
                          Standard_Boolean&                        bAllowSamplingU,
                          Standard_Boolean&                        bAllowSamplingV)
{

  const Standard_Real dLimit = 1000;
  bAllowSamplingC            = Standard_True;
  bAllowSamplingU            = Standard_True;
  bAllowSamplingV            = Standard_True;

  Standard_Integer aSamplesNb = theCurveRange.GetDepth() == 0 ? 1 : theCurveData.GetNbSample();

  // check
  if ((pow((Standard_Real)theCurveData.GetNbSample(), (Standard_Real)(theCurveRange.GetDepth() + 1))
       > dLimit)
      || ((DiffC / (Standard_Real)aSamplesNb) < theCurveData.GetMinRange()))
  {
    bAllowSamplingC = Standard_False;
  }

  aSamplesNb = theSurfaceRange.GetDepthU() == 0 ? 1 : theSurfaceData.GetNbSampleU();

  if ((pow((Standard_Real)theSurfaceData.GetNbSampleU(),
           (Standard_Real)(theSurfaceRange.GetDepthU() + 1))
       > dLimit)
      || ((DiffU / (Standard_Real)aSamplesNb) < theSurfaceData.GetMinRangeU()))
  {
    bAllowSamplingU = Standard_False;
  }

  aSamplesNb = theSurfaceRange.GetDepthV() == 0 ? 1 : theSurfaceData.GetNbSampleV();

  if ((pow((Standard_Real)theSurfaceData.GetNbSampleV(),
           (Standard_Real)(theSurfaceRange.GetDepthV() + 1))
       > dLimit)
      || ((DiffV / (Standard_Real)aSamplesNb) < theSurfaceData.GetMinRangeV()))
  {
    bAllowSamplingV = Standard_False;
  }
}
