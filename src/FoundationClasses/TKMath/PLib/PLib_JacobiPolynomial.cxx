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

#include <math.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <algorithm>
#include <cstring>

IMPLEMENT_STANDARD_RTTIEXT(PLib_JacobiPolynomial, PLib_Base)

namespace
{
#include "PLib_JacobiPolynomial_Data.pxx"
#include "PLib_JacobiPolynomial_Coeffs.pxx"

// The possible values for NbGaussPoints
const Standard_Integer NDEG8 = 8, NDEG10 = 10, NDEG15 = 15, NDEG20 = 20, NDEG25 = 25, NDEG30 = 30,
                       NDEG40 = 40, NDEG50 = 50, NDEG61 = 61;

const Standard_Integer UNDEFINED = -999;
} // namespace

//=================================================================================================

PLib_JacobiPolynomial::PLib_JacobiPolynomial(const Standard_Integer WorkDegree,
                                             const GeomAbs_Shape    ConstraintOrder)
    : myWorkDegree(WorkDegree),
      myNivConstr(ConstraintOrder == GeomAbs_C0   ? 0
                  : ConstraintOrder == GeomAbs_C1 ? 1
                  : ConstraintOrder == GeomAbs_C2
                    ? 2
                    : throw Standard_ConstructionError("Invalid ConstraintOrder")),
      myDegree(WorkDegree - 2 * (myNivConstr + 1))
{
  if (myDegree > 30)
    throw Standard_ConstructionError("Invalid Degree");
}

//=================================================================================================

void PLib_JacobiPolynomial::Points(const Standard_Integer NbGaussPoints,
                                   TColStd_Array1OfReal&  TabPoints) const
{
  if ((NbGaussPoints != NDEG8 && NbGaussPoints != NDEG10 && NbGaussPoints != NDEG15
       && NbGaussPoints != NDEG20 && NbGaussPoints != NDEG25 && NbGaussPoints != NDEG30
       && NbGaussPoints != NDEG40 && NbGaussPoints != NDEG50 && NbGaussPoints != NDEG61)
      || NbGaussPoints <= myDegree)
    throw Standard_ConstructionError("Invalid NbGaussPoints");

  math_Vector DecreasingPoints(1, NbGaussPoints);

  math::GaussPoints(NbGaussPoints, DecreasingPoints);

  // TabPoints consist of only positive increasing values
  for (Standard_Integer i = 1; i <= NbGaussPoints / 2; i++)
    TabPoints(i) = DecreasingPoints(NbGaussPoints / 2 - i + 1);
  if (NbGaussPoints % 2 == 1)
    TabPoints(0) = 0.;
  else
    TabPoints(0) = UNDEFINED;
}

//=================================================================================================

