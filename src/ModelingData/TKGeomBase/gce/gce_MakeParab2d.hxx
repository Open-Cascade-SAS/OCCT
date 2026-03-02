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

#ifndef _gce_MakeParab2d_HeaderFile
#define _gce_MakeParab2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Parab2d.hxx>
#include <gce_Root.hxx>
#include <Standard_Boolean.hxx>
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;

//! Implements construction algorithms for `gp_Parab2d`.
//! The parabola is infinite and represented in a local 2D coordinate system.
//! The `XAxis` is the symmetry axis directed from vertex to focus,
//! and the `YAxis` is parallel to the directrix.
//! The equation in local coordinates is:
//! `Y**2 = (2*P) * X`, where `P` is the distance between focus and directrix.
//! The focal length `F = P / 2` is the distance from vertex to focus.
//!
//! Supported constructions:
//! - from symmetry axis and focal length;
//! - from full axis system and focal length;
//! - from directrix and focus;
//! - from focus and vertex.
class gce_MakeParab2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a parabola from symmetry axis and focal length.
  //! @param[in] MirrorAxis symmetry axis of the parabola
  //! @param[in] Focal focal length
  //! @param[in] Sense orientation of parametrization
  //! @note `Focal = 0` is accepted.
  //! @note `TheError` is set to `gce_NullFocusLength` if `Focal < 0.0`.
  Standard_EXPORT gce_MakeParab2d(const gp_Ax2d& MirrorAxis,
                                  const double   Focal,
                                  const bool     Sense = true);

  //! Creates a parabola from full local coordinate system and focal length.
  //! @param[in] A local coordinate system of the parabola
  //! @param[in] Focal focal length
  //! @note `Focal = 0` is accepted.
  //! @note `TheError` is set to `gce_NullFocusLength` if `Focal < 0.0`.
  Standard_EXPORT gce_MakeParab2d(const gp_Ax22d& A, const double Focal);

  //! Creates a parabola from directrix and focus.
  //! @param[in] D directrix of the parabola
  //! @param[in] F focus point of the parabola
  //! @param[in] Sense orientation of parametrization
  Standard_EXPORT gce_MakeParab2d(const gp_Ax2d& D, const gp_Pnt2d& F, const bool Sense = true);

  //! Creates a parabola from focus and vertex.
  //! @param[in] S1 focus point
  //! @param[in] Center vertex point
  //! @param[in] Sense orientation of parametrization
  //! @note The class does not prevent zero focal distance.
  //! @note `TheError` is set to `gce_NullAxis` if `S1` and `Center` are coincident.
  Standard_EXPORT gce_MakeParab2d(const gp_Pnt2d& S1,
                                  const gp_Pnt2d& Center,
                                  const bool      Sense = true);

  //! Returns the constructed parabola.
  //! @return resulting parabola
  //! @throw StdFail_NotDone if construction has failed
  Standard_EXPORT const gp_Parab2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting parabola
  gp_Parab2d Operator() const
  {
    return Value();
  }

  //! Conversion operator returning the constructed parabola.
  //! @return resulting parabola
  operator gp_Parab2d() const
  {
    return Operator();
  }

private:
  gp_Parab2d TheParab2d;
};

#endif // _gce_MakeParab2d_HeaderFile
