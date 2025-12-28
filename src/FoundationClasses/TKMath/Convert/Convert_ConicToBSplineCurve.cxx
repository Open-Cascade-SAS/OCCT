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

// JCV 16/10/91

#define No_Standard_OutOfRange

#include <BSplCLib.hxx>
#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_CosAndSinEvalFunction.hxx>
#include <Convert_PolynomialCosAndSin.hxx>
#include <gp_Pnt2d.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

//=================================================================================================

Convert_ConicToBSplineCurve::Convert_ConicToBSplineCurve(const int NbPoles,
                                                         const int NbKnots,
                                                         const int Degree)
    : degree(Degree),
      nbPoles(NbPoles),
      nbKnots(NbKnots),
      isperiodic(false)

{
  if (NbPoles >= 2)
  {
    poles = new NCollection_HArray1<gp_Pnt2d>(1, NbPoles);

    weights = new NCollection_HArray1<double>(1, NbPoles);
  }
  if (NbKnots >= 2)
  {
    knots = new NCollection_HArray1<double>(1, NbKnots);
    mults = new NCollection_HArray1<int>(1, NbKnots);
  }
}

//=================================================================================================

int Convert_ConicToBSplineCurve::Degree() const
{
  return degree;
}

//=================================================================================================

int Convert_ConicToBSplineCurve::NbPoles() const
{
  return nbPoles;
}

//=================================================================================================

int Convert_ConicToBSplineCurve::NbKnots() const
{
  return nbKnots;
}

//=================================================================================================

bool Convert_ConicToBSplineCurve::IsPeriodic() const
{
  return isperiodic;
}

//=================================================================================================

gp_Pnt2d Convert_ConicToBSplineCurve::Pole(const int Index) const
{
  if (Index < 1 || Index > nbPoles)
    throw Standard_OutOfRange(" ");
  return poles->Value(Index);
}

//=================================================================================================

double Convert_ConicToBSplineCurve::Weight(const int Index) const
{
  if (Index < 1 || Index > nbPoles)
    throw Standard_OutOfRange(" ");
  return weights->Value(Index);
}

//=================================================================================================

double Convert_ConicToBSplineCurve::Knot(const int Index) const
{
  if (Index < 1 || Index > nbKnots)
    throw Standard_OutOfRange(" ");
  return knots->Value(Index);
}

//=================================================================================================

int Convert_ConicToBSplineCurve::Multiplicity(const int Index) const
{
  if (Index < 1 || Index > nbKnots)
    throw Standard_OutOfRange(" ");
  return mults->Value(Index);
}

//=======================================================================
// function : CosAndSinRationalC1
// purpose  : evaluates U(t) and V(t) such that
//                      2      2
//                     U   -  V
//   cos (theta(t)) = ----------
//                      2      2
//                     U   +  V
//

//                      2 * U*V
//   sin (theta(t)) = ----------
//                      2      2
//                     U   +  V
//                                                    2     2
//  such that the derivative at the domain bounds of U   + V   is 0.0e0
//  with is helpful when having to make a C1 BSpline by merging two BSpline together
//=======================================================================

void CosAndSinRationalC1(double                              Parameter,
                         const int                           EvalDegree,
                         const NCollection_Array1<gp_Pnt2d>& EvalPoles,
                         const NCollection_Array1<double>&   EvalKnots,
                         const NCollection_Array1<int>*      EvalMults,
                         double                              Result[2])
{
  gp_Pnt2d a_point;
  BSplCLib::D0(Parameter,
               0,
               EvalDegree,
               false,
               EvalPoles,
               BSplCLib::NoWeights(),
               EvalKnots,
               EvalMults,
               a_point);
  Result[0] = a_point.Coord(1);
  Result[1] = a_point.Coord(2);
}

//=======================================================================
// function : CosAndSinQuasiAngular
// purpose  : evaluates U(t) and V(t) such that
//                      2      2
//                     U   -  V
//   cos (theta(t)) = ----------
//                      2      2
//                     U   +  V
//

//                      2 * U*V
//   sin (theta(t)) = ----------
//                      2      2
//                     U   +  V
//=======================================================================

