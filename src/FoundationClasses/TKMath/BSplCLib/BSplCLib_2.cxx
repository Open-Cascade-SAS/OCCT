// Created on: 1995-01-17
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

// xab : modified 15-Mar-95 : added BuildBSplMatrix,FactorBandedMatrix,
//                            EvalBsplineBasis,
//                            EvalPolynomial : Horners method

#include <Standard_Macro.hxx>

#include <iostream>

#include <iomanip>

#include <fstream>

#include <BSplCLib.hxx>
#include <gp_Mat2d.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include "BSplCLib_CurveComputation.pxx"

// Use 1D specialization of the template data container
using BSplCLib_DataContainer = BSplCLib_DataContainer_T<1>;

// methods for 1 dimensional BSplines

//=================================================================================================

void BSplCLib::BuildEval(const int                         Degree,
                         const int                         Index,
                         const NCollection_Array1<double>& Poles,
                         const NCollection_Array1<double>* Weights,
                         double&                           LP)
{
  int    PLower = Poles.Lower();
  int    PUpper = Poles.Upper();
  int    i;
  int    ip = PLower + Index - 1;
  double w, *pole = &LP;
  if (Weights == nullptr)
  {

    for (i = 0; i <= Degree; i++)
    {
      ip++;
      if (ip > PUpper)
        ip = PLower;
      pole[0] = Poles(ip);
      pole += 1;
    }
  }
  else
  {

    for (i = 0; i <= Degree; i++)
    {
      ip++;
      if (ip > PUpper)
        ip = PLower;
      pole[1] = w = (*Weights)(ip);
      pole[0]     = Poles(ip) * w;
      pole += 2;
    }
  }
}

//=================================================================================================

static void PrepareEval(double&                           u,
                        int&                              index,
                        int&                              dim,
                        bool&                             rational,
                        const int                         Degree,
                        const bool                        Periodic,
                        const NCollection_Array1<double>& Poles,
                        const NCollection_Array1<double>* Weights,
                        const NCollection_Array1<double>& Knots,
                        const NCollection_Array1<int>*    Mults,
                        BSplCLib_DataContainer&           dc)
{
  // Set the Index
  BSplCLib::LocateParameter(Degree, Knots, Mults, u, Periodic, index, u);

  // make the knots
  BSplCLib::BuildKnots(Degree, index, Periodic, Knots, Mults, *dc.knots);
  if (Mults == nullptr)
    index -= Knots.Lower() + Degree;
  else
    index = BSplCLib::PoleIndex(Degree, index, Periodic, *Mults);

  // check truly rational
  rational = (Weights != nullptr);
  if (rational)
  {
    int WLower = Weights->Lower() + index;
    rational   = BSplCLib::IsRational(*Weights, WLower, WLower + Degree);
  }

  // make the poles
  if (rational)
  {
    dim = 2;
    BSplCLib::BuildEval(Degree, index, Poles, Weights, *dc.poles);
  }
  else
  {
    dim = 1;
    BSplCLib::BuildEval(Degree, index, Poles, BSplCLib::NoWeights(), *dc.poles);
  }
}

//=================================================================================================

void BSplCLib::D0(const double                      U,
                  const int                         Index,
                  const int                         Degree,
                  const bool                        Periodic,
                  const NCollection_Array1<double>& Poles,
                  const NCollection_Array1<double>* Weights,
                  const NCollection_Array1<double>& Knots,
                  const NCollection_Array1<int>*    Mults,
                  double&                           P)
{
  int    dim, index = Index;
  double u = U;
  bool   rational;
  validateBSplineDegree(Degree);
  BSplCLib_DataContainer dc;
  PrepareEval(u, index, dim, rational, Degree, Periodic, Poles, Weights, Knots, Mults, dc);
  BSplCLib::Eval(u, Degree, *dc.knots, dim, *dc.poles);
  if (rational)
    P = dc.poles[0] / dc.poles[1];
  else
    P = dc.poles[0];
}

//=================================================================================================

void BSplCLib::D1(const double                      U,
                  const int                         Index,
                  const int                         Degree,
                  const bool                        Periodic,
                  const NCollection_Array1<double>& Poles,
                  const NCollection_Array1<double>* Weights,
                  const NCollection_Array1<double>& Knots,
                  const NCollection_Array1<int>*    Mults,
                  double&                           P,
                  double&                           V)
{
  int    dim, index = Index;
  double u = U;
  bool   rational;
  validateBSplineDegree(Degree);
  BSplCLib_DataContainer dc;
  PrepareEval(u, index, dim, rational, Degree, Periodic, Poles, Weights, Knots, Mults, dc);
  BSplCLib::Bohm(u, Degree, 1, *dc.knots, dim, *dc.poles);
  double* result = dc.poles;
  if (rational)
  {
    PLib::RationalDerivative(Degree, 1, 1, *dc.poles, *dc.ders);
    result = dc.ders;
  }
  P = result[0];
  V = result[1];
}

