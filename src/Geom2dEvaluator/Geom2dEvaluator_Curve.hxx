// Created on: 2015-09-21
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Geom2dEvaluator_Curve_HeaderFile
#define _Geom2dEvaluator_Curve_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

class gp_Pnt2d;
class gp_Vec2d;

//! Interface for calculation of values and derivatives for different kinds of curves in 2D.
//! Works both with adaptors and curves.
class Geom2dEvaluator_Curve : public Standard_Transient
{
public:
  Geom2dEvaluator_Curve() {}

  //! Value of 2D curve
  virtual void D0(const Standard_Real theU,
                  gp_Pnt2d& theValue) const = 0;
  //! Value and first derivatives of curve
  virtual void D1(const Standard_Real theU,
                  gp_Pnt2d& theValue, gp_Vec2d& theD1) const = 0;
  //! Value, first and second derivatives of curve
  virtual void D2(const Standard_Real theU,
                  gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2) const = 0;
  //! Value, first, second and third derivatives of curve
  virtual void D3(const Standard_Real theU,
                  gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2, gp_Vec2d& theD3) const = 0;
  //! Calculates N-th derivatives of curve, where N = theDerU. Raises if N < 1
  virtual gp_Vec2d DN(const Standard_Real theU,
                      const Standard_Integer theDerU) const = 0;

  virtual Handle(Geom2dEvaluator_Curve) ShallowCopy() const = 0;

  DEFINE_STANDARD_RTTI_INLINE(Geom2dEvaluator_Curve,Standard_Transient)
};

DEFINE_STANDARD_HANDLE(Geom2dEvaluator_Curve, Standard_Transient)


#endif // _Geom2dEvaluator_Curve_HeaderFile
