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

#include <ShapeConstruct_ProjectCurveOnSurface.hxx>

#include <Approx_CurveOnSurface.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurve_IntConicConic.hxx>
#include <IntRes2d_Domain.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeExtend.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(ShapeConstruct_ProjectCurveOnSurface, Standard_Transient)

namespace
{
//! Default number of control points for discretization.
constexpr int THE_NCONTROL = 23;

//! Utility class for projecting points onto a surface with B-spline corner cache optimization.
//! For clamped B-spline surfaces, caches corner pole positions and their exact UV parameters
//! to avoid expensive ValueOfUV calls when projected points coincide with surface corners.
//! Only corner poles are cached because B-spline surfaces pass through corners only
//! (when end multiplicities equal degree + 1).
class SurfaceProjectorWithCache
{
public:
  //! Constructor - initializes the projector with a surface.
  //! For B-spline surfaces, builds the corner cache automatically.
  //! @param[in] theSurf the surface analysis object
  SurfaceProjectorWithCache(const occ::handle<ShapeAnalysis_Surface>& theSurf)
      : mySurf(theSurf)
  {
    if (theSurf.IsNull())
      return;

    occ::handle<Geom_BSplineSurface> aBSplineSurf =
      occ::down_cast<Geom_BSplineSurface>(theSurf->Surface());
    if (!aBSplineSurf.IsNull())
    {
      buildCornerCache(aBSplineSurf);
    }
  }

  //! Projects a 3D point onto the surface.
  //! First checks B-spline corner cache, then falls back to ValueOfUV.
  //! @param[in] thePoint the 3D point to project
  //! @param[in] theTol the tolerance for projection
  //! @param[in] theTolSq squared tolerance for corner matching
  //! @return the UV coordinates on the surface
  gp_Pnt2d ValueOfUV(const gp_Pnt& thePoint, const double theTol, const double theTolSq) const
  {
    gp_Pnt2d aResult;
    if (findInCornerCache(thePoint, theTolSq, aResult))
    {
      // Corner UV is exact, but refine for numerical stability
      return mySurf->NextValueOfUV(aResult, thePoint, theTol, theTol);
    }
    return mySurf->ValueOfUV(thePoint, theTol);
  }

  //! Projects a 3D point onto the surface using a hint from previous projection.
  //! First checks B-spline corner cache, then falls back to NextValueOfUV.
  //! @param[in] theHint the UV hint from previous projection
  //! @param[in] thePoint the 3D point to project
  //! @param[in] theTol the tolerance for projection
  //! @param[in] theTolSq squared tolerance for corner matching
  //! @param[in] theStep the step tolerance
  //! @return the UV coordinates on the surface
  gp_Pnt2d NextValueOfUV(const gp_Pnt2d& theHint,
                         const gp_Pnt&   thePoint,
                         const double    theTol,
                         const double    theTolSq,
                         const double    theStep) const
  {
    gp_Pnt2d aResult;
    if (findInCornerCache(thePoint, theTolSq, aResult))
    {
      // Corner UV is exact, but refine for numerical stability
      return mySurf->NextValueOfUV(aResult, thePoint, theTol, theTol);
    }
    return mySurf->NextValueOfUV(theHint, thePoint, theTol, theStep);
  }

  //! Returns the gap from the last projection
  double Gap() const { return mySurf->Gap(); }

private:
  //! Builds cache from B-spline surface corner poles.
  //! For clamped B-splines (multiplicity = degree + 1 at ends), the surface passes
  //! exactly through corner poles, so we can use their UV parameters directly.
  //! @param[in] theSurface the B-spline surface
  void buildCornerCache(const occ::handle<Geom_BSplineSurface>& theSurface)
  {
    const int aNbPolesU = theSurface->NbUPoles();
    const int aNbPolesV = theSurface->NbVPoles();
    const int aDegreeU  = theSurface->UDegree();
    const int aDegreeV  = theSurface->VDegree();

    // Check if surface is clamped (end multiplicities = degree + 1)
    const int aFirstUMult = theSurface->UMultiplicity(1);
    const int aLastUMult  = theSurface->UMultiplicity(theSurface->NbUKnots());
    const int aFirstVMult = theSurface->VMultiplicity(1);
    const int aLastVMult  = theSurface->VMultiplicity(theSurface->NbVKnots());

    const bool isUFirstClamped = (aFirstUMult >= aDegreeU + 1);
    const bool isULastClamped  = (aLastUMult >= aDegreeU + 1);
    const bool isVFirstClamped = (aFirstVMult >= aDegreeV + 1);
    const bool isVLastClamped  = (aLastVMult >= aDegreeV + 1);

    // Get parameter bounds
    const double aUFirst = theSurface->UKnot(1);
    const double aULast  = theSurface->UKnot(theSurface->NbUKnots());
    const double aVFirst = theSurface->VKnot(1);
    const double aVLast  = theSurface->VKnot(theSurface->NbVKnots());

    // Cache corner poles where surface passes through them
    // Corner (1, 1) - UFirst, VFirst
    if (isUFirstClamped && isVFirstClamped)
    {
      myCorners3d.Append(theSurface->Pole(1, 1));
      myCorners2d.Append(gp_Pnt2d(aUFirst, aVFirst));
    }

    // Corner (NbPolesU, 1) - ULast, VFirst
    if (isULastClamped && isVFirstClamped)
    {
      myCorners3d.Append(theSurface->Pole(aNbPolesU, 1));
      myCorners2d.Append(gp_Pnt2d(aULast, aVFirst));
    }

    // Corner (1, NbPolesV) - UFirst, VLast
    if (isUFirstClamped && isVLastClamped)
    {
      myCorners3d.Append(theSurface->Pole(1, aNbPolesV));
      myCorners2d.Append(gp_Pnt2d(aUFirst, aVLast));
    }

    // Corner (NbPolesU, NbPolesV) - ULast, VLast
    if (isULastClamped && isVLastClamped)
    {
      myCorners3d.Append(theSurface->Pole(aNbPolesU, aNbPolesV));
      myCorners2d.Append(gp_Pnt2d(aULast, aVLast));
    }
  }

