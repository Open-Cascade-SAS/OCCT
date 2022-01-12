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

#ifndef _Convert_EllipseToBSplineCurve_HeaderFile
#define _Convert_EllipseToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_ParameterisationType.hxx>
class gp_Elips2d;



//! This algorithm converts a ellipse into a rational B-spline curve.
//! The ellipse is represented an Elips2d from package gp with
//! the parametrization :
//! P (U) =
//! Loc + (MajorRadius * Cos(U) * Xdir + MinorRadius * Sin(U) * Ydir)
//! where Loc is the center of the ellipse, Xdir and Ydir are the
//! normalized directions of the local cartesian coordinate system of
//! the ellipse. The parametrization range is U [0, 2PI].
//! KeyWords :
//! Convert, Ellipse, BSplineCurve, 2D .
class Convert_EllipseToBSplineCurve  : public Convert_ConicToBSplineCurve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! The equivalent B-spline curve has the same orientation
  //! as the ellipse E.
  Standard_EXPORT Convert_EllipseToBSplineCurve(const gp_Elips2d& E, const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);
  

  //! The ellipse E is limited between the parametric values U1, U2.
  //! The equivalent B-spline curve is oriented from U1 to U2 and has
  //! the same orientation as E.
  //!
  //! Raised if U1 = U2 or U1 = U2 + 2.0 * Pi
  Standard_EXPORT Convert_EllipseToBSplineCurve(const gp_Elips2d& E, const Standard_Real U1, const Standard_Real U2, const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);




protected:





private:





};







#endif // _Convert_EllipseToBSplineCurve_HeaderFile
