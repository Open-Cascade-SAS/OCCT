// Created on: 1995-11-02
// Created by: Jacques GOUSSARD
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

#ifndef _GCPnts_QuasiUniformDeflection_HeaderFile
#define _GCPnts_QuasiUniformDeflection_HeaderFile

#include <StdFail_NotDone.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <GeomAbs_Shape.hxx>

class Adaptor3d_Curve;
class Adaptor2d_Curve2d;
class gp_Pnt;

//! This class computes a distribution of points on a curve.
//! The points may respect the deflection.
//! The algorithm is not based on the classical prediction (with second derivative of curve),
//! but either on the evaluation of the distance between the mid point
//! and the point of mid parameter of the two points,
//! or the distance between the mid point and the point at parameter 0.5
//! on the cubic interpolation of the two points and their tangents.
//!
//! Note: this algorithm is faster than a GCPnts_UniformDeflection algorithm,
//! and is able to work with non-"C2" continuous curves.
//! However, it generates more points in the distribution.
class GCPnts_QuasiUniformDeflection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty algorithm.
  //! To define the problem to be solved, use the function Initialize().
  Standard_EXPORT GCPnts_QuasiUniformDeflection();

  //! Computes a QuasiUniform Deflection distribution of points on the Curve.
  Standard_EXPORT GCPnts_QuasiUniformDeflection(const Adaptor3d_Curve& theC,
                                                const Standard_Real    theDeflection,
                                                const GeomAbs_Shape    theContinuity = GeomAbs_C1);

  //! Computes a QuasiUniform Deflection distribution of points on the Curve.
  Standard_EXPORT GCPnts_QuasiUniformDeflection(const Adaptor2d_Curve2d& theC,
                                                const Standard_Real      theDeflection,
                                                const GeomAbs_Shape theContinuity = GeomAbs_C1);

  //! Computes a QuasiUniform Deflection distribution of points on a part of the Curve.
  Standard_EXPORT GCPnts_QuasiUniformDeflection(const Adaptor3d_Curve& theC,
                                                const Standard_Real    theDeflection,
                                                const Standard_Real    theU1,
                                                const Standard_Real    theU2,
                                                const GeomAbs_Shape    theContinuity = GeomAbs_C1);

  //! Computes a QuasiUniform Deflection distribution of points on a part of the Curve.
  //! This and the above algorithms compute a distribution of points:
  //! -   on the curve theC, or
  //! -   on the part of curve theC limited by the two parameter values theU1 and theU2,
  //! where the deflection resulting from the distributed
  //! points is not greater than theDeflection.
  //!
  //! The first point of the distribution is either the origin of
  //! curve theC or the point of parameter theU1.
  //! The last point of the distribution is either the end point
  //! of curve theC or the point of parameter theU2.
  //!
  //! Intermediate points of the distribution are built such
  //! that the deflection is not greater than theDeflection.
  //! Using the following evaluation of the deflection:
  //! if Pi and Pj are two consecutive points of the
  //! distribution, respectively of parameter ui and uj on the curve,
  //! the deflection is the distance between:
  //! -   the mid-point of Pi and Pj (the center of the chord joining these two points)
  //! -   and the point of mid-parameter of these two
  //!     points (the point of parameter [(ui+uj) / 2] on curve theC).
  //! theContinuity, defaulted to GeomAbs_C1, gives the degree of continuity of the curve theC.
  //! (Note that C is an Adaptor3d_Curve or an Adaptor2d_Curve2d object,
  //! and does not know the degree of continuity of the underlying curve).
  //! Use the function IsDone() to verify that the computation was successful,
  //! the function NbPoints() to obtain the number of points of the computed distribution,
  //! and the function Parameter() to read the parameter of each point.
  //!
  //! Warning
  //! -   The roles of theU1 and theU2 are inverted if theU1 > theU2.
  //! -   Derivative functions on the curve are called according to theContinuity.
  //!     An error may occur if theContinuity is greater than
  //!     the real degree of continuity of the curve.
  //!
  //! Warning
  //! theC is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by either a 2D curve from
  //!     the package Geom2d (in the case of an Adaptor2d_Curve2d curve)
  //!     or a 3D curve from the package Geom (in the case of an Adaptor3d_Curve curve),
  //! -   and those required on the curve by the computation algorithm.
  Standard_EXPORT GCPnts_QuasiUniformDeflection(const Adaptor2d_Curve2d& theC,
                                                const Standard_Real      theDeflection,
                                                const Standard_Real      theU1,
                                                const Standard_Real      theU2,
                                                const GeomAbs_Shape theContinuity = GeomAbs_C1);

  //! Initialize the algorithms with 3D curve and deflection.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theDeflection,
                                  const GeomAbs_Shape    theContinuity = GeomAbs_C1);

  //! Initialize the algorithms with 2D curve and deflection.
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theDeflection,
                                  const GeomAbs_Shape      theContinuity = GeomAbs_C1);

  //! Initialize the algorithms with 3D curve, deflection and parameter range.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theDeflection,
                                  const Standard_Real    theU1,
                                  const Standard_Real    theU2,
                                  const GeomAbs_Shape    theContinuity = GeomAbs_C1);

  //! Initialize the algorithms with theC, theDeflection, theU1, theU2.
  //! This and the above algorithms initialize (or reinitialize)
  //! this algorithm and compute a distribution of points:
  //! -   on the curve theC, or
  //! -   on the part of curve theC limited by the two parameter values theU1 and theU2,
  //! where the deflection resulting from the distributed
  //! points is not greater than theDeflection.
  //!
  //! The first point of the distribution is either the origin
  //! of curve theC or the point of parameter theU1.
  //! The last point of the distribution is either the end point of
  //! curve theC or the point of parameter theU2.
  //!
  //! Intermediate points of the distribution are built in
  //! such a way that the deflection is not greater than theDeflection.
  //! Using the following evaluation of the deflection:
  //! if Pi and Pj are two consecutive points of the distribution,
  //! respectively of parameter ui and uj on the curve,
  //! the deflection is the distance between:
  //! -   the mid-point of Pi and Pj (the center of the chord joining these two points)
  //! -   and the point of mid-parameter of these two
  //!     points (the point of parameter [(ui+uj) / 2] on curve theC).
  //! theContinuity, defaulted to GeomAbs_C1, gives the degree of continuity of the curve theC.
  //! (Note that C is an Adaptor3d_Curve or an Adaptor2d_Curve2d object,
  //! and does not know the degree of continuity of the underlying curve).
  //! Use the function IsDone to verify that the computation was successful,
  //! the function NbPoints() to obtain the number of points of the computed distribution,
  //! and the function Parameter() to read the parameter of each point.
  //!
  //! Warning
  //! -   The roles of theU1 and theU2 are inverted if theU1 > theU2.
  //! -   Derivative functions on the curve are called according to theContinuity.
  //!     An error may occur if theContinuity is greater than
  //!     the real degree of continuity of the curve.
  //!
  //! Warning
  //! theC is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by either a 2D curve from
  //!     the package Geom2d (in the case of an Adaptor2d_Curve2d curve)
  //!     or a 3D curve from the package Geom (in the case of an Adaptor3d_Curve curve),
  //!     and those required on the curve by the computation algorithm.
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theDeflection,
                                  const Standard_Real      theU1,
                                  const Standard_Real      theU2,
                                  const GeomAbs_Shape      theContinuity = GeomAbs_C1);

  //! Returns true if the computation was successful.
  //! IsDone is a protection against:
  //! -   non-convergence of the algorithm
  //! -   querying the results before computation.
  Standard_Boolean IsDone() const { return myDone; }

  //! Returns the number of points of the distribution
  //! computed by this algorithm.
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_Integer NbPoints() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformDeflection::NbPoints()");
    return myParams.Length();
  }

  //! Returns the parameter of the point of index Index in
  //! the distribution computed by this algorithm.
  //! Warning
  //! Index must be greater than or equal to 1, and less
  //! than or equal to the number of points of the
  //! distribution. However, pay particular attention as this
  //! condition is not checked by this function.
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_Real Parameter(const Standard_Integer Index) const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformDeflection::Parameter()");
    return myParams(Index);
  }

  //! Returns the point of index Index in the distribution
  //! computed by this algorithm.
  //! Warning
  //! Index must be greater than or equal to 1, and less
  //! than or equal to the number of points of the
  //! distribution. However, pay particular attention as this
  //! condition is not checked by this function.
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_EXPORT gp_Pnt Value(const Standard_Integer Index) const;

  //! Returns the deflection between the curve and the
  //! polygon resulting from the points of the distribution
  //! computed by this algorithm.
  //! This is the value given to the algorithm at the time
  //! of construction (or initialization).
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_Real Deflection() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformDeflection::Deflection()");
    return myDeflection;
  }

private:
  //! Initializes algorithm.
  template <class TheCurve>
  void initialize(const TheCurve&     theC,
                  const Standard_Real theDeflection,
                  const Standard_Real theU1,
                  const Standard_Real theU2,
                  const GeomAbs_Shape theContinuity);

private:
  Standard_Boolean       myDone;
  Standard_Real          myDeflection;
  TColStd_SequenceOfReal myParams;
  TColgp_SequenceOfPnt   myPoints;
  GeomAbs_Shape          myCont;
};

#endif // _GCPnts_QuasiUniformDeflection_HeaderFile
