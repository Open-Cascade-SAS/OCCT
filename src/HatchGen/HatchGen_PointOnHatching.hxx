// Created on: 1993-10-29
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HatchGen_PointOnHatching_HeaderFile
#define _HatchGen_PointOnHatching_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HatchGen_PointsOnElement.hxx>
#include <HatchGen_IntersectionPoint.hxx>
#include <Standard_Boolean.hxx>
class IntRes2d_IntersectionPoint;
class HatchGen_PointOnElement;

class HatchGen_PointOnHatching : public HatchGen_IntersectionPoint
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty point.
  Standard_EXPORT HatchGen_PointOnHatching();

  //! Creates a point from an intersection point.
  Standard_EXPORT HatchGen_PointOnHatching(const IntRes2d_IntersectionPoint& Point);

  //! Adds a point on element to the point.
  Standard_EXPORT void AddPoint(const HatchGen_PointOnElement& Point,
                                const Standard_Real            Confusion);

  //! Returns the number of elements intersecting the
  //! hatching at this point.
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Returns the Index-th point on element of the point.
  //! The exception OutOfRange is raised if
  //! Index > NbPoints.
  Standard_EXPORT const HatchGen_PointOnElement& Point(const Standard_Integer Index) const;

  //! Removes the Index-th point on element of the point.
  //! The exception OutOfRange is raised if
  //! Index > NbPoints.
  Standard_EXPORT void RemPoint(const Standard_Integer Index);

  //! Removes all the points on element of the point.
  Standard_EXPORT void ClrPoints();

  //! Tests if the point is lower than an other.
  //! A point on hatching P1 is said to be lower than an
  //! other P2 if :
  //! P2.myParam - P1.myParam > Confusion
  Standard_EXPORT Standard_Boolean IsLower(const HatchGen_PointOnHatching& Point,
                                           const Standard_Real             Confusion) const;

  //! Tests if the  point is equal to an other.
  //! A  point on hatching P1 is said to be equal to an
  //! other P2 if :
  //! | P2.myParam - P1.myParam | <= Confusion
  Standard_EXPORT Standard_Boolean IsEqual(const HatchGen_PointOnHatching& Point,
                                           const Standard_Real             Confusion) const;

  //! Tests if the point is greater than an other.
  //! A point on hatching P1 is said to be greater than an
  //! other P2 if :
  //! P1.myParam - P2.myParam > Confusion
  Standard_EXPORT Standard_Boolean IsGreater(const HatchGen_PointOnHatching& Point,
                                             const Standard_Real             Confusion) const;

  //! Dump of the point.
  Standard_EXPORT void Dump(const Standard_Integer Index = 0) const;

protected:
  HatchGen_PointsOnElement myPoints;

private:
};

#endif // _HatchGen_PointOnHatching_HeaderFile
