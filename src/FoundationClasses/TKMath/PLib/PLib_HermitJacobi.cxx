// Created on: 1997-10-22
// Created by: Sergey SOKOLOV
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

#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <PLib_HermitJacobi.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_Type.hxx>
#include <math_Matrix.hxx>

namespace
{
// W coefficients for C0 continuity (NivConstr = 0, DegreeH = 1)
// W(t) = (1 - t^2)
constexpr Standard_Real WCoeff_C0[3] = {1.0, 0.0, -1.0};

// W coefficients for C1 continuity (NivConstr = 1, DegreeH = 3)
// W(t) = (1 - t^2)^2 = 1 - 2t^2 + t^4
constexpr Standard_Real WCoeff_C1[5] = {1.0, 0.0, -2.0, 0.0, 1.0};

// W coefficients for C2 continuity (NivConstr = 2, DegreeH = 5)
// W(t) = (1 - t^2)^3 = 1 - 3t^2 + 3t^4 - t^6
constexpr Standard_Real WCoeff_C2[7] = {1.0, 0.0, -3.0, 0.0, 3.0, 0.0, -1.0};

inline const Standard_Real& GetWCoefficients(const Standard_Integer theNivConstr)
{
  switch (theNivConstr)
  {
    case 0:
      return WCoeff_C0[0];
    case 1:
      return WCoeff_C1[0];
    case 2:
      return WCoeff_C2[0];
    default:
      return WCoeff_C0[0]; // Fallback, should never happen
  }
}

const math_Matrix& GetHermiteMatrix_C0()
{
  static math_Matrix aMatrix = []() {
    math_Matrix aResult(1, 2, 1, 2);
    PLib::HermiteCoefficients(-1., 1., 0, 0, aResult);
    return aResult;
  }();
  return aMatrix;
}

const math_Matrix& GetHermiteMatrix_C1()
{
  static math_Matrix aMatrix = []() {
    math_Matrix aResult(1, 4, 1, 4);
    PLib::HermiteCoefficients(-1., 1., 1, 1, aResult);
    return aResult;
  }();
  return aMatrix;
}

const math_Matrix& GetHermiteMatrix_C2()
{
  static math_Matrix aMatrix = []() {
    math_Matrix aResult(1, 6, 1, 6);
    PLib::HermiteCoefficients(-1., 1., 2, 2, aResult);
    return aResult;
  }();
  return aMatrix;
}

inline const math_Matrix& GetHermiteMatrix(const Standard_Integer theNivConstr)
{
  switch (theNivConstr)
  {
    case 0:
      return GetHermiteMatrix_C0();
    case 1:
      return GetHermiteMatrix_C1();
    case 2:
      return GetHermiteMatrix_C2();
    default:
      return GetHermiteMatrix_C0(); // Fallback, should never happen
  }
}
} // namespace

//=================================================================================================

PLib_HermitJacobi::PLib_HermitJacobi(const Standard_Integer WorkDegree,
                                     const GeomAbs_Shape    ConstraintOrder)
    : myJacobi(WorkDegree, ConstraintOrder)
{
}

//=================================================================================================

Standard_Real PLib_HermitJacobi::MaxError(const Standard_Integer Dimension,
                                          Standard_Real&         HermJacCoeff,
                                          const Standard_Integer NewDegree) const
{
  return myJacobi.MaxError(Dimension, HermJacCoeff, NewDegree);
}

//=================================================================================================

void PLib_HermitJacobi::ReduceDegree(const Standard_Integer Dimension,
                                     const Standard_Integer MaxDegree,
                                     const Standard_Real    Tol,
                                     Standard_Real&         HermJacCoeff,
                                     Standard_Integer&      NewDegree,
                                     Standard_Real&         MaxError) const
{
  myJacobi.ReduceDegree(Dimension, MaxDegree, Tol, HermJacCoeff, NewDegree, MaxError);
}

//=================================================================================================

Standard_Real PLib_HermitJacobi::AverageError(const Standard_Integer Dimension,
                                              Standard_Real&         HermJacCoeff,
                                              const Standard_Integer NewDegree) const
{
  return myJacobi.AverageError(Dimension, HermJacCoeff, NewDegree);
}

//=================================================================================================

