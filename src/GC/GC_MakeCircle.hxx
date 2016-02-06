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

#ifndef _GC_MakeCircle_HeaderFile
#define _GC_MakeCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Circle.hxx>

class StdFail_NotDone;
class gp_Circ;
class gp_Ax2;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;


//! This class implements the following algorithms used
//! to create Cirlec from Geom.
//!
//! * Create a Circle parallel to another and passing
//! though a point.
//! * Create a Circle parallel to another at the distance
//! Dist.
//! * Create a Circle passing through 3 points.
//! * Create a Circle with its center and the normal of its
//! plane and its radius.
//! * Create a Circle with its axis and radius.
//! The circle's parameter is the angle (Radian).
//! The parametrization range is [0,2*PI].
//! The circle is a closed and periodic curve.
//! The center of the circle is the Location point of its axis
//! placement. The XDirection of the axis placement defines the
//! origin of the parametrization.
class GC_MakeCircle  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates a circle from a non persistent circle C by its conversion.
  Standard_EXPORT GC_MakeCircle(const gp_Circ& C);
  

  //! A2 is the local coordinates system of the circle.
  //! It is not forbidden to create a circle with Radius = 0.0
  //! Status is "NegativeRadius" if Radius < 0.
  Standard_EXPORT GC_MakeCircle(const gp_Ax2& A2, const Standard_Real Radius);
  
  //! Make a Circle from Geom <TheCirc> parallel to another
  //! Circ <Circ> with a distance <Dist>.
  //! If Dist is greater than zero the result is enclosing
  //! the circle <Circ>, else the result is enclosed by the
  //! circle <Circ>.
  Standard_EXPORT GC_MakeCircle(const gp_Circ& Circ, const Standard_Real Dist);
  
  //! Make a Circle from Geom <TheCirc> parallel to another
  //! Circ <Circ> and passing through a Pnt <Point>.
  Standard_EXPORT GC_MakeCircle(const gp_Circ& Circ, const gp_Pnt& Point);
  
  //! Make a Circ from gp <TheCirc> passing through 3
  //! Pnt2d <P1>,<P2>,<P3>.
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);
  
  //! Make a Circle from Geom <TheCirc> with its center
  //! <Center> and the normal of its plane <Norm> and
  //! its radius <Radius>.
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& Center, const gp_Dir& Norm, const Standard_Real Radius);
  
  //! Make a Circle from Geom <TheCirc> with its center
  //! <Center> and the normal of its plane defined by the
  //! two points <Center> and <PtAxis> and its radius <Radius>.
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& Center, const gp_Pnt& PtAxis, const Standard_Real Radius);
  
  //! Make a Circle from Geom <TheCirc> with its center
  //! <Center> and its radius <Radius>.
  Standard_EXPORT GC_MakeCircle(const gp_Ax1& Axis, const Standard_Real Radius);
  

  //! Returns the constructed circle.
  //! Exceptions
  //! StdFail_NotDone if no circle is constructed.
  Standard_EXPORT const Handle(Geom_Circle)& Value() const;

  operator const Handle(Geom_Circle)& () const { return Value(); }

private:
  Handle(Geom_Circle) TheCircle;
};

#endif // _GC_MakeCircle_HeaderFile
