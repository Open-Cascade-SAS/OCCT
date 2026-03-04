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

#ifndef _gce_MakeElips_HeaderFile
#define _gce_MakeElips_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Elips.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;

//! This class implements construction algorithms for `gp_Elips`.
//! Supported constructions include:
//! - ellipse from local coordinate system and radii;
//! - ellipse from center and two points.
class gce_MakeElips : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! The major radius of the ellipse is on the "XAxis" and the
  //! minor radius is on the "YAxis" of the ellipse. The "XAxis"
  //! is defined with the "XDirection" of A2 and the "YAxis" is
  //! defined with the "YDirection" of A2.
  //! @note It is possible to create an ellipse with
  //!       `MajorRadius == MinorRadius`.
  //! @note Construction fails with `gce_InvertRadius` if
  //!       `MajorRadius < MinorRadius`.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MinorRadius < 0.0`.
  //! @param[in] A2 local coordinate system
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  Standard_EXPORT gce_MakeElips(const gp_Ax2& A2,
                                const double  MajorRadius,
                                const double  MinorRadius);

  //! Creates an ellipse from center and two points.
  //! @note `S1` defines major radius direction and value.
  //! @note Minor radius is computed as distance from `S2` to major axis.
  //! @note Construction fails with `gce_NullAxis` if `S1` and `Center` coincide.
  //! @note Construction fails with `gce_InvertAxis` when computed minor radius
  //!       is null/greater than major radius, or when points are collinear.
  //! @param[in] S1 first point
  //! @param[in] S2 second point
  //! @param[in] Center center point
  Standard_EXPORT gce_MakeElips(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center);

  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  //! @return resulting ellipse
  Standard_EXPORT const gp_Elips& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Elips Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Elips() const { return Operator(); }

private:
  gp_Elips TheElips;
};

#endif // _gce_MakeElips_HeaderFile
