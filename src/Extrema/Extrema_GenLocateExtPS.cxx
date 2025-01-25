// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Extrema_GenLocateExtPS.hxx>

#include <Extrema_FuncPSNorm.hxx>
#include <Extrema_FuncPSDist.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_BFGS.hxx>
#include <math_FRPR.hxx>
#include <StdFail_NotDone.hxx>

static void CorrectTol(const Standard_Real theU0, const Standard_Real theV0, math_Vector& theTol)
{
  // Correct tolerance for large values of UV parameters
  constexpr Standard_Real aTolRef  = Precision::PConfusion();
  Standard_Real           anEpsRef = Epsilon(1.);
  Standard_Real           epsu     = Epsilon(theU0);
  const Standard_Real     tolog10  = 0.43429;
  if (epsu > anEpsRef)
  {
    Standard_Integer n = RealToInt(tolog10 * Log(epsu / anEpsRef) + 1) + 1;
    Standard_Integer i;
    Standard_Real    tol = aTolRef;
    for (i = 1; i <= n; ++i)
    {
      tol *= 10.;
    }
    theTol(1) = Max(theTol(1), tol);
  }
  Standard_Real epsv = Epsilon(theV0);
  if (epsv > anEpsRef)
  {
    Standard_Integer n = RealToInt(tolog10 * Log(epsv / anEpsRef) + 1) + 1;
    Standard_Integer i;
    Standard_Real    tol = aTolRef;
    for (i = 1; i <= n; ++i)
    {
      tol *= 10.;
    }
    theTol(2) = Max(theTol(2), tol);
  }
}

//=================================================================================================

Standard_Boolean Extrema_GenLocateExtPS::IsMinDist(const gp_Pnt&            theP,
                                                   const Adaptor3d_Surface& theS,
                                                   const Standard_Real      theU0,
                                                   const Standard_Real      theV0)
{
  Standard_Real du =
    Max(theS.UResolution(10. * Precision::Confusion()), 10. * Precision::PConfusion());
  Standard_Real dv =
    Max(theS.VResolution(10. * Precision::Confusion()), 10. * Precision::PConfusion());
  Standard_Real    u, v;
  gp_Pnt           aP0 = theS.Value(theU0, theV0);
  Standard_Real    d0  = theP.SquareDistance(aP0);
  Standard_Integer iu, iv;
  for (iu = -1; iu <= 1; ++iu)
  {
    u = theU0 + iu * du;
    if (!theS.IsUPeriodic())
    {
      u = Max(u, theS.FirstUParameter());
      u = Min(u, theS.LastUParameter());
    }
    for (iv = -1; iv <= 1; ++iv)
    {
      if (iu == 0 && iv == 0)
        continue;

      v = theV0 + iv * dv;
      if (!theS.IsVPeriodic())
      {
        v = Max(v, theS.FirstVParameter());
        v = Min(v, theS.LastVParameter());
      }
      Standard_Real d = theP.SquareDistance(theS.Value(u, v));
      if (d < d0)
        return Standard_False;
    }
  }
  return Standard_True;
}

//=================================================================================================

Extrema_GenLocateExtPS::Extrema_GenLocateExtPS(const Adaptor3d_Surface& theS,
                                               const Standard_Real      theTolU,
                                               const Standard_Real      theTolV)
    : mySurf(theS),
      myTolU(theTolU),
      myTolV(theTolV),
      myDone(Standard_False),
      mySqDist(-1.0)
{
}

//=================================================================================================

void Extrema_GenLocateExtPS::Perform(const gp_Pnt&          theP,
                                     const Standard_Real    theU0,
                                     const Standard_Real    theV0,
                                     const Standard_Boolean isDistanceCriteria)
{
  myDone = Standard_False;

  // Prepare initial data structures.
  math_Vector aTol(1, 2), aStart(1, 2), aBoundInf(1, 2), aBoundSup(1, 2);

  // Tolerance.
  aTol(1) = myTolU;
  aTol(2) = myTolV;

  // Initial solution approximation.
  aStart(1) = theU0;
  aStart(2) = theV0;

  // Borders.
  aBoundInf(1) = mySurf.FirstUParameter();
  aBoundInf(2) = mySurf.FirstVParameter();
  aBoundSup(1) = mySurf.LastUParameter();
  aBoundSup(2) = mySurf.LastVParameter();

  if (isDistanceCriteria)
  {
    // Distance criteria.
    Standard_Real aRelTol = 1.e-8;
    math_Vector   aResPnt(1, 2);

    Extrema_FuncPSDist F(mySurf, theP);

    math_BFGS aSolver(2, aRelTol);
    aSolver.Perform(F, aStart);

    if (!aSolver.IsDone())
    {
      // Try another method
      math_FRPR aSolver1(F, aRelTol);
      aSolver1.Perform(F, aStart);
      if (!aSolver1.IsDone())
        return;
      aSolver1.Location(aResPnt);
      mySqDist = aSolver1.Minimum();
    }
    else
    {
      aSolver.Location(aResPnt);
      mySqDist = aSolver.Minimum();
    }

    myPoint.SetParameters(aResPnt(1), aResPnt(2), mySurf.Value(aResPnt(1), aResPnt(2)));
    myDone = Standard_True;
  }
  else
  {
    // Normal projection criteria.
    Extrema_FuncPSNorm F(theP, mySurf);

    if (mySurf.GetType() == GeomAbs_BSplineSurface)
    {
      aTol(1) = myTolU;
      aTol(2) = myTolV;
      CorrectTol(theU0, theV0, aTol);
    }

    Standard_Boolean isCorrectTol = (Abs(aTol(1) - myTolU) > Precision::PConfusion()
                                     || Abs(aTol(2) - myTolV) > Precision::PConfusion());

    math_FunctionSetRoot aSR(F, aTol);
    aSR.Perform(F, aStart, aBoundInf, aBoundSup);

    if (!aSR.IsDone() || isCorrectTol)
    {
      if (isCorrectTol)
      {
        aTol(1) = myTolU;
        aTol(2) = myTolV;
      }
      math_NewtonFunctionSetRoot aNSR(F, aTol, Precision::Confusion());
      aNSR.Perform(F, aStart, aBoundInf, aBoundSup);
      if (!aSR.IsDone() && !aNSR.IsDone())
      {
        return;
      }
    }

    Standard_Real aNbExt = F.NbExt();
    mySqDist             = F.SquareDistance(1);
    myPoint              = F.Point(1);
    Standard_Integer i;
    for (i = 2; i <= aNbExt; ++i)
    {
      if (F.SquareDistance(i) < mySqDist)
      {
        mySqDist = F.SquareDistance(i);
        myPoint  = F.Point(i);
      }
    }
    myDone = Standard_True;
  }
}

//=================================================================================================

Standard_Boolean Extrema_GenLocateExtPS::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Real Extrema_GenLocateExtPS::SquareDistance() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return mySqDist;
}

//=================================================================================================

const Extrema_POnSurf& Extrema_GenLocateExtPS::Point() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myPoint;
}
