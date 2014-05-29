// Created on: 2014-01-20
// Created by: Alexaner Malyshev
// Copyright (c) 2014-2014 OPEN CASCADE SAS
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
#include <Precision.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(math_GlobOptMin)
{
  static Handle(Standard_Type) _atype = new Standard_Type ("math_GlobOptMin", sizeof (math_GlobOptMin));
  return _atype;
}

//=======================================================================
//function : math_GlobOptMin
//purpose  : Constructor
//=======================================================================
math_GlobOptMin::math_GlobOptMin(math_MultipleVarFunction* theFunc,
                                 const math_Vector& theA,
                                 const math_Vector& theB,
                                 Standard_Real theC)
: myN(theFunc->NbVariables()),
  myA(1, myN),
  myB(1, myN),
  myGlobA(1, myN),
  myGlobB(1, myN),
  myX(1, myN),
  myTmp(1, myN),
  myV(1, myN)
{
  Standard_Integer i;

  myFunc = theFunc;
  myC = theC;
  myZ = -1;
  mySolCount = 0;

  for(i = 1; i <= myN; i++)
  {
    myGlobA(i) = theA(i);
    myGlobB(i) = theB(i);

    myA(i) = theA(i);
    myB(i) = theB(i);
  }

  myDone = Standard_False;
}

//=======================================================================
//function : SetGlobalParams
//purpose  : Set params without memory allocation.
//=======================================================================
void math_GlobOptMin::SetGlobalParams(math_MultipleVarFunction* theFunc,
                                      const math_Vector& theA,
                                      const math_Vector& theB,
                                      Standard_Real theC)
{
  Standard_Integer i;

  myFunc = theFunc;
  myC = theC;
  myZ = -1;
  mySolCount = 0;

  for(i = 1; i <= myN; i++)
  {
    myGlobA(i) = theA(i);
    myGlobB(i) = theB(i);

    myA(i) = theA(i);
    myB(i) = theB(i);
  }

  myDone = Standard_False;
}

//=======================================================================
//function : SetLocalParams
//purpose  : Set params without memory allocation.
//=======================================================================
void math_GlobOptMin::SetLocalParams(const math_Vector& theLocalA,
                                     const math_Vector& theLocalB)
{
  Standard_Integer i;

  myZ = -1;
  mySolCount = 0;

  for(i = 1; i <= myN; i++)
  {
    myA(i) = theLocalA(i);
    myB(i) = theLocalB(i);
  }

  myDone = Standard_False;
}

//=======================================================================
//function : ~math_GlobOptMin
//purpose  : 
//=======================================================================
math_GlobOptMin::~math_GlobOptMin()
{
}

//=======================================================================
//function : Perform
//purpose  : Compute Global extremum point
//=======================================================================
// In this algo indexes started from 1, not from 0.
void math_GlobOptMin::Perform()
{
  Standard_Integer i;

  // Compute parameters range
  Standard_Real minLength = RealLast();
  Standard_Real maxLength = RealFirst();
  for(i = 1; i <= myN; i++)
  {
    Standard_Real currentLength = myB(i) - myA(i);
    if (currentLength < minLength)
      minLength = currentLength;
    if (currentLength > maxLength)
      maxLength = currentLength;
  }

  Standard_Real myTol = 1e-2;

  myE1 = minLength * myTol / myC;
  myE2 = 2.0 * myTol / myC * maxLength;
  myE3 = - maxLength * myTol / 4;

  // Compure start point.
  math_Vector aPnt(1,myN);
  for(i = 1; i <= myN; i++)
  {
    Standard_Real currCentral = (myA(i) + myB(i)) / 2.0;
    aPnt(i) = currCentral;
  }

  myFunc->Value(aPnt, myF);

  math_Vector aExtremumPoint(1,myN);
  Standard_Real aExtremumValue = RealLast();
  if (computeLocalExtremum(aPnt, aExtremumValue, aExtremumPoint))
  {
    // Local Extremum finds better solution than midpoint.
    if (aExtremumValue < myF)
    {
      myF = aExtremumValue;
      aPnt = aExtremumPoint;
    }
  }

  myY.Clear();
  for(i = 1; i <= myN; i++)
    myY.Append(aPnt(i));
  mySolCount++;

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
  if (dynamic_cast<math_MultipleVarFunctionWithHessian*>(myFunc))
  {
    math_MultipleVarFunctionWithHessian* myTmp = 
      dynamic_cast<math_MultipleVarFunctionWithHessian*> (myFunc);
    
    math_NewtonMinimum newtonMinimum(*myTmp, thePnt);
    if (newtonMinimum.IsDone())
    {
      newtonMinimum.Location(theOutPnt);
      theVal = newtonMinimum.Minimum();
    }
    else return Standard_False;
  } else

  // BFGS method used.
  if (dynamic_cast<math_MultipleVarFunctionWithGradient*>(myFunc))
  {
    math_MultipleVarFunctionWithGradient* myTmp = 
      dynamic_cast<math_MultipleVarFunctionWithGradient*> (myFunc);
    math_BFGS bfgs(*myTmp, thePnt);
    if (bfgs.IsDone())
    {
      bfgs.Location(theOutPnt);
      theVal = bfgs.Minimum();
    }
    else return Standard_False;
  } else

  // Powell method used.
  if (dynamic_cast<math_MultipleVarFunction*>(myFunc))
  {
    math_Matrix m(1, myN, 1, myN, 0.0);
    for(i = 1; i <= myN; i++)
      m(1, 1) = 1.0;

    math_Powell powell(*myFunc, thePnt, m, 1e-10);

    if (powell.IsDone())
    {
      powell.Location(theOutPnt);
      theVal = powell.Minimum();
    }
    else return Standard_False;
  }

  if (isInside(theOutPnt))
    return Standard_True;
  else
    return Standard_False;
}

