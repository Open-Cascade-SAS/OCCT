// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#ifndef _GeomLProp_HeaderFile
#define _GeomLProp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
class Geom_Curve;

//! These global functions compute the degree of
//! continuity of a 3D curve built by concatenation of two
//! other curves (or portions of curves) at their junction point.
class GeomLProp
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the regularity at the junction between C1 and
  //! C2. The booleans r1 and r2 are true if the curves must
  //! be taken reversed. The point u1 on C1 and the point
  //! u2 on C2 must be confused.
  //! tl and ta are the linear and angular tolerance used two
  //! compare the derivative.
  Standard_EXPORT static GeomAbs_Shape Continuity(const occ::handle<Geom_Curve>& C1,
                                                  const occ::handle<Geom_Curve>& C2,
                                                  const double                   u1,
                                                  const double                   u2,
                                                  const bool                     r1,
                                                  const bool                     r2,
                                                  const double                   tl,
                                                  const double                   ta);

  //! The same as preceding but using the standard
  //! tolerances from package Precision.
  Standard_EXPORT static GeomAbs_Shape Continuity(const occ::handle<Geom_Curve>& C1,
                                                  const occ::handle<Geom_Curve>& C2,
                                                  const double                   u1,
                                                  const double                   u2,
                                                  const bool                     r1,
                                                  const bool                     r2);
};

#endif // _GeomLProp_HeaderFile
