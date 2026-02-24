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

  // Mean curvature: H = (EN - 2FM + GL) / (2 * det)
  const double aH = (aE * aN_ - 2.0 * aF * aM + aG * aL) / (2.0 * aDet);

  // Gaussian curvature: K = (LN - M^2) / det
  const double aK = (aL * aN_ - aM * aM) / aDet;

  // Principal curvatures from: k^2 - 2Hk + K = 0
  const double aDiscriminant = aH * aH - aK;

  SurfaceCurvatureResult aResult;
  aResult.IsDefined = true;

  if (aDiscriminant <= theTol * theTol)
  {
    // Umbilic point: both principal curvatures are equal.
    aResult.MinCurvature = aH;
    aResult.MaxCurvature = aH;
    aResult.IsUmbilic    = true;
    // At umbilic points, directions are undefined - use U and V directions.
    if (theD1U.SquareMagnitude() > theTol * theTol)
    {
      aResult.MinDirection = gp_Dir(theD1U);
    }
    if (theD1V.SquareMagnitude() > theTol * theTol)
    {
      aResult.MaxDirection = gp_Dir(theD1V);
    }
    return aResult;
  }

  const double aSqrtDisc = std::sqrt(std::max(aDiscriminant, 0.0));
  const double aK1       = aH - aSqrtDisc; // min curvature
  const double aK2       = aH + aSqrtDisc; // max curvature

  aResult.MinCurvature = aK1;
  aResult.MaxCurvature = aK2;
  aResult.IsUmbilic    = false;

  // Compute principal directions from the shape operator (Weingarten map).
  // For each principal curvature k, the principal direction (a, b) satisfies:
  // (L - kE)*a + (M - kF)*b = 0
  // (M - kF)*a + (N - kG)*b = 0
  // We pick the equation with the largest coefficient to avoid division by near-zero.
  for (int i = 0; i < 2; ++i)
  {
    const double aKi     = (i == 0) ? aK1 : aK2;
    const double aCoeffA = aL - aKi * aE;
    const double aCoeffB = aM - aKi * aF;
    // TODO: aCoeffC is always equal to aCoeffB (symmetric shape operator matrix).
    //   Consider removing the redundant variable and using aCoeffB directly.
    const double aCoeffC = aM - aKi * aF;
    const double aCoeffD = aN_ - aKi * aG;

    gp_Vec aDir;
    if (std::abs(aCoeffA) > std::abs(aCoeffD))
    {
      // From first equation: a*coeff_a + b*coeff_b = 0 => b/a = -coeff_a/coeff_b
      if (std::abs(aCoeffB) > theTol)
      {
        aDir = theD1U * (-aCoeffB) + theD1V * aCoeffA;
      }
      else
      {
        aDir = theD1V;
      }
    }
    else
    {
      // From second equation: a*coeff_c + b*coeff_d = 0 => a/b = -coeff_d/coeff_c
      if (std::abs(aCoeffC) > theTol)
      {
        aDir = theD1U * aCoeffD + theD1V * (-aCoeffC);
      }
      else
      {
        aDir = theD1U;
      }
    }

    if (aDir.SquareMagnitude() > theTol * theTol)
    {
      if (i == 0)
      {
        aResult.MinDirection = gp_Dir(aDir);
      }
      else
      {
        aResult.MaxDirection = gp_Dir(aDir);
      }
    }
  }

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
