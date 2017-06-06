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

#ifndef _GeomAPI_ExtremaSurfaceSurface_HeaderFile
#define _GeomAPI_ExtremaSurfaceSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_ExtSS.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class Geom_Surface;
class gp_Pnt;
class Extrema_ExtSS;


//! Describes functions for computing all the extrema
//! between two surfaces.
//! An ExtremaSurfaceSurface algorithm minimizes or
//! maximizes the distance between a point on the first
//! surface and a point on the second surface. Results
//! are start and end points of perpendiculars common to the two surfaces.
//! Solutions consist of pairs of points, and an extremum
//! is considered to be a segment joining the two points of a solution.
//! An ExtremaSurfaceSurface object provides a framework for:
//! -   defining the construction of the extrema,
//! -   implementing the construction algorithm, and
//! -   consulting the results.
//! Warning
//! In some cases, the nearest points between the two
//! surfaces do not correspond to one of the computed
//! extrema. Instead, they may be given by:
//! -   a point of a bounding curve of one surface and one of the following:
//! -   its orthogonal projection on the other surface,
//! -   a point of a bounding curve of the other surface; or
//! -   any point on intersection curves between the two surfaces.
class GeomAPI_ExtremaSurfaceSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty algorithm for computing
  //! extrema between two surfaces. Use an Init function
  //! to define the surfaces on which it is going to work.
  Standard_EXPORT GeomAPI_ExtremaSurfaceSurface();
  
  //! Computes  the  extrema  distances  between  the
  //! surfaces <S1>  and <S2>
  Standard_EXPORT GeomAPI_ExtremaSurfaceSurface(const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2);
  
  //! Computes  the  extrema  distances  between
  //! the portion of the surface S1 limited by the
  //! two values of parameter (U1min,U1max) in
  //! the u parametric direction, and by the two
  //! values of parameter (V1min,V1max) in the v
  //! parametric direction, and
  //! -   the portion of the surface S2 limited by the
  //! two values of parameter (U2min,U2max) in
  //! the u parametric direction, and by the two
  //! values of parameter (V2min,V2max) in the v
  //! parametric direction.
  Standard_EXPORT GeomAPI_ExtremaSurfaceSurface(const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real U1min, const Standard_Real U1max, const Standard_Real V1min, const Standard_Real V1max, const Standard_Real U2min, const Standard_Real U2max, const Standard_Real V2min, const Standard_Real V2max);
  
  //! Initializes this algorithm with the given arguments
  //! and computes  the  extrema  distances  between  the
  //! surfaces <S1>  and <S2>
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2);
  
  //! Initializes this algorithm with the given arguments
  //! and computes  the  extrema  distances  between -
  //! the portion of the surface S1 limited by the two
  //! values of parameter (U1min,U1max) in the u
  //! parametric direction, and by the two values of
  //! parameter (V1min,V1max) in the v parametric direction, and
  //! -   the portion of the surface S2 limited by the two
  //! values of parameter (U2min,U2max) in the u
  //! parametric direction, and by the two values of
  //! parameter (V2min,V2max) in the v parametric direction.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real U1min, const Standard_Real U1max, const Standard_Real V1min, const Standard_Real V1max, const Standard_Real U2min, const Standard_Real U2max, const Standard_Real V2min, const Standard_Real V2max);
  
  //! Returns the number of extrema computed by this algorithm.
  //! Note: if this algorithm fails, NbExtrema returns 0.
  Standard_EXPORT Standard_Integer NbExtrema() const;
Standard_EXPORT operator Standard_Integer() const;
  
  //! Returns the points P1 on the first surface and P2 on
  //! the second surface, which are the ends of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Points (const Standard_Integer Index, gp_Pnt& P1, gp_Pnt& P2) const;
  
  //! Returns the parameters (U1,V1) of the point on the
  //! first surface, and (U2,V2) of the point on the second
  //! surface, which are the ends of the extremum of index
  //! Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT void Parameters (const Standard_Integer Index, Standard_Real& U1, Standard_Real& V1, Standard_Real& U2, Standard_Real& V2) const;
  
  //! Computes the distance between the end points of the
  //! extremum of index Index computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [
  //! 1,NbExtrema ], where NbExtrema is the
  //! number of extrema computed by this algorithm.
  Standard_EXPORT Standard_Real Distance (const Standard_Integer Index) const;
  
  //! Returns the points P1 on the first surface and P2 on
  //! the second surface, which are the ends of the
  //! shortest extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void NearestPoints (gp_Pnt& P1, gp_Pnt& P2) const;
  
  //! Returns the parameters (U1,V1) of the point on the
  //! first surface and (U2,V2) of the point on the second
  //! surface, which are the ends of the shortest extremum
  //! computed by this algorithm.
  //! Exceptions - StdFail_NotDone if this algorithm fails.
  Standard_EXPORT void LowerDistanceParameters (Standard_Real& U1, Standard_Real& V1, Standard_Real& U2, Standard_Real& V2) const;
  
  //! Computes the distance between the end points of the
  //! shortest extremum computed by this algorithm.
  //! Exceptions StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
Standard_EXPORT operator Standard_Real() const;
  
  //! return the algorithmic object from Extrema
    const Extrema_ExtSS& Extrema() const;

private:

  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtSS myExtSS;

};


#include <GeomAPI_ExtremaSurfaceSurface.lxx>





#endif // _GeomAPI_ExtremaSurfaceSurface_HeaderFile
