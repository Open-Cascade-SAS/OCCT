// Created by: Nikolai BUKHALOV
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

#include <GeomLib_CheckCurveOnSurface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <ElCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_XYZ.hxx>
#include <math_MultipleVarFunction.hxx>
#include <MathOpt_Brent.hxx>
#include <MathUtils_FunctorScalar.hxx>
#include <MathUtils_Domain.hxx>
#include <math_PSO.hxx>
#include <math_PSOParticlesPool.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LinearVector.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <utility>
#include <optional>
#include <BSplCLib_Basis.pxx>
#include <PLib_EvalPolynomial.pxx>
#include <NCollection_Array2.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Real.hxx>

typedef NCollection_Array1<occ::handle<Adaptor3d_Curve>> Array1OfHCurve;

class GeomLib_CheckCurveOnSurface_TargetFunc;

static bool MinComputing(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                         const double                            theEpsilon, // 1.0e-3
                         const int                               theNbParticles,
                         double&                                 theBestValue,
                         double&                                 theBestParameter);

namespace
{
// Build interval bounds once, borrowing spline knots until the merge is complete.
bool FillSubIntervals(const occ::handle<Adaptor3d_Curve>&   theCurve3d,
                      const occ::handle<Adaptor2d_Curve2d>& theCurve2d,
                      double                                theFirst,
                      double                                theLast,
                      int&                                  theNbParticles,
                      NCollection_LinearVector<double>&     theSubIntervals);
} // namespace

//=================================================================================================

namespace
{
// A two-component expansion keeps rounding errors through a spline composition.
// This is needed when its final difference is small compared with either point.
class CompensatedReal
{
public:
  CompensatedReal(double theValue = 0.0);
  explicit operator double() const;

  friend CompensatedReal operator+(const CompensatedReal& theA, const CompensatedReal& theB);
  friend CompensatedReal operator-(const CompensatedReal& theA, const CompensatedReal& theB);
  friend CompensatedReal operator*(const CompensatedReal& theA, const CompensatedReal& theB);
  friend CompensatedReal operator/(const CompensatedReal& theA, const CompensatedReal& theB);
  friend bool            operator<(const CompensatedReal& theA, const CompensatedReal& theB);

private:
  CompensatedReal(double theHigh, double theLow);

  double myHigh;
  double myLow;
};

//=================================================================================================

CompensatedReal::CompensatedReal(double theValue)
    : myHigh(theValue),
      myLow(0.0)
{
}

//=================================================================================================

CompensatedReal::CompensatedReal(double theHigh, double theLow)
{
  myHigh             = theHigh + theLow;
  const double aPart = myHigh - theHigh;
  myLow              = (theHigh - (myHigh - aPart)) + (theLow - aPart);
}

//=================================================================================================

CompensatedReal::operator double() const
{
  return myHigh + myLow;
}

//=================================================================================================

CompensatedReal operator+(const CompensatedReal& theA, const CompensatedReal& theB)
{
  const double aSum    = theA.myHigh + theB.myHigh;
  const double aPart   = aSum - theA.myHigh;
  const double anError = (theA.myHigh - (aSum - aPart)) + (theB.myHigh - aPart);
  return CompensatedReal(aSum, anError + theA.myLow + theB.myLow);
}

//=================================================================================================

CompensatedReal operator-(const CompensatedReal& theA, const CompensatedReal& theB)
{
  return theA + CompensatedReal(-theB.myHigh, -theB.myLow);
}

//=================================================================================================

CompensatedReal operator*(const CompensatedReal& theA, const CompensatedReal& theB)
{
  const double aProduct = theA.myHigh * theB.myHigh;
  const double anError  = std::fma(theA.myHigh, theB.myHigh, -aProduct);
  return CompensatedReal(aProduct,
                         anError + theA.myHigh * theB.myLow + theA.myLow * theB.myHigh
                           + theA.myLow * theB.myLow);
}

//=================================================================================================

CompensatedReal operator/(const CompensatedReal& theA, const CompensatedReal& theB)
{
  const double aQuotient = theA.myHigh / theB.myHigh;
  // A scalar divisor needs only the exact product remainder.
  if (theB.myLow == 0.0)
  {
    const double aRemainder = std::fma(-aQuotient, theB.myHigh, theA.myHigh) + theA.myLow;
    return CompensatedReal(aQuotient, aRemainder / theB.myHigh);
  }
  const CompensatedReal aRemainder = theA - theB * aQuotient;
  return CompensatedReal(aQuotient, (aRemainder.myHigh + aRemainder.myLow) / theB.myHigh);
}

//=================================================================================================

bool operator<(const CompensatedReal& theA, const CompensatedReal& theB)
{
  return theA.myHigh < theB.myHigh || (theA.myHigh == theB.myHigh && theA.myLow < theB.myLow);
}

// Evaluates spline and planar residuals without rounding intermediate points.
// Reuses compensated Bernstein coefficients within the supplied parameter interval.
// Geometry must stay unchanged during evaluation.
class BSplineResidual
{
  template <class Real>
  using Point            = std::array<Real, 4>;
  using CompensatedPoint = Point<CompensatedReal>;

public:
  // Retain fixed geometry and prepare evaluation within the supplied interval.
  BSplineResidual(const Adaptor3d_Curve& theCurve,
                  const Adaptor3d_Curve& theCurveOnSurface,
                  double                 theFirst,
                  double                 theLast);
  // Return false for unsupported geometry or a non-finite residual.
  bool Value(double theParameter, gp_Vec& theResidual) const;

private:
  // Evaluate polynomial and rational curves without rounding intermediate coordinates.
  template <class Adaptor>
  static CompensatedPoint planeCurveValue(const Adaptor& theCurve,
                                          double         theParameter,
                                          size_t         theDimension);

  struct BernsteinParameter
  {
    CompensatedReal Ratio;      // Smaller Bernstein parameter divided by the larger one.
    CompensatedReal Factor;     // Larger Bernstein parameter raised to the degree.
    bool            IsReversed; // Evaluate from the last pole when U exceeds one half.
  };

  struct PolynomialCache
  {
    NCollection_Array2<CompensatedPoint> Coefficients; // Binomial-weighted Bernstein poles.
    CompensatedPoint                     Origin; // Common coordinates excluded from interpolation.
    std::array<CompensatedReal, 2>       First;  // Lower knot bounds in each direction.
    std::array<CompensatedReal, 2>       Last;   // Upper knot bounds in each direction.
    std::array<CompensatedReal, 2>       Length; // Exact knot differences.
  };

  // Extract one span by knot insertion.
  // Pole buffers contain degree + 1 points.
  static void extractBezier(size_t                            theDegree,
                            const NCollection_Array1<double>& theKnots,
                            size_t                            theSpan,
                            size_t                            theDimension,
                            const CompensatedPoint*           thePoles,
                            CompensatedPoint*                 theWorkPoles,
                            CompensatedPoint*                 theCoefficients);
  // Normalize inside a positive knot span and keep the Horner ratio at most one.
  static BernsteinParameter parameterData(const CompensatedReal& theParameter,
                                          const CompensatedReal& theFirst,
                                          const CompensatedReal& theLength,
                                          size_t                 theDegree);
  // Evaluate binomial-weighted poles in the stable parameter direction.
  template <class CoefficientAccessor>
  static CompensatedPoint horner(const BernsteinParameter&  theParameter,
                                 size_t                     theDegree,
                                 size_t                     theDimension,
                                 const CoefficientAccessor& theCoefficient);
  // Convert one surface knot patch.
  void initSurfaceCache(PolynomialCache& theCache, size_t theUSpan, size_t theVSpan) const;
  // Find the active knot span, including extrapolation at either end.
  template <class Real>
  static size_t findSpan(size_t                            theDegree,
                         const NCollection_Array1<double>& theKnots,
                         size_t                            theNbPoles,
                         const Real&                       theParameter);

  // Apply de Boor to working poles, or evaluate a blossom with supplied parameters.
  template <class Real, class ParameterAccessor>
  static Point<Real> evaluatePoles(size_t                            theDegree,
                                   const NCollection_Array1<double>& theKnots,
                                   size_t                            theSpan,
                                   const ParameterAccessor&          theParameter,
                                   size_t                            theDimension,
                                   Point<Real>*                      thePoles);

