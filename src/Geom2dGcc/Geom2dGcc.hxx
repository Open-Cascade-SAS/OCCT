// Created on: 1992-06-29
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

#ifndef _Geom2dGcc_HeaderFile
#define _Geom2dGcc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom2dGcc_QualifiedCurve;
class Geom2dAdaptor_Curve;

//! The Geom2dGcc package describes qualified 2D
//! curves used in the construction of constrained geometric
//! objects by an algorithm provided by the Geom2dGcc package.
//! A qualified 2D curve is a curve with a qualifier which
//! specifies whether the solution of a construction
//! algorithm using the qualified curve (as an argument):
//! -   encloses the curve, or
//! -   is enclosed by the curve, or
//! -   is built so that both the curve and this solution are external to one another, or
//! -   is undefined (all solutions apply).
//! These package methods provide simpler functions to construct a qualified curve.
//! Note: the interior of a curve is defined as the left-hand
//! side of the curve in relation to its orientation.
class Geom2dGcc
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs such a qualified curve that the relative
  //! position of the solution computed by a construction
  //! algorithm using the qualified curve to the circle or line is
  //! not qualified, i.e. all solutions apply.
  //! Warning
  //! Obj is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve Obj ( mycurve )
  //! ;
  //! The qualified curve is then constructed with this object:
  //! Geom2dGcc_QualifiedCurve
  //! myQCurve = Geom2dGcc::Unqualified(Obj);
  Standard_EXPORT static Geom2dGcc_QualifiedCurve Unqualified(const Geom2dAdaptor_Curve& Obj);

  //! Constructs such a qualified curve that the solution
  //! computed by a construction algorithm using the qualified
  //! curve encloses the curve.
  //! Warning
  //! Obj is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve Obj ( mycurve )
  //! ;
  //! The qualified curve is then constructed with this object:
  //! Geom2dGcc_QualifiedCurve
  //! myQCurve = Geom2dGcc::Enclosing(Obj);
  Standard_EXPORT static Geom2dGcc_QualifiedCurve Enclosing(const Geom2dAdaptor_Curve& Obj);

  //! Constructs such a qualified curve that the solution
  //! computed by a construction algorithm using the qualified
  //! curve is enclosed by the curve.
  //! Warning
  //! Obj is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve Obj ( mycurve )
  //! ;
  //! The qualified curve is then constructed with this object:
  //! Geom2dGcc_QualifiedCurve
  //! myQCurve = Geom2dGcc::Enclosed(Obj);
  Standard_EXPORT static Geom2dGcc_QualifiedCurve Enclosed(const Geom2dAdaptor_Curve& Obj);

  //! Constructs such a qualified curve that the solution
  //! computed by a construction algorithm using the qualified
  //! curve and the curve are external to one another.
  //! Warning
  //! Obj is an adapted curve, i.e. an object which is an interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by a computation algorithm.
  //! The adapted curve is created in the following way:
  //! Handle(Geom2d_Curve) mycurve = ...
  //! ;
  //! Geom2dAdaptor_Curve Obj ( mycurve )
  //! ;
  //! The qualified curve is then constructed with this object:
  //! Geom2dGcc_QualifiedCurve
  //! myQCurve = Geom2dGcc::Outside(Obj);
  Standard_EXPORT static Geom2dGcc_QualifiedCurve Outside(const Geom2dAdaptor_Curve& Obj);
};

#endif // _Geom2dGcc_HeaderFile
