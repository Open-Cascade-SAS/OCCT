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

#ifndef _GCE2d_MakeArcOfHyperbola_HeaderFile
#define _GCE2d_MakeArcOfHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class gp_Hypr2d;
class gp_Pnt2d;


//! Implements construction algorithms for an arc of
//! hyperbola in the plane. The result is a Geom2d_TrimmedCurve curve.
//! A MakeArcOfHyperbola object provides a framework for:
//! -   defining the construction of the arc of hyperbola,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed arc of hyperbola.
class GCE2d_MakeArcOfHyperbola  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Makes an arc of Hyperbola (TrimmedCurve from Geom2d) from
  //! a Hyperbola between two parameters Alpha1 and Alpha2.
  Standard_EXPORT GCE2d_MakeArcOfHyperbola(const gp_Hypr2d& Hypr, const Standard_Real Alpha1, const Standard_Real Alpha2, const Standard_Boolean Sense = Standard_True);
  
  //! Makes an arc of Hyperbola (TrimmedCurve from Geom2d) from
  //! a Hyperbola between point <P> and the parameter
  //! Alpha.
  Standard_EXPORT GCE2d_MakeArcOfHyperbola(const gp_Hypr2d& Hypr, const gp_Pnt2d& P, const Standard_Real Alpha, const Standard_Boolean Sense = Standard_True);
  
  //! Makes an arc of Hyperbola (TrimmedCurve from Geom2d) from
  //! a Hyperbola between two points P1 and P2.
  //! Note: the orientation of the arc of hyperbola is:
  //! -   the trigonometric sense if Sense is not defined or
  //! is true (default value), or
  //! -   the opposite sense if Sense is false.
  //! - IsDone always returns true.
  Standard_EXPORT GCE2d_MakeArcOfHyperbola(const gp_Hypr2d& Hypr, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Boolean Sense = Standard_True);
  
  //! Returns the constructed arc of hyperbola.
  Standard_EXPORT const Handle(Geom2d_TrimmedCurve)& Value() const;

  operator const Handle(Geom2d_TrimmedCurve)& () const { return Value(); }

private:
  Handle(Geom2d_TrimmedCurve) TheArc;
};

#endif // _GCE2d_MakeArcOfHyperbola_HeaderFile