void CosAndSinQuasiAngular(double                              Parameter,
                           const int                           EvalDegree,
                           const NCollection_Array1<gp_Pnt2d>& EvalPoles,
                           //			    const NCollection_Array1<double>&    EvalKnots,
                           const NCollection_Array1<double>&,
                           //			    const NCollection_Array1<int>& EvalMults,
                           const NCollection_Array1<int>*,
                           double Result[2])
{
  double param, *coeff;

  coeff = (double*)&EvalPoles(EvalPoles.Lower());
  //
  //   rational_function_coeff represent a rational approximation
  //   of U ---> cotan( PI * U /2) between [0 1]
  //   rational_function_coeff[i][0] is the denominator
  //   rational_function_coeff[i][1] is the numerator
  //
  param = Parameter * 0.5e0;
  PLib::NoDerivativeEvalPolynomial(param, EvalDegree, 2, EvalDegree << 1, coeff[0], Result[0]);
}

//=======================================================================
// function : function that build the Bspline Representation of
// an algorithmic description of the function cos and sin
// purpose  :
//=======================================================================
void AlgorithmicCosAndSin(int                                 Degree,
                          const NCollection_Array1<double>&   FlatKnots,
                          const int                           EvalDegree,
                          const NCollection_Array1<gp_Pnt2d>& EvalPoles,
                          const NCollection_Array1<double>&   EvalKnots,
                          const NCollection_Array1<int>*      EvalMults,
                          Convert_CosAndSinEvalFunction       Evaluator,
                          NCollection_Array1<double>&         CosNumerator,
                          NCollection_Array1<double>&         SinNumerator,
                          NCollection_Array1<double>&         Denominator)
{
  int order, num_poles, pivot_index_problem, ii;

  double result[2], inverse;

  order     = Degree + 1;
  num_poles = FlatKnots.Length() - order;

  if (num_poles != CosNumerator.Length() || num_poles != SinNumerator.Length()
      || num_poles != Denominator.Length())
  {
    throw Standard_ConstructionError();
  }
  NCollection_Array1<double> parameters(1, num_poles);
  NCollection_Array1<gp_Pnt> poles_array(1, num_poles);
  NCollection_Array1<int>    contact_order_array(1, num_poles);
  BSplCLib::BuildSchoenbergPoints(Degree, FlatKnots, parameters);
  for (ii = parameters.Lower(); ii <= parameters.Upper(); ii++)
  {
    Evaluator(parameters(ii), EvalDegree, EvalPoles, EvalKnots, EvalMults, result);
    contact_order_array(ii) = 0;

    poles_array(ii).SetCoord(1, (result[1] * result[1] - result[0] * result[0]));
    poles_array(ii).SetCoord(2, 2.0e0 * result[1] * result[0]);
    poles_array(ii).SetCoord(3, result[1] * result[1] + result[0] * result[0]);
  }
  BSplCLib::Interpolate(Degree,
                        FlatKnots,
                        parameters,
                        contact_order_array,
                        poles_array,
                        pivot_index_problem);
  for (ii = 1; ii <= num_poles; ii++)
  {
    inverse          = 1.0e0 / poles_array(ii).Coord(3);
    CosNumerator(ii) = poles_array(ii).Coord(1) * inverse;
    SinNumerator(ii) = poles_array(ii).Coord(2) * inverse;
    Denominator(ii)  = poles_array(ii).Coord(3);
  }
}

//=================================================================================================

