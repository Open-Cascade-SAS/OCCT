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

#include <Geom2dProp_BSplineCurve.hxx>

#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <math_BracketedRoot.hxx>
#include <math_FunctionRoots.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

//! Function for finding curvature extrema: F = d(curvature)/dU = 0
class FuncCurExt : public math_FunctionWithDerivative
{
public:
  FuncCurExt(const Geom2dAdaptor_Curve* theCurve, const double theTol)
      : myCurve(theCurve),
        myEpsX(theTol)
  {
  }

  bool Value(const double X, double& F) override
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

    F = aCPV1V3 / aV13 - 3.0 * aCPV1V2 * aV1V2 / aV15;
    return true;
  }

  bool Derivative(const double X, double& D) override
  {
    double aF;
    return Values(X, aF, D);
  }

  bool Values(const double X, double& F, double& D) override
  {
    double aDx = myEpsX / 100.0;
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

//! Function for finding inflection points: F = (V1^V2) / (||V1|| * ||V2||) = 0
class FuncCurNul : public math_FunctionWithDerivative
{
public:
  FuncCurNul(const Geom2dAdaptor_Curve* theCurve)
      : myCurve(theCurve)
  {
  }

  bool Value(const double X, double& F) override
  {
    double aD;
    return Values(X, F, aD);
  }

  bool Derivative(const double X, double& D) override
  {
    double aF;
    return Values(X, aF, D);
  }

  bool Values(const double X, double& F, double& D) override
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

    if (aNV2 < 1.0e-4)
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

//! Perform numeric curvature extrema finding on a curve interval.
void numericCurvatureExtrema(const Geom2dAdaptor_Curve* theCurve,
                             const double               theUMin,
                             const double               theUMax,
                             Geom2dProp::CurveAnalysis& theResult)
{
  const double     aEpsH = 1.0e-4 * (theUMax - theUMin);
  constexpr double aTol  = Precision::PConfusion();

  FuncCurExt    aFunc(theCurve, aEpsH);
  constexpr int aNbSamples = 100;

  math_FunctionRoots aSolRoot(aFunc, theUMin, theUMax, aNbSamples, aEpsH, aEpsH, aEpsH);

  if (aSolRoot.IsDone())
  {
    for (int j = 1; j <= aSolRoot.NbSolutions(); ++j)
    {
      double             aParam = aSolRoot.Value(j);
      math_BracketedRoot aBS(aFunc, aParam - aEpsH, aParam + aEpsH, aTol);
      if (aBS.IsDone())
      {
        aParam = aBS.Root();
      }
      const bool               aIsMin = aFunc.IsMinKC(aParam);
      const Geom2dProp::CIType aType =
        aIsMin ? Geom2dProp::CIType::MinCurvature : Geom2dProp::CIType::MaxCurvature;
      theResult.Points.Append({aParam, aType});
    }
  }
  else
  {
    theResult.IsDone = false;
  }
}

//! Perform numeric inflection finding on a curve interval.
void numericInflections(const Geom2dAdaptor_Curve* theCurve,
                        const double               theUMin,
                        const double               theUMax,
                        Geom2dProp::CurveAnalysis& theResult)
{
  FuncCurNul    aFunc(theCurve);
  constexpr int aNbSamples = 30;

  math_FunctionRoots aSolRoot(aFunc, theUMin, theUMax, aNbSamples, 1.0e-6, 1.0e-6, 1.0e-6);

  if (aSolRoot.IsDone())
  {
    for (int j = 1; j <= aSolRoot.NbSolutions(); ++j)
    {
      theResult.Points.Append({aSolRoot.Value(j), Geom2dProp::CIType::Inflection});
    }
  }
  else
  {
    theResult.IsDone = false;
  }
}

} // namespace

//==================================================================================================

Geom2dProp::TangentResult Geom2dProp_BSplineCurve::Tangent(const double theParam,
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

Geom2dProp::CurvatureResult Geom2dProp_BSplineCurve::Curvature(const double theParam,
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

Geom2dProp::NormalResult Geom2dProp_BSplineCurve::Normal(const double theParam,
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

Geom2dProp::CentreResult Geom2dProp_BSplineCurve::CentreOfCurvature(const double theParam,
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

Geom2dProp::CurveAnalysis Geom2dProp_BSplineCurve::FindCurvatureExtrema() const
{
  Geom2dProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  if (myAdaptor->Continuity() >= GeomAbs_C3)
  {
    numericCurvatureExtrema(myAdaptor,
                            myAdaptor->FirstParameter(),
                            myAdaptor->LastParameter(),
                            aResult);
  }
  else
  {
    // Subdivide into C3 intervals.
    const int                  aNbInt = myAdaptor->NbIntervals(GeomAbs_C3);
    NCollection_Array1<double> aParams(1, aNbInt + 1);
    myAdaptor->Intervals(aParams, GeomAbs_C3);
    for (int i = 1; i <= aNbInt; ++i)
    {
      numericCurvatureExtrema(myAdaptor, aParams(i), aParams(i + 1), aResult);
    }
  }

  return aResult;
}

//==================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_BSplineCurve::FindInflections() const
{
  Geom2dProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  if (myAdaptor->Continuity() >= GeomAbs_C3)
  {
    numericInflections(myAdaptor, myAdaptor->FirstParameter(), myAdaptor->LastParameter(), aResult);
  }
  else
  {
    // Subdivide into C3 intervals.
    const int                  aNbInt = myAdaptor->NbIntervals(GeomAbs_C3);
    NCollection_Array1<double> aParams(1, aNbInt + 1);
    myAdaptor->Intervals(aParams, GeomAbs_C3);
    for (int i = 1; i <= aNbInt; ++i)
    {
      numericInflections(myAdaptor, aParams(i), aParams(i + 1), aResult);
    }
  }

  return aResult;
}
