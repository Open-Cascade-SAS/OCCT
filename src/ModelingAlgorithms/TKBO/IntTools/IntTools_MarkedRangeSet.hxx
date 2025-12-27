// Created on: 2001-09-26
// Created by: Michael KLOKOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_MarkedRangeSet_HeaderFile
#define _IntTools_MarkedRangeSet_HeaderFile

#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>

class IntTools_Range;

//! class MarkedRangeSet provides continuous set of ranges marked with flags
class IntTools_MarkedRangeSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_MarkedRangeSet();

  //! build set of ranges which consists of one range with
  //! boundary values theFirstBoundary and theLastBoundary
  Standard_EXPORT IntTools_MarkedRangeSet(const double    theFirstBoundary,
                                          const double    theLastBoundary,
                                          const int theInitFlag);

  //! Build set of ranges based on the array of progressive sorted values
  //!
  //! Warning:
  //! The constructor do not check if the values of array are not sorted
  //! It should be checked before function invocation
  Standard_EXPORT IntTools_MarkedRangeSet(const NCollection_Array1<double>& theSortedArray,
                                          const int      theInitFlag);

  //! build set of ranges which consists of one range with
  //! boundary values theFirstBoundary and theLastBoundary
  Standard_EXPORT void SetBoundaries(const double    theFirstBoundary,
                                     const double    theLastBoundary,
                                     const int theInitFlag);

  //! Build set of ranges based on the array of progressive sorted values
  //!
  //! Warning:
  //! The function do not check if the values of array are not sorted
  //! It should be checked before function invocation
  Standard_EXPORT void SetRanges(const NCollection_Array1<double>& theSortedArray,
                                 const int      theInitFlag);

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT bool InsertRange(const double    theFirstBoundary,
                                               const double    theLastBoundary,
                                               const int theFlag);

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT bool InsertRange(const IntTools_Range&  theRange,
                                               const int theFlag);

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! The index theIndex is a position where the range will be inserted.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT bool InsertRange(const double    theFirstBoundary,
                                               const double    theLastBoundary,
                                               const int theFlag,
                                               const int theIndex);

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! The index theIndex is a position where the range will be inserted.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT bool InsertRange(const IntTools_Range&  theRange,
                                               const int theFlag,
                                               const int theIndex);

  //! Set flag theFlag for range with index theIndex
  Standard_EXPORT void SetFlag(const int theIndex, const int theFlag);

  //! Returns flag of the range with index theIndex
  Standard_EXPORT int Flag(const int theIndex) const;

  //! Returns index of range which contains theValue.
  //! If theValue do not belong any range returns 0.
  Standard_EXPORT int GetIndex(const double theValue) const;

  Standard_EXPORT const NCollection_Sequence<int>& GetIndices(const double theValue);

  //! Returns index of range which contains theValue
  //! If theValue do not belong any range returns 0.
  //! If UseLower is true then lower boundary of the range
  //! can be equal to theValue, otherwise upper boundary of the range
  //! can be equal to theValue.
  Standard_EXPORT int GetIndex(const double    theValue,
                                            const bool UseLower) const;

  //! Returns number of ranges
  int Length() const { return myRangeNumber; }

  //! Returns the range with index theIndex.
  //! the Index can be from 1 to Length()
  Standard_EXPORT IntTools_Range Range(const int theIndex) const;

private:
  NCollection_Sequence<double>    myRangeSetStorer;
  int          myRangeNumber;
  NCollection_Sequence<int> myFlags;
  NCollection_Sequence<int> myFoundIndices;
};

#endif // _IntTools_MarkedRangeSet_HeaderFile