  // Convert an interval only when it fits inside one knot span.
  template <class PoleAccessor>
  static void initCache(PolynomialCache&                  theCache,
                        size_t                            theDegree,
                        const NCollection_Array1<double>& theKnots,
                        size_t                            theNbPoles,
                        double                            theFirst,
                        double                            theLast,
                        size_t                            theDimension,
                        const PoleAccessor&               thePole);

  // Evaluate either a 2D or a 3D curve through the shared span cache.
  template <class Curve>
  static CompensatedPoint curveValue(const Curve&                                  theCurve,
                                     const NCollection_Array1<double>*             theWeights,
                                     const PolynomialCache&                        theCache,
                                     NCollection_DataMap<size_t, PolynomialCache>& theSpans,
                                     double                                        theParameter,
                                     size_t                                        theDimension);

  // Shift and weight Bernstein poles; retain de Boor when Horner could overflow.
  static void initPolynomialCache(PolynomialCache&                      theCache,
                                  NCollection_Array2<CompensatedPoint>& theCoefficients,
                                  size_t                                theDimension,
                                  const std::array<double, 2>&          theFirst,
                                  const std::array<double, 2>&          theLast);

  // Evaluate inside the cached knot bounds and restore the common coordinate origin.
  static CompensatedPoint cacheValue(const PolynomialCache& theCache,
                                     const CompensatedReal& theU,
                                     const CompensatedReal& theV,
                                     size_t                 theDimension);
  // Read a curve or surface pole, retaining homogeneous coordinates for rational data.
  template <class Poles, class Weights, class... Indices>
  static CompensatedPoint pole(const Poles&   thePoles,
                               const Weights* theWeights,
                               size_t         theDimension,
                               Indices... theIndices);

  struct Caches
  {
    PolynomialCache Curve;  // Spatial curve span.
    PolynomialCache PCurve; // Parameter curve span.

    mutable NCollection_DataMap<size_t, PolynomialCache> CurveSpans; // Visited spatial curve spans.
    mutable NCollection_DataMap<size_t, PolynomialCache>
      PCurveSpans; // Visited parameter curve spans.
    mutable NCollection_DataMap<size_t, PolynomialCache> SurfaceSpans; // Visited surface patches.
  };

  std::optional<Caches> myCaches; // Owned coefficients for the non-affine evaluation path.

  // Build the difference spline when the surface is affine and the curve bases match.
  void initAffineResidual();

  // Evaluate homogeneous poles through de Boor, including end-span extrapolation.
  template <class Real, class PoleAccessor>
  static Point<Real> evaluate(size_t                            theDegree,
                              const NCollection_Array1<double>& theKnots,
                              size_t                            theNbPoles,
                              const Real&                       theParameter,
                              const PoleAccessor&               thePole,
                              size_t                            theDimension = 4);

