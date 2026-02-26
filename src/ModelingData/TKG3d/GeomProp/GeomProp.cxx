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

#include <GeomProp.hxx>

#include <cmath>

//==================================================================================================

GeomProp::TangentResult GeomProp::ComputeTangent(const gp_Vec& theD1,
                                                 const gp_Vec& theD2,
                                                 const gp_Vec& theD3,
                                                 const double  theTol)
{
  const double aTol2 = theTol * theTol;

  // Try first derivative
  if (theD1.SquareMagnitude() > aTol2)
  {
    return {gp_Dir(theD1), true};
  }

  // Try second derivative
  if (theD2.SquareMagnitude() > aTol2)
  {
    return {gp_Dir(theD2), true};
  }

  // Try third derivative
  if (theD3.SquareMagnitude() > aTol2)
  {
    return {gp_Dir(theD3), true};
  }

  return {{}, false};
}

//==================================================================================================

GeomProp::CurvatureResult GeomProp::ComputeCurvature(const gp_Vec& theD1,
                                                     const gp_Vec& theD2,
                                                     const double  theTol)
{
  const double aTol2 = theTol * theTol;
  const double aDD1  = theD1.SquareMagnitude();

  // If first derivative is null, curvature is infinite (singular point).
  if (aDD1 <= aTol2)
  {
    return {0.0, true, true};
  }

  const double aDD2 = theD2.SquareMagnitude();

  // If second derivative is null, curvature is zero.
  if (aDD2 <= aTol2)
  {
    return {0.0, true, false};
  }

  // Cross product magnitude squared: |D1 x D2|^2
  const gp_Vec aCross = theD1.Crossed(theD2);
  const double aN     = aCross.SquareMagnitude();

  // If D1 and D2 are collinear, curvature is zero.
  const double aT = aN / aDD1 / aDD2;
  if (aT <= aTol2)
  {
    return {0.0, true, false};
  }

  // Curvature = |D1 x D2| / |D1|^3
  const double aCurvature = std::sqrt(aN) / aDD1 / std::sqrt(aDD1);
  return {aCurvature, true, false};
}

//==================================================================================================

GeomProp::NormalResult GeomProp::ComputeNormal(const gp_Vec& theD1,
                                               const gp_Vec& theD2,
                                               const double  theTol)
{
  // First compute curvature to check if normal is defined.
  const CurvatureResult aCurvRes = ComputeCurvature(theD1, theD2, theTol);
  if (!aCurvRes.IsDefined || aCurvRes.IsInfinite || std::abs(aCurvRes.Value) <= theTol)
  {
    return {{}, false};
  }

  // Normal = D2 * (D1.D1) - D1 * (D1.D2)
  // This is equivalent to (D1 x D2) x D1 using the vector triple product identity.
  const gp_Vec aNorm = theD2 * theD1.Dot(theD1) - theD1 * theD1.Dot(theD2);
  if (aNorm.SquareMagnitude() <= theTol * theTol)
  {
    return {{}, false};
  }
  return {gp_Dir(aNorm), true};
}

//==================================================================================================

GeomProp::CentreResult GeomProp::ComputeCentreOfCurvature(const gp_Pnt& thePnt,
                                                          const gp_Vec& theD1,
                                                          const gp_Vec& theD2,
                                                          const double  theTol)
{
  const CurvatureResult aCurvRes = ComputeCurvature(theD1, theD2, theTol);
  if (!aCurvRes.IsDefined || aCurvRes.IsInfinite || std::abs(aCurvRes.Value) <= theTol)
  {
    return {{}, false};
  }

  // Normal vector (unnormalized) = D2 * (D1.D1) - D1 * (D1.D2)
  gp_Vec aNorm = theD2 * theD1.Dot(theD1) - theD1 * theD1.Dot(theD2);
  aNorm.Normalize();
  aNorm.Divide(aCurvRes.Value);

  return {thePnt.Translated(aNorm), true};
}

//==================================================================================================

GeomProp::SurfaceNormalResult GeomProp::ComputeSurfaceNormal(const gp_Vec& theD1U,
                                                             const gp_Vec& theD1V,
                                                             const double  theTol)
{
  const gp_Vec aCross = theD1U.Crossed(theD1V);
  if (aCross.SquareMagnitude() <= theTol * theTol)
  {
    return {{}, false};
  }
  return {gp_Dir(aCross), true};
}

//==================================================================================================

