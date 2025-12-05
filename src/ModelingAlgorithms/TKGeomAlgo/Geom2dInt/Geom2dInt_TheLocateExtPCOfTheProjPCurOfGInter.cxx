// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#include <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <gp_Pnt2d.hxx>
#include <math_FunctionRoot.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter()
    : myDone(Standard_False),
      mytolU(0.0),
      myumin(0.0),
      myusup(0.0)
{
}

//=================================================================================================

Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter(
  const gp_Pnt2d&          P,
  const Adaptor2d_Curve2d& C,
  const Standard_Real      U0,
  const Standard_Real      TolU)
{
  Initialize(C,
             Geom2dInt_Geom2dCurveTool::FirstParameter(C),
             Geom2dInt_Geom2dCurveTool::LastParameter(C),
             TolU);
  Perform(P, U0);
}

//=================================================================================================

Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter(
  const gp_Pnt2d&          P,
  const Adaptor2d_Curve2d& C,
  const Standard_Real      U0,
  const Standard_Real      Umin,
  const Standard_Real      Usup,
  const Standard_Real      TolU)
{
  Initialize(C, Umin, Usup, TolU);
  Perform(P, U0);
}

//=================================================================================================

void Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Initialize(const Adaptor2d_Curve2d& C,
                                                               const Standard_Real      Umin,
                                                               const Standard_Real      Usup,
                                                               const Standard_Real      TolU)
{
  myDone = Standard_False;
  myF.Initialize(C);
  myumin = Umin;
  myusup = Usup;
  mytolU = TolU;
}

//=================================================================================================

void Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Perform(const gp_Pnt2d&     P,
                                                            const Standard_Real U0)
{
  myF.SetPoint(P);
  math_FunctionRoot S(myF, U0, mytolU, myumin, myusup);
  myDone = S.IsDone();
  if (myDone)
  {
    Standard_Real            uu, ff;
    const Extrema_POnCurv2d& PP = Point();
    uu                          = PP.Parameter();
    if (myF.Value(uu, ff))
    {
      if (std::abs(ff) >= 1.e-07)
        myDone = Standard_False;
    }
    else
      myDone = Standard_False;
  }
}

//=================================================================================================

Standard_Boolean Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Real Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::SquareDistance() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.SquareDistance(1);
}

//=================================================================================================

Standard_Boolean Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::IsMin() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.IsMin(1);
}

//=================================================================================================

const Extrema_POnCurv2d& Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter::Point() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.Point(1);
}
