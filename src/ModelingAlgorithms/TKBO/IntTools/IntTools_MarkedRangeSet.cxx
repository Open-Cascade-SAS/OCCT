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

#include <IntTools_MarkedRangeSet.hxx>

#include <IntTools_Range.hxx>

IntTools_MarkedRangeSet::IntTools_MarkedRangeSet()
    : myRangeNumber(0)
{
}

IntTools_MarkedRangeSet::IntTools_MarkedRangeSet(const double    theFirstBoundary,
                                                 const double    theLastBoundary,
                                                 const int theInitFlag)
{
  SetBoundaries(theFirstBoundary, theLastBoundary, theInitFlag);
}

IntTools_MarkedRangeSet::IntTools_MarkedRangeSet(const NCollection_Array1<double>& theSortedArray,
                                                 const int      theInitFlag)
{
  SetRanges(theSortedArray, theInitFlag);
}

void IntTools_MarkedRangeSet::SetBoundaries(const double    theFirstBoundary,
                                            const double    theLastBoundary,
                                            const int theInitFlag)
{
  myRangeSetStorer.Clear();
  myRangeSetStorer.Append(theFirstBoundary);
  myRangeSetStorer.Append(theLastBoundary);
  myRangeNumber = 1;
  myFlags.Clear();
  myFlags.Append(theInitFlag);
}

void IntTools_MarkedRangeSet::SetRanges(const NCollection_Array1<double>& theSortedArray,
                                        const int      theInitFlag)
{
  myRangeSetStorer.Clear();
  myFlags.Clear();
  for (NCollection_Array1<double>::Iterator aRangeIter(theSortedArray); aRangeIter.More();
       aRangeIter.Next())
  {
    myRangeSetStorer.Append(aRangeIter.Value());
  }
  myRangeNumber = myRangeSetStorer.Length() - 1;

  for (int i = 1; i <= myRangeNumber; i++)
  {
    myFlags.Append(theInitFlag);
  }
}

bool IntTools_MarkedRangeSet::InsertRange(const double    theFirstBoundary,
                                                      const double    theLastBoundary,
                                                      const int theFlag)
{
  int anIndex1 = GetIndex(theFirstBoundary, true);

  if (!anIndex1)
    return false;
  int anIndex2 = GetIndex(theLastBoundary, false);

  if (!anIndex2)
    return false;

  if (anIndex2 < anIndex1)
  { // it can be if theLastBoundary==theFirstBoundary==boundary_of_a_range or theFirstBoundary >
    // theLastBoundary
    int atmpindex = anIndex1;
    anIndex1                   = anIndex2;
    anIndex2                   = atmpindex;

    if (theLastBoundary < theFirstBoundary)
      return false;
  }

  bool areEqualIndices = (anIndex1 == anIndex2);
  int aPrevFlag       = myFlags(anIndex1);

  myRangeSetStorer.InsertAfter(anIndex1, theFirstBoundary);
  anIndex2++;
  myFlags.InsertAfter(anIndex1, theFlag);
  myRangeNumber = myRangeSetStorer.Length() - 1;

  myRangeSetStorer.InsertAfter(anIndex2, theLastBoundary);

  if (areEqualIndices)
  {
    myFlags.InsertAfter(anIndex2, aPrevFlag);
  }
  else
  {
    myFlags.InsertBefore(anIndex2, theFlag);
  }

  if (!areEqualIndices)
  {
    anIndex1++;
    anIndex2++;

    for (int i = anIndex1; i < anIndex2; i++)
    {
      myFlags.SetValue(i, theFlag);
    }
  }

  myRangeNumber = myRangeSetStorer.Length() - 1;

  return true;
}

bool IntTools_MarkedRangeSet::InsertRange(const IntTools_Range&  theRange,
                                                      const int theFlag)
{
  return InsertRange(theRange.First(), theRange.Last(), theFlag);
}