//=================================================================================================

void BSplCLib::D2(const double                      U,
                  const int                         Index,
                  const int                         Degree,
                  const bool                        Periodic,
                  const NCollection_Array1<double>& Poles,
                  const NCollection_Array1<double>* Weights,
                  const NCollection_Array1<double>& Knots,
                  const NCollection_Array1<int>*    Mults,
                  double&                           P,
                  double&                           V1,
                  double&                           V2)
{
  int    dim, index = Index;
  double u = U;
  bool   rational;
  validateBSplineDegree(Degree);
  BSplCLib_DataContainer dc;
  PrepareEval(u, index, dim, rational, Degree, Periodic, Poles, Weights, Knots, Mults, dc);
  BSplCLib::Bohm(u, Degree, 2, *dc.knots, dim, *dc.poles);
  double* result = dc.poles;
  if (rational)
  {
    PLib::RationalDerivative(Degree, 2, 1, *dc.poles, *dc.ders);
    result = dc.ders;
  }
  P  = result[0];
  V1 = result[1];
  if (!rational && (Degree < 2))
    V2 = 0.;
  else
    V2 = result[2];
}

//=================================================================================================

void BSplCLib::D3(const double                      U,
                  const int                         Index,
                  const int                         Degree,
                  const bool                        Periodic,
                  const NCollection_Array1<double>& Poles,
                  const NCollection_Array1<double>* Weights,
                  const NCollection_Array1<double>& Knots,
                  const NCollection_Array1<int>*    Mults,
                  double&                           P,
                  double&                           V1,
                  double&                           V2,
                  double&                           V3)
{
  int    dim, index = Index;
  double u = U;
  bool   rational;
  validateBSplineDegree(Degree);
  BSplCLib_DataContainer dc;
  PrepareEval(u, index, dim, rational, Degree, Periodic, Poles, Weights, Knots, Mults, dc);
  BSplCLib::Bohm(u, Degree, 3, *dc.knots, dim, *dc.poles);
  double* result = dc.poles;
  if (rational)
  {
    PLib::RationalDerivative(Degree, 3, 1, *dc.poles, *dc.ders);
    result = dc.ders;
  }
  P  = result[0];
  V1 = result[1];
  if (!rational && (Degree < 2))
    V2 = 0.;
  else
    V2 = result[2];
  if (!rational && (Degree < 3))
    V3 = 0.;
  else
    V3 = result[3];
}

//=================================================================================================

void BSplCLib::DN(const double                      U,
                  const int                         N,
                  const int                         Index,
                  const int                         Degree,
                  const bool                        Periodic,
                  const NCollection_Array1<double>& Poles,
                  const NCollection_Array1<double>* Weights,
                  const NCollection_Array1<double>& Knots,
                  const NCollection_Array1<int>*    Mults,
                  double&                           VN)
{
  int    dim, index = Index;
  double u = U;
  bool   rational;
  validateBSplineDegree(Degree);
  BSplCLib_DataContainer dc;
  PrepareEval(u, index, dim, rational, Degree, Periodic, Poles, Weights, Knots, Mults, dc);
  BSplCLib::Bohm(u, Degree, N, *dc.knots, dim, *dc.poles);
  if (rational)
  {
    double v;
    PLib::RationalDerivative(Degree, N, 1, *dc.poles, v, false);
    VN = v;
  }
  else
  {
    if (N > Degree)
      VN = 0.;
    else
      VN = dc.poles[N];
  }
}

//=================================================================================================

