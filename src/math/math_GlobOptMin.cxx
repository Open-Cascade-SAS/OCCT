// Created on: 2014-01-20
// Created by: Alexaner Malyshev
// Copyright (c) 2014-2015 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#include <math_GlobOptMin.hxx>

#include <math_BFGS.hxx>
#include <math_Matrix.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_NewtonMinimum.hxx>
#include <math_Powell.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Precision.hxx>

//=======================================================================
//function : DistanceToBorder
//purpose  :
//=======================================================================
static Standard_Real DistanceToBorder(const math_Vector & theX,
                                      const math_Vector & theMin,
                                      const math_Vector & theMax)
{
  Standard_Real aDist = RealLast();

  for (Standard_Integer anIdx = theMin.Lower(); anIdx <= theMin.Upper(); ++anIdx)
  {
    const Standard_Real aDist1 = Abs (theX(anIdx) - theMin(anIdx));
    const Standard_Real aDist2 = Abs (theX(anIdx) - theMax(anIdx));

    aDist = Min (aDist, Min (aDist1, aDist2));
  }

  return aDist;
}


//=======================================================================
//function : math_GlobOptMin
//purpose  : Constructor
//=======================================================================
math_GlobOptMin::math_GlobOptMin(math_MultipleVarFunction* theFunc,
                                 const math_Vector& theA,
                                 const math_Vector& theB,
                                 const Standard_Real theC,
                                 const Standard_Real theDiscretizationTol,
                                 const Standard_Real theSameTol)
: myN(theFunc->NbVariables()),
  myA(1, myN),
  myB(1, myN),
  myGlobA(1, myN),
  myGlobB(1, myN),
  myIsConstLocked(Standard_False),
  myX(1, myN),
  myTmp(1, myN),
  myV(1, myN),
  myMaxV(1, myN),
  myCellSize(0, myN - 1),
  myFilter(theFunc->NbVariables()),
  myCont(2)
{
  Standard_Integer i;

  myFunc = theFunc;
  myC = theC;
  myInitC = theC;
  myIsFindSingleSolution = Standard_False;
  myFunctionalMinimalValue = -Precision::Infinite();
  myZ = -1;
  mySolCount = 0;

  for(i = 1; i <= myN; i++)
  {
    myGlobA(i) = theA(i);
    myGlobB(i) = theB(i);

    myA(i) = theA(i);
    myB(i) = theB(i);
  }

  for(i = 1; i <= myN; i++)
  {
    myMaxV(i) = (myB(i) - myA(i)) / 3.0;
  }

  myTol = theDiscretizationTol;
  mySameTol = theSameTol;

  const Standard_Integer aMaxSquareSearchSol = 200;
  Standard_Integer aSolNb = Standard_Integer(Pow(3.0, Standard_Real(myN)));
  myMinCellFilterSol = Max(2 * aSolNb, aMaxSquareSearchSol);
  initCellSize();
  ComputeInitSol();

  myDone = Standard_False;
}

//=======================================================================
//function : SetGlobalParams
//purpose  : Set parameters without memory allocation.
//=======================================================================
void math_GlobOptMin::SetGlobalParams(math_MultipleVarFunction* theFunc,
                                      const math_Vector& theA,
                                      const math_Vector& theB,
                                      const Standard_Real theC,
                                      const Standard_Real theDiscretizationTol,
                                      const Standard_Real theSameTol)
{
  Standard_Integer i;

  myFunc = theFunc;
  myC = theC;
  myInitC = theC;
  myZ = -1;
  mySolCount = 0;

  for(i = 1; i <= myN; i++)
  {
    myGlobA(i) = theA(i);
    myGlobB(i) = theB(i);

    myA(i) = theA(i);
    myB(i) = theB(i);
  }

  for(i = 1; i <= myN; i++)
  {
    myMaxV(i) = (myB(i) - myA(i)) / 3.0;
  }

  myTol = theDiscretizationTol;
  mySameTol = theSameTol;

  initCellSize();
  ComputeInitSol();

  myDone = Standard_False;
}