void PLib_HermitJacobi::ToCoefficients(const Standard_Integer      Dimension,
                                       const Standard_Integer      Degree,
                                       const TColStd_Array1OfReal& HermJacCoeff,
                                       TColStd_Array1OfReal&       Coefficients) const
{
  Standard_Integer i, k, idim, i1, i2;
  Standard_Real    h1, h2;
  Standard_Integer aNivConstr = this->NivConstr(), aDegreeH = 2 * aNivConstr + 1;
  Standard_Integer ibegHJC = HermJacCoeff.Lower(), kdim;

  const math_Matrix& aHermiteMatrix = GetHermiteMatrix(aNivConstr);

  TColStd_Array1OfReal AuxCoeff(0, (Degree + 1) * Dimension - 1);
  AuxCoeff.Init(0.);

  for (k = 0; k <= aDegreeH; k++)
  {
    kdim = k * Dimension;
    for (i = 0; i <= aNivConstr; i++)
    {
      h1 = aHermiteMatrix(i + 1, k + 1);
      h2 = aHermiteMatrix(i + aNivConstr + 2, k + 1);
      i1 = ibegHJC + i * Dimension;
      i2 = ibegHJC + (i + aNivConstr + 1) * Dimension;

      for (idim = 0; idim < Dimension; idim++)
      {
        AuxCoeff(idim + kdim) += HermJacCoeff(i1 + idim) * h1 + HermJacCoeff(i2 + idim) * h2;
      }
    }
  }
  kdim = (Degree + 1) * Dimension;
  for (k = (aDegreeH + 1) * Dimension; k < kdim; k++)
  {
    AuxCoeff(k) = HermJacCoeff(ibegHJC + k);
  }

  if (Degree > aDegreeH)
    myJacobi.ToCoefficients(Dimension, Degree, AuxCoeff, Coefficients);
  else
  {
    Standard_Integer ibegC = Coefficients.Lower();
    kdim                   = (Degree + 1) * Dimension;
    for (k = 0; k < kdim; k++)
      Coefficients(ibegC + k) = AuxCoeff(k);
  }
}

//=======================================================================
// function : D0123
// purpose  : common part of D0,D1,D2,D3 (FORTRAN subroutine MPOBAS)
//=======================================================================

