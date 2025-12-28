// Copyright (c) 1997-1999 Matra Datavision
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

#include <PLib_JacobiPolynomial.hxx>

#include <math.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>

#include <array>

namespace
{
#include "PLib_JacobiPolynomial_Data.pxx"
#include "PLib_JacobiPolynomial_Coeffs.pxx"

// The possible values for NbGaussPoints
constexpr int THE_NB_GAUSS_POINTS_8 = 8, THE_NB_GAUSS_POINTS_10 = 10,
                           THE_NB_GAUSS_POINTS_15 = 15, THE_NB_GAUSS_POINTS_20 = 20,
                           THE_NB_GAUSS_POINTS_25 = 25, THE_NB_GAUSS_POINTS_30 = 30,
                           THE_NB_GAUSS_POINTS_40 = 40, THE_NB_GAUSS_POINTS_50 = 50,
                           THE_NB_GAUSS_POINTS_61 = 61;

constexpr int THE_INVALID_VALUE = -999;

// Maximum supported polynomial degree
constexpr int THE_MAX_DEGREE = 30;

// Lookup tables for database pointers indexed by myNivConstr (constraint level)
constexpr double const* THE_WEIGHTS_DB[3]    = {WeightsDB_C0, WeightsDB_C1, WeightsDB_C2};
constexpr double const* THE_WEIGHTS_DB0[3]   = {WeightsDB0_C0, WeightsDB0_C1, WeightsDB0_C2};
constexpr double const* THE_MAX_VALUES_DB[3] = {MaxValuesDB_C0,
                                                       MaxValuesDB_C1,
                                                       MaxValuesDB_C2};
constexpr double const* THE_TRANS_MATRIX[3]  = {&TransMatrix_C0[0][0],
                                                       &TransMatrix_C1[0][0],
                                                       &TransMatrix_C2[0][0]};
} // namespace

//=================================================================================================

