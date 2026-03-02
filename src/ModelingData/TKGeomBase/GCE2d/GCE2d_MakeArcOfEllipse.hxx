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

#ifndef _GCE2d_MakeArcOfEllipse_HeaderFile
#define _GCE2d_MakeArcOfEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class gp_Elips2d;
class gp_Pnt2d;

//! This class implements construction algorithms for arcs of ellipses in the plane.
//! The result is a `Geom2d_TrimmedCurve`.
//! A `GCE2d_MakeArcOfEllipse` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting arc via `Value()`.
//! @note Angular parameters are expressed in radians.
class GCE2d_MakeArcOfEllipse : public GCE2d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an arc from angular bounds on an ellipse.
  //! @param[in] theEllipse source ellipse
  //! @param[in] theAlpha1 first angle (radians)
  //! @param[in] theAlpha2 second angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& theEllipse,
                                         const double      theAlpha1,
                                         const double      theAlpha2,
                                         const bool        theSense = true);

  //! Constructs an arc from a point and angular bound on an ellipse.
  //! @param[in] theEllipse source ellipse
  //! @param[in] thePoint point on source ellipse
  //! @param[in] theAlpha angle value (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& theEllipse,
                                         const gp_Pnt2d&   thePoint,
                                         const double      theAlpha,
                                         const bool        theSense = true);

  //! Constructs an arc between two points on an ellipse.
  //! @param[in] theEllipse source ellipse
  //! @param[in] theP1 first point on source ellipse
  //! @param[in] theP2 second point on source ellipse
  //! @param[in] theSense orientation of resulting arc
  //! @note Orientation is trigonometric when `theSense` is true,
  //!       otherwise opposite.
  //! @note IsDone always returns true.
  Standard_EXPORT GCE2d_MakeArcOfEllipse(const gp_Elips2d& theEllipse,
                                         const gp_Pnt2d&   theP1,
                                         const gp_Pnt2d&   theP2,
                                         const bool        theSense = true);

  //! Returns the constructed arc of ellipse.
  //! @return resulting trimmed curve
  Standard_EXPORT const occ::handle<Geom2d_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting trimmed curve
  operator const occ::handle<Geom2d_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom2d_TrimmedCurve> TheArc;
};

#endif // _GCE2d_MakeArcOfEllipse_HeaderFile