void PLib_HermitJacobi::D0123(const Standard_Integer NDeriv,
                              const Standard_Real    U,
                              TColStd_Array1OfReal&  BasisValue,
                              TColStd_Array1OfReal&  BasisD1,
                              TColStd_Array1OfReal&  BasisD2,
                              TColStd_Array1OfReal&  BasisD3) const
{
  NCollection_LocalArray<Standard_Real> jac0(4 * 20);
  NCollection_LocalArray<Standard_Real> jac1(4 * 20);
  NCollection_LocalArray<Standard_Real> jac2(4 * 20);
  NCollection_LocalArray<Standard_Real> jac3(4 * 20);
  NCollection_LocalArray<Standard_Real> wvalues(4);

  Standard_Integer i, j;
  Standard_Integer aNivConstr = this->NivConstr(), aWorkDegree = this->WorkDegree(),
                   aDegreeH = 2 * aNivConstr + 1;
  Standard_Integer ibeg0 = BasisValue.Lower(), ibeg1 = BasisD1.Lower(), ibeg2 = BasisD2.Lower(),
                   ibeg3      = BasisD3.Lower();
  Standard_Integer aJacDegree = aWorkDegree - aDegreeH - 1;
  Standard_Real    W0;

  const math_Matrix& aHermiteMatrix = GetHermiteMatrix(aNivConstr);

  TColStd_Array1OfReal JacValue0(jac0[0], 0, std::max(0, aJacDegree));
  TColStd_Array1OfReal WValues(wvalues[0], 0, NDeriv);
  WValues.Init(0.);

  // Evaluation des polynomes d'hermite
  math_Matrix HermitValues(0, aDegreeH, 0, NDeriv, 0.);
  if (NDeriv == 0)
    for (i = 0; i <= aDegreeH; i++)
    {
      PLib::NoDerivativeEvalPolynomial(U,
                                       aDegreeH,
                                       1,
                                       aDegreeH,
                                       aHermiteMatrix(i + 1, 1),
                                       HermitValues(i, 0));
    }
  else
    for (i = 0; i <= aDegreeH; i++)
    {
      PLib::EvalPolynomial(U, NDeriv, aDegreeH, 1, aHermiteMatrix(i + 1, 1), HermitValues(i, 0));
    }

  // Evaluation des polynomes de Jaccobi
  if (aJacDegree >= 0)
  {

    switch (NDeriv)
    {
      case 0:
        myJacobi.D0(U, JacValue0);
        break;
      case 1: {
        TColStd_Array1OfReal JacValue1(jac1[0], 0, aJacDegree);
        myJacobi.D1(U, JacValue0, JacValue1);
        break;
      }
      case 2: {
        TColStd_Array1OfReal JacValue1(jac1[0], 0, aJacDegree);
        TColStd_Array1OfReal JacValue2(jac2[0], 0, aJacDegree);
        myJacobi.D2(U, JacValue0, JacValue1, JacValue2);
        break;
      }
      case 3: {
        TColStd_Array1OfReal JacValue1(jac1[0], 0, aJacDegree);
        TColStd_Array1OfReal JacValue2(jac2[0], 0, aJacDegree);
        TColStd_Array1OfReal JacValue3(jac3[0], 0, aJacDegree);
        myJacobi.D3(U, JacValue0, JacValue1, JacValue2, JacValue3);
      }
    }

    // Evaluation de W(t)
    const Standard_Real& aWCoeff = GetWCoefficients(aNivConstr);
    if (NDeriv == 0)
      PLib::NoDerivativeEvalPolynomial(U, aDegreeH + 1, 1, aDegreeH + 1, aWCoeff, WValues(0));
    else
      PLib::EvalPolynomial(U, NDeriv, aDegreeH + 1, 1, aWCoeff, WValues(0));
  }

  // Evaluation a l'ordre 0
  for (i = 0; i <= aDegreeH; i++)
  {
    BasisValue(ibeg0 + i) = HermitValues(i, 0);
  }
  W0 = WValues(0);
  for (i = aDegreeH + 1, j = 0; i <= aWorkDegree; i++, j++)
  {
    BasisValue(ibeg0 + i) = W0 * jac0[j];
  }

  // Evaluation a l'ordre 1
  if (NDeriv >= 1)
  {
    Standard_Real W1 = WValues(1);
    for (i = 0; i <= aDegreeH; i++)
    {
      BasisD1(ibeg1 + i) = HermitValues(i, 1);
    }
    for (i = aDegreeH + 1, j = 0; i <= aWorkDegree; i++, j++)
    {
      BasisD1(ibeg1 + i) = W0 * jac1[j] + W1 * jac0[j];
    }
    // Evaluation a l'ordre 2
    if (NDeriv >= 2)
    {
      Standard_Real W2 = WValues(2);
      for (i = 0; i <= aDegreeH; i++)
      {
        BasisD2(ibeg2 + i) = HermitValues(i, 2);
      }
      for (i = aDegreeH + 1, j = 0; i <= aWorkDegree; i++, j++)
      {
        BasisD2(ibeg2 + i) = W0 * jac2[j] + 2 * W1 * jac1[j] + W2 * jac0[j];
      }

      // Evaluation a l'ordre 3
      if (NDeriv == 3)
      {
        Standard_Real W3 = WValues(3);
        for (i = 0; i <= aDegreeH; i++)
        {
          BasisD3(ibeg3 + i) = HermitValues(i, 3);
        }
        for (i = aDegreeH + 1, j = 0; i <= aWorkDegree; i++, j++)
        {
          BasisD3(ibeg3 + i) = W0 * jac3[j] + W3 * jac0[j] + 3 * (W1 * jac2[j] + W2 * jac1[j]);
        }
      }
    }
  }
}

//=================================================================================================

void PLib_HermitJacobi::D0(const Standard_Real U, TColStd_Array1OfReal& BasisValue) const
{
  D0123(0, U, BasisValue, BasisValue, BasisValue, BasisValue);
}

//=================================================================================================

void PLib_HermitJacobi::D1(const Standard_Real   U,
                           TColStd_Array1OfReal& BasisValue,
                           TColStd_Array1OfReal& BasisD1) const
{
  D0123(1, U, BasisValue, BasisD1, BasisD1, BasisD1);
}

//=================================================================================================

void PLib_HermitJacobi::D2(const Standard_Real   U,
                           TColStd_Array1OfReal& BasisValue,
                           TColStd_Array1OfReal& BasisD1,
                           TColStd_Array1OfReal& BasisD2) const
{
  D0123(2, U, BasisValue, BasisD1, BasisD2, BasisD2);
}

//=================================================================================================

void PLib_HermitJacobi::D3(const Standard_Real   U,
                           TColStd_Array1OfReal& BasisValue,
                           TColStd_Array1OfReal& BasisD1,
                           TColStd_Array1OfReal& BasisD2,
                           TColStd_Array1OfReal& BasisD3) const
{
  D0123(3, U, BasisValue, BasisD1, BasisD2, BasisD3);
}
