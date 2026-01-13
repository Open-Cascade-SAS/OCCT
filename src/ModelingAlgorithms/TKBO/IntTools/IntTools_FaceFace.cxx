// Created on: 2000-11-23
// Created by: Michael KLOKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <IntTools_FaceFace.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomInt_WLApprox.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntPatch_GLine.hxx>
#include <IntTools_TopolTool.hxx>
#include <IntPatch_RLine.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <IntTools_WLineTool.hxx>
#include <ProjLib_Plane.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Elips.hxx>
#include <ApproxInt_KnotTools.hxx>

namespace
{

//! Angular tolerance used for analytical intersection.
constexpr double THE_TOLANG = 1.e-8;

//! Maximum ratio between major and minor radii for ellipse intersection to be valid.
constexpr double THE_MAX_ELLIPSE_RATIO = 100000.0;

//! Extracts cylinder height from surface V-parameter bounds.
//! @param[in]  theBAS    surface adaptor
//! @param[out] theHeight computed height (VMax - VMin)
//! @return false if V-bounds are infinite, true otherwise
static bool getCylinderHeight(const BRepAdaptor_Surface& theBAS, double& theHeight)
{
  const double aVMin = theBAS.FirstVParameter();
  const double aVMax = theBAS.LastVParameter();

  if (Precision::IsNegativeInfinite(aVMin) || Precision::IsPositiveInfinite(aVMax))
  {
    return false;
  }

  theHeight = aVMax - aVMin;
  return true;
}

//! Checks if plane-cylinder intersection should use analytical treatment.
//! @param[in] thePlane    plane surface
//! @param[in] theCylinder cylinder surface
//! @param[in] theHeight   cylinder height
//! @param[in] theTol      tolerance
//! @return true if analytical treatment is appropriate
static bool treatPlaneCylinder(const gp_Pln&      thePlane,
                               const gp_Cylinder& theCylinder,
                               const double       theHeight,
                               const double       theTol)
{
  IntAna_QuadQuadGeo anInter;
  anInter.Perform(thePlane, theCylinder, THE_TOLANG, theTol, theHeight);

  if (anInter.TypeInter() == IntAna_Ellipse)
  {
    const gp_Elips anEllipse = anInter.Ellipse(1);
    const double   aMajorR   = anEllipse.MajorRadius();
    const double   aMinorR   = anEllipse.MinorRadius();

    return (aMajorR < THE_MAX_ELLIPSE_RATIO * aMinorR);
  }

  return anInter.IsDone();
}

//! Computes distance from a point to an infinite line and returns the projection parameter.
//! Line is defined as: theLoc + t * theDir
//! @param[in]  thePoint point to compute distance from
//! @param[in]  theLoc   point on the line (at t=0)
//! @param[in]  theDir   direction of the line (unit vector)
//! @param[out] theParam parameter t where the closest point on line is located
//! @return distance from thePoint to the line
static double distancePointToLine(const gp_Pnt& thePoint,
                                  const gp_Pnt& theLoc,
                                  const gp_Dir& theDir,
                                  double&       theParam)
{
  const gp_Vec aVec(theLoc, thePoint);
  theParam              = aVec.Dot(theDir);
  const gp_Pnt aClosest = theLoc.Translated(gp_Vec(theDir) * theParam);
  return thePoint.Distance(aClosest);
}

//! Checks if a boundary circle of one perpendicular cylinder touches the other cylinder's surface.
//! For perpendicular cylinders: the boundary circle lies in a plane perpendicular to the first
//! cylinder's axis. Since the axes are perpendicular, the second axis direction lies IN this plane.
//! The boundary circle extends perpendicular to the direction towards the other axis, so it doesn't
//! get any closer - the minimum distance from the circle to the other axis equals the distance
//! from the circle's center (axis endpoint) to that axis.
//! Touch occurs when this distance equals R2 (the other cylinder's radius).
//! @param[in] theAxisPoint point on first cylinder's axis (at V-boundary) - center of boundary
//! circle
//! @param[in] theLoc2      location of second cylinder's axis (at V=0)
//! @param[in] theDir2      direction of second cylinder's axis
//! @param[in] theV2Min     minimum V parameter of second cylinder
//! @param[in] theV2Max     maximum V parameter of second cylinder
//! @param[in] theR2        radius of second cylinder (touch distance)
//! @param[in] theTol       tolerance for touch detection
//! @return true if boundary circle touches the other cylinder's surface
static bool checkBoundaryTouch(const gp_Pnt& theAxisPoint,
                               const gp_Pnt& theLoc2,
                               const gp_Dir& theDir2,
                               const double  theV2Min,
                               const double  theV2Max,
                               const double  theR2,
                               const double  theTol)
{
  // Compute distance from axis endpoint to second cylinder's axis
  double       aParam = 0.0;
  const double aDist  = distancePointToLine(theAxisPoint, theLoc2, theDir2, aParam);

  // Check if projection falls within second cylinder's V-bounds
  if (aParam < theV2Min - theTol || aParam > theV2Max + theTol)
  {
    return false;
  }

  // For perpendicular cylinders: boundary circle touches when center is at distance R2 from axis
  return std::abs(aDist - theR2) < theTol;
}

//! Checks if cylinder-cylinder intersection should use analytical treatment.
//! Disables analytical treatment for perpendicular cylinders that touch (tangent case).
//! Handles both interior touch and boundary touch cases.
//! @param[in] theBAS1 first cylinder surface adaptor (with UV bounds)
//! @param[in] theBAS2 second cylinder surface adaptor (with UV bounds)
//! @param[in] theTol  tolerance
//! @return true if analytical treatment is appropriate, false to disable it
static bool treatCylinderCylinder(const BRepAdaptor_Surface& theBAS1,
                                  const BRepAdaptor_Surface& theBAS2,
                                  const double               theTol)
{
  const gp_Cylinder aCyl1 = theBAS1.Cylinder();
  const gp_Cylinder aCyl2 = theBAS2.Cylinder();

  const gp_Ax3& aPos1 = aCyl1.Position();
  const gp_Ax3& aPos2 = aCyl2.Position();
  const gp_Dir& aDir1 = aPos1.Direction();
  const gp_Dir& aDir2 = aPos2.Direction();

  // Check axis relationship using angular tolerance
  const double aDot       = std::abs(aDir1.Dot(aDir2));
  const bool   isParallel = (1.0 - aDot) < Precision::Angular();
  const bool   isPerp     = aDot < Precision::Angular();

  if (isParallel)
  {
    // Parallel cylinders - use default analytical treatment
    return true;
  }

  if (!isPerp)
  {
    // Non-perpendicular, non-parallel - use analytical treatment
    return true;
  }

  // Get V-parameter bounds (height along axis)
  const double aV1Min = theBAS1.FirstVParameter();
  const double aV1Max = theBAS1.LastVParameter();
  const double aV2Min = theBAS2.FirstVParameter();
  const double aV2Max = theBAS2.LastVParameter();

  // Check for infinite bounds
  if (Precision::IsNegativeInfinite(aV1Min) || Precision::IsPositiveInfinite(aV1Max)
      || Precision::IsNegativeInfinite(aV2Min) || Precision::IsPositiveInfinite(aV2Max))
  {
    return true;
  }

  const double aR2 = aCyl2.Radius();

  // Compute axis locations (at V=0)
  const gp_Pnt& aLoc1 = aPos1.Location();
  const gp_Pnt& aLoc2 = aPos2.Location();

  // Compute axis segment endpoints
  const gp_Pnt aP1Min = aLoc1.Translated(gp_Vec(aDir1) * aV1Min);
  const gp_Pnt aP1Max = aLoc1.Translated(gp_Vec(aDir1) * aV1Max);

  // Check boundary touch: endpoint of cylinder 1's axis touching cylinder 2's surface
  if (checkBoundaryTouch(aP1Min, aLoc2, aDir2, aV2Min, aV2Max, aR2, theTol)
      || checkBoundaryTouch(aP1Max, aLoc2, aDir2, aV2Min, aV2Max, aR2, theTol))
  {
    return false; // Touch detected - disable analytical
  }

  // No boundary touch detected - allow analytical treatment
  return true;
}

//! Determines if analytical intersection treatment is appropriate for the given surfaces.
//! @param[in] theBAS1 first surface adaptor
//! @param[in] theBAS2 second surface adaptor
//! @param[in] theTol  tolerance
//! @return true if analytical treatment should be used
static bool isTreatAnalityc(const BRepAdaptor_Surface& theBAS1,
                            const BRepAdaptor_Surface& theBAS2,
                            const double               theTol)
{
  const GeomAbs_SurfaceType aType1 = theBAS1.GetType();
  const GeomAbs_SurfaceType aType2 = theBAS2.GetType();

  // Handle Plane-Cylinder case
  if ((aType1 == GeomAbs_Plane && aType2 == GeomAbs_Cylinder)
      || (aType1 == GeomAbs_Cylinder && aType2 == GeomAbs_Plane))
  {
    const BRepAdaptor_Surface& aPlaneAdaptor = (aType1 == GeomAbs_Plane) ? theBAS1 : theBAS2;
    const BRepAdaptor_Surface& aCylAdaptor   = (aType1 == GeomAbs_Cylinder) ? theBAS1 : theBAS2;

    double aHeight = 0.0;
    if (!getCylinderHeight(aCylAdaptor, aHeight))
    {
      return true;
    }

    return treatPlaneCylinder(aPlaneAdaptor.Plane(), aCylAdaptor.Cylinder(), aHeight, theTol);
  }

  // Handle Cylinder-Cylinder case (including perpendicular touching)
  if (aType1 == GeomAbs_Cylinder && aType2 == GeomAbs_Cylinder)
  {
    return treatCylinderCylinder(theBAS1, theBAS2, theTol * 10.);
  }

  // Default: use analytical treatment for unhandled cases
  return true;
}

} // namespace

static void Parameters(const occ::handle<GeomAdaptor_Surface>&,
                       const occ::handle<GeomAdaptor_Surface>&,
                       const gp_Pnt&,
                       double&,
                       double&,
                       double&,
                       double&);

static void CorrectSurfaceBoundaries(const TopoDS_Face& theFace,
                                     const double       theTolerance,
                                     double&            theumin,
                                     double&            theumax,
                                     double&            thevmin,
                                     double&            thevmax);

static bool ParameterOutOfBoundary(const double                   theParameter,
                                   const occ::handle<Geom_Curve>& theCurve,
                                   const TopoDS_Face&             theFace1,
                                   const TopoDS_Face&             theFace2,
                                   const double                   theOtherParameter,
                                   const bool                     bIncreasePar,
                                   const double                   theTol,
                                   double&                        theNewParameter,
                                   const occ::handle<IntTools_Context>&);

static bool IsCurveValid(const occ::handle<Geom2d_Curve>& thePCurve);

static bool ApproxWithPCurves(const gp_Cylinder& theCyl, const gp_Sphere& theSph);

static void PerformPlanes(const occ::handle<GeomAdaptor_Surface>& theS1,
                          const occ::handle<GeomAdaptor_Surface>& theS2,
                          const double                            TolF1,
                          const double                            TolF2,
                          const double                            TolAng,
                          const double                            TolTang,
                          const bool                              theApprox1,
                          const bool                              theApprox2,
                          NCollection_Sequence<IntTools_Curve>&   theSeqOfCurve,
                          bool&                                   theTangentFaces);

static bool ClassifyLin2d(const occ::handle<GeomAdaptor_Surface>& theS,
                          const gp_Lin2d&                         theLin2d,
                          const double                            theTol,
                          double&                                 theP1,
                          double&                                 theP2);
//
static void ApproxParameters(const occ::handle<GeomAdaptor_Surface>& aHS1,
                             const occ::handle<GeomAdaptor_Surface>& aHS2,
                             int&                                    iDegMin,
                             int&                                    iNbIter,
                             int&                                    iDegMax);

static void Tolerances(const occ::handle<GeomAdaptor_Surface>& aHS1,
                       const occ::handle<GeomAdaptor_Surface>& aHS2,
                       double&                                 aTolTang);

static bool SortTypes(const GeomAbs_SurfaceType aType1, const GeomAbs_SurfaceType aType2);
static int  IndexType(const GeomAbs_SurfaceType aType);

//
static bool CheckPCurve(const occ::handle<Geom2d_Curve>&     aPC,
                        const TopoDS_Face&                   aFace,
                        const occ::handle<IntTools_Context>& theCtx);

static double MaxDistance(const occ::handle<Geom_Curve>& theC,
                          const double                   aT,
                          GeomAPI_ProjectPointOnSurf&    theProjPS);

