// Created on: 1991-04-15
// Created by: Philippe DAUTRY
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

#ifndef _Geom2dGcc_QualifiedCurve_HeaderFile
#define _Geom2dGcc_QualifiedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GccEnt_Position.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Boolean.hxx>

//! Describes functions for building a qualified 2D curve.
//! A qualified 2D curve is a curve with a qualifier which
//! specifies whether the solution of a construction
//! algorithm using the qualified curve (as an argument):
//! -   encloses the curve, or
//! -   is enclosed by the curve, or
//! -   is built so that both the curve and it are external to one another, or
//! -   is undefined (all solutions apply).
class Geom2dGcc_QualifiedCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a qualified curve by assigning the qualifier
  //! Qualifier to the curve Curve. Qualifier may be:
  //! -   GccEnt_enclosing if the solution of a construction
  //! algorithm using the qualified curve encloses the curve, or
  //! -   GccEnt_enclosed if the solution is enclosed by the curve, or
  //! -   GccEnt_outside if both the solution and the curve
  //! are external to one another, or
  //! -   GccEnt_unqualified if all solutions apply.
  //! Note: The interior of a curve is defined as the left-hand
  //! side of the curve in relation to its orientation.
  //! Warning
  //! Curve is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ... ;
  //! Geom2dAdaptor_Curve Curve ( mycurve ) ;
  //! The qualified curve is then constructed with this object:
  //! GccEnt_Position myQualif = GccEnt_outside ;
  //! Geom2dGcc_QualifiedCurve myQCurve ( Curve, myQualif );
  //! is private;
  Standard_EXPORT Geom2dGcc_QualifiedCurve(const Geom2dAdaptor_Curve& Curve,
                                           const GccEnt_Position      Qualifier);

  //! Returns a 2D curve to which the qualifier is assigned.
  //! Warning
  //! The returned curve is an adapted curve, i.e. an object
  //! which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The Geom2d curve on which the adapted curve is
  //! based can be obtained in the following way:
  //! myQualifiedCurve = ... ;
  //! Geom2dAdaptor_Curve myAdaptedCurve = myQualifiedCurve.Qualified();
  //! Handle(Geom2d_Curve) = myAdaptedCurve.Curve();
  Standard_EXPORT Geom2dAdaptor_Curve Qualified() const;

  //! Returns
  //! - the qualifier of this qualified curve if it is enclosing,
  //! enclosed or outside, or
  //! -   GccEnt_noqualifier if it is unqualified.
  Standard_EXPORT GccEnt_Position Qualifier() const;

  //! Returns true if the solution is unqualified and false in the other cases.
  Standard_EXPORT Standard_Boolean IsUnqualified() const;

  //! It returns true if the solution is Enclosing the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsEnclosing() const;

  //! It returns true if the solution is Enclosed in the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsEnclosed() const;

  //! It returns true if the solution is Outside the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsOutside() const;

protected:
private:
  GccEnt_Position     TheQualifier;
  Geom2dAdaptor_Curve TheQualified;
};

#endif // _Geom2dGcc_QualifiedCurve_HeaderFile
