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

#ifndef _gce_MakeCirc2d_HeaderFile
#define _gce_MakeCirc2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Circ2d.hxx>
#include <gce_Root.hxx>
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;

//! This class implements construction algorithms for `gp_Circ2d`.
//! Supported constructions include:
//! - circle from axis and radius;
//! - circle concentric to another one, through point or at signed offset;
//! - circle through three points;
//! - circle from center and radius;
//! - circle from center and one point.
class gce_MakeCirc2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a circle from axis and radius.
  //! @note The location of `XAxis` is the circle center.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] XAxis axis placement
  //! @param[in] Radius radius value
  //! @param[in] Sense orientation flag
  Standard_EXPORT gce_MakeCirc2d(const gp_Ax2d& XAxis,
                                 const double   Radius,
                                 const bool     Sense = true);

  //! Creates a circle from local coordinate system and radius.
  //! @note The location of `Axis` is the circle center.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] Axis axis definition
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc2d(const gp_Ax22d& Axis, const double Radius);

  //! Creates a circle concentric to input circle with signed offset.
  //! @note Result radius is `Abs(Circ.Radius() + Dist)`.
  //! @param[in] Circ source circle
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeCirc2d(const gp_Circ2d& Circ, const double Dist);

  //! Creates a circle concentric to input circle and passing through a point.
  //! @param[in] Circ source circle
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeCirc2d(const gp_Circ2d& Circ, const gp_Pnt2d& Point);

  //! Creates a circle passing through three points.
  //! @note The local coordinate system of the result is derived
  //!       from input points.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] P3 third point
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const gp_Pnt2d& P3);

  //! Creates a circle from center and radius.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] Center center point
  //! @param[in] Radius radius value
  //! @param[in] Sense orientation flag
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& Center,
                                 const double    Radius,
                                 const bool      Sense = true);

  //! Creates a circle from center and one point on circle.
  //! @note `Sense` controls result orientation.
  //! @param[in] Center center point
  //! @param[in] Point reference point
  //! @param[in] Sense orientation flag
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& Center,
                                 const gp_Pnt2d& Point,
                                 const bool      Sense = true);

  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  //! @return resulting circle
  Standard_EXPORT const gp_Circ2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Circ2d Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Circ2d() const { return Operator(); }

private:
  gp_Circ2d TheCirc2d;
};

#endif // _gce_MakeCirc2d_HeaderFile