static double FindMaxDistance(const occ::handle<Geom_Curve>& theC,
                              const double                   theFirst,
                              const double                   theLast,
                              GeomAPI_ProjectPointOnSurf&    theProjPS,
                              const double                   theEps);

static double FindMaxDistance(const occ::handle<Geom_Curve>&       theCurve,
                              const double                         theFirst,
                              const double                         theLast,
                              const TopoDS_Face&                   theFace,
                              const occ::handle<IntTools_Context>& theContext);

static void CorrectPlaneBoundaries(double& aUmin, double& aUmax, double& aVmin, double& aVmax);

//=================================================================================================

IntTools_FaceFace::IntTools_FaceFace()
{
  myIsDone       = false;
  myTangentFaces = false;
  //
  myHS1        = new GeomAdaptor_Surface();
  myHS2        = new GeomAdaptor_Surface();
  myTolF1      = 0.;
  myTolF2      = 0.;
  myTol        = 0.;
  myFuzzyValue = Precision::Confusion();
  SetParameters(true, true, true, 1.e-07);
}

//=================================================================================================

void IntTools_FaceFace::SetContext(const occ::handle<IntTools_Context>& aContext)
{
  myContext = aContext;
}

//=================================================================================================

const occ::handle<IntTools_Context>& IntTools_FaceFace::Context() const
{
  return myContext;
}

//=================================================================================================

const TopoDS_Face& IntTools_FaceFace::Face1() const
{
  return myFace1;
}

//=================================================================================================

const TopoDS_Face& IntTools_FaceFace::Face2() const
{
  return myFace2;
}

//=================================================================================================

bool IntTools_FaceFace::TangentFaces() const
{
  return myTangentFaces;
}

//=================================================================================================

const NCollection_Sequence<IntTools_PntOn2Faces>& IntTools_FaceFace::Points() const
{
  return myPnts;
}

//=================================================================================================

bool IntTools_FaceFace::IsDone() const
{
  return myIsDone;
}

//=======================================================================
// function : Lines
// purpose  : return lines of intersection
//=======================================================================
const NCollection_Sequence<IntTools_Curve>& IntTools_FaceFace::Lines() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "IntTools_FaceFace::Lines() => myIntersector NOT DONE");
  return mySeqOfCurve;
}

// =======================================================================
// function: SetParameters
//
// =======================================================================
void IntTools_FaceFace::SetParameters(const bool   ToApproxC3d,
                                      const bool   ToApproxC2dOnS1,
                                      const bool   ToApproxC2dOnS2,
                                      const double ApproximationTolerance)
{
  myApprox    = ToApproxC3d;
  myApprox1   = ToApproxC2dOnS1;
  myApprox2   = ToApproxC2dOnS2;
  myTolApprox = ApproximationTolerance;
}

//=================================================================================================

void IntTools_FaceFace::SetFuzzyValue(const double theFuzz)
{
  myFuzzyValue = std::max(theFuzz, Precision::Confusion());
}

//=================================================================================================

double IntTools_FaceFace::FuzzyValue() const
{
  return myFuzzyValue;
}

//=================================================================================================

void IntTools_FaceFace::SetList(NCollection_List<IntSurf_PntOn2S>& aListOfPnts)
{
  myListOfPnts = aListOfPnts;
}

//=======================================================================
// function : Perform
// purpose  : intersect surfaces of the faces
//=======================================================================
void IntTools_FaceFace::Perform(const TopoDS_Face& aF1,
                                const TopoDS_Face& aF2,
                                const bool         theToRunParallel)
{
  if (myContext.IsNull())
  {
    myContext = new IntTools_Context;
  }

  mySeqOfCurve.Clear();
  myIsDone  = false;
  myNbrestr = 0; //?

  myFace1 = aF1;
  myFace2 = aF2;

  const BRepAdaptor_Surface& aBAS1  = myContext->SurfaceAdaptor(myFace1);
  const BRepAdaptor_Surface& aBAS2  = myContext->SurfaceAdaptor(myFace2);
  GeomAbs_SurfaceType        aType1 = aBAS1.GetType();
  GeomAbs_SurfaceType        aType2 = aBAS2.GetType();

  const bool bReverse = SortTypes(aType1, aType2);
  if (bReverse)
  {
    myFace1 = aF2;
    myFace2 = aF1;
    aType1  = aBAS2.GetType();
    aType2  = aBAS1.GetType();

    if (myListOfPnts.Extent())
    {
      double                                      aU1, aV1, aU2, aV2;
      NCollection_List<IntSurf_PntOn2S>::Iterator aItP2S;
      //
      aItP2S.Initialize(myListOfPnts);
      for (; aItP2S.More(); aItP2S.Next())
      {
        IntSurf_PntOn2S& aP2S = aItP2S.ChangeValue();
        aP2S.Parameters(aU1, aV1, aU2, aV2);
        aP2S.SetValue(aU2, aV2, aU1, aV1);
      }
    }
    //
    bool anAproxTmp = myApprox1;
    myApprox1       = myApprox2;
    myApprox2       = anAproxTmp;
  }

  const occ::handle<Geom_Surface> S1 = BRep_Tool::Surface(myFace1);
  const occ::handle<Geom_Surface> S2 = BRep_Tool::Surface(myFace2);

  double aFuzz = myFuzzyValue / 2.;
  myTolF1      = BRep_Tool::Tolerance(myFace1) + aFuzz;
  myTolF2      = BRep_Tool::Tolerance(myFace2) + aFuzz;
  myTol        = myTolF1 + myTolF2;

  double TolArc  = myTol;
  double TolTang = TolArc;

  const bool isFace1Quad =
    (aType1 == GeomAbs_Cylinder || aType1 == GeomAbs_Cone || aType1 == GeomAbs_Torus);

  const bool isFace2Quad =
    (aType2 == GeomAbs_Cylinder || aType2 == GeomAbs_Cone || aType2 == GeomAbs_Torus);

  if (aType1 == GeomAbs_Plane && aType2 == GeomAbs_Plane)
  {
    double umin, umax, vmin, vmax;
    //
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    myHS1->Load(S1, umin, umax, vmin, vmax);
    //
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    myHS2->Load(S2, umin, umax, vmin, vmax);
    //
    double TolAng = 1.e-8;
    //
    PerformPlanes(myHS1,
                  myHS2,
                  myTolF1,
                  myTolF2,
                  TolAng,
                  TolTang,
                  myApprox1,
                  myApprox2,
                  mySeqOfCurve,
                  myTangentFaces);
    //
    myIsDone = true;
    //
    if (!myTangentFaces)
    {
      const int NbLinPP = mySeqOfCurve.Length();
      if (NbLinPP && bReverse)
      {
        occ::handle<Geom2d_Curve> aC2D1, aC2D2;
        const int                 aNbLin = mySeqOfCurve.Length();
        for (int i = 1; i <= aNbLin; ++i)
        {
          IntTools_Curve& aIC = mySeqOfCurve(i);
          aC2D1               = aIC.FirstCurve2d();
          aC2D2               = aIC.SecondCurve2d();
          aIC.SetFirstCurve2d(aC2D2);
          aIC.SetSecondCurve2d(aC2D1);
        }
      }
    }
    return;
  } // if(aType1==GeomAbs_Plane && aType2==GeomAbs_Plane){

  if ((aType1 == GeomAbs_Plane) && isFace2Quad)
  {
    double umin, umax, vmin, vmax;
    // F1
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS1->Load(S1, umin, umax, vmin, vmax);
    // F2
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, myTol * 2., umin, umax, vmin, vmax);
    myHS2->Load(S2, umin, umax, vmin, vmax);
  }
  else if ((aType2 == GeomAbs_Plane) && isFace1Quad)
  {
    double umin, umax, vmin, vmax;
    // F1
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, myTol * 2., umin, umax, vmin, vmax);
    myHS1->Load(S1, umin, umax, vmin, vmax);
    // F2
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS2->Load(S2, umin, umax, vmin, vmax);
  }
  else
  {
    double umin, umax, vmin, vmax;
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, myTol * 2., umin, umax, vmin, vmax);
    myHS1->Load(S1, umin, umax, vmin, vmax);
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, myTol * 2., umin, umax, vmin, vmax);
    myHS2->Load(S2, umin, umax, vmin, vmax);
  }

  const occ::handle<Adaptor3d_TopolTool> dom1 = new IntTools_TopolTool(myHS1);
  const occ::handle<Adaptor3d_TopolTool> dom2 = new IntTools_TopolTool(myHS2);

  myLConstruct.Load(dom1, dom2, myHS1, myHS2);

  Tolerances(myHS1, myHS2, TolTang);

  {
    const double UVMaxStep  = IntPatch_Intersection::DefineUVMaxStep(myHS1, dom1, myHS2, dom2);
    double       Deflection = 0.1;
    if (aType1 == GeomAbs_BSplineSurface && aType2 == GeomAbs_BSplineSurface)
    {
      Deflection /= 10.;
    }
    myIntersector.SetTolerances(TolArc, TolTang, UVMaxStep, Deflection);
  }

  if ((aType1 != GeomAbs_BSplineSurface) && (aType1 != GeomAbs_BezierSurface)
      && (aType1 != GeomAbs_OtherSurface) && (aType2 != GeomAbs_BSplineSurface)
      && (aType2 != GeomAbs_BezierSurface) && (aType2 != GeomAbs_OtherSurface))
  {
    if ((aType1 == GeomAbs_Torus) || (aType2 == GeomAbs_Torus))
    {
      myListOfPnts.Clear();
    }
  }

#ifdef INTTOOLS_FACEFACE_DEBUG
  if (!myListOfPnts.IsEmpty())
  {
    char aBuff[10000];

    Sprintf(aBuff, "bopcurves <face1 face2> -2d");
    NCollection_List<IntSurf_PntOn2S>::Iterator IterLOP1(myListOfPnts);
    for (; IterLOP1.More(); IterLOP1.Next())
    {
      const IntSurf_PntOn2S& aPt = IterLOP1.Value();
      double                 u1, v1, u2, v2;
      aPt.Parameters(u1, v1, u2, v2);

      Sprintf(aBuff, "%s -p %+10.20f %+10.20f %+10.20f %+10.20f", aBuff, u1, v1, u2, v2);
    }

    std::cout << aBuff << std::endl;
  }
#endif

  const bool isGeomInt = isTreatAnalityc(aBAS1, aBAS2, myTol);
  if (aF1.IsSame(aF2))
    myIntersector.Perform(myHS1, dom1, TolArc, TolTang);
  else
    myIntersector.Perform(myHS1, dom1, myHS2, dom2, TolArc, TolTang, myListOfPnts, isGeomInt);

  myIsDone = myIntersector.IsDone();

  if (myIsDone)
  {
    myTangentFaces = myIntersector.TangentFaces();
    if (myTangentFaces)
    {
      return;
    }
    //
    const int aNbLinIntersector = myIntersector.NbLines();
    for (int i = 1; i <= aNbLinIntersector; ++i)
    {
      MakeCurve(i, dom1, dom2, TolArc);
    }
    //
    ComputeTolReached3d(theToRunParallel);
    //
    if (bReverse)
    {
      occ::handle<Geom2d_Curve> aC2D1, aC2D2;
      //
      const int aNbLinSeqOfCurve = mySeqOfCurve.Length();
      for (int i = 1; i <= aNbLinSeqOfCurve; ++i)
      {
        IntTools_Curve& aIC = mySeqOfCurve(i);
        aC2D1               = aIC.FirstCurve2d();
        aC2D2               = aIC.SecondCurve2d();
        aIC.SetFirstCurve2d(aC2D2);
        aIC.SetSecondCurve2d(aC2D1);
      }
    }

    // Points
    bool                 bValid2D1, bValid2D2;
    double               U1, V1, U2, V2;
    IntTools_PntOnFace   aPntOnF1, aPntOnF2;
    IntTools_PntOn2Faces aPntOn2Faces;
    //
    const int aNbPnts = myIntersector.NbPnts();
    for (int i = 1; i <= aNbPnts; ++i)
    {
      const IntSurf_PntOn2S& aISPnt = myIntersector.Point(i).PntOn2S();
      const gp_Pnt&          aPnt   = aISPnt.Value();
      aISPnt.Parameters(U1, V1, U2, V2);
      //
      // check the validity of the intersection point for the faces
      bValid2D1 = myContext->IsPointInOnFace(myFace1, gp_Pnt2d(U1, V1));
      if (!bValid2D1)
      {
        continue;
      }
      //
      bValid2D2 = myContext->IsPointInOnFace(myFace2, gp_Pnt2d(U2, V2));
      if (!bValid2D2)
      {
        continue;
      }
      //
      // add the intersection point
      aPntOnF1.Init(myFace1, aPnt, U1, V1);
      aPntOnF2.Init(myFace2, aPnt, U2, V2);
      //
      if (!bReverse)
      {
        aPntOn2Faces.SetP1(aPntOnF1);
        aPntOn2Faces.SetP2(aPntOnF2);
      }
      else
      {
        aPntOn2Faces.SetP2(aPntOnF1);
        aPntOn2Faces.SetP1(aPntOnF2);
      }

      myPnts.Append(aPntOn2Faces);
    }
  }
}

