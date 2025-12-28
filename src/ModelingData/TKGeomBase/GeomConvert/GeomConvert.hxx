// Created on: 1991-10-03
// Created by: JeanClaude VAUTHIER
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

#ifndef _GeomConvert_HeaderFile
#define _GeomConvert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Geom_BSplineCurve;
class Geom_BSplineSurface;
class Geom_Curve;
class Geom_Surface;

//! The GeomConvert package provides some global functions as follows
//! -   converting classical Geom curves into BSpline curves,
//! -   segmenting BSpline curves, particularly at knots
//! values: this function may be used in conjunction with the
//! GeomConvert_BSplineCurveKnotSplitting
//! class to segment a BSpline curve into arcs which
//! comply with required continuity levels,
//! -   converting classical Geom surfaces into BSpline surfaces, and
//! -   segmenting BSpline surfaces, particularly at
//! knots values: this function may be used in conjunction with the
//! GeomConvert_BSplineSurfaceKnotSplitting
//! class to segment a BSpline surface into patches
//! which comply with required continuity levels.
//! All geometric entities used in this package are bounded.
//!
//! References :
//! . Generating the Bezier Points of B-spline curves and surfaces
//! (Wolfgang Bohm) CAGD volume 13 number 6 november 1981
//! . On NURBS: A Survey (Leslie Piegl) IEEE Computer Graphics and
//! Application January 1991
//! . Curve and surface construction using rational B-splines
//! (Leslie Piegl and Wayne Tiller) CAD Volume 19 number 9 november
//! 1987
//! . A survey of curve and surface methods in CAGD (Wolfgang BOHM)
//! CAGD 1 1984
class GeomConvert
{
public:
  DEFINE_STANDARD_ALLOC

