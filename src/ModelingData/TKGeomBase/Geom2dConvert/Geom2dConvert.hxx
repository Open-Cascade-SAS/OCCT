// Created on: 1991-10-03
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

#ifndef _Geom2dConvert_HeaderFile
#define _Geom2dConvert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Convert_ParameterisationType.hxx>
#include <TColGeom2d_Array1OfBSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <TColStd_HArray1OfInteger.hxx>
class Geom2d_BSplineCurve;
class Geom2d_Curve;

//! This package provides an implementation of algorithms to do
//! the conversion between equivalent geometric entities from
//! package Geom2d.
//! It gives the possibility :
//! . to obtain the B-spline representation of bounded curves.
//! . to split a B-spline curve into several B-spline curves
//! with some constraints of continuity,
//! . to convert a B-spline curve into several Bezier curves
//! or surfaces.
//! All the geometric entities used in this package are bounded.
//! References :
//! . Generating the Bezier Points of B-spline curves and surfaces
//! (Wolfgang Bohm) CAGD volume 13 number 6 november 1981
//! . On NURBS: A Survey  (Leslie Piegl) IEEE Computer Graphics and
//! Application January 1991
//! . Curve and surface construction using rational B-splines
//! (Leslie Piegl and Wayne Tiller) CAD Volume 19 number 9 november
//! 1987
//! . A survey of curve and surface methods in CAGD (Wolfgang BOHM)
//! CAGD 1 1984
class Geom2dConvert
{
public:
  DEFINE_STANDARD_ALLOC

