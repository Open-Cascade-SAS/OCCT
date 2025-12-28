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

#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <GeomAbs_Shape.hxx>
class math_Matrix;

//! PLib means Polynomial functions library. This pk
//! provides basic computation functions for
//! polynomial functions.
//! Note: weight arrays can be passed by pointer for
//! some functions so that NULL pointer is valid.
//! That means no weights passed.
class PLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Used as argument for a non rational functions
  inline static NCollection_Array1<double>* NoWeights() { return NULL; }

  //! Used as argument for a non rational functions
  inline static NCollection_Array2<double>* NoWeights2() { return NULL; }

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array1<gp_Pnt>& Poles, NCollection_Array1<double>& FP);

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array1<gp_Pnt>&   Poles,
                                       const NCollection_Array1<double>& Weights,
                                       NCollection_Array1<double>&       FP);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP, NCollection_Array1<gp_Pnt>& Poles);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP,
                                       NCollection_Array1<gp_Pnt>&         Poles,
                                       NCollection_Array1<double>&       Weights);

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array1<gp_Pnt2d>& Poles, NCollection_Array1<double>& FP);

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array1<gp_Pnt2d>& Poles,
                                       const NCollection_Array1<double>& Weights,
                                       NCollection_Array1<double>&       FP);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP, NCollection_Array1<gp_Pnt2d>& Poles);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP,
                                       NCollection_Array1<gp_Pnt2d>&       Poles,
                                       NCollection_Array1<double>&       Weights);

  //! Returns the Binomial Cnp. N should be <= BSplCLib::MaxDegree().
  Standard_EXPORT static double Bin(const int N, const int P);

  //! Computes the derivatives of a ratio at order
  //! <N> in dimension <Dimension>.
  //!
  //! <Ders> is an array containing the values of the
  //! input derivatives from 0 to std::min(<N>,<Degree>).
  //! For orders higher than <Degree> the inputcd /s2d1/BMDL/
  //! derivatives are assumed to be 0.
  //!
  //! Content of <Ders>:
  //!
  //! x(1),x(2),...,x(Dimension),w
  //! x'(1),x'(2),...,x'(Dimension),w'
  //! x''(1),x''(2),...,x''(Dimension),w''
  //!
  //! If <All> is false, only the derivative at order
  //! <N> is computed. <RDers> is an array of length
  //! Dimension which will contain the result:
  //!
  //! x(1)/w , x(2)/w ,  ... derivated <N> times
  //!
  //! If <All> is true all the derivatives up to order
  //! <N> are computed. <RDers> is an array of length
  //! Dimension * (N+1) which will contains:
  //!
  //! x(1)/w , x(2)/w ,  ...
  //! x(1)/w , x(2)/w ,  ... derivated <1> times
  //! x(1)/w , x(2)/w ,  ... derivated <2> times
  //! ...
  //! x(1)/w , x(2)/w ,  ... derivated <N> times
  //!
  //! Warning: <RDers> must be dimensioned properly.
  Standard_EXPORT static void RationalDerivative(const int Degree,
                                                 const int N,
                                                 const int Dimension,
                                                 double&         Ders,
                                                 double&         RDers,
                                                 const bool All = true);

  //! Computes DerivativesRequest derivatives of a ratio at
  //! of a BSpline function of degree <Degree>
  //! dimension <Dimension>.
  //!
  //! <PolesDerivatives> is an array containing the values
  //! of the input derivatives from 0 to <DerivativeRequest>
  //! For orders higher than <Degree> the input
  //! derivatives are assumed to be 0.
  //!
  //! Content of <PoleasDerivatives> :
  //!
  //! x(1),x(2),...,x(Dimension)
  //! x'(1),x'(2),...,x'(Dimension)
  //! x''(1),x''(2),...,x''(Dimension)
  //!
  //! WeightsDerivatives is an array that contains derivatives
  //! from 0 to <DerivativeRequest>
  //! After returning from the routine the array
  //! RationalDerivatives contains the following
  //! x(1)/w , x(2)/w ,  ...
  //! x(1)/w , x(2)/w ,  ... derivated once
  //! x(1)/w , x(2)/w ,  ... twice
  //! x(1)/w , x(2)/w ,  ... derivated <DerivativeRequest> times
  //!
  //! The array RationalDerivatives and PolesDerivatives
  //! can be same since the overwrite is non destructive within
  //! the algorithm
  //!
  //! Warning: <RationalDerivates> must be dimensioned properly.
  Standard_EXPORT static void RationalDerivatives(const int DerivativesRequest,
                                                  const int Dimension,
                                                  double&         PolesDerivatives,
                                                  double&         WeightsDerivatives,
                                                  double&         RationalDerivates);

  //! Performs Horner method with synthetic division for derivatives
  //! parameter <U>, with <Degree> and <Dimension>.
  //! PolynomialCoeff are stored in the following fashion
  //! @code
  //! c0(1)      c0(2) ....       c0(Dimension)
  //! c1(1)      c1(2) ....       c1(Dimension)
  //!
  //! cDegree(1) cDegree(2) ....  cDegree(Dimension)
  //! @endcode
  //! where the polynomial is defined as :
  //! @code
  //! 2                     Degree
  //! c0 + c1 X + c2 X  +  ....   cDegree X
  //! @endcode
  //! Results stores the result in the following format
  //! @code
  //! f(1)             f(2)  ....     f(Dimension)
  //! (1)           (1)              (1)
  //! f  (1)        f   (2) ....     f   (Dimension)
  //!
  //! (DerivativeRequest)            (DerivativeRequest)
  //! f  (1)                         f   (Dimension)
  //! @endcode
  //! this just evaluates the point at parameter U
  //!
  //! Warning: <Results> and <PolynomialCoeff> must be dimensioned properly
  Standard_EXPORT static void EvalPolynomial(const double    U,
                                             const int DerivativeOrder,
                                             const int Degree,
                                             const int Dimension,
                                             const double&   PolynomialCoeff,
                                             double&         Results);

  //! Same as above with DerivativeOrder = 0;
  Standard_EXPORT static void NoDerivativeEvalPolynomial(const double    U,
                                                         const int Degree,
                                                         const int Dimension,
                                                         const int DegreeDimension,
                                                         const double&   PolynomialCoeff,
                                                         double&         Results);

  //! Applies EvalPolynomial twice to evaluate the derivative
  //! of orders UDerivativeOrder in U, VDerivativeOrder in V
  //! at parameters U,V
  //!
  //! PolynomialCoeff are stored in the following fashion
  //! @code
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
  //! @endcode
  //! where the polynomial is defined as :
  //! @code
  //! 2                 m
  //! c00 + c10 U + c20 U  +  ....  + cm0 U
  //! 2                   m
  //! + c01 V + c11 UV + c21 U V  +  ....  + cm1 U  V
  //! n               m n
  //! + .... + c0n V +  ....  + cmn U V
  //! @endcode
  //! with m = UDegree and n = VDegree
  //!
  //! Results stores the result in the following format
  //! @code
  //! f(1)             f(2)  ....     f(Dimension)
  //! @endcode
  //! Warning: <Results> and <PolynomialCoeff> must be dimensioned properly
  Standard_EXPORT static void EvalPoly2Var(const double    U,
                                           const double    V,
                                           const int UDerivativeOrder,
                                           const int VDerivativeOrder,
                                           const int UDegree,
                                           const int VDegree,
                                           const int Dimension,
                                           double&         PolynomialCoeff,
                                           double&         Results);

  //! Performs the Lagrange Interpolation of
  //! given series of points with given parameters
  //! with the requested derivative order
  //! Results will store things in the following format
  //! with d = DerivativeOrder
  //! @code
  //! [0],             [Dimension-1]              : value
  //! [Dimension],     [Dimension  + Dimension-1] : first derivative
  //!
  //! [d *Dimension],  [d*Dimension + Dimension-1]: dth   derivative
  //! @endcode
  Standard_EXPORT static int EvalLagrange(const double    U,
                                                       const int DerivativeOrder,
                                                       const int Degree,
                                                       const int Dimension,
                                                       double&         ValueArray,
                                                       double&         ParameterArray,
                                                       double&         Results);

  //! Performs the Cubic Hermite Interpolation of
  //! given series of points with given parameters
  //! with the requested derivative order.
  //! ValueArray stores the value at the first and
  //! last parameter. It has the following format :
  //! @code
  //! [0],             [Dimension-1]              : value at first param
  //! [Dimension],     [Dimension  + Dimension-1] : value at last param
  //! @endcode
  //! Derivative array stores the value of the derivatives
  //! at the first parameter and at the last parameter
  //! in the following format
  //! @code
  //! [0],             [Dimension-1]              : derivative at
  //! @endcode
  //! first param
  //! @code
  //! [Dimension],     [Dimension  + Dimension-1] : derivative at
  //! @endcode
  //! last param
  //!
  //! ParameterArray  stores the first and last parameter
  //! in the following format :
  //! @code
  //! [0] : first parameter
  //! [1] : last  parameter
  //! @endcode
  //!
  //! Results will store things in the following format
  //! with d = DerivativeOrder
  //! @code
  //! [0],             [Dimension-1]              : value
  //! [Dimension],     [Dimension  + Dimension-1] : first derivative
  //!
  //! [d *Dimension],  [d*Dimension + Dimension-1]: dth   derivative
  //! @endcode
  Standard_EXPORT static int EvalCubicHermite(const double    U,
                                                           const int DerivativeOrder,
                                                           const int Dimension,
                                                           double&         ValueArray,
                                                           double&         DerivativeArray,
                                                           double&         ParameterArray,
                                                           double&         Results);

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
  Standard_EXPORT static bool HermiteCoefficients(const double    FirstParameter,
                                                              const double    LastParameter,
                                                              const int FirstOrder,
                                                              const int LastOrder,
                                                              math_Matrix&           MatrixCoefs);

  Standard_EXPORT static void CoefficientsPoles(const NCollection_Array1<gp_Pnt>&   Coefs,
                                                const NCollection_Array1<double>* WCoefs,
                                                NCollection_Array1<gp_Pnt>&         Poles,
                                                NCollection_Array1<double>*       WPoles);

  Standard_EXPORT static void CoefficientsPoles(const NCollection_Array1<gp_Pnt2d>& Coefs,
                                                const NCollection_Array1<double>* WCoefs,
                                                NCollection_Array1<gp_Pnt2d>&       Poles,
                                                NCollection_Array1<double>*       WPoles);

  Standard_EXPORT static void CoefficientsPoles(const NCollection_Array1<double>& Coefs,
                                                const NCollection_Array1<double>* WCoefs,
                                                NCollection_Array1<double>&       Poles,
                                                NCollection_Array1<double>*       WPoles);

  Standard_EXPORT static void CoefficientsPoles(const int      dim,
                                                const NCollection_Array1<double>& Coefs,
                                                const NCollection_Array1<double>* WCoefs,
                                                NCollection_Array1<double>&       Poles,
                                                NCollection_Array1<double>*       WPoles);

  Standard_EXPORT static void Trimming(const double   U1,
                                       const double   U2,
                                       NCollection_Array1<gp_Pnt>&   Coeffs,
                                       NCollection_Array1<double>* WCoeffs);

  Standard_EXPORT static void Trimming(const double   U1,
                                       const double   U2,
                                       NCollection_Array1<gp_Pnt2d>& Coeffs,
                                       NCollection_Array1<double>* WCoeffs);

  Standard_EXPORT static void Trimming(const double   U1,
                                       const double   U2,
                                       NCollection_Array1<double>& Coeffs,
                                       NCollection_Array1<double>* WCoeffs);

  Standard_EXPORT static void Trimming(const double    U1,
                                       const double    U2,
                                       const int dim,
                                       NCollection_Array1<double>&  Coeffs,
                                       NCollection_Array1<double>*  WCoeffs);

  Standard_EXPORT static void CoefficientsPoles(const NCollection_Array2<gp_Pnt>&   Coefs,
                                                const NCollection_Array2<double>* WCoefs,
                                                NCollection_Array2<gp_Pnt>&         Poles,
                                                NCollection_Array2<double>*       WPoles);

  Standard_EXPORT static void UTrimming(const double   U1,
                                        const double   U2,
                                        NCollection_Array2<gp_Pnt>&   Coeffs,
                                        NCollection_Array2<double>* WCoeffs);

  Standard_EXPORT static void VTrimming(const double   V1,
                                        const double   V2,
                                        NCollection_Array2<gp_Pnt>&   Coeffs,
                                        NCollection_Array2<double>* WCoeffs);

  //! Compute the coefficients in the canonical base of the
  //! polynomial satisfying the given constraints
  //! at the given parameters
  //! The array FirstContr(i,j) i=1,Dimension j=0,FirstOrder
  //! contains the values of the constraint at parameter FirstParameter
  //! idem for LastConstr
  Standard_EXPORT static bool HermiteInterpolate(
    const int      Dimension,
    const double         FirstParameter,
    const double         LastParameter,
    const int      FirstOrder,
    const int      LastOrder,
    const NCollection_Array2<double>& FirstConstr,
    const NCollection_Array2<double>& LastConstr,
    NCollection_Array1<double>&       Coefficients);

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
  Standard_EXPORT static void JacobiParameters(const GeomAbs_Shape    ConstraintOrder,
                                               const int MaxDegree,
                                               const int Code,
                                               int&      NbGaussPoints,
                                               int&      WorkDegree);

  //! translates from GeomAbs_Shape to Integer
  Standard_EXPORT static int NivConstr(const GeomAbs_Shape ConstraintOrder);

  //! translates from Integer to GeomAbs_Shape
  Standard_EXPORT static GeomAbs_Shape ConstraintOrder(const int NivConstr);

  Standard_EXPORT static void EvalLength(const int Degree,
                                         const int Dimension,
                                         double&         PolynomialCoeff,
                                         const double    U1,
                                         const double    U2,
                                         double&         Length);

  Standard_EXPORT static void EvalLength(const int Degree,
                                         const int Dimension,
                                         double&         PolynomialCoeff,
                                         const double    U1,
                                         const double    U2,
                                         const double    Tol,
                                         double&         Length,
                                         double&         Error);
};

#endif // _PLib_HeaderFile