int BSplCLib::BuildBSpMatrix(const NCollection_Array1<double>& Parameters,
                             const NCollection_Array1<int>&    ContactOrderArray,
                             const NCollection_Array1<double>& FlatKnots,
                             const int                         Degree,
                             math_Matrix&                      Matrix,
                             int&                              UpperBandWidth,
                             int&                              LowerBandWidth)
{
  constexpr int aMaxOrder = BSplCLib::MaxDegree() + 1;
  const int     anOrder   = Degree + 1;
  UpperBandWidth          = Degree;
  LowerBandWidth          = Degree;
  const int aBandWidth    = UpperBandWidth + LowerBandWidth + 1;

  if (Matrix.LowerRow() != Parameters.Lower() || Matrix.UpperRow() != Parameters.Upper()
      || Matrix.LowerCol() != 1 || Matrix.UpperCol() != aBandWidth)
  {
    return 1;
  }

  double      aBasisBuf[aMaxOrder * aMaxOrder];
  math_Matrix aBSplineBasis(aBasisBuf, 1, aMaxOrder, 1, aMaxOrder);

  // Zero the entire matrix once instead of per-row zero-fill loops.
  Matrix.Init(0.0);
  const int aMatLRow  = Matrix.LowerRow();
  const int aMatNCols = Matrix.ColNumber();
  double*   aMatData  = &Matrix(aMatLRow, 1);

  for (int i = Parameters.Lower(); i <= Parameters.Upper(); i++)
  {
    int       aFirstNonZeroIndex = 0;
    const int aContactOrder      = ContactOrderArray(i);
    const int anErrorCode        = BSplCLib::EvalBsplineBasis(aContactOrder,
                                                       anOrder,
                                                       FlatKnots,
                                                       Parameters(i),
                                                       aFirstNonZeroIndex,
                                                       aBSplineBasis);
    if (anErrorCode != 0)
    {
      return 2;
    }

    int           anIndex   = LowerBandWidth + 1 + aFirstNonZeroIndex - i;
    double*       aRowData  = aMatData + (i - aMatLRow) * aMatNCols;
    const double* aBasisSrc = aBasisBuf + aContactOrder * aMaxOrder;
    for (int j = 0; j < anOrder; j++)
    {
      aRowData[anIndex + j - 1] = aBasisSrc[j];
    }
  }

  return 0;
}

//=================================================================================================

int BSplCLib::FactorBandedMatrix(math_Matrix& Matrix,
                                 const int    UpperBandWidth,
                                 const int    LowerBandWidth,
                                 int&         PivotIndexProblem)
{
  const int aBandWidth = UpperBandWidth + LowerBandWidth + 1;
  PivotIndexProblem    = 0;

  const int aLRow  = Matrix.LowerRow();
  const int aNCols = Matrix.ColNumber();
  double*   aData  = &Matrix(aLRow, 1);

  for (int i = aLRow + 1; i <= Matrix.UpperRow(); i++)
  {
    const int aMinIndex = (LowerBandWidth - i + 2 >= 1 ? LowerBandWidth - i + 2 : 1);
    double*   aRowI     = aData + (i - aLRow) * aNCols;

    for (int j = aMinIndex; j <= LowerBandWidth; j++)
    {
      const int     anIndex = i - LowerBandWidth + j - 1;
      const double* aRowIdx = aData + (anIndex - aLRow) * aNCols;
      const double  aPivot  = aRowIdx[LowerBandWidth];
      if (std::abs(aPivot) <= RealSmall())
      {
        PivotIndexProblem = anIndex;
        return 1;
      }

      const double anInverse = -1.0 / aPivot;
      aRowI[j - 1]           = aRowI[j - 1] * anInverse;
      const int aMaxIndex    = aBandWidth + anIndex - i;

      for (int k = j + 1; k <= aMaxIndex; k++)
      {
        aRowI[k - 1] += aRowI[j - 1] * aRowIdx[k + i - anIndex - 1];
      }
    }
  }

  return 0;
}

//=================================================================================================

