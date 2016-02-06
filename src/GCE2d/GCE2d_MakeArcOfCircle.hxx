// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GCE2d_MakeArcOfCircle_HeaderFile
#define _GCE2d_MakeArcOfCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class StdFail_NotDone;
class gp_Circ2d;
class gp_Pnt2d;
class gp_Vec2d;


//! Implements construction algorithms for an arc of
//! circle in the plane. The result is a Geom2d_TrimmedCurve curve.
//! A MakeArcOfCircle object provides a framework for:
//! -   defining the construction of the arc of circle,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed arc of circle.
class GCE2d_MakeArcOfCircle  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Makes an arc of circle (TrimmedCurve from Geom2d) from
  //! a circle between two parameters Alpha1 and Alpha2.
  //! The two parameters are angles. The parameters are
  //! in radians.
  Standard_EXPORT GCE2d_MakeArcOfCircle(const gp_Circ2d& Circ, const Standard_Real Alpha1, const Standard_Real Alpha2, const Standard_Boolean Sense = Standard_True);
  
  //! Makes an arc of circle (TrimmedCurve from Geom2d) from
  //! a circle between point <P> and the parameter
  //! Alpha. Alpha is given in radians.
  Standard_EXPORT GCE2d_MakeArcOfCircle(const gp_Circ2d& Circ, const gp_Pnt2d& P, const Standard_Real Alpha, const Standard_Boolean Sense = Standard_True);
  
  //! Makes an arc of circle (TrimmedCurve from Geom2d) from
  //! a circle between two points P1 and P2.
  Standard_EXPORT GCE2d_MakeArcOfCircle(const gp_Circ2d& Circ, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Boolean Sense = Standard_True);
  
  //! Makes an arc of circle (TrimmedCurve from Geom2d) from
  //! three points P1,P2,P3 between two points P1 and P3,
  //! and passing through the point P2.
  Standard_EXPORT GCE2d_MakeArcOfCircle(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const gp_Pnt2d& P3);
  
  //! Makes an arc of circle (TrimmedCurve from Geom2d) from
  //! two points P1,P2 and the tangente to the solution at
  //! the point P1.
  Standard_EXPORT GCE2d_MakeArcOfCircle(const gp_Pnt2d& P1, const gp_Vec2d& V, const gp_Pnt2d& P2);
  
  //! Returns the constructed arc of circle.
  //! Exceptions StdFail_NotDone if no arc of circle is constructed.
  Standard_EXPORT const Handle(Geom2d_TrimmedCurve)& Value() const;

  operator const Handle(Geom2d_TrimmedCurve)& () const { return Value(); }

private:
  Handle(Geom2d_TrimmedCurve) TheArc;
};

#endif // _GCE2d_MakeArcOfCircle_HeaderFile
