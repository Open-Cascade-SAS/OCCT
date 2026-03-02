// Created on: 1992-09-28
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

#ifndef _GC_MakeEllipse2d_HeaderFile
#define _GC_MakeEllipse2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom2d_Ellipse.hxx>

class gp_Elips2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;

//! This class implements construction algorithms for ellipses in the plane.
//! The result is a `Geom2d_Ellipse`.
//! A `GC_MakeEllipse2d` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting ellipse via `Value()`.
//! @note Ellipse parameterization range is [0, 2*PI].
//! @note The X axis of the local coordinate system is the major axis,
//!       and the Y axis is the minor axis.
class GC_MakeEllipse2d : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an ellipse from a non-persistent one from package gp.
  //! @param[in] theEllipse source ellipse
  Standard_EXPORT GC_MakeEllipse2d(const gp_Elips2d& theEllipse);

  //! Creates an ellipse from major axis placement and radii.
  //! @param[in] theMajorAxis major axis placement
  //! @param[in] theMajorRadius major radius value
  //! @param[in] theMinorRadius minor radius value
  //! @param[in] theSense orientation flag
  //! @note Error status is provided by the underlying `gce_MakeElips2d`
  //!       (for example `gce_InvertRadius` or `gce_NegativeRadius`).
  Standard_EXPORT GC_MakeEllipse2d(const gp_Ax2d& theMajorAxis,
                                    const double   theMajorRadius,
                                    const double   theMinorRadius,
                                    const bool     theSense = true);

  //! Creates an ellipse from a local coordinate system and radii.
  //! @param[in] theAxis local coordinate system
  //! @param[in] theMajorRadius major radius value
  //! @param[in] theMinorRadius minor radius value
  //! @note Error status is provided by the underlying `gce_MakeElips2d`
  //!       (for example `gce_InvertRadius` or `gce_NegativeRadius`).
  Standard_EXPORT GC_MakeEllipse2d(const gp_Ax22d& theAxis,
                                    const double    theMajorRadius,
                                    const double    theMinorRadius);

  //! Creates an ellipse from two apex points and center point.
  //! @param[in] theS1 first apex point
  //! @param[in] theS2 second point defining minor radius
  //! @param[in] theCenter center point
  //! @note Error status is provided by the underlying `gce_MakeElips2d`.
  Standard_EXPORT GC_MakeEllipse2d(const gp_Pnt2d& theS1,
                                    const gp_Pnt2d& theS2,
                                    const gp_Pnt2d& theCenter);

  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  //! @return resulting ellipse
  Standard_EXPORT const occ::handle<Geom2d_Ellipse>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting ellipse
  operator const occ::handle<Geom2d_Ellipse>&() const { return Value(); }

private:
  occ::handle<Geom2d_Ellipse> TheEllipse;
};

#endif // _GC_MakeEllipse2d_HeaderFile
