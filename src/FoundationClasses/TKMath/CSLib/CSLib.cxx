// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

#include <CSLib.hxx>
#include <CSLib_NormalPolyDef.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <math_FunctionRoots.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <algorithm>
#include <cmath>

namespace
{
//! Angular tolerance for parallel/opposite vector detection (radians).
constexpr double THE_PARALLEL_ANGULAR_TOL = 1e-6;

//! Maximum number of iterations for root finding algorithm.
constexpr int THE_MAX_ROOT_ITERATIONS = 200;

//! Tolerance for root finding algorithm.
constexpr double THE_ROOT_FINDING_TOL = 1e-5;
} // namespace

//=================================================================================================

void CSLib::Normal(const gp_Vec&           theD1U,
                   const gp_Vec&           theD1V,
                   const double            theSinTol,
                   CSLib_DerivativeStatus& theStatus,
                   gp_Dir&                 theNormal)
{
  const double aD1UMag  = theD1U.SquareMagnitude();
  const double aD1VMag  = theD1V.SquareMagnitude();
  const gp_Vec aD1UxD1V = theD1U.Crossed(theD1V);

  if (aD1UMag <= gp::Resolution() && aD1VMag <= gp::Resolution())
  {
    theStatus = CSLib_D1IsNull;
  }
  else if (aD1UMag <= gp::Resolution())
  {
    theStatus = CSLib_D1uIsNull;
  }
  else if (aD1VMag <= gp::Resolution())
  {
    theStatus = CSLib_D1vIsNull;
  }
  else
  {
    const double aSin2 = aD1UxD1V.SquareMagnitude() / (aD1UMag * aD1VMag);
    if (aSin2 < theSinTol * theSinTol)
    {
      theStatus = CSLib_D1uIsParallelD1v;
    }
    else
    {
      theNormal = gp_Dir(aD1UxD1V);
      theStatus = CSLib_Done;
    }
  }
}

//=================================================================================================

void CSLib::Normal(const gp_Vec&       theD1U,
                   const gp_Vec&       theD1V,
                   const gp_Vec&       theD2U,
                   const gp_Vec&       theD2V,
                   const gp_Vec&       theD2UV,
                   const double        theSinTol,
                   bool&               theDone,
                   CSLib_NormalStatus& theStatus,
                   gp_Dir&             theNormal)
{
  // Compute approximate normal using Taylor expansion:
  // N(u0+du, v0+dv) = N0 + dN/du * du + dN/dv * dv + O(du^2, dv^2)
  // where N = D1U ^ D1V, so dN/du = D2U ^ D1V + D1U ^ D2UV
  // and dN/dv = D2UV ^ D1V + D1U ^ D2V

  gp_Vec aD1Nu = theD2U.Crossed(theD1V);
  aD1Nu.Add(theD1U.Crossed(theD2UV));

  gp_Vec aD1Nv = theD2UV.Crossed(theD1V);
  aD1Nv.Add(theD1U.Crossed(theD2V));

  const double aLD1Nu = aD1Nu.SquareMagnitude();
  const double aLD1Nv = aD1Nv.SquareMagnitude();

  if (aLD1Nu <= RealEpsilon() && aLD1Nv <= RealEpsilon())
  {
    theStatus = CSLib_D1NIsNull;
    theDone   = false;
  }
  else if (aLD1Nu < RealEpsilon())
  {
    theStatus = CSLib_D1NuIsNull;
    theDone   = true;
    theNormal = gp_Dir(aD1Nv);
  }
  else if (aLD1Nv < RealEpsilon())
  {
    theStatus = CSLib_D1NvIsNull;
    theDone   = true;
    theNormal = gp_Dir(aD1Nu);
  }
  else if ((aLD1Nv / aLD1Nu) <= RealEpsilon())
  {
    theStatus = CSLib_D1NvNuRatioIsNull;
    theDone   = false;
  }
  else if ((aLD1Nu / aLD1Nv) <= RealEpsilon())
  {
    theStatus = CSLib_D1NuNvRatioIsNull;
    theDone   = false;
  }
  else
  {
    const gp_Vec aD1NCross = aD1Nu.Crossed(aD1Nv);
    const double aSin2     = aD1NCross.SquareMagnitude() / (aLD1Nu * aLD1Nv);

    if (aSin2 < theSinTol * theSinTol)
    {
      theStatus = CSLib_D1NuIsParallelD1Nv;
      theDone   = true;
      theNormal = gp_Dir(aD1Nu);
    }
    else
    {
      theStatus = CSLib_InfinityOfSolutions;
      theDone   = false;
    }
  }
}

