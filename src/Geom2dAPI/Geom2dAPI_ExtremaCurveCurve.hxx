// Created on: 1994-03-23
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dAPI_ExtremaCurveCurve_HeaderFile
#define _Geom2dAPI_ExtremaCurveCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_ExtCC2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
class Geom2d_Curve;
class gp_Pnt2d;


//! Describes functions for computing all the extrema
//! between two 2D curves.
//! An ExtremaCurveCurve algorithm minimizes or
//! maximizes the distance between a point on the first
//! curve and a point on the second curve. Thus, it
//! computes the start point and end point of
//! perpendiculars common to the two curves (an
//! intersection point is not an extremum except where
//! the two curves are tangential at this point).
//! Solutions consist of pairs of points, and an extremum
//! is considered to be a segment joining the two points of a solution.
//! An ExtremaCurveCurve object provides a framework for:
//! -   defining the construction of the extrema,
//! -   implementing the construction algorithm, and
//! -   consulting the results.
//! Warning
//! In some cases, the nearest points between two
//! curves do not correspond to one of the computed
//! extrema. Instead, they may be given by:
//! -   a limit point of one curve and one of the following:
//! -   its orthogonal projection on the other curve,
//! -   a limit point of the other curve; or
//! -   an intersection point between the two curves.
class Geom2dAPI_ExtremaCurveCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes the extrema between
  //! -   the portion of the curve C1 limited by the two
  //! points of parameter (U1min,U1max), and
  //! -   the portion of the curve C2 limited by the two
  //! points of parameter (U2min,U2max).
  //! Warning
  //! Use the function NbExtrema to obtain the number
  //! of solutions. If this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT Geom2dAPI_ExtremaCurveCurve(const Handle(Geom2d_Curve)& C1, const Handle(Geom2d_Curve)& C2, const Standard_Real U1min, const Standard_Real U1max, const Standard_Real U2min, const Standard_Real U2max);
  
  //! Returns the number of extrema computed by this algorithm.
  //! Note: if this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT Standard_Integer NbExtrema() const;
Standard_EXPORT operator Standard_Integer() const;
  
  //! Returns the points P1 on the first curve and P2 on
  //! the second curve, which are the ends of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Points (const Standard_Integer Index, gp_Pnt2d& P1, gp_Pnt2d& P2) const;
  
  //! Returns the parameters U1 of the point on the first
  //! curve and U2 of the point on the second curve, which
  //! are the ends of the extremum of index Index
  //! computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Parameters (const Standard_Integer Index, Standard_Real& U1, Standard_Real& U2) const;
  
  //! Computes the distance between the end points of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT Standard_Real Distance (const Standard_Integer Index) const;
  
  //! Returns the points P1 on the first curve and P2 on
  //! the second curve, which are the ends of the shortest
  //! extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void NearestPoints (gp_Pnt2d& P1, gp_Pnt2d& P2) const;
  
  //! Returns the parameters U1 of the point on the first
  //! curve and U2 of the point on the second curve, which
  //! are the ends of the shortest extremum computed by this algorithm.
  //! Exceptions
  //! StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void LowerDistanceParameters (Standard_Real& U1, Standard_Real& U2) const;
  
  //! Computes the distance between the end points of the
  //! shortest extremum computed by this algorithm.
  //! Exceptions - StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
Standard_EXPORT operator Standard_Real() const;
  
    const Extrema_ExtCC2d& Extrema() const;




protected:





private:



  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtCC2d myExtCC;
  Geom2dAdaptor_Curve myC1;
  Geom2dAdaptor_Curve myC2;


};


#include <Geom2dAPI_ExtremaCurveCurve.lxx>





#endif // _Geom2dAPI_ExtremaCurveCurve_HeaderFile