//=======================================================================
//function : SetLocalParams
//purpose  : Set parameters without memory allocation.
//=======================================================================
void math_GlobOptMin::SetLocalParams(const math_Vector& theLocalA,
                                     const math_Vector& theLocalB)
{
  Standard_Integer i;

  myZ = -1;
  for(i = 1; i <= myN; i++)
  {
    myA(i) = theLocalA(i);
    myB(i) = theLocalB(i);
  }

  for(i = 1; i <= myN; i++)
  {
    myMaxV(i) = (myB(i) - myA(i)) / 3.0;
  }

  myDone = Standard_False;
}

//=======================================================================
//function : SetTol
//purpose  : Set algorithm tolerances.
//=======================================================================
void math_GlobOptMin::SetTol(const Standard_Real theDiscretizationTol,
                             const Standard_Real theSameTol)
{
  myTol = theDiscretizationTol;
  mySameTol = theSameTol;
}

//=======================================================================
//function : GetTol
//purpose  : Get algorithm tolerances.
//=======================================================================
void math_GlobOptMin::GetTol(Standard_Real& theDiscretizationTol,
                             Standard_Real& theSameTol)
{
  theDiscretizationTol = myTol;
  theSameTol = mySameTol;
}

//=======================================================================
//function : Perform
//purpose  : Compute Global extremum point
//=======================================================================
// In this algo indexes started from 1, not from 0.
void math_GlobOptMin::Perform(const Standard_Boolean isFindSingleSolution)
{
  myDone = Standard_False;

  // Compute parameters range
  Standard_Real minLength = RealLast();
  Standard_Real maxLength = RealFirst();
  for(Standard_Integer i = 1; i <= myN; i++)
  {
    Standard_Real currentLength = myB(i) - myA(i);
    if (currentLength < minLength)
      minLength = currentLength;
    if (currentLength > maxLength)
      maxLength = currentLength;

    myV(i) = 0.0;
  }

  if (minLength < Precision::PConfusion())
  {
    #ifdef OCCT_DEBUG
    cout << "math_GlobOptMin::Perform(): Degenerated parameters space" << endl;
    #endif

    return;
  }

  if (!myIsConstLocked)
  {
    // Compute initial value for myC.
    computeInitialValues();
  }

  myE1 = minLength * myTol;
  myE2 = maxLength * myTol;

  myIsFindSingleSolution = isFindSingleSolution;
  if (isFindSingleSolution)
  {
    // Run local optimization if current value better than optimal.
    myE3 = 0.0;
  }
  else
  {
    if (myC > 1.0)
      myE3 = - maxLength * myTol / 4.0;
    else
      myE3 = - maxLength * myTol * myC / 4.0;
  }

  // Search single solution and current solution in its neighborhood.
  if (CheckFunctionalStopCriteria())
  {
    myDone = Standard_True;
    return;
  }

  myLastStep = 0.0;
  isFirstCellFilterInvoke = Standard_True;
  computeGlobalExtremum(myN);

  myDone = Standard_True;
}