  //! Finds the closest corner to a 3D point within tolerance.
  //! @param[in] thePoint the 3D point to find
  //! @param[in] theTolSq squared tolerance for matching
  //! @param[out] theUV the UV parameter if found
  //! @return true if a matching corner was found
  bool findInCornerCache(const gp_Pnt& thePoint, const double theTolSq, gp_Pnt2d& theUV) const
  {
    for (int i = 0; i < myCorners3d.Length(); ++i)
    {
      if (myCorners3d(i).SquareDistance(thePoint) < theTolSq)
      {
        theUV = myCorners2d(i);
        return true;
      }
    }
    return false;
  }

private:
  occ::handle<ShapeAnalysis_Surface> mySurf;      //!< Surface to project on
  NCollection_Vector<gp_Pnt>         myCorners3d; //!< 3D positions of B-spline surface corners
  NCollection_Vector<gp_Pnt2d>       myCorners2d; //!< UV parameters of B-spline surface corners
};

//=================================================================================================

//! Extracts B-spline curve from a possibly nested trimmed curve.
//! Recursively unwraps trimmed curves to find B-spline basis.
//! @param[in] theCurve the curve to extract from
//! @return the extracted B-spline curve, or null if not a B-spline
occ::handle<Geom_BSplineCurve> extractBSplineCurve(const occ::handle<Geom_Curve>& theCurve)
{
  occ::handle<Geom_Curve> aCurve = theCurve;

  // Recursively unwrap trimmed curves
  while (!aCurve.IsNull() && aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    aCurve = occ::down_cast<Geom_TrimmedCurve>(aCurve)->BasisCurve();
  }

  return occ::down_cast<Geom_BSplineCurve>(aCurve);
}

//=================================================================================================

//! Adjusts second point to first point considering surface periodicity.
//! For periodic surfaces, adjusts the second point coordinates to be within
//! half-period of the first point.
//! @param[in] theFirstPoint the reference point
//! @param[in,out] theSecondPoint the point to adjust
//! @param[in] theSurf the surface to check for periodicity
void adjustSecondToFirstPoint(const gp_Pnt2d&                  theFirstPoint,
                              gp_Pnt2d&                        theSecondPoint,
                              const occ::handle<Geom_Surface>& theSurf)
{
  if (theSurf->IsUPeriodic())
  {
    const double anUPeriod = theSurf->UPeriod();
    const double aNewU     = ElCLib::InPeriod(theSecondPoint.X(),
                                          theFirstPoint.X() - anUPeriod / 2,
                                          theFirstPoint.X() + anUPeriod / 2);
    theSecondPoint.SetX(aNewU);
  }
  if (theSurf->IsVPeriodic())
  {
    const double aVPeriod = theSurf->VPeriod();
    const double aNewV    = ElCLib::InPeriod(theSecondPoint.Y(),
                                          theFirstPoint.Y() - aVPeriod / 2,
                                          theFirstPoint.Y() + aVPeriod / 2);
    theSecondPoint.SetY(aNewV);
  }
}

//=================================================================================================

//! Fixes possible period jumps in an array of 4 points.
//! Handles the walking period parameter to ensure smooth transition
//! across periodic boundaries.
//! @param[in,out] thePnt array of 4 points to fix
//! @param[in] theIdx coordinate index (1 for U, 2 for V)
//! @param[in] thePeriod the period value
//! @param[in] theSavedPoint index of the reference point (-1 for none)
//! @param[in] theSavedParam the saved parameter value
//! @return true if a period jump was detected and fixed
bool fixPeriodicityTroubles(gp_Pnt2d*    thePnt,
                            const int    theIdx,
                            const double thePeriod,
                            const int    theSavedPoint,
                            const double theSavedParam)
{
  double aSavedParam;
  int    aSavedPoint;
  double aMinParam = 0.0, aMaxParam = thePeriod;

  if (theSavedPoint < 0)
  {
    aSavedParam = 0.5 * thePeriod;
    aSavedPoint = 0;
  }
  else
  {
    aSavedParam = theSavedParam;
    aSavedPoint = theSavedPoint;
    while (aMinParam > aSavedParam)
    {
      aMinParam -= thePeriod;
      aMaxParam -= thePeriod;
    }
    while (aMaxParam < aSavedParam)
    {
      aMinParam += thePeriod;
      aMaxParam += thePeriod;
    }
  }

  double aFixIsoParam = aMinParam;
  bool   isIsoLine    = false;
  if (aMaxParam - aSavedParam < Precision::PConfusion()
      || aSavedParam - aMinParam < Precision::PConfusion())
  {
    aFixIsoParam = aSavedParam;
    isIsoLine    = true;
  }

  // Normalize all coordinates to [aMinParam, aMaxParam)
  for (int i = 0; i < 4; i++)
  {
    double aParam = thePnt[i].Coord(theIdx);
    double aShift = ShapeAnalysis::AdjustToPeriod(aParam, aMinParam, aMaxParam);
    aParam += aShift;

    if (isIsoLine)
    {
      if (aMaxParam - aParam < Precision::PConfusion()
          || aParam - aMinParam < Precision::PConfusion())
        aParam = aFixIsoParam;
    }
    else
    {
      if (aMaxParam - aParam < Precision::PConfusion())
        aParam = aMaxParam;
      if (aParam - aMinParam < Precision::PConfusion())
        aParam = aMinParam;
    }

    thePnt[i].SetCoord(theIdx, aParam);
  }

  // Find possible period jump and increasing or decreasing coordinates vector
  bool   isJump    = false;
  double aPrevDiff = 0.0;
  double aSumDiff  = 1.0;
  for (int i = 0; i < 3; i++)
  {
    double aDiff = thePnt[i + 1].Coord(theIdx) - thePnt[i].Coord(theIdx);
    if (aDiff < -Precision::PConfusion())
    {
      aSumDiff *= -1.0;
    }
    if (aDiff * aPrevDiff < -Precision::PConfusion())
    {
      isJump = true;
    }
    aPrevDiff = aDiff;
  }

  if (!isJump)
    return false;

  if (aSumDiff > 0)
  {
    for (int i = aSavedPoint; i > 0; i--)
      if (thePnt[i].Coord(theIdx) > thePnt[i - 1].Coord(theIdx))
      {
        thePnt[i - 1].SetCoord(theIdx, thePnt[i - 1].Coord(theIdx) + thePeriod);
      }
    for (int i = aSavedPoint; i < 3; i++)
      if (thePnt[i].Coord(theIdx) < thePnt[i + 1].Coord(theIdx))
      {
        thePnt[i + 1].SetCoord(theIdx, thePnt[i + 1].Coord(theIdx) - thePeriod);
      }
  }
  else
  {
    for (int i = aSavedPoint; i > 0; i--)
      if (thePnt[i].Coord(theIdx) < thePnt[i - 1].Coord(theIdx))
      {
        thePnt[i - 1].SetCoord(theIdx, thePnt[i - 1].Coord(theIdx) - thePeriod);
      }
    for (int i = aSavedPoint; i < 3; i++)
      if (thePnt[i].Coord(theIdx) > thePnt[i + 1].Coord(theIdx))
      {
        thePnt[i + 1].SetCoord(theIdx, thePnt[i + 1].Coord(theIdx) + thePeriod);
      }
  }

  return true;
}

//=================================================================================================

//! Checks if B-spline curve has uneven parameterization requiring special handling.
//! Computes the ratio of maximum to minimum parameterization speed across knot intervals.
//! If this ratio exceeds a threshold, the curve should be projected using ProjLib instead
//! of the standard approximation approach.
//! @param[in] theCurve the curve to check (may be trimmed)
//! @param[in] theFirst the first parameter of the working range
//! @param[in] theLast the last parameter of the working range
//! @param[out] theBSpline the extracted B-spline curve (if any)
//! @return true if the curve has uneven parameterization requiring ProjLib
bool isBSplineCurveInvalid(const occ::handle<Geom_Curve>&  theCurve,
                           const double                    theFirst,
                           const double                    theLast,
                           occ::handle<Geom_BSplineCurve>& theBSpline)
{
  theBSpline = extractBSplineCurve(theCurve);
  if (theBSpline.IsNull())
    return false;

  // Compute parametrization speed on each knot interval inside [theFirst, theLast].
  // If quotient = (MaxSpeed / MinSpeed) >= aMaxQuotientCoeff then use PerformByProjLib.
  double aFirstParam = theFirst;
  double aLastParam  = theLast;

  // Find first knot index
  int anIdx = 1;
  for (; anIdx <= theBSpline->NbKnots() && aFirstParam < theLast; anIdx++)
  {
    if (theBSpline->Knot(anIdx) > theFirst)
    {
      break;
    }
  }

  GeomAdaptor_Curve            aC3DAdaptor(theCurve);
  double                       aMinParSpeed = Precision::Infinite();
  NCollection_Sequence<double> aKnotCoeffs;

  for (; anIdx <= theBSpline->NbKnots() && aFirstParam < theLast; anIdx++)
  {
    // Fill current knot interval
    aLastParam     = std::min(theLast, theBSpline->Knot(anIdx));
    int aNbIntPnts = THE_NCONTROL;

    // Adapt number of inner points according to the length of the interval
    // to avoid a lot of calculations on small range of parameters.
    if (anIdx > 1)
    {
      const double aLenThres = 1.e-2;
      const double aLenRatio =
        (aLastParam - aFirstParam) / (theBSpline->Knot(anIdx) - theBSpline->Knot(anIdx - 1));
      if (aLenRatio < aLenThres)
      {
        aNbIntPnts = int(aLenRatio / aLenThres * aNbIntPnts);
        if (aNbIntPnts < 2)
          aNbIntPnts = 2;
      }
    }

    double aStep = (aLastParam - aFirstParam) / (aNbIntPnts - 1);
    gp_Pnt p3d1, p3d2;

    // Start filling from first point
    aC3DAdaptor.D0(aFirstParam, p3d1);

    double aLength3d = 0.0;
    for (int anIntIdx = 1; anIntIdx < aNbIntPnts; anIntIdx++)
    {
      double aParam = aFirstParam + aStep * anIntIdx;
      aC3DAdaptor.D0(aParam, p3d2);
      const double aDist = p3d2.Distance(p3d1);

      aLength3d += aDist;
      p3d1 = p3d2;

      aMinParSpeed = std::min(aMinParSpeed, aDist / aStep);
    }

    const double aCoeff = aLength3d / (aLastParam - aFirstParam);
    if (std::abs(aCoeff) > gp::Resolution())
      aKnotCoeffs.Append(aCoeff);
    aFirstParam = aLastParam;
  }

  double anEvenlyCoeff = 0;
  if (aKnotCoeffs.Size() > 0)
  {
    anEvenlyCoeff = *std::max_element(aKnotCoeffs.begin(), aKnotCoeffs.end())
                    / *std::min_element(aKnotCoeffs.begin(), aKnotCoeffs.end());
  }

  const double aMaxQuotientCoeff = 1500.0;
  return (anEvenlyCoeff > aMaxQuotientCoeff && aMinParSpeed > Precision::Confusion());
}

//=================================================================================================

//! Generates discretization points for a curve.
//! Uses uniform distribution for general curves, and adjusts the number of points
//! for B-splines based on their knot structure to ensure adequate sampling.
//! @param[in] theCurve the curve to discretize
//! @param[in] theFirst the first parameter
//! @param[in] theLast the last parameter
//! @param[in] theNbControlPoints the minimum number of points
//! @param[out] thePoints the generated 3D points (resized as needed)
//! @param[out] theParams the corresponding parameters (resized as needed)
//! @return the actual number of generated points
int generateCurvePoints(const occ::handle<Geom_Curve>&                     theCurve,
                        const double                                       theFirst,
                        const double                                       theLast,
                        const int                                          theNbControlPoints,
                        ShapeConstruct_ProjectCurveOnSurface::ArrayOfPnt&  thePoints,
                        ShapeConstruct_ProjectCurveOnSurface::ArrayOfReal& theParams)
{
  occ::handle<Geom_BSplineCurve> aBSpline = extractBSplineCurve(theCurve);
  int                            aNbPini  = theNbControlPoints;

  if (!aBSpline.IsNull())
  {
    int aUsedKnots = 0;
    for (int i = 1; i < aBSpline->NbKnots(); i++)
    {
      if (aBSpline->Knot(i + 1) > theFirst && aBSpline->Knot(i) < theLast)
      {
        aUsedKnots++;
      }
    }
    int aMinPnt = aUsedKnots * (aBSpline->Degree() + 1);
    while (aNbPini < aMinPnt)
    {
      aNbPini += THE_NCONTROL - 1;
    }
  }

  thePoints.Resize(1, aNbPini, false);
  theParams.Resize(1, aNbPini, false);

  const double aDeltaParam = (theLast - theFirst) / (aNbPini - 1);

  for (int i = 1; i <= aNbPini; ++i)
  {
    double aParam;
    if (i == 1)
      aParam = theFirst;
    else if (i == aNbPini)
      aParam = theLast;
    else
      aParam = theFirst + (i - 1) * aDeltaParam;

    gp_Pnt aPoint;
    theCurve->D0(aParam, aPoint);
    thePoints.SetValue(i, aPoint);
    theParams.SetValue(i, aParam);
  }

  return aNbPini;
}

//=================================================================================================

//! Wrapper for ShapeAnalysis_Surface::ProjectDegenerated.
//! Converts NCollection_Array1 containers to sequences, performs projection,
//! then copies results back. Required because ShapeAnalysis_Surface uses
//! sequence containers.
//! @param[in] theSurf the analysis surface
//! @param[in] theNbPnt the number of points
//! @param[in] thePoints the 3D points array
//! @param[in,out] thePoints2d the 2D points array (adjusted for degenerate regions)
//! @param[in] thePreci the precision
//! @param[in] theDirect true to check from start, false to check from end
void projectDegeneratedPoints(const occ::handle<ShapeAnalysis_Surface>& theSurf,
                              const int                                 theNbPnt,
                              const NCollection_Array1<gp_Pnt>&         thePoints,
                              NCollection_Array1<gp_Pnt2d>&             thePoints2d,
                              const double                              thePreci,
                              const bool                                theDirect)
{
  // Use incremental allocator for sequences when there are enough elements
  // to benefit from pooled memory allocation.
  constexpr int THE_ALLOC_THRESHOLD = 100;

  occ::handle<NCollection_BaseAllocator> anAlloc;
  if (theNbPnt > THE_ALLOC_THRESHOLD)
  {
    anAlloc = new NCollection_IncAllocator(NCollection_IncAllocator::THE_MINIMUM_BLOCK_SIZE);
  }

  // Convert arrays to sequences for ShapeAnalysis_Surface::ProjectDegenerated
  NCollection_Sequence<gp_Pnt>   aPoints3d(anAlloc);
  NCollection_Sequence<gp_Pnt2d> aPoints2d(anAlloc);

  for (int i = 1; i <= theNbPnt; ++i)
  {
    aPoints3d.Append(thePoints(i));
    aPoints2d.Append(thePoints2d(i));
  }

  // Call the method that expects sequences
  theSurf->ProjectDegenerated(theNbPnt, aPoints3d, aPoints2d, thePreci, theDirect);

  // Copy results back to array
  for (int i = 1; i <= theNbPnt; ++i)
  {
    thePoints2d.SetValue(i, aPoints2d.Value(i));
  }
}

} // namespace

