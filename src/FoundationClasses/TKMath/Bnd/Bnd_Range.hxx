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

#include <Standard_ConstructionError.hxx>

#include <NCollection_List.hxx>

//! This class describes a range in 1D space restricted
//! by two real values.
//! A range can be void indicating there is no point included in the range.
class Bnd_Range
{
public:
  //! Default constructor. Creates VOID range.
  Bnd_Range()
      : myFirst(0.0),
        myLast(-1.0)
  {
  }

  //! Constructor. Never creates VOID range.
  Bnd_Range(const double theMin, const double theMax)
      : myFirst(theMin),
        myLast(theMax)
  {
    if (myLast < myFirst)
      throw Standard_ConstructionError("Last < First");
  }

  //! Replaces <this> with common-part of <this> and theOther
  Standard_EXPORT void Common(const Bnd_Range& theOther);

  //! Joins *this and theOther to one interval.
  //! Replaces *this to the result.
  //! Returns false if the operation cannot be done (e.g.
  //! input arguments are empty or separated).
  //! @sa use method ::Add() to merge two ranges unconditionally
  Standard_EXPORT bool Union(const Bnd_Range& theOther);

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
  Standard_EXPORT void Split(const double                 theVal,
                             NCollection_List<Bnd_Range>& theList,
                             const double                 thePeriod = 0.0) const;

  //! Checks if <this> intersects values like
  //!   theVal+k*thePeriod, where k is an integer number (k = 0, +/-1, +/-2, ...).
  //! Returns:
  //!     0 - if <this> does not intersect the theVal+k*thePeriod.
  //!     1 - if <this> intersects theVal+k*thePeriod.
  //!     2 - if myFirst or/and myLast are equal to theVal+k*thePeriod.
  //!
  //! ATTENTION!!!
  //!  If (myFirst == myLast) then this function will return only either 0 or 2.
  Standard_EXPORT int IsIntersected(const double theVal, const double thePeriod = 0.0) const;

  //! Extends <this> to include theParameter
  void Add(const double theParameter)
  {
    if (IsVoid())
    {
      myFirst = myLast = theParameter;
      return;
    }

    myFirst = (std::min)(myFirst, theParameter);
    myLast  = (std::max)(myLast, theParameter);
  }

  //! Extends this range to include both ranges.
  //! @sa use method ::Union() to check if two ranges overlap method merging
  void Add(const Bnd_Range& theRange)
  {
    if (theRange.IsVoid())
    {
      return;
    }
    else if (IsVoid())
    {
      *this = theRange;
    }
    myFirst = (std::min)(myFirst, theRange.myFirst);
    myLast  = (std::max)(myLast, theRange.myLast);
  }

  //! Obtain MIN boundary of <this>.
  //! If <this> is VOID the method returns false.
  bool GetMin(double& thePar) const
  {
    if (IsVoid())
    {
      return false;
    }

    thePar = myFirst;
    return true;
  }

  //! Obtain MAX boundary of <this>.
  //! If <this> is VOID the method returns false.
  bool GetMax(double& thePar) const
  {
    if (IsVoid())
    {
      return false;
    }

    thePar = myLast;
    return true;
  }

  //! Obtain first and last boundary of <this>.
  //! If <this> is VOID the method returns false.
  bool GetBounds(double& theFirstPar, double& theLastPar) const
  {
    if (IsVoid())
    {
      return false;
    }

    theFirstPar = myFirst;
    theLastPar  = myLast;
    return true;
  }

  //! Obtain theParameter satisfied to the equation
  //!     (theParameter-MIN)/(MAX-MIN) == theLambda.
  //!   *  theLambda == 0 --> MIN boundary will be returned;
  //!   *  theLambda == 0.5 --> Middle point will be returned;
  //!   *  theLambda == 1 --> MAX boundary will be returned;
  //!   *  theLambda < 0 --> the value less than MIN will be returned;
  //!   *  theLambda > 1 --> the value greater than MAX will be returned.
  //! If <this> is VOID the method returns false.
  bool GetIntermediatePoint(const double theLambda, double& theParameter) const
  {
    if (IsVoid())
    {
      return false;
    }

    theParameter = myFirst + theLambda * (myLast - myFirst);
    return true;
  }

  //! Returns range value (MAX-MIN). Returns negative value for VOID range.
  double Delta() const { return (myLast - myFirst); }

  //! Is <this> initialized.
  bool IsVoid() const { return (myLast < myFirst); }

  //! Initializes <this> by default parameters. Makes <this> VOID.
  void SetVoid()
  {
    myLast  = -1.0;
    myFirst = 0.0;
  }

  //! Extends this to the given value (in both side)
  void Enlarge(const double theDelta)
  {
    if (IsVoid())
    {
      return;
    }

    myFirst -= theDelta;
    myLast += theDelta;
  }

  //! Returns the copy of <*this> shifted by theVal
  Bnd_Range Shifted(const double theVal) const
  {
    return !IsVoid() ? Bnd_Range(myFirst + theVal, myLast + theVal) : Bnd_Range();
  }

  //! Shifts <*this> by theVal
  void Shift(const double theVal)
  {
    if (!IsVoid())
    {
      myFirst += theVal;
      myLast += theVal;
    }
  }

  //! Trims the First value in range by the given lower limit.
  //! Marks range as Void if the given Lower value is greater than range Max.
  void TrimFrom(const double theValLower)
  {
    if (!IsVoid())
    {
      myFirst = (std::max)(myFirst, theValLower);
    }
  }

  //! Trim the Last value in range by the given Upper limit.
  //! Marks range as Void if the given Upper value is smaller than range Max.
  void TrimTo(const double theValUpper)
  {
    if (!IsVoid())
    {
      myLast = (std::min)(myLast, theValUpper);
    }
  }

  //! Returns True if the value is out of this range.
  bool IsOut(double theValue) const { return IsVoid() || theValue < myFirst || theValue > myLast; }

  //! Returns True if the given range is out of this range.
  bool IsOut(const Bnd_Range& theRange) const
  {
    return IsVoid() || theRange.IsVoid() || theRange.myLast < myFirst || theRange.myFirst > myLast;
  }

  //! Returns TRUE if theOther is equal to <*this>
  bool operator==(const Bnd_Range& theOther) const
  {
    return ((myFirst == theOther.myFirst) && (myLast == theOther.myLast));
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  double myFirst; //!< Start of range
  double myLast;  //!< End   of range
};

#endif
