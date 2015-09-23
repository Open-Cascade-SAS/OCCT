// Created on: 1995-08-28
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

#ifndef _PLib_HeaderFile
#define _PLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAbs_Shape.hxx>
class math_Matrix;
class PLib_Base;
class PLib_JacobiPolynomial;
class PLib_HermitJacobi;
class PLib_DoubleJacobiPolynomial;


//! PLib means Polynomial  functions library.  This pk
//! provides  basic       computation    functions for
//! polynomial functions.
//! Note: weight arrays can be passed by pointer for
//! some functions so that NULL pointer is valid.
//! That means no weights passed.
class PLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Used as argument for a non rational functions
  inline static TColStd_Array1OfReal* NoWeights()
  {
    return NULL;
  }
  
  //! Used as argument for a non rational functions
  inline static TColStd_Array2OfReal* NoWeights2()
  {
    return NULL;
  }

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles (const TColgp_Array1OfPnt& Poles, TColStd_Array1OfReal& FP);
  
  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles (const TColgp_Array1OfPnt& Poles, const TColStd_Array1OfReal& Weights, TColStd_Array1OfReal& FP);
  
  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles (const TColStd_Array1OfReal& FP, TColgp_Array1OfPnt& Poles);
  
  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles (const TColStd_Array1OfReal& FP, TColgp_Array1OfPnt& Poles, TColStd_Array1OfReal& Weights);
  
  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles (const TColgp_Array1OfPnt2d& Poles, TColStd_Array1OfReal& FP);
  
  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles (const TColgp_Array1OfPnt2d& Poles, const TColStd_Array1OfReal& Weights, TColStd_Array1OfReal& FP);
  
  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles (const TColStd_Array1OfReal& FP, TColgp_Array1OfPnt2d& Poles);
  
  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles (const TColStd_Array1OfReal& FP, TColgp_Array1OfPnt2d& Poles, TColStd_Array1OfReal& Weights);
  
  //! Returns the Binomial Cnp. N should be <= BSplCLib::MaxDegree().
  Standard_EXPORT static Standard_Real Bin (const Standard_Integer N, const Standard_Integer P);
  
  //! Computes the derivatives of a ratio at order
  //! <N> in dimension <Dimension>.
  //!
  //! <Ders> is an  array containing the  values  of the
  //! input   derivatives from  0 to  Min(<N>,<Degree>).
  //! For   orders   higher  than <Degree>    the  inputcd /s2d1/BMDL/
  //! derivatives   are assumed to be  0.
  //!
  //! Content of <Ders> :
  //!
  //! x(1),x(2),...,x(Dimension),w
  //! x'(1),x'(2),...,x'(Dimension),w'
  //! x''(1),x''(2),...,x''(Dimension),w''
  //!
  //! If  <All> is false, only the   derivative at order
  //! <N> is computed.  <RDers>  is an array   of length
  //! Dimension which will contain the result :
  //!
  //! x(1)/w , x(2)/w ,  ... derivated <N> times
  //!
  //! If <All> is  true all the  derivatives up to order
  //! <N> are computed.   <RDers> is an array of  length
  //! Dimension * (N+1) which will contains :
  //!
  //! x(1)/w , x(2)/w ,  ...
  //! x(1)/w , x(2)/w ,  ... derivated <1> times
  //! x(1)/w , x(2)/w ,  ... derivated <2> times
  //! ...
  //! x(1)/w , x(2)/w ,  ... derivated <N> times
  //!
  //! Warning: <RDers> must be dimensionned properly.
  Standard_EXPORT static void RationalDerivative (const Standard_Integer Degree, const Standard_Integer N, const Standard_Integer Dimension, Standard_Real& Ders, Standard_Real& RDers, const Standard_Boolean All = Standard_True);
  
  //! Computes DerivativesRequest derivatives of a ratio at
  //! of a BSpline function of degree <Degree>
  //! dimension <Dimension>.
  //!
  //! <PolesDerivatives> is an  array containing the  values
  //! of the input   derivatives from  0 to  <DerivativeRequest>
  //! For   orders   higher  than <Degree>    the  input
  //! derivatives   are assumed to be  0.
  //!
  //! Content of <PoleasDerivatives> :
  //!
  //! x(1),x(2),...,x(Dimension)
  //! x'(1),x'(2),...,x'(Dimension)
  //! x''(1),x''(2),...,x''(Dimension)
  //!
  //! WeightsDerivatives is an array that contains derivatives
  //! from 0 to  <DerivativeRequest>
  //! After returning from the routine the array
  //! RationalDerivatives contains the following
  //! x(1)/w , x(2)/w ,  ...
  //! x(1)/w , x(2)/w ,  ...   derivated once
  //! x(1)/w , x(2)/w ,  ...   twice
  //! x(1)/w , x(2)/w ,  ... derivated <DerivativeRequest> times
  //!
  //! The array RationalDerivatives and PolesDerivatives
  //! can be same since the overwrite is non destructive within
  //! the algorithm
  //!
  //! Warning: <RationalDerivates> must be dimensionned properly.
  Standard_EXPORT static void RationalDerivatives (const Standard_Integer DerivativesRequest, const Standard_Integer Dimension, Standard_Real& PolesDerivatives, Standard_Real& WeightsDerivatives, Standard_Real& RationalDerivates);
  
  //! Performs Horner method with synthethic division
  //! for derivatives
  //! parameter <U>, with <Degree> and <Dimension>.
  //! PolynomialCoeff are stored in the following fashion
  //! c0(1)      c0(2) ....       c0(Dimension)
  //! c1(1)      c1(2) ....       c1(Dimension)
  //!
  //! cDegree(1) cDegree(2) ....  cDegree(Dimension)
  //! where the polynomial is defined as :
  //!
  //! 2                     Degree
  //! c0 + c1 X + c2 X  +  ....   cDegree X
  //!
  //! Results stores the result in the following format
  //!
  //! f(1)             f(2)  ....     f(Dimension)
  //! (1)           (1)              (1)
  //! f  (1)        f   (2) ....     f   (Dimension)
  //!
  //! (DerivativeRequest)            (DerivativeRequest)
  //! f  (1)                         f   (Dimension)
  //!
  //! this just evaluates the point at parameter U
  //!
  //! Warning: <Results> and <PolynomialCoeff> must be dimensioned properly
  Standard_EXPORT static void EvalPolynomial (const Standard_Real U, const Standard_Integer DerivativeOrder, const Standard_Integer Degree, const Standard_Integer Dimension, Standard_Real& PolynomialCoeff, Standard_Real& Results);
  
  //! Same as above with DerivativeOrder = 0;
  Standard_EXPORT static void NoDerivativeEvalPolynomial (const Standard_Real U, const Standard_Integer Degree, const Standard_Integer Dimension, const Standard_Integer DegreeDimension, Standard_Real& PolynomialCoeff, Standard_Real& Results);
  
  //! Applies EvalPolynomial twice to evaluate the derivative
  //! of orders UDerivativeOrder in U, VDerivativeOrder in V
  //! at parameters U,V
  //!
  //! PolynomialCoeff are stored in the following fashion
  //! c00(1)  ....       c00(Dimension)
  //! c10(1)  ....       c10(Dimension)
  //! ....
  //! cm0(1)  ....       cm0(Dimension)
  //! ....
  //! c01(1)  ....       c01(Dimension)
  //! c11(1)  ....       c11(Dimension)
  //! ....
  //! cm1(1)  ....       cm1(Dimension)
  //! ....
  //! c0n(1)  ....       c0n(Dimension)
  //! c1n(1)  ....       c1n(Dimension)
  //! ....
  //! cmn(1)  ....       cmn(Dimension)
  //!
  //! where the polynomial is defined as :
  //! 2                 m
  //! c00 + c10 U + c20 U  +  ....  + cm0 U
  //! 2                   m
  //! + c01 V + c11 UV + c21 U V  +  ....  + cm1 U  V
  //! n               m n
  //! + .... + c0n V +  ....  + cmn U V
  //!
  //! with m = UDegree and n = VDegree
  //!
  //! Results stores the result in the following format
  //!
  //! f(1)             f(2)  ....     f(Dimension)
  //!
  //! Warning: <Results> and <PolynomialCoeff> must be dimensioned properly
  Standard_EXPORT static void EvalPoly2Var (const Standard_Real U, const Standard_Real V, const Standard_Integer UDerivativeOrder, const Standard_Integer VDerivativeOrder, const Standard_Integer UDegree, const Standard_Integer VDegree, const Standard_Integer Dimension, Standard_Real& PolynomialCoeff, Standard_Real& Results);
  
  //! Performs the Lagrange Interpolation of
  //! given series of points with given parameters
  //! with the requested derivative order
  //! Results will store things in the following format
  //! with d = DerivativeOrder
  //!
  //! [0],             [Dimension-1]              : value
  //! [Dimension],     [Dimension  + Dimension-1] : first derivative
  //!
  //! [d *Dimension],  [d*Dimension + Dimension-1]: dth   derivative
  Standard_EXPORT static Standard_Integer EvalLagrange (const Standard_Real U, const Standard_Integer DerivativeOrder, const Standard_Integer Degree, const Standard_Integer Dimension, Standard_Real& ValueArray, Standard_Real& ParameterArray, Standard_Real& Results);
  
  //! Performs the Cubic Hermite Interpolation of
  //! given series of points with given parameters
  //! with the requested derivative order.
  //! ValueArray stores the value at the first and
  //! last parameter. It has the following format :
  //! [0],             [Dimension-1]              : value at first param
  //! [Dimension],     [Dimension  + Dimension-1] : value at last param
  //! Derivative array stores the value of the derivatives
  //! at the first parameter and at the last parameter
  //! in the following format
  //! [0],             [Dimension-1]              : derivative at
  //! first param
  //! [Dimension],     [Dimension  + Dimension-1] : derivative at
  //! last param
  //!
  //! ParameterArray  stores the first and last parameter
  //! in the following format :
  //! [0] : first parameter
  //! [1] : last  parameter
  //!
  //! Results will store things in the following format
  //! with d = DerivativeOrder
  //!
  //! [0],             [Dimension-1]              : value
  //! [Dimension],     [Dimension  + Dimension-1] : first derivative
  //!
  //! [d *Dimension],  [d*Dimension + Dimension-1]: dth   derivative
  Standard_EXPORT static Standard_Integer EvalCubicHermite (const Standard_Real U, const Standard_Integer DerivativeOrder, const Standard_Integer Dimension, Standard_Real& ValueArray, Standard_Real& DerivativeArray, Standard_Real& ParameterArray, Standard_Real& Results);
  
  //! This build the coefficient of Hermite's polynomes on
  //! [FirstParameter, LastParameter]
  //!
  //! if j <= FirstOrder+1 then
  //!
  //! MatrixCoefs[i, j] = ith coefficient of the polynome H0,j-1
  //!
  //! else
  //!
  //! MatrixCoefs[i, j] = ith coefficient of the polynome H1,k
  //! with k = j - FirstOrder - 2
  //!
  //! return false if
  //! - |FirstParameter| > 100
  //! - |LastParameter| > 100
  //! - |FirstParameter| +|LastParameter| < 1/100
  //! -   |LastParameter - FirstParameter|
  //! / (|FirstParameter| +|LastParameter|)  < 1/100
  Standard_EXPORT static Standard_Boolean HermiteCoefficients (const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Integer FirstOrder, const Standard_Integer LastOrder, math_Matrix& MatrixCoefs);
  
  Standard_EXPORT static void CoefficientsPoles (const TColgp_Array1OfPnt& Coefs, const TColStd_Array1OfReal* WCoefs, TColgp_Array1OfPnt& Poles, TColStd_Array1OfReal* WPoles);
  
  Standard_EXPORT static void CoefficientsPoles (const TColgp_Array1OfPnt2d& Coefs, const TColStd_Array1OfReal* WCoefs, TColgp_Array1OfPnt2d& Poles, TColStd_Array1OfReal* WPoles);
  
  Standard_EXPORT static void CoefficientsPoles (const TColStd_Array1OfReal& Coefs, const TColStd_Array1OfReal* WCoefs, TColStd_Array1OfReal& Poles, TColStd_Array1OfReal* WPoles);
  
  Standard_EXPORT static void CoefficientsPoles (const Standard_Integer dim, const TColStd_Array1OfReal& Coefs, const TColStd_Array1OfReal* WCoefs, TColStd_Array1OfReal& Poles, TColStd_Array1OfReal* WPoles);
  
  Standard_EXPORT static void Trimming (const Standard_Real U1, const Standard_Real U2, TColgp_Array1OfPnt& Coeffs, TColStd_Array1OfReal* WCoeffs);
  
  Standard_EXPORT static void Trimming (const Standard_Real U1, const Standard_Real U2, TColgp_Array1OfPnt2d& Coeffs, TColStd_Array1OfReal* WCoeffs);
  
  Standard_EXPORT static void Trimming (const Standard_Real U1, const Standard_Real U2, TColStd_Array1OfReal& Coeffs, TColStd_Array1OfReal* WCoeffs);
  
  Standard_EXPORT static void Trimming (const Standard_Real U1, const Standard_Real U2, const Standard_Integer dim, TColStd_Array1OfReal& Coeffs, TColStd_Array1OfReal* WCoeffs);
  
  Standard_EXPORT static void CoefficientsPoles (const TColgp_Array2OfPnt& Coefs, const TColStd_Array2OfReal* WCoefs, TColgp_Array2OfPnt& Poles, TColStd_Array2OfReal* WPoles);
  
  Standard_EXPORT static void UTrimming (const Standard_Real U1, const Standard_Real U2, TColgp_Array2OfPnt& Coeffs, TColStd_Array2OfReal* WCoeffs);
  
  Standard_EXPORT static void VTrimming (const Standard_Real V1, const Standard_Real V2, TColgp_Array2OfPnt& Coeffs, TColStd_Array2OfReal* WCoeffs);
  
  //! Compute the coefficients in the canonical base of the
  //! polynomial satisfying the given constraints
  //! at the given parameters
  //! The array FirstContr(i,j) i=1,Dimension j=0,FirstOrder
  //! contains the values of the constraint at parameter FirstParameter
  //! idem for LastConstr
  Standard_EXPORT static Standard_Boolean HermiteInterpolate (const Standard_Integer Dimension, const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Integer FirstOrder, const Standard_Integer LastOrder, const TColStd_Array2OfReal& FirstConstr, const TColStd_Array2OfReal& LastConstr, TColStd_Array1OfReal& Coefficients);
  
  //! Compute the number of points used for integral
  //! computations (NbGaussPoints) and the degree of Jacobi
  //! Polynomial (WorkDegree).
  //! ConstraintOrder has to be GeomAbs_C0, GeomAbs_C1 or GeomAbs_C2
  //! Code: Code d' init. des parametres de discretisation.
  //! = -5
  //! = -4
  //! = -3
  //! = -2
  //! = -1
  //! =  1 calcul rapide avec precision moyenne.
  //! =  2 calcul rapide avec meilleure precision.
  //! =  3 calcul un peu plus lent avec bonne precision.
  //! =  4 calcul lent avec la meilleure precision possible.
  Standard_EXPORT static void JacobiParameters (const GeomAbs_Shape ConstraintOrder, const Standard_Integer MaxDegree, const Standard_Integer Code, Standard_Integer& NbGaussPoints, Standard_Integer& WorkDegree);
  
  //! translates from GeomAbs_Shape to Integer
  Standard_EXPORT static Standard_Integer NivConstr (const GeomAbs_Shape ConstraintOrder);
  
  //! translates from Integer to GeomAbs_Shape
  Standard_EXPORT static GeomAbs_Shape ConstraintOrder (const Standard_Integer NivConstr);
  
  Standard_EXPORT static void EvalLength (const Standard_Integer Degree, const Standard_Integer Dimension, Standard_Real& PolynomialCoeff, const Standard_Real U1, const Standard_Real U2, Standard_Real& Length);
  
  Standard_EXPORT static void EvalLength (const Standard_Integer Degree, const Standard_Integer Dimension, Standard_Real& PolynomialCoeff, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol, Standard_Real& Length, Standard_Real& Error);




protected:





private:




friend class PLib_Base;
friend class PLib_JacobiPolynomial;
friend class PLib_HermitJacobi;
friend class PLib_DoubleJacobiPolynomial;

};

#endif // _PLib_HeaderFile
