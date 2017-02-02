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


#include <Adaptor3d_Curve.hxx>
#include <Extrema_LocateExtCC.hxx>
#include <Extrema_LocECC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

Extrema_LocateExtCC::Extrema_LocateExtCC (const Adaptor3d_Curve& C1,
                                          const Adaptor3d_Curve& C2,
                                          const Standard_Real U0,
                                          const Standard_Real V0)
{
  Standard_Real TolU = C1.Resolution(Precision::Confusion());
  Standard_Real TolV = C2.Resolution(Precision::Confusion());
  Extrema_POnCurv P1, P2;

  // Non implemente pour l instant: l appel a Extrema_ELCC.

  Extrema_LocECC Xtrem(C1, C2,
    U0, V0, TolU, TolV);	
  // Exploitation

  myDone = Xtrem.IsDone();
  if (Xtrem.IsDone()) {
    mySqDist = Xtrem.SquareDistance();
    Xtrem.Point(P1, P2);
    myPoint1 = P1;
    myPoint2 = P2;
  }
}

Standard_Boolean Extrema_LocateExtCC::IsDone () const {

  return myDone;
}

Standard_Real Extrema_LocateExtCC::SquareDistance() const {

  if (!myDone) { throw StdFail_NotDone(); }
  return mySqDist;
}

void Extrema_LocateExtCC::Point (Extrema_POnCurv& P1, Extrema_POnCurv& P2) const {

  if (!myDone) { throw StdFail_NotDone(); }
  P1 = myPoint1;
  P2 = myPoint2;
}

