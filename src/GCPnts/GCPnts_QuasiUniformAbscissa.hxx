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

class Standard_DomainError;
class Standard_ConstructionError;
class Standard_OutOfRange;
class StdFail_NotDone;
class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! This class provides an algorithm to compute a uniform abscissa
//! distribution of points on a curve, i.e. a sequence of
//! equidistant points. The distance between two
//! consecutive points is measured along the curve.
//! The distribution is defined:
//! -   either by the curvilinear distance between two consecutive points
//! -   or by a number of points.
class GCPnts_QuasiUniformAbscissa 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty algorithm. To define the problem
  //! to be solved, use the function Initialize.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa();
  
  //! Computes a uniform abscissa distribution of points
  //! -   on the curve C where Abscissa is the curvilinear distance between
  //! two consecutive points of the distribution.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor3d_Curve& C, const Standard_Integer NbPoints);
  
  //! Computes a uniform abscissa distribution of points
  //! on the part of curve C limited by the two parameter values U1 and U2,
  //! where Abscissa is the curvilinear distance between
  //! two consecutive points of the distribution.
  //! The first point of the distribution is either the origin of
  //! curve C or the point of parameter U1. The following
  //! points are computed such that the curvilinear
  //! distance between two consecutive points is equal to Abscissa.
  //! The last point of the distribution is either the end
  //! point of curve C or the point of parameter U2.
  //! However the curvilinear distance between this last
  //! point and the point just preceding it in the distribution
  //! is, of course, generally not equal to Abscissa.
  //! Use the function IsDone to verify that the
  //! computation was successful, the function NbPoints
  //! to obtain the number of points of the computed
  //! distribution, and the function Parameter to read the
  //! parameter of each point.
  //! Warning
  //! The roles of U1 and U2 are inverted if U1 > U2 .
  //! Warning
  //! C is an adapted curve, that is, an object which is an
  //! interface between:
  //! -   the services provided by either a 2D curve from
  //! the package Geom2d (in the case of an
  //! Adaptor2d_Curve2d curve) or a 3D curve from
  //! the package Geom (in the case of an Adaptor3d_Curve curve),
  //! -   and those required on the curve by the computation algorithm.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2);
  
  //! Initialize the algoritms with <C>, <NbPoints> and
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbPoints);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2);
  
  //! Computes a uniform abscissa distribution of points on
  //! the Curve2d <C>.
  //! <NbPoints> defines the nomber of desired points.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints);
  
  //! Computes a Uniform abscissa distribution of points
  //! on a part of the Curve2d <C>.
  Standard_EXPORT GCPnts_QuasiUniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2);
  
  //! Initialize the algoritms with <C>, <NbPoints> and
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2);
  
  //! Returns true if the computation was successful.
  //! IsDone is a protection against:
  //! -   non-convergence of the algorithm
  //! -   querying the results before computation.
  Standard_Boolean IsDone () const
  {
    return myDone;
  }

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
  Standard_Integer NbPoints () const
  {
    StdFail_NotDone_Raise_if (!myDone, "GCPnts_QuasiUniformAbscissa::NbPoints()");
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
  Standard_Real Parameter (const Standard_Integer Index) const
  {
    StdFail_NotDone_Raise_if (!myDone, "GCPnts_QuasiUniformAbscissa::Parameter()");
    return myParams->Value (Index);
  }

private:
  Standard_Boolean myDone;
  Standard_Integer myNbPoints;
  Handle(TColStd_HArray1OfReal) myParams;
};

#endif // _GCPnts_QuasiUniformAbscissa_HeaderFile