  NCollection_Array1<Point<double>>
                                   myAffineResidual; // Difference poles in the shared spline basis.
  occ::handle<Geom_BSplineCurve>   myCurve;
  occ::handle<Geom2d_BSplineCurve> myPCurve;
  occ::handle<Geom_BSplineSurface> mySurface;
  const Adaptor3d_Curve* myPlaneCurve = nullptr; // Borrowed spatial curve for planar residuals.
  const Adaptor3d_CurveOnSurface* myPlaneCoS = nullptr; // Borrowed planar composition.
};

//=================================================================================================

BSplineResidual::BSplineResidual(const Adaptor3d_Curve& theCurve,
                                 const Adaptor3d_Curve& theCurveOnSurface,
                                 double                 theFirst,
                                 double                 theLast)
{
  const Adaptor3d_CurveOnSurface* aCoS =
    dynamic_cast<const Adaptor3d_CurveOnSurface*>(&theCurveOnSurface);
  const auto isPolynomialCurve = [](GeomAbs_CurveType theType) {
    return theType == GeomAbs_Line || theType == GeomAbs_BezierCurve
           || theType == GeomAbs_BSplineCurve;
  };
  if (aCoS != nullptr && aCoS->GetSurface()->GetType() == GeomAbs_Plane
      && isPolynomialCurve(theCurve.GetType()) && isPolynomialCurve(aCoS->GetCurve()->GetType()))
  {
    myPlaneCurve = &theCurve;
    myPlaneCoS   = aCoS;
    return;
  }
  if (theCurve.GetType() != GeomAbs_BSplineCurve || aCoS == nullptr
      || aCoS->GetCurve()->GetType() != GeomAbs_BSplineCurve
      || aCoS->GetSurface()->GetType() != GeomAbs_BSplineSurface)
  {
    return;
  }
  myCurve   = theCurve.BSpline();
  myPCurve  = aCoS->GetCurve()->BSpline();
  mySurface = aCoS->GetSurface()->BSpline();
  if (myCurve->IsPeriodic() || myPCurve->IsPeriodic() || mySurface->IsUPeriodic()
      || mySurface->IsVPeriodic())
  {
    myCurve.Nullify();
    return;
  }
  initAffineResidual();
  if (!myAffineResidual.IsEmpty())
  {
    return;
  }
  myCaches.emplace();
  const NCollection_Array1<double>* aCurveWeights  = myCurve->Weights();
  const NCollection_Array1<double>* aPCurveWeights = myPCurve->Weights();
  initCache(myCaches->Curve,
            myCurve->Degree(),
            myCurve->KnotSequence(),
            myCurve->NbPoles(),
            theFirst,
            theLast,
            aCurveWeights == nullptr ? 3 : 4,
            [&](size_t theIndex) { return pole(myCurve->Poles(), aCurveWeights, 3, theIndex); });
  initCache(myCaches->PCurve,
            myPCurve->Degree(),
            myPCurve->KnotSequence(),
            myPCurve->NbPoles(),
            theFirst,
            theLast,
            aPCurveWeights == nullptr ? 2 : 4,
            [&](size_t theIndex) { return pole(myPCurve->Poles(), aPCurveWeights, 2, theIndex); });
}

//=================================================================================================

void BSplineResidual::initAffineResidual()
{
  if (myCurve->IsRational() || myPCurve->IsRational() || mySurface->IsURational()
      || mySurface->IsVRational() || mySurface->UDegree() != 1 || mySurface->VDegree() != 1
      || mySurface->NbUPoles() != 2 || mySurface->NbVPoles() != 2
      || myCurve->Degree() != myPCurve->Degree() || myCurve->NbPoles() != myPCurve->NbPoles())
  {
    return;
  }
  const NCollection_Array1<double>& aCurveKnots  = myCurve->KnotSequence();
  const NCollection_Array1<double>& aPCurveKnots = myPCurve->KnotSequence();
  if (aCurveKnots.Size() != aPCurveKnots.Size()
      || !std::equal(aCurveKnots.begin(), aCurveKnots.end(), aPCurveKnots.begin()))
  {
    return;
  }
  const gp_Pnt& aP00 = mySurface->Poles().At(0, 0);
  const gp_Pnt& aP10 = mySurface->Poles().At(1, 0);
  const gp_Pnt& aP01 = mySurface->Poles().At(0, 1);
  const gp_Pnt& aP11 = mySurface->Poles().At(1, 1);
  for (int aCoordinate = 1; aCoordinate <= 3; ++aCoordinate)
  {
    if (double(CompensatedReal(aP11.Coord(aCoordinate)) - aP10.Coord(aCoordinate)
               - aP01.Coord(aCoordinate) + aP00.Coord(aCoordinate))
        != 0.0)
    {
      return;
    }
  }
  // Subtract affine-mapped poles with compensation; interpolate the small
  // residual with ordinary arithmetic.
  const NCollection_Array1<double>& aUKnots = mySurface->UKnotSequence();
  const NCollection_Array1<double>& aVKnots = mySurface->VKnotSequence();
  myAffineResidual.Resize(1, myCurve->NbPoles(), false);
  myAffineResidual.Init({});
  for (size_t anIndex = 0; anIndex < myAffineResidual.Size(); ++anIndex)
  {
    const gp_Pnt2d&       aUV = myPCurve->Poles().At(anIndex);
    const CompensatedReal aU =
      (CompensatedReal(aUV.X()) - aUKnots.At(1)) / (CompensatedReal(aUKnots.At(2)) - aUKnots.At(1));
    const CompensatedReal aV =
      (CompensatedReal(aUV.Y()) - aVKnots.At(1)) / (CompensatedReal(aVKnots.At(2)) - aVKnots.At(1));
    for (int aCoordinate = 1; aCoordinate <= 3; ++aCoordinate)
    {
      myAffineResidual.ChangeAt(anIndex)[aCoordinate - 1] = double(
        CompensatedReal(aP00.Coord(aCoordinate)) - myCurve->Poles().At(anIndex).Coord(aCoordinate)
        + aU * (CompensatedReal(aP10.Coord(aCoordinate)) - aP00.Coord(aCoordinate))
        + aV * (CompensatedReal(aP01.Coord(aCoordinate)) - aP00.Coord(aCoordinate)));
    }
  }
}

//=================================================================================================

template <class Real>
size_t BSplineResidual::findSpan(size_t                            theDegree,
                                 const NCollection_Array1<double>& theKnots,
                                 size_t                            theNbPoles,
                                 const Real&                       theParameter)
{
  const auto aSpanEnd =
    std::upper_bound(theKnots.cbegin() + theDegree + 1,
                     theKnots.cbegin() + theNbPoles,
                     theParameter,
                     [](const Real& theValue, double theKnot) { return theValue < Real(theKnot); });
  return static_cast<size_t>(aSpanEnd - theKnots.cbegin()) - 1;
}

//==================================================================================================

template <class Real, class ParameterAccessor>
BSplineResidual::Point<Real> BSplineResidual::evaluatePoles(
  size_t                            theDegree,
  const NCollection_Array1<double>& theKnots,
  size_t                            theSpan,
  const ParameterAccessor&          theParameter,
  size_t                            theDimension,
  Point<Real>*                      thePoles)
{
  for (size_t aLevel = 1; aLevel <= theDegree; ++aLevel)
  {
    const Real aParameter = theParameter(aLevel - 1);
    for (size_t anIndex = theDegree; anIndex >= aLevel; --anIndex)
    {
      const Real aLeftKnot  = theKnots.At(theSpan - theDegree + anIndex);
      const Real aRightKnot = theKnots.At(theSpan + 1 + anIndex - aLevel);
      const Real aWeight    = (aParameter - aLeftKnot) / (aRightKnot - aLeftKnot);
      for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
      {
        thePoles[anIndex][aCoordinate] = (1.0 - aWeight) * thePoles[anIndex - 1][aCoordinate]
                                         + aWeight * thePoles[anIndex][aCoordinate];
      }
    }
  }
  return thePoles[theDegree];
}

//==================================================================================================

template <class PoleAccessor>
void BSplineResidual::initCache(PolynomialCache&                  theCache,
                                size_t                            theDegree,
                                const NCollection_Array1<double>& theKnots,
                                size_t                            theNbPoles,
                                double                            theFirst,
                                double                            theLast,
                                size_t                            theDimension,
                                const PoleAccessor&               thePole)
{
  const double aMiddle = 0.5 * theFirst + 0.5 * theLast;
  const size_t aSpan   = findSpan(theDegree, theKnots, theNbPoles, aMiddle);
  const double aFirst = theKnots.At(aSpan), aLast = theKnots.At(aSpan + 1);
  if (theFirst < aFirst || theLast > aLast)
  {
    return;
  }
  NCollection_Array2<CompensatedPoint>        aCoefficients(theDegree + 1, size_t{1});
  NCollection_LocalArray<CompensatedPoint, 8> aPoles(theDegree + 1);
  for (size_t anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aPoles[anIndex] = thePole(aSpan - theDegree + anIndex);
  }
  NCollection_LocalArray<CompensatedPoint, 8> aWorkPoles(theDegree + 1);
  extractBezier(theDegree,
                theKnots,
                aSpan,
                theDimension,
                &aPoles[0],
                &aWorkPoles[0],
                &aCoefficients.ChangeAt(0));
  initPolynomialCache(theCache, aCoefficients, theDimension, {aFirst, 0.0}, {aLast, 1.0});
}

//==================================================================================================

void BSplineResidual::extractBezier(size_t                            theDegree,
                                    const NCollection_Array1<double>& theKnots,
                                    size_t                            theSpan,
                                    size_t                            theDimension,
                                    const CompensatedPoint*           thePoles,
                                    CompensatedPoint*                 theWorkPoles,
                                    CompensatedPoint*                 theCoefficients)
{
  const double aFirst = theKnots.At(theSpan), aLast = theKnots.At(theSpan + 1);
  const bool   isBezier =
    theKnots.At(theSpan - theDegree + 1) == aFirst && theKnots.At(theSpan + theDegree) == aLast;
  if (isBezier)
  {
    std::copy_n(thePoles, theDegree + 1, theCoefficients);
  }
  else
  {
    // Clamp the span ends by knot insertion.
    std::copy_n(thePoles, theDegree + 1, theWorkPoles);
    theCoefficients[theDegree] = theWorkPoles[theDegree];
    for (size_t aLevel = 1; aLevel <= theDegree; ++aLevel)
    {
      for (size_t anIndex = theDegree; anIndex >= aLevel; --anIndex)
      {
        const CompensatedReal aLeftKnot  = theKnots.At(theSpan - theDegree + anIndex);
        const CompensatedReal aRightKnot = theKnots.At(theSpan + 1 + anIndex - aLevel);
        const CompensatedReal aWeight =
          (CompensatedReal(aFirst) - aLeftKnot) / (aRightKnot - aLeftKnot);
        for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
        {
          theWorkPoles[anIndex][aCoordinate] =
            (1.0 - aWeight) * theWorkPoles[anIndex - 1][aCoordinate]
            + aWeight * theWorkPoles[anIndex][aCoordinate];
        }
      }
      theCoefficients[theDegree - aLevel] = theWorkPoles[theDegree];
    }
    for (size_t aLevel = 1; aLevel <= theDegree; ++aLevel)
    {
      for (size_t anIndex = theDegree; anIndex >= aLevel; --anIndex)
      {
        const CompensatedReal aWeight =
          (CompensatedReal(aLast) - aFirst)
          / (CompensatedReal(theKnots.At(theSpan + 1 + anIndex - aLevel)) - aFirst);
        for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
        {
          theCoefficients[anIndex][aCoordinate] =
            (1.0 - aWeight) * theCoefficients[anIndex - 1][aCoordinate]
            + aWeight * theCoefficients[anIndex][aCoordinate];
        }
      }
    }
  }
}

//==================================================================================================

BSplineResidual::BernsteinParameter BSplineResidual::parameterData(
  const CompensatedReal& theParameter,
  const CompensatedReal& theFirst,
  const CompensatedReal& theLength,
  size_t                 theDegree)
{
  const CompensatedReal  aU         = (theParameter - theFirst) / theLength;
  const CompensatedReal  aV         = 1.0 - aU;
  const bool             isReversed = aV < aU;
  const CompensatedReal& aLarge     = isReversed ? aU : aV;
  BernsteinParameter     aResult{(isReversed ? aV : aU) / aLarge, 1.0, isReversed};
  for (size_t aStep = 0; aStep < theDegree; ++aStep)
  {
    aResult.Factor = aResult.Factor * aLarge;
  }
  return aResult;
}

//==================================================================================================

template <class CoefficientAccessor>
BSplineResidual::CompensatedPoint BSplineResidual::horner(const BernsteinParameter&  theParameter,
                                                          size_t                     theDegree,
                                                          size_t                     theDimension,
                                                          const CoefficientAccessor& theCoefficient)
{
  CompensatedPoint aResult{0.0, 0.0, 0.0, 1.0};
  evalPolynomialValue(
    theDegree,
    theDimension,
    theParameter.Ratio,
    [&](size_t theStep) -> decltype(auto) {
      return theCoefficient(theParameter.IsReversed ? theStep : theDegree - theStep);
    },
    aResult.data());
  return aResult;
}

//==================================================================================================

void BSplineResidual::initSurfaceCache(PolynomialCache& theCache,
                                       size_t           theUSpan,
                                       size_t           theVSpan) const
{
  const size_t aUDegree = mySurface->UDegree(), aVDegree = mySurface->VDegree();
  const NCollection_Array1<double>&           aUKnots    = mySurface->UKnotSequence();
  const NCollection_Array1<double>&           aVKnots    = mySurface->VKnotSequence();
  const NCollection_Array2<double>*           aWeights   = mySurface->Weights();
  const size_t                                aDimension = aWeights == nullptr ? 3 : 4;
  NCollection_Array2<CompensatedPoint>        aCoefficients(aUDegree + 1, aVDegree + 1);
  const size_t                                aDegree = std::max(aUDegree, aVDegree);
  NCollection_LocalArray<CompensatedPoint, 8> aPoles(aDegree + 1), aWorkPoles(aDegree + 1),
    aBezierPoles(aDegree + 1);
  for (size_t aUIndex = 0; aUIndex <= aUDegree; ++aUIndex)
  {
    for (size_t aVIndex = 0; aVIndex <= aVDegree; ++aVIndex)
    {
      aPoles[aVIndex] = pole(mySurface->Poles(),
                             aWeights,
                             3,
                             theUSpan - aUDegree + aUIndex,
                             theVSpan - aVDegree + aVIndex);
    }
    extractBezier(aVDegree,
                  aVKnots,
                  theVSpan,
                  aDimension,
                  &aPoles[0],
                  &aWorkPoles[0],
                  &aCoefficients.ChangeAt(aUIndex, 0));
  }
  for (size_t aVIndex = 0; aVIndex <= aVDegree; ++aVIndex)
  {
    for (size_t aUIndex = 0; aUIndex <= aUDegree; ++aUIndex)
    {
      aPoles[aUIndex] = aCoefficients.At(aUIndex, aVIndex);
    }
    extractBezier(aUDegree,
                  aUKnots,
                  theUSpan,
                  aDimension,
                  &aPoles[0],
                  &aWorkPoles[0],
                  &aBezierPoles[0]);
    for (size_t aUIndex = 0; aUIndex <= aUDegree; ++aUIndex)
    {
      aCoefficients.ChangeAt(aUIndex, aVIndex) = aBezierPoles[aUIndex];
    }
  }
  initPolynomialCache(theCache,
                      aCoefficients,
                      aDimension,
                      {aUKnots.At(theUSpan), aVKnots.At(theVSpan)},
                      {aUKnots.At(theUSpan + 1), aVKnots.At(theVSpan + 1)});
}

//==================================================================================================

template <class Curve>
BSplineResidual::CompensatedPoint BSplineResidual::curveValue(
  const Curve&                                  theCurve,
  const NCollection_Array1<double>*             theWeights,
  const PolynomialCache&                        theCache,
  NCollection_DataMap<size_t, PolynomialCache>& theSpans,
  double                                        theParameter,
  size_t                                        theDimension)
{
  const size_t aDimension = theWeights == nullptr ? theDimension : 4;
  if (!theCache.Coefficients.IsEmpty())
  {
    return cacheValue(theCache, theParameter, 0.0, aDimension);
  }
  const auto&  aKnots  = theCurve.KnotSequence();
  const size_t aDegree = theCurve.Degree();
  const size_t aSpan   = findSpan(aDegree, aKnots, theCurve.NbPoles(), theParameter);
  const double aFirst = aKnots.At(aSpan), aLast = aKnots.At(aSpan + 1);
  if (theParameter >= aFirst && theParameter <= aLast)
  {
    const PolynomialCache* aSpanCache = theSpans.Seek(aSpan);
    if (aSpanCache == nullptr)
    {
      PolynomialCache aNewCache;
      initCache(aNewCache,
                aDegree,
                aKnots,
                theCurve.NbPoles(),
                aFirst,
                aLast,
                aDimension,
                [&](size_t theIndex) {
                  return pole(theCurve.Poles(), theWeights, theDimension, theIndex);
                });
      aSpanCache = &theSpans.TryBound(aSpan, std::move(aNewCache));
    }
    if (!aSpanCache->Coefficients.IsEmpty())
    {
      return cacheValue(*aSpanCache, theParameter, 0.0, aDimension);
    }
  }
  return evaluate<CompensatedReal>(
    theCurve.Degree(),
    theCurve.KnotSequence(),
    theCurve.NbPoles(),
    theParameter,
    [&](size_t theIndex) { return pole(theCurve.Poles(), theWeights, theDimension, theIndex); },
    aDimension);
}

//==================================================================================================

void BSplineResidual::initPolynomialCache(PolynomialCache&                      theCache,
                                          NCollection_Array2<CompensatedPoint>& theCoefficients,
                                          size_t                                theDimension,
                                          const std::array<double, 2>&          theFirst,
                                          const std::array<double, 2>&          theLast)
{
  CompensatedPoint anOrigin{};
  const auto       aPole = [&](size_t theIndex, size_t theCoordinate) -> CompensatedReal& {
    return theCoefficients.ChangeAt(theIndex)[theCoordinate];
  };
  translateControlPolygon(theCoefficients.Size(), theDimension, aPole, anOrigin.data());
  // The Horner ratio is at most one. Reserve room for each term and the origin.
  const double aLimit   = RealLast() / static_cast<double>(theCoefficients.Size() + 1);
  const size_t aUDegree = theCoefficients.NbRows() - 1;
  const size_t aVDegree = theCoefficients.NbColumns() - 1;
  for (size_t aUIndex = 0; aUIndex <= aUDegree; ++aUIndex)
  {
    for (size_t aVIndex = 0; aVIndex <= aVDegree; ++aVIndex)
    {
      const double aBinomial = PLib::Bin(static_cast<int>(aUDegree), static_cast<int>(aUIndex))
                               * PLib::Bin(static_cast<int>(aVDegree), static_cast<int>(aVIndex));
      for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
      {
        CompensatedReal& aValue = theCoefficients.ChangeAt(aUIndex, aVIndex)[aCoordinate];
        aValue                  = aValue * aBinomial;
        const double aMagnitude = std::abs(double(aValue));
        if (!Precision::IsFinite(aMagnitude) || aMagnitude > aLimit)
        {
          return;
        }
      }
    }
  }
  theCache.Coefficients = std::move(theCoefficients);
  theCache.Origin       = anOrigin;
  for (size_t aDirection = 0; aDirection < 2; ++aDirection)
  {
    theCache.First[aDirection]  = theFirst[aDirection];
    theCache.Last[aDirection]   = theLast[aDirection];
    theCache.Length[aDirection] = theCache.Last[aDirection] - theCache.First[aDirection];
  }
}

//==================================================================================================

BSplineResidual::CompensatedPoint BSplineResidual::cacheValue(const PolynomialCache& theCache,
                                                              const CompensatedReal& theU,
                                                              const CompensatedReal& theV,
                                                              size_t                 theDimension)
{
  const size_t             aUDegree = theCache.Coefficients.NbRows() - 1;
  const size_t             aVDegree = theCache.Coefficients.NbColumns() - 1;
  const BernsteinParameter aU =
    parameterData(theU, theCache.First[0], theCache.Length[0], aUDegree);
  CompensatedPoint aResult;
  CompensatedReal  aFactor = aU.Factor;
  if (aVDegree == 0)
  {
    aResult = horner(aU, aUDegree, theDimension, [&](size_t theIndex) -> const CompensatedPoint& {
      return theCache.Coefficients.At(theIndex);
    });
  }
  else
  {
    const BernsteinParameter aV =
      parameterData(theV, theCache.First[1], theCache.Length[1], aVDegree);
    aResult = horner(aU, aUDegree, theDimension, [&](size_t theUIndex) {
      return horner(aV, aVDegree, theDimension, [&](size_t theVIndex) -> const CompensatedPoint& {
        return theCache.Coefficients.At(theUIndex, theVIndex);
      });
    });
    aFactor = aFactor * aV.Factor;
  }
  for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
  {
    aResult[aCoordinate] = aFactor * aResult[aCoordinate] + theCache.Origin[aCoordinate];
  }
  return aResult;
}

//==================================================================================================

template <class Poles, class Weights, class... Indices>
BSplineResidual::CompensatedPoint BSplineResidual::pole(const Poles&   thePoles,
                                                        const Weights* theWeights,
                                                        size_t         theDimension,
                                                        Indices... theIndices)
{
  const auto&      aPole = thePoles.At(theIndices...);
  CompensatedPoint aResult{0.0, 0.0, 0.0, 1.0};
  if (theWeights != nullptr)
  {
    aResult[3] = theWeights->At(theIndices...);
  }
  for (size_t aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
  {
    aResult[aCoordinate] = aPole.Coord(static_cast<int>(aCoordinate) + 1);
    if (theWeights != nullptr)
    {
      aResult[aCoordinate] = aResult[aCoordinate] * aResult[3];
    }
  }
  return aResult;
}

//==================================================================================================

// de Boor evaluation of homogeneous poles, including end-span extrapolation.
template <class Real, class PoleAccessor>
BSplineResidual::Point<Real> BSplineResidual::evaluate(size_t                            theDegree,
                                                       const NCollection_Array1<double>& theKnots,
                                                       size_t                            theNbPoles,
                                                       const Real&         theParameter,
                                                       const PoleAccessor& thePole,
                                                       size_t              theDimension)
{
  const size_t aFirst = findSpan(theDegree, theKnots, theNbPoles, theParameter);
  NCollection_LocalArray<Point<Real>, 8> aPoles(theDegree + 1);
  for (size_t anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aPoles[anIndex] = thePole(aFirst - theDegree + anIndex);
  }
  return evaluatePoles(
    theDegree,
    theKnots,
    aFirst,
    [&](size_t) -> const Real& { return theParameter; },
    theDimension,
    &aPoles[0]);
}

//=================================================================================================

template <class Adaptor>
BSplineResidual::CompensatedPoint BSplineResidual::planeCurveValue(const Adaptor& theCurve,
                                                                   double         theParameter,
                                                                   size_t         theDimension)
{
  CompensatedPoint aPoint{};
  if (theCurve.GetType() == GeomAbs_Line)
  {
    const auto aLine = theCurve.Line();
    for (size_t anAxis = 0; anAxis < theDimension; ++anAxis)
    {
      const int aCoord = static_cast<int>(anAxis) + 1;
      aPoint[anAxis]   = CompensatedReal(aLine.Location().Coord(aCoord))
                       + CompensatedReal(theParameter) * aLine.Direction().Coord(aCoord);
    }
    aPoint[3] = 1.0;
    return aPoint;
  }
  if (theCurve.GetType() == GeomAbs_BSplineCurve)
  {
    const auto aCurve = theCurve.BSpline();
    if (aCurve->IsPeriodic())
    {
      theParameter =
        ElCLib::InPeriod(theParameter, aCurve->FirstParameter(), aCurve->LastParameter());
    }
    const NCollection_Array1<double>* aWeights = aCurve->Weights();
    aPoint                                     = evaluate<CompensatedReal>(
      aCurve->Degree(),
      aCurve->KnotSequence(),
      aCurve->NbPoles(),
      theParameter,
      [&](size_t theIndex) { return pole(aCurve->Poles(), aWeights, theDimension, theIndex); },
      aWeights == nullptr ? theDimension : 4);
  }
  else
  {
    const auto                        aCurve   = theCurve.Bezier();
    const NCollection_Array1<double>* aWeights = aCurve->Weights();
    const size_t                      aDegree  = aCurve->Degree();
    const size_t aDimension                    = aWeights == nullptr ? theDimension : aPoint.size();
    NCollection_LocalArray<CompensatedPoint, 8> aPoles(aDegree + 1);
    for (size_t anIndex = 0; anIndex <= aDegree; ++anIndex)
    {
      aPoles[anIndex] = pole(aCurve->Poles(), aWeights, theDimension, anIndex);
    }
    const CompensatedReal aParameter(theParameter);
    const CompensatedReal aComplement = CompensatedReal(1.0) - aParameter;
    for (size_t aLevel = aDegree; aLevel > 0; --aLevel)
    {
      for (size_t anIndex = 0; anIndex < aLevel; ++anIndex)
      {
        for (size_t anAxis = 0; anAxis < aDimension; ++anAxis)
        {
          aPoles[anIndex][anAxis] =
            aComplement * aPoles[anIndex][anAxis] + aParameter * aPoles[anIndex + 1][anAxis];
        }
      }
    }
    aPoint = aPoles[0];
  }
  if (theCurve.IsRational())
  {
    for (size_t anAxis = 0; anAxis < theDimension; ++anAxis)
    {
      aPoint[anAxis] = aPoint[anAxis] / aPoint[3];
    }
  }
  return aPoint;
}

//==================================================================================================

bool BSplineResidual::Value(double theParameter, gp_Vec& theResidual) const
{
  if (myPlaneCoS != nullptr)
  {
    const CompensatedPoint aCurve    = planeCurveValue(*myPlaneCurve, theParameter, 3);
    const CompensatedPoint aUV       = planeCurveValue(*myPlaneCoS->GetCurve(), theParameter, 2);
    const gp_Pln           aPlane    = myPlaneCoS->GetSurface()->Plane();
    const gp_Ax3&          aPosition = aPlane.Position();
    for (int anAxis = 1; anAxis <= 3; ++anAxis)
    {
      const CompensatedReal aSurface = CompensatedReal(aPosition.Location().Coord(anAxis))
                                       + aUV[0] * aPosition.XDirection().Coord(anAxis)
                                       + aUV[1] * aPosition.YDirection().Coord(anAxis);
      theResidual.SetCoord(anAxis, static_cast<double>(aSurface - aCurve[anAxis - 1]));
    }
    return Precision::IsFinite(theResidual.X()) && Precision::IsFinite(theResidual.Y())
           && Precision::IsFinite(theResidual.Z());
  }
  if (myCurve.IsNull())
  {
    return false;
  }
  gp_Vec aResidual;
  if (!myAffineResidual.IsEmpty())
  {
    const Point<double> aResidualPoint = evaluate<double>(
      myCurve->Degree(),
      myCurve->KnotSequence(),
      myCurve->NbPoles(),
      theParameter,
      [&](size_t theIndex) { return myAffineResidual.At(theIndex); },
      3);
    aResidual.SetCoord(aResidualPoint[0], aResidualPoint[1], aResidualPoint[2]);
  }
  else
  {
    const NCollection_Array1<double>* aCurveWeights  = myCurve->Weights();
    const NCollection_Array1<double>* aPCurveWeights = myPCurve->Weights();
    const CompensatedPoint            aCurvePoint =
      curveValue(*myCurve, aCurveWeights, myCaches->Curve, myCaches->CurveSpans, theParameter, 3);
    const CompensatedPoint            aUV = curveValue(*myPCurve,
                                            aPCurveWeights,
                                            myCaches->PCurve,
                                            myCaches->PCurveSpans,
                                            theParameter,
                                            2);
    const CompensatedReal             aU  = aPCurveWeights == nullptr ? aUV[0] : aUV[0] / aUV[3];
    const CompensatedReal             aV  = aPCurveWeights == nullptr ? aUV[1] : aUV[1] / aUV[3];
    const NCollection_Array2<double>* aSurfaceWeights   = mySurface->Weights();
    const bool                        isSurfaceRational = aSurfaceWeights != nullptr;
    CompensatedPoint                  aSurfacePoint;
    const PolynomialCache*            aSurfaceCache = nullptr;
    const size_t aUDegree = mySurface->UDegree(), aVDegree = mySurface->VDegree();
    const NCollection_Array1<double>& aUKnots = mySurface->UKnotSequence();
    const NCollection_Array1<double>& aVKnots = mySurface->VKnotSequence();
    if (Precision::IsFinite(double(aU)) && Precision::IsFinite(double(aV))
        && !(aU < CompensatedReal(aUKnots.At(aUDegree)))
        && !(CompensatedReal(aUKnots.At(mySurface->NbUPoles())) < aU)
        && !(aV < CompensatedReal(aVKnots.At(aVDegree)))
        && !(CompensatedReal(aVKnots.At(mySurface->NbVPoles())) < aV))
    {
      const size_t aUSpan = findSpan(aUDegree, aUKnots, mySurface->NbUPoles(), aU);
      const size_t aVSpan = findSpan(aVDegree, aVKnots, mySurface->NbVPoles(), aV);
      const size_t aPatch =
        (aUSpan - aUDegree) * mySurface->Poles().NbColumns() + aVSpan - aVDegree;
      aSurfaceCache = myCaches->SurfaceSpans.Seek(aPatch);
      if (aSurfaceCache == nullptr)
      {
        PolynomialCache aNewCache;
        initSurfaceCache(aNewCache, aUSpan, aVSpan);
        aSurfaceCache = &myCaches->SurfaceSpans.TryBound(aPatch, std::move(aNewCache));
      }
    }
    if (aSurfaceCache != nullptr && !aSurfaceCache->Coefficients.IsEmpty())
    {
      aSurfacePoint = cacheValue(*aSurfaceCache, aU, aV, isSurfaceRational ? 4 : 3);
    }
    else
    {
      const size_t aDimension = isSurfaceRational ? 4 : 3;
      const size_t aVSpan     = findSpan(aVDegree, aVKnots, mySurface->NbVPoles(), aV);
      NCollection_LocalArray<CompensatedPoint, 8> aVPoles(aVDegree + 1);
      aSurfacePoint = evaluate<CompensatedReal>(
        mySurface->UDegree(),
        mySurface->UKnotSequence(),
        mySurface->NbUPoles(),
        aU,
        [&](size_t theUIndex) {
          for (size_t aVIndex = 0; aVIndex <= aVDegree; ++aVIndex)
          {
            aVPoles[aVIndex] =
              pole(mySurface->Poles(), aSurfaceWeights, 3, theUIndex, aVSpan - aVDegree + aVIndex);
          }
          return evaluatePoles(
            aVDegree,
            aVKnots,
            aVSpan,
            [&](size_t) -> const CompensatedReal& { return aV; },
            aDimension,
            &aVPoles[0]);
        },
        aDimension);
    }

    for (size_t aCoordinate = 0; aCoordinate < 3; ++aCoordinate)
    {
      const CompensatedReal aSurfaceValue = isSurfaceRational
                                              ? aSurfacePoint[aCoordinate] / aSurfacePoint[3]
                                              : aSurfacePoint[aCoordinate];
      const CompensatedReal aCurveValue   = aCurveWeights != nullptr
                                              ? aCurvePoint[aCoordinate] / aCurvePoint[3]
                                              : aCurvePoint[aCoordinate];
      aResidual.SetCoord(static_cast<int>(aCoordinate) + 1, double(aSurfaceValue - aCurveValue));
    }
  }
  if (!Precision::IsFinite(aResidual.X()) || !Precision::IsFinite(aResidual.Y())
      || !Precision::IsFinite(aResidual.Z()))
  {
    return false;
  }
  theResidual = aResidual;
  return true;
}
} // namespace

class GeomLib_CheckCurveOnSurface_TargetFunc : public math_MultipleVarFunction
{
public:
  GeomLib_CheckCurveOnSurface_TargetFunc(const Adaptor3d_Curve& theC3D,
                                         const Adaptor3d_Curve& theCurveOnSurface,
                                         const BSplineResidual& theResidual,
                                         const double           theFirst,
                                         const double           theLast);

