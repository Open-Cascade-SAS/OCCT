// Created on: 1996-08-22
// Created by: Stagiaire Mary FABIEN
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

#ifndef _GCPnts_QuasiUniformAbscissa_HeaderFile
#define _GCPnts_QuasiUniformAbscissa_HeaderFile

#include <StdFail_NotDone.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! This class provides an algorithm to compute a uniform abscissa
//! distribution of points on a curve, i.e. a sequence of equidistant points.
//! The distance between two consecutive points is measured along the curve.
//!
//! The distribution is defined by a number of points.
class GCPnts_QuasiUniformAbscissa
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty algorithm.
  //! To define the problem to be solved, use the function Initialize.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa();

  //! Computes a uniform abscissa distribution of points
  //! -   on the curve where Abscissa is the curvilinear distance between
  //! two consecutive points of the distribution.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor3d_Curve& theC,
                                              const Standard_Integer theNbPoints);

  //! Computes a uniform abscissa distribution of points
  //! on the part of curve limited by the two parameter values theU1 and theU2,
  //! where Abscissa is the curvilinear distance between
  //! two consecutive points of the distribution.
  //! The first point of the distribution is either the origin of
  //! curve or the point of parameter theU1.
  //! The following points are computed such that the curvilinear
  //! distance between two consecutive points is equal to Abscissa.
  //! The last point of the distribution is either the end
  //! point of curve or the point of parameter theU2.
  //! However the curvilinear distance between this last
  //! point and the point just preceding it in the distribution is,
  //! of course, generally not equal to Abscissa.
  //! Use the function IsDone() to verify that the computation was successful,
  //! the function NbPoints() to obtain the number of points of the computed distribution,
  //! and the function Parameter() to read the parameter of each point.
  //!
  //! Warning
  //! The roles of theU1 and theU2 are inverted if theU1 > theU2.
  //! Warning
  //! theC is an adapted curve, that is, an object which is an interface between:
  //! -   the services provided by either a 2D curve from
  //!     the package Geom2d (in the case of an Adaptor2d_Curve2d curve)
  //!     or a 3D curve from the package Geom (in the case of an Adaptor3d_Curve curve),
  //! -   and those required on the curve by the computation algorithm.
  //! @param[in] theC  input 3D curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor3d_Curve& theC,
                                              const Standard_Integer theNbPoints,
                                              const Standard_Real    theU1,
                                              const Standard_Real    theU2);

  //! Initialize the algorithms with 3D curve and target number of points.
  //! @param[in] theC  input 3D curve
  //! @param[in] theNbPoints  defines the number of desired points
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC, const Standard_Integer theNbPoints);

  //! Initialize the algorithms with 3D curve, target number of points and curve parameter range.
  //! @param[in] theC  input 3D curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Integer theNbPoints,
                                  const Standard_Real    theU1,
                                  const Standard_Real    theU2);

  //! Computes a uniform abscissa distribution of points on the 2D curve.
  //! @param[in] theC  input 2D curve
  //! @param[in] theNbPoints  defines the number of desired points
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor2d_Curve2d& theC,
                                              const Standard_Integer   theNbPoints);

  //! Computes a Uniform abscissa distribution of points on a part of the 2D curve.
  //! @param[in] theC  input 2D curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor2d_Curve2d& theC,
                                              const Standard_Integer   theNbPoints,
                                              const Standard_Real      theU1,
                                              const Standard_Real      theU2);

  //! Initialize the algorithms with 2D curve and target number of points.
  //! @param[in] theC  input 2D curve
  //! @param[in] theNbPoints  defines the number of desired points
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Integer   theNbPoints);

  //! Initialize the algorithms with 2D curve, target number of points and curve parameter range.
  //! @param[in] theC  input 2D curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Integer   theNbPoints,
                                  const Standard_Real      theU1,
                                  const Standard_Real      theU2);

  //! Returns true if the computation was successful.
  //! IsDone is a protection against:
  //! -   non-convergence of the algorithm
  //! -   querying the results before computation.
  Standard_Boolean IsDone() const { return myDone; }

  //! Returns the number of points of the distribution
  //! computed by this algorithm.
  //! This value is either:
  //! -   the one imposed on the algorithm at the time of
  //! construction (or initialization), or
  //! -   the one computed by the algorithm when the
  //! curvilinear distance between two consecutive
  //! points of the distribution is imposed on the
  //! algorithm at the time of construction (or initialization).
  //! Exceptions
  //! StdFail_NotDone if this algorithm has not been
  //! initialized, or if the computation was not successful.
  Standard_Integer NbPoints() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformAbscissa::NbPoints()");
    return myNbPoints;
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
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformAbscissa::Parameter()");
    return myParams->Value(Index);
  }

private:
  //! This function divides given curve on the several parts with equal length.
  //! It returns array of parameters in the control points.
  template <class TheCurve>
  void initialize(const TheCurve&        theC,
                  const Standard_Integer theNbPoints,
                  const Standard_Real    theU1,
                  const Standard_Real    theU2);

private:
  Standard_Boolean              myDone;
  Standard_Integer              myNbPoints;
  Handle(TColStd_HArray1OfReal) myParams;
};

#endif // _GCPnts_QuasiUniformAbscissa_HeaderFile