//=================================================================================================

void CSLib::Normal(const gp_Vec&       theD1U,
                   const gp_Vec&       theD1V,
                   const double        theMagTol,
                   CSLib_NormalStatus& theStatus,
                   gp_Dir&             theNormal)
{
  const double aD1UMag  = theD1U.Magnitude();
  const double aD1VMag  = theD1V.Magnitude();
  const gp_Vec aD1UxD1V = theD1U.Crossed(theD1V);
  const double aNMag    = aD1UxD1V.Magnitude();

  if (aNMag <= theMagTol || aD1UMag <= theMagTol || aD1VMag <= theMagTol)
  {
    theStatus = CSLib_Singular;
  }
  else
  {
    // Normalize tangent vectors first for better numerical stability.
    const gp_Dir aD1UDir(theD1U);
    const gp_Dir aD1VDir(theD1V);
    theNormal = gp_Dir(aD1UDir.Crossed(aD1VDir));
    theStatus = CSLib_Defined;
  }
}

//=================================================================================================

void CSLib::Normal(const int                 theMaxOrder,
                   const TColgp_Array2OfVec& theDerNUV,
                   const double              theSinTol,
                   const double              theU,
                   const double              theV,
                   const double              theUmin,
                   const double              theUmax,
                   const double              theVmin,
                   const double              theVmax,
                   CSLib_NormalStatus&       theStatus,
                   gp_Dir&                   theNormal,
                   int&                      theOrderU,
                   int&                      theOrderV)
{
  int    anOrder    = -1;
  int    aFoundUIdx = 0;
  bool   aFound     = false;
  double aNorme     = 0.0;
  gp_Vec aD;

  // Find k0 such that all derivatives N = dS/du ^ dS/dv are null till order k0-1.
  while (!aFound && anOrder < theMaxOrder)
  {
    ++anOrder;
    aFoundUIdx = anOrder;
    while (aFoundUIdx >= 0 && !aFound)
    {
      const int aVIdx = anOrder - aFoundUIdx;
      aD              = theDerNUV(aFoundUIdx, aVIdx);
      aNorme          = aD.Magnitude();
      aFound          = (aNorme >= theSinTol);
      --aFoundUIdx;
    }
  }

  theOrderU = aFoundUIdx + 1;
  theOrderV = anOrder - theOrderU;

  // Vk0 is the first non-null derivative of N: the reference vector.
  if (!aFound)
  {
    theStatus = CSLib_Singular;
    return;
  }

  if (anOrder == 0)
  {
    theStatus = CSLib_Defined;
    theNormal = aD.Normalized();
    return;
  }

  const gp_Vec         aVk0 = theDerNUV(theOrderU, theOrderV);
  TColStd_Array1OfReal aRatio(0, anOrder);

  // Calculate lambda_i ratios for each derivative at this order.
  int  aRatioIdx = 0;
  bool isDefined = false;
  while (aRatioIdx <= anOrder && !isDefined)
  {
    const gp_Vec& aDerVec = theDerNUV(aRatioIdx, anOrder - aRatioIdx);
    if (aDerVec.Magnitude() <= theSinTol)
    {
      aRatio(aRatioIdx) = 0.0;
    }
    else if (aDerVec.IsParallel(aVk0, THE_PARALLEL_ANGULAR_TOL))
    {
      double aMagnitudeRatio = aDerVec.Magnitude() / aVk0.Magnitude();
      if (aDerVec.IsOpposite(aVk0, THE_PARALLEL_ANGULAR_TOL))
      {
        aMagnitudeRatio = -aMagnitudeRatio;
      }
      aRatio(aRatioIdx) = aMagnitudeRatio;
    }
    else
    {
      isDefined = true;
    }
    ++aRatioIdx;
  }

  if (isDefined)
  {
    theStatus = CSLib_Defined;
    theNormal = aD.Normalized();
    return;
  }

  // All lambda_i exist - analyze the polynomial sign.
  double aInf = -M_PI;
  double aSup = M_PI;

  // Determine domain based on position (interior, edge, corner).
  const bool isFU = (std::abs(theU - theUmin) < Precision::PConfusion());
  const bool isLU = (std::abs(theU - theUmax) < Precision::PConfusion());
  const bool isFV = (std::abs(theV - theVmin) < Precision::PConfusion());
  const bool isLV = (std::abs(theV - theVmax) < Precision::PConfusion());

  if (isLU)
  {
    aInf = M_PI / 2.0;
    aSup = 3.0 * M_PI / 2.0;
    if (isLV)
    {
      aInf = M_PI;
    }
    if (isFV)
    {
      aSup = M_PI;
    }
  }
  else if (isFU)
  {
    aSup = M_PI / 2.0;
    aInf = -M_PI / 2.0;
    if (isLV)
    {
      aSup = 0.0;
    }
    if (isFV)
    {
      aInf = 0.0;
    }
  }
  else if (isLV)
  {
    aInf = -M_PI;
    aSup = 0.0;
  }
  else if (isFV)
  {
    aInf = 0.0;
    aSup = M_PI;
  }

  bool   aChangesSign = false;
  double aVprec       = 0.0;
  double aVsuiv       = 0.0;

  // Create polynomial and find its roots.
  CSLib_NormalPolyDef aPoly(anOrder, aRatio);
  math_FunctionRoots  aFindRoots(aPoly,
                                aInf,
                                aSup,
                                THE_MAX_ROOT_ITERATIONS,
                                THE_ROOT_FINDING_TOL,
                                Precision::Confusion(),
                                Precision::Confusion());

  if (aFindRoots.IsDone() && aFindRoots.NbSolutions() > 0)
  {
    // Sort roots in ascending order.
    const int            aNbSol = aFindRoots.NbSolutions();
    TColStd_Array1OfReal aSol(0, aNbSol + 1);

    for (int aRootIdx = 1; aRootIdx <= aNbSol; ++aRootIdx)
    {
      aSol(aRootIdx) = aFindRoots.Value(aRootIdx);
    }
    std::sort(&aSol(1), &aSol(aNbSol) + 1);

    // Add domain limits.
    aSol(0)          = aInf;
    aSol(aNbSol + 1) = aSup;

    // Check for sign changes between consecutive roots.
    int aFirst = 0;
    for (int aIntervalIdx = 0; aIntervalIdx <= aNbSol; ++aIntervalIdx)
    {
      if (std::abs(aSol(aIntervalIdx + 1) - aSol(aIntervalIdx)) > Precision::PConfusion())
      {
        aPoly.Value((aSol(aIntervalIdx) + aSol(aIntervalIdx + 1)) / 2.0, aVsuiv);
        if (aFirst == 0)
        {
          aFirst       = aIntervalIdx;
          aChangesSign = false;
          aVprec       = aVsuiv;
        }
        else
        {
          aChangesSign = aChangesSign || (aVprec * aVsuiv) < 0.0;
          aVprec       = aVsuiv;
        }
      }
    }
  }
  else
  {
    // No roots found, polynomial doesn't change sign.
    aChangesSign = false;
    aPoly.Value(aInf, aVsuiv);
  }

  // Determine status based on polynomial sign.
  if (aChangesSign)
  {
    theStatus = CSLib_InfinityOfSolutions;
  }
  else
  {
    theStatus       = CSLib_Defined;
    const int aSign = (aVsuiv > 0.0) ? 1 : -1;
    theNormal       = aSign * aVk0.Normalized();
  }
}