  // returns the number of parameters of the function
  // (the function is one-dimension).
  int NbVariables() const override { return 1; }

  // Evaluates a normalized interval coordinate.
  bool Value(const math_Vector& theX, double& theFVal) override;

  // returns value of the one-dimension-function when parameter
  // is equal to theX
  bool Value(const double theX, double& theFVal) const;

  // Returns the physical parameter interval.
  const MathUtils::Domain1D& Domain() const;

  // Returns whether any objective evaluation failed.
  bool HasFailed() const;

  // Computes the exact maximum for elementary compositions supported by the adaptors.
  bool ElementaryMaximum(double& theBestValue, double& theBestParameter) const
  {
    const GeomAbs_CurveType aType1 = myCurve1.GetType();
    const GeomAbs_CurveType aType2 = myCurve2.GetType();
    if (aType1 == GeomAbs_Line && aType2 == GeomAbs_Line)
    {
      double aFirstValue = RealLast(), aLastValue = RealLast();
      if (!Value(myDomain.Min, aFirstValue) || !Value(myDomain.Max, aLastValue))
      {
        return false;
      }

      if (aFirstValue <= aLastValue)
      {
        theBestValue     = aFirstValue;
        theBestParameter = myDomain.Min;
      }
      else
      {
        theBestValue     = aLastValue;
        theBestParameter = myDomain.Max;
      }
      return true;
    }

    const bool isTrigType1 = aType1 == GeomAbs_Circle || aType1 == GeomAbs_Ellipse;
    const bool isTrigType2 = aType2 == GeomAbs_Circle || aType2 == GeomAbs_Ellipse;
    if (!isTrigType1 || !isTrigType2)
    {
      return false;
    }

    struct Coefficients
    {
      gp_XYZ Origin;
      gp_XYZ Cos;
      gp_XYZ Sin;
    };

    const auto getCoefficients = [](const Adaptor3d_Curve&  theCurve,
                                    const GeomAbs_CurveType theType) -> Coefficients {
      if (theType == GeomAbs_Circle)
      {
        const gp_Circ aCircle = theCurve.Circle();
        return {aCircle.Location().XYZ(),
                aCircle.XAxis().Direction().XYZ() * aCircle.Radius(),
                aCircle.YAxis().Direction().XYZ() * aCircle.Radius()};
      }

      const gp_Elips anEllipse = theCurve.Ellipse();
      return {anEllipse.Location().XYZ(),
              anEllipse.XAxis().Direction().XYZ() * anEllipse.MajorRadius(),
              anEllipse.YAxis().Direction().XYZ() * anEllipse.MinorRadius()};
    };

    const auto [anOrigin1, aCosCoeff1, aSinCoeff1] = getCoefficients(myCurve1, aType1);
    const auto [anOrigin2, aCosCoeff2, aSinCoeff2] = getCoefficients(myCurve2, aType2);

    const gp_XYZ anOffset  = anOrigin1 - anOrigin2;
    const gp_XYZ aCosCoeff = aCosCoeff1 - aCosCoeff2;
    const gp_XYZ aSinCoeff = aSinCoeff1 - aSinCoeff2;

    // Coefficients of the derivative of the squared distance in trigonometric form.
    const double aCosCos   = 2.0 * aCosCoeff.Dot(aSinCoeff);
    const double aCosSin   = 0.5 * (aSinCoeff.SquareModulus() - aCosCoeff.SquareModulus());
    const double aCos      = anOffset.Dot(aSinCoeff);
    const double aSin      = -anOffset.Dot(aCosCoeff);
    const double aConstant = -aCosCoeff.Dot(aSinCoeff);

    // Exact zero is required here: a tolerance could classify a varying distance as constant.
    if (aCosCos == 0.0 && aCosSin == 0.0 && aCos == 0.0 && aSin == 0.0 && aConstant == 0.0)
    {
      theBestParameter = myDomain.Min;
      return Value(myDomain.Min, theBestValue);
    }

    const double aPeriod = 2.0 * M_PI;
    const double aFirst  = ElCLib::InPeriod(myDomain.Min, 0.0, aPeriod);
    const double aShift  = aFirst - myDomain.Min;
    const double aLast   = aFirst + std::min(myDomain.Max - myDomain.Min, aPeriod);
    const double aScale  = std::max(
      {std::abs(aCosCos), std::abs(aCosSin), std::abs(aCos), std::abs(aSin), std::abs(aConstant)});
    math_TrigonometricFunctionRoots aRoots(aCosCos / aScale,
                                           aCosSin / aScale,
                                           aCos / aScale,
                                           aSin / aScale,
                                           aConstant / aScale,
                                           aFirst,
                                           aLast);
    if (!aRoots.IsDone() || aRoots.InfiniteRoots()
        || (myDomain.Max - myDomain.Min >= aPeriod && aRoots.NbSolutions() == 0))
    {
      return false;
    }

    theBestValue          = RealLast();
    theBestParameter      = myDomain.Min;
    const auto updateBest = [&](const double theParameter) {
      double aValue = RealLast();
      if (!Value(theParameter, aValue))
      {
        return false;
      }
      if (aValue < theBestValue)
      {
        theBestValue     = aValue;
        theBestParameter = theParameter;
      }
      return true;
    };

    if (!updateBest(myDomain.Min) || !updateBest(myDomain.Max))
    {
      return false;
    }
    for (int anIndex = 1; anIndex <= aRoots.NbSolutions(); ++anIndex)
    {
      if (!updateBest(aRoots.Value(anIndex) - aShift))
      {
        return false;
      }
    }
    return true;
  }

private:
  GeomLib_CheckCurveOnSurface_TargetFunc operator=(GeomLib_CheckCurveOnSurface_TargetFunc&) =
    delete;

