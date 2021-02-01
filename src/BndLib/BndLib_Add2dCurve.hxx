// Created on: 1993-07-09
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BndLib_Add2dCurve_HeaderFile
#define _BndLib_Add2dCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Adaptor2d_Curve2d;
class Bnd_Box2d;
class Geom2d_Curve;


//! Computes the bounding box for a curve in 2d .
//! Functions to add a 2D curve to a bounding box.
//! The 2D curve is defined from a Geom2d curve.
class BndLib_Add2dCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds to the bounding box B the curve C
  //! B is then enlarged by the tolerance value Tol.
  //! Note: depending on the type of curve, one of the following
  //! representations of the curve C is used to include it in the bounding box B:
  //! -   an exact representation if C is built from a line, a circle   or a conic curve,
  //! -   the poles of the curve if C is built from a Bezier curve or   a BSpline curve,
  //! -   if not, the points of an approximation of the curve C.
  //! Warning
  //! C is an adapted curve, that is, an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d
  //! -   and those required of the curve by the computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve C(mycurve);
  //! The bounding box B is then enlarged by adding it:
  //! Bnd_Box2d B;
  //! // ...
  //! Standard_Real Tol = ... ;
  //! Add2dCurve::Add ( C, Tol, B );
  //! Exceptions
  //! Standard_Failure if the curve is built from:
  //! -   a Geom_Line, or
  //! -   a Geom_Parabola, or
  //! -   a Geom_Hyperbola,
  //! and P1 and P2 are either two negative infinite real
  //! numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add (const Adaptor2d_Curve2d& C, const Standard_Real Tol, Bnd_Box2d& B);
  
  //! Adds to the bounding box Bthe arc of the curve C limited by the two parameter
  //! values P1 and P2.
  //! B is then enlarged by the tolerance value Tol.
  //! Note: depending on the type of curve, one of the following
  //! representations of the curve C is used to include it in the bounding box B:
  //! -   an exact representation if C is built from a line, a circle   or a conic curve,
  //! -   the poles of the curve if C is built from a Bezier curve or   a BSpline curve,
  //! -   if not, the points of an approximation of the curve C.
  //! Warning
  //! C is an adapted curve, that is, an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d
  //! -   and those required of the curve by the computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve C(mycurve);
  //! The bounding box B is then enlarged by adding it:
  //! Bnd_Box2d B;
  //! // ...
  //! Standard_Real Tol = ... ;
  //! Add2dCurve::Add ( C, Tol, B );
  //! Exceptions
  //! Standard_Failure if the curve is built from:
  //! -   a Geom_Line, or
  //! -   a Geom_Parabola, or
  //! -   a Geom_Hyperbola,
  //! and P1 and P2 are either two negative infinite real
  //! numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add (const Adaptor2d_Curve2d& C, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol, Bnd_Box2d& B);
  
  //! Adds to the bounding box B the curve C
  //! B is then enlarged by the tolerance value Tol.
  //! Note: depending on the type of curve, one of the following
  //! representations of the curve C is used to include it in the bounding box B:
  //! -   an exact representation if C is built from a line, a circle or a conic curve,
  //! -   the poles of the curve if C is built from a Bezier curve or a BSpline curve,
  //! -   if not, the points of an approximation of the curve C.
  Standard_EXPORT static void Add (const Handle(Geom2d_Curve)& C, const Standard_Real Tol, Bnd_Box2d& Box);
  
  //! Adds to the bounding box B the part of curve C
  //! B is then enlarged by the tolerance value Tol.
  //! U1, U2 - the parametric range to compute the bounding box;
  //! Note: depending on the type of curve, one of the following
  //! representations of the curve C is used to include it in the bounding box B:
  //! -   an exact representation if C is built from a line, a circle or a conic curve,
  //! -   the poles of the curve if C is built from a Bezier curve or a BSpline curve,
  //! -   if not, the points of an approximation of the curve C.
  Standard_EXPORT static void Add (const Handle(Geom2d_Curve)& C, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol, Bnd_Box2d& B);

  //! Adds to the bounding box B the part of curve C
  //! B is then enlarged by the tolerance value Tol.
  //! U1, U2 - the parametric range to compute the bounding box;
  //! Note: depending on the type of curve, one of the following
  //! algorithms  is used to include it in the bounding box B:
  //! -   an exact analytical if C is built from a line, a circle or a conic curve,
  //! -   numerical calculation of bounding box sizes, based on minimization algorithm,  for other types of curve
  //! If Tol = < Precision::PConfusion(), Precision::PConfusion is used as tolerance for calculation
  Standard_EXPORT static void AddOptimal(const Handle(Geom2d_Curve)& C,
			                                   const Standard_Real U1,
			                                   const Standard_Real U2,
			                                   const Standard_Real Tol,
			                                   Bnd_Box2d& B);


protected:





private:





};







#endif // _BndLib_Add2dCurve_HeaderFile
