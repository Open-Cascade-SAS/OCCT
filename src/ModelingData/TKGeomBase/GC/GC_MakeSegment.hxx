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

#ifndef _GC_MakeSegment_HeaderFile
#define _GC_MakeSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_TrimmedCurve.hxx>

class gp_Pnt;
class gp_Lin;

//! Implements construction algorithms for a line
//! segment in 3D space.
//! Makes a segment of Line from the 2 points <P1> and <P2>.
//! The result is a Geom_TrimmedCurve curve.
//! A MakeSegment object provides a framework for:
//! -   defining the construction of the line segment,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed line segment.
class GC_MakeSegment : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a segment of a line from two points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @note Construction fails if the two points are coincident.
  Standard_EXPORT GC_MakeSegment(const gp_Pnt& theP1, const gp_Pnt& theP2);

  //! Creates a segment of the input line between two parameters.
  //! @param[in] theLine source line
  //! @param[in] theU1 first parameter
  //! @param[in] theU2 second parameter
  //! @note Construction fails when both parameters are equal.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& theLine, const double theU1, const double theU2);

  //! Creates a segment of the input line between a point and a parameter.
  //! @param[in] theLine source line
  //! @param[in] thePoint start point on line
  //! @param[in] theUlast end parameter
  //! @note Construction fails if trimming parameters are equal.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& theLine, const gp_Pnt& thePoint, const double theUlast);

  //! Creates a segment of the input line between two points.
  //! @param[in] theLine source line
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @note Construction fails if trimming parameters are equal.
  Standard_EXPORT GC_MakeSegment(const gp_Lin& theLine, const gp_Pnt& theP1, const gp_Pnt& theP2);

  //! Returns the constructed line segment.
  //! @return resulting line segment
  Standard_EXPORT const occ::handle<Geom_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom_TrimmedCurve> TheSegment;
};

#endif // _GC_MakeSegment_HeaderFile