  const BSplineResidual&    myResidual;
  const Adaptor3d_Curve&    myCurve1;
  const Adaptor3d_Curve&    myCurve2;
  const MathUtils::Domain1D myDomain;            // Parameter bounds.
  mutable bool              myHasFailed = false; // Failure during this interval search.
};

//==================================================================================================

bool GeomLib_CheckCurveOnSurface_TargetFunc::Value(const math_Vector& theX, double& theFVal)
{
  return Value(myDomain.Lerp(theX(1)), theFVal);
}

//==================================================================================================

bool GeomLib_CheckCurveOnSurface_TargetFunc::Value(const double theX, double& theFVal) const
{
  if (!(myDomain.Min <= theX && theX <= myDomain.Max))
  {
    myHasFailed = true;
    return false;
  }

  gp_Vec aResidual;
  if (!myResidual.Value(theX, aResidual))
  {
    const gp_Pnt aP1(myCurve1.EvalD0(theX)), aP2(myCurve2.EvalD0(theX));
    aResidual = gp_Vec(aP1, aP2);
  }
  if (!Precision::IsFinite(aResidual.X()) || !Precision::IsFinite(aResidual.Y())
      || !Precision::IsFinite(aResidual.Z()))
  {
    myHasFailed = true;
    return false;
  }
  theFVal = -aResidual.SquareMagnitude();
  return true;
}

//==================================================================================================

bool GeomLib_CheckCurveOnSurface_TargetFunc::HasFailed() const
{
  return myHasFailed;
}

//==================================================================================================

const MathUtils::Domain1D& GeomLib_CheckCurveOnSurface_TargetFunc::Domain() const
{
  return myDomain;
}

//==================================================================================================

GeomLib_CheckCurveOnSurface_TargetFunc::GeomLib_CheckCurveOnSurface_TargetFunc(
  const Adaptor3d_Curve& theC3D,
  const Adaptor3d_Curve& theCurveOnSurface,
  const BSplineResidual& theResidual,
  const double           theFirst,
  const double           theLast)
    : myResidual(theResidual),
      myCurve1(theC3D),
      myCurve2(theCurveOnSurface),
      myDomain(theFirst, theLast)
{
}

//=================================================================================================

class GeomLib_CheckCurveOnSurface_Local
{
public:
  GeomLib_CheckCurveOnSurface_Local(const Array1OfHCurve&             theCurveArray,
                                    const Array1OfHCurve&             theCurveOnSurfaceArray,
                                    const NCollection_Array1<double>& theIntervalsArr,
                                    const double                      theEpsilonRange,
                                    const int                         theNbParticles)
      : myCurveArray(theCurveArray),
        myCurveOnSurfaceArray(theCurveOnSurfaceArray),
        mySubIntervals(theIntervalsArr),
        myEpsilonRange(theEpsilonRange),
        myNbParticles(theNbParticles),
        myResiduals(theCurveArray.Lower(), theCurveArray.Upper()),
        myResults(theIntervalsArr.Lower(), theIntervalsArr.Upper() - 1)
  {
  }

