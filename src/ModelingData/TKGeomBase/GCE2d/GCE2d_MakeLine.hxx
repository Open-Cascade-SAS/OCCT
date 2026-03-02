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

#ifndef _GCE2d_MakeLine_HeaderFile
#define _GCE2d_MakeLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Line.hxx>

class gp_Ax2d;
class gp_Lin2d;
class gp_Pnt2d;
class gp_Dir2d;

//! This class implements construction algorithms for lines in the plane.
//! The result is a `Geom2d_Line`.
//! A `GCE2d_MakeLine` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting line via `Value()`.
//! Supported constructions include:
//! - line from axis placement;
//! - line from existing `gp_Lin2d`;
//! - line from point and direction;
//! - line parallel to input line through a point;
//! - line parallel to input line at signed distance;
//! - line through two points.
class GCE2d_MakeLine : public GCE2d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a line from an axis placement.
  //! @param[in] theAxis axis placement
  //! @note The location of `theAxis` is the line origin.
  Standard_EXPORT GCE2d_MakeLine(const gp_Ax2d& theAxis);

  //! Creates a line from a non-persistent line from package gp.
  //! @param[in] theLine source line
  Standard_EXPORT GCE2d_MakeLine(const gp_Lin2d& theLine);

  //! Constructs a line from origin and direction.
  //! @param[in] thePoint point on line
  //! @param[in] theDir direction
  Standard_EXPORT GCE2d_MakeLine(const gp_Pnt2d& thePoint, const gp_Dir2d& theDir);

  //! Constructs a line parallel to input line and passing through a point.
  //! @param[in] theLine source line
  //! @param[in] thePoint point on resulting line
  Standard_EXPORT GCE2d_MakeLine(const gp_Lin2d& theLine, const gp_Pnt2d& thePoint);

  //! Constructs a line parallel to input line at signed distance.
  //! @param[in] theLine source line
  //! @param[in] theDist signed distance
  Standard_EXPORT GCE2d_MakeLine(const gp_Lin2d& theLine, const double theDist);

  //! Constructs a line passing through two points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @note Status is `gce_ConfusedPoints` if points are coincident.
  Standard_EXPORT GCE2d_MakeLine(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2);

  //! Returns the constructed line.
  //! Exceptions StdFail_NotDone if no line is constructed.
  //! @return resulting line
  Standard_EXPORT const occ::handle<Geom2d_Line>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting line
  operator const occ::handle<Geom2d_Line>&() const { return Value(); }

private:
  occ::handle<Geom2d_Line> TheLine;
};

#endif // _GCE2d_MakeLine_HeaderFile
