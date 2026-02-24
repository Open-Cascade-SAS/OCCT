// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomProp_BezierCurve.hxx>

#include <gp.hxx>
#include <MathRoot_Brent.hxx>
#include <MathRoot_Multiple.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

constexpr double THE_CURVATURE_DERIV_COEFF  = 3.0;   //!< Coefficient in d(KC)/dU formula
constexpr double THE_DIFF_STEP_DIVISOR      = 100.0; //!< Divisor for numerical differentiation step
constexpr double THE_D2_MAGNITUDE_THRESHOLD = 1.0e-4; //!< Threshold for second derivative magnitude
constexpr double THE_EPSILON_SCALE      = 1.0e-4; //!< Scale factor for epsilon relative to domain
constexpr int    THE_EXTREMA_NB_SAMPLES = 100;   //!< Number of samples for curvature extrema search
constexpr int    THE_INFLECTION_NB_SAMPLES = 30; //!< Number of samples for inflection search
constexpr double THE_INFLECTION_TOLERANCE  = 1.0e-6; //!< Tolerance for inflection point finding

//! Function for finding curvature extrema: F = d(curvature)/dU = 0
//! In 3D: KC = |V1 x V2| / ||V1||^3
//! F = d KC / dU
class FuncCurExt
{
public:
  FuncCurExt(const GeomAdaptor_Curve* theCurve, const double theTol)
      : myCurve(theCurve),
        myEpsX(theTol)
  {
  }

  bool Value(const double X, double& F)
  {
    gp_Pnt aP;
    gp_Vec aV1, aV2, aV3;
    myCurve->D3(X, aP, aV1, aV2, aV3);

    // In 3D: |V1 x V2| = magnitude of cross product vector
    const gp_Vec aCPV1V2 = aV1.Crossed(aV2);
    const double aCPMag  = aCPV1V2.Magnitude();
    const gp_Vec aCPV1V3 = aV1.Crossed(aV3);
    const double aV1V2   = aV1.Dot(aV2);
    const double aV1V1   = aV1.SquareMagnitude();
    const double aNV1    = std::sqrt(aV1V1);
    const double aV13    = aV1V1 * aNV1;
    const double aV15    = aV13 * aV1V1;

    if (aV15 < gp::Resolution())
    {
      return false;
    }

    // d(KC)/dU for 3D curves.
    // KC = |V1 x V2| / |V1|^3
    // dKC/dU = d|V1xV2|/dU / |V1|^3 - 3 * |V1xV2| * (V1.V2) / |V1|^5
    // d|V1xV2|/dU = (V1xV2).(V1xV3) / |V1xV2|
    if (aCPMag < gp::Resolution())
    {
      // Cross product is zero - inflection or straight region.
      // Use simplified formula.
      F = aCPV1V3.Magnitude() / aV13;
      return true;
    }
    const double aDCrossDU = aCPV1V2.Dot(aCPV1V3) / aCPMag;
    F = aDCrossDU / aV13 - THE_CURVATURE_DERIV_COEFF * aCPMag * aV1V2 / aV15;
    return true;
  }

  bool Values(const double X, double& F, double& D)
  {
    double aDx = myEpsX / THE_DIFF_STEP_DIVISOR;
    if (X + aDx > myCurve->LastParameter())
    {
      aDx = -aDx;
    }

    Value(X, F);
    double aF2;
    Value(X + aDx, aF2);
    D = (aF2 - F) / aDx;
    return true;
  }

  //! Test if parameter corresponds to a minimum of the radius of curvature
  //! (maximum of |curvature|) by comparison with a neighboring point.
  bool IsMinKC(const double X) const
  {
    gp_Pnt aP;
    gp_Vec aV1, aV2, aV3;

    myCurve->D3(X, aP, aV1, aV2, aV3);
    const double aCPMag = aV1.Crossed(aV2).Magnitude();
    const double aV1V1  = aV1.SquareMagnitude();
    const double aNV1   = std::sqrt(aV1V1);
    const double aV13   = aV1V1 * aNV1;

    if (aV13 < gp::Resolution())
    {
      return false;
    }
    const double aKC = aCPMag / aV13;

    double aDx = myEpsX;
    if (X + aDx > myCurve->LastParameter())
    {
      aDx = -aDx;
    }

    myCurve->D3(X + aDx, aP, aV1, aV2, aV3);
    const double aCPMagN = aV1.Crossed(aV2).Magnitude();
    const double aV1V1n  = aV1.SquareMagnitude();
    const double aNV1n   = std::sqrt(aV1V1n);
    const double aV13n   = aV1V1n * aNV1n;

    if (aV13n < gp::Resolution())
    {
      return false;
    }
    const double aKP = aCPMagN / aV13n;

    return std::abs(aKC) > std::abs(aKP);
  }

private:
  const GeomAdaptor_Curve* myCurve;
  double                   myEpsX;
};

//! Function for finding inflection points: F = |V1 x V2| / (||V1|| * ||V2||) = 0
class FuncCurNul
{
public:
  FuncCurNul(const GeomAdaptor_Curve* theCurve)
      : myCurve(theCurve)
  {
  }

  bool Value(const double X, double& F)
  {
    double aD;
    return Values(X, F, aD);
  }