  //! -- Convert a curve to BSpline  by Approximation
  //!
  //! This method computes the arc of B-spline curve between the two
  //! knots FromK1 and ToK2.  If C is periodic the arc has the same
  //! orientation as C if SameOrientation = Standard_True.
  //! If C is not periodic  SameOrientation is not used for the
  //! computation and C is oriented from the knot fromK1 to the
  //! knot toK2.
  //! We just keep the local definition of C between the knots
  //! FromK1 and ToK2.  The returned B-spline curve has its first
  //! and last knots with a multiplicity equal to degree + 1, where
  //! degree is the polynomial degree of C.
  //! The indexes of the knots FromK1 and ToK2 doesn't include the
  //! repetition of multiple knots in their definition.
  //!
  //! Raised if FromK1 or ToK2 are out of the bounds
  //! [FirstUKnotIndex, LastUKnotIndex]
  //! Raised if FromK1 = ToK2
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) SplitBSplineCurve(
    const Handle(Geom2d_BSplineCurve)& C,
    const Standard_Integer             FromK1,
    const Standard_Integer             ToK2,
    const Standard_Boolean             SameOrientation = Standard_True);

  //! This function computes the segment of B-spline curve between the
  //! parametric values FromU1, ToU2.
  //! If C is periodic the arc has the same orientation as C if
  //! SameOrientation = True.
  //! If C is not periodic SameOrientation is not used for the
  //! computation and C is oriented fromU1 toU2.
  //! If U1 and U2 and two parametric values we consider that
  //! U1 = U2 if Abs (U1 - U2) <= ParametricTolerance and
  //! ParametricTolerance must  be greater or equal to Resolution
  //! from package gp.
  //!
  //! Raised if FromU1 or ToU2 are out of the parametric bounds of the
  //! curve (The tolerance criterion is ParametricTolerance).
  //! Raised if Abs (FromU1 - ToU2) <= ParametricTolerance
  //! Raised if ParametricTolerance < Resolution from gp.
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) SplitBSplineCurve(
    const Handle(Geom2d_BSplineCurve)& C,
    const Standard_Real                FromU1,
    const Standard_Real                ToU2,
    const Standard_Real                ParametricTolerance,
    const Standard_Boolean             SameOrientation = Standard_True);

  //! This function converts a non infinite curve from
  //! Geom into a  B-spline curve.  C must  be  an ellipse or a
  //! circle or a trimmed conic  or a trimmed  line or a Bezier
  //! curve or a trimmed  Bezier curve or a  BSpline curve or  a
  //! trimmed BSpline   curve  or an  Offset  curve or a  trimmed
  //! Offset curve.
  //! The returned B-spline is not periodic except if C is a
  //! Circle or an Ellipse.
  //! ParameterisationType applies only if the curve is a Circle
  //! or an ellipse :
  //! TgtThetaOver2,
  //! TgtThetaOver2_1,
  //! TgtThetaOver2_2,
  //! TgtThetaOver2_3,
  //! TgtThetaOver2_4,
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
  //! with TgtThetaOver2  the routine will compute the number of spans
  //! using the rule num_spans = [ (ULast - UFirst) / 1.2 ] + 1
  //! with TgtThetaOver2_N, N  spans will be forced: an error will
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
  //! Standard_DomainError if the curve C is infinite.
  //! Standard_ConstructionError:
  //! -   if C is a complete circle or ellipse, and if
  //! Parameterisation is not equal to
  //! Convert_TgtThetaOver2 or to Convert_RationalC1, or
  //! -   if C is a trimmed circle or ellipse and if
  //! Parameterisation is equal to
  //! Convert_TgtThetaOver2_1 and if U2 - U1 >
  //! 0.9999 * Pi where U1 and U2 are
  //! respectively the first and the last parameters of the
  //! trimmed curve (this method of parameterization
  //! cannot be used to convert a half-circle or a
  //! half-ellipse, for example), or
  //! -   if C is a trimmed circle or ellipse and
  //! Parameterisation is equal to
  //! Convert_TgtThetaOver2_2 and U2 - U1 >
  //! 1.9999 * Pi where U1 and U2 are
  //! respectively the first and the last parameters of the
  //! trimmed curve (this method of parameterization
  //! cannot be used to convert a quasi-complete circle or ellipse).
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) CurveToBSplineCurve(
    const Handle(Geom2d_Curve)&        C,
    const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);

  //! This Method concatenates G1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the  biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag     indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatG1(
    TColGeom2d_Array1OfBSplineCurve&          ArrayOfCurves,
    const TColStd_Array1OfReal&               ArrayOfToler,
    Handle(TColGeom2d_HArray1OfBSplineCurve)& ArrayOfConcatenated,
    Standard_Boolean&                         ClosedFlag,
    const Standard_Real                       ClosedTolerance);

  //! This Method concatenates C1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the  biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag     indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatC1(
    TColGeom2d_Array1OfBSplineCurve&          ArrayOfCurves,
    const TColStd_Array1OfReal&               ArrayOfToler,
    Handle(TColStd_HArray1OfInteger)&         ArrayOfIndices,
    Handle(TColGeom2d_HArray1OfBSplineCurve)& ArrayOfConcatenated,
    Standard_Boolean&                         ClosedFlag,
    const Standard_Real                       ClosedTolerance);

  //! This Method concatenates C1 the ArrayOfCurves as far
  //! as it is possible.
  //! ArrayOfCurves[0..N-1]
  //! ArrayOfToler contains the  biggest tolerance of the two
  //! points shared by two consecutives curves.
  //! Its dimension: [0..N-2]
  //! ClosedFlag     indicates if the ArrayOfCurves is closed.
  //! In this case ClosedTolerance contains the biggest tolerance
  //! of the two points which are at the closure.
  //! Otherwise its value is 0.0
  //! ClosedFlag becomes False on the output
  //! if it is impossible to build closed curve.
  Standard_EXPORT static void ConcatC1(
    TColGeom2d_Array1OfBSplineCurve&          ArrayOfCurves,
    const TColStd_Array1OfReal&               ArrayOfToler,
    Handle(TColStd_HArray1OfInteger)&         ArrayOfIndices,
    Handle(TColGeom2d_HArray1OfBSplineCurve)& ArrayOfConcatenated,
    Standard_Boolean&                         ClosedFlag,
    const Standard_Real                       ClosedTolerance,
    const Standard_Real                       AngularTolerance);

  //! This Method reduces as   far as it  is possible the
  //! multiplicities of  the  knots of  the BSpline BS.(keeping  the geometry).
  //! It returns a new BSpline which could still be C0.
  //! tolerance is a geometrical tolerance
  Standard_EXPORT static void C0BSplineToC1BSplineCurve(Handle(Geom2d_BSplineCurve)& BS,
                                                        const Standard_Real          Tolerance);

  //! This Method   reduces as far  as  it is possible  the
  //! multiplicities  of  the knots  of the BSpline  BS.(keeping the geometry).
  //! It returns an array of BSpline C1.
  //! Tolerance is a geometrical tolerance
  Standard_EXPORT static void C0BSplineToArrayOfC1BSplineCurve(
    const Handle(Geom2d_BSplineCurve)&        BS,
    Handle(TColGeom2d_HArray1OfBSplineCurve)& tabBS,
    const Standard_Real                       Tolerance);

  //! This Method   reduces as far  as  it is possible  the
  //! multiplicities  of  the knots  of the BSpline  BS.(keeping the geometry).
  //! It returns an array of BSpline C1.
  //! tolerance is a geometrical tolerance
  Standard_EXPORT static void C0BSplineToArrayOfC1BSplineCurve(
    const Handle(Geom2d_BSplineCurve)&        BS,
    Handle(TColGeom2d_HArray1OfBSplineCurve)& tabBS,
    const Standard_Real                       AngularTolerance,
    const Standard_Real                       Tolerance);
};

#endif // _Geom2dConvert_HeaderFile
