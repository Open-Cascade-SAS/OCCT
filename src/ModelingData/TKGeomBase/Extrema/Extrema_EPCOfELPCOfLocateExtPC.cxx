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

#include <Extrema_EPCOfELPCOfLocateExtPC.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_FunctionRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

Extrema_EPCOfELPCOfLocateExtPC::Extrema_EPCOfELPCOfLocateExtPC()
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

Extrema_EPCOfELPCOfLocateExtPC::Extrema_EPCOfELPCOfLocateExtPC(const gp_Pnt&          P,
                                                               const Adaptor3d_Curve& C,
                                                               const Standard_Integer NbSample,
                                                               const Standard_Real    TolU,
                                                               const Standard_Real    TolF)
    : myF(P, C)
{
  Initialize(C, NbSample, TolU, TolF);
  Perform(P);
}

//=================================================================================================

Extrema_EPCOfELPCOfLocateExtPC::Extrema_EPCOfELPCOfLocateExtPC(const gp_Pnt&          P,
                                                               const Adaptor3d_Curve& C,
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

void Extrema_EPCOfELPCOfLocateExtPC::Initialize(const Adaptor3d_Curve& C,
                                                const Standard_Integer NbU,
                                                const Standard_Real    TolU,
                                                const Standard_Real    TolF)
{
  myInit     = Standard_True;
  mynbsample = NbU;
  mytolu     = TolU;
  mytolF     = TolF;
  myF.Initialize(C);
  myumin = Extrema_CurveTool::FirstParameter(C);
  myusup = Extrema_CurveTool::LastParameter(C);
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC::Initialize(const Adaptor3d_Curve& C,
                                                const Standard_Integer NbU,
                                                const Standard_Real    Umin,
                                                const Standard_Real    Usup,
                                                const Standard_Real    TolU,
                                                const Standard_Real    TolF)
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

void Extrema_EPCOfELPCOfLocateExtPC::Initialize(const Standard_Integer NbU,
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

void Extrema_EPCOfELPCOfLocateExtPC::Initialize(const Adaptor3d_Curve& C)
{
  myF.Initialize(C);
}

//=================================================================================================

void Extrema_EPCOfELPCOfLocateExtPC::Perform(const gp_Pnt& P)
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

Standard_Boolean Extrema_EPCOfELPCOfLocateExtPC::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Integer Extrema_EPCOfELPCOfLocateExtPC::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//=================================================================================================

Standard_Real Extrema_EPCOfELPCOfLocateExtPC::SquareDistance(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.SquareDistance(N);
}

//=================================================================================================

Standard_Boolean Extrema_EPCOfELPCOfLocateExtPC::IsMin(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.IsMin(N);
}

//=================================================================================================

const Extrema_POnCurv& Extrema_EPCOfELPCOfLocateExtPC::Point(const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  return myF.Point(N);
}
