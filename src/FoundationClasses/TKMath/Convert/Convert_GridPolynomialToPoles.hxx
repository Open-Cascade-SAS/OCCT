// Created on: 1996-07-08
// Created by: Philippe MANGIN
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

#ifndef _Convert_GridPolynomialToPoles_HeaderFile
#define _Convert_GridPolynomialToPoles_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//! Convert a grid of Polynomial Surfaces
//! that are have continuity CM to an
//! Bspline Surface that has continuity
//! CM
class Convert_GridPolynomialToPoles
{
public:
  DEFINE_STANDARD_ALLOC

  //! To only one polynomial Surface.
  //! The Length of <PolynomialUIntervals> and <PolynomialVIntervals>
  //! have to be 2.
  //! This values defined the parametric domain of the Polynomial Equation.
  //!
  //! Coefficients:
  //! The <Coefficients> have to be formatted than an "C array"
  //! [MaxUDegree+1] [MaxVDegree+1] [3]
  Standard_EXPORT Convert_GridPolynomialToPoles(
    const int                                       MaxUDegree,
    const int                                       MaxVDegree,
    const occ::handle<NCollection_HArray1<int>>&    NumCoeff,
    const occ::handle<NCollection_HArray1<double>>& Coefficients,
    const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
    const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals);

  //! To one grid of polynomial Surface.
  //! Warning!
  //! Continuity in each parametric direction can be at MOST the
  //! maximum degree of the polynomial functions.
  //!
  //! <TrueUIntervals>, <TrueVIntervals> :
  //! this is the true parameterisation for the composite surface
  //!
  //! Coefficients:
  //! The Coefficients have to be formatted than an "C array"
  //! [NbVSurfaces] [NBUSurfaces] [MaxUDegree+1] [MaxVDegree+1] [3]
  //! raises DomainError if <NumCoeffPerSurface> is not a
  //! [1, NbVSurfaces*NbUSurfaces, 1,2] array.
  //! if <Coefficients> is not a
  Standard_EXPORT Convert_GridPolynomialToPoles(
    const int                                       NbUSurfaces,
    const int                                       NBVSurfaces,
    const int                                       UContinuity,
    const int                                       VContinuity,
    const int                                       MaxUDegree,
    const int                                       MaxVDegree,
    const occ::handle<NCollection_HArray2<int>>&    NumCoeffPerSurface,
    const occ::handle<NCollection_HArray1<double>>& Coefficients,
    const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
    const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals,
    const occ::handle<NCollection_HArray1<double>>& TrueUIntervals,
    const occ::handle<NCollection_HArray1<double>>& TrueVIntervals);

  //! Returns the number of poles in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int NbUPoles() const;

  //! Returns the number of poles in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int NbVPoles() const;

  //! Returns the poles of the BSpline Surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array2<gp_Pnt>& Poles() const;

  //! Returns the degree in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int UDegree() const;

  //! Returns the degree in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int VDegree() const;

  //! Returns the number of knots in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int NbUKnots() const;

  //! Returns the number of knots in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int NbVKnots() const;

  //! Returns the knots in the U direction.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& UKnots() const;

  //! Returns the knots in the V direction.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& VKnots() const;

  //! Returns the multiplicities of the knots in the U direction.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& UMultiplicities() const;

  //! Returns the multiplicities of the knots in the V direction.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& VMultiplicities() const;

  //! Returns true if the conversion was successful.
  [[nodiscard]] Standard_EXPORT bool IsDone() const;

private:
  Standard_EXPORT void Perform(const int                                       UContinuity,
                               const int                                       VContinuity,
                               const int                                       MaxUDegree,
                               const int                                       MaxVDegree,
                               const occ::handle<NCollection_HArray2<int>>&    NumCoeffPerSurface,
                               const occ::handle<NCollection_HArray1<double>>& Coefficients,
                               const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
                               const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals,
                               const occ::handle<NCollection_HArray1<double>>& TrueUIntervals,
                               const occ::handle<NCollection_HArray1<double>>& TrueVIntervals);

  Standard_EXPORT void BuildArray(const int                         Degree,
                                  const NCollection_Array1<double>& Knots,
                                  const int                         Continuity,
                                  NCollection_Array1<double>&       FlatKnots,
                                  NCollection_Array1<int>&          Mults,
                                  NCollection_Array1<double>&       Parameters) const;

private:

  NCollection_Array1<double> myUFlatKnots;
  NCollection_Array1<double> myVFlatKnots;
  NCollection_Array1<double> myUKnots;
  NCollection_Array1<double> myVKnots;
  NCollection_Array1<int>    myUMults;
  NCollection_Array1<int>    myVMults;
  NCollection_Array2<gp_Pnt> myPoles;
  int                        myUDegree;
  int                        myVDegree;
  bool                       myDone;
};

#endif // _Convert_GridPolynomialToPoles_HeaderFile
