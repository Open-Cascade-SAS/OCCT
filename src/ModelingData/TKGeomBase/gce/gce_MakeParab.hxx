// Created on: 1992-08-26
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

#ifndef _gce_MakeParab_HeaderFile
#define _gce_MakeParab_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Parab.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;

//! Implements construction algorithms for `gp_Parab`.
//! The parabola is infinite in the parameter range ]-infinite, +infinite[.
//! The vertex is the `Location` point of the local coordinate system.
//!
//! The `XDirection` and `YDirection` define the parabola plane.
//!
//! The `XAxis` (`Location`, `XDirection`) is the symmetry axis and is oriented
//! from the vertex to the focus.
//!
//! The `YAxis` (`Location`, `YDirection`) is parallel to the directrix.
//!
//! The equation in the local coordinate system is:
//! `Y**2 = (2*P) * X`, where `P` is the parameter
//! (distance between focus and directrix).
//! The focal length `F = P / 2` is the distance from vertex to focus.
//!
//! Supported constructions:
//! - from local coordinate system and focal length;
//! - from directrix and focus.
class gce_MakeParab : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a parabola from local coordinate system and focal length.
  //! @param[in] A2 local coordinate system of the parabola
  //! @param[in] Focal focal length
  //! @note `TheError` is set to `gce_NullFocusLength` if `Focal < 0.0`.
  Standard_EXPORT gce_MakeParab(const gp_Ax2& A2, const double Focal);

  //! Creates a parabola from directrix and focus.
  //! @param[in] D directrix of the parabola
  //! @param[in] F focus point of the parabola
  Standard_EXPORT gce_MakeParab(const gp_Ax1& D, const gp_Pnt& F);

  //! Returns the constructed parabola.
  //! @return resulting parabola
  //! @throw StdFail_NotDone if construction has failed
  Standard_EXPORT const gp_Parab& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting parabola
  gp_Parab Operator() const { return Value(); }

  //! Conversion operator returning the constructed parabola.
  //! @return resulting parabola
  operator gp_Parab() const { return Operator(); }

private:
  gp_Parab TheParab;
};

#endif // _gce_MakeParab_HeaderFile
