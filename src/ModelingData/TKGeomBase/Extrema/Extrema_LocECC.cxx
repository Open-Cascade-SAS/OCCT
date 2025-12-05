// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Extrema_LocECC.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CCLocFOfLocECC.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

Extrema_LocECC::Extrema_LocECC(const Adaptor3d_Curve& C1,
                               const Adaptor3d_Curve& C2,
                               const Standard_Real    U0,
                               const Standard_Real    V0,
                               const Standard_Real    TolU,
                               const Standard_Real    TolV)
{
  myDone   = Standard_False;
  mySqDist = RealLast();

  Standard_Real Uinf = Extrema_CurveTool::FirstParameter(C1);
  Standard_Real Usup = Extrema_CurveTool::LastParameter(C1);
  Standard_Real Uu;
  if (Uinf > Usup)
  {
    Uu   = Uinf;
    Uinf = Usup;
    Usup = Uu;
  }

  Standard_Real Vinf = Extrema_CurveTool::FirstParameter(C2);
  Standard_Real Vsup = Extrema_CurveTool::LastParameter(C2);
  if (Vinf > Vsup)
  {
    Uu   = Vinf;
    Vinf = Vsup;
    Vsup = Uu;
  }

  Extrema_CCLocFOfLocECC F(C1, C2);
  math_Vector            Tol(1, 2);
  Tol(1)             = TolU;
  Tol(2)             = TolV;
  Standard_Real Tolf = 1.e-10;

  math_Vector Start(1, 2);
  math_Vector Uuinf(1, 2);
  math_Vector Uusup(1, 2);

  Start(1) = U0;
  Start(2) = V0;

  Uuinf(1) = Uinf;
  Uuinf(2) = Vinf;
  Uusup(1) = Usup;
  Uusup(2) = Vsup;

  math_FunctionSetRoot S(F, Tol);
  S.Perform(F, Start, Uuinf, Uusup);

  if (S.IsDone() && F.NbExt() > 0)
  {
    mySqDist = F.SquareDistance(1);
    F.Points(1, myPoint1, myPoint2);
    Start(1) = myPoint1.Parameter();
    Start(2) = myPoint2.Parameter();
    math_Vector Ff(1, 2);
    F.Value(Start, Ff);
    if ((Ff(1) < Tolf) && (Ff(2) < Tolf))
      myDone = Standard_True;
  }
}

//=================================================================================================

Standard_Boolean Extrema_LocECC::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Real Extrema_LocECC::SquareDistance() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return mySqDist;
}

//=================================================================================================

void Extrema_LocECC::Point(Extrema_POnCurv& P1, Extrema_POnCurv& P2) const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  P1 = myPoint1;
  P2 = myPoint2;
}