//=================================================================================================

gp_Vec CSLib::DNNUV(const int theNu, const int theNv, const TColgp_Array2OfVec& theDerSurf)
{
  gp_Vec aResult(0.0, 0.0, 0.0);

  for (int i = 0; i <= theNu; ++i)
  {
    for (int j = 0; j <= theNv; ++j)
    {
      const gp_Vec& aVG      = theDerSurf.Value(i + 1, j);
      const gp_Vec& aVD      = theDerSurf.Value(theNu - i, theNv + 1 - j);
      const gp_Vec  aCross   = aVG.Crossed(aVD);
      const double  aBinCoef = PLib::Bin(theNu, i) * PLib::Bin(theNv, j);
      aResult.Add(aBinCoef * aCross);
    }
  }

  return aResult;
}

//=================================================================================================

gp_Vec CSLib::DNNUV(const int                 theNu,
                    const int                 theNv,
                    const TColgp_Array2OfVec& theDerSurf1,
                    const TColgp_Array2OfVec& theDerSurf2)
{
  gp_Vec aResult(0.0, 0.0, 0.0);

  for (int i = 0; i <= theNu; ++i)
  {
    for (int j = 0; j <= theNv; ++j)
    {
      const gp_Vec& aVG      = theDerSurf1.Value(i + 1, j);
      const gp_Vec& aVD      = theDerSurf2.Value(theNu - i, theNv + 1 - j);
      const gp_Vec  aCross   = aVG.Crossed(aVD);
      const double  aBinCoef = PLib::Bin(theNu, i) * PLib::Bin(theNv, j);
      aResult.Add(aBinCoef * aCross);
    }
  }

  return aResult;
}

