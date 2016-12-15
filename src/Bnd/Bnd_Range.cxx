// Created on: 2016-06-07
// Created by: Nikolai BUKHALOV
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <Bnd_Range.hxx>

//=======================================================================
//function : Common
//purpose  : 
//=======================================================================
void Bnd_Range::Common(const Bnd_Range& theOther)
{
  if(theOther.IsVoid())
  {
    SetVoid();
  }

  if(IsVoid())
  {
    return;
  }

  myFirst = Max(myFirst, theOther.myFirst);
  myLast = Min(myLast, theOther.myLast);
}

//=======================================================================
//function : Union
//purpose  : 
//=======================================================================
Standard_Boolean Bnd_Range::Union(const Bnd_Range& theOther)
{
  if (IsVoid() || theOther.IsVoid())
    return Standard_False;

  if (myLast < theOther.myFirst)
    return Standard_False;

  if (myFirst > theOther.myLast)
    return Standard_False;

  myFirst = Min(myFirst, theOther.myFirst);
  myLast = Max(myLast, theOther.myLast);

  return Standard_True;
}

//=======================================================================
//function : IsIntersected
//purpose  : 
//=======================================================================
Standard_Integer Bnd_Range::IsIntersected(const Standard_Real theVal,
                                          const Standard_Real thePeriod) const
{
  if (IsVoid())
    return Standard_False;

  const Standard_Real aPeriod = Abs(thePeriod);
  const Standard_Real aDF = myFirst - theVal,
                      aDL = myLast - theVal;

  if (aPeriod <= RealSmall())
  { 
    const Standard_Real aDelta = aDF*aDL;
    if (IsEqual(aDelta, 0.0))
      return 2;

    if (aDelta > 0.0)
      return 0;

    return 1;
  }

  //If <this> intersects theVal then there exists an integer
  //number N such as 
  //    (myFirst <= theVal+aPeriod*N <= myLast) <=>
  //    ((myFirst-theVal)/aPeriod <= N <= (myLast-theVal)/aPeriod).
  //I.e. the interval [aDF/aPeriod, aDL/aPeriod] must contain at least one
  //integer number.
  //In this case, Floor(aDF/aPeriod) and Floor(aDL/aPeriod)
  //return different values or aDF/aPeriod (aDL/aPeriod)
  //is strictly integer number.
  //Examples:
  //  1. (aDF/aPeriod==2.8, aDL/aPeriod==3.5 =>
  //        Floor(aDF/aPeriod) == 2, Floor(aDL/aPeriod) == 3.
  //  2. aDF/aPeriod==2.0, aDL/aPeriod==2.6 =>
  //        Floor(aDF/aPeriod) == Floor(aDL/aPeriod) == 2.

  const Standard_Real aVal1 = aDF / aPeriod,
                      aVal2 = aDL / aPeriod;
  const Standard_Integer aPar1 = static_cast<Standard_Integer>(Floor(aVal1));
  const Standard_Integer aPar2 = static_cast<Standard_Integer>(Floor(aVal2));
  if (aPar1 != aPar2)
  {//Interval (myFirst, myLast] intersects seam-edge
    if (IsEqual(aVal2, static_cast<Standard_Real>(aPar2)))
    {//aVal2 is an integer number => myLast lies ON the "seam-edge"
      return 2;
    }

    return 1;
  }

  //Here, aPar1 == aPar2. 

  if (IsEqual(aVal1, static_cast<Standard_Real>(aPar1)))
  {//aVal1 is an integer number => myFirst lies ON the "seam-edge"
    return 2;
  }

#if 0
  // This check is excess because always myFirst <= myLast.
  // So, this condition is never satisfied.
  if (IsEqual(aVal2, static_cast<Standard_Real>(aPar2)))
  {//aVal2 is an integer number => myLast lies ON the "seam-edge"
    return 2;
  }
#endif

  return 0;
}

//=======================================================================
//function : Split
//purpose  : 
//=======================================================================
void Bnd_Range::Split(const Standard_Real theVal,
                      NCollection_List<Bnd_Range>& theList,
                      const Standard_Real thePeriod) const
{
  const Standard_Real aPeriod = Abs(thePeriod);
  if (IsIntersected(theVal, aPeriod) != 1)
  {
    theList.Append(*this);
    return;
  }

  const Standard_Boolean isPeriodic = (aPeriod > 0.0);

  if (!isPeriodic)
  {
    theList.Append(Bnd_Range(myFirst, theVal));
    theList.Append(Bnd_Range(theVal, myLast));
    return;
  }

  Standard_Real aValPrev = theVal + aPeriod*Ceiling((myFirst - theVal) / aPeriod);

  //Now, (myFirst <= aValPrev < myFirst+aPeriod).

  if (aValPrev > myFirst)
  {
    theList.Append(Bnd_Range(myFirst, aValPrev));
  }

  for (Standard_Real aVal = aValPrev+aPeriod; aVal <= myLast; aVal += aPeriod)
  {
    theList.Append(Bnd_Range(aValPrev, aVal));
    aValPrev = aVal;
  }

  if (aValPrev < myLast)
  {
    theList.Append(Bnd_Range(aValPrev, myLast));
  }
}