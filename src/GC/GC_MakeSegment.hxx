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

#ifndef _GC_MakeSegment_HeaderFile
#define _GC_MakeSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_TrimmedCurve.hxx>

class StdFail_NotDone;
class gp_Pnt;
class gp_Lin;


//! Implements construction algorithms for a line
//! segment in 3D space.
//! Makes a segment of Line from the 2 points <P1> and <P2>.
//! The result is a Geom_TrimmedCurve curve.
//! A MakeSegment object provides a framework for:
//! -   defining the construction of the line segment,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed line segment.
class GC_MakeSegment  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a segment of Line from the 2 points <P1> and <P2>.
  //! It returns NullObject if <P1> and <P2> are confused.
  Standard_EXPORT GC_MakeSegment(const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Make a segment of Line from the line <Line1>
  //! between the two parameters U1 and U2.
  //! It returns NullObject if <U1> is equal <U2>.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& Line, const Standard_Real U1, const Standard_Real U2);
  
  //! Make a segment of Line from the line <Line1>
  //! between the point <Point> and the parameter Ulast.
  //! It returns NullObject if <U1> is equal <U2>.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& Line, const gp_Pnt& Point, const Standard_Real Ulast);
  
  //! Make a segment of Line from the line <Line1>
  //! between the two points <P1> and <P2>.
  //! It returns NullObject if <U1> is equal <U2>.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& Line, const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Returns the constructed line segment.
  Standard_EXPORT const Handle(Geom_TrimmedCurve)& Value() const;

  operator const Handle(Geom_TrimmedCurve)& () const { return Value(); }

private:
  Handle(Geom_TrimmedCurve) TheSegment;
};

#endif // _GC_MakeSegment_HeaderFile