//=================================================================================================

void IntTools_FaceFace::ComputeTolReached3d(const bool theToRunParallel)
{
  int i, j, aNbLin = mySeqOfCurve.Length();
  if (!aNbLin)
  {
    return;
  }
  //
  // Minimal tangential tolerance for the curve
  double aTolFMax = std::max(myTolF1, myTolF2);
  //
  const occ::handle<Geom_Surface>& aS1 = myHS1->Surface();
  const occ::handle<Geom_Surface>& aS2 = myHS2->Surface();
  //
  for (i = 1; i <= aNbLin; ++i)
  {
    IntTools_Curve&                aIC  = mySeqOfCurve(i);
    const occ::handle<Geom_Curve>& aC3D = aIC.Curve();
    if (aC3D.IsNull())
    {
      continue;
    }
    //
    double aTolC  = aIC.Tolerance();
    double aFirst = aC3D->FirstParameter();
    double aLast  = aC3D->LastParameter();
    //
    // Compute the tolerance for the curve
    const occ::handle<Geom2d_Curve>& aC2D1 = aIC.FirstCurve2d();
    const occ::handle<Geom2d_Curve>& aC2D2 = aIC.SecondCurve2d();
    //
    for (j = 0; j < 2; ++j)
    {
      const occ::handle<Geom2d_Curve>& aC2D = !j ? aC2D1 : aC2D2;
      if (!aC2D.IsNull())
      {
        // Look for the maximal deviation between 3D and 2D curves
        double                           aD, aT;
        const occ::handle<Geom_Surface>& aS = !j ? aS1 : aS2;
        if (IntTools_Tools::ComputeTolerance(aC3D,
                                             aC2D,
                                             aS,
                                             aFirst,
                                             aLast,
                                             aD,
                                             aT,
                                             Precision::PConfusion(),
                                             theToRunParallel))
        {
          if (aD > aTolC)
          {
            aTolC = aD;
          }
        }
      }
      else
      {
        // Look for the maximal deviation between 3D curve and surface
        const TopoDS_Face& aF = !j ? myFace1 : myFace2;
        double             aD = FindMaxDistance(aC3D, aFirst, aLast, aF, myContext);
        if (aD > aTolC)
        {
          aTolC = aD;
        }
      }
    }
    // Set the valid tolerance for the curve
    aIC.SetTolerance(aTolC);
    //
    // Set the tangential tolerance for the curve.
    // Note, that, currently, computation of the tangential tolerance is
    // implemented for the Plane/Plane case only.
    // Thus, set the tangential tolerance equal to maximal tolerance of faces.
    if (aIC.TangentialTolerance() < aTolFMax)
    {
      aIC.SetTangentialTolerance(aTolFMax);
    }
  }
}

//=================================================================================================

