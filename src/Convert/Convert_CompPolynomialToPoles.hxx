// Created on: 1995-05-30
// Created by: Xavier BENVENISTE
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

#ifndef _Convert_CompPolynomialToPoles_HeaderFile
#define _Convert_CompPolynomialToPoles_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
class Standard_OutOfRange;
class Standard_ConstructionError;


//! Convert a serie of Polynomial N-Dimensional Curves
//! that are have continuity CM to an N-Dimensional Bspline Curve
//! that has continuity CM.
//! (to convert an function (curve) polynomial by span in a BSpline)
//! This class uses the following arguments :
//! NumCurves :  the number of Polynomial Curves
//! Continuity:  the requested continuity for the n-dimensional Spline
//! Dimension :  the dimension of the Spline
//! MaxDegree :  maximum allowed degree for each composite
//! polynomial segment.
//! NumCoeffPerCurve : the number of coefficient per segments = degree - 1
//! Coefficients  :  the coefficients organized in the following way
//! [1..<myNumPolynomials>][1..myMaxDegree +1][1..myDimension]
//! that is : index [n,d,i] is at slot
//! (n-1) * (myMaxDegree + 1) * myDimension + (d-1) * myDimension + i
//! PolynomialIntervals :  nth polynomial represents a polynomial between
//! myPolynomialIntervals->Value(n,0) and
//! myPolynomialIntervals->Value(n,1)
//! TrueIntervals : the nth polynomial has to be mapped linearly to be
//! defined on the following interval :
//! myTrueIntervals->Value(n) and myTrueIntervals->Value(n+1)
//! so that it represent adequatly the function with the
//! required continuity
class Convert_CompPolynomialToPoles 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Warning!
  //! Continuity can be at MOST the maximum degree of
  //! the polynomial functions
  //! TrueIntervals :
  //! this is the true parameterisation for the composite curve
  //! that is : the curve has myContinuity if the nth curve
  //! is parameterized between myTrueIntervals(n) and myTrueIntervals(n+1)
  //!
  //! Coefficients have to be the implicit "c form":
  //! Coefficients[Numcurves][MaxDegree+1][Dimension]
  //!
  //! Warning!
  //! The NumberOfCoefficient of an polynome is his degree + 1
  //! Example: To convert the linear function f(x) = 2*x + 1 on the
  //! domaine [2,5] to BSpline with the bound [-1,1]. Arguments are :
  //! NumCurves  = 1;
  //! Continuity = 1;
  //! Dimension  = 1;
  //! MaxDegree  = 1;
  //! NumCoeffPerCurve [1] = {2};
  //! Coefficients[2] = {1, 2};
  //! PolynomialIntervals[1,2] = {{2,5}}
  //! TrueIntervals[2] = {-1, 1}
  Standard_EXPORT Convert_CompPolynomialToPoles(const Standard_Integer NumCurves, const Standard_Integer Continuity, const Standard_Integer Dimension, const Standard_Integer MaxDegree, const Handle(TColStd_HArray1OfInteger)& NumCoeffPerCurve, const Handle(TColStd_HArray1OfReal)& Coefficients, const Handle(TColStd_HArray2OfReal)& PolynomialIntervals, const Handle(TColStd_HArray1OfReal)& TrueIntervals);
  
  //! To Convert sevral span with different order of Continuity.
  //! Warning: The Length of Continuity have to be NumCurves-1
  Standard_EXPORT Convert_CompPolynomialToPoles(const Standard_Integer NumCurves, const Standard_Integer Dimension, const Standard_Integer MaxDegree, const TColStd_Array1OfInteger& Continuity, const TColStd_Array1OfInteger& NumCoeffPerCurve, const TColStd_Array1OfReal& Coefficients, const TColStd_Array2OfReal& PolynomialIntervals, const TColStd_Array1OfReal& TrueIntervals);
  
  //! To Convert only one span.
  Standard_EXPORT Convert_CompPolynomialToPoles(const Standard_Integer Dimension, const Standard_Integer MaxDegree, const Standard_Integer Degree, const TColStd_Array1OfReal& Coefficients, const TColStd_Array1OfReal& PolynomialIntervals, const TColStd_Array1OfReal& TrueIntervals);
  
  //! number of poles of the n-dimensional BSpline
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  //! returns the poles of the n-dimensional BSpline
  //! in the following format :
  //! [1..NumPoles][1..Dimension]
  Standard_EXPORT void Poles (Handle(TColStd_HArray2OfReal)& Poles) const;
  
  Standard_EXPORT Standard_Integer Degree() const;
  
  //! Degree of the n-dimensional Bspline
  Standard_EXPORT Standard_Integer NbKnots() const;
  
  //! Knots of the n-dimensional Bspline
  Standard_EXPORT void Knots (Handle(TColStd_HArray1OfReal)& K) const;
  
  //! Multiplicities of the knots in the BSpline
  Standard_EXPORT void Multiplicities (Handle(TColStd_HArray1OfInteger)& M) const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:

  
  Standard_EXPORT void Perform (const Standard_Integer NumCurves, const Standard_Integer MaxDegree, const Standard_Integer Dimension, const TColStd_Array1OfInteger& NumCoeffPerCurve, const TColStd_Array1OfReal& Coefficients, const TColStd_Array2OfReal& PolynomialIntervals, const TColStd_Array1OfReal& TrueIntervals);


  Handle(TColStd_HArray1OfReal) myFlatKnots;
  Handle(TColStd_HArray1OfReal) myKnots;
  Handle(TColStd_HArray1OfInteger) myMults;
  Handle(TColStd_HArray2OfReal) myPoles;
  Standard_Integer myDegree;
  Standard_Boolean myDone;


};







#endif // _Convert_CompPolynomialToPoles_HeaderFile