PLib_JacobiPolynomial::PLib_JacobiPolynomial(const int theWorkDegree,
                                             const GeomAbs_Shape    theConstraintOrder)
    : myWorkDegree(theWorkDegree),
      myNivConstr(PLib::NivConstr(theConstraintOrder)),
      myDegree(theWorkDegree - 2 * (myNivConstr + 1))
{
  if (myDegree < 0)
  {
    throw Standard_ConstructionError("WorkDegree too small for given ConstraintOrder");
  }
  if (myDegree > THE_MAX_DEGREE)
  {
    throw Standard_ConstructionError("Invalid Degree");
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::Points(const int theNbGaussPoints,
                                   NCollection_Array1<double>&  theTabPoints) const
{
  if ((theNbGaussPoints != THE_NB_GAUSS_POINTS_8 && theNbGaussPoints != THE_NB_GAUSS_POINTS_10
       && theNbGaussPoints != THE_NB_GAUSS_POINTS_15 && theNbGaussPoints != THE_NB_GAUSS_POINTS_20
       && theNbGaussPoints != THE_NB_GAUSS_POINTS_25 && theNbGaussPoints != THE_NB_GAUSS_POINTS_30
       && theNbGaussPoints != THE_NB_GAUSS_POINTS_40 && theNbGaussPoints != THE_NB_GAUSS_POINTS_50
       && theNbGaussPoints != THE_NB_GAUSS_POINTS_61)
      || theNbGaussPoints <= myDegree)
  {
    throw Standard_ConstructionError("Invalid NbGaussPoints");
  }

  math_Vector aDecreasingPoints(1, theNbGaussPoints);

  math::GaussPoints(theNbGaussPoints, aDecreasingPoints);

  // theTabPoints consist of only positive increasing values
  for (int i = 1; i <= theNbGaussPoints / 2; i++)
  {
    theTabPoints(i) = aDecreasingPoints(theNbGaussPoints / 2 - i + 1);
  }
  if (theNbGaussPoints % 2 == 1)
  {
    theTabPoints(0) = 0.;
  }
  else
  {
    theTabPoints(0) = THE_INVALID_VALUE;
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::Weights(const int theNbGaussPoints,
                                    NCollection_Array2<double>&  theTabWeights) const
{
  double const*   aDbPointer = THE_WEIGHTS_DB[myNivConstr];
  const int aMinDegree = 2 * (myNivConstr + 1);

  // Calculate offset into the weights database
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_8)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_8 * (THE_NB_GAUSS_POINTS_8 - aMinDegree) / 2);
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_10)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_10 * (THE_NB_GAUSS_POINTS_10 - aMinDegree) / 2);
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_15)
  {
    aDbPointer += (((THE_NB_GAUSS_POINTS_15 - 1) / 2) * (THE_NB_GAUSS_POINTS_15 - aMinDegree));
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_20)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_20 * (THE_NB_GAUSS_POINTS_20 - aMinDegree) / 2);
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_25)
  {
    aDbPointer += (((THE_NB_GAUSS_POINTS_25 - 1) / 2) * (THE_NB_GAUSS_POINTS_25 - aMinDegree));
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_30)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_30 * (THE_NB_GAUSS_POINTS_30 - aMinDegree) / 2);
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_40)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_40 * (THE_NB_GAUSS_POINTS_40 - aMinDegree) / 2);
  }
  if (theNbGaussPoints > THE_NB_GAUSS_POINTS_50)
  {
    aDbPointer += (THE_NB_GAUSS_POINTS_50 * (THE_NB_GAUSS_POINTS_50 - aMinDegree) / 2);
  }

  // Copy TabWeightsDB into theTabWeights (explicit loops for safe 2D array access)
  const int aHalfPoints = theNbGaussPoints / 2;
  for (int j = 0; j <= myDegree; j++)
  {
    for (int i = 1; i <= aHalfPoints; i++)
    {
      theTabWeights.ChangeValue(i, j) = *aDbPointer++;
    }
  }

  if (theNbGaussPoints % 2 == 1)
  {
    // theNbGaussPoints is odd - fill row 0 with special values
    double const* aDbPointer0 = THE_WEIGHTS_DB0[myNivConstr];

    if (theNbGaussPoints > THE_NB_GAUSS_POINTS_15)
    {
      aDbPointer0 += ((THE_NB_GAUSS_POINTS_15 - 1 - aMinDegree) / 2 + 1);
    }
    if (theNbGaussPoints > THE_NB_GAUSS_POINTS_25)
    {
      aDbPointer0 += ((THE_NB_GAUSS_POINTS_25 - 1 - aMinDegree) / 2 + 1);
    }

    // Fill row 0: zeros everywhere (explicit column loop for proper 2D array access)
    for (int j = 0; j <= myDegree; j++)
    {
      theTabWeights.ChangeValue(0, j) = 0.0;
    }

    // Overwrite even columns with data from database
    for (int j = 0; j <= myDegree; j += 2)
    {
      theTabWeights.ChangeValue(0, j) = *aDbPointer0++;
    }
  }
  else
  {
    // Fill row 0 with THE_INVALID_VALUE for even theNbGaussPoints (explicit column loop)
    for (int j = 0; j <= myDegree; j++)
    {
      theTabWeights.ChangeValue(0, j) = THE_INVALID_VALUE;
    }
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::MaxValue(NCollection_Array1<double>& theTabMax) const
{
  double const* aDbPointer = THE_MAX_VALUES_DB[myNivConstr];
  for (int i = theTabMax.Lower(); i <= theTabMax.Upper(); i++)
  {
    theTabMax.ChangeValue(i) = *aDbPointer++;
  }
}

//=================================================================================================

double PLib_JacobiPolynomial::MaxError(const int theDimension,
                                              double&         theJacCoeff,
                                              const int theNewDegree) const
{
  // Buffering on stack to avoid dynamic allocation in this frequently called method
  std::array<double, THE_MAX_DEGREE + 2> aMaxErrBuffer;
  NCollection_Array1<double>                          aTabMax(aMaxErrBuffer.front(), 0, myDegree + 1);
  MaxValue(aTabMax);

  const int aBegIdx = 2 * (myNivConstr + 1);
  const int aCutIdx = std::max(aBegIdx, theNewDegree + 1);

  math_Vector    aMaxErrDim(1, theDimension, 0.);
  double* aJacArray = &theJacCoeff;

  for (int aDimIdx = 1; aDimIdx <= theDimension; aDimIdx++)
  {
    for (int aCoeffIdx = aCutIdx; aCoeffIdx <= myWorkDegree; aCoeffIdx++)
    {
      const double aCoeffValue = aJacArray[aCoeffIdx * theDimension + aDimIdx - 1];
      const double aBasisMax   = aTabMax(aCoeffIdx - aBegIdx);
      aMaxErrDim(aDimIdx) += std::abs(aCoeffValue) * aBasisMax;
    }
  }

  return aMaxErrDim.Norm();
}

//=================================================================================================

void PLib_JacobiPolynomial::ReduceDegree(const int theDimension,
                                         const int theMaxDegree,
                                         const double    theTol,
                                         double&         theJacCoeff,
                                         int&      theNewDegree,
                                         double&         theMaxError) const
{
  const int anIdx   = 2 * (myNivConstr + 1) - 1;
  const int aCutIdx = anIdx + 1;

  math_Vector          aMaxErrDim(1, theDimension, 0.);
  NCollection_Array1<double> aTabMax(0, myDegree + 1);
  MaxValue(aTabMax);

  double* const aJacArray = &theJacCoeff;
  theNewDegree                   = anIdx;
  theMaxError                    = 0.;

  // Search for theNewDegree from high degree to low
  for (int i = myWorkDegree; i >= aCutIdx; i--)
  {
    // Accumulate error contribution for all dimensions
    const int iOffset = i * theDimension;
    for (int idim = 1; idim <= theDimension; idim++)
    {
      aMaxErrDim(idim) += std::abs(aJacArray[iOffset + idim - 1]) * aTabMax(i - aCutIdx);
    }

    const double anError = aMaxErrDim.Norm();
    if (anError > theTol && i <= theMaxDegree)
    {
      theNewDegree = i;
      break;
    }
    theMaxError = anError;
  }

  // Fallback: find last non-negligible coefficient
  if (theNewDegree == anIdx)
  {
    constexpr double anEps = 1.0e-9;
    theNewDegree                  = 0;

    for (int i = anIdx; i >= 1; i--)
    {
      double          aBid    = 0.;
      const int iOffset = i * theDimension;
      for (int idim = 1; idim <= theDimension; idim++)
      {
        aBid += std::abs(aJacArray[iOffset + idim - 1]);
      }
      if (aBid > anEps)
      {
        theNewDegree = i;
        break;
      }
    }
  }
}

//=================================================================================================

double PLib_JacobiPolynomial::AverageError(const int theDimension,
                                                  double&         theJacCoeff,
                                                  const int theNewDegree) const
{
  const int aCutIdx      = std::max(2 * (myNivConstr + 1) + 1, theNewDegree + 1);
  double* const   aJacArray    = &theJacCoeff;
  double          anAverageErr = 0.;

  // Compute sum of squares of coefficients beyond theNewDegree
  for (int idim = 1; idim <= theDimension; idim++)
  {
    for (int i = aCutIdx; i <= myDegree; i++)
    {
      const double aJacCoeff = aJacArray[i * theDimension + idim - 1];
      anAverageErr += aJacCoeff * aJacCoeff;
    }
  }

  return sqrt(anAverageErr / 2.);
}

//=================================================================================================

void PLib_JacobiPolynomial::ToCoefficients(const int      theDimension,
                                           const int      theDegree,
                                           const NCollection_Array1<double>& theJacCoeff,
                                           NCollection_Array1<double>&       theCoefficients) const
{
  constexpr int aMaxM       = THE_MAX_DEGREE + 1;
  const int     aHalfDegree = theDegree / 2;
  const int     aDoubleDim  = 2 * theDimension;
  const int     aBegJC      = theJacCoeff.Lower();
  const int     aBegC       = theCoefficients.Lower();
  double const*       aTrPointer  = THE_TRANS_MATRIX[myNivConstr];

  // Convert even elements of theJacCoeff
  for (int i = 0; i <= aHalfDegree; i++)
  {
    const int iPtrIdx      = i * aMaxM - (i + 1) * i / 2;
    const int iCoeffOffset = aBegC + aDoubleDim * i;

    for (int idim = 1; idim <= theDimension; idim++)
    {
      double aValue = 0.;
      for (int j = i; j <= aHalfDegree; j++)
      {
        aValue += aTrPointer[iPtrIdx + j] * theJacCoeff(aBegJC + aDoubleDim * j + idim - 1);
      }
      theCoefficients(iCoeffOffset + idim - 1) = aValue;
    }
  }

  if (theDegree == 0)
  {
    return;
  }

  // Convert odd elements of theJacCoeff
  aTrPointer += aMaxM * (aMaxM + 1) / 2;
  const int aHalfDegreeMinus1 = (theDegree - 1) / 2;

  for (int i = 0; i <= aHalfDegreeMinus1; i++)
  {
    const int iPtrIdx  = i * aMaxM - (i + 1) * i / 2;
    const int iBaseIdx = aBegC + (2 * i + 1) * theDimension;
    const int jBaseIdx = aBegJC + (2 * i + 1) * theDimension;

    for (int idim = 1; idim <= theDimension; idim++)
    {
      double    aValue = 0.;
      int jj     = jBaseIdx + idim - 1;

      for (int j = i; j <= aHalfDegreeMinus1; j++, jj += aDoubleDim)
      {
        aValue += aTrPointer[iPtrIdx + j] * theJacCoeff(jj);
      }
      theCoefficients(iBaseIdx + idim - 1) = aValue;
    }
  }
}

//=======================================================================
// function : D0123
// purpose  : common part of D0,D1,D2,D3 (FORTRAN subroutine MPOJAC)
//=======================================================================

void PLib_JacobiPolynomial::D0123(const int theNDeriv,
                                  const double    theU,
                                  NCollection_Array1<double>&  theBasisValue,
                                  NCollection_Array1<double>&  theBasisD1,
                                  NCollection_Array1<double>&  theBasisD2,
                                  NCollection_Array1<double>&  theBasisD3) const
{
  const int aHermitNivConstr = 2 * (myNivConstr + 1);
  int       i;
  double          anAux;

  // Get pre-computed coefficients from static cache (zero per-instance overhead!)
  const JacobiCoefficientsCache& aCoeffs = GetJacobiCoefficients(myNivConstr, myDegree);

  //  --- Positionements triviaux -----
  int aBeg0 = theBasisValue.Lower();
  int aBeg1 = theBasisD1.Lower();
  int aBeg2 = theBasisD2.Lower();
  int aBeg3 = theBasisD3.Lower();
  int i0, i1, i2, i3;

  if (myDegree == 0)
  {
    theBasisValue(aBeg0 + 0) = 1.;
    if (theNDeriv >= 1)
    {
      theBasisD1(aBeg1 + 0) = 0.;
      if (theNDeriv >= 2)
      {
        theBasisD2(aBeg2 + 0) = 0.;
        if (theNDeriv == 3)
        {
          theBasisD3(aBeg3 + 0) = 0.;
        }
      }
    }
  }
  else
  {
    theBasisValue(aBeg0 + 0) = 1.;
    anAux                    = aHermitNivConstr + 1;
    theBasisValue(aBeg0 + 1) = anAux * theU;
    if (theNDeriv >= 1)
    {
      theBasisD1(aBeg1 + 0) = 0.;
      theBasisD1(aBeg1 + 1) = anAux;
      if (theNDeriv >= 2)
      {
        theBasisD2(aBeg2 + 0) = 0.;
        theBasisD2(aBeg2 + 1) = 0.;
        if (theNDeriv == 3)
        {
          theBasisD3(aBeg3 + 0) = 0.;
          theBasisD3(aBeg3 + 1) = 0.;
        }
      }
    }
  }

  //  --- Positionement par reccurence
  if (myDegree > 1)
  {
    if (theNDeriv == 0)
    {
      double*       aBV    = &theBasisValue(aBeg0);
      const double* aCofA  = aCoeffs.CofA;
      const double* aCofB  = aCoeffs.CofB;
      const double* aDenom = aCoeffs.Denom;
      for (i = 2; i <= myDegree; i++)
      {
        aBV[i] = (aCofA[i] * theU * aBV[i - 1] + aCofB[i] * aBV[i - 2]) * aDenom[i];
      }
    }

    else
    {
      double aCofA, aCofB, aDenom;
      for (i = 2; i <= myDegree; i++)
      {
        i0     = i + aBeg0;
        i1     = i + aBeg1;
        aCofA  = aCoeffs.CofA[i];
        aCofB  = aCoeffs.CofB[i];
        aDenom = aCoeffs.Denom[i];

        theBasisValue(i0) =
          (aCofA * theU * theBasisValue(i0 - 1) + aCofB * theBasisValue(i0 - 2)) * aDenom;
        theBasisD1(i1) =
          (aCofA * (theU * theBasisD1(i1 - 1) + theBasisValue(i0 - 1)) + aCofB * theBasisD1(i1 - 2))
          * aDenom;
        if (theNDeriv >= 2)
        {
          i2             = i + aBeg2;
          theBasisD2(i2) = (aCofA * (theU * theBasisD2(i2 - 1) + 2 * theBasisD1(i1 - 1))
                            + aCofB * theBasisD2(i2 - 2))
                           * aDenom;
          if (theNDeriv == 3)
          {
            i3             = i + aBeg3;
            theBasisD3(i3) = (aCofA * (theU * theBasisD3(i3 - 1) + 3 * theBasisD2(i2 - 1))
                              + aCofB * theBasisD3(i3 - 2))
                             * aDenom;
          }
        }
      }
    }
  }

  // Normalization
  if (theNDeriv == 0)
  {
    double*       aBV   = &theBasisValue(aBeg0);
    const double* aNorm = aCoeffs.TNorm;
    for (i = 0; i <= myDegree; i++)
    {
      aBV[i] *= aNorm[i];
    }
  }
  else
  {
    const double* aNorm = aCoeffs.TNorm;
    for (i = 0; i <= myDegree; i++)
    {
      const double aNormValue = aNorm[i];
      theBasisValue(i + aBeg0) *= aNormValue;
      theBasisD1(i + aBeg1) *= aNormValue;
      if (theNDeriv >= 2)
      {
        theBasisD2(i + aBeg2) *= aNormValue;
        if (theNDeriv >= 3)
        {
          theBasisD3(i + aBeg3) *= aNormValue;
        }
      }
    }
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::D0(const double theU, NCollection_Array1<double>& theBasisValue) const
{
  D0123(0, theU, theBasisValue, theBasisValue, theBasisValue, theBasisValue);
}

//=================================================================================================

void PLib_JacobiPolynomial::D1(const double   theU,
                               NCollection_Array1<double>& theBasisValue,
                               NCollection_Array1<double>& theBasisD1) const
{
  D0123(1, theU, theBasisValue, theBasisD1, theBasisD1, theBasisD1);
}

//=================================================================================================

void PLib_JacobiPolynomial::D2(const double   theU,
                               NCollection_Array1<double>& theBasisValue,
                               NCollection_Array1<double>& theBasisD1,
                               NCollection_Array1<double>& theBasisD2) const
{
  D0123(2, theU, theBasisValue, theBasisD1, theBasisD2, theBasisD2);
}

//=================================================================================================

void PLib_JacobiPolynomial::D3(const double   theU,
                               NCollection_Array1<double>& theBasisValue,
                               NCollection_Array1<double>& theBasisD1,
                               NCollection_Array1<double>& theBasisD2,
                               NCollection_Array1<double>& theBasisD3) const
{
  D0123(3, theU, theBasisValue, theBasisD1, theBasisD2, theBasisD3);
}
