// Copyright (c) 1996-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp.hxx>
#include <MathOpt_Brent.hxx>
#include <MathOpt_PSO.hxx>
#include <MathPoly_Quartic.hxx>
#include <NCollection_Array2.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

/// Maximum bisection iterations for derivative root isolation.
/// 53 iterations guarantee full double precision (~2^-53 relative interval).
constexpr int THE_MAX_BISECTION_ITER = 53;

/// Stack buffer size for Taylor polynomial coefficients.
/// 128 doubles covers degree 31 for 3D rational curves (32 rows * 4 columns).
constexpr int THE_CACHE_BUF_SIZE = 128;

/// Maximum curve degree for which direct polynomial root-finding is used.
/// Derivative polynomial degree is (curve degree - 1), and MathPoly::Quartic handles up to
/// degree 4.
constexpr int THE_MAX_DIRECT_ROOT_DEGREE = 5;

//=================================================================================================

class BndLib_Box2dCurve
{
public:
  BndLib_Box2dCurve();

  virtual ~BndLib_Box2dCurve();

  void SetCurve(const occ::handle<Geom2d_Curve>& aC);

  const occ::handle<Geom2d_Curve>& Curve() const;

  void SetRange(const double aT1, const double aT2);

  void Range(double& aT1, double& aT2) const;

  const Bnd_Box2d& Box() const;

  void Perform();

  void PerformOptimal(const double Tol);

  // Keep myT1 and myT2 unchanged
  void Clear();

  int ErrorStatus() const;
  //
  //-----------------------------
protected:
  void CheckData();
  void GetInfoBase();
  void PerformLineConic();
  void PerformBezier();
  void PerformBSpline();
  void PerformOther();
  void D0(const double, gp_Pnt2d&);
  //
  void Compute(const occ::handle<Geom2d_Conic>&,
               const GeomAbs_CurveType,
               const double,
               const double,
               Bnd_Box2d& aBox2D);
  //
  static int    Compute(const occ::handle<Geom2d_Conic>&, const GeomAbs_CurveType, double*);
  static bool   IsTypeBase(const occ::handle<Geom2d_Curve>&, GeomAbs_CurveType&);
  static double AdjustToPeriod(const double, const double);
  //
  void PerformOnePoint();
  //
  void PerformGenCurv(const double Tol = Precision::PConfusion());
  //
  int NbSamples();
  //
  double AdjustExtr(const double UMin,
                    const double UMax,
                    const double Extr0,
                    const int    CoordIndx,
                    const double Tol,
                    const bool   IsMin);
  //-----------------------------
protected:
  occ::handle<Geom2d_Curve> myCurve;
  Bnd_Box2d                 myBox;
  int                       myErrorStatus;
  occ::handle<Geom2d_Curve> myCurveBase;
  double                    myOffsetBase;
  bool                      myOffsetFlag;
  double                    myT1;
  double                    myT2;
  GeomAbs_CurveType         myTypeBase;
};

