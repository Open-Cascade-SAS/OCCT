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

#ifndef _GC_MakeArcOfEllipse_HeaderFile
#define _GC_MakeArcOfEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_TrimmedCurve.hxx>

class gp_Elips;
class gp_Pnt;


//! Implements construction algorithms for an arc
//! of ellipse in 3D space. The result is a Geom_TrimmedCurve curve.
//! A MakeArcOfEllipse object provides a framework for:
//! -   defining the construction of the arc of ellipse,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the
//! Value function returns the constructed arc of ellipse.
class GC_MakeArcOfEllipse  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an arc of Ellipse (TrimmedCurve from Geom) from
  //! a Ellipse between two parameters Alpha1 and Alpha2.
  Standard_EXPORT GC_MakeArcOfEllipse(const gp_Elips& Elips, const Standard_Real Alpha1, const Standard_Real Alpha2, const Standard_Boolean Sense);
  
  //! Constructs an arc of Ellipse (TrimmedCurve from Geom) from
  //! a Ellipse between point <P> and the angle Alpha
  //! given in radians.
  Standard_EXPORT GC_MakeArcOfEllipse(const gp_Elips& Elips, const gp_Pnt& P, const Standard_Real Alpha, const Standard_Boolean Sense);
  
  //! Constructs an arc of Ellipse (TrimmedCurve from Geom) from
  //! a Ellipse between two points P1 and P2.
  //! The orientation of the arc of ellipse is:
  //! -   the sense of Elips if Sense is true, or
  //! -   the opposite sense if Sense is false.
  //! Notes:
  //! -   Alpha1, Alpha2 and Alpha are angle values, given in radians.
  //! -   IsDone always returns true.
  Standard_EXPORT GC_MakeArcOfEllipse(const gp_Elips& Elips, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Boolean Sense);
  
  //! Returns the constructed arc of ellipse.
  Standard_EXPORT const Handle(Geom_TrimmedCurve)& Value() const;

  operator const Handle(Geom_TrimmedCurve)& () const { return Value(); }

private:
  Handle(Geom_TrimmedCurve) TheArc;
};

#endif // _GC_MakeArcOfEllipse_HeaderFile