//=======================================================================
//function : computeLocalExtremum
//purpose  :
//=======================================================================
Standard_Boolean math_GlobOptMin::computeLocalExtremum(const math_Vector& thePnt,
                                                       Standard_Real& theVal,
                                                       math_Vector& theOutPnt)
{
  Standard_Integer i;

  //Newton method
  if (myCont >= 2 &&
      dynamic_cast<math_MultipleVarFunctionWithHessian*>(myFunc))
  {
    math_MultipleVarFunctionWithHessian* aTmp = 
      dynamic_cast<math_MultipleVarFunctionWithHessian*> (myFunc);
    math_NewtonMinimum newtonMinimum(*aTmp);
    newtonMinimum.SetBoundary(myGlobA, myGlobB);
    newtonMinimum.Perform(*aTmp, thePnt);

    if (newtonMinimum.IsDone())
    {
      newtonMinimum.Location(theOutPnt);
      theVal = newtonMinimum.Minimum();

      if (isInside(theOutPnt))
        return Standard_True;
    }
  }

  // BFGS method used.
  if (myCont >= 1 &&
      dynamic_cast<math_MultipleVarFunctionWithGradient*>(myFunc))
  {
    math_MultipleVarFunctionWithGradient* aTmp =
      dynamic_cast<math_MultipleVarFunctionWithGradient*> (myFunc);
    math_BFGS bfgs(aTmp->NbVariables());
    bfgs.SetBoundary(myGlobA, myGlobB);
    bfgs.Perform(*aTmp, thePnt);

    if (bfgs.IsDone())
    {
      bfgs.Location(theOutPnt);
      theVal = bfgs.Minimum();

      if (isInside(theOutPnt))
        return Standard_True;
    }
  }

  // Powell method used.
  if (dynamic_cast<math_MultipleVarFunction*>(myFunc))
  {
    math_Matrix m(1, myN, 1, myN, 0.0);
    for(i = 1; i <= myN; i++)
      m(i, i) = 1.0;

    math_Powell powell(*myFunc, 1e-10);
    powell.Perform(*myFunc, thePnt, m);

    if (powell.IsDone())
    {
      powell.Location(theOutPnt);
      theVal = powell.Minimum();

      if (isInside(theOutPnt))
        return Standard_True;
    }
  }

  return Standard_False;
}

//=======================================================================
//function : computeInitialValues
//purpose  : 
//=======================================================================
void math_GlobOptMin::computeInitialValues()
{
  const Standard_Real aMinLC = 0.01;
  const Standard_Real aMaxLC = 1000.;
  const Standard_Real aMinEps = 0.1;
  const Standard_Real aMaxEps = 100.;
  Standard_Integer i;
  math_Vector aCurrPnt(1, myN);
  math_Vector aBestPnt(1, myN);
  math_Vector aParamStep(1, myN);
  Standard_Real aCurrVal = RealLast();

  // Lipchitz const approximation.
  Standard_Real aLipConst = 0.0, aPrevValDiag, aPrevValProj;
  Standard_Integer aPntNb = 13;
  myFunc->Value(myA, aPrevValDiag);
  aPrevValProj = aPrevValDiag;
  Standard_Real aStep = (myB - myA).Norm() / aPntNb;
  aParamStep = (myB - myA) / aPntNb;
  for(i = 1; i <= aPntNb; i++)
  {
    aCurrPnt = myA + aParamStep * i;

    // Walk over diagonal.
    myFunc->Value(aCurrPnt, aCurrVal);
    aLipConst = Max (Abs(aCurrVal - aPrevValDiag), aLipConst);
    aPrevValDiag = aCurrVal;

    // Walk over diag in projected space aPnt(1) = myA(1) = const.
    aCurrPnt(1) = myA(1);
    myFunc->Value(aCurrPnt, aCurrVal);
    aLipConst = Max (Abs(aCurrVal - aPrevValProj), aLipConst);
    aPrevValProj = aCurrVal;
  }

  myC = myInitC;
  aLipConst *= Sqrt(myN) / aStep;
  if (aLipConst < myC * aMinEps)
    myC = Max(aLipConst * aMinEps, aMinLC);
  else if (aLipConst > myC * aMaxEps)
    myC = Min(myC * aMaxEps, aMaxLC);

  // Clear all solutions except one.
  if (myY.Size() != myN)
  {
    for(i = 1; i <= myN; i++)
      aBestPnt(i) = myY(i);
    myY.Clear();
    for(i = 1; i <= myN; i++)
      myY.Append(aBestPnt(i));
  }
  mySolCount = 1;
}

