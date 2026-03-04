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

#ifndef _gce_MakeLin2d_HeaderFile
#define _gce_MakeLin2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Lin2d.hxx>
#include <gce_Root.hxx>
class gp_Ax2d;
class gp_Pnt2d;
class gp_Dir2d;

//! This class implements construction algorithms for `gp_Lin2d`.
//! Supported constructions include:
//! - line from axis placement;
//! - line from point and direction;
//! - line from cartesian equation;
//! - parallel line through point or at signed distance;
//! - line through two points.
class gce_MakeLin2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a line located with A.
  //! @note The location of `A` is the line origin.
  //! @param[in] A local coordinate system
  Standard_EXPORT gce_MakeLin2d(const gp_Ax2d& A);

  //! <P> is the location point (origin) of the line and
  //! <V> is the direction of the line.
  //! @param[in] P point
  //! @param[in] V direction vector
  Standard_EXPORT gce_MakeLin2d(const gp_Pnt2d& P, const gp_Dir2d& V);

  //! Creates the line from the equation A*X + B*Y + C = 0.0
  //! @note Construction fails with `gce_NullAxis` if
  //!       `A*A + B*B <= gp::Resolution()`.
  //! @param[in] A equation coefficient A
  //! @param[in] B equation coefficient B
  //! @param[in] C equation coefficient C
  Standard_EXPORT gce_MakeLin2d(const double A, const double B, const double C);

  //! Creates a line parallel to input line at signed distance.
  //! @note If `Dist` is positive, the result is on the right side
  //!       of `Lin` (in line local orientation), otherwise on the left.
  //! @param[in] Lin source line
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeLin2d(const gp_Lin2d& Lin, const double Dist);

  //! Creates a line parallel to input line and passing through a point.
  //! @param[in] Lin source line
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeLin2d(const gp_Lin2d& Lin, const gp_Pnt2d& Point);

  //! Creates a line passing through two points.
  //! @note Construction fails with `gce_ConfusedPoints` if `P1` and `P2`
  //!       are coincident.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeLin2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2);

  //! Returns the constructed line.
  //! Exceptions StdFail_NotDone if no line is constructed.
  //! @return resulting line
  Standard_EXPORT const gp_Lin2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Lin2d Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Lin2d() const { return Operator(); }

private:
  gp_Lin2d TheLin2d;
};

#endif // _gce_MakeLin2d_HeaderFile