GeomProp::SurfaceCurvatureResult GeomProp::ComputeSurfaceCurvatures(const gp_Vec& theD1U,
                                                                    const gp_Vec& theD1V,
                                                                    const gp_Vec& theD2U,
                                                                    const gp_Vec& theD2V,
                                                                    const gp_Vec& theDUV,
                                                                    const double  theTol)
{
  // Compute surface normal.
  const SurfaceNormalResult aNormRes = ComputeSurfaceNormal(theD1U, theD1V, theTol);
  if (!aNormRes.IsDefined)
  {
    return {};
  }

  const gp_Vec aNormal(aNormRes.Direction);

  // First fundamental form coefficients.
  const double aE = theD1U.Dot(theD1U);
  const double aF = theD1U.Dot(theD1V);
  const double aG = theD1V.Dot(theD1V);

  // Check that both tangent directions are non-degenerate.
  // This matches the old LProp_SLProps guard against "pointed patches"
  // where one tangent is degenerate, preventing division by zero below.
  const double aTol2 = theTol * theTol;
  if (aE <= aTol2 || aG <= aTol2)
  {
    return {};
  }

  // Second fundamental form coefficients.
  const double aL  = aNormal.Dot(theD2U);
  const double aM  = aNormal.Dot(theDUV);
  const double aN_ = aNormal.Dot(theD2V);

  // Discriminant of first fundamental form.
  const double aDet = aE * aG - aF * aF;
  if (std::abs(aDet) <= aTol2)
  {
    return {};
  }

  // Compute principal curvatures and directions using the directional equation
  // approach (matching the legacy LProp_SLProps algorithm).
  //
  // The principal directions satisfy the equation:
  //   A*t^2 + B*t + C = 0
  // where A = E*M - F*L, B = E*N - G*L, C = F*N - G*M,
  // and t is the ratio defining the direction in parameter space.
  const double aA = aE * aM - aF * aL;
  const double aB = aE * aN_ - aG * aL;
  const double aC = aF * aN_ - aG * aM;

  const double aMaxABC = std::max({std::abs(aA), std::abs(aB), std::abs(aC)});

  SurfaceCurvatureResult aResult;
  aResult.IsDefined = true;

  if (aMaxABC < RealEpsilon())
  {
    // Umbilic point: both principal curvatures are equal.
    const double aUmbilicCurv = (std::abs(aG) > theTol * theTol) ? aN_ / aG : 0.0;
    aResult.MinCurvature      = aUmbilicCurv;
    aResult.MaxCurvature      = aUmbilicCurv;
    aResult.IsUmbilic         = true;
    if (theD1U.SquareMagnitude() > theTol * theTol)
    {
      aResult.MinDirection = gp_Dir(theD1U);
      const gp_Vec aMaxDir = theD1U.Crossed(aNormal);
      if (aMaxDir.SquareMagnitude() > theTol * theTol)
      {
        aResult.MaxDirection = gp_Dir(aMaxDir);
      }
    }
    return aResult;
  }

  // Lambda to handle umbilic (double root or degenerate) cases.
  auto handleUmbilic = [&]() {
    const double aUmbilicCurv = (std::abs(aG) > theTol * theTol) ? aN_ / aG : 0.0;
    aResult.MinCurvature      = aUmbilicCurv;
    aResult.MaxCurvature      = aUmbilicCurv;
    aResult.IsUmbilic         = true;
    if (theD1U.SquareMagnitude() > theTol * theTol)
    {
      aResult.MinDirection = gp_Dir(theD1U);
      const gp_Vec aMaxDir = theD1U.Crossed(aNormal);
      if (aMaxDir.SquareMagnitude() > theTol * theTol)
      {
        aResult.MaxDirection = gp_Dir(aMaxDir);
      }
    }
    return aResult;
  };

  double aCurv1, aCurv2;
  gp_Vec aVecCurv1, aVecCurv2;

  // Normalize coefficients for numerical stability.
  const double aNormA = aA / aMaxABC;
  const double aNormB = aB / aMaxABC;
  const double aNormC = aC / aMaxABC;

  if (std::abs(aNormA) > RealEpsilon())
  {
    // Solve A*t^2 + B*t + C = 0; direction = t * D1U + D1V.
    const double aDisc = aNormB * aNormB - 4.0 * aNormA * aNormC;
    if (aDisc < 0.0)
    {
      return handleUmbilic();
    }
    const double aSqrtDisc = std::sqrt(std::max(aDisc, 0.0));
    if (aSqrtDisc < RealEpsilon())
    {
      return handleUmbilic();
    }
    const double aRoot1  = (-aNormB + aSqrtDisc) / (2.0 * aNormA);
    const double aRoot2  = (-aNormB - aSqrtDisc) / (2.0 * aNormA);
    const double aDenom1 = (aE * aRoot1 + 2.0 * aF) * aRoot1 + aG;
    const double aDenom2 = (aE * aRoot2 + 2.0 * aF) * aRoot2 + aG;
    if (std::abs(aDenom1) < RealEpsilon() || std::abs(aDenom2) < RealEpsilon())
    {
      return handleUmbilic();
    }
    aCurv1    = ((aL * aRoot1 + 2.0 * aM) * aRoot1 + aN_) / aDenom1;
    aCurv2    = ((aL * aRoot2 + 2.0 * aM) * aRoot2 + aN_) / aDenom2;
    aVecCurv1 = theD1U * aRoot1 + theD1V;
    aVecCurv2 = theD1U * aRoot2 + theD1V;
  }
  else if (std::abs(aNormC) > RealEpsilon())
  {
    // Solve C*t^2 + B*t + A = 0; direction = D1U + t * D1V.
    const double aDisc = aNormB * aNormB - 4.0 * aNormC * aNormA;
    if (aDisc < 0.0)
    {
      return handleUmbilic();
    }
    const double aSqrtDisc = std::sqrt(std::max(aDisc, 0.0));
    if (aSqrtDisc < RealEpsilon())
    {
      return handleUmbilic();
    }
    const double aRoot1  = (-aNormB + aSqrtDisc) / (2.0 * aNormC);
    const double aRoot2  = (-aNormB - aSqrtDisc) / (2.0 * aNormC);
    const double aDenom1 = (aG * aRoot1 + 2.0 * aF) * aRoot1 + aE;
    const double aDenom2 = (aG * aRoot2 + 2.0 * aF) * aRoot2 + aE;
    if (std::abs(aDenom1) < RealEpsilon() || std::abs(aDenom2) < RealEpsilon())
    {
      return handleUmbilic();
    }
    aCurv1    = ((aN_ * aRoot1 + 2.0 * aM) * aRoot1 + aL) / aDenom1;
    aCurv2    = ((aN_ * aRoot2 + 2.0 * aM) * aRoot2 + aL) / aDenom2;
    aVecCurv1 = theD1U + theD1V * aRoot1;
    aVecCurv2 = theD1U + theD1V * aRoot2;
  }
  else
  {
    // Both A and C are near zero: principal directions align with D1U and D1V.
    aCurv1    = aL / aE;
    aCurv2    = aN_ / aG;
    aVecCurv1 = theD1U;
    aVecCurv2 = theD1V;
  }

  // Sort so that min curvature is first.
  if (aCurv1 < aCurv2)
  {
    aResult.MinCurvature = aCurv1;
    aResult.MaxCurvature = aCurv2;
    if (aVecCurv1.SquareMagnitude() > theTol * theTol)
      aResult.MinDirection = gp_Dir(aVecCurv1);
    if (aVecCurv2.SquareMagnitude() > theTol * theTol)
      aResult.MaxDirection = gp_Dir(aVecCurv2);
  }
  else
  {
    aResult.MinCurvature = aCurv2;
    aResult.MaxCurvature = aCurv1;
    if (aVecCurv2.SquareMagnitude() > theTol * theTol)
      aResult.MinDirection = gp_Dir(aVecCurv2);
    if (aVecCurv1.SquareMagnitude() > theTol * theTol)
      aResult.MaxDirection = gp_Dir(aVecCurv1);
  }
  aResult.IsUmbilic = false;

  return aResult;
}

