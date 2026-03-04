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

#ifndef _GC_MakeEllipse_HeaderFile
#define _GC_MakeEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Ellipse.hxx>

class gp_Elips;
class gp_Ax2;
class gp_Pnt;

//! Implements construction algorithms for ellipses in 3D space.
//! The result is a `Geom_Ellipse`.
//! A MakeEllipse object provides a framework for:
//! -   defining the construction of the ellipse,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed ellipse.
class GC_MakeEllipse : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an ellipse from a `gp_Elips`.
  //! @param[in] theE source ellipse
  Standard_EXPORT GC_MakeEllipse(const gp_Elips& theE);

  //! Constructs an ellipse with major and minor radii MajorRadius and
  //! MinorRadius, and located in the plane defined by
  //! the "X Axis" and "Y Axis" of the coordinate system A2, where:
  //! -   its center is the origin of A2, and
  //! -   its major axis is the "X Axis" of A2;
  //! @note Construction with `theMajorRadius == theMinorRadius` is allowed.
  //! @note Construction fails with `gce_NegativeRadius` if `theMinorRadius < 0.0`.
  //! @note Construction fails with `gce_InvertAxis` if
  //!       `theMajorRadius < theMinorRadius`.
  //! @param[in] theA2 ellipse local coordinate system
  //! @param[in] theMajorRadius major radius
  //! @param[in] theMinorRadius minor radius
  Standard_EXPORT GC_MakeEllipse(const gp_Ax2& theA2,
                                 const double  theMajorRadius,
                                 const double  theMinorRadius);

  //! Constructs an ellipse centered on the point Center, where
  //! -   the plane of the ellipse is defined by Center, S1 and S2,
  //! -   its major axis is defined by Center and S1,
  //! -   its major radius is the distance between Center and S1, and
  //! -   its minor radius is the distance between S2 and the major axis.
  //! @param[in] theS1 point defining the major axis
  //! @param[in] theS2 point defining the minor radius
  //! @param[in] theCenter ellipse center
  Standard_EXPORT GC_MakeEllipse(const gp_Pnt& theS1, const gp_Pnt& theS2, const gp_Pnt& theCenter);

  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  //! @return resulting ellipse
  Standard_EXPORT const occ::handle<Geom_Ellipse>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_Ellipse>&() const { return Value(); }

private:
  occ::handle<Geom_Ellipse> TheEllipse;
};

#endif // _GC_MakeEllipse_HeaderFile
