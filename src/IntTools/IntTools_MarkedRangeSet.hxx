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

#include <IntTools_CArray1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>

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
  Standard_EXPORT IntTools_MarkedRangeSet(const Standard_Real theFirstBoundary, const Standard_Real theLastBoundary, const Standard_Integer theInitFlag);
  

  //! Build set of ranges based on the array of progressive sorted values
  //!
  //! Warning:
  //! The constructor do not check if the values of array are not sorted
  //! It should be checked before function invocation
  Standard_EXPORT IntTools_MarkedRangeSet(const TColStd_Array1OfReal& theSortedArray, const Standard_Integer theInitFlag);
  

  //! build set of ranges which consists of one range with
  //! boundary values theFirstBoundary and theLastBoundary
  Standard_EXPORT void SetBoundaries (const Standard_Real theFirstBoundary, const Standard_Real theLastBoundary, const Standard_Integer theInitFlag);
  

  //! Build set of ranges based on the array of progressive sorted values
  //!
  //! Warning:
  //! The function do not check if the values of array are not sorted
  //! It should be checked before function invocation
  Standard_EXPORT void SetRanges (const TColStd_Array1OfReal& theSortedArray, const Standard_Integer theInitFlag);
  

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT Standard_Boolean InsertRange (const Standard_Real theFirstBoundary, const Standard_Real theLastBoundary, const Standard_Integer theFlag);
  

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT Standard_Boolean InsertRange (const IntTools_Range& theRange, const Standard_Integer theFlag);
  

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! The index theIndex is a position where the range will be inserted.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT Standard_Boolean InsertRange (const Standard_Real theFirstBoundary, const Standard_Real theLastBoundary, const Standard_Integer theFlag, const Standard_Integer theIndex);
  

  //! Inserts a new range marked with flag theFlag
  //! It replace the existing ranges or parts of ranges
  //! and their flags.
  //! The index theIndex is a position where the range will be inserted.
  //! Returns True if the range is inside the initial boundaries,
  //! otherwise or in case of some error returns False
  Standard_EXPORT Standard_Boolean InsertRange (const IntTools_Range& theRange, const Standard_Integer theFlag, const Standard_Integer theIndex);
  

  //! Set flag theFlag for range with index theIndex
  Standard_EXPORT void SetFlag (const Standard_Integer theIndex, const Standard_Integer theFlag);
  

  //! Returns flag of the range with index theIndex
  Standard_EXPORT Standard_Integer Flag (const Standard_Integer theIndex) const;
  

  //! Returns index of range which contains theValue.
  //! If theValue do not belong any range returns 0.
  Standard_EXPORT Standard_Integer GetIndex (const Standard_Real theValue) const;
  
  Standard_EXPORT const TColStd_SequenceOfInteger& GetIndices (const Standard_Real theValue);
  

  //! Returns index of range which contains theValue
  //! If theValue do not belong any range returns 0.
  //! If UseLower is Standard_True then lower boundary of the range
  //! can be equal to theValue, otherwise upper boundary of the range
  //! can be equal to theValue.
  Standard_EXPORT Standard_Integer GetIndex (const Standard_Real theValue, const Standard_Boolean UseLower) const;

  //! Returns number of ranges
  Standard_Integer Length() const { return myRangeNumber; }

  //! Returns the range with index theIndex.
  //! the Index can be from 1 to Length()
  Standard_EXPORT IntTools_Range Range (const Standard_Integer theIndex) const;

private:

  TColStd_SequenceOfReal myRangeSetStorer;
  Standard_Integer myRangeNumber;
  TColStd_SequenceOfInteger myFlags;
  TColStd_SequenceOfInteger myFoundIndices;

};

#endif // _IntTools_MarkedRangeSet_HeaderFile
