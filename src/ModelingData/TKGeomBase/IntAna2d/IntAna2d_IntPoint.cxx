// Created on: 1992-10-07
// Created by: Laurent BUCHARD
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

#include <IntAna2d_IntPoint.hxx>

IntAna2d_IntPoint::IntAna2d_IntPoint(const double X,
                                     const double Y,
                                     const double U1,
                                     const double U2)
    : myu1(U1),
      myu2(U2),
      myp(X, Y),
      myimplicit(false)
{
}

IntAna2d_IntPoint::IntAna2d_IntPoint(const double X, const double Y, const double U1)
    : myu1(U1),
      myu2(RealLast()),
      myp(X, Y),
      myimplicit(true)
{
}

IntAna2d_IntPoint::IntAna2d_IntPoint()
    :

      myu1(RealLast()),
      myu2(RealLast()),
      myp(RealLast(), RealLast()),
      myimplicit(false)
{
}

void IntAna2d_IntPoint::SetValue(const double X, const double Y, const double U1, const double U2)
{

  myimplicit = false;
  myp.SetCoord(X, Y);
  myu1 = U1;
  myu2 = U2;
}

void IntAna2d_IntPoint::SetValue(const double X, const double Y, const double U1)
{

  myimplicit = true;
  myp.SetCoord(X, Y);
  myu1 = U1;
  myu2 = RealLast();
}