void PLib_JacobiPolynomial::Weights(const Standard_Integer NbGaussPoints,
                                    TColStd_Array2OfReal&  TabWeights) const
{
  // Lookup tables for database pointers indexed by myNivConstr
  static constexpr Standard_Real const* WeightsDB[3]  = {WeightsDB_C0, WeightsDB_C1, WeightsDB_C2};
  static constexpr Standard_Real const* WeightsDB0[3] = {WeightsDB0_C0,
                                                         WeightsDB0_C1,
                                                         WeightsDB0_C2};

  Standard_Real const*   pdb   = WeightsDB[myNivConstr];
  const Standard_Integer infdg = 2 * (myNivConstr + 1);

  // Calculate offset into the weights database
  if (NbGaussPoints > NDEG8)
    pdb += (NDEG8 * (NDEG8 - infdg) / 2);
  if (NbGaussPoints > NDEG10)
    pdb += (NDEG10 * (NDEG10 - infdg) / 2);
  if (NbGaussPoints > NDEG15)
    pdb += (((NDEG15 - 1) / 2) * (NDEG15 - infdg));
  if (NbGaussPoints > NDEG20)
    pdb += (NDEG20 * (NDEG20 - infdg) / 2);
  if (NbGaussPoints > NDEG25)
    pdb += (((NDEG25 - 1) / 2) * (NDEG25 - infdg));
  if (NbGaussPoints > NDEG30)
    pdb += (NDEG30 * (NDEG30 - infdg) / 2);
  if (NbGaussPoints > NDEG40)
    pdb += (NDEG40 * (NDEG40 - infdg) / 2);
  if (NbGaussPoints > NDEG50)
    pdb += (NDEG50 * (NDEG50 - infdg) / 2);

  // Copy TabWeightsDB into TabWeights using memcpy per column
  const Standard_Integer aHalfPoints = NbGaussPoints / 2;
  for (Standard_Integer j = 0; j <= myDegree; j++)
  {
    std::memcpy(&TabWeights.ChangeValue(1, j), pdb, aHalfPoints * sizeof(Standard_Real));
    pdb += aHalfPoints;
  }

  if (NbGaussPoints % 2 == 1)
  {
    // NbGaussPoints is odd - fill row 0 with special values
    Standard_Real const* pdb0 = WeightsDB0[myNivConstr];

    if (NbGaussPoints > NDEG15)
      pdb0 += ((NDEG15 - 1 - infdg) / 2 + 1);
    if (NbGaussPoints > NDEG25)
      pdb0 += ((NDEG25 - 1 - infdg) / 2 + 1);

    // Fill row 0: zeros everywhere, then overwrite even indices with data
    std::fill_n(&TabWeights.ChangeValue(0, 0), myDegree + 1, 0.);
    for (Standard_Integer j = 0; j <= myDegree; j += 2)
    {
      TabWeights.ChangeValue(0, j) = *pdb0++;
    }
  }
  else
  {
    // Fill row 0 with UNDEFINED for even NbGaussPoints
    std::fill_n(&TabWeights.ChangeValue(0, 0), myDegree + 1, UNDEFINED);
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::MaxValue(TColStd_Array1OfReal& TabMax) const
{
  // Lookup table for database pointers indexed by myNivConstr
  static constexpr Standard_Real const* MaxValuesDB[3] = {MaxValuesDB_C0,
                                                          MaxValuesDB_C1,
                                                          MaxValuesDB_C2};

  const Standard_Integer aSize = TabMax.Upper() - TabMax.Lower() + 1;
  std::memcpy(&TabMax.ChangeFirst(), MaxValuesDB[myNivConstr], aSize * sizeof(Standard_Real));
}

//=================================================================================================

Standard_Real PLib_JacobiPolynomial::MaxError(const Standard_Integer Dimension,
                                              Standard_Real&         JacCoeff,
                                              const Standard_Integer NewDegree) const
{
  Standard_Integer i, idim, ibeg, icut;

  math_Vector MaxErrDim(1, Dimension, 0.);

  TColStd_Array1OfReal TabMax(0, myDegree + 1);
  MaxValue(TabMax);

  ibeg                    = 2 * (myNivConstr + 1);
  icut                    = Max(ibeg, NewDegree + 1);
  Standard_Real* JacArray = &JacCoeff;
  for (idim = 1; idim <= Dimension; idim++)
  {
    for (i = icut; i <= myWorkDegree; i++)
    {
      MaxErrDim(idim) += Abs(JacArray[i * Dimension + idim - 1]) * TabMax(i - ibeg);
    }
  }
  Standard_Real MaxErr = MaxErrDim.Norm();
  return (MaxErr);
}

//=================================================================================================

void PLib_JacobiPolynomial::ReduceDegree(const Standard_Integer Dimension,
                                         const Standard_Integer MaxDegree,
                                         const Standard_Real    Tol,
                                         Standard_Real&         JacCoeff,
                                         Standard_Integer&      NewDegree,
                                         Standard_Real&         MaxError) const
{
  const Standard_Integer ia   = 2 * (myNivConstr + 1) - 1;
  const Standard_Integer icut = ia + 1;

  math_Vector          MaxErrDim(1, Dimension, 0.);
  TColStd_Array1OfReal TabMax(0, myDegree + 1);
  MaxValue(TabMax);

  Standard_Real* const JacArray = &JacCoeff;
  NewDegree                     = ia;
  MaxError                      = 0.;

  // Search for NewDegree from high degree to low
  for (Standard_Integer i = myWorkDegree; i >= icut; i--)
  {
    // Accumulate error contribution for all dimensions
    const Standard_Integer iOffset = i * Dimension;
    for (Standard_Integer idim = 1; idim <= Dimension; idim++)
    {
      MaxErrDim(idim) += Abs(JacArray[iOffset + idim - 1]) * TabMax(i - icut);
    }

    const Standard_Real Error = MaxErrDim.Norm();
    if (Error > Tol && i <= MaxDegree)
    {
      NewDegree = i;
      break;
    }
    MaxError = Error;
  }

  // Fallback: find last non-negligible coefficient
  if (NewDegree == ia)
  {
    constexpr Standard_Real Eps1 = 1.0e-9;
    NewDegree                    = 0;

    for (Standard_Integer i = ia; i >= 1; i--)
    {
      Standard_Real          Bid     = 0.;
      const Standard_Integer iOffset = i * Dimension;
      for (Standard_Integer idim = 1; idim <= Dimension; idim++)
      {
        Bid += Abs(JacArray[iOffset + idim - 1]);
      }
      if (Bid > Eps1)
      {
        NewDegree = i;
        break;
      }
    }
  }
}

//=================================================================================================

Standard_Real PLib_JacobiPolynomial::AverageError(const Standard_Integer Dimension,
                                                  Standard_Real&         JacCoeff,
                                                  const Standard_Integer NewDegree) const
{
  const Standard_Integer icut       = Max(2 * (myNivConstr + 1) + 1, NewDegree + 1);
  Standard_Real* const   JacArray   = &JacCoeff;
  Standard_Real          AverageErr = 0.;

  // Compute sum of squares of coefficients beyond NewDegree
  for (Standard_Integer idim = 1; idim <= Dimension; idim++)
  {
    for (Standard_Integer i = icut; i <= myDegree; i++)
    {
      const Standard_Real BidJ = JacArray[i * Dimension + idim - 1];
      AverageErr += BidJ * BidJ;
    }
  }

  return sqrt(AverageErr / 2.);
}

//=================================================================================================

void PLib_JacobiPolynomial::ToCoefficients(const Standard_Integer      Dimension,
                                           const Standard_Integer      Degree,
                                           const TColStd_Array1OfReal& JacCoeff,
                                           TColStd_Array1OfReal&       Coefficients) const
{
  // Lookup table for transformation matrix pointers indexed by myNivConstr
  static constexpr Standard_Real const* TransMatrix[3] = {&TransMatrix_C0[0][0],
                                                          &TransMatrix_C1[0][0],
                                                          &TransMatrix_C2[0][0]};

  constexpr Standard_Integer MAXM       = 31;
  const Standard_Integer     HalfDegree = Degree / 2;
  const Standard_Integer     DoubleDim  = 2 * Dimension;
  const Standard_Integer     ibegJC     = JacCoeff.Lower();
  const Standard_Integer     ibegC      = Coefficients.Lower();
  Standard_Real const*       pTr        = TransMatrix[myNivConstr];

  // Convert even elements of JacCoeff
  for (Standard_Integer i = 0; i <= HalfDegree; i++)
  {
    const Standard_Integer iptt         = i * MAXM - (i + 1) * i / 2;
    const Standard_Integer iCoeffOffset = DoubleDim * i;

    for (Standard_Integer idim = 1; idim <= Dimension; idim++)
    {
      Standard_Real Bid = 0.;
      for (Standard_Integer j = i; j <= HalfDegree; j++)
      {
        Bid += pTr[iptt + j] * JacCoeff(DoubleDim * j + idim - 1);
      }
      Coefficients(iCoeffOffset + idim - 1) = Bid;
    }
  }

  if (Degree == 0)
    return;

  // Convert odd elements of JacCoeff
  pTr += MAXM * (MAXM + 1) / 2;
  const Standard_Integer HalfDegreeMinus1 = (Degree - 1) / 2;

  for (Standard_Integer i = 0; i <= HalfDegreeMinus1; i++)
  {
    const Standard_Integer iptt     = i * MAXM - (i + 1) * i / 2;
    const Standard_Integer iBaseIdx = ibegC + (2 * i + 1) * Dimension;
    const Standard_Integer jBaseIdx = ibegJC + (2 * i + 1) * Dimension;

    for (Standard_Integer idim = 1; idim <= Dimension; idim++)
    {
      Standard_Real    Bid = 0.;
      Standard_Integer jj  = jBaseIdx + idim - 1;

      for (Standard_Integer j = i; j <= HalfDegreeMinus1; j++, jj += DoubleDim)
      {
        Bid += pTr[iptt + j] * JacCoeff(jj);
      }
      Coefficients(iBaseIdx + idim - 1) = Bid;
    }
  }
}

//=======================================================================
// function : D0123
// purpose  : common part of D0,D1,D2,D3 (FORTRAN subroutine MPOJAC)
//=======================================================================

void PLib_JacobiPolynomial::D0123(const Standard_Integer NDeriv,
                                  const Standard_Real    U,
                                  TColStd_Array1OfReal&  BasisValue,
                                  TColStd_Array1OfReal&  BasisD1,
                                  TColStd_Array1OfReal&  BasisD2,
                                  TColStd_Array1OfReal&  BasisD3)
{
  const Standard_Integer HermitNivConstr = 2 * (myNivConstr + 1);
  Standard_Integer       i;
  Standard_Real          Aux1;

  // Get pre-computed coefficients from static cache (zero per-instance overhead!)
  const JacobiCoefficientsCache& aCoeffs = GetJacobiCoefficients(myNivConstr, myDegree);

  //  --- Positionements triviaux -----
  Standard_Integer ibeg0 = BasisValue.Lower();
  Standard_Integer ibeg1 = BasisD1.Lower();
  Standard_Integer ibeg2 = BasisD2.Lower();
  Standard_Integer ibeg3 = BasisD3.Lower();
  Standard_Integer i0, i1, i2, i3;

  if (myDegree == 0)
  {
    BasisValue(ibeg0 + 0) = 1.;
    if (NDeriv >= 1)
    {
      BasisD1(ibeg1 + 0) = 0.;
      if (NDeriv >= 2)
      {
        BasisD2(ibeg2 + 0) = 0.;
        if (NDeriv == 3)
          BasisD3(ibeg3 + 0) = 0.;
      }
    }
  }
  else
  {
    BasisValue(ibeg0 + 0) = 1.;
    Aux1                  = HermitNivConstr + 1;
    BasisValue(ibeg0 + 1) = Aux1 * U;
    if (NDeriv >= 1)
    {
      BasisD1(ibeg1 + 0) = 0.;
      BasisD1(ibeg1 + 1) = Aux1;
      if (NDeriv >= 2)
      {
        BasisD2(ibeg2 + 0) = 0.;
        BasisD2(ibeg2 + 1) = 0.;
        if (NDeriv == 3)
        {
          BasisD3(ibeg3 + 0) = 0.;
          BasisD3(ibeg3 + 1) = 0.;
        }
      }
    }
  }

  //  --- Positionement par reccurence
  if (myDegree > 1)
  {
    if (NDeriv == 0)
    {
      Standard_Real*       BV    = &BasisValue(ibeg0);
      const Standard_Real* CofA  = aCoeffs.CofA;
      const Standard_Real* CofB  = aCoeffs.CofB;
      const Standard_Real* Denom = aCoeffs.Denom;
      for (i = 2; i <= myDegree; i++)
      {
        BV[i] = (CofA[i] * U * BV[i - 1] + CofB[i] * BV[i - 2]) * Denom[i];
      }
    }

    else
    {
      Standard_Real CofA, CofB, Denom;
      for (i = 2; i <= myDegree; i++)
      {
        i0    = i + ibeg0;
        i1    = i + ibeg1;
        CofA  = aCoeffs.CofA[i];
        CofB  = aCoeffs.CofB[i];
        Denom = aCoeffs.Denom[i];

        BasisValue(i0) = (CofA * U * BasisValue(i0 - 1) + CofB * BasisValue(i0 - 2)) * Denom;
        BasisD1(i1) =
          (CofA * (U * BasisD1(i1 - 1) + BasisValue(i0 - 1)) + CofB * BasisD1(i1 - 2)) * Denom;
        if (NDeriv >= 2)
        {
          i2 = i + ibeg2;
          BasisD2(i2) =
            (CofA * (U * BasisD2(i2 - 1) + 2 * BasisD1(i1 - 1)) + CofB * BasisD2(i2 - 2)) * Denom;
          if (NDeriv == 3)
          {
            i3 = i + ibeg3;
            BasisD3(i3) =
              (CofA * (U * BasisD3(i3 - 1) + 3 * BasisD2(i2 - 1)) + CofB * BasisD3(i3 - 2)) * Denom;
          }
        }
      }
    }
  }

  // Normalization
  if (NDeriv == 0)
  {
    Standard_Real*       BV    = &BasisValue(ibeg0);
    const Standard_Real* TNorm = aCoeffs.TNorm;
    for (i = 0; i <= myDegree; i++)
      BV[i] *= TNorm[i];
  }
  else
  {
    const Standard_Real* TNorm = aCoeffs.TNorm;
    for (i = 0; i <= myDegree; i++)
    {
      const Standard_Real aNorm = TNorm[i];
      BasisValue(i + ibeg0) *= aNorm;
      BasisD1(i + ibeg1) *= aNorm;
      if (NDeriv >= 2)
      {
        BasisD2(i + ibeg2) *= aNorm;
        if (NDeriv >= 3)
          BasisD3(i + ibeg3) *= aNorm;
      }
    }
  }
}

//=================================================================================================

void PLib_JacobiPolynomial::D0(const Standard_Real U, TColStd_Array1OfReal& BasisValue)
{
  D0123(0, U, BasisValue, BasisValue, BasisValue, BasisValue);
}

//=================================================================================================

void PLib_JacobiPolynomial::D1(const Standard_Real   U,
                               TColStd_Array1OfReal& BasisValue,
                               TColStd_Array1OfReal& BasisD1)
{
  D0123(1, U, BasisValue, BasisD1, BasisD1, BasisD1);
}

//=================================================================================================

void PLib_JacobiPolynomial::D2(const Standard_Real   U,
                               TColStd_Array1OfReal& BasisValue,
                               TColStd_Array1OfReal& BasisD1,
                               TColStd_Array1OfReal& BasisD2)
{
  D0123(2, U, BasisValue, BasisD1, BasisD2, BasisD2);
}

//=================================================================================================

void PLib_JacobiPolynomial::D3(const Standard_Real   U,
                               TColStd_Array1OfReal& BasisValue,
                               TColStd_Array1OfReal& BasisD1,
                               TColStd_Array1OfReal& BasisD2,
                               TColStd_Array1OfReal& BasisD3)
{
  D0123(3, U, BasisValue, BasisD1, BasisD2, BasisD3);
}