int BSplCLib::EvalBsplineBasis(const int                         DerivativeRequest,
                               const int                         Order,
                               const NCollection_Array1<double>& FlatKnots,
                               const double                      Parameter,
                               int&                              FirstNonZeroBsplineIndex,
                               math_Matrix&                      BsplineBasis,
                               bool                              isPeriodic)
{
  // the matrix must have at least DerivativeRequest + 1
  //   row and Order columns
  // the result are stored in the following way in
  // the Bspline matrix
  // Let i be the FirstNonZeroBsplineIndex and
  // t be the parameter value, k the order of the
  // knot vector, r the DerivativeRequest :
  //
  //   B (t)   B (t)                     B (t)
  //    i       i+1                       i+k-1
  //
  //    (1)     (1)                       (1)
  //   B (t)   B (t)                     B (t)
  //    i       i+1                       i+k-1
  //
  //
  //
  //
  //    (r)     (r)                       (r)
  //   B (t)   B (t)                     B (t)
  //    i       i+1                       i+k-1
  //
  FirstNonZeroBsplineIndex = 0;
  int aLocalRequest        = DerivativeRequest;
  if (DerivativeRequest >= Order)
  {
    aLocalRequest = Order - 1;
  }

  if (BsplineBasis.LowerCol() != 1 || BsplineBasis.UpperCol() < Order
      || BsplineBasis.LowerRow() != 1 || BsplineBasis.UpperRow() <= aLocalRequest)
  {
    return 1;
  }

  const int aNumPoles = FlatKnots.Upper() - FlatKnots.Lower() + 1 - Order;
  int       ii        = 0;
  double    aNewParam = 0.0;
  BSplCLib::LocateParameter(Order - 1,
                            FlatKnots,
                            Parameter,
                            isPeriodic,
                            Order,
                            aNumPoles + 1,
                            ii,
                            aNewParam);

  FirstNonZeroBsplineIndex = ii - Order + 1;

  // Use raw pointers for BsplineBasis and FlatKnots to bypass accessor overhead
  // (math_Matrix::Value -> math_DoubleTab::Value -> NCollection_Array2::Value ->
  // NCollection_Array1::at with bounds checks and DYLD stubs in tight loops).
  const int        aBasisNCols = BsplineBasis.ColNumber();
  double*          aBasisData  = &BsplineBasis(1, 1);
  const double*    aKnotsData  = &FlatKnots(FlatKnots.Lower());
  constexpr double aResolution = gp::Resolution();
  ii -= FlatKnots.Lower(); // rebase to zero-based indexing into aKnotsData

  aBasisData[0] = 1.0;
  aLocalRequest = DerivativeRequest;
  if (DerivativeRequest >= Order)
  {
    aLocalRequest = Order - 1;
  }

  for (int qq = 2; qq <= Order - aLocalRequest; qq++)
  {
    aBasisData[qq - 1] = 0.0;

    for (int pp = 1; pp <= qq - 1; pp++)
    {
      const double aScale = aKnotsData[ii + pp] - aKnotsData[ii - qq + pp + 1];
      if (std::abs(aScale) < aResolution)
      {
        return 2;
      }

      const double aFactor = (Parameter - aKnotsData[ii - qq + pp + 1]) / aScale;
      const double aSaved  = aFactor * aBasisData[pp - 1];
      aBasisData[pp - 1] *= (1.0 - aFactor);
      aBasisData[pp - 1] += aBasisData[qq - 1];
      aBasisData[qq - 1] = aSaved;
    }
  }

  for (int qq = Order - aLocalRequest + 1; qq <= Order; qq++)
  {
    for (int pp = 1; pp <= qq - 1; pp++)
    {
      aBasisData[(Order - qq + 1) * aBasisNCols + (pp - 1)] = aBasisData[pp - 1];
    }
    aBasisData[qq - 1] = 0.0;

    for (int ss = Order - aLocalRequest + 1; ss <= qq; ss++)
    {
      aBasisData[(Order - ss + 1) * aBasisNCols + (qq - 1)] = 0.0;
    }

    for (int pp = 1; pp <= qq - 1; pp++)
    {
      const double aScale = aKnotsData[ii + pp] - aKnotsData[ii - qq + pp + 1];
      if (std::abs(aScale) < aResolution)
      {
        return 2;
      }

      const double anInverse = 1.0 / aScale;
      const double aFactor   = (Parameter - aKnotsData[ii - qq + pp + 1]) * anInverse;
      double       aSaved    = aFactor * aBasisData[pp - 1];
      aBasisData[pp - 1] *= (1.0 - aFactor);
      aBasisData[pp - 1] += aBasisData[qq - 1];
      aBasisData[qq - 1]         = aSaved;
      const double aLocalInverse = static_cast<double>(qq - 1) * anInverse;

      for (int ss = Order - aLocalRequest + 1; ss <= qq; ss++)
      {
        double* aRowS = aBasisData + (Order - ss + 1) * aBasisNCols;
        aSaved        = aLocalInverse * aRowS[pp - 1];
        aRowS[pp - 1] *= -aLocalInverse;
        aRowS[pp - 1] += aRowS[qq - 1];
        aRowS[qq - 1] = aSaved;
      }
    }
  }

  return 0;
}

//=================================================================================================

