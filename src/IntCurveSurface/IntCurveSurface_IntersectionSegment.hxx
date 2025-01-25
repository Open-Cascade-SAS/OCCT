// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_IntersectionSegment_HeaderFile
#define _IntCurveSurface_IntersectionSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntCurveSurface_IntersectionPoint.hxx>

//! A IntersectionSegment describes a segment of curve
//! (w1,w2) where distance(C(w),Surface) is less than a
//! given tolerances.
class IntCurveSurface_IntersectionSegment
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntCurveSurface_IntersectionSegment();

  Standard_EXPORT IntCurveSurface_IntersectionSegment(const IntCurveSurface_IntersectionPoint& P1,
                                                      const IntCurveSurface_IntersectionPoint& P2);

  Standard_EXPORT void SetValues(const IntCurveSurface_IntersectionPoint& P1,
                                 const IntCurveSurface_IntersectionPoint& P2);

  Standard_EXPORT void Values(IntCurveSurface_IntersectionPoint& P1,
                              IntCurveSurface_IntersectionPoint& P2) const;

  Standard_EXPORT void FirstPoint(IntCurveSurface_IntersectionPoint& P1) const;

  Standard_EXPORT void SecondPoint(IntCurveSurface_IntersectionPoint& P2) const;

  Standard_EXPORT const IntCurveSurface_IntersectionPoint& FirstPoint() const;

  Standard_EXPORT const IntCurveSurface_IntersectionPoint& SecondPoint() const;

  Standard_EXPORT void Dump() const;

protected:
private:
  IntCurveSurface_IntersectionPoint myP1;
  IntCurveSurface_IntersectionPoint myP2;
};

#endif // _IntCurveSurface_IntersectionSegment_HeaderFile
