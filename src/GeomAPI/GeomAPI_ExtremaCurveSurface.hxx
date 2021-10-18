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

#ifndef _GeomAPI_ExtremaCurveSurface_HeaderFile
#define _GeomAPI_ExtremaCurveSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_ExtCS.hxx>
class Geom_Curve;
class Geom_Surface;
class gp_Pnt;


//! Describes functions for computing all the extrema
//! between a curve and a surface.
//! An ExtremaCurveSurface algorithm minimizes or
//! maximizes the distance between a point on the curve
//! and a point on the surface. Thus, it computes start
//! and end points of perpendiculars common to the
//! curve and the surface (an intersection point is not an
//! extremum except where the curve and the surface
//! are tangential at this point).
//! Solutions consist of pairs of points, and an extremum
//! is considered to be a segment joining the two points of a solution.
//! An ExtremaCurveSurface object provides a framework for:
//! -   defining the construction of the extrema,
//! -   implementing the construction algorithm, and
//! -   consulting the results.
//! Warning
//! In some cases, the nearest points between a curve
//! and a surface do not correspond to one of the
//! computed extrema. Instead, they may be given by:
//! -   a point of a bounding curve of the surface and one of the following:
//! -   its orthogonal projection on the curve,
//! -   a limit point of the curve; or
//! -   a limit point of the curve and its projection on the surface; or
//! -   an intersection point between the curve and the surface.
class GeomAPI_ExtremaCurveSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty algorithm for computing
  //! extrema between a curve and a surface. Use an
  //! Init function to define the curve and the surface on
  //! which it is going to work.
  Standard_EXPORT GeomAPI_ExtremaCurveSurface();
  
  //! Computes  the  extrema  distances  between  the
  //! curve <C> and the surface  <S>.
  Standard_EXPORT GeomAPI_ExtremaCurveSurface(const Handle(Geom_Curve)& Curve, const Handle(Geom_Surface)& Surface);
  
  //! Computes  the  extrema  distances  between  the
  //! curve <C>  and the  surface  <S>.  The solution
  //! point are computed in the domain [Wmin,Wmax] of
  //! the  curve   and  in  the  domain   [Umin,Umax]
  //! [Vmin,Vmax] of the surface.
  //! Warning
  //! Use the function NbExtrema to obtain the number
  //! of solutions. If this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT GeomAPI_ExtremaCurveSurface(const Handle(Geom_Curve)& Curve, const Handle(Geom_Surface)& Surface, const Standard_Real Wmin, const Standard_Real Wmax, const Standard_Real Umin, const Standard_Real Umax, const Standard_Real Vmin, const Standard_Real Vmax);
  
  //! Computes  the  extrema  distances  between  the
  //! curve <C> and the surface  <S>.
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Curve, const Handle(Geom_Surface)& Surface);
  
  //! Computes  the  extrema  distances  between  the
  //! curve <C>  and the  surface  <S>.  The solution
  //! point are computed in the domain [Wmin,Wmax] of
  //! the  curve   and  in  the  domain   [Umin,Umax]
  //! [Vmin,Vmax] of the surface.
  //! Warning
  //! Use the function NbExtrema to obtain the number
  //! of solutions. If this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Curve, const Handle(Geom_Surface)& Surface, const Standard_Real Wmin, const Standard_Real Wmax, const Standard_Real Umin, const Standard_Real Umax, const Standard_Real Vmin, const Standard_Real Vmax);
  
  //! Returns the number of extrema computed by this algorithm.
  //! Note: if this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT Standard_Integer NbExtrema() const;
Standard_EXPORT operator Standard_Integer() const;
  
  //! Returns the points P1 on the curve and P2 on the
  //! surface, which are the ends of the extremum of index
  //! Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Points (const Standard_Integer Index, gp_Pnt& P1, gp_Pnt& P2) const;
  
  //! Returns the parameters W of the point on the curve,
  //! and (U,V) of the point on the surface, which are the
  //! ends of the extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Parameters (const Standard_Integer Index, Standard_Real& W, Standard_Real& U, Standard_Real& V) const;
  
  //! Computes the distance between the end points of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT Standard_Real Distance (const Standard_Integer Index) const;

  //! Returns True if the curve is on a parallel surface.
  Standard_Boolean IsParallel() const
  {
    return myExtCS.IsParallel();
  }
  
  //! Returns the points PC on the curve and PS on the
  //! surface, which are the ends of the shortest extremum computed by this algorithm.
  //! Exceptions - StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void NearestPoints (gp_Pnt& PC, gp_Pnt& PS) const;
  
  //! Returns the parameters W of the point on the curve
  //! and (U,V) of the point on the surface, which are the
  //! ends of the shortest extremum computed by this algorithm.
  //! Exceptions - StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void LowerDistanceParameters (Standard_Real& W, Standard_Real& U, Standard_Real& V) const;
  
  //! Computes the distance between the end points of the
  //! shortest extremum computed by this algorithm.
  //! Exceptions - StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
Standard_EXPORT operator Standard_Real() const;
  
  //! Returns the algorithmic object from Extrema
    const Extrema_ExtCS& Extrema() const;

private:

  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtCS myExtCS;

};


#include <GeomAPI_ExtremaCurveSurface.lxx>





#endif // _GeomAPI_ExtremaCurveSurface_HeaderFile