void BSplCLib::MovePointAndTangent(const double                      U,
                                   const int                         ArrayDimension,
                                   double&                           Delta,
                                   double&                           DeltaDerivatives,
                                   const double                      Tolerance,
                                   const int                         Degree,
                                   const int                         StartingCondition,
                                   const int                         EndingCondition,
                                   double&                           Poles,
                                   const NCollection_Array1<double>* Weights,
                                   const NCollection_Array1<double>& FlatKnots,
                                   double&                           NewPoles,
                                   int&                              ErrorStatus)
{
  int num_poles, num_knots, ii, jj, conditions, start_num_poles, end_num_poles, index, start_index,
    end_index, other_index, type, order;

  double new_parameter, value, divide, end_value, start_value, *poles_array, *new_poles_array,
    *delta_array, *derivatives_array, *weights_array;

  ErrorStatus   = 0;
  weights_array = nullptr;
  if (Weights != nullptr)
  {
    weights_array = const_cast<double*>(&Weights->First());
  }

  poles_array       = &Poles;
  new_poles_array   = &NewPoles;
  delta_array       = &Delta;
  derivatives_array = &DeltaDerivatives;
  order             = Degree + 1;
  num_knots         = FlatKnots.Length();
  num_poles         = num_knots - order;
  conditions        = StartingCondition + EndingCondition + 4;
  //
  // check validity of input data
  //
  if (StartingCondition >= -1 && StartingCondition <= Degree && EndingCondition >= -1
      && EndingCondition <= Degree && conditions <= num_poles)
  {
    //
    // check the parameter is within bounds
    //
    start_index = FlatKnots.Lower() + Degree;
    end_index   = FlatKnots.Upper() - Degree;
    //
    //  check if there is enough room to move the poles
    //
    conditions = 1;
    if (StartingCondition == -1)
    {
      conditions = conditions && (FlatKnots(start_index) <= U);
    }
    else
    {
      conditions = conditions && (FlatKnots(start_index) + Tolerance < U);
    }
    if (EndingCondition == -1)
    {
      conditions = conditions && (FlatKnots(end_index) >= U);
    }
    else
    {
      conditions = conditions && (FlatKnots(end_index) - Tolerance > U);
    }

    if (conditions)
    {
      //
      // build 2 auxiliary functions
      //
      NCollection_Array1<double> schoenberg_points(1, num_poles);
      NCollection_Array1<double> first_function(1, num_poles);
      NCollection_Array1<double> second_function(1, num_poles);

      BuildSchoenbergPoints(Degree, FlatKnots, schoenberg_points);
      start_index = StartingCondition + 2;
      end_index   = num_poles - EndingCondition - 1;
      LocateParameter(schoenberg_points,
                      U,
                      false,
                      start_index,
                      end_index,
                      index,
                      new_parameter,
                      0,
                      1);

      if (index == start_index)
      {
        other_index = index + 1;
      }
      else if (index == end_index)
      {
        other_index = index - 1;
      }
      else if (U - FlatKnots(index) < FlatKnots(index + 1) - U)
      {
        other_index = index - 1;
      }
      else
      {
        other_index = index + 1;
      }
      type = 3;

      start_num_poles = StartingCondition + 2;
      end_num_poles   = num_poles - EndingCondition - 1;
      if (start_num_poles == 1)
      {
        start_value = schoenberg_points(num_poles) - schoenberg_points(1);
        start_value = schoenberg_points(1) - start_value;
      }
      else
      {
        start_value = schoenberg_points(start_num_poles - 1);
      }
      if (end_num_poles == num_poles)
      {
        end_value = schoenberg_points(num_poles) - schoenberg_points(1);
        end_value = schoenberg_points(num_poles) + end_value;
      }
      else
      {
        end_value = schoenberg_points(end_num_poles + 1);
      }

      for (ii = 1; ii < start_num_poles; ii++)
      {
        first_function(ii)  = 0.e0;
        second_function(ii) = 0.0e0;
      }

      for (ii = end_num_poles + 1; ii <= num_poles; ii++)
      {
        first_function(ii)  = 0.e0;
        second_function(ii) = 0.0e0;
      }
      divide = 1.0e0 / (schoenberg_points(index) - start_value);

      for (ii = start_num_poles; ii <= index; ii++)
      {
        value = schoenberg_points(ii) - start_value;
        value *= divide;
        first_function(ii) = 1.0e0;

        for (jj = 0; jj < type; jj++)
        {
          first_function(ii) *= value;
        }
      }
      divide = 1.0e0 / (end_value - schoenberg_points(index));

      for (ii = index; ii <= end_num_poles; ii++)
      {
        value = end_value - schoenberg_points(ii);
        value *= divide;
        first_function(ii) = 1.0e0;

        for (jj = 0; jj < type; jj++)
        {
          first_function(ii) *= value;
        }
      }

      divide = 1.0e0 / (schoenberg_points(other_index) - start_value);

      for (ii = start_num_poles; ii <= other_index; ii++)
      {
        value = schoenberg_points(ii) - start_value;
        value *= divide;
        second_function(ii) = 1.0e0;

        for (jj = 0; jj < type; jj++)
        {
          second_function(ii) *= value;
        }
      }
      divide = 1.0e0 / (end_value - schoenberg_points(other_index));

      for (ii = other_index; ii <= end_num_poles; ii++)
      {
        value = end_value - schoenberg_points(ii);
        value *= divide;
        second_function(ii) = 1.0e0;

        for (jj = 0; jj < type; jj++)
        {
          second_function(ii) *= value;
        }
      }

      //
      //  compute the point and derivatives of both functions
      //
      double results[2][2], weights_results[2][2];
      int    extrap_mode[2], derivative_request = 1, dimension = 1;
      bool   periodic_flag = false;

      extrap_mode[0] = Degree;
      extrap_mode[1] = Degree;
      if (Weights != nullptr)
      {
        //
        // evaluate in homogenised form
        //
        Eval(U,
             periodic_flag,
             derivative_request,
             extrap_mode[0],
             Degree,
             FlatKnots,
             dimension,
             first_function(1),
             weights_array[0],
             results[0][0],
             weights_results[0][0]);

        Eval(U,
             periodic_flag,
             derivative_request,
             extrap_mode[0],
             Degree,
             FlatKnots,
             dimension,
             second_function(1),
             weights_array[0],
             results[1][0],
             weights_results[1][0]);
        //
        //  compute the rational derivatives values
        //

        for (ii = 0; ii < 2; ii++)
        {
          PLib::RationalDerivatives(1, 1, results[ii][0], weights_results[ii][0], results[ii][0]);
        }
      }
      else
      {
        Eval(U, false, 1, extrap_mode[0], Degree, FlatKnots, 1, first_function(1), results[0][0]);

        Eval(U, false, 1, extrap_mode[0], Degree, FlatKnots, 1, second_function(1), results[1][0]);
      }
      gp_Mat2d a_matrix;

      for (ii = 0; ii < 2; ii++)
      {

        for (jj = 0; jj < 2; jj++)
        {
          a_matrix.SetValue(ii + 1, jj + 1, results[ii][jj]);
        }
      }
      a_matrix.Invert();
      // Use math_Vector for stack allocation (ArrayDimension is typically 2-4)
      math_Vector the_a_vector(0, ArrayDimension - 1);
      math_Vector the_b_vector(0, ArrayDimension - 1);

      for (ii = 0; ii < ArrayDimension; ii++)
      {
        the_a_vector(ii) =
          a_matrix.Value(1, 1) * delta_array[ii] + a_matrix.Value(2, 1) * derivatives_array[ii];
        the_b_vector(ii) =
          a_matrix.Value(1, 2) * delta_array[ii] + a_matrix.Value(2, 2) * derivatives_array[ii];
      }
      index = 0;

      for (ii = 0; ii < num_poles; ii++)
      {

        for (jj = 0; jj < ArrayDimension; jj++)
        {
          new_poles_array[index] = poles_array[index];
          new_poles_array[index] += first_function(ii + 1) * the_a_vector(jj);
          new_poles_array[index] += second_function(ii + 1) * the_b_vector(jj);
          index += 1;
        }
      }
    }
    else
    {
      ErrorStatus = 1;
    }
  }
  else
  {
    ErrorStatus = 2;
  }
}