void IntTools_FaceFace::MakeCurve(const int                               Index,
                                  const occ::handle<Adaptor3d_TopolTool>& dom1,
                                  const occ::handle<Adaptor3d_TopolTool>& dom2,
                                  const double                            theToler)
{
  bool                       bDone, rejectSurface, reApprox, bAvoidLineConstructor;
  bool                       ok, bPCurvesOk;
  int                        i, j, aNbParts;
  double                     fprm, lprm;
  double                     Tolpc;
  occ::handle<IntPatch_Line> L;
  IntPatch_IType             typl;
  occ::handle<Geom_Curve>    newc;
  //
  const double TOLCHECK    = 1.e-7;
  const double TOLANGCHECK = 1.e-6;
  //
  rejectSurface = false;
  reApprox      = false;
  //
  bPCurvesOk = true;

reapprox:;

  Tolpc                 = myTolApprox;
  bAvoidLineConstructor = false;
  L                     = myIntersector.Line(Index);
  typl                  = L->ArcType();
  //
  if (typl == IntPatch_Walking)
  {
    occ::handle<IntPatch_WLine> aWLine(occ::down_cast<IntPatch_WLine>(L));
    if (aWLine.IsNull())
    {
      return;
    }
    L = aWLine;

    int                    nbp = aWLine->NbPnts();
    const IntSurf_PntOn2S& p1  = aWLine->Point(1);
    const IntSurf_PntOn2S& p2  = aWLine->Point(nbp);

    const gp_Pnt& P1 = p1.Value();
    const gp_Pnt& P2 = p2.Value();

    if (P1.SquareDistance(P2) < 1.e-14)
    {
      bAvoidLineConstructor = false;
    }
  }

  typl = L->ArcType();

  if (typl == IntPatch_Restriction)
    bAvoidLineConstructor = true;

  //
  // Line Constructor
  if (!bAvoidLineConstructor)
  {
    myLConstruct.Perform(L);
    //
    bDone = myLConstruct.IsDone();
    if (!bDone)
    {
      return;
    }

    if (typl != IntPatch_Restriction)
    {
      aNbParts = myLConstruct.NbParts();
      if (aNbParts <= 0)
      {
        return;
      }
    }
  }
  // Do the Curve

  switch (typl)
  {
    // ########################################
    //  Line, Parabola, Hyperbola
    // ########################################
    case IntPatch_Lin:
    case IntPatch_Parabola:
    case IntPatch_Hyperbola: {
      if (typl == IntPatch_Lin)
      {
        newc = new Geom_Line(occ::down_cast<IntPatch_GLine>(L)->Line());
      }

      else if (typl == IntPatch_Parabola)
      {
        newc = new Geom_Parabola(occ::down_cast<IntPatch_GLine>(L)->Parabola());
      }

      else if (typl == IntPatch_Hyperbola)
      {
        newc = new Geom_Hyperbola(occ::down_cast<IntPatch_GLine>(L)->Hyperbola());
      }

      // Compute maximum vertex tolerance from GLine vertices.
      // This tolerance accounts for boundary intersection computation errors
      // (e.g., pcurve-to-3D-curve deviation) and must be propagated to the curve
      // to ensure vertices from different FF intersections can be unified.
      double aMaxVertTol = 0.0;
      if (myHS1->GetType() == GeomAbs_Cylinder || myHS2->GetType() == GeomAbs_Cylinder)
      {
        occ::handle<IntPatch_GLine> aGL    = occ::down_cast<IntPatch_GLine>(L);
        int                         aNbVtx = aGL->NbVertex();
        for (int iv = 1; iv <= aNbVtx; ++iv)
        {
          const IntPatch_Point& aVtx = aGL->Vertex(iv);
          if (aVtx.Tolerance() > aMaxVertTol)
          {
            aMaxVertTol = aVtx.Tolerance();
          }
        }
      }
      //
      aNbParts = myLConstruct.NbParts();
      for (i = 1; i <= aNbParts; i++)
      {
        bool bFNIt, bLPIt;
        //
        myLConstruct.Part(i, fprm, lprm);
        //
        bFNIt = Precision::IsNegativeInfinite(fprm);
        bLPIt = Precision::IsPositiveInfinite(lprm);
        //
        if (!bFNIt && !bLPIt)
        {
          //
          IntTools_Curve aCurve;
          //
          occ::handle<Geom_TrimmedCurve> aCT3D = new Geom_TrimmedCurve(newc, fprm, lprm);
          aCurve.SetCurve(aCT3D);
          if (typl == IntPatch_Parabola)
          {
            double aTolC = IntTools_Tools::CurveTolerance(aCT3D, myTol);
            aCurve.SetTolerance(aTolC);
          }
          //
          if (myApprox1)
          {
            occ::handle<Geom2d_Curve> C2d;
            GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, myHS1->Surface(), newc, C2d);

            if (C2d.IsNull())
            {
              continue;
            }

            aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d, fprm, lprm));
          }
          //
          if (myApprox2)
          {
            occ::handle<Geom2d_Curve> C2d;
            GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, myHS2->Surface(), newc, C2d);

            if (C2d.IsNull())
            {
              continue;
            }

            aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d, fprm, lprm));
          }
          // Ensure curve tolerance is at least the maximum vertex tolerance
          if (aCurve.Tolerance() < aMaxVertTol)
          {
            aCurve.SetTolerance(aMaxVertTol);
          }
          //
          mySeqOfCurve.Append(aCurve);
        } // if (!bFNIt && !bLPIt) {
        else
        {
          //  on regarde si on garde
          //
          double aTestPrm, dT = 100.;
          //
          aTestPrm = 0.;
          if (bFNIt && !bLPIt)
          {
            aTestPrm = lprm - dT;
          }
          else if (!bFNIt && bLPIt)
          {
            aTestPrm = fprm + dT;
          }
          else
          {
            // i.e, if (bFNIt && bLPIt)
            aTestPrm = IntTools_Tools::IntermediatePoint(-dT, dT);
          }
          //
          gp_Pnt ptref(newc->Value(aTestPrm));
          //
          GeomAbs_SurfaceType typS1 = myHS1->GetType();
          GeomAbs_SurfaceType typS2 = myHS2->GetType();
          if (typS1 == GeomAbs_SurfaceOfExtrusion || typS1 == GeomAbs_OffsetSurface
              || typS1 == GeomAbs_SurfaceOfRevolution || typS2 == GeomAbs_SurfaceOfExtrusion
              || typS2 == GeomAbs_OffsetSurface || typS2 == GeomAbs_SurfaceOfRevolution)
          {
            occ::handle<Geom2d_BSplineCurve> H1;
            IntTools_Curve                   aCurve(newc, H1, H1);
            if (aCurve.Tolerance() < aMaxVertTol)
            {
              aCurve.SetTolerance(aMaxVertTol);
            }
            mySeqOfCurve.Append(aCurve);
            continue;
          }

          double u1, v1, u2, v2, Tol;

          Tol = Precision::Confusion();
          Parameters(myHS1, myHS2, ptref, u1, v1, u2, v2);
          ok = (dom1->Classify(gp_Pnt2d(u1, v1), Tol) != TopAbs_OUT);
          if (ok)
          {
            ok = (dom2->Classify(gp_Pnt2d(u2, v2), Tol) != TopAbs_OUT);
          }
          if (ok)
          {
            occ::handle<Geom2d_BSplineCurve> H1;
            mySeqOfCurve.Append(IntTools_Curve(newc, H1, H1));
          }
        }
      } // for (i=1; i<=aNbParts; i++) {
    } // case IntPatch_Lin:  case IntPatch_Parabola:  case IntPatch_Hyperbola:
    break;

    // ########################################
    //  Circle and Ellipse
    // ########################################
    case IntPatch_Circle:
    case IntPatch_Ellipse: {

      if (typl == IntPatch_Circle)
      {
        newc = new Geom_Circle(occ::down_cast<IntPatch_GLine>(L)->Circle());
      }
      else
      { // IntPatch_Ellipse
        newc = new Geom_Ellipse(occ::down_cast<IntPatch_GLine>(L)->Ellipse());
      }
      //
      aNbParts = myLConstruct.NbParts();
      //
      double                       aPeriod, aNul;
      NCollection_Sequence<double> aSeqFprm, aSeqLprm;

      aNul    = 0.;
      aPeriod = M_PI + M_PI;

      for (i = 1; i <= aNbParts; i++)
      {
        myLConstruct.Part(i, fprm, lprm);

        if (fprm < aNul && lprm > aNul)
        {
          // interval that goes through 0. is divided on two intervals;
          while (fprm < aNul || fprm > aPeriod)
            fprm = fprm + aPeriod;
          while (lprm < aNul || lprm > aPeriod)
            lprm = lprm + aPeriod;
          //
          if ((aPeriod - fprm) > Tolpc)
          {
            aSeqFprm.Append(fprm);
            aSeqLprm.Append(aPeriod);
          }
          else
          {
            gp_Pnt P1 = newc->Value(fprm);
            gp_Pnt P2 = newc->Value(aPeriod);

            if (P1.Distance(P2) > myTol)
            {
              double anewpar = fprm;

              if (ParameterOutOfBoundary(fprm,
                                         newc,
                                         myFace1,
                                         myFace2,
                                         lprm,
                                         false,
                                         myTol,
                                         anewpar,
                                         myContext))
              {
                fprm = anewpar;
              }
              aSeqFprm.Append(fprm);
              aSeqLprm.Append(aPeriod);
            }
          }

          //
          if ((lprm - aNul) > Tolpc)
          {
            aSeqFprm.Append(aNul);
            aSeqLprm.Append(lprm);
          }
          else
          {
            gp_Pnt P1 = newc->Value(aNul);
            gp_Pnt P2 = newc->Value(lprm);

            if (P1.Distance(P2) > myTol)
            {
              double anewpar = lprm;

              if (ParameterOutOfBoundary(lprm,
                                         newc,
                                         myFace1,
                                         myFace2,
                                         fprm,
                                         true,
                                         myTol,
                                         anewpar,
                                         myContext))
              {
                lprm = anewpar;
              }
              aSeqFprm.Append(aNul);
              aSeqLprm.Append(lprm);
            }
          }
        }
        else
        {
          // usual interval
          aSeqFprm.Append(fprm);
          aSeqLprm.Append(lprm);
        }
      }
      //
      aNbParts = aSeqFprm.Length();
      for (i = 1; i <= aNbParts; i++)
      {
        fprm = aSeqFprm(i);
        lprm = aSeqLprm(i);
        //
        constexpr double aRealEpsilon = RealEpsilon();
        if (std::abs(fprm) > aRealEpsilon || std::abs(lprm - 2. * M_PI) > aRealEpsilon)
        {
          //==============================================
          ////
          IntTools_Curve                 aCurve;
          occ::handle<Geom_TrimmedCurve> aTC3D = new Geom_TrimmedCurve(newc, fprm, lprm);
          aCurve.SetCurve(aTC3D);
          fprm = aTC3D->FirstParameter();
          lprm = aTC3D->LastParameter();
          ////
          if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
          { ////
            if (myApprox1)
            {
              occ::handle<Geom2d_Curve> C2d;
              GeomInt_IntSS::BuildPCurves(fprm,
                                          lprm,
                                          myHS1->FirstUParameter(),
                                          myHS1->LastUParameter(),
                                          myHS1->FirstVParameter(),
                                          myHS1->LastVParameter(),
                                          Tolpc,
                                          myHS1->Surface(),
                                          newc,
                                          C2d);
              aCurve.SetFirstCurve2d(C2d);
            }

            if (myApprox2)
            {
              occ::handle<Geom2d_Curve> C2d;
              GeomInt_IntSS::BuildPCurves(fprm,
                                          lprm,
                                          myHS2->FirstUParameter(),
                                          myHS2->LastUParameter(),
                                          myHS2->FirstVParameter(),
                                          myHS2->LastVParameter(),
                                          Tolpc,
                                          myHS2->Surface(),
                                          newc,
                                          C2d);
              aCurve.SetSecondCurve2d(C2d);
            }
          }
          //
          mySeqOfCurve.Append(aCurve);
          //==============================================
        } // if (std::abs(fprm) > RealEpsilon() || std::abs(lprm-2.*M_PI) > RealEpsilon())

        else
        {
          //  on regarde si on garde
          //
          if (aNbParts == 1)
          {
            //           if (std::abs(fprm) < RealEpsilon() &&  std::abs(lprm-2.*M_PI) <
            //           RealEpsilon()) {
            if (std::abs(fprm) <= aRealEpsilon && std::abs(lprm - 2. * M_PI) <= aRealEpsilon)
            {
              IntTools_Curve                 aCurve;
              occ::handle<Geom_TrimmedCurve> aTC3D = new Geom_TrimmedCurve(newc, fprm, lprm);
              aCurve.SetCurve(aTC3D);
              fprm = aTC3D->FirstParameter();
              lprm = aTC3D->LastParameter();

              if (myApprox1)
              {
                occ::handle<Geom2d_Curve> C2d;
                GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, myHS1->Surface(), newc, C2d);
                aCurve.SetFirstCurve2d(C2d);
              }

              if (myApprox2)
              {
                occ::handle<Geom2d_Curve> C2d;
                GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, myHS2->Surface(), newc, C2d);
                aCurve.SetSecondCurve2d(C2d);
              }
              //
              mySeqOfCurve.Append(aCurve);
              break;
            }
          }
          //
          double aTwoPIdiv17, u1, v1, u2, v2, Tol;

          aTwoPIdiv17 = 2. * M_PI / 17.;

          for (j = 0; j <= 17; j++)
          {
            gp_Pnt ptref(newc->Value(j * aTwoPIdiv17));
            Tol = Precision::Confusion();

            Parameters(myHS1, myHS2, ptref, u1, v1, u2, v2);
            ok = (dom1->Classify(gp_Pnt2d(u1, v1), Tol) != TopAbs_OUT);
            if (ok)
            {
              ok = (dom2->Classify(gp_Pnt2d(u2, v2), Tol) != TopAbs_OUT);
            }
            if (ok)
            {
              IntTools_Curve aCurve;
              aCurve.SetCurve(newc);
              //==============================================
              if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
              {

                if (myApprox1)
                {
                  occ::handle<Geom2d_Curve> C2d;
                  GeomInt_IntSS::BuildPCurves(fprm,
                                              lprm,
                                              myHS1->FirstUParameter(),
                                              myHS1->LastUParameter(),
                                              myHS1->FirstVParameter(),
                                              myHS1->LastVParameter(),
                                              Tolpc,
                                              myHS1->Surface(),
                                              newc,
                                              C2d);
                  aCurve.SetFirstCurve2d(C2d);
                }

                if (myApprox2)
                {
                  occ::handle<Geom2d_Curve> C2d;
                  GeomInt_IntSS::BuildPCurves(fprm,
                                              lprm,
                                              myHS2->FirstUParameter(),
                                              myHS2->LastUParameter(),
                                              myHS2->FirstVParameter(),
                                              myHS2->LastVParameter(),
                                              Tolpc,
                                              myHS2->Surface(),
                                              newc,
                                              C2d);
                  aCurve.SetSecondCurve2d(C2d);
                }
              } //  end of if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
              //==============================================
              //
              mySeqOfCurve.Append(aCurve);
              break;

            } //  end of if (ok) {
          } //  end of for (int j=0; j<=17; j++)
        } //  end of else { on regarde si on garde
      } // for (i=1; i<=myLConstruct.NbParts(); i++)
    } // IntPatch_Circle: IntPatch_Ellipse:
    break;

    case IntPatch_Analytic:
      // This case was processed earlier (in IntPatch_Intersection)
      break;

    case IntPatch_Walking: {
      occ::handle<IntPatch_WLine> WL = occ::down_cast<IntPatch_WLine>(L);

#ifdef INTTOOLS_FACEFACE_DEBUG
      WL->Dump(0);
#endif

      //
      int ifprm, ilprm;
      //
      if (!myApprox)
      {
        aNbParts = 1;
        if (!bAvoidLineConstructor)
        {
          aNbParts = myLConstruct.NbParts();
        }
        for (i = 1; i <= aNbParts; ++i)
        {
          occ::handle<Geom2d_BSplineCurve> H1, H2;
          occ::handle<Geom_Curve>          aBSp;
          //
          if (bAvoidLineConstructor)
          {
            ifprm = 1;
            ilprm = WL->NbPnts();
          }
          else
          {
            myLConstruct.Part(i, fprm, lprm);
            ifprm = (int)fprm;
            ilprm = (int)lprm;
          }
          //
          if (myApprox1)
          {
            H1 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, true);
          }
          //
          if (myApprox2)
          {
            H2 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, false);
          }
          //
          aBSp = GeomInt_IntSS::MakeBSpline(WL, ifprm, ilprm);
          IntTools_Curve aIC(aBSp, H1, H2);
          mySeqOfCurve.Append(aIC);
        } // for (i=1; i<=aNbParts; ++i) {
      } // if (!myApprox) {
      //
      else
      { // X
        bool                                             bIsDecomposited;
        int                                              nbiter, aNbSeqOfL;
        double                                           tol2d, aTolApproxImp;
        NCollection_Sequence<occ::handle<IntPatch_Line>> aSeqOfL;
        GeomInt_WLApprox                                 theapp3d;
        Approx_ParametrizationType                       aParType = Approx_ChordLength;
        //
        bool anApprox1 = myApprox1;
        bool anApprox2 = myApprox2;
        //
        aTolApproxImp = 1.e-5;
        tol2d         = myTolApprox;

        GeomAbs_SurfaceType typs1, typs2;
        typs1         = myHS1->GetType();
        typs2         = myHS2->GetType();
        bool anWithPC = true;

        if (typs1 == GeomAbs_Cylinder && typs2 == GeomAbs_Sphere)
        {
          anWithPC = ApproxWithPCurves(myHS1->Cylinder(), myHS2->Sphere());
        }
        else if (typs1 == GeomAbs_Sphere && typs2 == GeomAbs_Cylinder)
        {
          anWithPC = ApproxWithPCurves(myHS2->Cylinder(), myHS1->Sphere());
        }
        //
        if (!anWithPC)
        {
          myTolApprox = aTolApproxImp; // 1.e-5;
          anApprox1   = false;
          anApprox2   = false;
          //
          tol2d = myTolApprox;
        }

        bIsDecomposited = IntTools_WLineTool::DecompositionOfWLine(WL,
                                                                   myHS1,
                                                                   myHS2,
                                                                   myFace1,
                                                                   myFace2,
                                                                   myLConstruct,
                                                                   bAvoidLineConstructor,
                                                                   myTol,
                                                                   aSeqOfL,
                                                                   myContext);
        //
        aNbSeqOfL = aSeqOfL.Length();
        //
        double aTolC = 0.;
        if (bIsDecomposited)
        {
          nbiter = aNbSeqOfL;
          aTolC  = Precision::Confusion();
        }
        else
        {
          nbiter   = 1;
          aNbParts = 1;
          if (!bAvoidLineConstructor)
          {
            aNbParts = myLConstruct.NbParts();
            nbiter   = aNbParts;
          }
        }
        //
        for (i = 1; i <= nbiter; ++i)
        {
          if (bIsDecomposited)
          {
            WL    = occ::down_cast<IntPatch_WLine>(aSeqOfL.Value(i));
            ifprm = 1;
            ilprm = WL->NbPnts();
          }
          else
          {
            if (bAvoidLineConstructor)
            {
              ifprm = 1;
              ilprm = WL->NbPnts();
            }
            else
            {
              myLConstruct.Part(i, fprm, lprm);
              ifprm = (int)fprm;
              ilprm = (int)lprm;
            }
          }

          bool anApprox = myApprox;
          if (typs1 == GeomAbs_Plane)
          {
            anApprox  = false;
            anApprox1 = true;
          }
          else if (typs2 == GeomAbs_Plane)
          {
            anApprox  = false;
            anApprox2 = true;
          }

          aParType =
            ApproxInt_KnotTools::DefineParType(WL, ifprm, ilprm, anApprox, anApprox1, anApprox2);
          if (myHS1 == myHS2)
          {
            theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, false, aParType);
            rejectSurface = true;
          }
          else
          {
            if (reApprox && !rejectSurface)
              theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, false, aParType);
            else
            {
              int iDegMax, iDegMin, iNbIter;
              //
              ApproxParameters(myHS1, myHS2, iDegMin, iDegMax, iNbIter);
              theapp3d
                .SetParameters(myTolApprox, tol2d, iDegMin, iDegMax, iNbIter, 30, true, aParType);
            }
          }

          //-- lbr :
          //-- Si une des surfaces est un plan , on approxime en 2d
          //-- sur cette surface et on remonte les points 2d en 3d.
          if (typs1 == GeomAbs_Plane)
          {
            theapp3d.Perform(myHS1, myHS2, WL, false, true, myApprox2, ifprm, ilprm);
          }
          else if (typs2 == GeomAbs_Plane)
          {
            theapp3d.Perform(myHS1, myHS2, WL, false, myApprox1, true, ifprm, ilprm);
          }
          else
          {
            //
            if (myHS1 != myHS2)
            {
              if ((typs1 == GeomAbs_BezierSurface || typs1 == GeomAbs_BSplineSurface)
                  && (typs2 == GeomAbs_BezierSurface || typs2 == GeomAbs_BSplineSurface))
              {

                theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, true, aParType);

                bool bUseSurfaces;
                bUseSurfaces =
                  IntTools_WLineTool::NotUseSurfacesForApprox(myFace1, myFace2, WL, ifprm, ilprm);
                if (bUseSurfaces)
                {
                  // ######
                  rejectSurface = true;
                  // ######
                  theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, false, aParType);
                }
              }
            }
            //
            theapp3d.Perform(myHS1, myHS2, WL, true, anApprox1, anApprox2, ifprm, ilprm);
          }
          //
          if (!theapp3d.IsDone())
          {
            occ::handle<Geom2d_BSplineCurve> H1;
            occ::handle<Geom2d_BSplineCurve> H2;
            //
            occ::handle<Geom_Curve> aBSp = GeomInt_IntSS::MakeBSpline(WL, ifprm, ilprm);
            //
            if (myApprox1)
            {
              H1 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, true);
            }
            //
            if (myApprox2)
            {
              H2 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, false);
            }
            //
            IntTools_Curve aIC(aBSp, H1, H2);
            mySeqOfCurve.Append(aIC);
          }
          else
          {
            if (typs1 == GeomAbs_Plane || typs2 == GeomAbs_Plane)
            {
              //
              if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus)
              {
                if (aTolC < 1.e-6)
                {
                  aTolC = 1.e-6;
                }
              }
            }
            //
            int aNbMultiCurves, nbpoles;
            aNbMultiCurves = theapp3d.NbMultiCurves();
            for (j = 1; j <= aNbMultiCurves; j++)
            {
              if (typs1 == GeomAbs_Plane)
              {
                const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
                nbpoles                                 = mbspc.NbPoles();

                NCollection_Array1<gp_Pnt2d> tpoles2d(1, nbpoles);
                NCollection_Array1<gp_Pnt>   tpoles(1, nbpoles);

                mbspc.Curve(1, tpoles2d);
                const gp_Pln& Pln = myHS1->Plane();
                //
                int ik;
                for (ik = 1; ik <= nbpoles; ik++)
                {
                  tpoles.SetValue(
                    ik,
                    ElSLib::Value(tpoles2d.Value(ik).X(), tpoles2d.Value(ik).Y(), Pln));
                }
                //
                occ::handle<Geom_BSplineCurve> BS = new Geom_BSplineCurve(tpoles,
                                                                          mbspc.Knots(),
                                                                          mbspc.Multiplicities(),
                                                                          mbspc.Degree());
                GeomLib_CheckBSplineCurve      Check(BS, TOLCHECK, TOLANGCHECK);
                Check.FixTangent(true, true);
                //
                IntTools_Curve aCurve;
                aCurve.SetCurve(BS);

                if (myApprox1)
                {
                  occ::handle<Geom2d_BSplineCurve> BS1 =
                    new Geom2d_BSplineCurve(tpoles2d,
                                            mbspc.Knots(),
                                            mbspc.Multiplicities(),
                                            mbspc.Degree());
                  GeomLib_Check2dBSplineCurve Check1(BS1, TOLCHECK, TOLANGCHECK);
                  Check1.FixTangent(true, true);
                  //
                  // ############################################
                  if (!rejectSurface && !reApprox)
                  {
                    bool isValid = IsCurveValid(BS1);
                    if (!isValid)
                    {
                      reApprox = true;
                      goto reapprox;
                    }
                  }
                  // ############################################
                  aCurve.SetFirstCurve2d(BS1);
                }

                if (myApprox2)
                {
                  mbspc.Curve(2, tpoles2d);

                  occ::handle<Geom2d_BSplineCurve> BS2 =
                    new Geom2d_BSplineCurve(tpoles2d,
                                            mbspc.Knots(),
                                            mbspc.Multiplicities(),
                                            mbspc.Degree());
                  GeomLib_Check2dBSplineCurve newCheck(BS2, TOLCHECK, TOLANGCHECK);
                  newCheck.FixTangent(true, true);

                  // ###########################################
                  if (!rejectSurface && !reApprox)
                  {
                    bool isValid = IsCurveValid(BS2);
                    if (!isValid)
                    {
                      reApprox = true;
                      goto reapprox;
                    }
                  }
                  // ###########################################
                  //
                  aCurve.SetSecondCurve2d(BS2);
                }
                //
                aCurve.SetTolerance(aTolC);
                //
                mySeqOfCurve.Append(aCurve);

              } // if(typs1 == GeomAbs_Plane) {

              else if (typs2 == GeomAbs_Plane)
              {
                const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
                nbpoles                                 = mbspc.NbPoles();

                NCollection_Array1<gp_Pnt2d> tpoles2d(1, nbpoles);
                NCollection_Array1<gp_Pnt>   tpoles(1, nbpoles);
                mbspc.Curve((myApprox1) ? 2 : 1, tpoles2d);
                const gp_Pln& Pln = myHS2->Plane();
                //
                int ik;
                for (ik = 1; ik <= nbpoles; ik++)
                {
                  tpoles.SetValue(
                    ik,
                    ElSLib::Value(tpoles2d.Value(ik).X(), tpoles2d.Value(ik).Y(), Pln));
                }
                //
                occ::handle<Geom_BSplineCurve> BS = new Geom_BSplineCurve(tpoles,
                                                                          mbspc.Knots(),
                                                                          mbspc.Multiplicities(),
                                                                          mbspc.Degree());
                GeomLib_CheckBSplineCurve      Check(BS, TOLCHECK, TOLANGCHECK);
                Check.FixTangent(true, true);
                //
                IntTools_Curve aCurve;
                aCurve.SetCurve(BS);
                aCurve.SetTolerance(aTolC);

                if (myApprox2)
                {
                  occ::handle<Geom2d_BSplineCurve> BS1 =
                    new Geom2d_BSplineCurve(tpoles2d,
                                            mbspc.Knots(),
                                            mbspc.Multiplicities(),
                                            mbspc.Degree());
                  GeomLib_Check2dBSplineCurve Check1(BS1, TOLCHECK, TOLANGCHECK);
                  Check1.FixTangent(true, true);
                  //
                  // ###########################################
                  if (!rejectSurface && !reApprox)
                  {
                    bool isValid = IsCurveValid(BS1);
                    if (!isValid)
                    {
                      reApprox = true;
                      goto reapprox;
                    }
                  }
                  // ###########################################
                  bPCurvesOk = CheckPCurve(BS1, myFace2, myContext);
                  aCurve.SetSecondCurve2d(BS1);
                }

                if (myApprox1)
                {
                  mbspc.Curve(1, tpoles2d);
                  occ::handle<Geom2d_BSplineCurve> BS2 =
                    new Geom2d_BSplineCurve(tpoles2d,
                                            mbspc.Knots(),
                                            mbspc.Multiplicities(),
                                            mbspc.Degree());
                  GeomLib_Check2dBSplineCurve Check2(BS2, TOLCHECK, TOLANGCHECK);
                  Check2.FixTangent(true, true);
                  //
                  // ###########################################
                  if (!rejectSurface && !reApprox)
                  {
                    bool isValid = IsCurveValid(BS2);
                    if (!isValid)
                    {
                      reApprox = true;
                      goto reapprox;
                    }
                  }
                  // ###########################################
                  bPCurvesOk = bPCurvesOk && CheckPCurve(BS2, myFace1, myContext);
                  aCurve.SetFirstCurve2d(BS2);
                }
                //
                // if points of the pcurves are out of the faces bounds
                // create 3d and 2d curves without approximation
                if (!bPCurvesOk)
                {
                  occ::handle<Geom2d_BSplineCurve> H1, H2;
                  bPCurvesOk = true;
                  //
                  occ::handle<Geom_Curve> aBSp = GeomInt_IntSS::MakeBSpline(WL, ifprm, ilprm);

                  if (myApprox1)
                  {
                    H1         = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, true);
                    bPCurvesOk = CheckPCurve(H1, myFace1, myContext);
                  }

                  if (myApprox2)
                  {
                    H2         = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, false);
                    bPCurvesOk = bPCurvesOk && CheckPCurve(H2, myFace2, myContext);
                  }
                  //
                  // if pcurves created without approximation are out of the
                  // faces bounds, use approximated 3d and 2d curves
                  if (bPCurvesOk)
                  {
                    IntTools_Curve aIC(aBSp, H1, H2, aTolC);
                    mySeqOfCurve.Append(aIC);
                  }
                  else
                  {
                    mySeqOfCurve.Append(aCurve);
                  }
                }
                else
                {
                  mySeqOfCurve.Append(aCurve);
                }

              } // else if(typs2 == GeomAbs_Plane)
              //
              else
              { // typs2 != GeomAbs_Plane && typs1 != GeomAbs_Plane
                bool                           bIsValid1, bIsValid2;
                occ::handle<Geom_BSplineCurve> BS;
                IntTools_Curve                 aCurve;
                //
                bIsValid1 = true;
                bIsValid2 = true;
                //
                const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
                nbpoles                                 = mbspc.NbPoles();
                NCollection_Array1<gp_Pnt> tpoles(1, nbpoles);
                mbspc.Curve(1, tpoles);
                BS = new Geom_BSplineCurve(tpoles,
                                           mbspc.Knots(),
                                           mbspc.Multiplicities(),
                                           mbspc.Degree());
                GeomLib_CheckBSplineCurve Check(BS, TOLCHECK, TOLANGCHECK);
                Check.FixTangent(true, true);
                //
                aCurve.SetCurve(BS);
                aCurve.SetTolerance(aTolC);
                //
                if (myApprox1)
                {
                  if (anApprox1)
                  {
                    occ::handle<Geom2d_BSplineCurve> BS1;
                    NCollection_Array1<gp_Pnt2d>     tpoles2d(1, nbpoles);
                    mbspc.Curve(2, tpoles2d);
                    //
                    BS1 = new Geom2d_BSplineCurve(tpoles2d,
                                                  mbspc.Knots(),
                                                  mbspc.Multiplicities(),
                                                  mbspc.Degree());
                    GeomLib_Check2dBSplineCurve newCheck(BS1, TOLCHECK, TOLANGCHECK);
                    newCheck.FixTangent(true, true);
                    //
                    if (!reApprox)
                    {
                      bIsValid1 = CheckPCurve(BS1, myFace1, myContext);
                    }
                    //
                    aCurve.SetFirstCurve2d(BS1);
                  }
                  else
                  {
                    occ::handle<Geom2d_BSplineCurve> BS1;
                    fprm = BS->FirstParameter();
                    lprm = BS->LastParameter();

                    occ::handle<Geom2d_Curve> C2d;
                    double                    aTol = myTolApprox;
                    GeomInt_IntSS::BuildPCurves(fprm, lprm, aTol, myHS1->Surface(), BS, C2d);
                    BS1 = occ::down_cast<Geom2d_BSplineCurve>(C2d);
                    aCurve.SetFirstCurve2d(BS1);
                  }
                } // if(myApprox1) {
                  //
                if (myApprox2)
                {
                  if (anApprox2)
                  {
                    occ::handle<Geom2d_BSplineCurve> BS2;
                    NCollection_Array1<gp_Pnt2d>     tpoles2d(1, nbpoles);
                    mbspc.Curve((myApprox1) ? 3 : 2, tpoles2d);
                    BS2 = new Geom2d_BSplineCurve(tpoles2d,
                                                  mbspc.Knots(),
                                                  mbspc.Multiplicities(),
                                                  mbspc.Degree());
                    GeomLib_Check2dBSplineCurve newCheck(BS2, TOLCHECK, TOLANGCHECK);
                    newCheck.FixTangent(true, true);
                    //
                    if (!reApprox)
                    {
                      bIsValid2 = CheckPCurve(BS2, myFace2, myContext);
                    }
                    aCurve.SetSecondCurve2d(BS2);
                  }
                  else
                  {
                    occ::handle<Geom2d_BSplineCurve> BS2;
                    fprm = BS->FirstParameter();
                    lprm = BS->LastParameter();

                    occ::handle<Geom2d_Curve> C2d;
                    double                    aTol = myTolApprox;
                    GeomInt_IntSS::BuildPCurves(fprm, lprm, aTol, myHS2->Surface(), BS, C2d);
                    BS2 = occ::down_cast<Geom2d_BSplineCurve>(C2d);
                    aCurve.SetSecondCurve2d(BS2);
                  }
                } // if(myApprox2) {
                if (!bIsValid1 || !bIsValid2)
                {
                  myTolApprox = aTolApproxImp; // 1.e-5;
                  tol2d       = myTolApprox;
                  reApprox    = true;
                  goto reapprox;
                }
                //
                mySeqOfCurve.Append(aCurve);
              }
            }
          }
        }
      } // else { // X
    } // case IntPatch_Walking:{
    break;

    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> RL = occ::down_cast<IntPatch_RLine>(L);

