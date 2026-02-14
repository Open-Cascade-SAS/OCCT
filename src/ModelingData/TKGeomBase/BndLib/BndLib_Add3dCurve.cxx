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

#include <Bnd_Box.hxx>
#include <BndLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <MathOpt_Brent.hxx>
#include <MathOpt_PSO.hxx>
#include <MathPoly_Quartic.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <PLib.hxx>
#include <Precision.hxx>

/// Maximum bisection iterations for derivative root isolation.
/// 53 iterations guarantee full double precision (~2^-53 relative interval).
constexpr int THE_MAX_BISECTION_ITER = 53;

/// Number of coordinates in homogeneous 3D cache rows (X, Y, Z, W).
constexpr int THE_CACHE_ROW_LEN = 4;

/// Stack buffer size for Taylor polynomial coefficients.
constexpr int THE_CACHE_BUF_SIZE = (BSplCLib::MaxDegree() + 1) * THE_CACHE_ROW_LEN;

/// Maximum curve degree for which direct polynomial root-finding is used.
/// Derivative polynomial degree is (curve degree - 1), and MathPoly::Quartic handles up to
/// degree 4.
constexpr int THE_MAX_DIRECT_ROOT_DEGREE = 5;

//
static int NbSamples(const Adaptor3d_Curve& C, const double Umin, const double Umax);

static double AdjustExtr(const Adaptor3d_Curve& C,
                         const double           UMin,
                         const double           UMax,
                         const double           Extr0,
                         const int              CoordIndx,
                         const double           Tol,
                         const bool             IsMin);

/// Evaluates derivative (or rational derivative numerator) for a single coordinate
/// from cached Taylor polynomial coefficients.
/// @param theCacheBuf  buffer with Taylor polynomial coefficients
/// @param theRowLen    number of columns per row (2D/3D, +-weight)
/// @param theDegree    polynomial degree
/// @param theCoord     coordinate index (0=X, 1=Y, 2=Z)
/// @param theIsRational true for rational curves (NURBS)
/// @param theT         local parameter value in [0, 1]
/// @return for non-rational: f'_c(t); for rational: d(xw)/dt * w - xw * dw/dt
static double evaluateDerivCoord(const double* theCacheBuf,
                                 const int     theRowLen,
                                 const int     theDegree,
                                 const int     theCoord,
                                 const bool    theIsRational,
                                 const double  theT)
{
  if (!theIsRational)
  {
    // Non-rational: f'(t) = sum((i+1) * c[(i+1)*rowLen + coord] * t^i, i=0..deg-1)
    double aResult = 0.0;
    double aPow    = 1.0;
    for (int i = 0; i < theDegree; ++i)
    {
      aResult += (i + 1) * theCacheBuf[(i + 1) * theRowLen + theCoord] * aPow;
      aPow *= theT;
    }
    return aResult;
  }

  // Rational case: numerator of dx/dt = d(xw)/dt * w - xw * dw/dt
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

/// Finds coordinate extrema of a non-rational polynomial span using direct root-finding.
/// Supports derivative polynomial degree 1-4 (curve degree 2-5).
/// @param theCacheBuf  buffer with Taylor polynomial coefficients
/// @param theRowLen    number of columns per row
/// @param theDegree    polynomial degree of the curve span
/// @param theTMin      lower bound of local parameter range
/// @param theTMax      upper bound of local parameter range
/// @param theBox       bounding box to extend with extremal points
static void addPolySpanExtrema3d(const double* theCacheBuf,
                                 const int     theRowLen,
                                 const int     theDegree,
                                 const double  theTMin,
                                 const double  theTMax,
                                 Bnd_Box&      theBox)
{
  if (theDegree < 2)
  {
    return;
  }

  const int aDerivDeg = theDegree - 1;

  for (int aCoord = 0; aCoord < 3; ++aCoord)
  {
    // Build derivative polynomial coefficients in ascending order:
    // f'(t) = d[0] + d[1]*t + ... + d[aDerivDeg]*t^aDerivDeg
    // where d[i] = (i+1) * c[(i+1)*rowLen + coord]
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

    // Filter roots to (theTMin, theTMax) and evaluate the polynomial at each root
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
        theBox.Add(gp_Pnt(aPoint[0], aPoint[1], aPoint[2]));
      }
    }
  }
}

