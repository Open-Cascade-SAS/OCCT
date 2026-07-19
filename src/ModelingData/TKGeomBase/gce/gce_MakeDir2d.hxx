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

#ifndef _gce_MakeDir2d_HeaderFile
#define _gce_MakeDir2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir2d.hxx>
#include <gce_Root.hxx>
class gp_Vec2d;
class gp_XY;
class gp_Pnt2d;

//! This class implements construction algorithms for `gp_Dir2d`.
//! Supported constructions include:
//! - direction from vector or coordinate components;
//! - direction from two points.
class gce_MakeDir2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Normalizes the vector V and creates a direction.
  //! @note Construction fails with `gce_NullVector` if
  //!       `V.Magnitude() <= gp::Resolution()`.
  //! @param[in] V direction vector
  Standard_EXPORT gce_MakeDir2d(const gp_Vec2d& V);

  //! Creates a direction from a coordinate vector.
  //! @note Construction fails with `gce_NullVector` if
  //!       `Coord.Modulus() <= gp::Resolution()`.
  //! @param[in] Coord coordinate vector
  Standard_EXPORT gce_MakeDir2d(const gp_XY& Coord);

  //! Creates a direction with its two cartesian coordinates.
  //! @note Construction fails with `gce_NullVector` if
  //!       `Xv*Xv + Yv*Yv <= gp::Resolution()`.
  //! @param[in] Xv X coordinate value
  //! @param[in] Yv Y coordinate value
  Standard_EXPORT gce_MakeDir2d(const double Xv, const double Yv);

  //! Creates a direction from two points.
  //! @note Construction fails with `gce_ConfusedPoints` if points are coincident.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeDir2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2);

  //! Returns the constructed unit vector.
  //! Exceptions StdFail_NotDone if no unit vector is constructed.
  //! @return resulting direction
  Standard_EXPORT const gp_Dir2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Dir2d Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Dir2d() const { return Operator(); }

private:
  gp_Dir2d TheDir2d;
};

#endif // _gce_MakeDir2d_HeaderFile
