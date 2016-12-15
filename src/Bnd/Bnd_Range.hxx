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

#ifndef _Bnd_Range_HeaderFile
#define _Bnd_Range_HeaderFile

#include <Standard_Real.hxx>
#include <Standard_ConstructionError.hxx>

#include <NCollection_List.hxx>

//! This class describes a range in 1D space restricted
//! by two real values.
//! A range can be void indicating there is no point included in the range.

class Bnd_Range
{
public:

  //! Default constructor. Creates VOID range.
  Bnd_Range() : myFirst(0.0), myLast(-1.0)
  {
  };

  //! Constructor. Never creates VOID range.
  Bnd_Range(const Standard_Real theMin, const Standard_Real theMax) : 
                                                    myFirst(theMin), myLast(theMax)
  {
    if(myLast < myFirst)
      throw Standard_ConstructionError("Last < First");
  };

  //! Replaces <this> with common-part of <this> and theOther
  Standard_EXPORT void Common(const Bnd_Range& theOther);
  
  //! Joins *this and theOther to one interval.
  //! Replaces *this to the result.
  //! Returns false if the operation cannot be done (e.g.
  //! input arguments are empty or separated).
  Standard_EXPORT Standard_Boolean Union(const Bnd_Range& theOther);

  //! Splits <this> to several sub-ranges by theVal value
  //! (e.g. range [3, 15] will be split by theVal==5 to the two
  //! ranges: [3, 5] and [5, 15]). New ranges will be pushed to
  //! theList (theList must be initialized correctly before
  //! calling this method).
  //! If thePeriod != 0.0 then at least one boundary of
  //! new ranges (if <*this> intersects theVal+k*thePeriod) will be equal to
  //! theVal+thePeriod*k, where k is an integer number (k = 0, +/-1, +/-2, ...).
  //! (let thePeriod in above example be 4 ==> we will obtain
  //! four ranges: [3, 5], [5, 9], [9, 13] and [13, 15].
  Standard_EXPORT void Split(const Standard_Real theVal,
                             NCollection_List<Bnd_Range>& theList,
                             const Standard_Real thePeriod = 0.0) const;

  //! Checks if <this> intersects values like
  //!   theVal+k*thePeriod, where k is an integer number (k = 0, +/-1, +/-2, ...).
  //! Returns:
  //!     0 - if <this> does not intersect the theVal+k*thePeriod.
  //!     1 - if <this> intersects theVal+k*thePeriod.
  //!     2 - if myFirst or/and myLast are equal to theVal+k*thePeriod.
  //!
  //! ATTENTION!!!
  //!  If (myFirst == myLast) then this function will return only either 0 or 2.
  Standard_EXPORT Standard_Integer
                      IsIntersected(const Standard_Real theVal,
                                    const Standard_Real thePeriod = 0.0) const;

  //! Extends <this> to include theParameter
  void Add(const Standard_Real theParameter)
  {
    if(IsVoid())
    {
      myFirst = myLast = theParameter;
      return;
    }

    myFirst = Min(myFirst, theParameter);
    myLast = Max(myLast, theParameter);
  }

  //! Obtain MIN boundary of <this>.
  //! If <this> is VOID the method returns false.
  Standard_Boolean GetMin(Standard_Real& thePar) const
  {
    if(IsVoid())
    {
      return Standard_False;
    }

    thePar = myFirst;
    return Standard_True;
  }

  //! Obtain MAX boundary of <this>.
  //! If <this> is VOID the method returns false.
  Standard_Boolean GetMAX(Standard_Real& thePar) const
  {
    if(IsVoid())
    {
      return Standard_False;
    }

    thePar = myLast;
    return Standard_True;
  }

  //! Obtain first and last boundary of <this>.
  //! If <this> is VOID the method returns false.
  Standard_Boolean GetBounds(Standard_Real& theFirstPar,
                             Standard_Real& theLastPar) const
  {
    if(IsVoid())
    {
      return Standard_False;
    }

    theFirstPar = myFirst;
    theLastPar = myLast;
    return Standard_True;
  }
  
  //! Returns range value (MAX-MIN). Returns negative value for VOID range.
  Standard_Real Delta() const
  {
    return (myLast - myFirst);
  }

  //! Is <this> initialized.
  Standard_Boolean IsVoid() const
  {
    return (myLast < myFirst);
  }

  //! Initializes <this> by default parameters. Makes <this> VOID.
  void SetVoid()
  {
    myLast = -1.0;
    myFirst = 0.0;
  }

  //! Extends this to the given value (in both side)
  void Enlarge(const Standard_Real theDelta)
  {
    if (IsVoid())
    {
      return;
    }

    myFirst -= theDelta;
    myLast += theDelta;
  }

  //! Returns the copy of <*this> shifted by theVal
  Bnd_Range Shifted(const Standard_Real theVal) const
  {
    return Bnd_Range(myFirst + theVal, myLast + theVal);
  }

  //! Shifts <*this> by theVal
  void Shift(const Standard_Real theVal)
  {
    myFirst += theVal;
    myLast += theVal;
  }

  //! Returns TRUE if theOther is equal to <*this>
  Standard_Boolean operator==(const Bnd_Range& theOther) const
  {
    return ((myFirst == theOther.myFirst) && (myLast == theOther.myLast));
  }

private:
  //! Start of range
  Standard_Real myFirst;

  //! End of range
  Standard_Real myLast;
};

#endif