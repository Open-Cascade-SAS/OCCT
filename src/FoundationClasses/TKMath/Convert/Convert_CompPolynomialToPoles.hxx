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
#include <Standard_Macro.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

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
//! so that it adequately represents the function with the
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
  Standard_EXPORT Convert_CompPolynomialToPoles(
    const int                                       NumCurves,
    const int                                       Continuity,
    const int                                       Dimension,
    const int                                       MaxDegree,
    const occ::handle<NCollection_HArray1<int>>&    NumCoeffPerCurve,
    const occ::handle<NCollection_HArray1<double>>& Coefficients,
    const occ::handle<NCollection_HArray2<double>>& PolynomialIntervals,
    const occ::handle<NCollection_HArray1<double>>& TrueIntervals);

  //! To Convert several span with different order of Continuity.
  //! Warning: The Length of Continuity have to be NumCurves-1
  Standard_EXPORT Convert_CompPolynomialToPoles(
    const int                         NumCurves,
    const int                         Dimension,
    const int                         MaxDegree,
    const NCollection_Array1<int>&    Continuity,
    const NCollection_Array1<int>&    NumCoeffPerCurve,
    const NCollection_Array1<double>& Coefficients,
    const NCollection_Array2<double>& PolynomialIntervals,
    const NCollection_Array1<double>& TrueIntervals);

  //! To Convert only one span.
  Standard_EXPORT Convert_CompPolynomialToPoles(
    const int                         Dimension,
    const int                         MaxDegree,
    const int                         Degree,
    const NCollection_Array1<double>& Coefficients,
    const NCollection_Array1<double>& PolynomialIntervals,
    const NCollection_Array1<double>& TrueIntervals);

  //! Returns the number of poles of the n-dimensional BSpline.
  [[nodiscard]] Standard_EXPORT int NbPoles() const;

  //! Returns the poles of the n-dimensional BSpline
  //! in the following format:
  //! [1..NumPoles][1..Dimension]
  [[nodiscard]] Standard_EXPORT const NCollection_Array2<double>& Poles() const;

  //! Returns the poles of the n-dimensional BSpline via output parameter.
  Standard_DEPRECATED("Use Poles() returning const reference instead")
  Standard_EXPORT void Poles(occ::handle<NCollection_HArray2<double>>& thePoles) const;

  //! Returns the degree of the n-dimensional BSpline.
  [[nodiscard]] Standard_EXPORT int Degree() const;

  //! Returns the number of knots of the n-dimensional BSpline.
  [[nodiscard]] Standard_EXPORT int NbKnots() const;

  //! Returns the knots of the n-dimensional BSpline.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& Knots() const;

  //! Returns the knots of the n-dimensional BSpline via output parameter.
  Standard_DEPRECATED("Use Knots() returning const reference instead")
  Standard_EXPORT void Knots(occ::handle<NCollection_HArray1<double>>& theKnots) const;

  //! Returns the multiplicities of the knots in the BSpline.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& Multiplicities() const;

  //! Returns the multiplicities of the knots via output parameter.
  Standard_DEPRECATED("Use Multiplicities() returning const reference instead")
  Standard_EXPORT void Multiplicities(occ::handle<NCollection_HArray1<int>>& theMults) const;

  //! Returns true if the conversion was successful.
  [[nodiscard]] Standard_EXPORT bool IsDone() const;

private:
  Standard_EXPORT void Perform(const int                         NumCurves,
                               const int                         MaxDegree,
                               const int                         Dimension,
                               const NCollection_Array1<int>&    NumCoeffPerCurve,
                               const NCollection_Array1<double>& Coefficients,
                               const NCollection_Array2<double>& PolynomialIntervals,
                               const NCollection_Array1<double>& TrueIntervals);

  NCollection_Array1<double> myFlatKnots;
  NCollection_Array1<double> myKnots;
  NCollection_Array1<int>    myMults;
  NCollection_Array2<double> myPoles;
  int                        myDegree;
  bool                       myDone;
};

#endif // _Convert_CompPolynomialToPoles_HeaderFile
