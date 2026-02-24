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

#include <GeomProp_BSplineCurve.hxx>

#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <MathRoot_Brent.hxx>
#include <MathRoot_Multiple.hxx>
#include <NCollection_Array1.hxx>
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

    if (aCPMag < gp::Resolution())
    {
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

  bool IsMinKC(const double X) const
  {
    gp_Pnt aP;
    gp_Vec aV1, aV2, aV3;

    myCurve->D3(X, aP, aV1, aV2, aV3);
    const double aV1V1 = aV1.SquareMagnitude();
    const double aNV1  = std::sqrt(aV1V1);
    const double aV13  = aV1V1 * aNV1;

    if (aV13 < gp::Resolution())
    {
      return false;
    }
    const double aKC = aV1.Crossed(aV2).Magnitude() / aV13;

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
    const double aKP = aV1.Crossed(aV2).Magnitude() / aV13n;

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

//! Remove duplicate points that may appear at shared interval boundaries.
void removeDuplicatePoints(GeomProp::CurveAnalysis& theResult, const double theTol)
{
  const int aNbPts = theResult.Points.Size();
  if (aNbPts <= 1)
  {
    return;
  }

  bool aHasDuplicates = false;
  for (int i = 1; i < aNbPts && !aHasDuplicates; ++i)
  {
    for (int j = 0; j < i; ++j)
    {
      if (std::abs(theResult.Points[i].Parameter - theResult.Points[j].Parameter) < theTol)
      {
        aHasDuplicates = true;
        break;
      }
    }
  }
  if (!aHasDuplicates)
  {
    return;
  }

  NCollection_DynamicArray<GeomProp::CurveSpecialPoint> aFiltered;
  aFiltered.Append(theResult.Points[0]);
  for (int i = 1; i < aNbPts; ++i)
  {
    bool aIsDuplicate = false;
    for (int j = static_cast<int>(aFiltered.Size()) - 1; j >= 0; --j)
    {
      if (std::abs(theResult.Points[i].Parameter - aFiltered[j].Parameter) < theTol)
      {
        aIsDuplicate = true;
        break;
      }
    }
    if (!aIsDuplicate)
    {
      aFiltered.Append(theResult.Points[i]);
    }
  }

  theResult.Points = std::move(aFiltered);
}

} // namespace

//==================================================================================================

GeomProp::TangentResult GeomProp_BSplineCurve::Tangent(const double theParam,
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

GeomProp::CurvatureResult GeomProp_BSplineCurve::Curvature(const double theParam,
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

GeomProp::NormalResult GeomProp_BSplineCurve::Normal(const double theParam,
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

GeomProp::CentreResult GeomProp_BSplineCurve::CentreOfCurvature(const double theParam,
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

GeomProp::CurveAnalysis GeomProp_BSplineCurve::FindCurvatureExtrema() const
{
  GeomProp::CurveAnalysis aResult;
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
    const double aEpsH =
      THE_EPSILON_SCALE * (myAdaptor->LastParameter() - myAdaptor->FirstParameter());
    removeDuplicatePoints(aResult, aEpsH);
  }

  return aResult;
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_BSplineCurve::FindInflections() const
{
  GeomProp::CurveAnalysis aResult;
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
    const int                  aNbInt = myAdaptor->NbIntervals(GeomAbs_C3);
    NCollection_Array1<double> aParams(1, aNbInt + 1);
    myAdaptor->Intervals(aParams, GeomAbs_C3);
    for (int i = 1; i <= aNbInt; ++i)
    {
      numericInflections(myAdaptor, aParams(i), aParams(i + 1), aResult);
    }
    removeDuplicatePoints(aResult, THE_INFLECTION_TOLERANCE);
  }

  return aResult;
}