#ifdef INTTOOLS_FACEFACE_DEBUG
      RL->Dump(0);
#endif

      occ::handle<Geom_Curve>   aC3d;
      occ::handle<Geom2d_Curve> aC2d1, aC2d2;
      double                    aTolReached;
      GeomInt_IntSS::TreatRLine(RL, myHS1, myHS2, aC3d, aC2d1, aC2d2, aTolReached);

      if (aC3d.IsNull())
        break;

      Bnd_Box2d aBox1, aBox2;

      const double aU1f = myHS1->FirstUParameter(), aV1f = myHS1->FirstVParameter(),
                   aU1l = myHS1->LastUParameter(), aV1l = myHS1->LastVParameter();
      const double aU2f = myHS2->FirstUParameter(), aV2f = myHS2->FirstVParameter(),
                   aU2l = myHS2->LastUParameter(), aV2l = myHS2->LastVParameter();

      aBox1.Add(gp_Pnt2d(aU1f, aV1f));
      aBox1.Add(gp_Pnt2d(aU1l, aV1l));
      aBox2.Add(gp_Pnt2d(aU2f, aV2f));
      aBox2.Add(gp_Pnt2d(aU2l, aV2l));

      NCollection_Vector<double> anArrayOfParameters;

      // We consider here that the intersection line is same-parameter-line
      anArrayOfParameters.Append(aC3d->FirstParameter());
      anArrayOfParameters.Append(aC3d->LastParameter());

      GeomInt_IntSS::TrimILineOnSurfBoundaries(aC2d1, aC2d2, aBox1, aBox2, anArrayOfParameters);

      // Intersect with true boundaries. After that, enlarge bounding-boxes in order to
      // correct definition, if point on curve is inscribed in the box.
      aBox1.Enlarge(theToler);
      aBox2.Enlarge(theToler);

      const int aNbIntersSolutionsm1 = anArrayOfParameters.Length() - 1;

      // Trim RLine found.
      for (int anInd = 0; anInd < aNbIntersSolutionsm1; anInd++)
      {
        double &aParF = anArrayOfParameters(anInd), &aParL = anArrayOfParameters(anInd + 1);

        if ((aParL - aParF) <= Precision::PConfusion())
        {
          // In order to more precise extending to the boundaries of source curves.
          if (anInd < aNbIntersSolutionsm1 - 1)
            aParL = aParF;

          continue;
        }

        const double aPar = 0.5 * (aParF + aParL);
        gp_Pnt2d     aPt;

        occ::handle<Geom2d_Curve> aCurv2d1, aCurv2d2;
        if (!aC2d1.IsNull())
        {
          aC2d1->D0(aPar, aPt);

          if (aBox1.IsOut(aPt))
            continue;

          if (myApprox1)
            aCurv2d1 = new Geom2d_TrimmedCurve(aC2d1, aParF, aParL);
        }

        if (!aC2d2.IsNull())
        {
          aC2d2->D0(aPar, aPt);

          if (aBox2.IsOut(aPt))
            continue;

          if (myApprox2)
            aCurv2d2 = new Geom2d_TrimmedCurve(aC2d2, aParF, aParL);
        }

        occ::handle<Geom_Curve> aCurv3d = new Geom_TrimmedCurve(aC3d, aParF, aParL);

        IntTools_Curve aIC(aCurv3d, aCurv2d1, aCurv2d2);
        mySeqOfCurve.Append(aIC);
      }
    }
    break;
    default:
      break;
  }
}