//==================================================================================================

GeomProp::MeanGaussianResult GeomProp::ComputeMeanGaussian(const gp_Vec& theD1U,
                                                           const gp_Vec& theD1V,
                                                           const gp_Vec& theD2U,
                                                           const gp_Vec& theD2V,
                                                           const gp_Vec& theDUV,
                                                           const double  theTol)
{
  // Compute surface normal.
  const SurfaceNormalResult aNormRes = ComputeSurfaceNormal(theD1U, theD1V, theTol);
  if (!aNormRes.IsDefined)
  {
    return {};
  }

  const gp_Vec aNormal(aNormRes.Direction);

  // First fundamental form coefficients.
  const double aE = theD1U.Dot(theD1U);
  const double aF = theD1U.Dot(theD1V);
  const double aG = theD1V.Dot(theD1V);

  // Second fundamental form coefficients.
  const double aL  = aNormal.Dot(theD2U);
  const double aM  = aNormal.Dot(theDUV);
  const double aN_ = aNormal.Dot(theD2V);

  // Discriminant of first fundamental form.
  const double aDet = aE * aG - aF * aF;
  if (std::abs(aDet) <= theTol * theTol)
  {
    return {};
  }

  MeanGaussianResult aResult;
  aResult.IsDefined         = true;
  aResult.MeanCurvature     = (aE * aN_ - 2.0 * aF * aM + aG * aL) / (2.0 * aDet);
  aResult.GaussianCurvature = (aL * aN_ - aM * aM) / aDet;
  return aResult;
}