  //! Convert a curve from Geom by an approximation method
  //!
  //! This method computes the arc of B-spline curve between the two
  //! knots FromK1 and ToK2. If C is periodic the arc has the same
  //! orientation as C if SameOrientation = true.
  //! If C is not periodic SameOrientation is not used for the
  //! computation and C is oriented from the knot fromK1 to the knot toK2.
  //! We just keep the local definition of C between the knots
  //! FromK1 and ToK2. The returned B-spline curve has its first
  //! and last knots with a multiplicity equal to degree + 1, where
  //! degree is the polynomial degree of C.
  //! The indexes of the knots FromK1 and ToK2 doesn't include the
  //! repetition of multiple knots in their definition.
  //! Raised if FromK1 = ToK2
  //! Raised if FromK1 or ToK2 are out of the bounds
  //! [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> SplitBSplineCurve(
    const occ::handle<Geom_BSplineCurve>& C,
    const int                             FromK1,
    const int                             ToK2,
    const bool                            SameOrientation = true);

  //! This function computes the segment of B-spline curve between the
  //! parametric values FromU1, ToU2.
  //! If C is periodic the arc has the same orientation as C if
  //! SameOrientation = True.
  //! If C is not periodic SameOrientation is not used for the
  //! computation and C is oriented fromU1 toU2.
  //! If U1 and U2 and two parametric values we consider that
  //! U1 = U2 if Abs (U1 - U2) <= ParametricTolerance and
  //! ParametricTolerance must be greater or equal to Resolution
  //! from package gp.
  //!
  //! Raised if FromU1 or ToU2 are out of the parametric bounds of the
  //! curve (The tolerance criterion is ParametricTolerance).
  //! Raised if Abs (FromU1 - ToU2) <= ParametricTolerance
  //! Raised if ParametricTolerance < Resolution from gp.
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> SplitBSplineCurve(
    const occ::handle<Geom_BSplineCurve>& C,
    const double                          FromU1,
    const double                          ToU2,
    const double                          ParametricTolerance,
    const bool                            SameOrientation = true);

  //! Computes the B-spline surface patche between the knots values
  //! FromUK1, ToUK2, FromVK1, ToVK2.
  //! If S is periodic in one direction the patche has the same
  //! orientation as S in this direction if the flag is true in this
  //! direction (SameUOrientation, SameVOrientation).
  //! If S is not periodic SameUOrientation and SameVOrientation are not
  //! used for the computation and S is oriented FromUK1 ToUK2 and
  //! FromVK1 ToVK2.
  //! Raised if
  //! FromUK1 = ToUK2 or FromVK1 = ToVK2
  //! FromUK1 or ToUK2 are out of the bounds
  //! [FirstUKnotIndex, LastUKnotIndex]
  //! FromVK1 or ToVK2 are out of the bounds
  //! [FirstVKnotIndex, LastVKnotIndex]
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> SplitBSplineSurface(
    const occ::handle<Geom_BSplineSurface>& S,
    const int                               FromUK1,
    const int                               ToUK2,
    const int                               FromVK1,
    const int                               ToVK2,
    const bool                              SameUOrientation = true,
    const bool                              SameVOrientation = true);

  //! This method splits a B-spline surface patche between the
  //! knots values FromK1, ToK2 in one direction.
  //! If USplit = True then the splitting direction is the U parametric
  //! direction else it is the V parametric direction.
  //! If S is periodic in the considered direction the patche has the
  //! same orientation as S in this direction if SameOrientation is True
  //! If S is not periodic in this direction SameOrientation is not used
  //! for the computation and S is oriented FromK1 ToK2.
  //! Raised if FromK1 = ToK2 or if
  //! FromK1 or ToK2 are out of the bounds
  //! [FirstUKnotIndex, LastUKnotIndex] in the
  //! considered parametric direction.
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> SplitBSplineSurface(
    const occ::handle<Geom_BSplineSurface>& S,
    const int                               FromK1,
    const int                               ToK2,
    const bool                              USplit,
    const bool                              SameOrientation = true);

  //! This method computes the B-spline surface patche between the
  //! parametric values FromU1, ToU2, FromV1, ToV2.
  //! If S is periodic in one direction the patche has the same
  //! orientation as S in this direction if the flag is True in this
  //! direction (SameUOrientation, SameVOrientation).
  //! If S is not periodic SameUOrientation and SameVOrientation are not
  //! used for the computation and S is oriented FromU1 ToU2 and
  //! FromV1 ToV2.
  //! If U1 and U2 and two parametric values we consider that U1 = U2 if
  //! Abs (U1 - U2) <= ParametricTolerance and ParametricTolerance must
  //! be greater or equal to Resolution from package gp.
  //!
  //! Raised if FromU1 or ToU2 or FromV1 or ToU2 are out of the
  //! parametric bounds of the surface (the tolerance criterion is
  //! ParametricTolerance).
  //! Raised if Abs (FromU1 - ToU2) <= ParametricTolerance or
  //! Abs (FromV1 - ToV2) <= ParametricTolerance.
  //! Raised if ParametricTolerance < Resolution.
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> SplitBSplineSurface(
    const occ::handle<Geom_BSplineSurface>& S,
    const double                            FromU1,
    const double                            ToU2,
    const double                            FromV1,
    const double                            ToV2,
    const double                            ParametricTolerance,
    const bool                              SameUOrientation = true,
    const bool                              SameVOrientation = true);

  //! This method splits the B-spline surface S in one direction
  //! between the parametric values FromParam1, ToParam2.
  //! If USplit = True then the Splitting direction is the U parametric
  //! direction else it is the V parametric direction.
  //! If S is periodic in the considered direction the patche has
  //! the same orientation as S in this direction if SameOrientation
  //! is true.
  //! If S is not periodic in the considered direction SameOrientation
  //! is not used for the computation and S is oriented FromParam1
  //! ToParam2.
  //! If U1 and U2 and two parametric values we consider that U1 = U2
  //! if Abs (U1 - U2) <= ParametricTolerance and ParametricTolerance
  //! must be greater or equal to Resolution from package gp.
  //!
  //! Raises if FromParam1 or ToParam2 are out of the parametric bounds
  //! of the surface in the considered direction.
  //! Raises if Abs (FromParam1 - ToParam2) <= ParametricTolerance.
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> SplitBSplineSurface(
    const occ::handle<Geom_BSplineSurface>& S,
    const double                            FromParam1,
    const double                            ToParam2,
    const bool                              USplit,
    const double                            ParametricTolerance,
    const bool                              SameOrientation = true);

  //! This function converts a non infinite curve from
  //! Geom into a B-spline curve. C must be an ellipse or a
  //! circle or a trimmed conic or a trimmed line or a Bezier
  //! curve or a trimmed Bezier curve or a BSpline curve or a
  //! trimmed BSpline curve or an OffsetCurve. The returned B-spline is
  //! not periodic except if C is a Circle or an Ellipse. If
  //! the Parameterisation is QuasiAngular than the returned
  //! curve is NOT periodic in case a periodic Geom_Circle or
  //! Geom_Ellipse. For TgtThetaOver2_1 and TgtThetaOver2_2 the
  //! method raises an exception in case of a periodic
  //! Geom_Circle or a Geom_Ellipse ParameterisationType applies
  //! only if the curve is a Circle or an ellipse:
  //! TgtThetaOver2, TgtThetaOver2_1, TgtThetaOver2_2,
  //! TgtThetaOver2_3, TgtThetaOver2_4,
  //!
  //! Purpose: this is the classical rational parameterisation
  //! 2
  //! 1 - t
  //! cos(theta) = ------
  //! 2
  //! 1 + t
  //!
  //! 2t
  //! sin(theta) = ------
  //! 2
  //! 1 + t
  //!
  //! t = tan (theta/2)
  //!
  //! with TgtThetaOver2 the routine will compute the number of spans
  //! using the rule num_spans = [ (ULast - UFirst) / 1.2 ] + 1
  //! with TgtThetaOver2_N, N spans will be forced: an error will
  //! be raized if (ULast - UFirst) >= PI and N = 1,
  //! ULast - UFirst >= 2 PI and N = 2
  //!
  //! QuasiAngular,
  //! here t is a rational function that approximates
  //! theta ----> tan(theta/2).
  //! Nevetheless the composing with above function yields exact
  //! functions whose square sum up to 1
  //! RationalC1 ;
  //! t is replaced by a polynomial function of u so as to grant
  //! C1 contiuity across knots.
  //! Exceptions
  //! Standard_DomainError:
  //! -   if the curve C is infinite, or
  //! -   if C is a (complete) circle or ellipse, and Parameterisation is equal to
  //! Convert_TgtThetaOver2_1 or Convert_TgtThetaOver2_2.
  //! Standard_ConstructionError:
  //! -   if C is a (complete) circle or ellipse, and if Parameterisation is not equal to
  //! Convert_TgtThetaOver2, Convert_RationalC1,
  //! Convert_QuasiAngular (the curve is converted
  //! in these three cases) or to Convert_TgtThetaOver2_1 or
  //! Convert_TgtThetaOver2_2 (another exception is raised in these two cases).
  //! -   if C is a trimmed circle or ellipse, if Parameterisation is equal to
  //! Convert_TgtThetaOver2_1 and if U2 - U1 > 0.9999 * Pi, where U1 and U2 are
  //! respectively the first and the last parameters of the
  //! trimmed curve (this method of parameterization
  //! cannot be used to convert a half-circle or a half-ellipse, for example), or
  //! -   if C is a trimmed circle or ellipse, if
  //! Parameterisation is equal to Convert_TgtThetaOver2_2 and U2 - U1 >
  //! 1.9999 * Pi where U1 and U2 are
  //! respectively the first and the last parameters of the
  //! trimmed curve (this method of parameterization
  //! cannot be used to convert a quasi-complete circle or ellipse).
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> CurveToBSplineCurve(
    const occ::handle<Geom_Curve>&     C,
    const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);

  //! This algorithm converts a non infinite surface from Geom
  //! into a B-spline surface.
  //! S must be a trimmed plane or a trimmed cylinder or a trimmed cone
  //! or a trimmed sphere or a trimmed torus or a sphere or a torus or
  //! a Bezier surface of a trimmed Bezier surface or a trimmed swept
  //! surface with a corresponding basis curve which can be turned into
  //! a B-spline curve (see the method CurveToBSplineCurve).
  //! Raises DomainError if the type of the surface is not previously defined.
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> SurfaceToBSplineSurface(
    const occ::handle<Geom_Surface>& S);

  //! This Method concatenates G1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatG1(
    NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
    const NCollection_Array1<double>&                                 ArrayOfToler,
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
    bool&                                                             ClosedFlag,
    const double                                                      ClosedTolerance);

  //! This Method concatenates C1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatC1(
    NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
    const NCollection_Array1<double>&                                 ArrayOfToler,
    occ::handle<NCollection_HArray1<int>>&                            ArrayOfIndices,
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
    bool&                                                             ClosedFlag,
    const double                                                      ClosedTolerance);

  //! This Method concatenates C1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatC1(
    NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
    const NCollection_Array1<double>&                                 ArrayOfToler,
    occ::handle<NCollection_HArray1<int>>&                            ArrayOfIndices,
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
    bool&                                                             ClosedFlag,
    const double                                                      ClosedTolerance,
    const double                                                      AngularTolerance);

  //! This Method reduces as far as it is possible the
  //! multiplicities of the knots of the BSpline BS.(keeping the
  //! geometry). It returns a new BSpline which could still be C0.
  //! tolerance is a geometrical tolerance.
  //! The Angular toleranceis in radians and measures the angle of
  //! the tangents on the left and on the right to decide if the
  //! curve is G1 or not at a given point
  Standard_EXPORT static void C0BSplineToC1BSplineCurve(occ::handle<Geom_BSplineCurve>& BS,
                                                        const double                    tolerance,
                                                        const double AngularTolerance = 1.0e-7);

  //! This Method reduces as far as it is possible the
  //! multiplicities of the knots of the BSpline BS.(keeping the geometry).
  //! It returns an array of BSpline C1. tolerance is a geometrical tolerance.
  Standard_EXPORT static void C0BSplineToArrayOfC1BSplineCurve(
    const occ::handle<Geom_BSplineCurve>&                             BS,
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& tabBS,
    const double                                                      tolerance);

  //! This Method reduces as far as it is possible the
  //! multiplicities of the knots of the BSpline BS.(keeping the
  //! geometry). It returns an array of BSpline C1. tolerance is a
  //! geometrical tolerance : it allows for the maximum deformation
  //! The Angular tolerance is in radians and measures the angle of
  //! the tangents on the left and on the right to decide if the curve
  //! is C1 or not at a given point
  Standard_EXPORT static void C0BSplineToArrayOfC1BSplineCurve(
    const occ::handle<Geom_BSplineCurve>&                             BS,
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& tabBS,
    const double                                                      AngularTolerance,
    const double                                                      tolerance);
};

#endif // _GeomConvert_HeaderFile