//=======================================================================
//function : ComputeGlobalExtremum
//purpose  :
//=======================================================================
void math_GlobOptMin::computeGlobalExtremum(Standard_Integer j)
{
  Standard_Integer i;
  Standard_Real d; // Functional in moved point.
  Standard_Real val = RealLast(); // Local extrema computed in moved point.
  Standard_Real stepBestValue = RealLast();
  math_Vector stepBestPoint(1,2);
  Standard_Boolean isInside = Standard_False;
  Standard_Real r;

  for(myX(j) = myA(j) + myE1; myX(j) < myB(j) + myE1; myX(j) += myV(j))
  {
    if (myX(j) > myB(j))
      myX(j) = myB(j);

    if (j == 1)
    {
      isInside = Standard_False;
      myFunc->Value(myX, d);
      r = (d - myF) * myZ;
      if(r > myE3)
      {
        isInside = computeLocalExtremum(myX, val, myTmp);
      }
      stepBestValue = (isInside && (val < d))? val : d;
      stepBestPoint = (isInside && (val < d))? myTmp : myX;

      // Solutions are close to each other.
      if (Abs(stepBestValue - myF) < Precision::Confusion() * 0.01)
      {
        if (!isStored(stepBestPoint))
        {
          if ((stepBestValue - myF) * myZ > 0.0)
            myF = stepBestValue;
          for(i = 1; i <= myN; i++)
            myY.Append(stepBestPoint(i));
          mySolCount++;
        }
      }

      // New best solution.
      if ((stepBestValue - myF) * myZ > Precision::Confusion() * 0.01)
      {
        mySolCount = 0;
        myF = stepBestValue;
        myY.Clear();
        for(i = 1; i <= myN; i++)
          myY.Append(stepBestPoint(i));
        mySolCount++;
      }

      myV(1) = myE2 + Abs(myF - d) / myC;
    }
    else
    {
      myV(j) = RealLast() / 2.0;
      computeGlobalExtremum(j - 1);
    }
    if ((j < myN) && (myV(j + 1) > myV(j)))
    {
      if (myV(j) > (myB(j + 1) - myA(j + 1)) / 3.0) // Case of too big step.
        myV(j + 1) = (myB(j + 1) - myA(j + 1)) / 3.0; 
      else
        myV(j + 1) = myV(j);
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

  for(i = 0; i < mySolCount; i++)
  {
    isSame = Standard_True;
    for(j = 1; j <= myN; j++)
    {
      if ((Abs(thePnt(j) - myY(i * myN + j))) > (myB(j) -  myA(j)) * Precision::Confusion())
      {
        isSame = Standard_False;
        break;
      }
    }
    if (isSame == Standard_True)
      return Standard_True;

  }
  return Standard_False;
}

//=======================================================================
//function : NbExtrema
//purpose  :
//=======================================================================
Standard_Integer math_GlobOptMin::NbExtrema()
{
  return mySolCount;
}

//=======================================================================
//function : GetF
//purpose  :
//=======================================================================
Standard_Real math_GlobOptMin::GetF()
{
  return myF;
}

//=======================================================================
//function : IsDone
//purpose  :
//=======================================================================
Standard_Boolean math_GlobOptMin::isDone()
{
  return myDone;
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