bool IntTools_MarkedRangeSet::InsertRange(const double    theFirstBoundary,
                                                      const double    theLastBoundary,
                                                      const int theFlag,
                                                      const int theIndex)
{
  double    aTolerance = 1.e-15;
  int anIndex    = theIndex;

  if ((theIndex <= 0) || (theIndex > myRangeNumber))
    return false;

  if ((theFirstBoundary < myRangeSetStorer(theIndex))
      || (theLastBoundary > myRangeSetStorer(theIndex + 1))
      || (std::abs(theFirstBoundary - theLastBoundary) < aTolerance))
  {
    return InsertRange(theFirstBoundary, theLastBoundary, theFlag);
  }
  else
  {
    int aPrevFlag = myFlags(anIndex);

    if ((std::abs(theFirstBoundary - myRangeSetStorer(anIndex)) > aTolerance)
        && (std::abs(theFirstBoundary - myRangeSetStorer(anIndex + 1)) > aTolerance))
    {
      myRangeSetStorer.InsertAfter(anIndex, theFirstBoundary);
      myFlags.InsertAfter(anIndex, theFlag);
      anIndex++;
      myRangeNumber = myRangeSetStorer.Length() - 1;
    }
    else
    {
      myFlags.SetValue(anIndex, theFlag);
    }

    if ((std::abs(theLastBoundary - myRangeSetStorer(anIndex)) > aTolerance)
        && (std::abs(theLastBoundary - myRangeSetStorer(anIndex + 1)) > aTolerance))
    {
      myRangeSetStorer.InsertAfter(anIndex, theLastBoundary);
      myRangeNumber = myRangeSetStorer.Length() - 1;
      myFlags.InsertAfter(anIndex, aPrevFlag);
    }
  }

  return true;
}

bool IntTools_MarkedRangeSet::InsertRange(const IntTools_Range&  theRange,
                                                      const int theFlag,
                                                      const int theIndex)
{
  return InsertRange(theRange.First(), theRange.Last(), theFlag, theIndex);
}

void IntTools_MarkedRangeSet::SetFlag(const int theIndex,
                                      const int theFlag)
{
  myFlags.SetValue(theIndex, theFlag);
}

int IntTools_MarkedRangeSet::Flag(const int theIndex) const
{
  return myFlags(theIndex);
}

const NCollection_Sequence<int>& IntTools_MarkedRangeSet::GetIndices(const double theValue)
{
  myFoundIndices.Clear();

  if (theValue < myRangeSetStorer(1))
    return myFoundIndices;
  else
  {
    bool found = false;

    for (int i = 2; i <= myRangeSetStorer.Length(); i++)
    {
      if (found)
      {
        if (theValue >= myRangeSetStorer(i - 1))
        {
          myFoundIndices.Append(i - 1);
        }
        else
        {
          break;
        }
      }
      else
      {
        if (theValue <= myRangeSetStorer(i))
        {
          myFoundIndices.Append(i - 1);
          found = true;
        }
        else
        {
          if (found)
          {
            break;
          }
        }
      }
    }
  }
  return myFoundIndices;
}

int IntTools_MarkedRangeSet::GetIndex(const double theValue) const
{
  int anIndex = 0;

  if (theValue < myRangeSetStorer(1))
    anIndex = 0;
  else
  {
    for (int i = 2; i <= myRangeSetStorer.Length(); i++)
    {
      if (theValue <= myRangeSetStorer(i))
      {
        anIndex = i - 1;
        break;
      }
    }
  }

  return anIndex;
}

int IntTools_MarkedRangeSet::GetIndex(const double    theValue,
                                                   const bool UseLower) const
{
  int anIndex = 0;

  if ((UseLower && (theValue < myRangeSetStorer(1)))
      || (!UseLower && (theValue <= myRangeSetStorer(1))))
    anIndex = 0;
  else
  {
    for (int i = 2; i <= myRangeSetStorer.Length(); i++)
    {
      if ((UseLower && theValue < myRangeSetStorer(i))
          || (!UseLower && theValue <= myRangeSetStorer(i)))
      {
        anIndex = i - 1;
        break;
      }
    }
  }
  return anIndex;
}

IntTools_Range IntTools_MarkedRangeSet::Range(const int theIndex) const
{
  IntTools_Range aRange(myRangeSetStorer(theIndex), myRangeSetStorer(theIndex + 1));
  return aRange;
}
