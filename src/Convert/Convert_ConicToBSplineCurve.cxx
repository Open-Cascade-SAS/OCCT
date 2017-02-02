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

//JCV 16/10/91

#define No_Standard_OutOfRange


#include <BSplCLib.hxx>
#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_CosAndSinEvalFunction.hxx>
#include <Convert_PolynomialCosAndSin.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : Convert_ConicToBSplineCurve
//purpose  : 
//=======================================================================
Convert_ConicToBSplineCurve::Convert_ConicToBSplineCurve 
  (const Standard_Integer NbPoles,
   const Standard_Integer NbKnots,
   const Standard_Integer Degree  ) 
: degree (Degree)    ,  nbPoles (NbPoles)   ,  nbKnots (NbKnots)
  
{ 
  if (NbPoles >= 2) {
    poles    = new TColgp_HArray1OfPnt2d (1, NbPoles)  ;
   
    weights  = new TColStd_HArray1OfReal (1, NbPoles)  ;
  }
  if (NbKnots >= 2) {
    knots    = new TColStd_HArray1OfReal (1, NbKnots)  ;
    mults    = new TColStd_HArray1OfInteger(1,NbKnots) ;
  }
}


//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Convert_ConicToBSplineCurve::Degree () const 
{
  return degree;
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Convert_ConicToBSplineCurve::NbPoles () const 
{
 return nbPoles; 
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Convert_ConicToBSplineCurve::NbKnots () const 
{
  return nbKnots;
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Convert_ConicToBSplineCurve::IsPeriodic() const 
{
  return isperiodic;
}

//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

gp_Pnt2d Convert_ConicToBSplineCurve::Pole 
  (const Standard_Integer Index) const 
{
  if (Index < 1 || Index > nbPoles)
    throw Standard_OutOfRange(" ");
  return poles->Value (Index);
}


//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Convert_ConicToBSplineCurve::Weight 
  (const Standard_Integer Index) const 
{
  if (Index < 1 || Index > nbPoles)
    throw Standard_OutOfRange(" ");
  return weights->Value (Index);
}


//=======================================================================
//function : Knot
//purpose  : 
//=======================================================================

Standard_Real Convert_ConicToBSplineCurve::Knot 
  (const Standard_Integer Index) const 
{
  if (Index < 1 || Index > nbKnots)
    throw Standard_OutOfRange(" ");
  return knots->Value (Index);
}


//=======================================================================
//function : Multiplicity
//purpose  : 
//=======================================================================

Standard_Integer Convert_ConicToBSplineCurve::Multiplicity
  (const Standard_Integer Index) const 
{
  if (Index < 1 || Index > nbKnots)
    throw Standard_OutOfRange(" ");
  return mults->Value (Index);
}
//=======================================================================
//function : CosAndSinRationalC1
//purpose  : evaluates U(t) and V(t) such that
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
//  with is helpfull when having to make a C1 BSpline  by merging two
//  BSpline toghether
//=======================================================================

void CosAndSinRationalC1(Standard_Real Parameter,
			  const Standard_Integer         EvalDegree,
			  const TColgp_Array1OfPnt2d&    EvalPoles,
			  const TColStd_Array1OfReal&    EvalKnots,
			  const TColStd_Array1OfInteger* EvalMults,
			  Standard_Real Result[2]) 
{
 gp_Pnt2d a_point ;
 BSplCLib::D0(Parameter,
	      0,
	      EvalDegree,
	      Standard_False,
	      EvalPoles,
	      BSplCLib::NoWeights(),
	      EvalKnots,
	      EvalMults,
	      a_point) ;
 Result[0] = a_point.Coord(1) ;
 Result[1] = a_point.Coord(2) ;
}
      

//=======================================================================
//function : CosAndSinQuasiAngular
//purpose  : evaluates U(t) and V(t) such that
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

void  CosAndSinQuasiAngular(Standard_Real  Parameter,
			    const Standard_Integer         EvalDegree,
			    const TColgp_Array1OfPnt2d&    EvalPoles,
//			    const TColStd_Array1OfReal&    EvalKnots,
			    const TColStd_Array1OfReal&    ,
//			    const TColStd_Array1OfInteger& EvalMults,
			    const TColStd_Array1OfInteger* ,
			    Standard_Real  Result[2])
{
  Standard_Real 
  param,
  *coeff ;
 
   coeff = (Standard_Real *) &EvalPoles(EvalPoles.Lower()) ;
//
//   rational_function_coeff represent a rational approximation
//   of U ---> cotan( PI * U /2) between [0 1] 
//   rational_function_coeff[i][0] is the denominator
//   rational_function_coeff[i][1] is the numerator
//   
   param = Parameter * 0.5e0 ;
  PLib::NoDerivativeEvalPolynomial (param,
				    EvalDegree,
				    2,
				    EvalDegree << 1,
				    coeff[0],
				    Result[0]) ;
}
   
//=======================================================================
//function : function that build the Bspline Representation of 
// an algorithmic description of the function cos and sin
//purpose  : 
//=======================================================================
void AlgorithmicCosAndSin(Standard_Integer               Degree,
			  const TColStd_Array1OfReal&    FlatKnots,
			  const Standard_Integer         EvalDegree,
			  const TColgp_Array1OfPnt2d&    EvalPoles,
			  const TColStd_Array1OfReal&    EvalKnots,
			  const TColStd_Array1OfInteger* EvalMults,
			  Convert_CosAndSinEvalFunction  Evaluator,
			  TColStd_Array1OfReal&          CosNumerator,
                          TColStd_Array1OfReal&          SinNumerator,
			  TColStd_Array1OfReal&          Denominator) 
{
   Standard_Integer order,
   num_poles,
   pivot_index_problem,
   ii;
 
   Standard_Real  result[2],
   inverse ;

   order = Degree + 1 ;
   num_poles = FlatKnots.Length() - order ;

   if (num_poles != CosNumerator.Length() ||
       num_poles != SinNumerator.Length() ||
       num_poles != Denominator.Length() ) {
      throw Standard_ConstructionError();
      }
   TColStd_Array1OfReal      parameters(1,num_poles)  ;
   TColgp_Array1OfPnt        poles_array(1,num_poles) ;
   TColStd_Array1OfInteger   contact_order_array(1,num_poles) ;  
   BSplCLib::BuildSchoenbergPoints(Degree,
				   FlatKnots,
				   parameters) ;
   for (ii = parameters.Lower() ; ii <= parameters.Upper() ; ii++) {
     Evaluator(parameters(ii),
               EvalDegree,
	       EvalPoles,
	       EvalKnots,
	       EvalMults,
	       result) ;
     contact_order_array(ii) = 0 ;
     
     poles_array(ii).SetCoord(1,
			      (result[1]*result[1] - result[0]*result[0]));
     poles_array(ii).SetCoord(2,
			      2.0e0  * result[1]* result[0]) ;
     poles_array(ii).SetCoord(3,
			      result[1]*result[1] + result[0] * result[0]) ;
   }
    BSplCLib::Interpolate(Degree,
			  FlatKnots,
			  parameters,
			  contact_order_array,
			  poles_array,
			  pivot_index_problem) ;
    for (ii = 1 ; ii <= num_poles ; ii++) {
      inverse = 1.0e0 / poles_array(ii).Coord(3) ;
      CosNumerator(ii) = poles_array(ii).Coord(1) * inverse ;
      SinNumerator(ii) = poles_array(ii).Coord(2) * inverse ;
      Denominator(ii)  = poles_array(ii).Coord(3) ;
    }
 }

//=======================================================================
//function : BuildCosAndSin
//purpose  : 
//=======================================================================

void Convert_ConicToBSplineCurve::BuildCosAndSin(
          const Convert_ParameterisationType     Parameterisation,
	  const Standard_Real 			 UFirst,
          const Standard_Real                    ULast,
	  Handle(TColStd_HArray1OfReal)&         CosNumeratorPtr,
          Handle(TColStd_HArray1OfReal)&         SinNumeratorPtr,
          Handle(TColStd_HArray1OfReal)&         DenominatorPtr,
          Standard_Integer&                      Degree,
          Handle(TColStd_HArray1OfReal)&         KnotsPtr,
          Handle(TColStd_HArray1OfInteger)&      MultsPtr)  const 
{
  Standard_Real delta = ULast - UFirst,
  direct,
  inverse,
  value1,
  value2,
  cos_beta,
  sin_beta,
  alpha=0,
  alpha_2,
  alpha_4,
  tan_alpha_2,
  beta,
  p_param,
  q_param,
  param ;

  Standard_Integer num_poles = 0,
  ii,
  jj,
  num_knots = 1,
  num_spans = 1,
  num_flat_knots,
  num_temp_knots,
  temp_degree = 0,
  tgt_theta_flag,
  num_temp_poles,
  order  = 0;

  Convert_CosAndSinEvalFunction *EvaluatorPtr=NULL ;

  tgt_theta_flag = 0 ;


  switch (Parameterisation) {
  case Convert_TgtThetaOver2: 
    num_spans =
      (Standard_Integer)IntegerPart( 1.2 * delta / M_PI) + 1;
    
    tgt_theta_flag = 1 ;
    break ;
  case Convert_TgtThetaOver2_1:
    num_spans = 1 ;
    if (delta > 0.9999 * M_PI) {
      throw Standard_ConstructionError() ; 
      }
    tgt_theta_flag = 1 ;
    break ;
  case Convert_TgtThetaOver2_2:
    num_spans = 2 ;
    if (delta > 1.9999 * M_PI) {
      throw Standard_ConstructionError() ;
      }
    tgt_theta_flag = 1 ;
    break ;
  
  case Convert_TgtThetaOver2_3:
    num_spans = 3 ;
    tgt_theta_flag = 1 ;
    break ;
  case Convert_TgtThetaOver2_4:
    num_spans = 4 ;
    tgt_theta_flag = 1 ;
    break ; 
  case Convert_QuasiAngular:
    num_poles = 7 ;
    Degree    = 6 ;
    num_spans = 1 ;
    num_knots = 2 ;
    order = Degree + 1 ;
    break ;
   case Convert_RationalC1:
    Degree    = 4 ;
    order = Degree + 1 ; 
    num_poles = 8 ;
    num_knots = 3 ;
    num_spans = 2 ;
    break ;
   case Convert_Polynomial:
    Degree    = 7 ;
    num_poles = 8 ;
    num_knots = 2 ;
    num_spans = 1 ;
    break ;
  default:
    break ;
  }
  if (tgt_theta_flag) {
    alpha = delta / ( 2.0e0 * num_spans) ;    
    Degree = 2 ;
    num_poles = 2 * num_spans + 1;
  }  
  
  CosNumeratorPtr = 
    new TColStd_HArray1OfReal(1,num_poles) ;
  SinNumeratorPtr =
    new TColStd_HArray1OfReal(1,num_poles) ;      
  DenominatorPtr =
    new TColStd_HArray1OfReal(1,num_poles) ;      
  KnotsPtr = 
    new TColStd_HArray1OfReal(1,num_spans+1) ;
  MultsPtr =
    new TColStd_HArray1OfInteger(1,num_spans+1) ;
  if (tgt_theta_flag) {

    param = UFirst ;
    CosNumeratorPtr->SetValue(1,Cos(UFirst)) ;
    SinNumeratorPtr->SetValue(1,Sin(UFirst)) ;
    DenominatorPtr ->SetValue(1,1.0e0) ;
    KnotsPtr->SetValue(1,param) ;
    MultsPtr->SetValue(1,Degree + 1) ;
    direct = Cos(alpha) ;
    inverse = 1.0e0 / direct ;
    for (ii = 1 ; ii <= num_spans ; ii++ ) {
      CosNumeratorPtr->SetValue(2 * ii, inverse * Cos(param + alpha)) ;
      SinNumeratorPtr->SetValue(2 * ii, inverse * Sin(param + alpha)) ;
      DenominatorPtr->SetValue(2 * ii,  direct) ; 
      CosNumeratorPtr->SetValue(2 * ii + 1, Cos(param + 2 * alpha)) ;
      SinNumeratorPtr->SetValue(2 * ii + 1, Sin(param + 2 * alpha)) ;
      DenominatorPtr->SetValue(2 * ii + 1,  1.0e0) ;
      KnotsPtr->SetValue(ii + 1, param + 2 * alpha) ;
      MultsPtr->SetValue(ii + 1, 2) ;
      param += 2 * alpha ;
    }
    MultsPtr->SetValue(num_spans + 1, Degree + 1) ;
  }
  else if (Parameterisation != Convert_Polynomial) {
    alpha = ULast - UFirst ;
    alpha *= 0.5e0 ;
    beta = ULast + UFirst ;
    beta *= 0.5e0 ;
    cos_beta = Cos(beta) ;
    sin_beta = Sin(beta) ;
    num_flat_knots = num_poles + order ;

    num_temp_poles = 4 ;
    num_temp_knots = 3 ;
    TColStd_Array1OfReal   flat_knots(1, num_flat_knots) ;
    

    TColgp_Array1OfPnt2d    temp_poles(1,num_temp_poles)  ;
    TColStd_Array1OfReal    temp_knots(1,num_temp_knots)  ;
    TColStd_Array1OfInteger temp_mults(1,num_temp_knots) ;

    for (ii = 1 ; ii <= order ; ii++) {
      flat_knots(ii) = -alpha ;
      flat_knots(ii + num_poles) = alpha ;
    }
    KnotsPtr->SetValue(1,UFirst) ;
    KnotsPtr->SetValue(num_knots, ULast) ;
    MultsPtr->SetValue(1,order) ;
    MultsPtr->SetValue(num_knots,order) ;
    
    switch (Parameterisation) {
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
     
      alpha_2 = alpha * 0.5e0 ;
      p_param = - 1.0e0 / (alpha_2 * alpha_2) ;
     
      if (alpha_2 <  M_PI * 0.5e0) 
      {
        if (alpha_2 < 1.0e-7)
        {
          // Fixed degenerate case, when obtain 0 / 0 uncertainty.
          // According to Taylor aprroximation:
          // b (gamma) = -6.0 / 15.0 + o(gamma^2)
          p_param = -6.0 / 15.0;
        }
        else
        {
          tan_alpha_2 = Tan(alpha_2) ;
          value1 = 3.0e0 * (tan_alpha_2 - alpha_2) ;
          value1 = alpha_2 / value1 ;
          p_param += value1 ;
        }
      }
      q_param = (1.0e0 / 3.0e0)  + p_param ;
      
      
      temp_degree = 3 ;
      temp_poles(1).SetCoord(1,0.0e0);
      temp_poles(2).SetCoord(1,1.0e0);
      temp_poles(3).SetCoord(1,0.0e0) ;
      temp_poles(4).SetCoord(1,q_param) ;

      temp_poles(1).SetCoord(2, 1.0e0) ;
      temp_poles(2).SetCoord(2, 0.0e0) ;
      temp_poles(3).SetCoord(2, p_param) ;
      temp_poles(4).SetCoord(2, 0.0e0);
      EvaluatorPtr = &CosAndSinQuasiAngular ;
      break ;
    case  Convert_RationalC1:
      for (ii = order + 1 ; ii <= num_poles ; ii++) {
	flat_knots(ii) = 0.0e0 ;
      }
      KnotsPtr->SetValue(2,UFirst + alpha) ;
      MultsPtr->SetValue(2,Degree -1) ;
      temp_degree = 2 ;   
      alpha_2 = alpha * 0.5e0 ;
      alpha_4 = alpha * 0.25e0 ;
      tan_alpha_2 = Tan(alpha_2) ;
      jj = 1 ;
      for (ii = 1 ; ii <= 2 ; ii++) {
	temp_poles(1+ ii).SetCoord(2,1.0e0 + alpha_4 * tan_alpha_2) ;
	temp_poles(jj).SetCoord(2,1.e0) ;
	jj += 3 ;
      }
      temp_poles(1).SetCoord(1,-tan_alpha_2) ;  
      temp_poles(2).SetCoord(1,alpha_4 - tan_alpha_2) ; 
      temp_poles(3).SetCoord(1,-alpha_4 + tan_alpha_2) ; 
      temp_poles(4).SetCoord(1,tan_alpha_2) ; 
      temp_knots(1) = -alpha ;
      temp_knots(2) = 0.0e0 ;
      temp_knots(3) = alpha ;
      temp_mults(1) = temp_degree + 1;
      temp_mults(2) = 1 ;
      temp_mults(3) = temp_degree + 1;
     
      EvaluatorPtr = &CosAndSinRationalC1 ;
      break ;
    default: 
      break ;
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
			 DenominatorPtr->ChangeArray1()) ;

    for (ii = 1 ; ii <= num_poles ; ii++) {
       value1 = cos_beta * CosNumeratorPtr->Value(ii) -
	 sin_beta * SinNumeratorPtr->Value(ii) ;
       value2 = sin_beta * CosNumeratorPtr->Value(ii) +
	 cos_beta * SinNumeratorPtr->Value(ii) ;
       CosNumeratorPtr->SetValue(ii,value1) ;
       SinNumeratorPtr->SetValue(ii,value2) ;
     }
  }
  else { // Convert_Polynomial

    KnotsPtr->SetValue(1, 0.) ;
    KnotsPtr->SetValue(num_knots, 1.);
    MultsPtr->SetValue(1, num_poles);
    MultsPtr->SetValue(num_knots, num_poles);
    
    BuildPolynomialCosAndSin(UFirst,ULast,num_poles,
			     CosNumeratorPtr,SinNumeratorPtr,DenominatorPtr);
  }


}
//=======================================================================
//function : BuildCosAndSin
//purpose  : 
//=======================================================================

void Convert_ConicToBSplineCurve::BuildCosAndSin(
          const Convert_ParameterisationType     Parameterisation,
	  Handle(TColStd_HArray1OfReal)&         CosNumeratorPtr,
          Handle(TColStd_HArray1OfReal)&         SinNumeratorPtr,
          Handle(TColStd_HArray1OfReal)&         DenominatorPtr,
          Standard_Integer&                      Degree,
          Handle(TColStd_HArray1OfReal)&         KnotsPtr,
          Handle(TColStd_HArray1OfInteger)&      MultsPtr)  const 
{
  Standard_Real half_pi,
  param,
  first_param,
  last_param,
//  direct,
  inverse,
  value1,
  value2,
  value3 ;

  Standard_Integer 
  ii,
  jj,
  index,
  num_poles,
  num_periodic_poles,
  temp_degree,
  pivot_index_problem,
  num_flat_knots,
  num_knots,
  order ;

  if (Parameterisation != Convert_TgtThetaOver2 &&
      Parameterisation != Convert_RationalC1) {
      throw Standard_ConstructionError() ;
      }
  Handle(TColStd_HArray1OfReal) temp_cos_ptr,
  temp_sin_ptr,
  temp_denominator_ptr,
  temp_knots_ptr;
  Handle(TColStd_HArray1OfInteger)  temp_mults_ptr;
  if (Parameterisation == Convert_TgtThetaOver2) {
    BuildCosAndSin(Convert_TgtThetaOver2_3,
		   0.0e0,
		   2 * M_PI,
		   temp_cos_ptr,
		   temp_sin_ptr,
		   temp_denominator_ptr,
		   Degree,
		   KnotsPtr,
		   MultsPtr) ;
     CosNumeratorPtr =
       new TColStd_HArray1OfReal(1,temp_cos_ptr->Length() -1) ;
     SinNumeratorPtr =
        new TColStd_HArray1OfReal(1,temp_cos_ptr->Length() -1) ;
     DenominatorPtr =
        new TColStd_HArray1OfReal(1,temp_cos_ptr->Length() -1) ; 
     for (ii = temp_cos_ptr->Lower()  ; ii <= temp_cos_ptr->Upper()-1 ; ii++) {
       CosNumeratorPtr->SetValue(ii,temp_cos_ptr->Value(ii)) ;
       SinNumeratorPtr->SetValue(ii,temp_sin_ptr->Value(ii)) ;
       DenominatorPtr->SetValue(ii,temp_denominator_ptr->Value(ii)) ;
     }
    for (ii = MultsPtr->Lower() ; ii <= MultsPtr->Upper() ; ii++) {
      MultsPtr->SetValue(ii, Degree) ;
    }
  }
  else if (Parameterisation == Convert_RationalC1) 
    {
     first_param = 0.0e0 ;
     last_param  = M_PI ;
     BuildCosAndSin(Convert_RationalC1,
		   first_param,
		   last_param,
		   temp_cos_ptr,
		   temp_sin_ptr,
		   temp_denominator_ptr,
		   temp_degree,
		   temp_knots_ptr,
		   temp_mults_ptr) ;


     Degree = 4 ;
     order = Degree + 1 ;
     num_knots = 5 ;
     num_flat_knots = (Degree -1) * num_knots + 2 * 2 ;
     num_poles = num_flat_knots - order ;
     num_periodic_poles = num_poles - 2 ;
     TColStd_Array1OfReal  flat_knots(1,num_flat_knots) ;
     CosNumeratorPtr = 
      new TColStd_HArray1OfReal(1,num_periodic_poles) ;
     SinNumeratorPtr = 
      new TColStd_HArray1OfReal(1,num_periodic_poles) ;
     DenominatorPtr = 
      new TColStd_HArray1OfReal(1,num_periodic_poles) ;
    
     half_pi = M_PI * 0.5e0 ;
     index = 1 ;
     for (jj = 1 ; jj <= 2 ; jj++) {
	 flat_knots(index) = -  half_pi  ;
         index += 1 ;
       }
     for (ii = 1 ; ii <= num_knots ; ii++) {
       for (jj = 1 ; jj <= Degree -1 ; jj++) {
	 flat_knots(index) = (ii-1) * half_pi ;

	 index += 1 ;

       }
     }
     for (jj = 1 ; jj <= 2 ; jj++) {
	 flat_knots(index) = 2 * M_PI +  half_pi  ;
         index += 1 ;
       }
     KnotsPtr = 
       new TColStd_HArray1OfReal(1,num_knots) ;
     MultsPtr =
       new TColStd_HArray1OfInteger(1,num_knots) ;
     for ( ii = 1 ; ii <= num_knots  ; ii++) { 
       KnotsPtr->SetValue(ii, (ii-1) * half_pi) ;
       MultsPtr->SetValue(ii, Degree-1) ;
     }
     order = degree + 1 ;

     TColStd_Array1OfReal      parameters(1,num_poles)  ;
     TColgp_Array1OfPnt        poles_array(1,num_poles) ;
     TColStd_Array1OfInteger   contact_order_array(1,num_poles) ;  
     BSplCLib::BuildSchoenbergPoints(Degree,
				     flat_knots,
				     parameters) ;
     inverse = 1.0e0 ;
     for (ii = parameters.Lower() ; ii <= parameters.Upper() ; ii++) {
       param = parameters(ii) ;
       if (param > M_PI) {
	 inverse = -1.0e0 ;
         param -= M_PI ;
       }
       BSplCLib::D0(param,
		    0,
		    temp_degree,
		    Standard_False,
		    temp_cos_ptr->Array1(),
		    &temp_denominator_ptr->Array1(),
		    temp_knots_ptr->Array1(),
		    &temp_mults_ptr->Array1(),
		    value1) ;

       BSplCLib::D0(param,
		    0,
		    temp_degree,
		    Standard_False,
		    temp_sin_ptr->Array1(),
		    &temp_denominator_ptr->Array1(),
		    temp_knots_ptr->Array1(),
		    &temp_mults_ptr->Array1(),
		    value2) ;
       BSplCLib::D0(param,
		    0,
		    temp_degree,
		    Standard_False,
		    temp_denominator_ptr->Array1(),
		    BSplCLib::NoWeights(),
		    temp_knots_ptr->Array1(),
		    &temp_mults_ptr->Array1(),
		    value3) ;
     contact_order_array(ii) = 0 ;
     
     poles_array(ii).SetCoord(1,
			      value1 * value3 * inverse) ;
     poles_array(ii).SetCoord(2,
			      value2 * value3 * inverse) ;
     poles_array(ii).SetCoord(3,
			      value3) ;
   }
    BSplCLib::Interpolate(Degree,
			  flat_knots,
			  parameters,
			  contact_order_array,
			  poles_array,
			  pivot_index_problem) ;
    for (ii = 1 ; ii <= num_periodic_poles ; ii++) {
      inverse = 1.0e0 / poles_array(ii).Coord(3) ;
      CosNumeratorPtr->ChangeArray1()(ii) = poles_array(ii).Coord(1) * inverse ;
      SinNumeratorPtr->ChangeArray1()(ii) = poles_array(ii).Coord(2) * inverse ;
      DenominatorPtr->ChangeArray1()(ii)  = poles_array(ii).Coord(3) ;
    }
 }
}


    
