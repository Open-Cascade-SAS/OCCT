// Created on: 1996-10-08
// Created by: Jeannine PANTIATICI
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _PLib_JacobiPolynomial_HeaderFile
#define _PLib_JacobiPolynomial_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <PLib_Base.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;


class PLib_JacobiPolynomial;
DEFINE_STANDARD_HANDLE(PLib_JacobiPolynomial, PLib_Base)

//! This class provides method  to work with Jacobi  Polynomials
//! relatively to   an order of constraint
//! q  = myWorkDegree-2*(myNivConstr+1)
//! Jk(t)  for k=0,q compose  the   Jacobi Polynomial  base relatively  to  the weigth W(t)
//! iorder is the integer  value for the constraints:
//! iorder = 0 <=> ConstraintOrder  = GeomAbs_C0
//! iorder = 1 <=>  ConstraintOrder = GeomAbs_C1
//! iorder = 2 <=> ConstraintOrder = GeomAbs_C2
//! P(t) = R(t) + W(t) * Q(t) Where W(t) = (1-t**2)**(2*iordre+2)
//! the coefficients JacCoeff represents P(t) JacCoeff are stored as follow:
//!
//! c0(1)      c0(2) ....       c0(Dimension)
//! c1(1)      c1(2) ....       c1(Dimension)
//!
//! cDegree(1) cDegree(2) ....  cDegree(Dimension)
//!
//! The coefficients
//! c0(1)                  c0(2) ....            c0(Dimension)
//! c2*ordre+1(1)                ...          c2*ordre+1(dimension)
//!
//! represents the  part  of the polynomial in  the
//! canonical base: R(t)
//! R(t) = c0 + c1   t + ...+ c2*iordre+1 t**2*iordre+1
//! The following coefficients represents the part of the
//! polynomial in the Jacobi base ie Q(t)
//! Q(t) = c2*iordre+2  J0(t) + ...+ cDegree JDegree-2*iordre-2
class PLib_JacobiPolynomial : public PLib_Base
{

public:

  

  //! Initialize the polynomial class
  //! Degree has to be <= 30
  //! ConstraintOrder has to be GeomAbs_C0
  //! GeomAbs_C1
  //! GeomAbs_C2
  Standard_EXPORT PLib_JacobiPolynomial(const Standard_Integer WorkDegree, const GeomAbs_Shape ConstraintOrder);
  

  //! returns  the  Jacobi  Points   for  Gauss  integration ie
  //! the positive values of the Legendre roots by increasing values
  //! NbGaussPoints is the number of   points chosen for the  integral
  //! computation.
  //! TabPoints (0,NbGaussPoints/2)
  //! TabPoints (0) is loaded only for the odd values of NbGaussPoints
  //! The possible values for NbGaussPoints are : 8, 10,
  //! 15, 20, 25, 30, 35, 40, 50, 61
  //! NbGaussPoints must be greater than Degree
  Standard_EXPORT void Points (const Standard_Integer NbGaussPoints, TColStd_Array1OfReal& TabPoints) const;
  

  //! returns the Jacobi weigths for Gauss integration only for
  //! the positive    values of the  Legendre roots   in the order they
  //! are given by the method Points
  //! NbGaussPoints   is the number of points chosen   for  the integral
  //! computation.
  //! TabWeights  (0,NbGaussPoints/2,0,Degree)
  //! TabWeights (0,.) are only loaded for the odd values of NbGaussPoints
  //! The possible values for NbGaussPoints are : 8 , 10 , 15 ,20 ,25 , 30,
  //! 35 , 40 , 50 , 61 NbGaussPoints must be greater than Degree
  Standard_EXPORT void Weights (const Standard_Integer NbGaussPoints, TColStd_Array2OfReal& TabWeights) const;
  

  //! this method loads for k=0,q the maximum value of
  //! abs ( W(t)*Jk(t) )for t bellonging to [-1,1]
  //! This values are loaded is the array TabMax(0,myWorkDegree-2*(myNivConst+1))
  //! MaxValue ( me ; TabMaxPointer : in  out  Real );
  Standard_EXPORT void MaxValue (TColStd_Array1OfReal& TabMax) const;
  

  //! This  method computes the  maximum  error on the polynomial
  //! W(t) Q(t)  obtained  by   missing  the   coefficients of  JacCoeff   from
  //! NewDegree +1 to Degree
  Standard_EXPORT Standard_Real MaxError (const Standard_Integer Dimension, Standard_Real& JacCoeff, const Standard_Integer NewDegree) const;
  

  //! Compute NewDegree <= MaxDegree  so that MaxError is lower
  //! than Tol.
  //! MaxError can be greater than Tol  if it is not possible
  //! to find a NewDegree <= MaxDegree.
  //! In this case NewDegree = MaxDegree
  Standard_EXPORT void ReduceDegree (const Standard_Integer Dimension, const Standard_Integer MaxDegree, const Standard_Real Tol, Standard_Real& JacCoeff, Standard_Integer& NewDegree, Standard_Real& MaxError) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real AverageError (const Standard_Integer Dimension, Standard_Real& JacCoeff, const Standard_Integer NewDegree) const;
  

  //! Convert the polynomial P(t) = R(t) + W(t) Q(t) in the canonical base.
  Standard_EXPORT void ToCoefficients (const Standard_Integer Dimension, const Standard_Integer Degree, const TColStd_Array1OfReal& JacCoeff, TColStd_Array1OfReal& Coefficients) const Standard_OVERRIDE;
  
  //! Compute the values of the basis functions in u
  Standard_EXPORT void D0 (const Standard_Real U, TColStd_Array1OfReal& BasisValue) Standard_OVERRIDE;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT void D1 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1) Standard_OVERRIDE;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT void D2 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1, TColStd_Array1OfReal& BasisD2) Standard_OVERRIDE;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT void D3 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1, TColStd_Array1OfReal& BasisD2, TColStd_Array1OfReal& BasisD3) Standard_OVERRIDE;
  
  //! returns WorkDegree
  Standard_Integer WorkDegree() const Standard_OVERRIDE;
  
  //! returns NivConstr
  Standard_Integer NivConstr() const;




  DEFINE_STANDARD_RTTIEXT(PLib_JacobiPolynomial,PLib_Base)

protected:




private:

  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT void D0123 (const Standard_Integer NDerive, const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1, TColStd_Array1OfReal& BasisD2, TColStd_Array1OfReal& BasisD3);

  Standard_Integer myWorkDegree;
  Standard_Integer myNivConstr;
  Standard_Integer myDegree;
  Handle(TColStd_HArray1OfReal) myTNorm;
  Handle(TColStd_HArray1OfReal) myCofA;
  Handle(TColStd_HArray1OfReal) myCofB;
  Handle(TColStd_HArray1OfReal) myDenom;


};


#include <PLib_JacobiPolynomial.lxx>





#endif // _PLib_JacobiPolynomial_HeaderFile