//=================================================================================================

void BSplCLib::FunctionMultiply(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                const int                         BSplineDegree,
                                const NCollection_Array1<double>& BSplineFlatKnots,
                                const int                         PolesDimension,
                                double&                           Poles,
                                const NCollection_Array1<double>& FlatKnots,
                                const int                         NewDegree,
                                double&                           NewPoles,
                                int&                              theStatus)
{
  double*   anArrayOfPoles = &NewPoles;
  const int aNumNewPoles   = FlatKnots.Length() - NewDegree - 1;
  double    aStartEnd[2]   = {FlatKnots(NewDegree + 1), FlatKnots(aNumNewPoles + 1)};

  NCollection_Array1<double> aParameters(1, aNumNewPoles);
  NCollection_Array1<int>    aContactOrderArray(1, aNumNewPoles);
  NCollection_Array1<double> aNewPolesArray(1, aNumNewPoles * PolesDimension);

  double* anArrayOfNewPoles = &aNewPolesArray(1);
  BuildSchoenbergPoints(NewDegree, FlatKnots, aParameters);

  if (aParameters(1) < aStartEnd[0])
  {
    aParameters(1) = aStartEnd[0];
  }
  if (aParameters(aNumNewPoles) > aStartEnd[1])
  {
    aParameters(aNumNewPoles) = aStartEnd[1];
  }

  int anExtrapMode = BSplineDegree;
  int anIndex      = 0;
  for (int i = 1; i <= aNumNewPoles; i++)
  {
    aContactOrderArray(i) = 0;
    double aResult        = 0.0;
    int    anErrorCode    = 0;
    FunctionPtr.Evaluate(aContactOrderArray(i), aStartEnd, aParameters(i), aResult, anErrorCode);
    if (anErrorCode)
    {
      theStatus = 1;
      return;
    }

    Eval(aParameters(i),
         false,
         0,
         anExtrapMode,
         BSplineDegree,
         BSplineFlatKnots,
         PolesDimension,
         Poles,
         anArrayOfNewPoles[anIndex]);

    for (int j = 0; j < PolesDimension; j++)
    {
      anArrayOfNewPoles[anIndex] *= aResult;
      anIndex += 1;
    }
  }

  Interpolate(NewDegree,
              FlatKnots,
              aParameters,
              aContactOrderArray,
              PolesDimension,
              anArrayOfNewPoles[0],
              theStatus);

  for (int i = 0; i < aNumNewPoles * PolesDimension; i++)
  {
    anArrayOfPoles[i] = anArrayOfNewPoles[i];
  }
}

