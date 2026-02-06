// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Convert_ConicToBSplineCurve_HeaderFile
#define _Convert_ConicToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>

#include <BSplCLib_CurveData.hxx>
#include <Convert_ParameterisationType.hxx>

//! Root class for algorithms which convert a conic curve into
//! a BSpline curve (CircleToBSplineCurve, EllipseToBSplineCurve,
//! HyperbolaToBSplineCurve, ParabolaToBSplineCurve).
//! These algorithms all work on 2D curves from the gp
//! package and compute all the data needed to construct a
//! BSpline curve equivalent to the conic curve. This data consists of:
//! -   the degree of the curve,
//! -   the periodic characteristics of the curve,
//! -   a poles table with associated weights,
//! -   a knots table with associated multiplicities.
//! The abstract class ConicToBSplineCurve provides a
//! framework for storing and consulting this computed data.
class Convert_ConicToBSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the degree of the BSpline curve whose data is
  //! computed in this framework.
  [[nodiscard]] Standard_EXPORT int Degree() const;

  //! Returns the number of poles of the BSpline curve whose
  //! data is computed in this framework.
  [[nodiscard]] Standard_EXPORT int NbPoles() const;

  //! Returns the number of knots of the BSpline curve whose
  //! data is computed in this framework.
  [[nodiscard]] Standard_EXPORT int NbKnots() const;

  //! Returns true if the BSpline curve whose data is computed in
  //! this framework is periodic.
  [[nodiscard]] Standard_EXPORT bool IsPeriodic() const;

  //! Returns the pole of index Index to the poles table of the
  //! BSpline curve whose data is computed in this framework.
  //! @param[in] theIndex pole index (1-based)
  //! @return pole at the given index
  //! @throws Standard_OutOfRange if theIndex is out of bounds
  Standard_DEPRECATED("Use Poles() batch accessor instead")
  Standard_EXPORT gp_Pnt2d Pole(const int theIndex) const;

  //! Returns the weight of the pole of index Index to the poles
  //! table of the BSpline curve whose data is computed in this framework.
  //! @param[in] theIndex weight index (1-based)
  //! @return weight at the given index
  //! @throws Standard_OutOfRange if theIndex is out of bounds
  Standard_DEPRECATED("Use Weights() batch accessor instead")
  Standard_EXPORT double Weight(const int theIndex) const;

  //! Returns the knot of index Index to the knots table of the
  //! BSpline curve whose data is computed in this framework.
  //! @param[in] theIndex knot index (1-based)
  //! @return knot at the given index
  //! @throws Standard_OutOfRange if theIndex is out of bounds
  Standard_DEPRECATED("Use Knots() batch accessor instead")
  Standard_EXPORT double Knot(const int theIndex) const;

  //! Returns the multiplicity of the knot of index Index to the
  //! knots table of the BSpline curve whose data is computed in this framework.
  //! @param[in] theIndex multiplicity index (1-based)
  //! @return multiplicity at the given index
  //! @throws Standard_OutOfRange if theIndex is out of bounds
  Standard_DEPRECATED("Use Multiplicities() batch accessor instead")
  Standard_EXPORT int Multiplicity(const int theIndex) const;

  //! Returns the poles of the BSpline curve.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<gp_Pnt2d>& Poles() const;

  //! Returns the weights of the BSpline curve.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& Weights() const;

  //! Returns the knots of the BSpline curve.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& Knots() const;

  //! Returns the multiplicities of the BSpline curve.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& Multiplicities() const;

protected:
  Standard_EXPORT Convert_ConicToBSplineCurve(const int theNumberOfPoles,
                                              const int theNumberOfKnots,
                                              const int theDegree);

  Standard_EXPORT void BuildCosAndSin(const Convert_ParameterisationType theParametrisation,
                                      NCollection_Array1<double>&        theCosNumerator,
                                      NCollection_Array1<double>&        theSinNumerator,
                                      NCollection_Array1<double>&        theDenominator,
                                      int&                               theDegree,
                                      NCollection_Array1<double>&        theKnots,
                                      NCollection_Array1<int>&           theMults) const;

  Standard_EXPORT void BuildCosAndSin(const Convert_ParameterisationType theParametrisation,
                                      const double                       theUFirst,
                                      const double                       theULast,
                                      NCollection_Array1<double>&        theCosNumerator,
                                      NCollection_Array1<double>&        theSinNumerator,
                                      NCollection_Array1<double>&        theDenominator,
                                      int&                               theDegree,
                                      NCollection_Array1<double>&        theKnots,
                                      NCollection_Array1<int>&           theMults) const;

protected:
  BSplCLib_CurveData<gp_Pnt2d> myData;
};

#endif // _Convert_ConicToBSplineCurve_HeaderFile