/// Fallback extrema finder for rational or high-degree spans.
/// Uses sign-change detection on sampled derivative values followed by
/// bisection refinement to isolate each root.
/// @param theCacheBuf   buffer with Taylor polynomial coefficients
/// @param theRowLen     number of columns per row
/// @param theDegree     polynomial degree of the curve span
/// @param theIsRational true for rational curves
/// @param theTMin       lower bound of local parameter range
/// @param theTMax       upper bound of local parameter range
/// @param theBox        bounding box to extend with extremal points
static void addSpanExtremaByBisection3d(const double* theCacheBuf,
                                        const int     theRowLen,
                                        const int     theDegree,
                                        const bool    theIsRational,
                                        const double  theTMin,
                                        const double  theTMax,
                                        Bnd_Box&      theBox)
{
  if (theDegree < 2)
  {
    return;
  }

  const int    aNbSamples = std::max(4 * theDegree, 8);
  const double aDt        = (theTMax - theTMin) / aNbSamples;

  for (int aCoord = 0; aCoord < 3; ++aCoord)
  {
    double aPrevDeriv =
      evaluateDerivCoord(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, theTMin);

    for (int i = 1; i <= aNbSamples; ++i)
    {
      const double aT = theTMin + i * aDt;
      const double aCurrDeriv =
        evaluateDerivCoord(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, aT);

      if (aPrevDeriv * aCurrDeriv < 0.0)
      {
        // Sign change detected - bisect to find root
        double aTLow  = theTMin + (i - 1) * aDt;
        double aTHigh = aT;
        double aDLow  = aPrevDeriv;

        for (int anIter = 0; anIter < THE_MAX_BISECTION_ITER; ++anIter)
        {
          const double aTMid = (aTLow + aTHigh) * 0.5;
          const double aDMid =
            evaluateDerivCoord(theCacheBuf, theRowLen, theDegree, aCoord, theIsRational, aTMid);

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

        // Evaluate point at root from cached coefficients
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
          theBox.Add(gp_Pnt(aPoint[0] / aW, aPoint[1] / aW, aPoint[2] / aW));
        }
        else
        {
          theBox.Add(gp_Pnt(aPoint[0], aPoint[1], aPoint[2]));
        }
      }

      aPrevDeriv = aCurrDeriv;
    }
  }
}

/// Computes exact bounding box for a 3D BSpline curve over its full parameter range.
/// Uses per-span polynomial root-finding to locate coordinate extrema.
/// @param theCurve  BSpline curve
/// @param theTol    tolerance to enlarge the resulting box
/// @param theBox    bounding box to extend
static void addBSplineBBox3d(const occ::handle<Geom_BSplineCurve>& theCurve,
                             const double                          theTol,
                             Bnd_Box&                              theBox)
{
  const int  aDegree      = theCurve->Degree();
  const bool anIsRational = theCurve->IsRational();
  const bool anIsPeriodic = theCurve->IsPeriodic();

  const NCollection_Array1<gp_Pnt>& aPoles     = theCurve->Poles();
  const NCollection_Array1<double>* aWeights   = theCurve->Weights();
  const NCollection_Array1<double>& aFlatKnots = theCurve->KnotSequence();
  const NCollection_Array1<double>& aKnots     = theCurve->Knots();
  const int                         aRowLen    = anIsRational ? 4 : 3;

  // Add all distinct knot points (these include the endpoints)
  for (int i = aKnots.Lower(); i <= aKnots.Upper(); ++i)
  {
    theBox.Add(theCurve->Value(aKnots(i)));
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

    // Build polynomial cache for this span
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

    // Use direct polynomial root-finding for non-rational curves with degree 2..5,
    // otherwise fall back to bisection.
    if (!anIsRational && aDegree <= THE_MAX_DIRECT_ROOT_DEGREE)
    {
      addPolySpanExtrema3d(aCacheBuf, aRowLen, aDegree, 0.0, 1.0, theBox);
    }
    else
    {
      addSpanExtremaByBisection3d(aCacheBuf, aRowLen, aDegree, anIsRational, 0.0, 1.0, theBox);
    }
  }

  theBox.Enlarge(theTol);
}

