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

#include <Extrema_LocateExtCC2d.hxx>

#include <GeomAbs_CurveType.hxx>
#include <StdFail_NotDone.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Precision.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_LocECC2d.hxx>


//=======================================================================
//function : Extrema_LocateExtCC2d
//purpose  : 
//=======================================================================

Extrema_LocateExtCC2d::Extrema_LocateExtCC2d (const Adaptor2d_Curve2d&       C1,
                                              const Adaptor2d_Curve2d&       C2,
                                              const Standard_Real U0,
                                              const Standard_Real V0)
{
  Standard_Real TolU = Extrema_Curve2dTool::Resolution(C1, Precision::Confusion());
  Standard_Real TolV = Extrema_Curve2dTool::Resolution(C2, Precision::Confusion());
  Extrema_POnCurv2d P1, P2;

  // Non implemente pour l instant: l appel a Geom2dExtrema_ExtCC.


  Extrema_LocECC2d Xtrem(C1, C2, U0, V0, TolU, TolV);	
  // Exploitation

  myDone = Xtrem.IsDone();
  if (Xtrem.IsDone()) {
    mySqDist = Xtrem.SquareDistance();
    Xtrem.Point(P1, P2);
    myPoint1 = P1;
    myPoint2 = P2;
  }

}




//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_LocateExtCC2d::IsDone () const {

  return myDone;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_LocateExtCC2d::SquareDistance() const {

  if (!myDone) { throw StdFail_NotDone(); }
  return mySqDist;
}



//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

void Extrema_LocateExtCC2d::Point (Extrema_POnCurv2d& P1, 
                                   Extrema_POnCurv2d& P2) const 
{
  if (!myDone) { throw StdFail_NotDone(); }
  P1 = myPoint1;
  P2 = myPoint2;
}
