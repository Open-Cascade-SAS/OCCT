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

#include <Adaptor3d_Surface.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <Extrema_FuncPSDist.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_BFGS.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : Extrema_GenLocateExtPS
//purpose  : 
//=======================================================================
Extrema_GenLocateExtPS::Extrema_GenLocateExtPS(const Adaptor3d_Surface& theS,
                                               const Standard_Real theTolU,
                                               const Standard_Real theTolV)
: mySurf(theS),
  myTolU(theTolU), myTolV(theTolV),
  myDone(Standard_False),
  mySqDist(-1.0)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void Extrema_GenLocateExtPS::Perform(const gp_Pnt& theP,
                                     const Standard_Real theU0,
                                     const Standard_Real theV0,
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

  if (isDistanceCriteria == Standard_False)
  {
    // Normal projection criteria.
    Extrema_FuncPSNorm F(theP,mySurf);

    math_FunctionSetRoot SR (F, aTol);
    SR.Perform(F, aStart, aBoundInf, aBoundSup);
    if (!SR.IsDone()) 
      return;

    mySqDist = F.SquareDistance(1);
    myPoint = F.Point(1);
    myDone = Standard_True;
  }
  else
  {
    // Distance criteria.
    Extrema_FuncPSDist F(mySurf, theP);
    math_BFGS aSolver(2);
    aSolver.Perform(F, aStart);

    if (!aSolver.IsDone()) 
      return;

    math_Vector aResPnt(1,2);
    aSolver.Location(aResPnt);
    mySqDist = aSolver.Minimum();
    myPoint.SetParameters(aResPnt(1), aResPnt(2), mySurf.Value(aResPnt(1), aResPnt(2)));
    myDone = Standard_True;
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenLocateExtPS::IsDone () const
{
  return myDone;
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================
Standard_Real Extrema_GenLocateExtPS::SquareDistance () const
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return mySqDist;
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================
const Extrema_POnSurf& Extrema_GenLocateExtPS::Point () const
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myPoint;
}
