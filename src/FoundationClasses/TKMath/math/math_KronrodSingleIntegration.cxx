// Created on: 2005-12-08
// Created by: Sergey KHROMOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <math.hxx>
#include <math_Function.hxx>
#include <math_KronrodSingleIntegration.hxx>
#include <math_Vector.hxx>
#include <NCollection_Sequence.hxx>

//==========================================================================
// function : An empty constructor.
//==========================================================================
math_KronrodSingleIntegration::math_KronrodSingleIntegration()
    : myIsDone(false),
      myValue(0.),
      myErrorReached(0.),
      myAbsolutError(0.),
      myNbPntsReached(0),
      myNbIterReached(0)
{
}

//==========================================================================
// function : Constructor
//
//==========================================================================

math_KronrodSingleIntegration::math_KronrodSingleIntegration(math_Function&         theFunction,
                                                             const double    theLower,
                                                             const double    theUpper,
                                                             const int theNbPnts)
    : myIsDone(false),
      myValue(0.),
      myErrorReached(0.),
      myAbsolutError(0.),
      myNbPntsReached(0),
      myNbIterReached(0)
{
  Perform(theFunction, theLower, theUpper, theNbPnts);
}

//==========================================================================
// function : Constructor
//
//==========================================================================

math_KronrodSingleIntegration::math_KronrodSingleIntegration(math_Function&         theFunction,
                                                             const double    theLower,
                                                             const double    theUpper,
                                                             const int theNbPnts,
                                                             const double    theTolerance,
                                                             const int theMaxNbIter)
    : myIsDone(false),
      myValue(0.),
      myErrorReached(0.),
      myAbsolutError(0.),
      myNbPntsReached(0),
      myNbIterReached(0)
{
  Perform(theFunction, theLower, theUpper, theNbPnts, theTolerance, theMaxNbIter);
}

//==========================================================================
// function : Perform
//           Computation of the integral.
//==========================================================================

void math_KronrodSingleIntegration::Perform(math_Function&         theFunction,
                                            const double    theLower,
                                            const double    theUpper,
                                            const int theNbPnts)
{
  // const double aMinVol = Epsilon(1.);
  const double aPtol = 1.e-9;
  myNbIterReached           = 0;

  if (theNbPnts < 3)
  {
    myIsDone = false;
    return;
  }

  if (theUpper - theLower < aPtol)
  {
    myIsDone = false;
    return;
  }

  // Get an odd value of number of initial points.
  myNbPntsReached = (theNbPnts % 2 == 0) ? theNbPnts + 1 : theNbPnts;
  myErrorReached  = RealLast();

  int aNGauss = myNbPntsReached / 2;
  math_Vector      aKronrodP(1, myNbPntsReached);
  math_Vector      aKronrodW(1, myNbPntsReached);
  math_Vector      aGaussP(1, aNGauss);
  math_Vector      aGaussW(1, aNGauss);

  if (!math::KronrodPointsAndWeights(myNbPntsReached, aKronrodP, aKronrodW)
      || !math::OrderedGaussPointsAndWeights(aNGauss, aGaussP, aGaussW))
  {
    myIsDone = false;
    return;
  }

  myIsDone = GKRule(theFunction,
                    theLower,
                    theUpper,
                    aGaussP,
                    aGaussW,
                    aKronrodP,
                    aKronrodW,
                    myValue,
                    myErrorReached);

  if (!myIsDone)
    return;

  // double anAbsVal = std::abs(myValue);

  myAbsolutError = myErrorReached;

  // if (anAbsVal > aMinVol)
  // myErrorReached /= anAbsVal;

  myNbIterReached++;
}

//=================================================================================================