//=======================================================================
//function : ComputeGlobalExtremum
//purpose  :
//=======================================================================
void math_GlobOptMin::computeGlobalExtremum(Standard_Integer j)
{
  Standard_Integer i;
  Standard_Real d = RealLast(), aPrevVal; // Functional in original and moved points.
  Standard_Real val = RealLast(); // Local extrema computed in moved point.
  Standard_Real aStepBestValue = RealLast();
  math_Vector aStepBestPoint(1, myN);
  Standard_Boolean isInside = Standard_False,
                   isReached = Standard_False;

  Standard_Real r1, r2, r;

  for(myX(j) = myA(j) + myE1; !isReached; myX(j) += myV(j))
  {
    if (myX(j) > myB(j))
    {
      myX(j) = myB(j);
      isReached = Standard_True;
    }

    if (CheckFunctionalStopCriteria())
      return; // Best possible value is obtained.

    if (j == 1)
    {
      isInside = Standard_False;
      aPrevVal = d;
      myFunc->Value(myX, d);
      r1 = (d + myZ * myC * myLastStep - myF) * myZ; // Evtushenko estimation.
      r2 = ((d + aPrevVal - myC * myLastStep) * 0.5 - myF) * myZ; // Shubert / Piyavsky estimation.
      r = Min(r1, r2);
      if(r > myE3)
      {
        Standard_Real aSaveParam = myX(1);

        // Piyavsky midpoint estimation.
        Standard_Real aParam = (2 * myX(1) - myV(1) ) * 0.5 + (aPrevVal - d) * 0.5 / myC;
        if (Precision::IsInfinite(aPrevVal))
          aParam = myX(1) - myV(1) * 0.5; // Protection from upper dimension step.

        myX(1) = aParam;
        Standard_Real aVal = 0;
        myFunc->Value(myX, aVal);
        myX(1) = aSaveParam;

        if ( (aVal < d && aVal < aPrevVal) ||
              DistanceToBorder(myX, myA, myB) < myE1 ) // Condition optimization case near the border.
        {
          isInside = computeLocalExtremum(myX, val, myTmp);
        }
      }
      aStepBestValue = (isInside && (val < d))? val : d;
      aStepBestPoint = (isInside && (val < d))? myTmp : myX;

      // Solutions are close to each other 
      // and it is allowed to have more than one solution.
      if (Abs(aStepBestValue - myF) < mySameTol * 0.01 &&
          !myIsFindSingleSolution)
      {
        if (!isStored(aStepBestPoint))
        {
          if ((aStepBestValue - myF) * myZ > 0.0)
            myF = aStepBestValue;
          for(i = 1; i <= myN; i++)
            myY.Append(aStepBestPoint(i));
          mySolCount++;
        }
      }

      // New best solution:
      // new point is out of (mySameTol * 0.01) surrounding or
      // new point is better than old + single point search.
      Standard_Real aFunctionalDelta = (aStepBestValue - myF) * myZ;
      if (aFunctionalDelta > mySameTol * 0.01 ||
         (aFunctionalDelta > 0.0 && myIsFindSingleSolution))
      {
        mySolCount = 0;
        myF = aStepBestValue;
        myY.Clear();
        for(i = 1; i <= myN; i++)
          myY.Append(aStepBestPoint(i));
        mySolCount++;

        isFirstCellFilterInvoke = Standard_True;
      }

      if (CheckFunctionalStopCriteria())
        return; // Best possible value is obtained.

      myV(1) = Min(myE2 + Abs(myF - d) / myC, myMaxV(1));
      myLastStep = myV(1);
    }
    else
    {
      myV(j) = RealLast() / 2.0;
      computeGlobalExtremum(j - 1);

      // Nullify steps on lower dimensions.
      for(i = 1; i < j; i++)
        myV(i) = 0.0;
    }
    if (j < myN)
    {
      Standard_Real aUpperDimStep =  Max(myV(j), myE2);
      if (myV(j + 1) > aUpperDimStep)
      {
        if (aUpperDimStep > myMaxV(j + 1)) // Case of too big step.
          myV(j + 1) = myMaxV(j + 1); 
        else
          myV(j + 1) = aUpperDimStep;
      }
    }
  }
}

