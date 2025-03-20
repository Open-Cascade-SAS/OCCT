// Created on: 1992-03-23
// Created by: Herve LEGRAND
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GCPnts_UniformDeflection_HeaderFile
#define _GCPnts_UniformDeflection_HeaderFile

#include <StdFail_NotDone.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_SequenceOfPnt.hxx>

class Adaptor3d_Curve;
class Adaptor2d_Curve2d;
class gp_Pnt;

//! Provides an algorithm to compute a distribution of
//! points on a 'C2' continuous curve.
//! The algorithm respects a criterion of maximum deflection between
//! the curve and the polygon that results from the computed points.
//! Note: This algorithm is relatively time consuming.
//! A GCPnts_QuasiUniformDeflection algorithm is quicker;
//! it can also work with non-'C2' continuous curves,
//! but it generates more points in the distribution.
class GCPnts_UniformDeflection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty algorithm.
  //! To define the problem to be solved, use the function Initialize.
  Standard_EXPORT GCPnts_UniformDeflection();

  //! Computes a uniform Deflection distribution of points on the curve.
  //! @param[in] theC  input 3D curve
  //! @param[in] theDeflection  target deflection
  //! @param[in] theWithControl  when TRUE, the algorithm controls the estimate deflection
  Standard_EXPORT GCPnts_UniformDeflection(const Adaptor3d_Curve& theC,
                                           const Standard_Real    theDeflection,
                                           const Standard_Boolean theWithControl = Standard_True);

  //! Computes a uniform Deflection distribution of points on the curve.
  //! @param[in] theC  input 2D curve
  //! @param[in] theDeflection  target deflection
  //! @param[in] theWithControl  when TRUE, the algorithm controls the estimate deflection
  Standard_EXPORT GCPnts_UniformDeflection(const Adaptor2d_Curve2d& theC,
                                           const Standard_Real      theDeflection,
                                           const Standard_Boolean   theWithControl = Standard_True);

  //! Computes a Uniform Deflection distribution of points on a part of the curve.
  //! @param[in] theC  input 3D curve
  //! @param[in] theDeflection  target deflection
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theWithControl  when TRUE, the algorithm controls the estimate deflection
  Standard_EXPORT GCPnts_UniformDeflection(const Adaptor3d_Curve& theC,
                                           const Standard_Real    theDeflection,
                                           const Standard_Real    theU1,
                                           const Standard_Real    theU2,
                                           const Standard_Boolean theWithControl = Standard_True);

  //! Computes a Uniform Deflection distribution of points on a part of the curve.
  //! @param[in] theC  input 2D curve
  //! @param[in] theDeflection  target deflection
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theWithControl  when TRUE, the algorithm controls the estimate deflection
  Standard_EXPORT GCPnts_UniformDeflection(const Adaptor2d_Curve2d& theC,
                                           const Standard_Real      theDeflection,
                                           const Standard_Real      theU1,
                                           const Standard_Real      theU2,
                                           const Standard_Boolean   theWithControl = Standard_True);

  //! Initialize the algorithms with 3D curve and deflection.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theDeflection,
                                  const Standard_Boolean theWithControl = Standard_True);

  //! Initialize the algorithms with 2D curve and deflection.
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theDeflection,
                                  const Standard_Boolean   theWithControl = Standard_True);

  //! Initialize the algorithms with 3D curve, deflection, parameter range.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theDeflection,
                                  const Standard_Real    theU1,
                                  const Standard_Real    theU2,
                                  const Standard_Boolean theWithControl = Standard_True);

  //! Initialize the algorithms with curve, deflection, parameter range.
  //! This and the above methods initialize (or reinitialize) this algorithm and
  //! compute a distribution of points:
  //! -   on the curve theC, or
  //! -   on the part of curve theC limited by the two parameter values theU1 and theU2,
  //! where the maximum distance between theC and the
  //! polygon that results from the points of the
  //! distribution is not greater than theDeflection.
  //! The first point of the distribution is either the origin
  //! of curve theC or the point of parameter theU1.
  //! The last point of the distribution is either the end point of
  //! curve theC or the point of parameter theU2.
  //! Intermediate points of the distribution are built using
  //! interpolations of segments of the curve limited at the 2nd degree.
  //! The construction ensures, in a first step,
  //! that the chordal deviation for this
  //! interpolation of the curve is less than or equal to theDeflection.
  //! However, it does not ensure that the chordal deviation
  //! for the curve itself is less than or equal to theDeflection.
  //! To do this a check is necessary,
  //! which may generate (second step) additional intermediate points.
  //! This check is time consuming, and can be avoided by setting theWithControl to false.
  //! Note that by default theWithControl is true and check is performed.
  //! Use the function IsDone to verify that the computation was successful,
  //! the function NbPoints() to obtain the number of points of the computed distribution,
  //! and the function Parameter to read the parameter of each point.
  //!
  //! Warning
  //! -   theC is necessary, 'C2' continuous.
  //!     This property is not checked at construction time.
  //! -   The roles of theU1 and theU2 are inverted if theU1 > theU2.
  //!
  //! Warning
  //! theC is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by either a 2D curve from
  //!     the package Geom2d (in the case of an Adaptor2d_Curve2d curve)
  //!     or a 3D curve from the package Geom (in the case of an Adaptor3d_Curve curve),
  //! -   and those required on the curve by the computation algorithm.
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theDeflection,
                                  const Standard_Real      theU1,
                                  const Standard_Real      theU2,
                                  const Standard_Boolean   theWithControl = Standard_True);

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
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformDeflection::NbPoints()");
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
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformDeflection::Parameter()");
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
  //! StdFAil_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_EXPORT gp_Pnt Value(const Standard_Integer Index) const;

  //! Returns the deflection between the curve and the
  //! polygon resulting from the points of the distribution
  //! computed by this algorithm.
  //! This value is the one given to the algorithm at the
  //! time of construction (or initialization).
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_Real Deflection() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformDeflection::Deflection()");
    return myDeflection;
  }

private:
  //! Initialize the algorithm.
  template <class TheCurve>
  void initialize(const TheCurve&        theC,
                  const Standard_Real    theDeflection,
                  const Standard_Real    theU1,
                  const Standard_Real    theU2,
                  const Standard_Boolean theWithControl);

private:
  Standard_Boolean       myDone;
  Standard_Real          myDeflection;
  TColStd_SequenceOfReal myParams;
  TColgp_SequenceOfPnt   myPoints;
};

#endif // _GCPnts_UniformDeflection_HeaderFile