//=================================================================================================

void Parameters(const occ::handle<GeomAdaptor_Surface>& HS1,
                const occ::handle<GeomAdaptor_Surface>& HS2,
                const gp_Pnt&                           Ptref,
                double&                                 U1,
                double&                                 V1,
                double&                                 U2,
                double&                                 V2)
{

  IntSurf_Quadric     quad1, quad2;
  GeomAbs_SurfaceType typs = HS1->GetType();

  switch (typs)
  {
    case GeomAbs_Plane:
      quad1.SetValue(HS1->Plane());
      break;
    case GeomAbs_Cylinder:
      quad1.SetValue(HS1->Cylinder());
      break;
    case GeomAbs_Cone:
      quad1.SetValue(HS1->Cone());
      break;
    case GeomAbs_Sphere:
      quad1.SetValue(HS1->Sphere());
      break;
    case GeomAbs_Torus:
      quad1.SetValue(HS1->Torus());
      break;
    default:
      throw Standard_ConstructionError("GeomInt_IntSS::MakeCurve");
  }

  typs = HS2->GetType();
  switch (typs)
  {
    case GeomAbs_Plane:
      quad2.SetValue(HS2->Plane());
      break;
    case GeomAbs_Cylinder:
      quad2.SetValue(HS2->Cylinder());
      break;
    case GeomAbs_Cone:
      quad2.SetValue(HS2->Cone());
      break;
    case GeomAbs_Sphere:
      quad2.SetValue(HS2->Sphere());
      break;
    case GeomAbs_Torus:
      quad2.SetValue(HS2->Torus());
      break;
    default:
      throw Standard_ConstructionError("GeomInt_IntSS::MakeCurve");
  }

  quad1.Parameters(Ptref, U1, V1);
  quad2.Parameters(Ptref, U2, V2);
}

//=================================================================================================

occ::handle<Geom_Curve> MakeBSpline(const occ::handle<IntPatch_WLine>& WL,
                                    const int                          ideb,
                                    const int                          ifin)
{
  int                        i, nbpnt = ifin - ideb + 1;
  NCollection_Array1<gp_Pnt> poles(1, nbpnt);
  NCollection_Array1<double> knots(1, nbpnt);
  NCollection_Array1<int>    mults(1, nbpnt);
  int                        ipidebm1;
  for (i = 1, ipidebm1 = i + ideb - 1; i <= nbpnt; ipidebm1++, i++)
  {
    poles(i) = WL->Point(ipidebm1).Value();
    mults(i) = 1;
    knots(i) = i - 1;
  }
  mults(1) = mults(nbpnt) = 2;
  return new Geom_BSplineCurve(poles, knots, mults, 1);
}

//=================================================================================================

void IntTools_FaceFace::PrepareLines3D(const bool bToSplit)
{
  int                                  i, aNbCurves;
  GeomAbs_SurfaceType                  aType1, aType2;
  NCollection_Sequence<IntTools_Curve> aNewCvs;
  //
  // 1. Treatment closed  curves
  aNbCurves = mySeqOfCurve.Length();
  for (i = 1; i <= aNbCurves; ++i)
  {
    const IntTools_Curve& aIC = mySeqOfCurve(i);
    //
    if (bToSplit)
    {
      int                                  j, aNbC;
      NCollection_Sequence<IntTools_Curve> aSeqCvs;
      //
      aNbC = IntTools_Tools::SplitCurve(aIC, aSeqCvs);
      if (aNbC)
      {
        for (j = 1; j <= aNbC; ++j)
        {
          const IntTools_Curve& aICNew = aSeqCvs(j);
          aNewCvs.Append(aICNew);
        }
      }
      else
      {
        aNewCvs.Append(aIC);
      }
    }
    else
    {
      aNewCvs.Append(aIC);
    }
  }
  //
  // 2. Plane\Cone intersection when we had 4 curves
  aType1    = myHS1->GetType();
  aType2    = myHS2->GetType();
  aNbCurves = aNewCvs.Length();
  //
  if ((aType1 == GeomAbs_Plane && aType2 == GeomAbs_Cone)
      || (aType2 == GeomAbs_Plane && aType1 == GeomAbs_Cone))
  {
    if (aNbCurves == 4)
    {
      GeomAbs_CurveType aCType1;
      //
      aCType1 = aNewCvs(1).Type();
      if (aCType1 == GeomAbs_Line)
      {
        NCollection_Sequence<IntTools_Curve> aSeqIn, aSeqOut;
        //
        for (i = 1; i <= aNbCurves; ++i)
        {
          const IntTools_Curve& aIC = aNewCvs(i);
          aSeqIn.Append(aIC);
        }
        //
        IntTools_Tools::RejectLines(aSeqIn, aSeqOut);
        //
        aNewCvs.Clear();
        aNbCurves = aSeqOut.Length();
        for (i = 1; i <= aNbCurves; ++i)
        {
          const IntTools_Curve& aIC = aSeqOut(i);
          aNewCvs.Append(aIC);
        }
      }
    }
  } // if ((aType1==GeomAbs_Plane && aType2==GeomAbs_Cone)...
  //
  // 3. Fill  mySeqOfCurve
  mySeqOfCurve.Clear();
  aNbCurves = aNewCvs.Length();
  for (i = 1; i <= aNbCurves; ++i)
  {
    const IntTools_Curve& aIC = aNewCvs(i);
    mySeqOfCurve.Append(aIC);
  }
}

//=================================================================================================

void CorrectSurfaceBoundaries(const TopoDS_Face& theFace,
                              const double       theTolerance,
                              double&            theumin,
                              double&            theumax,
                              double&            thevmin,
                              double&            thevmax)
{
  bool                      enlarge, isuperiodic, isvperiodic;
  double                    uinf, usup, vinf, vsup, delta;
  GeomAbs_SurfaceType       aType;
  occ::handle<Geom_Surface> aSurface;
  //
  aSurface = BRep_Tool::Surface(theFace);
  aSurface->Bounds(uinf, usup, vinf, vsup);
  delta   = theTolerance;
  enlarge = false;
  //
  GeomAdaptor_Surface anAdaptorSurface(aSurface);
  //
  if (aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_Surface> aBasisSurface =
      (occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface))->BasisSurface();

    if (aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))
        || aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      return;
    }
  }
  //
  if (aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_Surface> aBasisSurface =
      (occ::down_cast<Geom_OffsetSurface>(aSurface))->BasisSurface();

    if (aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))
        || aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      return;
    }
  }
  //
  isuperiodic = anAdaptorSurface.IsUPeriodic();
  isvperiodic = anAdaptorSurface.IsVPeriodic();
  //
  aType = anAdaptorSurface.GetType();
  if ((aType == GeomAbs_BezierSurface) || (aType == GeomAbs_BSplineSurface)
      || (aType == GeomAbs_SurfaceOfExtrusion) || (aType == GeomAbs_SurfaceOfRevolution)
      || (aType == GeomAbs_Cylinder))
  {
    enlarge = true;
  }
  //

  if (!isuperiodic && enlarge)
  {

    if (!Precision::IsInfinite(theumin) && ((theumin - uinf) > delta))
      theumin -= delta;
    else
    {
      theumin = uinf;
    }

    if (!Precision::IsInfinite(theumax) && ((usup - theumax) > delta))
      theumax += delta;
    else
      theumax = usup;
  }

  //
  if (!isvperiodic && enlarge)
  {
    if (!Precision::IsInfinite(thevmin) && ((thevmin - vinf) > delta))
    {
      thevmin -= delta;
    }
    else
    {
      thevmin = vinf;
    }
    if (!Precision::IsInfinite(thevmax) && ((vsup - thevmax) > delta))
    {
      thevmax += delta;
    }
    else
    {
      thevmax = vsup;
    }
  }

  //
  if (isuperiodic || isvperiodic)
  {
    bool            correct  = false;
    bool            correctU = false;
    bool            correctV = false;
    Bnd_Box2d       aBox;
    TopExp_Explorer anExp;

    for (anExp.Init(theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      if (BRep_Tool::IsClosed(TopoDS::Edge(anExp.Current()), theFace))
      {
        correct = true;
        double      f, l;
        TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());

        for (int i = 0; i < 2; i++)
        {
          if (i == 0)
          {
            anEdge.Orientation(TopAbs_FORWARD);
          }
          else
          {
            anEdge.Orientation(TopAbs_REVERSED);
          }
          occ::handle<Geom2d_Curve> aCurve = BRep_Tool::CurveOnSurface(anEdge, theFace, f, l);

          if (aCurve.IsNull())
          {
            correct = false;
            break;
          }
          occ::handle<Geom2d_Line> aLine = occ::down_cast<Geom2d_Line>(aCurve);

          if (aLine.IsNull())
          {
            correct = false;
            break;
          }
          gp_Dir2d anUDir(gp_Dir2d::D::X);
          gp_Dir2d aVDir(gp_Dir2d::D::Y);
          double   anAngularTolerance = Precision::Angular();

          correctU =
            correctU || aLine->Position().Direction().IsParallel(aVDir, anAngularTolerance);
          correctV =
            correctV || aLine->Position().Direction().IsParallel(anUDir, anAngularTolerance);

          gp_Pnt2d pp1 = aCurve->Value(f);
          aBox.Add(pp1);
          gp_Pnt2d pp2 = aCurve->Value(l);
          aBox.Add(pp2);
        }
        if (!correct)
          break;
      }
    }

    if (correct)
    {
      double umin, vmin, umax, vmax;
      aBox.Get(umin, vmin, umax, vmax);

      if (isuperiodic && correctU)
      {
        if (theumin < umin)
          theumin = umin;
        if (theumax > umax)
        {
          theumax = umax;
        }
      }
      if (isvperiodic && correctV)
      {
        if (thevmin < vmin)
          thevmin = vmin;
        if (thevmax > vmax)
          thevmax = vmax;
      }
    }
  }
}