  bool Values(const double X, double& F, double& D)
  {
    gp_Pnt aP;
    gp_Vec aV1, aV2, aV3;
    myCurve->D3(X, aP, aV1, aV2, aV3);

    const gp_Vec aCPV1V2 = aV1.Crossed(aV2);
    const double aCPMag  = aCPV1V2.Magnitude();
    const gp_Vec aCPV1V3 = aV1.Crossed(aV3);
    const double aV1V2   = aV1.Dot(aV2);
    const double aV2V3   = aV2.Dot(aV3);
    const double aNV1    = aV1.Magnitude();
    const double aNV2    = aV2.Magnitude();

    F = 0.0;
    D = 0.0;

    if (aNV2 < THE_D2_MAGNITUDE_THRESHOLD)
    {
      return true;
    }
    if (aNV1 * aNV2 < gp::Resolution())
    {
      return false;
    }

    F = aCPMag / (aNV1 * aNV2);

    // Derivative of |V1xV2|/(|V1|*|V2|) w.r.t. parameter
    if (aCPMag < gp::Resolution())
    {
      D = aCPV1V3.Magnitude() / (aNV1 * aNV2);
    }
    else
    {
      const double aDCrossDU = aCPV1V2.Dot(aCPV1V3) / aCPMag;
      D = (aDCrossDU - aCPMag * aV1V2 / (aNV1 * aNV1) - aCPMag * aV2V3 / (aNV2 * aNV2))
          / (aNV1 * aNV2);
    }
    return true;
  }

private:
  const GeomAdaptor_Curve* myCurve;
};

//! Perform numeric curvature extrema finding on a curve interval.
void numericCurvatureExtrema(const GeomAdaptor_Curve* theCurve,
                             const double             theUMin,
                             const double             theUMax,
                             GeomProp::CurveAnalysis& theResult)
{
  const double aEpsH = THE_EPSILON_SCALE * (theUMax - theUMin);

  FuncCurExt aFunc(theCurve, aEpsH);

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = THE_EXTREMA_NB_SAMPLES;
  aConfig.XTolerance = aEpsH;
  aConfig.FTolerance = aEpsH;

  MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunc, theUMin, theUMax, aConfig);

  if (aRoots.IsDone())
  {
    for (int j = 0; j < aRoots.NbRoots(); ++j)
    {
      double aParam = aRoots[j];
      // Refine the solution.
      MathUtils::Config aBrentCfg;
      aBrentCfg.XTolerance = Precision::PConfusion();
      aBrentCfg.FTolerance = Precision::PConfusion();
      auto aBrent          = MathRoot::Brent(aFunc, aParam - aEpsH, aParam + aEpsH, aBrentCfg);
      if (aBrent.IsDone() && aBrent.Root.has_value())
      {
        aParam = *aBrent.Root;
      }
      const bool             aIsMin = aFunc.IsMinKC(aParam);
      const GeomProp::CIType aType =
        aIsMin ? GeomProp::CIType::MinCurvature : GeomProp::CIType::MaxCurvature;
      theResult.Points.Append({aParam, aType});
    }
  }
  else
  {
    theResult.IsDone = false;
  }
}

//! Perform numeric inflection finding on a curve interval.
void numericInflections(const GeomAdaptor_Curve* theCurve,
                        const double             theUMin,
                        const double             theUMax,
                        GeomProp::CurveAnalysis& theResult)
{
  FuncCurNul aFunc(theCurve);

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = THE_INFLECTION_NB_SAMPLES;
  aConfig.XTolerance = THE_INFLECTION_TOLERANCE;
  aConfig.FTolerance = THE_INFLECTION_TOLERANCE;

  MathRoot::MultipleResult aRoots = MathRoot::FindAllRoots(aFunc, theUMin, theUMax, aConfig);

  if (aRoots.IsDone())
  {
    for (int j = 0; j < aRoots.NbRoots(); ++j)
    {
      theResult.Points.Append({aRoots[j], GeomProp::CIType::Inflection});
    }
  }
  else
  {
    theResult.IsDone = false;
  }
}

} // namespace

//==================================================================================================

GeomProp::TangentResult GeomProp_BezierCurve::Tangent(const double theParam,
                                                       const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2, aD3;
  myAdaptor->D3(theParam, aPnt, aD1, aD2, aD3);
  return GeomProp::ComputeTangent(aD1, aD2, aD3, theTol);
}

//==================================================================================================

GeomProp::CurvatureResult GeomProp_BezierCurve::Curvature(const double theParam,
                                                           const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {0.0, false, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCurvature(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::NormalResult GeomProp_BezierCurve::Normal(const double theParam,
                                                     const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeNormal(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::CentreResult GeomProp_BezierCurve::CentreOfCurvature(const double theParam,
                                                                const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_BezierCurve::FindCurvatureExtrema() const
{
  GeomProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  numericCurvatureExtrema(myAdaptor,
                          myAdaptor->FirstParameter(),
                          myAdaptor->LastParameter(),
                          aResult);
  return aResult;
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_BezierCurve::FindInflections() const
{
  GeomProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  numericInflections(myAdaptor, myAdaptor->FirstParameter(), myAdaptor->LastParameter(), aResult);
  return aResult;
}
