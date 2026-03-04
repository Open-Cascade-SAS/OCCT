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

#ifndef _gce_MakeCylinder_HeaderFile
#define _gce_MakeCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Cylinder.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Ax1;
class gp_Circ;

//! This class implements construction algorithms for `gp_Cylinder`.
//! Supported constructions include:
//! - cylinder from axis placement and radius;
//! - cylinder coaxial to another, through point or at signed offset;
//! - cylinder from three points;
//! - cylinder from axis and radius;
//! - cylinder from circular base.
class gce_MakeCylinder : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a cylinder from axis placement and radius.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] A2 local coordinate system
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCylinder(const gp_Ax2& A2, const double Radius);

  //! Creates a cylinder coaxial to input cylinder and passing through a point.
  //! @param[in] Cyl source cylinder
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeCylinder(const gp_Cylinder& Cyl, const gp_Pnt& Point);

  //! Creates a cylinder coaxial to input cylinder at signed distance.
  //! @note Construction fails with `gce_NegativeRadius` if resulting radius is negative.
  //! @param[in] Cyl source cylinder
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeCylinder(const gp_Cylinder& Cyl, const double Dist);

  //! Creates a cylinder from three points.
  //! @note Axis is defined by points `P1` and `P2`.
  //! @note Radius is the distance from `P3` to that axis.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] P3 third point
  Standard_EXPORT gce_MakeCylinder(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);

  //! Makes a Cylinder by its axis <Axis> and radius <Radius>.
  //! @param[in] Axis axis definition
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCylinder(const gp_Ax1& Axis, const double Radius);

  //! Creates a cylinder from circular base.
  //! @note The resulting cylinder axis equals the circle axis.
  //! @note This constructor succeeds for any valid `Circ`.
  //! @param[in] Circ source circle
  Standard_EXPORT gce_MakeCylinder(const gp_Circ& Circ);

  //! Returns the constructed cylinder.
  //! Exceptions StdFail_NotDone if no cylinder is constructed.
  //! @return resulting cylinder
  Standard_EXPORT const gp_Cylinder& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Cylinder Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Cylinder() const { return Operator(); }

private:
  gp_Cylinder TheCylinder;
};

#endif // _gce_MakeCylinder_HeaderFile