void Convert_ConicToBSplineCurve::BuildCosAndSin(
  const Convert_ParameterisationType        Parameterisation,
  const double                              UFirst,
  const double                              ULast,
  occ::handle<NCollection_HArray1<double>>& CosNumeratorPtr,
  occ::handle<NCollection_HArray1<double>>& SinNumeratorPtr,
  occ::handle<NCollection_HArray1<double>>& DenominatorPtr,
  int&                                      Degree,
  occ::handle<NCollection_HArray1<double>>& KnotsPtr,
  occ::handle<NCollection_HArray1<int>>&    MultsPtr) const
{
  double delta = ULast - UFirst, direct, inverse, value1, value2, cos_beta, sin_beta, alpha = 0,
         alpha_2, alpha_4, tan_alpha_2, beta, p_param, q_param, param;

  int num_poles = 0, ii, jj, num_knots = 1, num_spans = 1, num_flat_knots, num_temp_knots,
      temp_degree = 0, tgt_theta_flag, num_temp_poles, order = 0;

  Convert_CosAndSinEvalFunction* EvaluatorPtr = nullptr;

  tgt_theta_flag = 0;

  switch (Parameterisation)
  {
    case Convert_TgtThetaOver2:
      num_spans = (int)std::trunc(1.2 * delta / M_PI) + 1;

      tgt_theta_flag = 1;
      break;
    case Convert_TgtThetaOver2_1:
      num_spans = 1;
      if (delta > 0.9999 * M_PI)
      {
        throw Standard_ConstructionError();
      }
      tgt_theta_flag = 1;
      break;
    case Convert_TgtThetaOver2_2:
      num_spans = 2;
      if (delta > 1.9999 * M_PI)
      {
        throw Standard_ConstructionError();
      }
      tgt_theta_flag = 1;
      break;

    case Convert_TgtThetaOver2_3:
      num_spans      = 3;
      tgt_theta_flag = 1;
      break;
    case Convert_TgtThetaOver2_4:
      num_spans      = 4;
      tgt_theta_flag = 1;
      break;
    case Convert_QuasiAngular:
      num_poles = 7;
      Degree    = 6;
      num_spans = 1;
      num_knots = 2;
      order     = Degree + 1;
      break;
    case Convert_RationalC1:
      Degree    = 4;
      order     = Degree + 1;
      num_poles = 8;
      num_knots = 3;
      num_spans = 2;
      break;
    case Convert_Polynomial:
      Degree    = 7;
      num_poles = 8;
      num_knots = 2;
      num_spans = 1;
      break;
    default:
      break;
  }
  if (tgt_theta_flag)
  {
    alpha     = delta / (2.0e0 * num_spans);
    Degree    = 2;
    num_poles = 2 * num_spans + 1;
  }

  CosNumeratorPtr = new NCollection_HArray1<double>(1, num_poles);
  SinNumeratorPtr = new NCollection_HArray1<double>(1, num_poles);
  DenominatorPtr  = new NCollection_HArray1<double>(1, num_poles);
  KnotsPtr        = new NCollection_HArray1<double>(1, num_spans + 1);
  MultsPtr        = new NCollection_HArray1<int>(1, num_spans + 1);
  if (tgt_theta_flag)
  {

    param = UFirst;
    CosNumeratorPtr->SetValue(1, std::cos(UFirst));
    SinNumeratorPtr->SetValue(1, std::sin(UFirst));
    DenominatorPtr->SetValue(1, 1.0e0);
    KnotsPtr->SetValue(1, param);
    MultsPtr->SetValue(1, Degree + 1);
    direct  = std::cos(alpha);
    inverse = 1.0e0 / direct;
    for (ii = 1; ii <= num_spans; ii++)
    {
      CosNumeratorPtr->SetValue(2 * ii, inverse * std::cos(param + alpha));
      SinNumeratorPtr->SetValue(2 * ii, inverse * std::sin(param + alpha));
      DenominatorPtr->SetValue(2 * ii, direct);
      CosNumeratorPtr->SetValue(2 * ii + 1, std::cos(param + 2 * alpha));
      SinNumeratorPtr->SetValue(2 * ii + 1, std::sin(param + 2 * alpha));
      DenominatorPtr->SetValue(2 * ii + 1, 1.0e0);
      KnotsPtr->SetValue(ii + 1, param + 2 * alpha);
      MultsPtr->SetValue(ii + 1, 2);
      param += 2 * alpha;
    }
    MultsPtr->SetValue(num_spans + 1, Degree + 1);
  }
  else if (Parameterisation != Convert_Polynomial)
  {
    alpha = ULast - UFirst;
    alpha *= 0.5e0;
    beta = ULast + UFirst;
    beta *= 0.5e0;
    cos_beta       = std::cos(beta);
    sin_beta       = std::sin(beta);
    num_flat_knots = num_poles + order;

    num_temp_poles = 4;
    num_temp_knots = 3;
    NCollection_Array1<double> flat_knots(1, num_flat_knots);

    NCollection_Array1<gp_Pnt2d> temp_poles(1, num_temp_poles);
    NCollection_Array1<double>   temp_knots(1, num_temp_knots);
    NCollection_Array1<int>      temp_mults(1, num_temp_knots);

    for (ii = 1; ii <= order; ii++)
    {
      flat_knots(ii)             = -alpha;
      flat_knots(ii + num_poles) = alpha;
    }
    KnotsPtr->SetValue(1, UFirst);
    KnotsPtr->SetValue(num_knots, ULast);
    MultsPtr->SetValue(1, order);
    MultsPtr->SetValue(num_knots, order);

    switch (Parameterisation)
    {
      case Convert_QuasiAngular:
        //
        //    we code here in temp_poles(xx).Coord(1) the following function V(t)
        //   and in temp_poles(xx).Coord(2) the function U(t)
        //                     3
        //       V(t) = t + c t
        //                     2
        //       U(t) = 1 + b t
        //            1
        //       c = ---  + b   = q_param
        //            3
        //                          3
        //                     gamma
        //            gamma +  ------  - tang gamma
        //                      3
        //       b =------------------------------    = p_param
        //                 2
        //            gamma  (tang gamma - gamma)
        //
        //     with gamma = alpha / 2
        //
        //

        alpha_2 = alpha * 0.5e0;
        p_param = -1.0e0 / (alpha_2 * alpha_2);

        if (alpha_2 < M_PI * 0.5e0)
        {
          if (alpha_2 < 1.0e-7)
          {
            // Fixed degenerate case, when obtain 0 / 0 uncertainty.
            // According to Taylor approximation:
            // b (gamma) = -6.0 / 15.0 + o(gamma^2)
            p_param = -6.0 / 15.0;
          }
          else
          {
            tan_alpha_2 = std::tan(alpha_2);
            value1      = 3.0e0 * (tan_alpha_2 - alpha_2);
            value1      = alpha_2 / value1;
            p_param += value1;
          }
        }
        q_param = (1.0e0 / 3.0e0) + p_param;

        temp_degree = 3;
        temp_poles(1).SetCoord(1, 0.0e0);
        temp_poles(2).SetCoord(1, 1.0e0);
        temp_poles(3).SetCoord(1, 0.0e0);
        temp_poles(4).SetCoord(1, q_param);

        temp_poles(1).SetCoord(2, 1.0e0);
        temp_poles(2).SetCoord(2, 0.0e0);
        temp_poles(3).SetCoord(2, p_param);
        temp_poles(4).SetCoord(2, 0.0e0);
        EvaluatorPtr = &CosAndSinQuasiAngular;
        break;
      case Convert_RationalC1:
        for (ii = order + 1; ii <= num_poles; ii++)
        {
          flat_knots(ii) = 0.0e0;
        }
        KnotsPtr->SetValue(2, UFirst + alpha);
        MultsPtr->SetValue(2, Degree - 1);
        temp_degree = 2;
        alpha_2     = alpha * 0.5e0;
        alpha_4     = alpha * 0.25e0;
        tan_alpha_2 = std::tan(alpha_2);
        jj          = 1;
        for (ii = 1; ii <= 2; ii++)
        {
          temp_poles(1 + ii).SetCoord(2, 1.0e0 + alpha_4 * tan_alpha_2);
          temp_poles(jj).SetCoord(2, 1.e0);
          jj += 3;
        }
        temp_poles(1).SetCoord(1, -tan_alpha_2);
        temp_poles(2).SetCoord(1, alpha_4 - tan_alpha_2);
        temp_poles(3).SetCoord(1, -alpha_4 + tan_alpha_2);
        temp_poles(4).SetCoord(1, tan_alpha_2);
        temp_knots(1) = -alpha;
        temp_knots(2) = 0.0e0;
        temp_knots(3) = alpha;
        temp_mults(1) = temp_degree + 1;
        temp_mults(2) = 1;
        temp_mults(3) = temp_degree + 1;

        EvaluatorPtr = &CosAndSinRationalC1;
        break;
      default:
        break;
    }
    AlgorithmicCosAndSin(Degree,
                         flat_knots,
                         temp_degree,
                         temp_poles,
                         temp_knots,
                         &temp_mults,
                         *EvaluatorPtr,
                         CosNumeratorPtr->ChangeArray1(),
                         SinNumeratorPtr->ChangeArray1(),
                         DenominatorPtr->ChangeArray1());

    for (ii = 1; ii <= num_poles; ii++)
    {
      value1 = cos_beta * CosNumeratorPtr->Value(ii) - sin_beta * SinNumeratorPtr->Value(ii);
      value2 = sin_beta * CosNumeratorPtr->Value(ii) + cos_beta * SinNumeratorPtr->Value(ii);
      CosNumeratorPtr->SetValue(ii, value1);
      SinNumeratorPtr->SetValue(ii, value2);
    }
  }
  else
  { // Convert_Polynomial

    KnotsPtr->SetValue(1, 0.);
    KnotsPtr->SetValue(num_knots, 1.);
    MultsPtr->SetValue(1, num_poles);
    MultsPtr->SetValue(num_knots, num_poles);

    BuildPolynomialCosAndSin(UFirst,
                             ULast,
                             num_poles,
                             CosNumeratorPtr,
                             SinNumeratorPtr,
                             DenominatorPtr);
  }
}