//=================================================================================================

void BSplCLib::FunctionReparameterise(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                      const int                         BSplineDegree,
                                      const NCollection_Array1<double>& BSplineFlatKnots,
                                      const int                         PolesDimension,
                                      double&                           Poles,
                                      const NCollection_Array1<double>& FlatKnots,
                                      const int                         NewDegree,
                                      double&                           NewPoles,
                                      int&                              theStatus)
{
  double*   anArrayOfPoles = &NewPoles;
  const int aNumNewPoles   = FlatKnots.Length() - NewDegree - 1;
  double    aStartEnd[2]   = {FlatKnots(NewDegree + 1), FlatKnots(aNumNewPoles + 1)};

  NCollection_Array1<double> aParameters(1, aNumNewPoles);
  NCollection_Array1<int>    aContactOrderArray(1, aNumNewPoles);
  NCollection_Array1<double> aNewPolesArray(1, aNumNewPoles * PolesDimension);

  double* anArrayOfNewPoles = &aNewPolesArray(1);
  BuildSchoenbergPoints(NewDegree, FlatKnots, aParameters);

  int anExtrapMode = BSplineDegree;
  int anIndex      = 0;
  for (int i = 1; i <= aNumNewPoles; i++)
  {
    aContactOrderArray(i) = 0;
    double aResult        = 0.0;
    int    anErrorCode    = 0;
    FunctionPtr.Evaluate(aContactOrderArray(i), aStartEnd, aParameters(i), aResult, anErrorCode);
    if (anErrorCode)
    {
      theStatus = 1;
      return;
    }

    Eval(aResult,
         false,
         0,
         anExtrapMode,
         BSplineDegree,
         BSplineFlatKnots,
         PolesDimension,
         Poles,
         anArrayOfNewPoles[anIndex]);
    anIndex += PolesDimension;
  }

  Interpolate(NewDegree,
              FlatKnots,
              aParameters,
              aContactOrderArray,
              PolesDimension,
              anArrayOfNewPoles[0],
              theStatus);

  for (int i = 0; i < aNumNewPoles * PolesDimension; i++)
  {
    anArrayOfPoles[i] = anArrayOfNewPoles[i];
  }
}

//=================================================================================================

void BSplCLib::FunctionMultiply(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                const int                         BSplineDegree,
                                const NCollection_Array1<double>& BSplineFlatKnots,
                                const NCollection_Array1<double>& Poles,
                                const NCollection_Array1<double>& FlatKnots,
                                const int                         NewDegree,
                                NCollection_Array1<double>&       NewPoles,
                                int&                              theStatus)
{
  int num_bspline_poles = BSplineFlatKnots.Length() - BSplineDegree - 1;
  int num_new_poles     = FlatKnots.Length() - NewDegree - 1;

  if (Poles.Length() != num_bspline_poles || NewPoles.Length() != num_new_poles)
  {
    throw Standard_ConstructionError();
  }
  double* array_of_poles     = (double*)&Poles(Poles.Lower());
  double* array_of_new_poles = (double*)&NewPoles(NewPoles.Lower());
  BSplCLib::FunctionMultiply(FunctionPtr,
                             BSplineDegree,
                             BSplineFlatKnots,
                             1,
                             array_of_poles[0],
                             FlatKnots,
                             NewDegree,
                             array_of_new_poles[0],
                             theStatus);
}

//=================================================================================================

void BSplCLib::FunctionReparameterise(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                      const int                         BSplineDegree,
                                      const NCollection_Array1<double>& BSplineFlatKnots,
                                      const NCollection_Array1<double>& Poles,
                                      const NCollection_Array1<double>& FlatKnots,
                                      const int                         NewDegree,
                                      NCollection_Array1<double>&       NewPoles,
                                      int&                              theStatus)
{
  int num_bspline_poles = BSplineFlatKnots.Length() - BSplineDegree - 1;
  int num_new_poles     = FlatKnots.Length() - NewDegree - 1;

  if (Poles.Length() != num_bspline_poles || NewPoles.Length() != num_new_poles)
  {
    throw Standard_ConstructionError();
  }
  double* array_of_poles     = (double*)&Poles(Poles.Lower());
  double* array_of_new_poles = (double*)&NewPoles(NewPoles.Lower());
  BSplCLib::FunctionReparameterise(FunctionPtr,
                                   BSplineDegree,
                                   BSplineFlatKnots,
                                   1,
                                   array_of_poles[0],
                                   FlatKnots,
                                   NewDegree,
                                   array_of_new_poles[0],
                                   theStatus);
}