//=================================================================================================

gp_Vec CSLib::DNNormal(const int                 theNu,
                       const int                 theNv,
                       const TColgp_Array2OfVec& theDerNUV,
                       const int                 theIduref,
                       const int                 theIdvref)
{
  const int aKderiv = theNu + theNv;

  TColgp_Array2OfVec   aDerVecNor(0, aKderiv, 0, aKderiv);
  TColStd_Array2OfReal aTabScal(0, aKderiv, 0, aKderiv);
  TColStd_Array2OfReal aTabNorm(0, aKderiv, 0, aKderiv);

  gp_Vec aDerNor = theDerNUV.Value(theIduref, theIdvref).Normalized();
  aDerVecNor.SetValue(0, 0, aDerNor);

  const double aDnorm0 = theDerNUV.Value(theIduref, theIdvref).Dot(aDerVecNor.Value(0, 0));
  aTabNorm.SetValue(0, 0, aDnorm0);
  aTabScal.SetValue(0, 0, 0.0);

  for (int aMderiv = 1; aMderiv <= aKderiv; ++aMderiv)
  {
    for (int aPderiv = 0; aPderiv <= aMderiv; ++aPderiv)
    {
      const int aQderiv = aMderiv - aPderiv;
      if (aPderiv > theNu || aQderiv > theNv)
      {
        continue;
      }

      // Compute n . derivative(p,q) of n
      double aScal = 0.0;
      if (aPderiv > aQderiv)
      {
        for (int aJderiv = 1; aJderiv <= aQderiv; ++aJderiv)
        {
          aScal -= PLib::Bin(aQderiv, aJderiv)
                   * aDerVecNor.Value(0, aJderiv).Dot(aDerVecNor.Value(aPderiv, aQderiv - aJderiv));
        }

        for (int aJderiv = 0; aJderiv < aQderiv; ++aJderiv)
        {
          aScal -= PLib::Bin(aQderiv, aJderiv)
                   * aDerVecNor.Value(aPderiv, aJderiv).Dot(aDerVecNor.Value(0, aQderiv - aJderiv));
        }

        for (int aIderiv = 1; aIderiv < aPderiv; ++aIderiv)
        {
          for (int aJderiv = 0; aJderiv <= aQderiv; ++aJderiv)
          {
            aScal -= PLib::Bin(aPderiv, aIderiv) * PLib::Bin(aQderiv, aJderiv)
                     * aDerVecNor.Value(aIderiv, aJderiv)
                         .Dot(aDerVecNor.Value(aPderiv - aIderiv, aQderiv - aJderiv));
          }
        }
      }
      else
      {
        for (int aIderiv = 1; aIderiv <= aPderiv; ++aIderiv)
        {
          aScal -= PLib::Bin(aPderiv, aIderiv)
                   * aDerVecNor.Value(aIderiv, 0).Dot(aDerVecNor.Value(aPderiv - aIderiv, aQderiv));
        }

        for (int aIderiv = 0; aIderiv < aPderiv; ++aIderiv)
        {
          aScal -= PLib::Bin(aPderiv, aIderiv)
                   * aDerVecNor.Value(aIderiv, aQderiv).Dot(aDerVecNor.Value(aPderiv - aIderiv, 0));
        }

        for (int aIderiv = 0; aIderiv <= aPderiv; ++aIderiv)
        {
          for (int aJderiv = 1; aJderiv < aQderiv; ++aJderiv)
          {
            aScal -= PLib::Bin(aPderiv, aIderiv) * PLib::Bin(aQderiv, aJderiv)
                     * aDerVecNor.Value(aIderiv, aJderiv)
                         .Dot(aDerVecNor.Value(aPderiv - aIderiv, aQderiv - aJderiv));
          }
        }
      }
      aTabScal.SetValue(aPderiv, aQderiv, aScal / 2.0);

      // Compute the derivative (n,p) of NUV Length.
      double aDnorm =
        theDerNUV.Value(aPderiv + theIduref, aQderiv + theIdvref).Dot(aDerVecNor.Value(0, 0));

      for (int aJderiv = 0; aJderiv < aQderiv; ++aJderiv)
      {
        aDnorm -= PLib::Bin(aQderiv + theIdvref, aJderiv + theIdvref)
                  * aTabNorm.Value(aPderiv, aJderiv) * aTabScal.Value(0, aQderiv - aJderiv);
      }

      for (int aIderiv = 0; aIderiv < aPderiv; ++aIderiv)
      {
        for (int aJderiv = 0; aJderiv <= aQderiv; ++aJderiv)
        {
          aDnorm -= PLib::Bin(aPderiv + theIduref, aIderiv + theIduref)
                    * PLib::Bin(aQderiv + theIdvref, aJderiv + theIdvref)
                    * aTabNorm.Value(aIderiv, aJderiv)
                    * aTabScal.Value(aPderiv - aIderiv, aQderiv - aJderiv);
        }
      }
      aTabNorm.SetValue(aPderiv, aQderiv, aDnorm);

      // Compute derivative (p,q) of n.
      aDerNor = theDerNUV.Value(aPderiv + theIduref, aQderiv + theIdvref);

      for (int aJderiv = 1; aJderiv <= aQderiv; ++aJderiv)
      {
        aDerNor.Subtract(PLib::Bin(aPderiv + theIduref, theIduref)
                         * PLib::Bin(aQderiv + theIdvref, aJderiv + theIdvref)
                         * aTabNorm.Value(0, aJderiv)
                         * aDerVecNor.Value(aPderiv, aQderiv - aJderiv));
      }

      for (int aIderiv = 1; aIderiv <= aPderiv; ++aIderiv)
      {
        for (int aJderiv = 0; aJderiv <= aQderiv; ++aJderiv)
        {
          aDerNor.Subtract(PLib::Bin(aPderiv + theIduref, aIderiv + theIduref)
                           * PLib::Bin(aQderiv + theIdvref, aJderiv + theIdvref)
                           * aTabNorm.Value(aIderiv, aJderiv)
                           * aDerVecNor.Value(aPderiv - aIderiv, aQderiv - aJderiv));
        }
      }

      aDerNor.Divide(PLib::Bin(aPderiv + theIduref, theIduref)
                     * PLib::Bin(aQderiv + theIdvref, theIdvref) * aTabNorm.Value(0, 0));
      aDerVecNor.SetValue(aPderiv, aQderiv, aDerNor);
    }
  }

  return aDerVecNor.Value(theNu, theNv);
}
