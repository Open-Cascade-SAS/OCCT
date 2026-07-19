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

#ifndef _gce_MakeElips2d_HeaderFile
#define _gce_MakeElips2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Elips2d.hxx>
#include <gce_Root.hxx>
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;

//! This class implements construction algorithms for `gp_Elips2d`.
//! Supported constructions include:
//! - ellipse from major axis (or local 2D coordinate system) and radii;
//! - ellipse from center and two points.
class gce_MakeElips2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an ellipse with the major axis, the major and the
  //! minor radius. The location of the MajorAxis is the center
  //! of the ellipse.
  //! The sense of parametrization is given by Sense.
  //! It is possible to create an ellipse with MajorRadius = MinorRadius.
  //! @note Construction fails with `gce_InvertRadius` if
  //!       `MajorRadius < MinorRadius`.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MajorRadius < 0.0`.
  //! @param[in] MajorAxis major axis placement
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  //! @param[in] Sense orientation flag
  Standard_EXPORT gce_MakeElips2d(const gp_Ax2d& MajorAxis,
                                  const double   MajorRadius,
                                  const double   MinorRadius,
                                  const bool     Sense = true);

  //! Axis defines the Xaxis and Yaxis of the ellipse which defines
  //! the origin and the sense of parametrization.
  //! Creates an ellipse with the AxisPlacement the major and the
  //! minor radius. The location of Axis is the center
  //! of the ellipse.
  //! It is possible to create an ellipse with MajorRadius = MinorRadius.
  //! @note Construction fails with `gce_InvertRadius` if
  //!       `MajorRadius < MinorRadius`.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MajorRadius < 0.0`.
  //! @param[in] A local coordinate system
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  Standard_EXPORT gce_MakeElips2d(const gp_Ax22d& A,
                                  const double    MajorRadius,
                                  const double    MinorRadius);

  //! Creates an ellipse from center and two points.
  //! @note `S1` defines major radius direction and value.
  //! @note Minor radius is computed as distance from `S2` to major axis.
  //! @note Construction fails with `gce_NullAxis` when computed minor radius
  //!       is null.
  //! @note Construction fails with `gce_InvertAxis` when computed minor radius
  //!       exceeds major radius.
  //! @param[in] S1 first point
  //! @param[in] S2 second point
  //! @param[in] Center center point
  Standard_EXPORT gce_MakeElips2d(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center);

  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  //! @return resulting ellipse
  Standard_EXPORT const gp_Elips2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Elips2d Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Elips2d() const { return Operator(); }

private:
  gp_Elips2d TheElips2d;
};

#endif // _gce_MakeElips2d_HeaderFile
