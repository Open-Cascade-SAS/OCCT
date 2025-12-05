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

#include <Extrema_EPCOfELPCOfLocateExtPC2d.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

Extrema_EPCOfELPCOfLocateExtPC2d::Extrema_EPCOfELPCOfLocateExtPC2d()
    : myDone(Standard_False),
      myInit(Standard_False),
      mynbsample(0),
      myumin(0.0),
      myusup(0.0),
      mytolu(0.0),
      mytolF(0.0)
{
}

//=================================================================================================

Extrema_EPCOfELPCOfLocateExtPC2d::Extrema_EPCOfELPCOfLocateExtPC2d(const gp_Pnt2d&          P,
                                                                   const Adaptor2d_Curve2d& C,
                                                                   const Standard_Integer NbSample,
                                                                   const Standard_Real    TolU,
                                                                   const Standard_Real    TolF)
    : myF(P, C)
{
  Initialize(C, NbSample, TolU, TolF);
  Perform(P);
}

//=================================================================================================

Extrema_EPCOfELPCOfLocateExtPC2d::Extrema_EPCOfELPCOfLocateExtPC2d(const gp_Pnt2d&          P,
                                                                   const Adaptor2d_Curve2d& C,
                                                                   const Standard_Integer NbSample,
                                                                   const Standard_Real    Umin,
                                                                   const Standard_Real    Usup,
                                                                   const Standard_Real    TolU,
                                                                   const Standard_Real    TolF)
    : myF(P, C)
{
  Initialize(C, NbSample, Umin, Usup, TolU, TolF);
  Perform(P);
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC2d::Initialize(const Adaptor2d_Curve2d& C,
                                                  const Standard_Integer   NbU,
                                                  const Standard_Real      TolU,
                                                  const Standard_Real      TolF)
{
  myInit     = Standard_True;
  mynbsample = NbU;
  mytolu     = TolU;
  mytolF     = TolF;
  myF.Initialize(C);
  myumin = Extrema_Curve2dTool::FirstParameter(C);
  myusup = Extrema_Curve2dTool::LastParameter(C);
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC2d::Initialize(const Adaptor2d_Curve2d& C,
                                                  const Standard_Integer   NbU,
                                                  const Standard_Real      Umin,
                                                  const Standard_Real      Usup,
                                                  const Standard_Real      TolU,
                                                  const Standard_Real      TolF)
{
  myInit     = Standard_True;
  mynbsample = NbU;
  mytolu     = TolU;
  mytolF     = TolF;
  myF.Initialize(C);
  myumin = Umin;
  myusup = Usup;
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC2d::Initialize(const Standard_Integer NbU,
                                                  const Standard_Real    Umin,
                                                  const Standard_Real    Usup,
                                                  const Standard_Real    TolU,
                                                  const Standard_Real    TolF)
{
  mynbsample = NbU;
  mytolu     = TolU;
  mytolF     = TolF;
  myumin     = Umin;
  myusup     = Usup;
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC2d::Initialize(const Adaptor2d_Curve2d& C)
{
  myF.Initialize(C);
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC2d::Perform(const gp_Pnt2d& P)
{
  myF.SetPoint(P);
  myF.SubIntervalInitialize(myumin, myusup);
  myDone = Standard_False;

  math_FunctionRoots S(myF, myumin, myusup, mynbsample, mytolu, mytolF, mytolF);
  if (!S.IsDone() || S.IsAllNull())
  {
    return;
  }

  myDone = Standard_True;
}

//=================================================================================================

Standard_Boolean Extrema_EPCOfELPCOfLocateExtPC2d::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Integer Extrema_EPCOfELPCOfLocateExtPC2d::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//=================================================================================================

Standard_Real Extrema_EPCOfELPCOfLocateExtPC2d::SquareDistance(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.SquareDistance(N);
}

//=================================================================================================

Standard_Boolean Extrema_EPCOfELPCOfLocateExtPC2d::IsMin(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.IsMin(N);
}

//=================================================================================================

const Extrema_POnCurv2d& Extrema_EPCOfELPCOfLocateExtPC2d::Point(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.Point(N);
}