//=======================================================================
//function : IsInside
//purpose  :
//=======================================================================
Standard_Boolean math_GlobOptMin::isInside(const math_Vector& thePnt)
{
  Standard_Integer i;

 for(i = 1; i <= myN; i++)
 {
   if (thePnt(i) < myGlobA(i) || thePnt(i) > myGlobB(i))
     return Standard_False;
 }

 return Standard_True;
}
//=======================================================================
//function : IsStored
//purpose  :
//=======================================================================
Standard_Boolean math_GlobOptMin::isStored(const math_Vector& thePnt)
{
  Standard_Integer i,j;
  Standard_Boolean isSame = Standard_True;
  math_Vector aTol(1, myN);
  aTol = (myB -  myA) * mySameTol;

  // C1 * n^2 = C2 * 3^dim * n
  if (mySolCount < myMinCellFilterSol)
  {
    for(i = 0; i < mySolCount; i++)
    {
      isSame = Standard_True;
      for(j = 1; j <= myN; j++)
      {
        if ((Abs(thePnt(j) - myY(i * myN + j))) > aTol(j))
        {
          isSame = Standard_False;
          break;
        }
      }
      if (isSame == Standard_True)
        return Standard_True;
    }
  }
  else
  {
    NCollection_CellFilter_Inspector anInspector(myN, Precision::PConfusion());
    if (isFirstCellFilterInvoke)
    {
      myFilter.Reset(myCellSize);

      // Copy initial data into cell filter.
      for(Standard_Integer aSolIdx = 0; aSolIdx < mySolCount; aSolIdx++)
      {
        math_Vector aVec(1, myN);
        for(Standard_Integer aSolDim = 1; aSolDim <= myN; aSolDim++)
          aVec(aSolDim) = myY(aSolIdx * myN + aSolDim);

        myFilter.Add(aVec, aVec);
      }
    }

    isFirstCellFilterInvoke = Standard_False;

    math_Vector aLow(1, myN), anUp(1, myN);
    anInspector.Shift(thePnt, myCellSize, aLow, anUp);

    anInspector.ClearFind();
    anInspector.SetCurrent(thePnt);
    myFilter.Inspect(aLow, anUp, anInspector);
    if (!anInspector.isFind())
    {
      // Point is out of close cells, add new one.
      myFilter.Add(thePnt, thePnt);
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Points
//purpose  :
//=======================================================================
void math_GlobOptMin::Points(const Standard_Integer theIndex, math_Vector& theSol)
{
  Standard_Integer j;

  for(j = 1; j <= myN; j++)
    theSol(j) = myY((theIndex - 1) * myN + j);
}

//=======================================================================
//function : initCellSize
//purpose  :
//=======================================================================
void math_GlobOptMin::initCellSize()
{
  for(Standard_Integer anIdx = 1; anIdx <= myN; anIdx++)
  {
    myCellSize(anIdx - 1) = (myGlobB(anIdx) - myGlobA(anIdx))
      * Precision::PConfusion() / (2.0 * Sqrt(2.0));
  }
}

//=======================================================================
//function : CheckFunctionalStopCriteria
//purpose  :
//=======================================================================
Standard_Boolean math_GlobOptMin::CheckFunctionalStopCriteria()
{
  // Search single solution and current solution in its neighborhood.
  if (myIsFindSingleSolution &&
      Abs (myF - myFunctionalMinimalValue) < mySameTol * 0.01)
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : ComputeInitSol
//purpose  :
//=======================================================================
void math_GlobOptMin::ComputeInitSol()
{
  Standard_Real aCurrVal, aBestVal;
  math_Vector aCurrPnt(1, myN);
  math_Vector aBestPnt(1, myN);
  math_Vector aParamStep(1, myN);
  // Check functional value in midpoint, lower and upper border points and
  // in each point try to perform local optimization.
  aBestPnt = (myGlobA + myGlobB) * 0.5;
  myFunc->Value(aBestPnt, aBestVal);

  Standard_Integer i;
  for(i = 1; i <= 3; i++)
  {
    aCurrPnt = myA + (myB - myA) * (i - 1) / 2.0;

    if(computeLocalExtremum(aCurrPnt, aCurrVal, aCurrPnt))
    {
      // Local search tries to find better solution than current point.
      if (aCurrVal < aBestVal)
      {
        aBestVal = aCurrVal;
        aBestPnt = aCurrPnt;
      }
    }
  }

  myF = aBestVal;
  myY.Clear();
  for(i = 1; i <= myN; i++)
    myY.Append(aBestPnt(i));
  mySolCount = 1;
}