//
class Curv2dMaxMinCoordMVar
{
public:
  Curv2dMaxMinCoordMVar(const occ::handle<Geom2d_Curve>& theCurve,
                        const double                     UMin,
                        const double                     UMax,
                        const int                        CoordIndx,
                        const double                     Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const math_Vector& X, double& F)
  {
    if (!CheckInputData(X(1)))
    {
      return false;
    }
    gp_Pnt2d aP = myCurve->Value(X(1));

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

private:
  Curv2dMaxMinCoordMVar& operator=(const Curv2dMaxMinCoordMVar&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const occ::handle<Geom2d_Curve>& myCurve;
  double                           myUMin;
  double                           myUMax;
  int                              myCoordIndx;
  double                           mySign;
};

//
class Curv2dMaxMinCoord
{
public:
  Curv2dMaxMinCoord(const occ::handle<Geom2d_Curve>& theCurve,
                    const double                     UMin,
                    const double                     UMax,
                    const int                        CoordIndx,
                    const double                     Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const double X, double& F)
  {
    if (!CheckInputData(X))
    {
      return false;
    }
    gp_Pnt2d aP = myCurve->Value(X);

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

private:
  Curv2dMaxMinCoord& operator=(const Curv2dMaxMinCoord&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const occ::handle<Geom2d_Curve>& myCurve;
  double                           myUMin;
  double                           myUMax;
  int                              myCoordIndx;
  double                           mySign;
};

/// Evaluates derivative (or rational derivative numerator) for a single coordinate
/// from cached 2D Taylor polynomial coefficients.
/// @param theCacheBuf  buffer with Taylor polynomial coefficients
/// @param theRowLen    number of columns per row (2 for non-rational 2D, 3 for rational)
/// @param theDegree    polynomial degree
/// @param theCoord     coordinate index (0=X, 1=Y)
/// @param theIsRational true for rational curves (NURBS)
/// @param theT         local parameter value in [0, 1]
/// @return for non-rational: f'_c(t); for rational: d(xw)/dt * w - xw * dw/dt
static double evaluateDerivCoord2d(const double* theCacheBuf,
                                   const int     theRowLen,
                                   const int     theDegree,
                                   const int     theCoord,
                                   const bool    theIsRational,
                                   const double  theT)
{
  if (!theIsRational)
  {
    double aResult = 0.0;
    double aPow    = 1.0;
    for (int i = 0; i < theDegree; ++i)
    {
      aResult += (i + 1) * theCacheBuf[(i + 1) * theRowLen + theCoord] * aPow;
      aPow *= theT;
    }
    return aResult;
  }

  const int aWCoord = theRowLen - 1;
  double    aXW = 0.0, aW = 0.0, aDXW = 0.0, aDW = 0.0;
  double    aPow = 1.0;
  for (int i = 0; i <= theDegree; ++i)
  {
    aXW += theCacheBuf[i * theRowLen + theCoord] * aPow;
    aW += theCacheBuf[i * theRowLen + aWCoord] * aPow;
    if (i < theDegree)
    {
      aDXW += (i + 1) * theCacheBuf[(i + 1) * theRowLen + theCoord] * aPow;
      aDW += (i + 1) * theCacheBuf[(i + 1) * theRowLen + aWCoord] * aPow;
    }
    aPow *= theT;
  }
  return aDXW * aW - aXW * aDW;
}

/// Finds coordinate extrema of a non-rational 2D polynomial span using direct root-finding.
/// Supports derivative polynomial degree 1-4 (curve degree 2-5).
/// @param theCacheBuf  buffer with Taylor polynomial coefficients
/// @param theRowLen    number of columns per row
/// @param theDegree    polynomial degree of the curve span
/// @param theTMin      lower bound of local parameter range
/// @param theTMax      upper bound of local parameter range
/// @param theBox       bounding box to extend with extremal points
static void addPolySpanExtrema2d(const double* theCacheBuf,
                                 const int     theRowLen,
                                 const int     theDegree,
                                 const double  theTMin,
                                 const double  theTMax,
                                 Bnd_Box2d&    theBox)
{
  if (theDegree < 2)
  {
    return;
  }

  const int aDerivDeg = theDegree - 1;

  for (int aCoord = 0; aCoord < 2; ++aCoord)
  {
    double aDerivCoeffs[5];
    for (int i = 0; i <= aDerivDeg; ++i)
    {
      aDerivCoeffs[i] = (i + 1.0) * theCacheBuf[(i + 1) * theRowLen + aCoord];
    }

    // Find derivative roots using MathPoly direct solvers (leading coefficient first)
    MathUtils::PolyResult aPolyResult;
    switch (aDerivDeg)
    {
      case 1:
        aPolyResult = MathPoly::Linear(aDerivCoeffs[1], aDerivCoeffs[0]);
        break;
      case 2:
        aPolyResult = MathPoly::Quadratic(aDerivCoeffs[2], aDerivCoeffs[1], aDerivCoeffs[0]);
        break;
      case 3:
        aPolyResult =
          MathPoly::Cubic(aDerivCoeffs[3], aDerivCoeffs[2], aDerivCoeffs[1], aDerivCoeffs[0]);
        break;
      case 4:
        aPolyResult = MathPoly::Quartic(aDerivCoeffs[4],
                                        aDerivCoeffs[3],
                                        aDerivCoeffs[2],
                                        aDerivCoeffs[1],
                                        aDerivCoeffs[0]);
        break;
      default:
        break;
    }

    if (!aPolyResult.IsDone())
    {
      continue;
    }

    for (size_t i = 0; i < aPolyResult.NbRoots; ++i)
    {
      const double aRoot = aPolyResult.Roots[i];
      if (aRoot > theTMin + Precision::PConfusion() && aRoot < theTMax - Precision::PConfusion())
      {
        double aPoint[4];
        PLib::NoDerivativeEvalPolynomial(aRoot,
                                         theDegree,
                                         theRowLen,
                                         theDegree * theRowLen,
                                         theCacheBuf[0],
                                         aPoint[0]);
        theBox.Add(gp_Pnt2d(aPoint[0], aPoint[1]));
      }
    }
  }
}

/// Fallback 2D extrema finder for rational or high-degree spans.
/// Uses sign-change detection on sampled derivative values followed by
/// bisection refinement to isolate each root.
/// @param theCacheBuf   buffer with Taylor polynomial coefficients
/// @param theRowLen     number of columns per row
/// @param theDegree     polynomial degree of the curve span
/// @param theIsRational true for rational curves
/// @param theTMin       lower bound of local parameter range
/// @param theTMax       upper bound of local parameter range
/// @param theBox        bounding box to extend with extremal points
static void addSpanExtremaByBisection2d(const double* theCacheBuf,
                                        const int     theRowLen,
                                        const int     theDegree,
                                        const bool    theIsRational,
                                        const double  theTMin,
                                        const double  theTMax,
                                        Bnd_Box2d&    theBox)
{
  if (theDegree < 2)
  {
    return;
  }

  const int    aNbSamples = std::max(4 * theDegree, 8);
  const double aDt        = (theTMax - theTMin) / aNbSamples;

  for (int aCoord = 0; aCoord < 2; ++aCoord)
  {
    double aPrevDeriv =
      evaluateDerivCoord2d(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, theTMin);

    for (int i = 1; i <= aNbSamples; ++i)
    {
      const double aT = theTMin + i * aDt;
      const double aCurrDeriv =
        evaluateDerivCoord2d(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, aT);

      if (aPrevDeriv * aCurrDeriv < 0.0)
      {
        double aTLow  = theTMin + (i - 1) * aDt;
        double aTHigh = aT;
        double aDLow  = aPrevDeriv;

        for (int anIter = 0; anIter < THE_MAX_BISECTION_ITER; ++anIter)
        {
          const double aTMid = (aTLow + aTHigh) * 0.5;
          const double aDMid =
            evaluateDerivCoord2d(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, aTMid);

          if (aDLow * aDMid < 0.0)
          {
            aTHigh = aTMid;
          }
          else
          {
            aTLow = aTMid;
            aDLow = aDMid;
          }
        }

        const double aTRoot = (aTLow + aTHigh) * 0.5;
        double       aPoint[4];
        PLib::NoDerivativeEvalPolynomial(aTRoot,
                                         theDegree,
                                         theRowLen,
                                         theDegree * theRowLen,
                                         theCacheBuf[0],
                                         aPoint[0]);

        if (theIsRational)
        {
          const double aW = aPoint[theRowLen - 1];
          theBox.Add(gp_Pnt2d(aPoint[0] / aW, aPoint[1] / aW));
        }
        else
        {
          theBox.Add(gp_Pnt2d(aPoint[0], aPoint[1]));
        }
      }

      aPrevDeriv = aCurrDeriv;
    }
  }
}

/// Computes exact bounding box for a 2D BSpline curve over its full parameter range.
/// Uses per-span polynomial root-finding to locate coordinate extrema.
/// @param theCurve  BSpline curve
/// @param theBox    bounding box to extend
static void addBSplineBBox2d(const occ::handle<Geom2d_BSplineCurve>& theCurve, Bnd_Box2d& theBox)
{
  const int  aDegree      = theCurve->Degree();
  const bool anIsRational = theCurve->IsRational();
  const bool anIsPeriodic = theCurve->IsPeriodic();

  const NCollection_Array1<gp_Pnt2d>& aPoles     = theCurve->Poles();
  const NCollection_Array1<double>*   aWeights   = theCurve->Weights();
  const NCollection_Array1<double>&   aFlatKnots = theCurve->KnotSequence();
  const NCollection_Array1<double>&   aKnots     = theCurve->Knots();
  const int                           aRowLen    = anIsRational ? 3 : 2;

  // Add all distinct knot points
  for (int i = aKnots.Lower(); i <= aKnots.Upper(); ++i)
  {
    gp_Pnt2d aPnt;
    theCurve->D0(aKnots(i), aPnt);
    theBox.Add(aPnt);
  }

  // Process each non-degenerate knot span
  const int aSpanMin = aFlatKnots.Lower() + aDegree;
  const int aSpanMax = aFlatKnots.Upper() - aDegree - 1;

  for (int anISpan = aSpanMin; anISpan <= aSpanMax; ++anISpan)
  {
    const double aSpanStart  = aFlatKnots(anISpan);
    const double aSpanEnd    = aFlatKnots(anISpan + 1);
    const double aSpanLength = aSpanEnd - aSpanStart;

    if (aSpanLength < Precision::PConfusion())
    {
      continue;
    }

    double                     aCacheBuf[THE_CACHE_BUF_SIZE];
    NCollection_Array2<double> aCacheArray(aCacheBuf[0], 1, aDegree + 1, 1, aRowLen);

    BSplCLib::BuildCache(aSpanStart,
                         aSpanLength,
                         anIsPeriodic,
                         aDegree,
                         anISpan,
                         aFlatKnots,
                         aPoles,
                         aWeights,
                         aCacheArray);

    if (!anIsRational && aDegree <= THE_MAX_DIRECT_ROOT_DEGREE)
    {
      addPolySpanExtrema2d(aCacheBuf, aRowLen, aDegree, 0.0, 1.0, theBox);
    }
    else
    {
      addSpanExtremaByBisection2d(aCacheBuf, aRowLen, aDegree, anIsRational, 0.0, 1.0, theBox);
    }
  }
}

/// Computes exact bounding box for a 2D Bezier curve over a given parameter range.
/// @param theCurve  Bezier curve
/// @param theTMin   start of parameter range
/// @param theTMax   end of parameter range
/// @param theBox    bounding box to extend
static void addBezierBBox2d(const occ::handle<Geom2d_BezierCurve>& theCurve,
                            const double                           theTMin,
                            const double                           theTMax,
                            Bnd_Box2d&                             theBox)
{
  const int  aDegree      = theCurve->Degree();
  const bool anIsRational = theCurve->IsRational();
  const int  aRowLen      = anIsRational ? 3 : 2;

  const NCollection_Array1<gp_Pnt2d>& aPoles     = theCurve->Poles();
  const NCollection_Array1<double>*   aWeights   = theCurve->Weights();
  const NCollection_Array1<double>&   aFlatKnots = theCurve->KnotSequence();

  // Add endpoints
  gp_Pnt2d aPnt;
  theCurve->D0(theTMin, aPnt);
  theBox.Add(aPnt);
  theCurve->D0(theTMax, aPnt);
  theBox.Add(aPnt);

  if (aDegree < 2)
  {
    return;
  }

  const double aSpanStart  = theCurve->FirstParameter();
  const double aSpanLength = theCurve->LastParameter() - aSpanStart;
  const int    aSpanIndex  = aFlatKnots.Lower() + aDegree;

  double                     aCacheBuf[THE_CACHE_BUF_SIZE];
  NCollection_Array2<double> aCacheArray(aCacheBuf[0], 1, aDegree + 1, 1, aRowLen);

  BSplCLib::BuildCache(aSpanStart,
                       aSpanLength,
                       false,
                       aDegree,
                       aSpanIndex,
                       aFlatKnots,
                       aPoles,
                       aWeights,
                       aCacheArray);

  const double aTMin = std::max(0.0, (theTMin - aSpanStart) / aSpanLength);
  const double aTMax = std::min(1.0, (theTMax - aSpanStart) / aSpanLength);

  if (!anIsRational && aDegree <= THE_MAX_DIRECT_ROOT_DEGREE)
  {
    addPolySpanExtrema2d(aCacheBuf, aRowLen, aDegree, aTMin, aTMax, theBox);
  }
  else
  {
    addSpanExtremaByBisection2d(aCacheBuf, aRowLen, aDegree, anIsRational, aTMin, aTMax, theBox);
  }
}

//=================================================================================================

BndLib_Box2dCurve::BndLib_Box2dCurve()
{
  Clear();
}

//=================================================================================================

BndLib_Box2dCurve::~BndLib_Box2dCurve() = default;

//=================================================================================================

void BndLib_Box2dCurve::Clear()
{
  myBox.SetVoid();
  //
  myErrorStatus = -1;
  myTypeBase    = GeomAbs_OtherCurve;
  myOffsetBase  = 0.;
  myOffsetFlag  = false;
}

//=================================================================================================

void BndLib_Box2dCurve::SetCurve(const occ::handle<Geom2d_Curve>& aC2D)
{
  myCurve = aC2D;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BndLib_Box2dCurve::Curve() const
{
  return myCurve;
}

//=================================================================================================

void BndLib_Box2dCurve::SetRange(const double aT1, const double aT2)
{
  myT1 = aT1;
  myT2 = aT2;
}

//=================================================================================================

void BndLib_Box2dCurve::Range(double& aT1, double& aT2) const
{
  aT1 = myT1;
  aT2 = myT2;
}

//=================================================================================================

int BndLib_Box2dCurve::ErrorStatus() const
{
  return myErrorStatus;
}

//=================================================================================================

const Bnd_Box2d& BndLib_Box2dCurve::Box() const
{
  return myBox;
}

//=================================================================================================

void BndLib_Box2dCurve::CheckData()
{
  myErrorStatus = 0;
  //
  if (myCurve.IsNull())
  {
    myErrorStatus = 10;
    return;
  }
  //
  if (myT1 > myT2)
  {
    myErrorStatus = 12; // invalid range
    return;
  }
}

//=================================================================================================

void BndLib_Box2dCurve::Perform()
{
  Clear();
  //
  myErrorStatus = 0;
  //
  CheckData();
  if (myErrorStatus)
  {
    return;
  }
  //
  if (myT1 == myT2)
  {
    PerformOnePoint();
    return;
  }
  //
  GetInfoBase();
  if (myErrorStatus)
  {
    return;
  }
  //
  if (myTypeBase == GeomAbs_Line || myTypeBase == GeomAbs_Circle || myTypeBase == GeomAbs_Ellipse
      || myTypeBase == GeomAbs_Parabola || myTypeBase == GeomAbs_Hyperbola)
  { // LineConic
    PerformLineConic();
  }
  else if (myTypeBase == GeomAbs_BezierCurve)
  { // Bezier
    PerformBezier();
  }
  else if (myTypeBase == GeomAbs_BSplineCurve)
  { // B-Spline
    PerformBSpline();
  }
  else
  {
    myErrorStatus = 11; // unknown type base
  }
}

//=================================================================================================

void BndLib_Box2dCurve::PerformOptimal(const double Tol)
{
  Clear();
  myErrorStatus = 0;
  CheckData();

  if (myErrorStatus)
  {
    return;
  }

  if (myT1 == myT2)
  {
    PerformOnePoint();
    return;
  }

  GetInfoBase();
  if (myErrorStatus)
  {
    return;
  }

  if (myTypeBase == GeomAbs_Line || myTypeBase == GeomAbs_Circle || myTypeBase == GeomAbs_Ellipse
      || myTypeBase == GeomAbs_Parabola || myTypeBase == GeomAbs_Hyperbola)
  { // LineConic
    PerformLineConic();
  }
  else if (myTypeBase == GeomAbs_BezierCurve)
  {
    PerformBezier();
  }
  else if (myTypeBase == GeomAbs_BSplineCurve)
  {
    PerformBSpline();
  }
  else
  {
    PerformGenCurv(Tol);
  }
}

//=================================================================================================

void BndLib_Box2dCurve::PerformOnePoint()
{
  gp_Pnt2d aP2D;
  //
  myCurve->D0(myT1, aP2D);
  myBox.Add(aP2D);
}

//=================================================================================================

void BndLib_Box2dCurve::PerformBezier()
{
  if (myOffsetFlag)
  {
    PerformOther();
    return;
  }

  myErrorStatus = 0;

  occ::handle<Geom2d_BezierCurve> aCBz = occ::down_cast<Geom2d_BezierCurve>(myCurveBase);

  double aTMin = std::max(myT1, aCBz->FirstParameter());
  double aTMax = std::min(myT2, aCBz->LastParameter());

  addBezierBBox2d(aCBz, aTMin, aTMax, myBox);
}

//=================================================================================================

void BndLib_Box2dCurve::PerformBSpline()
{
  if (myOffsetFlag)
  {
    PerformOther();
    return;
  }

  myErrorStatus = 0;

  occ::handle<Geom2d_BSplineCurve> aCBS = occ::down_cast<Geom2d_BSplineCurve>(myCurveBase);
  const double                     aT1  = aCBS->FirstParameter();
  const double                     aT2  = aCBS->LastParameter();

  double aTMin = std::max(myT1, aT1);
  double aTMax = std::min(myT2, aT2);

  if (aTMax < aTMin)
  {
    aTMin = aT1;
    aTMax = aT2;
  }

  // Segment the curve if the range doesn't match
  constexpr double eps = Precision::PConfusion();
  if (std::abs(aT1 - aTMin) > eps || std::abs(aT2 - aTMax) > eps)
  {
    occ::handle<Geom2d_Geometry>     aG    = aCBS->Copy();
    occ::handle<Geom2d_BSplineCurve> aCBSs = occ::down_cast<Geom2d_BSplineCurve>(aG);
    aCBSs->Segment(aTMin, aTMax);
    aCBS = aCBSs;
  }

  addBSplineBBox2d(aCBS, myBox);
}

//=================================================================================================

void BndLib_Box2dCurve::PerformOther()
{
  int      j, aNb;
  double   aT, dT;
  gp_Pnt2d aP2D;
  //
  aNb = 33;
  dT  = (myT2 - myT1) / (aNb - 1);
  //
  for (j = 0; j < aNb; ++j)
  {
    aT = myT1 + j * dT;
    myCurve->D0(aT, aP2D);
    myBox.Add(aP2D);
  }
  myCurve->D0(myT2, aP2D);
  myBox.Add(aP2D);
}

//=================================================================================================

int BndLib_Box2dCurve::NbSamples()
{
  int N;
  switch (myTypeBase)
  {
    case GeomAbs_BezierCurve: {
      occ::handle<Geom2d_BezierCurve> aCBz = occ::down_cast<Geom2d_BezierCurve>(myCurveBase);
      N                                    = aCBz->NbPoles();
      // By default parametric range of Bezier curv is [0, 1]
      double du = myT2 - myT1;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineCurve: {
      occ::handle<Geom2d_BSplineCurve> aCBS = occ::down_cast<Geom2d_BSplineCurve>(myCurveBase);
      N                                     = (aCBS->Degree() + 1) * (aCBS->NbKnots() - 1);
      double umin = aCBS->FirstParameter(), umax = aCBS->LastParameter();
      double du = (myT2 - myT1) / (umax - umin);
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 17;
  }
  return std::min(23, N);
}

//=================================================================================================

double BndLib_Box2dCurve::AdjustExtr(const double UMin,
                                     const double UMax,
                                     const double Extr0,
                                     const int    CoordIndx,
                                     const double Tol,
                                     const bool   IsMin)
{
  double aSign = IsMin ? 1. : -1.;
  double extr  = aSign * Extr0;
  //
  double Du = (myCurve->LastParameter() - myCurve->FirstParameter());
  //
  Geom2dAdaptor_Curve aGAC(myCurve);
  double              UTol   = std::max(aGAC.Resolution(Tol), Precision::PConfusion());
  double              reltol = UTol / std::max(std::abs(UMin), std::abs(UMax));
  if (UMax - UMin < 0.01 * Du)
  {
    // It is suggested that function has one extremum on small interval
    MathUtils::Config       aBrentConfig(reltol, 100);
    Curv2dMaxMinCoord       aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
    MathUtils::ScalarResult aBrentResult = MathOpt::Brent(aFunc, UMin, UMax, aBrentConfig);
    if (aBrentResult.IsDone())
    {
      return aSign * (*aBrentResult.Value);
    }
  }
  //
  const int aNbParticles = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  double    aMaxStep     = (UMax - UMin) / (aNbParticles + 1);

  math_Vector aLowBorder(1, 1);
  math_Vector aUppBorder(1, 1);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;

  Curv2dMaxMinCoordMVar   aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
  MathOpt::PSOConfig      aPSOConfig(aNbParticles, 100);
  MathUtils::VectorResult aPSOResult = MathOpt::PSO(aFunc, aLowBorder, aUppBorder, aPSOConfig);
  //
  if (aPSOResult.IsDone())
  {
    const double aStep  = std::min(0.1 * Du, aMaxStep);
    const double aBestT = (*aPSOResult.Solution)(1);

    MathUtils::Config       aBrentConfig(reltol, 100);
    Curv2dMaxMinCoord       aFunc1(myCurve, UMin, UMax, CoordIndx, aSign);
    MathUtils::ScalarResult aBrentResult = MathOpt::Brent(aFunc1,
                                                          std::max(aBestT - aStep, UMin),
                                                          std::min(aBestT + aStep, UMax),
                                                          aBrentConfig);

    if (aBrentResult.IsDone())
    {
      return aSign * (*aBrentResult.Value);
    }
    return aSign * (*aPSOResult.Value);
  }

  return aSign * extr;
}

//=================================================================================================

void BndLib_Box2dCurve::PerformGenCurv(const double Tol)
{
  //
  int Nu = NbSamples();
  //
  double CoordMin[2] = {RealLast(), RealLast()};
  double CoordMax[2] = {-RealLast(), -RealLast()};
  double DeflMax[2]  = {-RealLast(), -RealLast()};
  //
  gp_Pnt2d                  P;
  int                       i, k;
  double                    du = (myT2 - myT1) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XY> aPnts(1, Nu);
  double                    u;
  for (i = 1, u = myT1; i <= Nu; i++, u += du)
  {
    D0(u, P);
    aPnts(i) = P.XY();
    //
    for (k = 0; k < 2; ++k)
    {
      if (CoordMin[k] > P.Coord(k + 1))
      {
        CoordMin[k] = P.Coord(k + 1);
      }
      if (CoordMax[k] < P.Coord(k + 1))
      {
        CoordMax[k] = P.Coord(k + 1);
      }
    }
    //
    if (i > 1)
    {
      gp_XY aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      D0(u - du2, P);
      gp_XY aD = (P.XY() - aPm);
      for (k = 0; k < 2; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
        {
          CoordMin[k] = P.Coord(k + 1);
        }
        if (CoordMax[k] < P.Coord(k + 1))
        {
          CoordMax[k] = P.Coord(k + 1);
        }
        double d = std::abs(aD.Coord(k + 1));
        if (DeflMax[k] < d)
        {
          DeflMax[k] = d;
        }
      }
    }
  }
  //
  // Adjusting minmax
  for (k = 0; k < 2; ++k)
  {
    double d = DeflMax[k];
    if (d <= Tol)
    {
      continue;
    }
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double tmin, tmax;
        tmin        = myT1 + std::max(0, i - 2) * du;
        tmax        = myT1 + std::min(Nu - 1, i) * du;
        double cmin = AdjustExtr(tmin, tmax, CMin, k + 1, Tol, true);
        if (cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double tmin, tmax;
        tmin        = myT1 + std::max(0, i - 2) * du;
        tmax        = myT1 + std::min(Nu - 1, i) * du;
        double cmax = AdjustExtr(tmin, tmax, CMax, k + 1, Tol, false);
        if (cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  myBox.Add(gp_Pnt2d(CoordMin[0], CoordMin[1]));
  myBox.Add(gp_Pnt2d(CoordMax[0], CoordMax[1]));
  myBox.Enlarge(Tol);
}

//=================================================================================================

void BndLib_Box2dCurve::D0(const double aU, gp_Pnt2d& aP2D)
{
  gp_Vec2d aV1;
  //
  myCurveBase->D1(aU, aP2D, aV1);
  //
  if (myOffsetFlag)
  {
    int    aIndex, aMaxDegree;
    double aA, aB, aR, aRes;
    //
    aMaxDegree = 9;
    aIndex     = 2;
    aRes       = gp::Resolution();
    //
    while (aV1.Magnitude() <= aRes && aIndex <= aMaxDegree)
    {
      aV1 = myCurveBase->DN(aU, aIndex);
      ++aIndex;
    }
    //
    aA = aV1.Y();
    aB = -aV1.X();
    aR = std::sqrt(aA * aA + aB * aB);
    if (aR <= aRes)
    {
      myErrorStatus = 13;
      return;
    }
    //
    aR = myOffsetBase / aR;
    aA = aA * aR;
    aB = aB * aR;
    aP2D.SetCoord(aP2D.X() + aA, aP2D.Y() + aB);
  }
  //
}

//=================================================================================================

void BndLib_Box2dCurve::GetInfoBase()
{
  bool                             bIsTypeBase;
  int                              iTrimmed, iOffset;
  GeomAbs_CurveType                aTypeB;
  occ::handle<Geom2d_Curve>        aC2DB;
  occ::handle<Geom2d_TrimmedCurve> aCT2D;
  occ::handle<Geom2d_OffsetCurve>  aCF2D;
  //
  myErrorStatus = 0;
  myTypeBase    = GeomAbs_OtherCurve;
  myOffsetBase  = 0;
  //
  aC2DB       = myCurve;
  bIsTypeBase = IsTypeBase(aC2DB, aTypeB);
  if (bIsTypeBase)
  {
    myTypeBase  = aTypeB;
    myCurveBase = myCurve;
    return;
  }
  //
  while (!bIsTypeBase)
  {
    iTrimmed = 0;
    iOffset  = 0;
    aCT2D    = occ::down_cast<Geom2d_TrimmedCurve>(aC2DB);
    if (!aCT2D.IsNull())
    {
      aC2DB = aCT2D->BasisCurve();
      ++iTrimmed;
    }
    //
    aCF2D = occ::down_cast<Geom2d_OffsetCurve>(aC2DB);
    if (!aCF2D.IsNull())
    {
      double aOffset;
      //
      aOffset      = aCF2D->Offset();
      myOffsetBase = myOffsetBase + aOffset;
      myOffsetFlag = true;
      //
      aC2DB = aCF2D->BasisCurve();
      ++iOffset;
    }
    //
    if (!(iTrimmed || iOffset))
    {
      break;
    }
    //
    bIsTypeBase = IsTypeBase(aC2DB, aTypeB);
    if (bIsTypeBase)
    {
      myTypeBase  = aTypeB;
      myCurveBase = aC2DB;
      return;
    }
  }
  //
  myErrorStatus = 11; // unknown type base
}

//=================================================================================================

bool BndLib_Box2dCurve::IsTypeBase(const occ::handle<Geom2d_Curve>& aC2D, GeomAbs_CurveType& aTypeB)
{
  bool                       bRet;
  occ::handle<Standard_Type> aType;
  //
  bRet = true;
  //
  aType = aC2D->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    aTypeB = GeomAbs_Line;
  }
  else if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    aTypeB = GeomAbs_Circle;
  }
  else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    aTypeB = GeomAbs_Ellipse;
  }
  else if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    aTypeB = GeomAbs_Parabola;
  }
  else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    aTypeB = GeomAbs_Hyperbola;
  }
  else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    aTypeB = GeomAbs_BezierCurve;
  }
  else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    aTypeB = GeomAbs_BSplineCurve;
  }
  else
  {
    aTypeB = GeomAbs_OtherCurve;
    bRet   = !bRet;
  }
  return bRet;
}

//=================================================================================================

void BndLib_Box2dCurve::PerformLineConic()
{
  int      i, iInf[2];
  double   aTb[2];
  gp_Pnt2d aP2D;
  //
  myErrorStatus = 0;
  //
  Bnd_Box2d& aBox2D = myBox;
  //
  iInf[0] = 0;
  iInf[1] = 0;
  aTb[0]  = myT1;
  aTb[1]  = myT2;
  //
  for (i = 0; i < 2; ++i)
  {
    if (Precision::IsNegativeInfinite(aTb[i]))
    {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
      ++iInf[0];
    }
    else if (Precision::IsPositiveInfinite(aTb[i]))
    {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
      ++iInf[1];
    }
    else
    {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
    }
  }
  //
  if (myTypeBase == GeomAbs_Line)
  {
    return;
  }
  //
  if (iInf[0] && iInf[1])
  {
    return;
  }
  //-------------
  occ::handle<Geom2d_Conic> aConic2D;
  //
  aConic2D = occ::down_cast<Geom2d_Conic>(myCurveBase);
  Compute(aConic2D, myTypeBase, aTb[0], aTb[1], aBox2D);
}

//=================================================================================================

void BndLib_Box2dCurve::Compute(const occ::handle<Geom2d_Conic>& aConic2D,
                                const GeomAbs_CurveType          aType,
                                const double                     aT1,
                                const double                     aT2,
                                Bnd_Box2d&                       aBox2D)
{
  int      i, aNbT;
  double   pT[10], aT, aTwoPI, dT, aEps;
  gp_Pnt2d aP2D;
  //
  aNbT = Compute(aConic2D, aType, pT);
  //
  if (aType == GeomAbs_Parabola || aType == GeomAbs_Hyperbola)
  {
    for (i = 0; i < aNbT; ++i)
    {
      aT = pT[i];
      if (aT > aT1 && aT < aT2)
      {
        D0(aT, aP2D);
        aBox2D.Add(aP2D);
      }
    }
    return;
  }
  //
  // aType==GeomAbs_Circle ||  aType==GeomAbs_Ellipse
  aEps   = Precision::Angular();
  aTwoPI = 2. * M_PI;
  dT     = aT2 - aT1;
  //
  double aT1z = AdjustToPeriod(aT1, aTwoPI);
  if (std::abs(aT1z) < aEps)
  {
    aT1z = 0.;
  }
  //
  double aT2z = aT1z + dT;
  if (std::abs(aT2z - aTwoPI) < aEps)
  {
    aT2z = aTwoPI;
  }
  //
  for (i = 0; i < aNbT; ++i)
  {
    aT = pT[i];
    // adjust aT to range [aT1z, aT1z + 2*PI]; note that pT[i] and aT1z
    // are adjusted to range [0, 2*PI], but aT2z can be greater than 2*PI
    aT = (aT < aT1z ? aT + aTwoPI : aT);
    if (aT <= aT2z)
    {
      D0(aT, aP2D);
      aBox2D.Add(aP2D);
    }
  }
}

//=================================================================================================

int BndLib_Box2dCurve::Compute(const occ::handle<Geom2d_Conic>& aConic2D,
                               const GeomAbs_CurveType          aType,
                               double*                          pT)
{
  int    iRet, i, j;
  double aCosBt, aSinBt, aCosGm, aSinGm;
  double aLx, aLy;
  //
  iRet = 0;
  //
  const gp_Ax22d& aPos  = aConic2D->Position();
  const gp_XY&    aXDir = aPos.XDirection().XY();
  const gp_XY&    aYDir = aPos.YDirection().XY();
  //
  aCosBt = aXDir.X();
  aSinBt = aXDir.Y();
  aCosGm = aYDir.X();
  aSinGm = aYDir.Y();
  //
  if (aType == GeomAbs_Circle || aType == GeomAbs_Ellipse)
  {
    double aR1 = 0.0, aR2 = 0.0, aTwoPI = M_PI + M_PI;
    double aA11 = 0.0, aA12 = 0.0, aA21 = 0.0, aA22 = 0.0;
    double aBx = 0.0, aBy = 0.0, aB = 0.0, aCosFi = 0.0, aSinFi = 0.0, aFi = 0.0;
    //
    if (aType == GeomAbs_Ellipse)
    {
      occ::handle<Geom2d_Ellipse> aEL2D;
      //
      aEL2D = occ::down_cast<Geom2d_Ellipse>(aConic2D);
      aR1   = aEL2D->MajorRadius();
      aR2   = aEL2D->MinorRadius();
    }
    else if (aType == GeomAbs_Circle)
    {
      occ::handle<Geom2d_Circle> aCR2D;
      //
      aCR2D = occ::down_cast<Geom2d_Circle>(aConic2D);
      aR1   = aCR2D->Radius();
      aR2   = aR1;
    }
    //
    aA11 = -aR1 * aCosBt;
    aA12 = aR2 * aCosGm;
    aA21 = -aR1 * aSinBt;
    aA22 = aR2 * aSinGm;
    //
    for (i = 0; i < 2; ++i)
    {
      aLx = (!i) ? 0. : 1.;
      aLy = (!i) ? 1. : 0.;
      aBx = aLx * aA21 - aLy * aA11;
      aBy = aLx * aA22 - aLy * aA12;
      aB  = std::sqrt(aBx * aBx + aBy * aBy);
      //
      aCosFi = aBx / aB;
      aSinFi = aBy / aB;
      //
      aFi = std::acos(aCosFi);
      if (aSinFi < 0.)
      {
        aFi = aTwoPI - aFi;
      }
      //
      j     = 2 * i;
      pT[j] = aTwoPI - aFi;
      pT[j] = AdjustToPeriod(pT[j], aTwoPI);
      //
      pT[j + 1] = M_PI - aFi;
      pT[j + 1] = AdjustToPeriod(pT[j + 1], aTwoPI);
    }
    iRet = 4;
  } // if (aType==GeomAbs_Ellipse) {
  //
  else if (aType == GeomAbs_Parabola)
  {
    double                       aFc, aEps;
    double                       aA1, aA2;
    occ::handle<Geom2d_Parabola> aPR2D;
    //
    aEps = Precision::Angular();
    //
    aPR2D = occ::down_cast<Geom2d_Parabola>(aConic2D);
    aFc   = aPR2D->Focal();
    //
    j = 0;
    for (i = 0; i < 2; i++)
    {
      aLx = (!i) ? 0. : 1.;
      aLy = (!i) ? 1. : 0.;
      //
      aA2 = aLx * aSinBt - aLy * aCosBt;
      if (std::abs(aA2) < aEps)
      {
        continue;
      }
      //
      aA1 = aLy * aCosGm - aLx * aSinGm;
      //
      pT[j] = 2. * aFc * aA1 / aA2;
      ++j;
    }
    iRet = j;
  } // else if (aType==GeomAbs_Parabola) {
  //
  else if (aType == GeomAbs_Hyperbola)
  {
    int                           k;
    double                        aR1, aR2;
    double                        aEps, aB1, aB2, aB12, aB22, aZ, aD;
    occ::handle<Geom2d_Hyperbola> aHP2D;
    //
    aEps = Precision::Angular();
    //
    aHP2D = occ::down_cast<Geom2d_Hyperbola>(aConic2D);
    aR1   = aHP2D->MajorRadius();
    aR2   = aHP2D->MinorRadius();
    //
    j = 0;
    for (i = 0; i < 2; i++)
    {
      aLx = (!i) ? 0. : 1.;
      aLy = (!i) ? 1. : 0.;
      //
      aB1 = aR1 * (aLx * aSinBt - aLy * aCosBt);
      aB2 = aR2 * (aLx * aSinGm - aLy * aCosGm);
      //
      if (std::abs(aB1) < aEps)
      {
        continue;
      }
      //
      if (std::abs(aB2) < aEps)
      {
        pT[j] = 0.;
        ++j;
      }
      else
      {
        aB12 = aB1 * aB1;
        aB22 = aB2 * aB2;
        if (!(aB12 > aB22))
        {
          continue;
        }
        //
        aD = std::sqrt(aB12 - aB22);
        //-------------
        for (k = -1; k < 2; k += 2)
        {
          aZ = (aB1 + k * aD) / aB2;
          if (std::abs(aZ) < 1.)
          {
            pT[j] = -std::log((1. + aZ) / (1. - aZ));
            ++j;
          }
        }
      }
    }
    iRet = j;
  } // else if (aType==GeomAbs_Hyperbola) {
  //
  return iRet;
}

//=================================================================================================

double BndLib_Box2dCurve::AdjustToPeriod(const double aT, const double aPeriod)
{
  int    k;
  double aTRet;
  //
  aTRet = aT;
  if (aT < 0.)
  {
    k     = 1 + static_cast<int>(-aT / aPeriod);
    aTRet = aT + k * aPeriod;
  }
  else if (aT > aPeriod)
  {
    k     = static_cast<int>(aT / aPeriod);
    aTRet = aT - k * aPeriod;
  }
  if (aTRet == aPeriod)
  {
    aTRet = 0.;
  }
  return aTRet;
}

//
// myErrorStatus:
//
// -1 - object is just initialized
// 10 - myCurve is Null
// 12 - invalid range myT1 >  myT2l
// 11 - unknown type of base curve
// 13 - offset curve can not be computed
// NMTTest

//=================================================================================================

void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC, const double aTol, Bnd_Box2d& aBox2D)
{
  BndLib_Add2dCurve::Add(aC, aC.FirstParameter(), aC.LastParameter(), aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC,
                            const double             aU1,
                            const double             aU2,
                            const double             aTol,
                            Bnd_Box2d&               aBox2D)
{
  const Geom2dAdaptor_Curve* pA = dynamic_cast<const Geom2dAdaptor_Curve*>(&aC);
  if (!pA)
  {
    double   U, DU;
    int      N, j;
    gp_Pnt2d P;
    N  = 33;
    U  = aU1;
    DU = (aU2 - aU1) / (N - 1);
    for (j = 1; j < N; j++)
    {
      aC.D0(U, P);
      U += DU;
      aBox2D.Add(P);
    }
    aC.D0(aU2, P);
    aBox2D.Add(P);
    aBox2D.Enlarge(aTol);
    return;
  }
  //
  const occ::handle<Geom2d_Curve>& aC2D = pA->Curve();
  //
  BndLib_Add2dCurve::Add(aC2D, aU1, aU2, aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const occ::handle<Geom2d_Curve>& aC2D,
                            const double                     aTol,
                            Bnd_Box2d&                       aBox2D)
{
  double aT1, aT2;
  //
  aT1 = aC2D->FirstParameter();
  aT2 = aC2D->LastParameter();
  //
  BndLib_Add2dCurve::Add(aC2D, aT1, aT2, aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const occ::handle<Geom2d_Curve>& aC2D,
                            const double                     aT1,
                            const double                     aT2,
                            const double                     aTol,
                            Bnd_Box2d&                       aBox2D)
{
  BndLib_Box2dCurve aBC;
  //
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  //
  aBC.Perform();
  //
  const Bnd_Box2d& aBoxC = aBC.Box();
  aBox2D.Add(aBoxC);
  aBox2D.Enlarge(aTol);
}

//=================================================================================================

void BndLib_Add2dCurve::AddOptimal(const occ::handle<Geom2d_Curve>& aC2D,
                                   const double                     aT1,
                                   const double                     aT2,
                                   const double                     aTol,
                                   Bnd_Box2d&                       aBox2D)
{
  BndLib_Box2dCurve aBC;
  //
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  //
  aBC.PerformOptimal(aTol);
  //
  const Bnd_Box2d& aBoxC = aBC.Box();
  aBox2D.Add(aBoxC);
  aBox2D.Enlarge(aTol);
}