  void operator()(int theThreadIndex, int theElemIndex) const
  {
    std::optional<BSplineResidual>& aResidual = myResiduals(theThreadIndex);
    if (!aResidual)
    {
      aResidual.emplace(*myCurveArray(theThreadIndex),
                        *myCurveOnSurfaceArray(theThreadIndex),
                        mySubIntervals.First(),
                        mySubIntervals.Last());
    }
    // Compute the minimum objective for one interval.
    GeomLib_CheckCurveOnSurface_TargetFunc aFunc(
      *(myCurveArray.Value(theThreadIndex).get()),
      *(myCurveOnSurfaceArray.Value(theThreadIndex).get()),
      *aResidual,
      mySubIntervals.Value(theElemIndex),
      mySubIntervals.Value(theElemIndex + 1));

    double aMinDist = RealLast(), aPar = 0.0;
    if (!MinComputing(aFunc, myEpsilonRange, myNbParticles, aMinDist, aPar))
    {
      myResults(theElemIndex) = std::nullopt;
      return;
    }

    myResults(theElemIndex) = std::make_pair(aMinDist, aPar);
  }

  // Returns the minimum objective and parameter, or no result if an interval failed.
  std::optional<std::pair<double, double>> OptimalValues() const;

private:
  GeomLib_CheckCurveOnSurface_Local operator=(const GeomLib_CheckCurveOnSurface_Local&) = delete;

private:
  const Array1OfHCurve& myCurveArray;
  const Array1OfHCurve& myCurveOnSurfaceArray;

