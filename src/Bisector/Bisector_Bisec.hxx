// Created on: 1992-10-19
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

#ifndef _Bisector_Bisec_HeaderFile
#define _Bisector_Bisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_JoinType.hxx>
class Geom2d_TrimmedCurve;
class Geom2d_Curve;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_Point;

//! Bisec provides the bisecting line between two elements
//! This line is trimmed by a point <P> and it's contained in the domain
//! defined by the two vectors <V1>, <V2> and <Sense>.
//!
//! Definition of the domain:
//! if <Sense>  is  true the bisecting line is contained in the sector
//! defined by <-V1> and <-V2> in the sense indirect.
//! if <Sense>  is  false the bisecting line is contained in the sector
//! defined by <-V1> and <-V2> in the sense direct.
//!
//! <Tolerance> is used to define degenerate bisector.
//! if the bisector is an hyperbola and one of this radius is smaller
//! than <Tolerance>, the bisector is replaced by a line or semi_line
//! corresponding to one of hyperbola's axes.
//! if the bisector is a parabola on the focal length is smaller than
//! <Tolerance>, the bisector is replaced by a semi_line corresponding
//! to the axe of symmetry of the parabola.
//! if the bisector is an ellipse  and the minor radius is smaller than
//! <Tolerance>, the bisector is replaced by a segment corresponding
//! to the great axe of the ellipse.
class Bisector_Bisec
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Bisector_Bisec();

  //! Performs  the bisecting line  between the  curves
  //! <Cu1> and <Cu2>.
  //! <oncurve> is True if the point <P> is common to <Cu1>
  //! and <Cu2>.
  Standard_EXPORT void Perform(const Handle(Geom2d_Curve)& Cu1,
                               const Handle(Geom2d_Curve)& Cu2,
                               const gp_Pnt2d&             P,
                               const gp_Vec2d&             V1,
                               const gp_Vec2d&             V2,
                               const Standard_Real         Sense,
                               const GeomAbs_JoinType      ajointype,
                               const Standard_Real         Tolerance,
                               const Standard_Boolean      oncurve = Standard_True);

  //! Performs  the bisecting line  between the  curve
  //! <Cu1> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform(const Handle(Geom2d_Curve)& Cu,
                               const Handle(Geom2d_Point)& Pnt,
                               const gp_Pnt2d&             P,
                               const gp_Vec2d&             V1,
                               const gp_Vec2d&             V2,
                               const Standard_Real         Sense,
                               const Standard_Real         Tolerance,
                               const Standard_Boolean      oncurve = Standard_True);

  //! Performs  the bisecting line  between the  curve
  //! <Cu> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform(const Handle(Geom2d_Point)& Pnt,
                               const Handle(Geom2d_Curve)& Cu,
                               const gp_Pnt2d&             P,
                               const gp_Vec2d&             V1,
                               const gp_Vec2d&             V2,
                               const Standard_Real         Sense,
                               const Standard_Real         Tolerance,
                               const Standard_Boolean      oncurve = Standard_True);

  //! Performs  the bisecting line  between the two points
  //! <Pnt1>  and <Pnt2>.
  Standard_EXPORT void Perform(const Handle(Geom2d_Point)& Pnt1,
                               const Handle(Geom2d_Point)& Pnt2,
                               const gp_Pnt2d&             P,
                               const gp_Vec2d&             V1,
                               const gp_Vec2d&             V2,
                               const Standard_Real         Sense,
                               const Standard_Real         Tolerance = 0.0,
                               const Standard_Boolean      oncurve   = Standard_True);

  //! Returns the Curve of <me>.
  Standard_EXPORT const Handle(Geom2d_TrimmedCurve)& Value() const;

  //! Returns the Curve of <me>.
  Standard_EXPORT const Handle(Geom2d_TrimmedCurve)& ChangeValue();

protected:
private:
  Handle(Geom2d_TrimmedCurve) thebisector;
};

#endif // _Bisector_Bisec_HeaderFile
