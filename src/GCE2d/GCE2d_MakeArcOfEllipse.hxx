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

#ifndef _GCE2d_MakeArcOfEllipse_HeaderFile
#define _GCE2d_MakeArcOfEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class StdFail_NotDone;
class gp_Elips2d;
class gp_Pnt2d;


//! Implements construction algorithms for an arc of
//! ellipse in the plane. The result is a Geom2d_TrimmedCurve curve.
//! A MakeArcOfEllipse object provides a framework for:
//! -   defining the construction of the arc of ellipse,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed arc of ellipse.
class GCE2d_MakeArcOfEllipse  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make an arc of Ellipse (TrimmedCurve from Geom2d) from
  //! a Ellipse between two parameters Alpha1 and Alpha2.
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& Elips, const Standard_Real Alpha1, const Standard_Real Alpha2, const Standard_Boolean Sense = Standard_True);
  
  //! Make an arc of Ellipse (TrimmedCurve from Geom2d) from
  //! a Ellipse between point <P> and the parameter
  //! Alpha.
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& Elips, const gp_Pnt2d& P, const Standard_Real Alpha, const Standard_Boolean Sense = Standard_True);
  
  //! Make an arc of Ellipse (TrimmedCurve from Geom2d) from
  //! a Ellipse between two points P1 and P2.
  //! Please, note: The orientation of the arc is:
  //! -   the trigonometric sense if Sense is not defined or
  //! is true (default value), or
  //! -   the opposite sense if Sense is false.
  //! -   Alpha1, Alpha2 and Alpha are angle values, given in radians.
  //! -   IsDone always returns true.
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& Elips, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Boolean Sense = Standard_True);
  
  //! Returns the constructed arc of ellipse.
  Standard_EXPORT const Handle(Geom2d_TrimmedCurve)& Value() const;

  operator const Handle(Geom2d_TrimmedCurve)& () const { return Value(); }

private:
  Handle(Geom2d_TrimmedCurve) TheArc;
};

#endif // _GCE2d_MakeArcOfEllipse_HeaderFile