//=================================================================================================

void BSplCLib::MergeBSplineKnots(const double                              Tolerance,
                                 const double                              StartValue,
                                 const double                              EndValue,
                                 const int                                 Degree1,
                                 const NCollection_Array1<double>&         Knots1,
                                 const NCollection_Array1<int>&            Mults1,
                                 const int                                 Degree2,
                                 const NCollection_Array1<double>&         Knots2,
                                 const NCollection_Array1<int>&            Mults2,
                                 int&                                      NumPoles,
                                 occ::handle<NCollection_HArray1<double>>& NewKnots,
                                 occ::handle<NCollection_HArray1<int>>&    NewMults)
{
  int ii, jj, continuity, set_mults_flag, degree, index, num_knots;
  if (StartValue < EndValue - Tolerance)
  {
    NCollection_Array1<double> knots1(1, Knots1.Length());
    NCollection_Array1<double> knots2(1, Knots2.Length());
    degree = Degree1 + Degree2;
    index  = 1;

    for (ii = Knots1.Lower(); ii <= Knots1.Upper(); ii++)
    {
      knots1(index) = Knots1(ii);
      index += 1;
    }
    index = 1;

    for (ii = Knots2.Lower(); ii <= Knots2.Upper(); ii++)
    {
      knots2(index) = Knots2(ii);
      index += 1;
    }
    BSplCLib::Reparametrize(StartValue, EndValue, knots1);

    BSplCLib::Reparametrize(StartValue, EndValue, knots2);
    num_knots = 0;
    jj        = 1;

    for (ii = 1; ii <= knots1.Length(); ii++)
    {

      while (jj <= knots2.Length() && knots2(jj) <= knots1(ii) - Tolerance)
      {
        jj += 1;
        num_knots += 1;
      }

      while (jj <= knots2.Length() && knots2(jj) <= knots1(ii) + Tolerance)
      {
        jj += 1;
      }
      num_knots += 1;
    }
    NewKnots  = new NCollection_HArray1<double>(1, num_knots);
    NewMults  = new NCollection_HArray1<int>(1, num_knots);
    num_knots = 1;
    jj        = 1;

    for (ii = 1; ii <= knots1.Length(); ii++)
    {

      while (jj <= knots2.Length() && knots2(jj) <= knots1(ii) - Tolerance)
      {
        NewKnots->ChangeArray1()(num_knots) = knots2(jj);
        NewMults->ChangeArray1()(num_knots) = Mults2(jj) + Degree1;
        jj += 1;
        num_knots += 1;
      }
      set_mults_flag = 0;

      while (jj <= knots2.Length() && knots2(jj) <= knots1(ii) + Tolerance)
      {
        continuity                          = std::min(Degree1 - Mults1(ii), Degree2 - Mults2(jj));
        set_mults_flag                      = 1;
        NewMults->ChangeArray1()(num_knots) = degree - continuity;
        jj += 1;
      }

      NewKnots->ChangeArray1()(num_knots) = knots1(ii);
      if (!set_mults_flag)
      {
        NewMults->ChangeArray1()(num_knots) = Mults1(ii) + Degree2;
      }
      num_knots += 1;
    }
    num_knots -= 1;
    NewMults->ChangeArray1()(1)         = degree + 1;
    NewMults->ChangeArray1()(num_knots) = degree + 1;
    index                               = 0;

    for (ii = 1; ii <= num_knots; ii++)
    {
      index += NewMults->Value(ii);
    }
    NumPoles = index - degree - 1;
  }
  else
  {
    degree                              = Degree1 + Degree2;
    num_knots                           = 2;
    NewKnots                            = new NCollection_HArray1<double>(1, num_knots);
    NewKnots->ChangeArray1()(1)         = StartValue;
    NewKnots->ChangeArray1()(num_knots) = EndValue;

    NewMults                            = new NCollection_HArray1<int>(1, num_knots);
    NewMults->ChangeArray1()(1)         = degree + 1;
    NewMults->ChangeArray1()(num_knots) = degree + 1;
    NumPoles                            = BSplCLib::NbPoles(degree, false, NewMults->Array1());
  }
}
