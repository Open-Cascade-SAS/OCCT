// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Convert_CircleToBSplineCurve_HeaderFile
#define _Convert_CircleToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_ParameterisationType.hxx>
class gp_Circ2d;

//! This algorithm converts a circle into a rational B-spline curve.
//! The circle is a Circ2d from package gp and its parametrization is :
//! P (U) = Loc + R * (Cos(U) * Xdir + Sin(U) * YDir) where Loc is the
//! center of the circle Xdir and Ydir are the normalized directions
//! of the local cartesian coordinate system of the circle.
//! The parametrization range for the circle is U [0, 2Pi].
//!
//! Warnings :
//! The parametrization range for the B-spline curve is not [0, 2Pi].
//!
//! KeyWords :
//! Convert, Circle, BSplineCurve, 2D .
class Convert_CircleToBSplineCurve : public Convert_ConicToBSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! The equivalent B-spline curve has the same orientation
  //! as the circle C.
  Standard_EXPORT Convert_CircleToBSplineCurve(
    const gp_Circ2d&                   C,
    const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);

  //! The circle C is limited between the parametric values U1, U2
  //! in radians. U1 and U2 [0.0, 2*Pi] .
  //! The equivalent B-spline curve is oriented from U1 to U2 and has
  //! the same orientation as the circle C.
  //!
  //! Raised if U1 = U2 or U1 = U2 + 2.0 * Pi
  Standard_EXPORT Convert_CircleToBSplineCurve(
    const gp_Circ2d&                   C,
    const Standard_Real                U1,
    const Standard_Real                U2,
    const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);

protected:
private:
};

#endif // _Convert_CircleToBSplineCurve_HeaderFile