/// Computes exact bounding box for a 3D Bezier curve over a given parameter range.
/// @param theCurve  Bezier curve
/// @param theU1     start of parameter range
/// @param theU2     end of parameter range
/// @param theTol    tolerance to enlarge the resulting box
/// @param theBox    bounding box to extend
static void addBezierBBox3d(const occ::handle<Geom_BezierCurve>& theCurve,
                            const double                         theU1,
                            const double                         theU2,
                            const double                         theTol,
                            Bnd_Box&                             theBox)
{
  const int  aDegree      = theCurve->Degree();
  const bool anIsRational = theCurve->IsRational();
  const int  aRowLen      = anIsRational ? 4 : 3;

  const NCollection_Array1<gp_Pnt>& aPoles     = theCurve->Poles();
  const NCollection_Array1<double>* aWeights   = theCurve->Weights();
  const NCollection_Array1<double>& aFlatKnots = theCurve->KnotSequence();

  // Add endpoints
  theBox.Add(theCurve->Value(theU1));
  theBox.Add(theCurve->Value(theU2));

  if (aDegree < 2)
  {
    theBox.Enlarge(theTol);
    return;
  }

  // Build polynomial cache for the single Bezier span
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

  // Compute local parameter range within the Bezier span
  const double aTMin = std::max(0.0, (theU1 - aSpanStart) / aSpanLength);
  const double aTMax = std::min(1.0, (theU2 - aSpanStart) / aSpanLength);

  if (!anIsRational && aDegree <= THE_MAX_DIRECT_ROOT_DEGREE)
  {
    addPolySpanExtrema3d(aCacheBuf, aRowLen, aDegree, aTMin, aTMax, theBox);
  }
  else
  {
    addSpanExtremaByBisection3d(aCacheBuf, aRowLen, aDegree, anIsRational, aTMin, aTMax, theBox);
  }

  theBox.Enlarge(theTol);
}

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::Add(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

// OCC566(apo)->
static double FillBox(Bnd_Box&               B,
                      const Adaptor3d_Curve& C,
                      const double           first,
                      const double           last,
                      const int              N)
{
  gp_Pnt P1, P2, P3;
  C.D0(first, P1);
  B.Add(P1);
  double p = first, dp = last - first, tol = 0.;
  if (std::abs(dp) > Precision::PConfusion())
  {
    int i;
    dp /= 2 * N;
    for (i = 1; i <= N; i++)
    {
      p += dp;
      C.D0(p, P2);
      B.Add(P2);
      p += dp;
      C.D0(p, P3);
      B.Add(P3);
      gp_Pnt Pc((P1.XYZ() + P3.XYZ()) / 2.0);
      tol = std::max(tol, Pc.Distance(P2));
      P1  = P3;
    }
  }
  else
  {
    C.D0(first, P1);
    B.Add(P1);
    C.D0(last, P3);
    B.Add(P3);
    tol = 0.;
  }
  return tol;
}

//<-OCC566(apo)
//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C,
                            const double           U1,
                            const double           U2,
                            const double           Tol,
                            Bnd_Box&               B)
{
  constexpr double weakness = 1.5; // OCC566(apo)
  double           tol      = 0.0;
  switch (C.GetType())
  {

    case GeomAbs_Line: {
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Circle: {
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Ellipse: {
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Hyperbola: {
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Parabola: {
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_BezierCurve: {
      addBezierBBox3d(C.Bezier(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_BSplineCurve: {
      occ::handle<Geom_BSplineCurve> Bs = C.BSpline();
      if (std::abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)
          || std::abs(Bs->LastParameter() - U2) > Precision::Parametric(Tol))
      {
        occ::handle<Geom_Geometry>     G = Bs->Copy();
        occ::handle<Geom_BSplineCurve> Bsaux(occ::down_cast<Geom_BSplineCurve>(G));
        double                         u1 = U1, u2 = U2;
        if (Bsaux->IsPeriodic())
          ElCLib::AdjustPeriodic(Bsaux->FirstParameter(),
                                 Bsaux->LastParameter(),
                                 Precision::PConfusion(),
                                 u1,
                                 u2);
        else
        {
          if (Bsaux->FirstParameter() > U1)
            u1 = Bsaux->FirstParameter();
          if (Bsaux->LastParameter() < U2)
            u2 = Bsaux->LastParameter();
        }
        double aSegmentTol = 2. * Precision::PConfusion();

        if (Bsaux->IsPeriodic())
        {
          const double aPeriod           = Bsaux->LastParameter() - Bsaux->FirstParameter();
          const double aDirectDiff       = std::abs(u2 - u1);
          const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
          const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);
          const double aMinDiff =
            std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));

          if (aMinDiff < aSegmentTol)
          {
            aSegmentTol = aMinDiff * 0.01;
          }
        }
        else if (std::abs(u2 - u1) < aSegmentTol)
        {
          aSegmentTol = std::abs(u2 - u1) * 0.01;
        }
        Bsaux->Segment(u1, u2, aSegmentTol);
        Bs = Bsaux;
      }
      addBSplineBBox3d(Bs, Tol, B);
      break;
    }
    default: {
      Bnd_Box       B1;
      constexpr int N = 33;
      tol             = FillBox(B1, C, U1, U2, N);
      B1.Enlarge(weakness * tol);
      double x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
    }
  }
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::AddOptimal(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C,
                                   const double           U1,
                                   const double           U2,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  switch (C.GetType())
  {

    case GeomAbs_Line: {
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Circle: {
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Ellipse: {
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Hyperbola: {
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Parabola: {
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_BezierCurve: {
      addBezierBBox3d(C.Bezier(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_BSplineCurve: {
      occ::handle<Geom_BSplineCurve> Bs = C.BSpline();
      if (std::abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)
          || std::abs(Bs->LastParameter() - U2) > Precision::Parametric(Tol))
      {
        occ::handle<Geom_Geometry>     G = Bs->Copy();
        occ::handle<Geom_BSplineCurve> Bsaux(occ::down_cast<Geom_BSplineCurve>(G));
        double                         u1 = U1, u2 = U2;
        if (Bsaux->IsPeriodic())
          ElCLib::AdjustPeriodic(Bsaux->FirstParameter(),
                                 Bsaux->LastParameter(),
                                 Precision::PConfusion(),
                                 u1,
                                 u2);
        else
        {
          if (Bsaux->FirstParameter() > U1)
            u1 = Bsaux->FirstParameter();
          if (Bsaux->LastParameter() < U2)
            u2 = Bsaux->LastParameter();
        }
        double aSegmentTol = 2. * Precision::PConfusion();

        if (Bsaux->IsPeriodic())
        {
          const double aPeriod           = Bsaux->LastParameter() - Bsaux->FirstParameter();
          const double aDirectDiff       = std::abs(u2 - u1);
          const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
          const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);
          const double aMinDiff =
            std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));

          if (aMinDiff < aSegmentTol)
          {
            aSegmentTol = aMinDiff * 0.01;
          }
        }
        else if (std::abs(u2 - u1) < aSegmentTol)
        {
          aSegmentTol = std::abs(u2 - u1) * 0.01;
        }
        Bsaux->Segment(u1, u2, aSegmentTol);
        Bs = Bsaux;
      }
      addBSplineBBox3d(Bs, Tol, B);
      break;
    }
    default: {
      AddGenCurv(C, U1, U2, Tol, B);
    }
  }
}

//=================================================================================================

void BndLib_Add3dCurve::AddGenCurv(const Adaptor3d_Curve& C,
                                   const double           UMin,
                                   const double           UMax,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  int Nu = NbSamples(C, UMin, UMax);
  //
  double CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};
  //
  gp_Pnt                     P;
  int                        i, k;
  double                     du = (UMax - UMin) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, Nu);
  double                     u;
  for (i = 1, u = UMin; i <= Nu; i++, u += du)
  {
    C.D0(u, P);
    aPnts(i) = P.XYZ();
    //
    for (k = 0; k < 3; ++k)
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
      gp_XYZ aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      C.D0(u - du2, P);
      gp_XYZ aD = (P.XYZ() - aPm);
      for (k = 0; k < 3; ++k)
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
  double eps = std::max(Tol, Precision::Confusion());
  for (k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
    {
      continue;
    }
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double umin, umax;
        umin        = UMin + std::max(0, i - 2) * du;
        umax        = UMin + std::min(Nu - 1, i) * du;
        double cmin = AdjustExtr(C, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin, umax;
        umin        = UMin + std::max(0, i - 2) * du;
        umax        = UMin + std::min(Nu - 1, i) * du;
        double cmax = AdjustExtr(C, umin, umax, CMax, k + 1, eps, false);
        if (cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  B.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  B.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  B.Enlarge(eps);
}

//
class CurvMaxMinCoordMVar
{
public:
  CurvMaxMinCoordMVar(const Adaptor3d_Curve& theCurve,
                      const double           UMin,
                      const double           UMax,
                      const int              CoordIndx,
                      const double           Sign)
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
    gp_Pnt aP = myCurve.Value(X(1));

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

private:
  CurvMaxMinCoordMVar& operator=(const CurvMaxMinCoordMVar&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const Adaptor3d_Curve& myCurve;
  double                 myUMin;
  double                 myUMax;
  int                    myCoordIndx;
  double                 mySign;
};

//
class CurvMaxMinCoord
{
public:
  CurvMaxMinCoord(const Adaptor3d_Curve& theCurve,
                  const double           UMin,
                  const double           UMax,
                  const int              CoordIndx,
                  const double           Sign)
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
    gp_Pnt aP = myCurve.Value(X);

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

private:
  CurvMaxMinCoord& operator=(const CurvMaxMinCoord&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const Adaptor3d_Curve& myCurve;
  double                 myUMin;
  double                 myUMax;
  int                    myCoordIndx;
  double                 mySign;
};

//=================================================================================================

double AdjustExtr(const Adaptor3d_Curve& C,
                  const double           UMin,
                  const double           UMax,
                  const double           Extr0,
                  const int              CoordIndx,
                  const double           Tol,
                  const bool             IsMin)
{
  double aSign = IsMin ? 1. : -1.;
  double extr  = aSign * Extr0;
  //
  double uTol = std::max(C.Resolution(Tol), Precision::PConfusion());
  double Du   = (C.LastParameter() - C.FirstParameter());
  //
  double reltol = uTol / std::max(std::abs(UMin), std::abs(UMax));
  if (UMax - UMin < 0.01 * Du)
  {
    MathUtils::Config       aBrentConfig(reltol, 100);
    CurvMaxMinCoord         aFunc(C, UMin, UMax, CoordIndx, aSign);
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

  CurvMaxMinCoordMVar     aFunc(C, UMin, UMax, CoordIndx, aSign);
  MathOpt::PSOConfig      aPSOConfig(aNbParticles, 100);
  MathUtils::VectorResult aPSOResult = MathOpt::PSO(aFunc, aLowBorder, aUppBorder, aPSOConfig);
  //
  if (aPSOResult.IsDone())
  {
    const double aStep  = std::min(0.1 * Du, aMaxStep);
    const double aBestT = (*aPSOResult.Solution)(1);

    MathUtils::Config       aBrentConfig2(reltol, 100);
    CurvMaxMinCoord         aFunc1(C, UMin, UMax, CoordIndx, aSign);
    MathUtils::ScalarResult aBrentResult = MathOpt::Brent(aFunc1,
                                                          std::max(aBestT - aStep, UMin),
                                                          std::min(aBestT + aStep, UMax),
                                                          aBrentConfig2);

    if (aBrentResult.IsDone())
    {
      return aSign * (*aBrentResult.Value);
    }
    return aSign * (*aPSOResult.Value);
  }

  return aSign * extr;
}

//=================================================================================================

int NbSamples(const Adaptor3d_Curve& C, const double Umin, const double Umax)
{
  int               N;
  GeomAbs_CurveType Type = C.GetType();
  switch (Type)
  {
    case GeomAbs_BezierCurve: {
      N = 2 * C.NbPoles();
      // By default parametric range of Bezier curv is [0, 1]
      double du = Umax - Umin;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineCurve: {
      const occ::handle<Geom_BSplineCurve>& BC = C.BSpline();
      N                                        = 2 * (BC->Degree() + 1) * (BC->NbKnots() - 1);
      double umin = BC->FirstParameter(), umax = BC->LastParameter();
      double du = (Umax - Umin) / (umax - umin);
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 33;
  }
  return std::min(500, N);
}
