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

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <Convert_ParameterisationType.hxx>
class gp_Pnt2d;

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
//! The data may then be used to construct a
//! Geom2d_BSplineCurve curvSuper class of the following classes :
//! This abstract class implements the methods to get the geometric
//! representation of the B-spline curve equivalent to the conic.
//! The B-spline is computed at the creation time in the sub classes.
//! The B-spline curve is defined with its degree, its control points
//! (Poles), its weights, its knots and their multiplicity.
//! All the geometric entities used in this package are defined in 2D
//! space.
//! KeyWords :
//! Convert, Conic, BSplineCurve, 2D.
class Convert_ConicToBSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the degree of the BSpline curve whose data is
  //! computed in this framework.
  Standard_EXPORT int Degree() const;

  //! Returns the number of poles of the BSpline curve whose
  //! data is computed in this framework.
  Standard_EXPORT int NbPoles() const;

  //! Returns the number of knots of the BSpline curve whose
  //! data is computed in this framework.
  Standard_EXPORT int NbKnots() const;

  //! Returns true if the BSpline curve whose data is computed in
  //! this framework is periodic.
  Standard_EXPORT bool IsPeriodic() const;

  //! Returns the pole of index Index to the poles table of the
  //! BSpline curve whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the bounds of
  //! the poles table of the BSpline curve whose data is computed in this framework.
  Standard_EXPORT gp_Pnt2d Pole(const int Index) const;

  //! Returns the weight of the pole of index Index to the poles
  //! table of the BSpline curve whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the bounds of
  //! the poles table of the BSpline curve whose data is computed in this framework.
  Standard_EXPORT double Weight(const int Index) const;

  //! Returns the knot of index Index to the knots table of the
  //! BSpline curve whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the bounds of
  //! the knots table of the BSpline curve whose data is computed in this framework.
  Standard_EXPORT double Knot(const int Index) const;

  //! Returns the multiplicity of the knot of index Index to the
  //! knots table of the BSpline curve whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the bounds of
  //! the knots table of the BSpline curve whose data is computed in this framework.
  Standard_EXPORT int Multiplicity(const int Index) const;

  Standard_EXPORT void BuildCosAndSin(const Convert_ParameterisationType        Parametrisation,
                                      occ::handle<NCollection_HArray1<double>>& CosNumerator,
                                      occ::handle<NCollection_HArray1<double>>& SinNumerator,
                                      occ::handle<NCollection_HArray1<double>>& Denominator,
                                      int&                                      Degree,
                                      occ::handle<NCollection_HArray1<double>>& Knots,
                                      occ::handle<NCollection_HArray1<int>>&    Mults) const;

  Standard_EXPORT void BuildCosAndSin(const Convert_ParameterisationType        Parametrisation,
                                      const double                              UFirst,
                                      const double                              ULast,
                                      occ::handle<NCollection_HArray1<double>>& CosNumerator,
                                      occ::handle<NCollection_HArray1<double>>& SinNumerator,
                                      occ::handle<NCollection_HArray1<double>>& Denominator,
                                      int&                                      Degree,
                                      occ::handle<NCollection_HArray1<double>>& Knots,
                                      occ::handle<NCollection_HArray1<int>>&    Mults) const;

protected:
  Standard_EXPORT Convert_ConicToBSplineCurve(const int NumberOfPoles,
                                              const int NumberOfKnots,
                                              const int Degree);

  occ::handle<NCollection_HArray1<gp_Pnt2d>> poles;
  occ::handle<NCollection_HArray1<double>>   weights;
  occ::handle<NCollection_HArray1<double>>   knots;
  occ::handle<NCollection_HArray1<int>>      mults;
  int                                        degree;
  int                                        nbPoles;
  int                                        nbKnots;
  bool                                       isperiodic;
};

#endif // _Convert_ConicToBSplineCurve_HeaderFile