// ------------------------------------------------------------------------------------------------
// static function: ParameterOutOfBoundary
// purpose:         Computes a new parameter for given curve. The corresponding 2d points
//                  does not lay on any boundary of given faces
// ------------------------------------------------------------------------------------------------
bool ParameterOutOfBoundary(const double                         theParameter,
                            const occ::handle<Geom_Curve>&       theCurve,
                            const TopoDS_Face&                   theFace1,
                            const TopoDS_Face&                   theFace2,
                            const double                         theOtherParameter,
                            const bool                           bIncreasePar,
                            const double                         theTol,
                            double&                              theNewParameter,
                            const occ::handle<IntTools_Context>& aContext)
{
  bool bIsComputed = false;
  theNewParameter  = theParameter;

  double       acurpar = theParameter;
  TopAbs_State aState  = TopAbs_ON;
  int          iter    = 0;
  double       asumtol = theTol;
  double       adelta  = asumtol * 0.1;
  adelta               = (adelta < Precision::Confusion()) ? Precision::Confusion() : adelta;
  occ::handle<Geom_Surface> aSurf1 = BRep_Tool::Surface(theFace1);
  occ::handle<Geom_Surface> aSurf2 = BRep_Tool::Surface(theFace2);

  double u1, u2, v1, v2;

  GeomAPI_ProjectPointOnSurf aPrj1;
  aSurf1->Bounds(u1, u2, v1, v2);
  aPrj1.Init(aSurf1, u1, u2, v1, v2);

  GeomAPI_ProjectPointOnSurf aPrj2;
  aSurf2->Bounds(u1, u2, v1, v2);
  aPrj2.Init(aSurf2, u1, u2, v1, v2);

  while (aState == TopAbs_ON)
  {
    if (bIncreasePar)
      acurpar += adelta;
    else
      acurpar -= adelta;
    gp_Pnt aPCurrent = theCurve->Value(acurpar);
    aPrj1.Perform(aPCurrent);
    double U = 0., V = 0.;

    if (aPrj1.IsDone())
    {
      aPrj1.LowerDistanceParameters(U, V);
      aState = aContext->StatePointFace(theFace1, gp_Pnt2d(U, V));
    }

    if (aState != TopAbs_ON)
    {
      aPrj2.Perform(aPCurrent);

      if (aPrj2.IsDone())
      {
        aPrj2.LowerDistanceParameters(U, V);
        aState = aContext->StatePointFace(theFace2, gp_Pnt2d(U, V));
      }
    }

    if (iter > 11)
    {
      break;
    }
    iter++;
  }

  if (iter <= 11)
  {
    theNewParameter = acurpar;
    bIsComputed     = true;

    if (bIncreasePar)
    {
      if (acurpar >= theOtherParameter)
        theNewParameter = theOtherParameter;
    }
    else
    {
      if (acurpar <= theOtherParameter)
        theNewParameter = theOtherParameter;
    }
  }
  return bIsComputed;
}

//=================================================================================================

bool IsCurveValid(const occ::handle<Geom2d_Curve>& thePCurve)
{
  if (thePCurve.IsNull())
    return false;

  double              tolint = 1.e-10;
  Geom2dAdaptor_Curve PCA;
  IntRes2d_Domain     PCD;
  Geom2dInt_GInter    PCI;

  double   pf = 0., pl = 0.;
  gp_Pnt2d pntf, pntl;

  if (!thePCurve->IsClosed() && !thePCurve->IsPeriodic())
  {
    pf   = thePCurve->FirstParameter();
    pl   = thePCurve->LastParameter();
    pntf = thePCurve->Value(pf);
    pntl = thePCurve->Value(pl);
    PCA.Load(thePCurve);
    if (!PCA.IsPeriodic())
    {
      if (PCA.FirstParameter() > pf)
        pf = PCA.FirstParameter();
      if (PCA.LastParameter() < pl)
        pl = PCA.LastParameter();
    }
    PCD.SetValues(pntf, pf, tolint, pntl, pl, tolint);
    PCI.Perform(PCA, PCD, tolint, tolint);
    if (PCI.IsDone())
      if (PCI.NbPoints() > 0)
      {
        return false;
      }
  }

  return true;
}

//=======================================================================
// static function : ApproxWithPCurves
// purpose  : for bug 20964 only
//=======================================================================
bool ApproxWithPCurves(const gp_Cylinder& theCyl, const gp_Sphere& theSph)
{
  bool   bRes = true;
  double R1 = theCyl.Radius(), R2 = theSph.Radius();
  //
  {
    double aD2, aRc2, aEps;
    gp_Pnt aApexSph;
    //
    aEps = 1.E-7;
    aRc2 = R1 * R1;
    //
    const gp_Ax3& aAx3Sph = theSph.Position();
    const gp_Pnt& aLocSph = aAx3Sph.Location();
    const gp_Dir& aDirSph = aAx3Sph.Direction();
    //
    const gp_Ax1& aAx1Cyl = theCyl.Axis();
    gp_Lin        aLinCyl(aAx1Cyl);
    //
    aApexSph.SetXYZ(aLocSph.XYZ() + R2 * aDirSph.XYZ());
    aD2 = aLinCyl.SquareDistance(aApexSph);
    if (fabs(aD2 - aRc2) < aEps)
    {
      return !bRes;
    }
    //
    aApexSph.SetXYZ(aLocSph.XYZ() - R2 * aDirSph.XYZ());
    aD2 = aLinCyl.SquareDistance(aApexSph);
    if (fabs(aD2 - aRc2) < aEps)
    {
      return !bRes;
    }
  }
  //

  if (R1 < 2. * R2)
  {
    return bRes;
  }
  gp_Lin anCylAx(theCyl.Axis());

  double aDist = anCylAx.Distance(theSph.Location());
  double aDRel = std::abs(aDist - R1) / R2;

  if (aDRel > .2)
    return bRes;

  double par = ElCLib::Parameter(anCylAx, theSph.Location());
  gp_Pnt aP  = ElCLib::Value(par, anCylAx);
  gp_Vec aV(aP, theSph.Location());

  double dd = aV.Dot(theSph.Position().XDirection());

  if (aDist < R1 && dd > 0.)
    return false;
  if (aDist > R1 && dd < 0.)
    return false;

  return bRes;
}

//=================================================================================================

void PerformPlanes(const occ::handle<GeomAdaptor_Surface>& theS1,
                   const occ::handle<GeomAdaptor_Surface>& theS2,
                   const double                            TolF1,
                   const double                            TolF2,
                   const double                            TolAng,
                   const double                            TolTang,
                   const bool                              theApprox1,
                   const bool                              theApprox2,
                   NCollection_Sequence<IntTools_Curve>&   theSeqOfCurve,
                   bool&                                   theTangentFaces)
{

  gp_Pln aPln1 = theS1->Plane();
  gp_Pln aPln2 = theS2->Plane();

  IntAna_QuadQuadGeo aPlnInter(aPln1, aPln2, TolAng, TolTang);

  if (!aPlnInter.IsDone())
  {
    theTangentFaces = false;
    return;
  }

  IntAna_ResultType aResType = aPlnInter.TypeInter();

  if (aResType == IntAna_Same)
  {
    theTangentFaces = true;
    return;
  }

  theTangentFaces = false;

  if (aResType == IntAna_Empty)
  {
    return;
  }

  gp_Lin aLin = aPlnInter.Line(1);

  ProjLib_Plane aProj;

  aProj.Init(aPln1);
  aProj.Project(aLin);
  gp_Lin2d aLin2d1 = aProj.Line();
  //
  aProj.Init(aPln2);
  aProj.Project(aLin);
  gp_Lin2d aLin2d2 = aProj.Line();
  //
  // classify line2d1 relatively first plane
  double P11, P12;
  bool   IsCrossed = ClassifyLin2d(theS1, aLin2d1, TolTang, P11, P12);
  if (!IsCrossed)
    return;
  // classify line2d2 relatively second plane
  double P21, P22;
  IsCrossed = ClassifyLin2d(theS2, aLin2d2, TolTang, P21, P22);
  if (!IsCrossed)
    return;

  // Analysis of parametric intervals: must have common part

  if (P21 >= P12)
    return;
  if (P22 <= P11)
    return;

  double pmin, pmax;
  pmin = std::max(P11, P21);
  pmax = std::min(P12, P22);

  if (pmax - pmin <= TolTang)
    return;

  occ::handle<Geom_Line> aGLin = new Geom_Line(aLin);

  IntTools_Curve                 aCurve;
  occ::handle<Geom_TrimmedCurve> aGTLin = new Geom_TrimmedCurve(aGLin, pmin, pmax);

  aCurve.SetCurve(aGTLin);

  if (theApprox1)
  {
    occ::handle<Geom2d_Line> C2d = new Geom2d_Line(aLin2d1);
    aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else
  {
    occ::handle<Geom2d_Curve> H1;
    aCurve.SetFirstCurve2d(H1);
  }
  if (theApprox2)
  {
    occ::handle<Geom2d_Line> C2d = new Geom2d_Line(aLin2d2);
    aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else
  {
    occ::handle<Geom2d_Curve> H1;
    aCurve.SetFirstCurve2d(H1);
  }
  //
  // Valid tolerance for the intersection curve between planar faces
  // is the maximal tolerance between tolerances of faces
  double aTolC = std::max(TolF1, TolF2);
  aCurve.SetTolerance(aTolC);
  //
  // Computation of the tangential tolerance
  double anAngle, aDt;
  gp_Dir aD1, aD2;
  //
  aD1     = aPln1.Position().Direction();
  aD2     = aPln2.Position().Direction();
  anAngle = aD1.Angle(aD2);
  //
  aDt             = IntTools_Tools::ComputeIntRange(TolF1, TolF2, anAngle);
  double aTangTol = sqrt(aDt * aDt + TolF1 * TolF1);
  //
  aCurve.SetTangentialTolerance(aTangTol);
  //
  theSeqOfCurve.Append(aCurve);
}

//=================================================================================================

static inline bool INTER(const double d1, const double d2, const double tol)
{
  return (d1 > tol && d2 < -tol) || (d1 < -tol && d2 > tol)
         || ((d1 <= tol && d1 >= -tol) && (d2 > tol || d2 < -tol))
         || ((d2 <= tol && d2 >= -tol) && (d1 > tol || d1 < -tol));
}

static inline bool COINC(const double d1, const double d2, const double tol)
{
  return (d1 <= tol && d1 >= -tol) && (d2 <= tol && d2 >= -tol);
}

bool ClassifyLin2d(const occ::handle<GeomAdaptor_Surface>& theS,
                   const gp_Lin2d&                         theLin2d,
                   const double                            theTol,
                   double&                                 theP1,
                   double&                                 theP2)

{
  double xmin, xmax, ymin, ymax, d1, d2, A, B, C;
  double par[2];
  int    nbi = 0;

  xmin = theS->FirstUParameter();
  xmax = theS->LastUParameter();
  ymin = theS->FirstVParameter();
  ymax = theS->LastVParameter();

  theLin2d.Coefficients(A, B, C);

  // xmin, ymin <-> xmin, ymax
  d1 = A * xmin + B * ymin + C;
  d2 = A * xmin + B * ymax + C;

  if (INTER(d1, d2, theTol))
  {
    // Intersection with boundary
    double y = -(C + A * xmin) / B;
    par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, y));
    nbi++;
  }
  else if (COINC(d1, d2, theTol))
  {
    // Coincidence with boundary
    par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
    par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
    nbi    = 2;
  }

  if (nbi == 2)
  {

    if (fabs(par[0] - par[1]) > theTol)
    {
      theP1 = std::min(par[0], par[1]);
      theP2 = std::max(par[0], par[1]);
      return true;
    }
    else
      return false;
  }

  // xmin, ymax <-> xmax, ymax
  d1 = d2;
  d2 = A * xmax + B * ymax + C;

  if (d1 > theTol || d1 < -theTol)
  { // to avoid checking of
    // coincidence with the same point
    if (INTER(d1, d2, theTol))
    {
      double x = -(C + B * ymax) / A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymax));
      nbi++;
    }
    else if (COINC(d1, d2, theTol))
    {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      nbi    = 2;
    }
  }

  if (nbi == 2)
  {

    if (fabs(par[0] - par[1]) > theTol)
    {
      theP1 = std::min(par[0], par[1]);
      theP2 = std::max(par[0], par[1]);
      return true;
    }
    else
      return false;
  }

  // xmax, ymax <-> xmax, ymin
  d1 = d2;
  d2 = A * xmax + B * ymin + C;

  if (d1 > theTol || d1 < -theTol)
  {
    if (INTER(d1, d2, theTol))
    {
      double y = -(C + A * xmax) / B;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, y));
      nbi++;
    }
    else if (COINC(d1, d2, theTol))
    {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      nbi    = 2;
    }
  }

  if (nbi == 2)
  {
    if (fabs(par[0] - par[1]) > theTol)
    {
      theP1 = std::min(par[0], par[1]);
      theP2 = std::max(par[0], par[1]);
      return true;
    }
    else
      return false;
  }

  // xmax, ymin <-> xmin, ymin
  d1 = d2;
  d2 = A * xmin + B * ymin + C;

  if (d1 > theTol || d1 < -theTol)
  {
    if (INTER(d1, d2, theTol))
    {
      double x = -(C + B * ymin) / A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymin));
      nbi++;
    }
    else if (COINC(d1, d2, theTol))
    {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
      nbi    = 2;
    }
  }

  if (nbi == 2)
  {
    if (fabs(par[0] - par[1]) > theTol)
    {
      theP1 = std::min(par[0], par[1]);
      theP2 = std::max(par[0], par[1]);
      return true;
    }
    else
      return false;
  }

  return false;
}

