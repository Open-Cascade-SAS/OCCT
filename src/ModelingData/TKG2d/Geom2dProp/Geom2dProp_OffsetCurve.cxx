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

#include <Geom2dProp_OffsetCurve.hxx>

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

//! Function for finding curvature extrema on offset curves.
class FuncCurExt
{
public:
  FuncCurExt(const Geom2dAdaptor_Curve* theCurve, const double theTol)
      : myCurve(theCurve),
        myEpsX(theTol)
  {
  }

  bool Value(const double X, double& F)
  {
    gp_Pnt2d aP;
    gp_Vec2d aV1, aV2, aV3;
    myCurve->D3(X, aP, aV1, aV2, aV3);

    const double aCPV1V2 = aV1.Crossed(aV2);
    const double aCPV1V3 = aV1.Crossed(aV3);
    const double aV1V2   = aV1.Dot(aV2);
    const double aV1V1   = aV1.SquareMagnitude();
    const double aNV1    = std::sqrt(aV1V1);
    const double aV13    = aV1V1 * aNV1;
    const double aV15    = aV13 * aV1V1;

    if (aV15 < gp::Resolution())
    {
      return false;
    }

    F = aCPV1V3 / aV13 - THE_CURVATURE_DERIV_COEFF * aCPV1V2 * aV1V2 / aV15;
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

  bool IsMinKC(const double X) const
  {
    gp_Pnt2d aP;
    gp_Vec2d aV1, aV2, aV3;

    myCurve->D3(X, aP, aV1, aV2, aV3);
    const double aV1V1 = aV1.SquareMagnitude();
    const double aNV1  = std::sqrt(aV1V1);
    const double aV13  = aV1V1 * aNV1;
    if (aV13 < gp::Resolution())
    {
      return false;
    }
    const double aKC = aV1.Crossed(aV2) / aV13;

    double aDx = myEpsX;
    if (X + aDx > myCurve->LastParameter())
    {
      aDx = -aDx;
    }

    myCurve->D3(X + aDx, aP, aV1, aV2, aV3);
    const double aV1V1n = aV1.SquareMagnitude();
    const double aNV1n  = std::sqrt(aV1V1n);
    const double aV13n  = aV1V1n * aNV1n;
    if (aV13n < gp::Resolution())
    {
      return false;
    }
    const double aKP = aV1.Crossed(aV2) / aV13n;

    return std::abs(aKC) > std::abs(aKP);
  }

private:
  const Geom2dAdaptor_Curve* myCurve;
  double                     myEpsX;
};

//! Function for finding inflection points on offset curves.
class FuncCurNul
{
public:
  FuncCurNul(const Geom2dAdaptor_Curve* theCurve)
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
    gp_Pnt2d aP;
    gp_Vec2d aV1, aV2, aV3;
    myCurve->D3(X, aP, aV1, aV2, aV3);

    const double aCP1  = aV1.Crossed(aV2);
    const double aCP2  = aV1.Crossed(aV3);
    const double aV1V2 = aV1.Dot(aV2);
    const double aV2V3 = aV2.Dot(aV3);
    const double aNV1  = aV1.Magnitude();
    const double aNV2  = aV2.Magnitude();

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

    F = aCP1 / (aNV1 * aNV2);
    D = (aCP2 - aCP1 * aV1V2 / (aNV1 * aNV1) - aCP1 * aV2V3 / (aNV2 * aNV2)) / (aNV1 * aNV2);
    return true;
  }

private:
  const Geom2dAdaptor_Curve* myCurve;
};

} // namespace

//==================================================================================================

Geom2dProp::TangentResult Geom2dProp_OffsetCurve::Tangent(const double theParam,
                                                          const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2, aD3;
  myAdaptor->D3(theParam, aPnt, aD1, aD2, aD3);
  return Geom2dProp::ComputeTangent(aD1, aD2, aD3, theTol);
}

//==================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_OffsetCurve::Curvature(const double theParam,
                                                              const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {0.0, false, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return Geom2dProp::ComputeCurvature(aD1, aD2, theTol);
}

//==================================================================================================

Geom2dProp::NormalResult Geom2dProp_OffsetCurve::Normal(const double theParam,
                                                        const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return Geom2dProp::ComputeNormal(aD1, aD2, theTol);
}

//==================================================================================================

Geom2dProp::CentreResult Geom2dProp_OffsetCurve::CentreOfCurvature(const double theParam,
                                                                   const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return Geom2dProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, theTol);
}

//==================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_OffsetCurve::FindCurvatureExtrema() const
{
  Geom2dProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  const double aUMin = myAdaptor->FirstParameter();
  const double aUMax = myAdaptor->LastParameter();
  const double aEpsH = THE_EPSILON_SCALE * (aUMax - aUMin);

  FuncCurExt aFunc(myAdaptor, aEpsH);

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = THE_EXTREMA_NB_SAMPLES;
  aConfig.XTolerance = aEpsH;
  aConfig.FTolerance = aEpsH;

  MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunc, aUMin, aUMax, aConfig);

  if (aRoots.IsDone())
  {
    for (int j = 0; j < aRoots.NbRoots(); ++j)
    {
      double            aParam = aRoots[j];
      MathUtils::Config aBrentCfg;
      aBrentCfg.XTolerance = Precision::PConfusion();
      aBrentCfg.FTolerance = Precision::PConfusion();
      auto aBrent          = MathRoot::Brent(aFunc, aParam - aEpsH, aParam + aEpsH, aBrentCfg);
      if (aBrent.IsDone() && aBrent.Root.has_value())
      {
        aParam = *aBrent.Root;
      }
      const bool               aIsMin = aFunc.IsMinKC(aParam);
      const Geom2dProp::CIType aType =
        aIsMin ? Geom2dProp::CIType::MinCurvature : Geom2dProp::CIType::MaxCurvature;
      aResult.Points.Append({aParam, aType});
    }
  }
  else
  {
    aResult.IsDone = false;
  }

  return aResult;
}

//==================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_OffsetCurve::FindInflections() const
{
  Geom2dProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  FuncCurNul aFunc(myAdaptor);

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = THE_INFLECTION_NB_SAMPLES;
  aConfig.XTolerance = THE_INFLECTION_TOLERANCE;
  aConfig.FTolerance = THE_INFLECTION_TOLERANCE;

  MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRoots(aFunc, myAdaptor->FirstParameter(), myAdaptor->LastParameter(), aConfig);

  if (aRoots.IsDone())
  {
    for (int j = 0; j < aRoots.NbRoots(); ++j)
    {
      aResult.Points.Append({aRoots[j], Geom2dProp::CIType::Inflection});
    }
  }
  else
  {
    aResult.IsDone = false;
  }

  return aResult;
}