void math_KronrodSingleIntegration::Perform(math_Function&         theFunction,
                                            const double    theLower,
                                            const double    theUpper,
                                            const int theNbPnts,
                                            const double    theTolerance,
                                            const int theMaxNbIter)
{
  double aMinVol = Epsilon(1.);
  myNbIterReached       = 0;

  // Check prerequisites.
  if (theNbPnts < 3 || theTolerance <= 0.)
  {
    myIsDone = false;
    return;
  }
  // Get an odd value of number of initial points.
  myNbPntsReached = (theNbPnts % 2 == 0) ? theNbPnts + 1 : theNbPnts;

  int aNGauss = myNbPntsReached / 2;
  math_Vector      aKronrodP(1, myNbPntsReached);
  math_Vector      aKronrodW(1, myNbPntsReached);
  math_Vector      aGaussP(1, aNGauss);
  math_Vector      aGaussW(1, aNGauss);

  if (!math::KronrodPointsAndWeights(myNbPntsReached, aKronrodP, aKronrodW)
      || !math::OrderedGaussPointsAndWeights(aNGauss, aGaussP, aGaussW))
  {
    myIsDone = false;
    return;
  }

  // First iteration
  myIsDone = GKRule(theFunction,
                    theLower,
                    theUpper,
                    aGaussP,
                    aGaussW,
                    aKronrodP,
                    aKronrodW,
                    myValue,
                    myErrorReached);

  if (!myIsDone)
    return;

  double anAbsVal = std::abs(myValue);

  myAbsolutError = myErrorReached;
  if (anAbsVal > aMinVol)
    myErrorReached /= anAbsVal;

  myNbIterReached++;

  if (myErrorReached <= theTolerance)
    return;
  if (myNbIterReached >= theMaxNbIter)
    return;

  NCollection_Sequence<double> anIntervals;
  NCollection_Sequence<double> anErrors;
  NCollection_Sequence<double> aValues;

  anIntervals.Append(theLower);
  anIntervals.Append(theUpper);

  anErrors.Append(myAbsolutError);
  aValues.Append(myValue);

  int i, nint, nbints;

  double    maxerr;
  int count = 0;

  while (myErrorReached > theTolerance && myNbIterReached < theMaxNbIter)
  {
    // Searching interval with max error
    nbints = anIntervals.Length() - 1;
    nint   = 0;
    maxerr = 0.;
    for (i = 1; i <= nbints; ++i)
    {
      if (anErrors(i) > maxerr)
      {
        maxerr = anErrors(i);
        nint   = i;
      }
    }

    double a = anIntervals(nint);
    double b = anIntervals(nint + 1);
    double c = 0.5 * (a + b);

    double v1, v2, e1, e2;

    myIsDone = GKRule(theFunction, a, c, aGaussP, aGaussW, aKronrodP, aKronrodW, v1, e1);

    if (!myIsDone)
      return;

    myIsDone = GKRule(theFunction, c, b, aGaussP, aGaussW, aKronrodP, aKronrodW, v2, e2);

    if (!myIsDone)
      return;

    myNbIterReached++;

    double deltav = v1 + v2 - aValues(nint);
    myValue += deltav;
    if (std::abs(deltav) <= Epsilon(std::abs(myValue)))
      ++count;

    double deltae = e1 + e2 - anErrors(nint);
    myAbsolutError += deltae;
    if (myAbsolutError <= Epsilon(std::abs(myValue)))
      ++count;

    if (std::abs(myValue) > aMinVol)
      myErrorReached = myAbsolutError / std::abs(myValue);
    else
      myErrorReached = myAbsolutError;

    if (count > 50)
      return;

    // Inserting new interval

    anIntervals.InsertAfter(nint, c);
    anErrors(nint) = e1;
    anErrors.InsertAfter(nint, e2);
    aValues(nint) = v1;
    aValues.InsertAfter(nint, v2);
  }
}

//=================================================================================================

bool math_KronrodSingleIntegration::GKRule(math_Function&      theFunction,
                                                       const double theLower,
                                                       const double theUpper,
                                                       const math_Vector& /*theGaussP*/,
                                                       const math_Vector& theGaussW,
                                                       const math_Vector& theKronrodP,
                                                       const math_Vector& theKronrodW,
                                                       double&     theValue,
                                                       double&     theError)
{

  bool IsDone;

  int aNKronrod = theKronrodP.Length();

  double    aGaussVal;
  int aNPnt2 = (aNKronrod + 1) / 2;
  int i;
  double    aDx;
  double    aVal1;
  double    aVal2;

  math_Vector f1(1, aNPnt2 - 1);
  math_Vector f2(1, aNPnt2 - 1);

  double aXm = 0.5 * (theUpper + theLower);
  double aXr = 0.5 * (theUpper - theLower);

  // Compute Gauss quadrature
  aGaussVal = 0.;
  theValue  = 0.;

  for (i = 2; i < aNPnt2; i += 2)
  {
    aDx = aXr * theKronrodP.Value(i);

    if (!theFunction.Value(aXm + aDx, aVal1) || !theFunction.Value(aXm - aDx, aVal2))
    {
      IsDone = false;
      return IsDone;
    }

    f1(i) = aVal1;
    f2(i) = aVal2;
    aGaussVal += (aVal1 + aVal2) * theGaussW.Value(i / 2);
    theValue += (aVal1 + aVal2) * theKronrodW.Value(i);
  }

  // Compute value in the middle point.
  if (!theFunction.Value(aXm, aVal1))
  {
    IsDone = false;
    return IsDone;
  }

  double fc = aVal1;
  theValue += aVal1 * theKronrodW.Value(aNPnt2);

  if (i == aNPnt2)
    aGaussVal += aVal1 * theGaussW.Value(aNPnt2 / 2);

  // Compute Kronrod quadrature
  for (i = 1; i < aNPnt2; i += 2)
  {
    aDx = aXr * theKronrodP.Value(i);

    if (!theFunction.Value(aXm + aDx, aVal1) || !theFunction.Value(aXm - aDx, aVal2))
    {
      IsDone = false;
      return IsDone;
    }

    f1(i) = aVal1;
    f2(i) = aVal2;
    theValue += (aVal1 + aVal2) * theKronrodW.Value(i);
  }

  double mean = 0.5 * theValue;

  double asc = std::abs(fc - mean) * theKronrodW.Value(aNPnt2);
  for (i = 1; i < aNPnt2; ++i)
  {
    asc += theKronrodW.Value(i) * (std::abs(f1(i) - mean) + std::abs(f2(i) - mean));
  }

  asc *= aXr;
  theValue *= aXr;
  aGaussVal *= aXr;

  // Compute the error and the new number of Kronrod points.

  theError = std::abs(theValue - aGaussVal);

  double scale = 1.;
  if (asc != 0. && theError != 0.)
    scale = std::pow((200. * theError / asc), 1.5);
  if (scale < 1.)
    theError = std::min(theError, asc * scale);

  // theFunction.GetStateNumber();

  return true;
}
