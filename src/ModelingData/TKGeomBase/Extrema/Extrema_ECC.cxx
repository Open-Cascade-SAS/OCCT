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

#include <Extrema_ECC.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_GlobOptFuncCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

#include <Extrema_GenExtCC.pxx>

//=================================================================================================

Extrema_ECC::Extrema_ECC()
    : myIsFindSingleSolution(false),
      myParallel(false),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(false)
{
  myC[0] = myC[1] = 0;
}

//=================================================================================================

Extrema_ECC::Extrema_ECC(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2)
    : myIsFindSingleSolution(false),
      myParallel(false),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(false)
{
  myC[0]         = (Standard_Address)&C1;
  myC[1]         = (Standard_Address)&C2;
  myLowBorder(1) = C1.FirstParameter();
  myLowBorder(2) = C2.FirstParameter();
  myUppBorder(1) = C1.LastParameter();
  myUppBorder(2) = C2.LastParameter();
}

//=================================================================================================

Extrema_ECC::Extrema_ECC(const Adaptor3d_Curve& C1,
                         const Adaptor3d_Curve& C2,
                         const Standard_Real    Uinf,
                         const Standard_Real    Usup,
                         const Standard_Real    Vinf,
                         const Standard_Real    Vsup)
    : myIsFindSingleSolution(false),
      myParallel(false),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(false)
{
  myC[0]         = (Standard_Address)&C1;
  myC[1]         = (Standard_Address)&C2;
  myLowBorder(1) = Uinf;
  myLowBorder(2) = Vinf;
  myUppBorder(1) = Usup;
  myUppBorder(2) = Vsup;
}

//=================================================================================================

void Extrema_ECC::SetParams(const Adaptor3d_Curve& C1,
                            const Adaptor3d_Curve& C2,
                            const Standard_Real    Uinf,
                            const Standard_Real    Usup,
                            const Standard_Real    Vinf,
                            const Standard_Real    Vsup)
{
  myC[0]         = (Standard_Address)&C1;
  myC[1]         = (Standard_Address)&C2;
  myLowBorder(1) = Uinf;
  myLowBorder(2) = Vinf;
  myUppBorder(1) = Usup;
  myUppBorder(2) = Vsup;
}

//=================================================================================================

void Extrema_ECC::SetTolerance(Standard_Real theTol)
{
  myCurveMinTol = theTol;
}

//=================================================================================================

void Extrema_ECC::Perform()
{
  myPoints1.Clear();
  myPoints2.Clear();

  const Adaptor3d_Curve& C1 = *static_cast<const Adaptor3d_Curve*>(myC[0]);
  const Adaptor3d_Curve& C2 = *static_cast<const Adaptor3d_Curve*>(myC[1]);

  Extrema_GenExtCC_Impl::PerformGenExtCC<Adaptor3d_Curve,
                                         Extrema_CurveTool,
                                         gp_Pnt,
                                         Extrema_POnCurv,
                                         Extrema_ExtPC,
                                         Extrema_GlobOptFuncCCC2>(C1,
                                                                  C2,
                                                                  myLowBorder,
                                                                  myUppBorder,
                                                                  myCurveMinTol,
                                                                  myIsFindSingleSolution,
                                                                  myDone,
                                                                  myParallel,
                                                                  myPoints1,
                                                                  myPoints2);
}

//=================================================================================================

Standard_Boolean Extrema_ECC::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Boolean Extrema_ECC::IsParallel() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myParallel;
}

//=================================================================================================

Standard_Integer Extrema_ECC::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myPoints1.Length();
}

//=================================================================================================

Standard_Real Extrema_ECC::SquareDistance(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  const Adaptor3d_Curve& C1 = *static_cast<const Adaptor3d_Curve*>(myC[0]);
  const Adaptor3d_Curve& C2 = *static_cast<const Adaptor3d_Curve*>(myC[1]);

  return Extrema_CurveTool::Value(C1, myPoints1(N))
    .SquareDistance(Extrema_CurveTool::Value(C2, myPoints2(N)));
}

//=================================================================================================

void Extrema_ECC::Points(const Standard_Integer N, Extrema_POnCurv& P1, Extrema_POnCurv& P2) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  const Adaptor3d_Curve& C1 = *static_cast<const Adaptor3d_Curve*>(myC[0]);
  const Adaptor3d_Curve& C2 = *static_cast<const Adaptor3d_Curve*>(myC[1]);

  P1.SetValues(myPoints1(N), Extrema_CurveTool::Value(C1, myPoints1(N)));
  P2.SetValues(myPoints2(N), Extrema_CurveTool::Value(C2, myPoints2(N)));
}

//=================================================================================================

void Extrema_ECC::SetSingleSolutionFlag(const Standard_Boolean theFlag)
{
  myIsFindSingleSolution = theFlag;
}

//=================================================================================================

Standard_Boolean Extrema_ECC::GetSingleSolutionFlag() const
{
  return myIsFindSingleSolution;
}
