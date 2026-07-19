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

#ifndef _gce_MakeCirc_HeaderFile
#define _gce_MakeCirc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Circ.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Dir;
class gp_Pln;
class gp_Ax1;

//! This class implements construction algorithms for `gp_Circ`.
//! Supported constructions include:
//! - circle from axis and radius;
//! - circle coaxial to another one, through point or at signed offset;
//! - circle through three points;
//! - circle from center and normal/plane;
//! - circle from center and axis-defining point;
//! - circle from axis and radius.
class gce_MakeCirc : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a circle from axis placement and radius.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] A2 local coordinate system
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc(const gp_Ax2& A2, const double Radius);

  //! Creates a circle coaxial to input circle at signed distance.
  //! @note If `Dist` is positive, the result encloses `Circ`.
  //! @note If `Dist` is negative, the result is enclosed by `Circ`.
  //! @param[in] Circ source circle
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeCirc(const gp_Circ& Circ, const double Dist);

  //! Creates a circle coaxial to input circle and passing through a point.
  //! @param[in] Circ source circle
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeCirc(const gp_Circ& Circ, const gp_Pnt& Point);

  //! Creates a circle passing through three points.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] P3 third point
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);

  //! Creates a circle from center, plane normal and radius.
  //! @param[in] Center center point
  //! @param[in] Norm input value
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Dir& Norm, const double Radius);

  //! Creates a circle from center, reference plane and radius.
  //! @param[in] Center center point
  //! @param[in] Plane reference plane
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Pln& Plane, const double Radius);

  //! Creates a circle from center, axis-defining point and radius.
  //! @param[in] Center center point
  //! @param[in] Ptaxis point defining axis direction
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Pnt& Ptaxis, const double Radius);

  //! Creates a circle from axis and radius.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @param[in] Axis axis definition
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCirc(const gp_Ax1& Axis, const double Radius);

  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  //! @return resulting circle
  Standard_EXPORT const gp_Circ& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Circ Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Circ() const { return Operator(); }

private:
  gp_Circ TheCirc;
};

#endif // _gce_MakeCirc_HeaderFile