//=================================================================================================

ShapeConstruct_ProjectCurveOnSurface::ShapeConstruct_ProjectCurveOnSurface()
    : myPreci(Precision::Confusion()),
      myStatus(ShapeExtend::EncodeStatus(ShapeExtend_OK)),
      myAdjustOverDegen(1)
{
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::Init(const occ::handle<Geom_Surface>& theSurf,
                                                const double                     thePreci)
{
  Init(new ShapeAnalysis_Surface(theSurf), thePreci);
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::Init(const occ::handle<ShapeAnalysis_Surface>& theSurf,
                                                const double                              thePreci)
{
  SetSurface(theSurf);
  SetPrecision(thePreci);
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetSurface(const occ::handle<Geom_Surface>& theSurf)
{
  SetSurface(new ShapeAnalysis_Surface(theSurf));
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetSurface(
  const occ::handle<ShapeAnalysis_Surface>& theSurf)
{
  if (mySurf == theSurf)
    return;
  mySurf  = theSurf;
  myCache = CacheArray();
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetPrecision(const double thePreci)
{
  myPreci = thePreci;
}

//=================================================================================================

int& ShapeConstruct_ProjectCurveOnSurface::AdjustOverDegenMode()
{
  return myAdjustOverDegen;
}

//=================================================================================================

bool ShapeConstruct_ProjectCurveOnSurface::Status(const ShapeExtend_Status theStatus) const
{
  return ShapeExtend::DecodeStatus(myStatus, theStatus);
}

//=================================================================================================

bool ShapeConstruct_ProjectCurveOnSurface::Perform(const occ::handle<Geom_Curve>& theC3D,
                                                   const double                   theFirst,
                                                   const double                   theLast,
                                                   occ::handle<Geom2d_Curve>&     theC2D,
                                                   const double                   theTolFirst,
                                                   const double                   theTolLast)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  if (mySurf.IsNull() || theC3D.IsNull())
  {
    theC2D.Nullify();
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }

  // Try analytical projection first
  occ::handle<Geom_Curve> aCurve3DTrim = theC3D;
  if (!theC3D->IsKind(STANDARD_TYPE(Geom_BoundedCurve)))
  {
    aCurve3DTrim = new Geom_TrimmedCurve(theC3D, theFirst, theLast);
  }

  theC2D = projectAnalytic(aCurve3DTrim);
  if (!theC2D.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return true;
  }

  // Handle problematic B-spline curves with uneven parameterization
  occ::handle<Geom_BSplineCurve> aBSpline;
  if (isBSplineCurveInvalid(theC3D, theFirst, theLast, aBSpline))
  {
    // Use ProjLib for curves with highly uneven parameterization speed
    if (PerformByProjLib(theC3D, theFirst, theLast, theC2D))
    {
      return Status(ShapeExtend_DONE);
    }
  }

  // Generate curve points
  ArrayOfPnt  aPoints;
  ArrayOfReal aParams;
  const int aNbPnt = generateCurvePoints(theC3D, theFirst, theLast, THE_NCONTROL, aPoints, aParams);

  // Approximate pcurve
  ArrayOfPnt2d aPoints2d(1, aNbPnt);
  for (int i = 1; i <= aNbPnt; i++)
  {
    aPoints2d.SetValue(i, gp_Pnt2d(0., 0.));
  }

  approxPCurve(aNbPnt, theC3D, theTolFirst, theTolLast, aPoints, aParams, aPoints2d, theC2D);

  const int aNbPini = aPoints.Length();
  if (!theC2D.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    return true;
  }

  // Interpolate the result
  theC2D = interpolatePCurve(aNbPini, aPoints2d, aParams);

  myStatus |= ShapeExtend::EncodeStatus(theC2D.IsNull() ? ShapeExtend_FAIL1 : ShapeExtend_DONE2);
  return Status(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeConstruct_ProjectCurveOnSurface::PerformByProjLib(const occ::handle<Geom_Curve>& theC3D,
                                                            const double                   theFirst,
                                                            const double                   theLast,
                                                            occ::handle<Geom2d_Curve>&     theC2D)
{
  theC2D.Nullify();
  if (mySurf.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }

  try
  {
    OCC_CATCH_SIGNALS
    occ::handle<GeomAdaptor_Surface> aGAS = mySurf->Adaptor3d();
    occ::handle<GeomAdaptor_Curve>   aGAC = new GeomAdaptor_Curve(theC3D, theFirst, theLast);
    ProjLib_ProjectedCurve           aProjector(aGAS, aGAC);

    switch (aProjector.GetType())
    {
      case GeomAbs_Line:
        theC2D = new Geom2d_Line(aProjector.Line());
        break;
      case GeomAbs_Circle:
        theC2D = new Geom2d_Circle(aProjector.Circle());
        break;
      case GeomAbs_Ellipse:
        theC2D = new Geom2d_Ellipse(aProjector.Ellipse());
        break;
      case GeomAbs_Parabola:
        theC2D = new Geom2d_Parabola(aProjector.Parabola());
        break;
      case GeomAbs_Hyperbola:
        theC2D = new Geom2d_Hyperbola(aProjector.Hyperbola());
        break;
      case GeomAbs_BSplineCurve:
        theC2D = aProjector.BSpline();
        break;
      default:
        break;
    }

    if (theC2D.IsNull())
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return false;
    }
    else
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return true;
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::PerformByProjLib(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    theC2D.Nullify();
  }
  return false;
}

//=================================================================================================

occ::handle<Geom2d_Curve> ShapeConstruct_ProjectCurveOnSurface::projectAnalytic(
  const occ::handle<Geom_Curve>& theC3D) const
{
  occ::handle<Geom2d_Curve> aResult;

  occ::handle<Geom_Surface> aSurf  = mySurf->Surface();
  occ::handle<Geom_Plane>   aPlane = occ::down_cast<Geom_Plane>(aSurf);

  if (aPlane.IsNull())
  {
    occ::handle<Geom_RectangularTrimmedSurface> aRTS =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf);
    if (!aRTS.IsNull())
      aPlane = occ::down_cast<Geom_Plane>(aRTS->BasisSurface());
    else
    {
      occ::handle<Geom_OffsetSurface> anOS = occ::down_cast<Geom_OffsetSurface>(aSurf);
      if (!anOS.IsNull())
        aPlane = occ::down_cast<Geom_Plane>(anOS->BasisSurface());
    }
  }

  if (!aPlane.IsNull())
  {
    occ::handle<Geom_Curve> aProjOnPlane =
      GeomProjLib::ProjectOnPlane(theC3D, aPlane, aPlane->Position().Direction(), true);
    occ::handle<GeomAdaptor_Curve> aHC = new GeomAdaptor_Curve(aProjOnPlane);
    ProjLib_ProjectedCurve         aProj(mySurf->Adaptor3d(), aHC);

    aResult = Geom2dAdaptor::MakeCurve(aProj);
    if (aResult.IsNull())
      return aResult;

    if (aResult->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> aTC = occ::down_cast<Geom2d_TrimmedCurve>(aResult);
      aResult                              = aTC->BasisCurve();
    }

    return aResult;
  }

  return aResult;
}

//=================================================================================================

occ::handle<Geom2d_Curve> ShapeConstruct_ProjectCurveOnSurface::getLine(
  const ArrayOfPnt&  thePoints,
  const ArrayOfReal& theParams,
  ArrayOfPnt2d&      thePoints2d,
  const double       theTol,
  bool&              theIsRecompute,
  bool&              theIsFromCache) const
{
  const int aNb = thePoints.Length();
  gp_Pnt    aP[4];
  aP[0] = thePoints(1);
  aP[1] = thePoints(2);
  aP[2] = thePoints(aNb - 1);
  aP[3] = thePoints(aNb);

  gp_Pnt2d aP2d[4];
  int      i = 0;

  double     aTol2       = theTol * theTol;
  double     aTolWorking = theTol;
  const bool isPeriodicU = mySurf->Surface()->IsUPeriodic();
  const bool isPeriodicV = mySurf->Surface()->IsVPeriodic();

  // Protection against bad tolerance shapes
  if (aTol2 > 1.0)
  {
    aTolWorking = Precision::Confusion();
    aTol2       = aTolWorking * aTolWorking;
  }
  if (aTol2 < Precision::SquareConfusion())
    aTol2 = Precision::SquareConfusion();
  const double anOldTol2 = aTol2;

  int      aSavedPointNum = -1;
  gp_Pnt2d aSavedPoint;

  // Create projector with B-spline surface pole cache optimization
  SurfaceProjectorWithCache aProjector(mySurf);

  // Helper lambda to project a point with cache lookup
  auto projectPoint = [&](const gp_Pnt& thePoint, gp_Pnt2d& theResult, const int theIndex) -> void {
    // Try existing endpoint cache first
    const int aNbCache = myCache.Length();
    for (int j = 0; j < aNbCache; ++j)
    {
      const CachePoint& aCachePnt = myCache(j);
      if (aCachePnt.first.SquareDistance(thePoint) < aTol2)
      {
        theResult =
          aProjector.NextValueOfUV(aCachePnt.second, thePoint, aTolWorking, aTol2, aTolWorking);
        aSavedPointNum = theIndex;
        aSavedPoint    = aCachePnt.second;
        if (theIndex == 0)
          theIsFromCache = true;
        return;
      }
    }

    // Fall back to full projection (with B-spline pole cache optimization inside)
    theResult = aProjector.ValueOfUV(thePoint, aTolWorking, aTol2);
  };

  // Project first and last points
  for (; i < 4; i += 3)
  {
    projectPoint(aP[i], aP2d[i], i);

    const double aDist    = aProjector.Gap();
    const double aCurDist = aDist * aDist;
    if (aTol2 < aCurDist)
      aTol2 = aCurDist;
  }

  if (isPeriodicU || isPeriodicV)
  {
    // Compute second and last but one c2d points
    for (i = 1; i < 3; i++)
    {
      projectPoint(aP[i], aP2d[i], i);

      const double aDist    = aProjector.Gap();
      const double aCurDist = aDist * aDist;
      if (aTol2 < aCurDist)
        aTol2 = aCurDist;
    }

    if (isPeriodicU)
    {
      theIsRecompute = fixPeriodicityTroubles(&aP2d[0],
                                              1,
                                              mySurf->Surface()->UPeriod(),
                                              aSavedPointNum,
                                              aSavedPoint.X());
    }

    if (isPeriodicV)
    {
      theIsRecompute = fixPeriodicityTroubles(&aP2d[0],
                                              2,
                                              mySurf->Surface()->VPeriod(),
                                              aSavedPointNum,
                                              aSavedPoint.Y());
    }
  }

  if (theIsRecompute && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    return nullptr;
  }

  thePoints2d.SetValue(1, aP2d[0]);
  thePoints2d.SetValue(aNb, aP2d[3]);

  // Restore old tolerance
  aTol2 = anOldTol2;

  const double dPar = theParams(aNb) - theParams(1);
  if (std::abs(dPar) < Precision::PConfusion())
    return nullptr;

  const gp_Vec2d            aVec0(aP2d[0], aP2d[3]);
  const gp_Vec2d            aVec          = aVec0 / dPar;
  occ::handle<Geom_Surface> aSurf         = mySurf->Surface();
  bool                      isNormalCheck = aSurf->IsCNu(1) && aSurf->IsCNv(1);

  if (isNormalCheck)
  {
    for (i = 1; i <= aNb; i++)
    {
      const gp_XY aCurPoint = aP2d[0].XY() + aVec.XY() * (theParams(i) - theParams(1));
      gp_Pnt      aCurP;
      gp_Vec      aNormalVec, aDu, aDv;
      aSurf->D1(aCurPoint.X(), aCurPoint.Y(), aCurP, aDu, aDv);
      aNormalVec = aDu ^ aDv;
      if (aNormalVec.SquareMagnitude() < Precision::SquareConfusion())
      {
        isNormalCheck = false;
        break;
      }
      const gp_Lin aNormalLine(aCurP, gp_Dir(aNormalVec));
      const double aDist = aNormalLine.Distance(thePoints(i));
      if (aDist > aTolWorking)
        return nullptr;
    }
  }

  if (!isNormalCheck)
  {
    const double aFirstPointDist =
      mySurf->Surface()->Value(aP2d[0].X(), aP2d[0].Y()).SquareDistance(thePoints(1));
    aTol2 = std::max(aTol2, aTol2 * 2 * aFirstPointDist);
    for (i = 2; i < aNb; i++)
    {
      const gp_XY aCurPoint = aP2d[0].XY() + aVec.XY() * (theParams(i) - theParams(1));
      gp_Pnt      aCurP;
      aSurf->D0(aCurPoint.X(), aCurPoint.Y(), aCurP);
      const double aDist1 = aCurP.SquareDistance(thePoints(i));

      if (std::abs(aFirstPointDist - aDist1) > aTol2)
        return nullptr;
    }
  }

  // Check if pcurve can be represented by Geom2d_Line
  const double aLLength = aVec0.Magnitude();
  if (std::abs(aLLength - dPar) <= Precision::PConfusion())
  {
    const gp_XY    aDirL = aVec0.XY() / aLLength;
    const gp_Pnt2d aPL(aP2d[0].XY() - theParams(1) * aDirL);
    return new Geom2d_Line(aPL, gp_Dir2d(aDirL));
  }

  // Create straight bspline
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = aP2d[0];
  aPoles(2) = aP2d[3];

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = theParams(1);
  aKnots(2) = theParams(theParams.Length());

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;

  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1);
}

//=================================================================================================

bool ShapeConstruct_ProjectCurveOnSurface::approxPCurve(const int                      theNbPnt,
                                                        const occ::handle<Geom_Curve>& theC3D,
                                                        const double                   theTolFirst,
                                                        const double                   theTolLast,
                                                        ArrayOfPnt&                    thePoints,
                                                        ArrayOfReal&                   theParams,
                                                        ArrayOfPnt2d&                  thePoints2d,
                                                        occ::handle<Geom2d_Curve>&     theC2D)
{
  // For performance, first try to handle typical case when pcurve is straight
  bool isRecompute     = false;
  bool isFromCacheLine = false;

  theC2D = getLine(thePoints, theParams, thePoints2d, myPreci, isRecompute, isFromCacheLine);
  if (!theC2D.IsNull())
  {
    // Fill cache
    bool aChangeCycle = false;
    if (!myCache.IsEmpty()
        && myCache(0).first.Distance(thePoints(1)) > myCache(0).first.Distance(thePoints(theNbPnt))
        && myCache(0).first.Distance(thePoints(theNbPnt)) < Precision::Confusion())
    {
      aChangeCycle = true;
    }

    myCache.Resize(0, 1, false);
    if (aChangeCycle)
    {
      myCache(0) = CachePoint(thePoints(1), thePoints2d(1));
      myCache(1) = CachePoint(thePoints(theNbPnt), thePoints2d(theNbPnt));
    }
    else
    {
      myCache(0) = CachePoint(thePoints(theNbPnt), thePoints2d(theNbPnt));
      myCache(1) = CachePoint(thePoints(1), thePoints2d(1));
    }
    return true;
  }

  bool isDone = true;

  // Test if the curve 3d is a boundary of the surface
  bool                    isoParam, isoPar2d3d, isoTypeU, p1OnIso, p2OnIso, isoClosed;
  gp_Pnt2d                valueP1, valueP2;
  occ::handle<Geom_Curve> cIso;
  double                  t1, t2;

  occ::handle<Standard_Type> sType      = mySurf->Surface()->DynamicType();
  bool                       isAnalytic = true;
  if (sType == STANDARD_TYPE(Geom_BezierSurface) || sType == STANDARD_TYPE(Geom_BSplineSurface))
    isAnalytic = false;

  double uf, ul, vf, vl;
  mySurf->Surface()->Bounds(uf, ul, vf, vl);
  isoClosed = false;

  ArrayOfReal pout(1, theNbPnt);
  for (int i = 1; i <= theNbPnt; i++)
    pout.SetValue(i, 0.0);

  isoParam = isAnIsoparametric(theNbPnt,
                               thePoints,
                               theParams,
                               isoTypeU,
                               p1OnIso,
                               valueP1,
                               p2OnIso,
                               valueP2,
                               isoPar2d3d,
                               cIso,
                               t1,
                               t2,
                               pout);

  // Projection of the points on surfaces
  gp_Pnt   p3d;
  gp_Pnt2d p2d;
  double   isoValue = 0., isoPar1 = 0., isoPar2 = 0., tPar = 0., tdeb, tfin;
  double   Cf, Cl, parf, parl;

  if (isoParam)
  {
    if (isoTypeU)
    {
      isoValue  = valueP1.X();
      isoPar1   = valueP1.Y();
      isoPar2   = valueP2.Y();
      isoClosed = mySurf->IsVClosed(myPreci);
      parf      = vf;
      parl      = vl;
    }
    else
    {
      isoValue  = valueP1.Y();
      isoPar1   = valueP1.X();
      isoPar2   = valueP2.X();
      isoClosed = mySurf->IsUClosed(myPreci);
      parf      = uf;
      parl      = ul;
    }
    if (!isoPar2d3d && !isAnalytic)
    {
      Cf = cIso->FirstParameter();
      Cl = cIso->LastParameter();
      if (Precision::IsInfinite(Cf))
        Cf = -1000;
      if (Precision::IsInfinite(Cl))
        Cl = +1000;

      tdeb = pout(2);

      if (isoClosed && (isoPar1 == parf || isoPar1 == parl))
      {
        if (std::abs(tdeb - parf) < std::abs(tdeb - parl))
          isoPar1 = parf;
        else
          isoPar1 = parl;
        if (isoTypeU)
          valueP1.SetY(isoPar1);
        else
          valueP1.SetX(isoPar1);
      }
      if (isoClosed && (isoPar2 == parf || isoPar2 == parl))
      {
        tfin = pout(theNbPnt - 1);
        if (std::abs(tfin - parf) < std::abs(tfin - parl))
          isoPar2 = parf;
        else
          isoPar2 = parl;
        if (isoTypeU)
          valueP2.SetY(isoPar2);
        else
          valueP2.SetX(isoPar2);
      }

      if (!isoClosed)
      {
        if ((std::abs(tdeb - isoPar1) > std::abs(tdeb - isoPar2))
            && (std::abs(pout(theNbPnt - 1) - isoPar2) > std::abs(pout(theNbPnt - 1) - isoPar1)))
        {
          gp_Pnt2d valueTmp = valueP1;
          valueP1           = valueP2;
          valueP2           = valueTmp;
          if (isoTypeU)
          {
            isoValue = valueP1.X();
            isoPar1  = valueP1.Y();
            isoPar2  = valueP2.Y();
          }
          else
          {
            isoValue = valueP1.Y();
            isoPar1  = valueP1.X();
            isoPar2  = valueP2.X();
          }
        }
      }
    }
  }

  const double Up           = ul - uf;
  const double Vp           = vl - vf;
  double       gap          = myPreci;
  bool         aChangeCycle = false;

  bool     isFromCache = false;
  gp_Pnt2d aSavedPoint;

  if (!myCache.IsEmpty()
      && myCache(0).first.Distance(thePoints(1)) > myCache(0).first.Distance(thePoints(theNbPnt)))
  {
    if (myCache(0).first.Distance(thePoints(theNbPnt)) < Precision::Confusion())
      aChangeCycle = true;
  }

  bool     needResolveUJump = false;
  bool     needResolveVJump = false;
  gp_Pnt   prevP3d;
  gp_Pnt2d prevP2d;

  for (int ii = 1; ii <= theNbPnt; ii++)
  {
    const int aPntIndex = aChangeCycle ? (theNbPnt - ii + 1) : ii;
    p3d                 = thePoints(aPntIndex);

    if (isoParam)
    {
      if (isoPar2d3d)
      {
        if (isoPar2 > isoPar1)
          tPar = theParams(aPntIndex);
        else
          tPar = t1 + t2 - theParams(aPntIndex);
      }
      else if (!isAnalytic)
      {
        if (aPntIndex == 1)
          tPar = isoPar1;
        else if (aPntIndex == theNbPnt)
          tPar = isoPar2;
        else
        {
          tPar = pout(aPntIndex);
        }
      }

      if (!isoPar2d3d && isAnalytic)
      {
        if (aPntIndex == 1)
          p2d = valueP1;
        else if (aPntIndex == theNbPnt)
          p2d = valueP2;
        else
        {
          p2d = mySurf->NextValueOfUV(p2d, p3d, myPreci, Precision::Confusion() + 1000 * gap);
          gap = mySurf->Gap();
        }
      }
      else
      {
        if (isoTypeU)
        {
          p2d.SetX(isoValue);
          p2d.SetY(tPar);
        }
        else
        {
          p2d.SetX(tPar);
          p2d.SetY(isoValue);
        }
      }
    }
    else
    {
      if ((aPntIndex == 1) && p1OnIso)
        p2d = valueP1;
      else if ((aPntIndex == theNbPnt) && p2OnIso)
        p2d = valueP2;
      else
      {
        if (aPntIndex == 1 || aPntIndex == theNbPnt)
        {
          if (!isRecompute)
          {
            p2d = thePoints2d(aPntIndex);
            gap = mySurf->Gap();
            if (aPntIndex == 1)
            {
              isFromCache = isFromCacheLine;
              aSavedPoint = p2d;
            }
            continue;
          }
          else
          {
            int       j        = 0;
            const int aNbCache = myCache.Length();
            for (; j < aNbCache; ++j)
            {
              const CachePoint& aCachePnt = myCache(j);
              if (aCachePnt.first.SquareDistance(p3d) < myPreci * myPreci)
              {
                p2d = mySurf->NextValueOfUV(aCachePnt.second,
                                            p3d,
                                            myPreci,
                                            Precision::Confusion() + gap);
                if (aPntIndex == 1)
                {
                  isFromCache = true;
                  aSavedPoint = aCachePnt.second;
                }
                break;
              }
            }
            if (j >= aNbCache)
            {
              p2d = mySurf->ValueOfUV(p3d, myPreci);
            }
          }
        }
        else
        {
          p2d = mySurf->NextValueOfUV(p2d, p3d, myPreci, Precision::Confusion() + 1000 * gap);
        }
        gap = mySurf->Gap();
      }
    }
    thePoints2d.SetValue(aPntIndex, p2d);

    if (theNbPnt > 23 && ii > 2 && ii < theNbPnt)
    {
      if (fabs(p2d.X() - prevP2d.X()) > 0.95 * Up && prevP3d.Distance(p3d) < myPreci
          && !mySurf->IsUClosed(myPreci) && mySurf->NbSingularities(myPreci) > 0
          && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
      {
        needResolveUJump = true;
      }
      if (fabs(p2d.Y() - prevP2d.Y()) > 0.95 * Vp && prevP3d.Distance(p3d) < myPreci
          && !mySurf->IsVClosed(myPreci) && mySurf->NbSingularities(myPreci) > 0
          && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
      {
        needResolveVJump = true;
      }
    }
    prevP3d = p3d;
    prevP2d = p2d;
    if (ii > 1)
    {
      if (aChangeCycle)
        p2d.SetXY(2. * p2d.XY() - thePoints2d(aPntIndex + 1).XY());
      else
        p2d.SetXY(2. * p2d.XY() - thePoints2d(aPntIndex - 1).XY());
    }
  }

  if (!isoPar2d3d)
  {
    projectDegeneratedPoints(mySurf, theNbPnt, thePoints, thePoints2d, myPreci, true);
    projectDegeneratedPoints(mySurf, theNbPnt, thePoints, thePoints2d, myPreci, false);
  }

  // Check extremities for singularities
  const gp_Pnt aPointFirst = thePoints.First();
  const gp_Pnt aPointLast  = thePoints.Last();
  const double aTolFirst   = (theTolFirst < 0) ? Precision::Confusion() : theTolFirst;
  const double aTolLast    = (theTolLast < 0) ? Precision::Confusion() : theTolLast;

  for (int i = 1;; i++)
  {
    double   aPreci, aFirstPar, aLastPar;
    gp_Pnt   aP3d;
    gp_Pnt2d aFirstP2d, aLastP2d;
    bool     IsUiso;
    if (!mySurf->Singularity(i, aPreci, aP3d, aFirstP2d, aLastP2d, aFirstPar, aLastPar, IsUiso))
      break;
    if (aPreci <= Precision::Confusion() && aPointFirst.Distance(aP3d) <= aTolFirst)
    {
      correctExtremity(theC3D, theParams, thePoints2d, true, aFirstP2d, IsUiso);
    }
    if (aPreci <= Precision::Confusion() && aPointLast.Distance(aP3d) <= aTolLast)
    {
      correctExtremity(theC3D, theParams, thePoints2d, false, aFirstP2d, IsUiso);
    }
  }

  // Handle U-closed surfaces
  double       dist2d;
  const double TolOnUPeriod = Precision::Confusion() * Up;
  const double TolOnVPeriod = Precision::Confusion() * Vp;

  if (mySurf->IsUClosed(myPreci) || needResolveUJump)
  {
    double prevX, firstX = thePoints2d(1).X();
    if (!isFromCache)
    {
      while (firstX < uf)
      {
        firstX += Up;
        thePoints2d.ChangeValue(1).SetX(firstX);
      }
      while (firstX > ul)
      {
        firstX -= Up;
        thePoints2d.ChangeValue(1).SetX(firstX);
      }
    }

    if (mySurf->Surface()->IsUPeriodic() && isFromCache)
    {
      double aMinParam = uf, aMaxParam = ul;
      while (aMinParam > aSavedPoint.X())
      {
        aMinParam -= Up;
        aMaxParam -= Up;
      }
      while (aMaxParam < aSavedPoint.X())
      {
        aMinParam += Up;
        aMaxParam += Up;
      }
      double aShift = ShapeAnalysis::AdjustToPeriod(firstX, aMinParam, aMaxParam);
      firstX += aShift;
      thePoints2d.ChangeValue(1).SetX(firstX);
    }
    prevX = firstX;

    double minX = firstX, maxX = firstX;
    bool   ToAdjust = false;

    for (int aPntIter = 2; aPntIter <= thePoints2d.Length(); ++aPntIter)
    {
      double CurX = thePoints2d(aPntIter).X();
      dist2d      = std::abs(CurX - prevX);
      if (dist2d > (Up / 2))
      {
        insertAdditionalPointOrAdjust(ToAdjust,
                                      1,
                                      Up,
                                      TolOnUPeriod,
                                      CurX,
                                      prevX,
                                      theC3D,
                                      aPntIter,
                                      thePoints,
                                      theParams,
                                      thePoints2d);
      }
      prevX = CurX;
      if (minX > CurX)
        minX = CurX;
      else if (maxX < CurX)
        maxX = CurX;
    }

    if (!isFromCache)
    {
      double midX   = 0.5 * (minX + maxX);
      double shiftX = 0.;
      if (midX > ul)
        shiftX = -Up;
      else if (midX < uf)
        shiftX = Up;
      if (shiftX != 0.)
        for (int aPntIter = 1; aPntIter <= thePoints2d.Length(); ++aPntIter)
          thePoints2d.ChangeValue(aPntIter).SetX(thePoints2d(aPntIter).X() + shiftX);
    }
  }

  // Handle V-closed surfaces
  if (mySurf->IsVClosed(myPreci) || needResolveVJump
      || mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    double prevY, firstY = thePoints2d(1).Y();
    if (!isFromCache)
    {
      while (firstY < vf)
      {
        firstY += Vp;
        thePoints2d.ChangeValue(1).SetY(firstY);
      }
      while (firstY > vl)
      {
        firstY -= Vp;
        thePoints2d.ChangeValue(1).SetY(firstY);
      }
    }

    if (mySurf->Surface()->IsVPeriodic() && isFromCache)
    {
      double aMinParam = vf, aMaxParam = vl;
      while (aMinParam > aSavedPoint.Y())
      {
        aMinParam -= Vp;
        aMaxParam -= Vp;
      }
      while (aMaxParam < aSavedPoint.Y())
      {
        aMinParam += Vp;
        aMaxParam += Vp;
      }
      double aShift = ShapeAnalysis::AdjustToPeriod(firstY, aMinParam, aMaxParam);
      firstY += aShift;
      thePoints2d.ChangeValue(1).SetY(firstY);
    }
    prevY = firstY;

    double minY = firstY, maxY = firstY;
    bool   ToAdjust = false;

    for (int aPntIter = 2; aPntIter <= thePoints2d.Length(); ++aPntIter)
    {
      double CurY = thePoints2d(aPntIter).Y();
      dist2d      = std::abs(CurY - prevY);
      if (dist2d > (Vp / 2))
      {
        insertAdditionalPointOrAdjust(ToAdjust,
                                      2,
                                      Vp,
                                      TolOnVPeriod,
                                      CurY,
                                      prevY,
                                      theC3D,
                                      aPntIter,
                                      thePoints,
                                      theParams,
                                      thePoints2d);
      }
      prevY = CurY;
      if (minY > CurY)
        minY = CurY;
      else if (maxY < CurY)
        maxY = CurY;
    }

    if (!isFromCache)
    {
      double midY   = 0.5 * (minY + maxY);
      double shiftY = 0.;
      if (midY > vl)
        shiftY = -Vp;
      else if (midY < vf)
        shiftY = Vp;
      if (shiftY != 0.)
        for (int aPntIter = 1; aPntIter <= thePoints2d.Length(); ++aPntIter)
          thePoints2d.ChangeValue(aPntIter).SetY(thePoints2d(aPntIter).Y() + shiftY);
    }
  }

  // Handle V-closed seam adjustment
  if (mySurf->IsVClosed(myPreci) || mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    for (int aPntIter = 2; aPntIter <= thePoints2d.Length(); ++aPntIter)
    {
      dist2d = std::abs(thePoints2d(aPntIter).Y() - thePoints2d(aPntIter - 1).Y());
      if (dist2d > (Vp / 2))
      {
        bool prevOnFirst = false;
        bool prevOnLast  = false;
        bool currOnFirst = false;
        bool currOnLast  = false;

        double distPrevVF = std::abs(thePoints2d(aPntIter - 1).Y() - vf);
        double distPrevVL = std::abs(thePoints2d(aPntIter - 1).Y() - vl);
        double distCurrVF = std::abs(thePoints2d(aPntIter).Y() - vf);
        double distCurrVL = std::abs(thePoints2d(aPntIter).Y() - vl);

        double theMin = distPrevVF;
        prevOnFirst   = true;
        if (distPrevVL < theMin)
        {
          theMin      = distPrevVL;
          prevOnFirst = false;
          prevOnLast  = true;
        }
        if (distCurrVF < theMin)
        {
          theMin      = distCurrVF;
          prevOnFirst = false;
          prevOnLast  = false;
          currOnFirst = true;
        }
        if (distCurrVL < theMin)
        {
          prevOnFirst = false;
          prevOnLast  = false;
          currOnFirst = false;
          currOnLast  = true;
        }

        if (prevOnFirst)
        {
          thePoints2d.ChangeValue(aPntIter - 1).SetY(vf);
        }
        else if (prevOnLast)
        {
          thePoints2d.ChangeValue(aPntIter - 1).SetY(vl);
        }
        else if (currOnFirst)
        {
          thePoints2d.ChangeValue(aPntIter).SetY(vf);
        }
        else if (currOnLast)
        {
          thePoints2d.ChangeValue(aPntIter).SetY(vl);
        }
      }
    }
  }

  // Handle AdjustOverDegen
  if (myAdjustOverDegen != -1)
  {
    if (mySurf->IsUClosed(myPreci))
    {
      mySurf->IsDegenerated(gp_Pnt(0, 0, 0), myPreci);
      if (mySurf->NbSingularities(myPreci) > 0)
      {
        double PrevX   = 0.;
        int    OnBound = 0, PrevOnBound = 0;
        int    ind;
        bool   start = true;
        for (ind = 1; ind <= thePoints2d.Length(); ind++)
        {
          double CurX = thePoints2d(ind).X();
          if (mySurf->IsDegenerated(thePoints(ind), Precision::Confusion()))
            continue;
          OnBound =
            (std::abs(std::abs(CurX - 0.5 * (ul + uf)) - Up / 2) <= Precision::PConfusion());
          if (!start && std::abs(std::abs(CurX - PrevX) - Up / 2) <= 0.01 * Up)
            break;
          start       = false;
          PrevX       = CurX;
          PrevOnBound = OnBound;
        }
        if (ind <= thePoints2d.Length())
        {
          PrevX     = (myAdjustOverDegen ? uf : ul);
          double dU = Up / 2 + Precision::PConfusion();
          if (PrevOnBound)
          {
            thePoints2d.ChangeValue(ind - 1).SetX(PrevX);
            for (int j = ind - 2; j > 0; j--)
            {
              double CurX = thePoints2d(j).X();
              while (CurX < PrevX - dU)
              {
                CurX += Up;
                thePoints2d.ChangeValue(j).SetX(CurX);
              }
              while (CurX > PrevX + dU)
              {
                CurX -= Up;
                thePoints2d.ChangeValue(j).SetX(CurX);
              }
            }
          }
          else if (OnBound)
          {
            thePoints2d.ChangeValue(ind).SetX(PrevX);
            for (int j = ind + 1; j <= thePoints2d.Length(); j++)
            {
              double CurX = thePoints2d(j).X();
              while (CurX < PrevX - dU)
              {
                CurX += Up;
                thePoints2d.ChangeValue(j).SetX(CurX);
              }
              while (CurX > PrevX + dU)
              {
                CurX -= Up;
                thePoints2d.ChangeValue(j).SetX(CurX);
              }
            }
          }
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        }
      }
    }
    else if (mySurf->IsVClosed(myPreci))
    {
      mySurf->IsDegenerated(gp_Pnt(0, 0, 0), myPreci);
      if (mySurf->NbSingularities(myPreci) > 0)
      {
        double PrevY   = 0.;
        int    OnBound = 0, PrevOnBound = 0;
        int    ind;
        bool   start = true;
        for (ind = 1; ind <= thePoints2d.Length(); ind++)
        {
          double CurY = thePoints2d(ind).Y();
          if (mySurf->IsDegenerated(thePoints(ind), Precision::Confusion()))
            continue;
          OnBound =
            (std::abs(std::abs(CurY - 0.5 * (vl + vf)) - Vp / 2) <= Precision::PConfusion());
          if (!start && std::abs(std::abs(CurY - PrevY) - Vp / 2) <= 0.01 * Vp)
            break;
          start       = false;
          PrevY       = CurY;
          PrevOnBound = OnBound;
        }
        if (ind <= thePoints2d.Length())
        {
          PrevY     = (myAdjustOverDegen ? vf : vl);
          double dV = Vp / 2 + Precision::PConfusion();
          if (PrevOnBound)
          {
            thePoints2d.ChangeValue(ind - 1).SetY(PrevY);
            for (int j = ind - 2; j > 0; j--)
            {
              double CurY = thePoints2d(j).Y();
              while (CurY < PrevY - dV)
              {
                CurY += Vp;
                thePoints2d.ChangeValue(j).SetY(CurY);
              }
              while (CurY > PrevY + dV)
              {
                CurY -= Vp;
                thePoints2d.ChangeValue(j).SetY(CurY);
              }
            }
          }
          else if (OnBound)
          {
            thePoints2d.ChangeValue(ind).SetY(PrevY);
            for (int j = ind + 1; j <= thePoints2d.Length(); j++)
            {
              double CurY = thePoints2d(j).Y();
              while (CurY < PrevY - dV)
              {
                CurY += Vp;
                thePoints2d.ChangeValue(j).SetY(CurY);
              }
              while (CurY > PrevY + dV)
              {
                CurY -= Vp;
                thePoints2d.ChangeValue(j).SetY(CurY);
              }
            }
          }
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        }
      }
    }
  }

  // Fill cache
  myCache.Resize(0, 1, false);
  if (aChangeCycle)
  {
    myCache(0) = CachePoint(thePoints(1), thePoints2d(1));
    myCache(1) = CachePoint(thePoints.Last(), thePoints2d.Last());
  }
  else
  {
    myCache(0) = CachePoint(thePoints.Last(), thePoints2d.Last());
    myCache(1) = CachePoint(thePoints(1), thePoints2d(1));
  }

  return isDone;
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::correctExtremity(const occ::handle<Geom_Curve>& theC3D,
                                                            const ArrayOfReal& theParams,
                                                            ArrayOfPnt2d&      thePoints2d,
                                                            const bool         theIsFirstPoint,
                                                            const gp_Pnt2d&    thePointOnIsoLine,
                                                            const bool         theIsUIso)
{
  const int    NbPnt            = thePoints2d.Length();
  const int    IndCoord         = (theIsUIso) ? 2 : 1;
  const double SingularityCoord = thePointOnIsoLine.Coord(3 - IndCoord);
  gp_Pnt2d     EndPoint         = (theIsFirstPoint) ? thePoints2d(1) : thePoints2d(NbPnt);
  const double FinishCoord      = EndPoint.Coord(3 - IndCoord);

  gp_Dir2d        aDir = (theIsUIso) ? gp::DY2d() : gp::DX2d();
  gp_Lin2d        anIsoLine(EndPoint, aDir);
  IntRes2d_Domain Dom1, Dom2;

  const bool IsPeriodic =
    (theIsUIso) ? mySurf->Surface()->IsVPeriodic() : mySurf->Surface()->IsUPeriodic();

  gp_Pnt2d FirstPointOfLine, SecondPointOfLine;
  double   FinishParam, FirstParam, SecondParam;

  if (theIsFirstPoint)
  {
    FirstPointOfLine  = thePoints2d(3);
    SecondPointOfLine = thePoints2d(2);
    FinishParam       = theParams(1);
    FirstParam        = theParams(3);
    SecondParam       = theParams(2);
  }
  else
  {
    FirstPointOfLine  = thePoints2d(NbPnt - 2);
    SecondPointOfLine = thePoints2d(NbPnt - 1);
    FinishParam       = theParams(NbPnt);
    FirstParam        = theParams(NbPnt - 2);
    SecondParam       = theParams(NbPnt - 1);
  }

  if (SingularityCoord > FinishCoord && SecondPointOfLine.Coord(3 - IndCoord) > FinishCoord)
    return;
  if (SingularityCoord < FinishCoord && SecondPointOfLine.Coord(3 - IndCoord) < FinishCoord)
    return;

  {
    const double aPrevDist =
      std::abs(SecondPointOfLine.Coord(IndCoord) - FirstPointOfLine.Coord(IndCoord));
    const double aCurDist = std::abs(EndPoint.Coord(IndCoord) - SecondPointOfLine.Coord(IndCoord));
    if (aCurDist <= 2 * aPrevDist)
      return;
  }

  gp_Pnt2d FinishPoint = (theIsUIso) ? gp_Pnt2d(FinishCoord, SecondPointOfLine.Y())
                                     : gp_Pnt2d(SecondPointOfLine.X(), FinishCoord);

  for (;;)
  {
    if (std::abs(SecondPointOfLine.Coord(3 - IndCoord) - FinishCoord)
        <= 2 * Precision::PConfusion())
      break;

    gp_Vec2d     aVec(FirstPointOfLine, SecondPointOfLine);
    const double aSqMagnitude = aVec.SquareMagnitude();
    if (aSqMagnitude <= 1.e-32)
      break;
    aDir.SetCoord(aVec.X(), aVec.Y());

    gp_Lin2d               aLine(FirstPointOfLine, aDir);
    IntCurve_IntConicConic Intersector(anIsoLine, Dom1, aLine, Dom2, 1.e-10, 1.e-10);
    if (Intersector.IsDone() && !Intersector.IsEmpty())
    {
      IntRes2d_IntersectionPoint IntPoint = Intersector.Point(1);
      FinishPoint                         = IntPoint.Value();
    }
    else
      FinishPoint = (theIsUIso) ? gp_Pnt2d(FinishCoord, SecondPointOfLine.Y())
                                : gp_Pnt2d(SecondPointOfLine.X(), FinishCoord);

    gp_Pnt2d PrevPoint = FirstPointOfLine;
    FirstPointOfLine   = SecondPointOfLine;
    FirstParam         = SecondParam;
    SecondParam        = (FirstParam + FinishParam) / 2;
    if (std::abs(SecondParam - FirstParam) <= 2 * Precision::PConfusion())
      break;
    gp_Pnt aP3d;
    theC3D->D0(SecondParam, aP3d);
    SecondPointOfLine =
      mySurf->NextValueOfUV(FirstPointOfLine, aP3d, myPreci, Precision::Confusion());
    if (IsPeriodic)
      adjustSecondToFirstPoint(FirstPointOfLine, SecondPointOfLine, mySurf->Surface());

    const double aPrevDist = std::abs(FirstPointOfLine.Coord(IndCoord) - PrevPoint.Coord(IndCoord));
    const double aCurDist =
      std::abs(SecondPointOfLine.Coord(IndCoord) - FirstPointOfLine.Coord(IndCoord));
    if (aCurDist > 2 * aPrevDist)
      break;
  }

  if (theIsFirstPoint)
    thePoints2d.ChangeValue(1) = FinishPoint;
  else
    thePoints2d.ChangeValue(NbPnt) = FinishPoint;
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::insertAdditionalPointOrAdjust(
  bool&                          theToAdjust,
  const int                      theIndCoord,
  const double                   thePeriod,
  const double                   theTolOnPeriod,
  double&                        theCurCoord,
  const double                   thePrevCoord,
  const occ::handle<Geom_Curve>& theC3D,
  int&                           theIndex,
  ArrayOfPnt&                    thePoints,
  ArrayOfReal&                   theParams,
  ArrayOfPnt2d&                  thePoints2d)
{
  const double CorrectedCurCoord =
    ElCLib::InPeriod(theCurCoord, thePrevCoord - thePeriod / 2, thePrevCoord + thePeriod / 2);

  if (!theToAdjust)
  {
    const double CurPar  = theParams(theIndex);
    const double PrevPar = theParams(theIndex - 1);
    double       MidPar  = (PrevPar + CurPar) / 2;
    gp_Pnt       MidP3d;
    theC3D->D0(MidPar, MidP3d);
    gp_Pnt2d MidP2d   = mySurf->ValueOfUV(MidP3d, myPreci);
    double   MidCoord = MidP2d.Coord(theIndCoord);
    MidCoord =
      ElCLib::InPeriod(MidCoord, thePrevCoord - thePeriod / 2, thePrevCoord + thePeriod / 2);
    double FirstCoord = thePrevCoord, LastCoord = CorrectedCurCoord;
    if (LastCoord < FirstCoord)
    {
      double tmp = FirstCoord;
      FirstCoord = LastCoord;
      LastCoord  = tmp;
    }
    if (LastCoord - FirstCoord <= theTolOnPeriod)
      theToAdjust = true;
    else if (FirstCoord <= MidCoord && MidCoord <= LastCoord)
      theToAdjust = true;
    else
    {
      bool   Success = true;
      double FirstT  = PrevPar;
      double LastT   = CurPar;
      MidCoord       = MidP2d.Coord(theIndCoord);
      while (std::abs(MidCoord - thePrevCoord) >= thePeriod / 2 - theTolOnPeriod
             || std::abs(theCurCoord - MidCoord) >= thePeriod / 2 - theTolOnPeriod)
      {
        if (MidPar - FirstT <= Precision::PConfusion() || LastT - MidPar <= Precision::PConfusion())
        {
          Success = false;
          break;
        }
        if (std::abs(MidCoord - thePrevCoord) >= thePeriod / 2 - theTolOnPeriod)
          LastT = (FirstT + LastT) / 2;
        else
          FirstT = (FirstT + LastT) / 2;
        MidPar = (FirstT + LastT) / 2;
        theC3D->D0(MidPar, MidP3d);
        MidP2d   = mySurf->ValueOfUV(MidP3d, myPreci);
        MidCoord = MidP2d.Coord(theIndCoord);
      }
      if (Success)
      {
        // Insert additional point - need to resize arrays
        const int    aNewLength = thePoints.Length() + 1;
        ArrayOfPnt   aNewPoints(1, aNewLength);
        ArrayOfReal  aNewParams(1, aNewLength);
        ArrayOfPnt2d aNewPoints2d(1, aNewLength);

        for (int i = 1; i < theIndex; i++)
        {
          aNewPoints.SetValue(i, thePoints(i));
          aNewParams.SetValue(i, theParams(i));
          aNewPoints2d.SetValue(i, thePoints2d(i));
        }
        aNewPoints.SetValue(theIndex, MidP3d);
        aNewParams.SetValue(theIndex, MidPar);
        aNewPoints2d.SetValue(theIndex, MidP2d);
        for (int i = theIndex; i <= thePoints.Length(); i++)
        {
          aNewPoints.SetValue(i + 1, thePoints(i));
          aNewParams.SetValue(i + 1, theParams(i));
          aNewPoints2d.SetValue(i + 1, thePoints2d(i));
        }

        thePoints   = aNewPoints;
        theParams   = aNewParams;
        thePoints2d = aNewPoints2d;
        theIndex++;
      }
      else
        theToAdjust = true;
    }
  }
  if (theToAdjust)
  {
    theCurCoord = CorrectedCurCoord;
    thePoints2d.ChangeValue(theIndex).SetCoord(theIndCoord, theCurCoord);
  }
}

//=================================================================================================

occ::handle<Geom2d_Curve> ShapeConstruct_ProjectCurveOnSurface::interpolatePCurve(
  const int           theNbPnt,
  const ArrayOfPnt2d& thePoints2d,
  const ArrayOfReal&  theParams) const
{
  occ::handle<Geom2d_Curve> aC2D;
  const double              theTolerance2d = myPreci / (100 * theNbPnt);

  try
  {
    OCC_CATCH_SIGNALS

    // Convert to HArrays for Geom2dAPI_Interpolate
    occ::handle<NCollection_HArray1<gp_Pnt2d>> aPnts2d =
      new NCollection_HArray1<gp_Pnt2d>(1, theNbPnt);
    occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, theNbPnt);

    for (int i = 1; i <= theNbPnt; i++)
    {
      aPnts2d->SetValue(i, thePoints2d(i));
      aParams->SetValue(i, theParams(i));
    }

    // Check for coincident points
    ArrayOfPnt2d aTmpPnts2d = thePoints2d;
    ArrayOfReal  aTmpParams = theParams;
    double       aPreci     = theTolerance2d;
    checkPoints2d(aTmpPnts2d, aTmpParams, aPreci);

    if (aTmpPnts2d.Length() < 2)
      return aC2D;

    // Rebuild HArrays if points were removed
    if (aTmpPnts2d.Length() != theNbPnt)
    {
      aPnts2d = new NCollection_HArray1<gp_Pnt2d>(1, aTmpPnts2d.Length());
      aParams = new NCollection_HArray1<double>(1, aTmpParams.Length());
      for (int i = 1; i <= aTmpPnts2d.Length(); i++)
      {
        aPnts2d->SetValue(i, aTmpPnts2d(i));
        aParams->SetValue(i, aTmpParams(i));
      }
    }

    Geom2dAPI_Interpolate myInterPol2d(aPnts2d, aParams, false, aPreci);
    myInterPol2d.Perform();
    if (myInterPol2d.IsDone())
      aC2D = myInterPol2d.Curve();
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::interpolatePCurve(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    aC2D.Nullify();
  }
  return aC2D;
}

//=================================================================================================

occ::handle<Geom2d_Curve> ShapeConstruct_ProjectCurveOnSurface::approximatePCurve(
  const ArrayOfPnt2d& thePoints2d,
  const ArrayOfReal&  theParams) const
{
  const double              theTolerance2d = myPreci;
  occ::handle<Geom2d_Curve> aC2D;

  try
  {
    OCC_CATCH_SIGNALS

    ArrayOfPnt2d aTmpPnts2d = thePoints2d;
    ArrayOfReal  aTmpParams = theParams;
    double       aPreci     = theTolerance2d;
    checkPoints2d(aTmpPnts2d, aTmpParams, aPreci);

    const int numberPnt = aTmpPnts2d.Length();
    if (numberPnt < 2)
      return aC2D;

    NCollection_Array1<gp_Pnt> points3d(1, numberPnt);
    NCollection_Array1<double> params(1, numberPnt);

    for (int i = 1; i <= numberPnt; i++)
    {
      const gp_Pnt2d& pnt2d = aTmpPnts2d(i);
      points3d(i).SetCoord(pnt2d.X(), pnt2d.Y(), 0);
      params(i) = aTmpParams(i);
    }

    GeomAPI_PointsToBSpline               appr(points3d, params, 1, 10, GeomAbs_C1, theTolerance2d);
    const occ::handle<Geom_BSplineCurve>& crv3d = appr.Curve();

    const int                         NbPoles = crv3d->NbPoles();
    const NCollection_Array1<gp_Pnt>& poles3d = crv3d->Poles();
    NCollection_Array1<gp_Pnt2d>      poles2d(1, NbPoles);

    for (int i = 1; i <= NbPoles; i++)
    {
      poles2d(i).SetCoord(poles3d(i).X(), poles3d(i).Y());
    }

    const NCollection_Array1<double>& weights        = crv3d->WeightsArray();
    const NCollection_Array1<double>& knots          = crv3d->Knots();
    const NCollection_Array1<int>&    multiplicities = crv3d->Multiplicities();

    aC2D = new Geom2d_BSplineCurve(poles2d,
                                   weights,
                                   knots,
                                   multiplicities,
                                   crv3d->Degree(),
                                   crv3d->IsPeriodic());
    return aC2D;
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::approximatePCurve(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    aC2D.Nullify();
  }
  return aC2D;
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::checkPoints(ArrayOfPnt&  thePoints,
                                                       ArrayOfReal& theParams,
                                                       double&      thePreci) const
{
  const int firstElem = thePoints.Lower();
  const int lastElem  = thePoints.Upper();

  int nbPntDropped = 0;
  int lastValid    = firstElem;

  NCollection_Array1<int> tmpParam(firstElem, lastElem);
  for (int i = firstElem; i <= lastElem; i++)
    tmpParam.SetValue(i, 1);

  double DistMin2 = RealLast();
  gp_Pnt Prev     = thePoints.Value(lastValid);

  for (int i = firstElem + 1; i <= lastElem; i++)
  {
    const gp_Pnt& Curr     = thePoints.Value(i);
    const double  CurDist2 = Prev.SquareDistance(Curr);
    if (CurDist2 < gp::Resolution())
    {
      nbPntDropped++;
      if (i == lastElem)
        tmpParam.SetValue(lastValid, 0);
      else
        tmpParam.SetValue(i, 0);
    }
    else
    {
      if (CurDist2 < DistMin2)
        DistMin2 = CurDist2;
      lastValid = i;
      Prev      = Curr;
    }
  }

  if (DistMin2 < RealLast())
    thePreci = 0.9 * std::sqrt(DistMin2);

  if (nbPntDropped == 0)
    return;

  const int newLast = lastElem - nbPntDropped;
  if ((newLast - firstElem + 1) < 2)
    return;

  ArrayOfPnt  newPnts(firstElem, newLast);
  ArrayOfReal newParams(firstElem, newLast);
  int         newCurr = firstElem;

  for (int i = firstElem; i <= lastElem; i++)
  {
    if (tmpParam.Value(i) == 1)
    {
      newPnts.SetValue(newCurr, thePoints.Value(i));
      newParams.SetValue(newCurr, theParams.Value(i));
      newCurr++;
    }
  }

  thePoints = newPnts;
  theParams = newParams;
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::checkPoints2d(ArrayOfPnt2d& thePoints2d,
                                                         ArrayOfReal&  theParams,
                                                         double&       thePreci) const
{
  const int firstElem = thePoints2d.Lower();
  const int lastElem  = thePoints2d.Upper();

  int nbPntDropped = 0;
  int lastValid    = firstElem;

  NCollection_Array1<int> tmpParam(firstElem, lastElem);
  for (int i = firstElem; i <= lastElem; i++)
    tmpParam.SetValue(i, 1);

  double   DistMin2 = RealLast();
  gp_Pnt2d Prev     = thePoints2d.Value(lastValid);

  for (int i = firstElem + 1; i <= lastElem; i++)
  {
    const gp_Pnt2d& Curr     = thePoints2d.Value(i);
    const double    CurDist2 = Prev.SquareDistance(Curr);
    if (CurDist2 < gp::Resolution())
    {
      nbPntDropped++;
      if (i == lastElem)
        tmpParam.SetValue(lastValid, 0);
      else
        tmpParam.SetValue(i, 0);
    }
    else
    {
      if (CurDist2 < DistMin2)
        DistMin2 = CurDist2;
      lastValid = i;
      Prev      = Curr;
    }
  }

  if (DistMin2 < RealLast())
    thePreci = 0.9 * std::sqrt(DistMin2);

  if (nbPntDropped == 0)
    return;

  int newLast = lastElem - nbPntDropped;
  if ((newLast - firstElem + 1) < 2)
  {
    // Create minimal length pcurve
    tmpParam.SetValue(firstElem, 1);
    tmpParam.SetValue(lastElem, 1);
    gp_XY lastPnt = thePoints2d.Value(lastElem).XY();
    lastPnt.Add(gp_XY(thePreci, thePreci));
    thePoints2d.ChangeValue(lastElem).SetXY(lastPnt);
    newLast = firstElem + 1;
  }

  ArrayOfPnt2d newPnts(firstElem, newLast);
  ArrayOfReal  newParams(firstElem, newLast);
  int          newCurr = firstElem;

  for (int i = firstElem; i <= lastElem; i++)
  {
    if (tmpParam.Value(i) == 1)
    {
      newPnts.SetValue(newCurr, thePoints2d.Value(i));
      newParams.SetValue(newCurr, theParams.Value(i));
      newCurr++;
    }
  }

  thePoints2d = std::move(newPnts);
  theParams   = std::move(newParams);
}

//=================================================================================================

bool ShapeConstruct_ProjectCurveOnSurface::isAnIsoparametric(const int                theNbPnt,
                                                             const ArrayOfPnt&        thePoints,
                                                             const ArrayOfReal&       theParams,
                                                             bool&                    theIsTypeU,
                                                             bool&                    theP1OnIso,
                                                             gp_Pnt2d&                theValueP1,
                                                             bool&                    theP2OnIso,
                                                             gp_Pnt2d&                theValueP2,
                                                             bool&                    theIsoPar2d3d,
                                                             occ::handle<Geom_Curve>& theCIso,
                                                             double&                  theT1,
                                                             double&                  theT2,
                                                             ArrayOfReal& theParamsOut) const
{
  try
  {
    OCC_CATCH_SIGNALS

    constexpr double prec = Precision::Confusion();

    bool isoParam = false;
    theIsoPar2d3d = false;

    double U1, U2, V1, V2;
    mySurf->Bounds(U1, U2, V1, V2);

    if (mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      occ::handle<Geom_RectangularTrimmedSurface> sTrim =
        occ::down_cast<Geom_RectangularTrimmedSurface>(mySurf->Surface());
      sTrim->Bounds(U1, U2, V1, V2);
    }

    gp_Pnt pt;
    int    mpt[2];
    mpt[0] = mpt[1] = 0;
    double t, tpar[2] = {0.0, 0.0}, isoValue = 0.;
    double mindist2;
    double mind2[2];
    mindist2 = mind2[0] = mind2[1] = 4 * prec * prec;

    theP1OnIso          = false;
    theP2OnIso          = false;
    const Bnd_Box* aBox = nullptr;

    for (int j = 1; j <= 4; j++)
    {
      double                  isoVal = 0.;
      bool                    isoU   = false;
      occ::handle<Geom_Curve> cI;
      double                  tt1, tt2;

      if (j == 1)
      {
        if (Precision::IsInfinite(U1))
          continue;
        cI     = mySurf->UIso(U1);
        isoU   = true;
        isoVal = U1;
        aBox   = &mySurf->GetBoxUF();
      }
      else if (j == 2)
      {
        if (Precision::IsInfinite(U2))
          continue;
        cI     = mySurf->UIso(U2);
        isoU   = true;
        isoVal = U2;
        aBox   = &mySurf->GetBoxUL();
      }
      else if (j == 3)
      {
        if (Precision::IsInfinite(V1))
          continue;
        cI     = mySurf->VIso(V1);
        isoU   = false;
        isoVal = V1;
        aBox   = &mySurf->GetBoxVF();
      }
      else if (j == 4)
      {
        if (Precision::IsInfinite(V2))
          continue;
        cI     = mySurf->VIso(V2);
        isoU   = false;
        isoVal = V2;
        aBox   = &mySurf->GetBoxVL();
      }
      if (cI.IsNull())
        continue;

      if (isoU)
      {
        tt1 = V1;
        tt2 = V2;
      }
      else
      {
        tt1 = U1;
        tt2 = U2;
      }

      gp_Pnt ext1, ext2;
      cI->D0(tt1, ext1);
      cI->D0(tt2, ext2);

      gp_Pnt extmi;
      cI->D0((tt1 + tt2) / 2, extmi);
      if (ext1.IsEqual(ext2, prec) && ext1.IsEqual(extmi, prec))
        continue;

      bool PtEQext1 = false;
      bool PtEQext2 = false;

      double currd2[2], tp[2] = {0, 0};
      int    mp[2];

      for (int i = 0; i < 2; i++)
      {
        mp[i]       = 0;
        const int k = (i == 0 ? 1 : theNbPnt);

        currd2[i] = thePoints(k).SquareDistance(ext1);
        if (currd2[i] <= prec * prec && !PtEQext1)
        {
          mp[i]    = 1;
          tp[i]    = tt1;
          PtEQext1 = true;
          continue;
        }

        currd2[i] = thePoints(k).SquareDistance(ext2);
        if (currd2[i] <= prec * prec && !PtEQext2)
        {
          mp[i]    = 2;
          tp[i]    = tt2;
          PtEQext2 = true;
          continue;
        }

        if (mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) && !isoU)
        {
          continue;
        }

        if (aBox->IsOut(thePoints(k)))
          continue;

        double Cf = cI->FirstParameter();
        double Cl = cI->LastParameter();
        if (Precision::IsInfinite(Cf))
          Cf = -1000;
        if (Precision::IsInfinite(Cl))
          Cl = +1000;

        ShapeAnalysis_Curve sac;
        const double        dist = sac.Project(cI, thePoints(k), prec, pt, t, Cf, Cl);
        currd2[i]                = dist * dist;
        if ((dist <= prec) && (t >= Cf) && (t <= Cl))
        {
          mp[i] = 3;
          tp[i] = t;
        }
      }

      if (mp[0] > 0 && mp[1] > 0 && std::abs(tp[0] - tp[1]) < Precision::PConfusion())
        continue;

      if (mp[0] > 0 && (!theP1OnIso || currd2[0] < mind2[0]))
      {
        theP1OnIso = true;
        mind2[0]   = currd2[0];
        if (isoU)
          theValueP1.SetCoord(isoVal, tp[0]);
        else
          theValueP1.SetCoord(tp[0], isoVal);
      }

      if (mp[1] > 0 && (!theP2OnIso || currd2[1] < mind2[1]))
      {
        theP2OnIso = true;
        mind2[1]   = currd2[1];
        if (isoU)
          theValueP2.SetCoord(isoVal, tp[1]);
        else
          theValueP2.SetCoord(tp[1], isoVal);
      }

      if (mp[0] <= 0 || mp[1] <= 0)
        continue;

      const double md2 = currd2[0] + currd2[1];
      if (mindist2 <= md2)
        continue;

      mindist2   = md2;
      mpt[0]     = mp[0];
      mpt[1]     = mp[1];
      tpar[0]    = tp[0];
      tpar[1]    = tp[1];
      theIsTypeU = isoU;
      isoValue   = isoVal;
      theCIso    = cI;
      theT1      = tt1;
      theT2      = tt2;
    }

    if (mpt[0] > 0 && mpt[1] > 0)
    {
      theP1OnIso = theP2OnIso = true;
      if (theIsTypeU)
      {
        theValueP1.SetCoord(isoValue, tpar[0]);
        theValueP2.SetCoord(isoValue, tpar[1]);
      }
      else
      {
        theValueP1.SetCoord(tpar[0], isoValue);
        theValueP2.SetCoord(tpar[1], isoValue);
      }

      if (mpt[0] != 3 && mpt[1] != 3)
      {
        theIsoPar2d3d = true;
        for (int i = 2; i < theNbPnt && theIsoPar2d3d; i++)
        {
          if (tpar[1] > tpar[0])
            t = theParams(i);
          else
            t = theT1 + theT2 - theParams(i);
          theCIso->D0(t, pt);
          if (!thePoints(i).IsEqual(pt, prec))
            theIsoPar2d3d = false;
        }
      }

      if (theIsoPar2d3d)
        isoParam = true;
      else
      {
        double prevParam = tpar[0];
        double Cf, Cl;
        bool   isoByDistance = true;
        Cf                   = theCIso->FirstParameter();
        Cl                   = theCIso->LastParameter();
        if (Precision::IsInfinite(Cf))
          Cf = -1000;
        if (Precision::IsInfinite(Cl))
          Cl = +1000;

        ShapeAnalysis_Curve sac;
        for (int i = 2; i < theNbPnt && isoByDistance; i++)
        {
          const double dist =
            sac.NextProject(prevParam, theCIso, thePoints(i), prec, pt, t, Cf, Cl, false);
          prevParam       = t;
          theParamsOut(i) = t;
          if ((dist > prec) || (t < Cf) || (t > Cl))
            isoByDistance = false;
        }
        if (isoByDistance)
          isoParam = true;
      }
    }
    return isoParam;
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::isAnIsoparametric(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    return false;
  }
}
