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

#ifndef _Convert_ParabolaToBSplineCurve_HeaderFile
#define _Convert_ParabolaToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ConicToBSplineCurve.hxx>
#include <Standard_Real.hxx>
class gp_Parab2d;



//! This algorithm converts a parabola into a non rational B-spline
//! curve.
//! The parabola is a Parab2d from package gp with the parametrization
//! P (U) = Loc + F * (U*U * Xdir + 2 * U * Ydir) where Loc is the
//! apex of the parabola, Xdir is the normalized direction of the
//! symmetry axis of the parabola, Ydir is the normalized direction of
//! the directrix and F is the focal length.
//! KeyWords :
//! Convert, Parabola, BSplineCurve, 2D .
class Convert_ParabolaToBSplineCurve  : public Convert_ConicToBSplineCurve
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The parabola Prb is limited between the parametric values U1, U2
  //! and the equivalent B-spline curve as the same orientation as the
  //! parabola Prb.
  Standard_EXPORT Convert_ParabolaToBSplineCurve(const gp_Parab2d& Prb, const Standard_Real U1, const Standard_Real U2);




protected:





private:





};







#endif // _Convert_ParabolaToBSplineCurve_HeaderFile