//=================================================================================================

void Convert_ConicToBSplineCurve::BuildCosAndSin(
  const Convert_ParameterisationType        Parameterisation,
  occ::handle<NCollection_HArray1<double>>& CosNumeratorPtr,
  occ::handle<NCollection_HArray1<double>>& SinNumeratorPtr,
  occ::handle<NCollection_HArray1<double>>& DenominatorPtr,
  int&                                      Degree,
  occ::handle<NCollection_HArray1<double>>& KnotsPtr,
  occ::handle<NCollection_HArray1<int>>&    MultsPtr) const
{
  double half_pi, param, first_param, last_param,
    //  direct,
    inverse, value1, value2, value3;

  int ii, jj, index, num_poles, num_periodic_poles, temp_degree, pivot_index_problem,
    num_flat_knots, num_knots;

  if (Parameterisation != Convert_TgtThetaOver2 && Parameterisation != Convert_RationalC1)
  {
    throw Standard_ConstructionError();
  }
  occ::handle<NCollection_HArray1<double>> temp_cos_ptr, temp_sin_ptr, temp_denominator_ptr,
    temp_knots_ptr;
  occ::handle<NCollection_HArray1<int>> temp_mults_ptr;
  if (Parameterisation == Convert_TgtThetaOver2)
  {
    BuildCosAndSin(Convert_TgtThetaOver2_3,
                   0.0e0,
                   2 * M_PI,
                   temp_cos_ptr,
                   temp_sin_ptr,
                   temp_denominator_ptr,
                   Degree,
                   KnotsPtr,
                   MultsPtr);
    CosNumeratorPtr = new NCollection_HArray1<double>(1, temp_cos_ptr->Length() - 1);
    SinNumeratorPtr = new NCollection_HArray1<double>(1, temp_cos_ptr->Length() - 1);
    DenominatorPtr  = new NCollection_HArray1<double>(1, temp_cos_ptr->Length() - 1);
    for (ii = temp_cos_ptr->Lower(); ii <= temp_cos_ptr->Upper() - 1; ii++)
    {
      CosNumeratorPtr->SetValue(ii, temp_cos_ptr->Value(ii));
      SinNumeratorPtr->SetValue(ii, temp_sin_ptr->Value(ii));
      DenominatorPtr->SetValue(ii, temp_denominator_ptr->Value(ii));
    }
    for (ii = MultsPtr->Lower(); ii <= MultsPtr->Upper(); ii++)
    {
      MultsPtr->SetValue(ii, Degree);
    }
  }
  else if (Parameterisation == Convert_RationalC1)
  {
    first_param = 0.0e0;
    last_param  = M_PI;
    BuildCosAndSin(Convert_RationalC1,
                   first_param,
                   last_param,
                   temp_cos_ptr,
                   temp_sin_ptr,
                   temp_denominator_ptr,
                   temp_degree,
                   temp_knots_ptr,
                   temp_mults_ptr);

    Degree             = 4;
    num_knots          = 5;
    num_flat_knots     = (Degree - 1) * num_knots + 2 * 2;
    num_poles          = num_flat_knots - Degree - 1;
    num_periodic_poles = num_poles - 2;
    NCollection_Array1<double> flat_knots(1, num_flat_knots);
    CosNumeratorPtr = new NCollection_HArray1<double>(1, num_periodic_poles);
    SinNumeratorPtr = new NCollection_HArray1<double>(1, num_periodic_poles);
    DenominatorPtr  = new NCollection_HArray1<double>(1, num_periodic_poles);

    half_pi = M_PI * 0.5e0;
    index   = 1;
    for (jj = 1; jj <= 2; jj++)
    {
      flat_knots(index) = -half_pi;
      index += 1;
    }
    for (ii = 1; ii <= num_knots; ii++)
    {
      for (jj = 1; jj <= Degree - 1; jj++)
      {
        flat_knots(index) = (ii - 1) * half_pi;

        index += 1;
      }
    }
    for (jj = 1; jj <= 2; jj++)
    {
      flat_knots(index) = 2 * M_PI + half_pi;
      index += 1;
    }
    KnotsPtr = new NCollection_HArray1<double>(1, num_knots);
    MultsPtr = new NCollection_HArray1<int>(1, num_knots);
    for (ii = 1; ii <= num_knots; ii++)
    {
      KnotsPtr->SetValue(ii, (ii - 1) * half_pi);
      MultsPtr->SetValue(ii, Degree - 1);
    }

    NCollection_Array1<double> parameters(1, num_poles);
    NCollection_Array1<gp_Pnt> poles_array(1, num_poles);
    NCollection_Array1<int>    contact_order_array(1, num_poles);
    BSplCLib::BuildSchoenbergPoints(Degree, flat_knots, parameters);
    inverse = 1.0e0;
    for (ii = parameters.Lower(); ii <= parameters.Upper(); ii++)
    {
      param = parameters(ii);
      if (param > M_PI)
      {
        inverse = -1.0e0;
        param -= M_PI;
      }
      BSplCLib::D0(param,
                   0,
                   temp_degree,
                   false,
                   temp_cos_ptr->Array1(),
                   &temp_denominator_ptr->Array1(),
                   temp_knots_ptr->Array1(),
                   &temp_mults_ptr->Array1(),
                   value1);

      BSplCLib::D0(param,
                   0,
                   temp_degree,
                   false,
                   temp_sin_ptr->Array1(),
                   &temp_denominator_ptr->Array1(),
                   temp_knots_ptr->Array1(),
                   &temp_mults_ptr->Array1(),
                   value2);
      BSplCLib::D0(param,
                   0,
                   temp_degree,
                   false,
                   temp_denominator_ptr->Array1(),
                   BSplCLib::NoWeights(),
                   temp_knots_ptr->Array1(),
                   &temp_mults_ptr->Array1(),
                   value3);
      contact_order_array(ii) = 0;

      poles_array(ii).SetCoord(1, value1 * value3 * inverse);
      poles_array(ii).SetCoord(2, value2 * value3 * inverse);
      poles_array(ii).SetCoord(3, value3);
    }
    BSplCLib::Interpolate(Degree,
                          flat_knots,
                          parameters,
                          contact_order_array,
                          poles_array,
                          pivot_index_problem);
    for (ii = 1; ii <= num_periodic_poles; ii++)
    {
      inverse                             = 1.0e0 / poles_array(ii).Coord(3);
      CosNumeratorPtr->ChangeArray1()(ii) = poles_array(ii).Coord(1) * inverse;
      SinNumeratorPtr->ChangeArray1()(ii) = poles_array(ii).Coord(2) * inverse;
      DenominatorPtr->ChangeArray1()(ii)  = poles_array(ii).Coord(3);
    }
  }
}
