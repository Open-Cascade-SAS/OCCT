// Created on: 1995-09-12
// Created by: Bruno DUMORTIER
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

#ifndef _GeomAPI_IntCS_HeaderFile
#define _GeomAPI_IntCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntCurveSurface_HInter.hxx>
#include <Standard_Integer.hxx>
class Geom_Curve;
class Geom_Surface;
class gp_Pnt;

//! This class implements methods for
//! computing intersection points and  segments between a
class GeomAPI_IntCS
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty object. Use the
  //! function Perform for further initialization of the algorithm by
  //! the curve and the surface.
  Standard_EXPORT GeomAPI_IntCS();

  //! Computes the intersections between
  //! the curve C and the surface S.
  //! Warning
  //! Use function IsDone to verify that the intersections are computed successfully.
  Standard_EXPORT GeomAPI_IntCS(const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S);

  //! This function Initializes an algorithm with the curve C and the
  //! surface S and computes the intersections between C and S.
  //! Warning
  //! Use function IsDone to verify that the intersections are computed successfully.
  Standard_EXPORT void Perform(const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S);

  //! Returns true if the intersections are successfully computed.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the number of Intersection Points
  //! if IsDone returns True.
  //! else NotDone is raised.
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Returns the Intersection Point of range <Index>in case of cross intersection.
  //! Raises NotDone if the computation has failed or if
  //! the computation has not been done
  //! raises OutOfRange if Index is not in the range <1..NbPoints>
  Standard_EXPORT const gp_Pnt& Point(const Standard_Integer Index) const;

  //! Returns parameter W on the curve
  //! and (parameters U,V) on the surface of the computed intersection point
  //! of index Index in case of cross intersection.
  //! Exceptions
  //! StdFail_NotDone if intersection algorithm fails or is not initialized.
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of computed intersection points.
  Standard_EXPORT void Parameters(const Standard_Integer Index,
                                  Standard_Real&         U,
                                  Standard_Real&         V,
                                  Standard_Real&         W) const;

  //! Returns the number of computed
  //! intersection segments in case of tangential intersection.
  //! Exceptions
  //! StdFail_NotDone if the intersection algorithm fails or is not initialized.
  Standard_EXPORT Standard_Integer NbSegments() const;

  //! Returns the computed intersection
  //! segment of index Index in case of tangential intersection.
  //! Intersection segment is a portion of the initial curve tangent to surface.
  //! Exceptions
  //! StdFail_NotDone if intersection algorithm fails or is not initialized.
  //! Standard_OutOfRange if Index is not in the range [ 1,NbSegments ],
  //! where NbSegments is the number of computed intersection segments.
  Standard_EXPORT Handle(Geom_Curve) Segment(const Standard_Integer Index) const;

  //! Returns the parameters of the first (U1,V1) and the last (U2,V2) points
  //! of curve's segment on the surface in case of tangential intersection.
  //! Index is the number of computed intersection segments.
  //! Exceptions
  //! StdFail_NotDone if intersection algorithm fails or is not initialized.
  //! Standard_OutOfRange if Index is not in the range [ 1,NbSegments ],
  //! where NbSegments is the number of computed intersection segments.
  Standard_EXPORT void Parameters(const Standard_Integer Index,
                                  Standard_Real&         U1,
                                  Standard_Real&         V1,
                                  Standard_Real&         U2,
                                  Standard_Real&         V2) const;

protected:
private:
  Handle(Geom_Curve)     myCurve;
  IntCurveSurface_HInter myIntCS;
};

#endif // _GeomAPI_IntCS_HeaderFile
