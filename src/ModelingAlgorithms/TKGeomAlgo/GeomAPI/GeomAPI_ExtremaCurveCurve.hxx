// Created on: 1994-03-18
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

#ifndef _GeomAPI_ExtremaCurveCurve_HeaderFile
#define _GeomAPI_ExtremaCurveCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Extrema_ExtCC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
class Geom_Curve;

//! Describes functions for computing all the extrema
//! between two 3D curves.
//! An ExtremaCurveCurve algorithm minimizes or
//! maximizes the distance between a point on the first
//! curve and a point on the second curve. Thus, it
//! computes start and end points of perpendiculars
//! common to the two curves (an intersection point is
//! not an extremum unless the two curves are tangential at this point).
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
class GeomAPI_ExtremaCurveCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty algorithm for computing
  //! extrema between two curves. Use an Init function
  //! to define the curves on which it is going to work.
  Standard_EXPORT GeomAPI_ExtremaCurveCurve();

  //! Computes the extrema between the curves C1 and C2.
  Standard_EXPORT GeomAPI_ExtremaCurveCurve(const Handle(Geom_Curve)& C1,
                                            const Handle(Geom_Curve)& C2);

  //! Computes   the portion of the curve C1 limited by the two
  //! points of parameter (U1min,U1max), and
  //! -   the portion of the curve C2 limited by the two
  //! points of parameter (U2min,U2max).
  //! Warning
  //! Use the function NbExtrema to obtain the number
  //! of solutions. If this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT GeomAPI_ExtremaCurveCurve(const Handle(Geom_Curve)& C1,
                                            const Handle(Geom_Curve)& C2,
                                            const Standard_Real       U1min,
                                            const Standard_Real       U1max,
                                            const Standard_Real       U2min,
                                            const Standard_Real       U2max);

  //! Initializes this algorithm with the given arguments
  //! and computes the extrema between the curves C1 and C2
  Standard_EXPORT void Init(const Handle(Geom_Curve)& C1, const Handle(Geom_Curve)& C2);

  //! Initializes this algorithm with the given arguments
  //! and computes the extrema between :
  //! -   the portion of the curve C1 limited by the two
  //! points of parameter (U1min,U1max), and
  //! -   the portion of the curve C2 limited by the two
  //! points of parameter (U2min,U2max).
  //! Warning
  //! Use the function NbExtrema to obtain the number
  //! of solutions. If this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT void Init(const Handle(Geom_Curve)& C1,
                            const Handle(Geom_Curve)& C2,
                            const Standard_Real       U1min,
                            const Standard_Real       U1max,
                            const Standard_Real       U2min,
                            const Standard_Real       U2max);

  //! Returns the number of extrema computed by this algorithm.
  //! Note: if this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT Standard_Integer NbExtrema() const;
  Standard_EXPORT                  operator Standard_Integer() const;

  //! Returns the points P1 on the first curve and P2 on
  //! the second curve, which are the ends of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Points(const Standard_Integer Index, gp_Pnt& P1, gp_Pnt& P2) const;

  //! Returns the parameters U1 of the point on the first
  //! curve and U2 of the point on the second curve, which
  //! are the ends of the extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Parameters(const Standard_Integer Index,
                                  Standard_Real&         U1,
                                  Standard_Real&         U2) const;

  //! Computes the distance between the end points of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT Standard_Real Distance(const Standard_Integer Index) const;

  //! Returns True if the two curves are parallel.
  Standard_Boolean IsParallel() const { return myExtCC.IsParallel(); }

  //! Returns the points P1 on the first curve and P2 on
  //! the second curve, which are the ends of the shortest
  //! extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void NearestPoints(gp_Pnt& P1, gp_Pnt& P2) const;

  //! Returns the parameters U1 of the point on the first
  //! curve and U2 of the point on the second curve, which
  //! are the ends of the shortest extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void LowerDistanceParameters(Standard_Real& U1, Standard_Real& U2) const;

  //! Computes the distance between the end points of the
  //! shortest extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
  Standard_EXPORT               operator Standard_Real() const;

  //! return the algorithmic object from Extrema
  const Extrema_ExtCC& Extrema() const;

  //! set  in  <P1>  and <P2> the couple solution points
  //! such a the distance [P1,P2] is the minimum. taking  in  account
  //! extremity  points  of  curves.
  Standard_EXPORT Standard_Boolean TotalNearestPoints(gp_Pnt& P1, gp_Pnt& P2);

  //! set  in <U1> and <U2> the parameters of the couple
  //! solution   points  which  represents  the  total  nearest
  //! solution.
  Standard_EXPORT Standard_Boolean TotalLowerDistanceParameters(Standard_Real& U1,
                                                                Standard_Real& U2);

  //! return the distance of the total  nearest couple solution
  //! point.
  //! if <myExtCC> is not done
  Standard_EXPORT Standard_Real TotalLowerDistance();

private:
  Standard_EXPORT void TotalPerform();

  Standard_Boolean  myIsDone;
  Standard_Integer  myIndex;
  Extrema_ExtCC     myExtCC;
  GeomAdaptor_Curve myC1;
  GeomAdaptor_Curve myC2;
  Standard_Boolean  myTotalExt;
  Standard_Boolean  myIsInfinite;
  Standard_Real     myTotalDist;
  gp_Pnt            myTotalPoints[2];
  Standard_Real     myTotalPars[2];
};

#include <GeomAPI_ExtremaCurveCurve.lxx>

#endif // _GeomAPI_ExtremaCurveCurve_HeaderFile
