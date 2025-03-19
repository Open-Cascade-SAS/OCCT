// Created on: 1994-07-06
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Extrema_LocateExtCC_HeaderFile
#define _Extrema_LocateExtCC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Extrema_POnCurv.hxx>
class Adaptor3d_Curve;

//! It calculates the distance between two curves with
//! a close point; these distances can be maximum or
//! minimum.
class Extrema_LocateExtCC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Calculates the distance with a close point. The
  //! close point is defined by a parameter value on each
  //! curve.
  //! The function F(u,v)=distance(C1(u),C2(v)) has an
  //! extremun when gradient(f)=0. The algorithm searches
  //! the zero near the close point.
  Standard_EXPORT Extrema_LocateExtCC(const Adaptor3d_Curve& C1,
                                      const Adaptor3d_Curve& C2,
                                      const Standard_Real    U0,
                                      const Standard_Real    V0);

  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the value of the extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance() const;

  //! Returns the points of the extremum distance.
  //! P1 is on the first curve, P2 on the second one.
  Standard_EXPORT void Point(Extrema_POnCurv& P1, Extrema_POnCurv& P2) const;

protected:
private:
  Standard_Boolean myDone;
  Standard_Real    mySqDist;
  Extrema_POnCurv  myPoint1;
  Extrema_POnCurv  myPoint2;
};

#endif // _Extrema_LocateExtCC_HeaderFile