  const NCollection_Array1<double>& mySubIntervals;
  const double                      myEpsilonRange;
  const int                         myNbParticles;
  // Each worker reuses coefficients while the geometry stays unchanged.
  mutable NCollection_Array1<std::optional<BSplineResidual>> myResiduals;
  // Negative squared distance and parameter for each successful interval.
  mutable NCollection_Array1<std::optional<std::pair<double, double>>> myResults;
};

//==================================================================================================

std::optional<std::pair<double, double>> GeomLib_CheckCurveOnSurface_Local::OptimalValues() const
{
  // Every interval is required for a complete maximum.
  std::optional<std::pair<double, double>> aMinimum;
  for (const auto& aResult : myResults)
  {
    if (!aResult)
    {
      return std::nullopt;
    }
    if (!aMinimum || aResult->first < aMinimum->first)
    {
      aMinimum = aResult;
    }
  }
  return aMinimum;
}

//==================================================================================================

GeomLib_CheckCurveOnSurface::GeomLib_CheckCurveOnSurface()
    : myErrorStatus(0),
      myMaxDistance(RealLast()),
      myMaxParameter(0.),
      myTolRange(Precision::PConfusion()),
      myIsParallel(false)
{
}

//=================================================================================================

GeomLib_CheckCurveOnSurface::GeomLib_CheckCurveOnSurface(
  const occ::handle<Adaptor3d_Curve>& theCurve,
  const double                        theTolRange)
    : myCurve(theCurve),
      myErrorStatus(0),
      myMaxDistance(RealLast()),
      myMaxParameter(0.),
      myTolRange(theTolRange),
      myIsParallel(false)
{
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Init()
{
  myCurve.Nullify();
  myErrorStatus  = 0;
  myMaxDistance  = RealLast();
  myMaxParameter = 0.0;
  myTolRange     = Precision::PConfusion();
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Init(const occ::handle<Adaptor3d_Curve>& theCurve,
                                       const double                        theTolRange)
{
  myCurve        = theCurve;
  myErrorStatus  = 0;
  myMaxDistance  = RealLast();
  myMaxParameter = 0.0;
  myTolRange     = theTolRange;
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Perform(
  const occ::handle<Adaptor3d_CurveOnSurface>& theCurveOnSurface)
{
  myErrorStatus  = 0;
  myMaxDistance  = RealLast();
  myMaxParameter = 0.0;
  if (myCurve.IsNull() || theCurveOnSurface.IsNull())
  {
    myErrorStatus = 1;
    return;
  }

  if ((myCurve->FirstParameter() - theCurveOnSurface->FirstParameter() > myTolRange)
      || (myCurve->LastParameter() - theCurveOnSurface->LastParameter() < -myTolRange))
  {
    myErrorStatus = 2;
    return;
  }

  const double anEpsilonRange = 1.e-3;

  int aNbParticles = 3;

  // Polynomial function with degree n has not more than n-1 maxima and
  // minima (degree of 1st derivative is equal to n-1 => 1st derivative has
  // no greater than n-1 roots). Consequently, this function has
  // maximum n monotonicity intervals. That is a good idea to try to put
  // at least one particle in every monotonicity interval. Therefore,
  // number of particles should be equal to n.

  try
  {
    OCC_CATCH_SIGNALS

    NCollection_LinearVector<double> anIntervalValues;
    if (!FillSubIntervals(myCurve,
                          theCurveOnSurface->GetCurve(),
                          myCurve->FirstParameter(),
                          myCurve->LastParameter(),
                          aNbParticles,
                          anIntervalValues))
    {
      myErrorStatus = 3;
      return;
    }
    const NCollection_Array1<double> anIntervals     = anIntervalValues.ToArray1();
    const int                        aNbSubIntervals = anIntervals.Length() - 1;
    const int                        aNbThreads =
      myIsParallel
                               ? std::min(aNbSubIntervals, OSD_ThreadPool::DefaultPool()->NbDefaultThreadsToLaunch())
                               : 1;
    Array1OfHCurve aCurveArray(0, aNbThreads - 1);
    Array1OfHCurve aCurveOnSurfaceArray(0, aNbThreads - 1);
    for (int anI = 0; anI < aNbThreads; ++anI)
    {
      aCurveArray.SetValue(anI, aNbThreads > 1 ? myCurve->ShallowCopy() : myCurve);
      aCurveOnSurfaceArray.SetValue(
        anI,
        aNbThreads > 1 ? theCurveOnSurface->ShallowCopy()
                       : static_cast<const occ::handle<Adaptor3d_Curve>&>(theCurveOnSurface));
    }
    GeomLib_CheckCurveOnSurface_Local aComp(aCurveArray,
                                            aCurveOnSurfaceArray,
                                            anIntervals,
                                            anEpsilonRange,
                                            aNbParticles);
    if (aNbThreads > 1)
    {
      const occ::handle<OSD_ThreadPool>& aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher           aLauncher(*aThreadPool, aNbThreads);
      aLauncher.Perform(anIntervals.Lower(), anIntervals.Upper(), aComp);
    }
    else
    {
      for (int anI = anIntervals.Lower(); anI < anIntervals.Upper(); ++anI)
      {
        aComp(0, anI);
      }
    }
    const std::optional<std::pair<double, double>> aMaximum = aComp.OptimalValues();
    if (!aMaximum)
    {
      myErrorStatus = 3;
      return;
    }

    const auto [aSquaredDistance, aParameter] = *aMaximum;
    myMaxDistance                             = sqrt(-aSquaredDistance);
    myMaxParameter                            = aParameter;
  }
  catch (Standard_Failure const&)
  {
    myErrorStatus = 3;
  }
}

//=================================================================================================

namespace
{
//==================================================================================================

bool FillSubIntervals(const occ::handle<Adaptor3d_Curve>&   theCurve3d,
                      const occ::handle<Adaptor2d_Curve2d>& theCurve2d,
                      double                                theFirst,
                      double                                theLast,
                      int&                                  theNbParticles,
                      NCollection_LinearVector<double>&     theSubIntervals)
{
  constexpr size_t                 aMaxKnots  = 101;
  const double                     aBounds[2] = {theFirst, theLast};
  const NCollection_Array1<double> aBoundKnots(aBounds[0], 1, 2);
  theNbParticles = 3;
  try
  {
    OCC_CATCH_SIGNALS

    const occ::handle<Geom_BSplineCurve> aCurve3d =
      theCurve3d->GetType() == GeomAbs_BSplineCurve ? theCurve3d->BSpline() : nullptr;
    const occ::handle<Geom2d_BSplineCurve> aCurve2d =
      theCurve2d->GetType() == GeomAbs_BSplineCurve ? theCurve2d->BSpline() : nullptr;
    NCollection_Array1<double> aGenerated3d, aGenerated2d;
    const auto                 aGetKnots =
      [&](const auto&                 theCurve,
          NCollection_Array1<double>& theGenerated) -> const NCollection_Array1<double>& {
      if (theCurve.IsNull())
      {
        return aBoundKnots;
      }
      theNbParticles = std::max(theNbParticles, theCurve->Degree());
      const size_t aKnotCount =
        static_cast<size_t>(theCurve->LastUKnotIndex() - theCurve->FirstUKnotIndex() + 1);
      if (theCurve->Knots().Size() <= aMaxKnots || aKnotCount <= aMaxKnots)
      {
        return theCurve->Knots();
      }
      theGenerated.Resize(1, static_cast<int>(aMaxKnots), false);
      theGenerated.ChangeAt(0)             = theFirst;
      theGenerated.ChangeAt(aMaxKnots - 1) = theLast;
      const double aStep                   = (theLast - theFirst) / (aMaxKnots - 1);
      double       aParameter              = theFirst + aStep;
      for (size_t anIndex = 1; anIndex + 1 < aMaxKnots; ++anIndex, aParameter += aStep)
      {
        theGenerated.ChangeAt(anIndex) = aParameter;
      }
      return theGenerated;
    };
    const NCollection_Array1<double>& aKnots3d = aGetKnots(aCurve3d, aGenerated3d);
    const NCollection_Array1<double>& aKnots2d = aGetKnots(aCurve2d, aGenerated2d);
    theSubIntervals.Reserve(2 + std::min(aMaxKnots, aKnots3d.Size())
                            + std::min(aMaxKnots, aKnots2d.Size()));
    theSubIntervals.Append(theFirst);
    size_t anIndex3d = 0, anIndex2d = 0;
    while (anIndex3d < aKnots3d.Size() && anIndex2d < aKnots2d.Size())
    {
      const double aValue3d = aKnots3d.At(anIndex3d), aValue2d = aKnots2d.At(anIndex2d);
      const double aDelta = aValue3d - aValue2d;
      double       aValue;
      if (aDelta < Precision::PConfusion())
      {
        aValue = aValue3d;
        ++anIndex3d;
        if (-aDelta < Precision::PConfusion())
        {
          ++anIndex2d;
        }
      }
      else
      {
        aValue = aValue2d;
        ++anIndex2d;
      }
      if (aValue > theFirst && aValue < theLast)
      {
        theSubIntervals.Append(aValue);
      }
    }
    theSubIntervals.Append(theLast);
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
  return true;
}
} // namespace

//=================================================================================================

bool PSO_Perform(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                 const double                            theEpsilon,
                 const int                               theNbParticles,
                 double&                                 theBestValue,
                 math_Vector&                            theOutputParam)
{
  const MathUtils::Domain1D& aDomain = theFunction.Domain();
  if (aDomain.Length() <= 0.0)
  {
    return false;
  }

  math_Vector aStepPar(1, 1), aNormalizedInf(1, 1), aNormalizedSup(1, 1);
  aStepPar(1)       = theEpsilon;
  aNormalizedInf(1) = 0.0;
  aNormalizedSup(1) = 1.0;

  math_PSOParticlesPool aParticles(theNbParticles, 1);

  // They are used for finding a position of theNbParticles worst places
  const int aNbControlPoints = 3 * theNbParticles;

  for (int aCount = 0; aCount < aNbControlPoints; ++aCount)
  {
    const double aNormalizedParam = double(aCount) / (aNbControlPoints - 1);
    const double aPrm             = aDomain.Lerp(aNormalizedParam);
    double       aVal             = RealLast();
    if (!theFunction.Value(aPrm, aVal))
    {
      return false;
    }

    PSO_Particle* aParticle = aParticles.GetWorstParticle();

    if (aVal > aParticle->BestDistance)
    {
      continue;
    }

    aParticle->Position[0]     = aNormalizedParam;
    aParticle->BestPosition[0] = aNormalizedParam;
    aParticle->Distance        = aVal;
    aParticle->BestDistance    = aVal;
  }

  math_PSO aPSO(&theFunction, aNormalizedInf, aNormalizedSup, aStepPar);
  aPSO.Perform(aParticles, theNbParticles, theBestValue, theOutputParam);

  if (theFunction.HasFailed() || !std::isfinite(theBestValue) || theBestValue > 0.0)
  {
    return false;
  }
  theOutputParam(1) = aDomain.Lerp(theOutputParam(1));
  return true;
}

//=================================================================================================

bool MinComputing(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                  const double                            theEpsilon, // 1.0e-3
                  const int                               theNbParticles,
                  double&                                 theBestValue,
                  double&                                 theBestParameter)
{
  const MathUtils::Domain1D& aDomain = theFunction.Domain();
  math_Vector                anOutputParam(1, 1);
  theBestParameter = aDomain.Min;
  theBestValue     = RealLast();

  if (theFunction.ElementaryMaximum(theBestValue, theBestParameter))
  {
    return true;
  }

  if (!PSO_Perform(theFunction, theEpsilon, theNbParticles, theBestValue, anOutputParam))
  {
#ifdef OCCT_DEBUG
    std::cout << "BRepLib_CheckCurveOnSurface::Compute(): math_PSO is failed!" << std::endl;
#endif
    return false;
  }

  theBestParameter = anOutputParam(1);

  // Normalize the bracket to make tolerance independent of parameter origin.
  const double              aStep = theEpsilon * aDomain.Length();
  const MathUtils::Domain1D aBracket(std::max(aDomain.Min, theBestParameter - 0.5 * aStep),
                                     std::min(aDomain.Max, theBestParameter + 0.5 * aStep));
  auto aLocal = MathUtils::MakeScalar([&](double theParameter, double& theValue) {
    return theFunction.Value(aBracket.Lerp(theParameter), theValue);
  });

  MathUtils::Config aConfig;
  aConfig.XTolerance  = Precision::PConfusion() / std::max(1.0, aBracket.Length());
  const auto aMinimum = MathOpt::Brent(aLocal, 0.0, 1.0, aConfig);
  if (aMinimum.IsDone() && *aMinimum.Value < theBestValue)
  {
    theBestValue     = *aMinimum.Value;
    theBestParameter = aBracket.Lerp(*aMinimum.Root);
  }

  return !theFunction.HasFailed();
}