//
//=================================================================================================

void ApproxParameters(const occ::handle<GeomAdaptor_Surface>& aHS1,
                      const occ::handle<GeomAdaptor_Surface>& aHS2,
                      int&                                    iDegMin,
                      int&                                    iDegMax,
                      int&                                    iNbIter)

{
  GeomAbs_SurfaceType aTS1, aTS2;

  //
  iNbIter = 0;
  iDegMin = 4;
  iDegMax = 8;
  //
  aTS1 = aHS1->GetType();
  aTS2 = aHS2->GetType();
  //
  // Cylinder/Torus
  if ((aTS1 == GeomAbs_Cylinder && aTS2 == GeomAbs_Torus)
      || (aTS2 == GeomAbs_Cylinder && aTS1 == GeomAbs_Torus))
  {
    double      aRC, aRT, dR, aPC;
    gp_Cylinder aCylinder;
    gp_Torus    aTorus;
    //
    aPC = Precision::Confusion();
    //
    aCylinder = (aTS1 == GeomAbs_Cylinder) ? aHS1->Cylinder() : aHS2->Cylinder();
    aTorus    = (aTS1 == GeomAbs_Torus) ? aHS1->Torus() : aHS2->Torus();
    //
    aRC = aCylinder.Radius();
    aRT = aTorus.MinorRadius();
    dR  = aRC - aRT;
    if (dR < 0.)
    {
      dR = -dR;
    }
    //
    if (dR < aPC)
    {
      iDegMax = 6;
    }
  }
  if (aTS1 == GeomAbs_Cylinder && aTS2 == GeomAbs_Cylinder)
  {
    iNbIter = 1;
  }
}

//=================================================================================================

void Tolerances(const occ::handle<GeomAdaptor_Surface>& aHS1,
                const occ::handle<GeomAdaptor_Surface>& aHS2,
                double&                                 aTolTang)
{
  GeomAbs_SurfaceType aTS1, aTS2;
  //
  aTS1 = aHS1->GetType();
  aTS2 = aHS2->GetType();
  //
  // Cylinder/Torus
  if ((aTS1 == GeomAbs_Cylinder && aTS2 == GeomAbs_Torus)
      || (aTS2 == GeomAbs_Cylinder && aTS1 == GeomAbs_Torus))
  {
    double      aRC, aRT, dR, aPC;
    gp_Cylinder aCylinder;
    gp_Torus    aTorus;
    //
    aPC = Precision::Confusion();
    //
    aCylinder = (aTS1 == GeomAbs_Cylinder) ? aHS1->Cylinder() : aHS2->Cylinder();
    aTorus    = (aTS1 == GeomAbs_Torus) ? aHS1->Torus() : aHS2->Torus();
    //
    aRC = aCylinder.Radius();
    aRT = aTorus.MinorRadius();
    dR  = aRC - aRT;
    if (dR < 0.)
    {
      dR = -dR;
    }
    //
    if (dR < aPC)
    {
      aTolTang = 0.1 * aTolTang;
    }
  }
}

//=================================================================================================

bool SortTypes(const GeomAbs_SurfaceType aType1, const GeomAbs_SurfaceType aType2)
{
  bool bRet;
  int  aI1, aI2;
  //
  bRet = false;
  //
  aI1 = IndexType(aType1);
  aI2 = IndexType(aType2);
  if (aI1 < aI2)
  {
    bRet = !bRet;
  }
  return bRet;
}

//=================================================================================================

int IndexType(const GeomAbs_SurfaceType aType)
{
  int aIndex;
  //
  aIndex = 11;
  //
  if (aType == GeomAbs_Plane)
  {
    aIndex = 0;
  }
  else if (aType == GeomAbs_Cylinder)
  {
    aIndex = 1;
  }
  else if (aType == GeomAbs_Cone)
  {
    aIndex = 2;
  }
  else if (aType == GeomAbs_Sphere)
  {
    aIndex = 3;
  }
  else if (aType == GeomAbs_Torus)
  {
    aIndex = 4;
  }
  else if (aType == GeomAbs_BezierSurface)
  {
    aIndex = 5;
  }
  else if (aType == GeomAbs_BSplineSurface)
  {
    aIndex = 6;
  }
  else if (aType == GeomAbs_SurfaceOfRevolution)
  {
    aIndex = 7;
  }
  else if (aType == GeomAbs_SurfaceOfExtrusion)
  {
    aIndex = 8;
  }
  else if (aType == GeomAbs_OffsetSurface)
  {
    aIndex = 9;
  }
  else if (aType == GeomAbs_OtherSurface)
  {
    aIndex = 10;
  }
  return aIndex;
}

//=================================================================================================

double FindMaxDistance(const occ::handle<Geom_Curve>&       theCurve,
                       const double                         theFirst,
                       const double                         theLast,
                       const TopoDS_Face&                   theFace,
                       const occ::handle<IntTools_Context>& theContext)
{
  int    aNbS;
  double aT1, aT2, aDt, aD, aDMax, anEps;
  //
  aNbS  = 11;
  aDt   = (theLast - theFirst) / aNbS;
  aDMax = 0.;
  anEps = 1.e-4 * aDt;
  //
  GeomAPI_ProjectPointOnSurf& aProjPS = theContext->ProjPS(theFace);
  aT2                                 = theFirst;
  for (;;)
  {
    aT1 = aT2;
    aT2 += aDt;
    //
    if (aT2 > theLast)
    {
      break;
    }
    //
    aD = FindMaxDistance(theCurve, aT1, aT2, aProjPS, anEps);
    if (aD > aDMax)
    {
      aDMax = aD;
    }
  }
  //
  return aDMax;
}

//=================================================================================================

double FindMaxDistance(const occ::handle<Geom_Curve>& theC,
                       const double                   theFirst,
                       const double                   theLast,
                       GeomAPI_ProjectPointOnSurf&    theProjPS,
                       const double                   theEps)
{
  double aA, aB, aCf, aX, aX1, aX2, aF1, aF2, aF;
  //
  aCf = 0.61803398874989484820458683436564; //(sqrt(5.)-1)/2.;
  aA  = theFirst;
  aB  = theLast;
  //
  aX1 = aB - aCf * (aB - aA);
  aF1 = MaxDistance(theC, aX1, theProjPS);
  aX2 = aA + aCf * (aB - aA);
  aF2 = MaxDistance(theC, aX2, theProjPS);

  while (std::abs(aX1 - aX2) > theEps)
  {
    if (aF1 > aF2)
    {
      aB  = aX2;
      aX2 = aX1;
      aF2 = aF1;
      aX1 = aB - aCf * (aB - aA);
      aF1 = MaxDistance(theC, aX1, theProjPS);
    }
    else
    {
      aA  = aX1;
      aX1 = aX2;
      aF1 = aF2;
      aX2 = aA + aCf * (aB - aA);
      aF2 = MaxDistance(theC, aX2, theProjPS);
    }
  }
  //
  aX = 0.5 * (aA + aB);
  aF = MaxDistance(theC, aX, theProjPS);
  //
  if (aF1 > aF)
  {
    aF = aF1;
  }
  //
  if (aF2 > aF)
  {
    aF = aF2;
  }
  //
  return aF;
}

//=================================================================================================

double MaxDistance(const occ::handle<Geom_Curve>& theC,
                   const double                   aT,
                   GeomAPI_ProjectPointOnSurf&    theProjPS)
{
  double aD;
  gp_Pnt aP;
  //
  theC->D0(aT, aP);
  theProjPS.Perform(aP);
  aD = theProjPS.NbPoints() ? theProjPS.LowerDistance() : 0.;
  //
  return aD;
}

//=======================================================================
// function : CheckPCurve
// purpose  : Checks if points of the pcurve are out of the face bounds.
//=======================================================================
bool CheckPCurve(const occ::handle<Geom2d_Curve>&     aPC,
                 const TopoDS_Face&                   aFace,
                 const occ::handle<IntTools_Context>& theCtx)
{
  const int NPoints = 23;
  int       i;
  double    umin, umax, vmin, vmax;

  theCtx->UVBounds(aFace, umin, umax, vmin, vmax);
  double tolU = std::max((umax - umin) * 0.01, Precision::Confusion());
  double tolV = std::max((vmax - vmin) * 0.01, Precision::Confusion());
  double fp   = aPC->FirstParameter();
  double lp   = aPC->LastParameter();

  // adjust domain for periodic surfaces
  TopLoc_Location           aLoc;
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace, aLoc);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    aSurf = (occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf))->BasisSurface();
  }
  gp_Pnt2d pnt = aPC->Value((fp + lp) / 2);
  double   u, v;
  pnt.Coord(u, v);
  //
  if (aSurf->IsUPeriodic())
  {
    double aPer   = aSurf->UPeriod();
    int    nshift = (int)((u - umin) / aPer);
    if (u < umin + aPer * nshift)
      nshift--;
    umin += aPer * nshift;
    umax += aPer * nshift;
  }
  if (aSurf->IsVPeriodic())
  {
    double aPer   = aSurf->VPeriod();
    int    nshift = (int)((v - vmin) / aPer);
    if (v < vmin + aPer * nshift)
      nshift--;
    vmin += aPer * nshift;
    vmax += aPer * nshift;
  }
  //
  //--------------------------------------------------------
  bool     bRet;
  int      j, aNbIntervals;
  double   aT, dT;
  gp_Pnt2d aP2D;
  //
  Geom2dAdaptor_Curve aGAC(aPC);
  aNbIntervals = aGAC.NbIntervals(GeomAbs_CN);
  //
  NCollection_Array1<double> aTI(1, aNbIntervals + 1);
  aGAC.Intervals(aTI, GeomAbs_CN);
  //
  bRet = false;
  //
  aT = aGAC.FirstParameter();
  for (j = 1; j <= aNbIntervals; ++j)
  {
    dT = (aTI(j + 1) - aTI(j)) / NPoints;
    //
    for (i = 1; i < NPoints; i++)
    {
      aT = aT + dT;
      aGAC.D0(aT, aP2D);
      aP2D.Coord(u, v);
      if (umin - u > tolU || u - umax > tolU || vmin - v > tolV || v - vmax > tolV)
      {
        return bRet;
      }
    }
  }
  return !bRet;
}

//=================================================================================================

void CorrectPlaneBoundaries(double& aUmin, double& aUmax, double& aVmin, double& aVmax)
{
  if (!(Precision::IsInfinite(aUmin) || Precision::IsInfinite(aUmax)))
  {
    double dU;
    //
    dU    = 0.1 * (aUmax - aUmin);
    aUmin = aUmin - dU;
    aUmax = aUmax + dU;
  }
  if (!(Precision::IsInfinite(aVmin) || Precision::IsInfinite(aVmax)))
  {
    double dV;
    //
    dV    = 0.1 * (aVmax - aVmin);
    aVmin = aVmin - dV;
    aVmax = aVmax + dV;
  }
}
