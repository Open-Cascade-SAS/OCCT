// Created on: 1993-01-04
// Created by: J.P. BOUDIER - J.P. TIRAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Quantity_Date_HeaderFile
#define _Quantity_Date_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Quantity_DateDefinitionError;
class Standard_OutOfRange;
class Quantity_Period;

//! This class provides services to manage date information.
//! A date represents the following time intervals:
//! year, month, day, hour, minute, second,
//! millisecond and microsecond.
//! Current time is expressed in elapsed seconds
//! and microseconds beginning from 00:00 GMT,
//! January 1, 1979 (zero hour). The valid date can
//! only be later than this one.
//! Note: a Period object gives the interval between two dates.
class Quantity_Date 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructs a default date
  //! (00:00 GMT, January 1, 1979 (zero hour)); use the function
  //! SetValues to define the required date; or
  Standard_EXPORT Quantity_Date();
  
  //! Constructs a date from the year yyyy, the
  //! month mm, the day dd, the hour hh, the minute
  //! mn, the second ss, the millisecond mis
  //! (defaulted to 0) and the microsecond mics (defaulted to 0)..
  //! With:      1 <= mm <= 12
  //! 1 <= dd <= max number of days of <mm>
  //! 1979 <= yyyy
  //! 0 <= hh <= 23
  //! 0 <= mn <= 59
  //! 0 <= ss <= 59
  //! 0 <= mis <= 999
  //! 0 <= mics <= 999
  //! Exceptions
  //! Quantity_DateDefinitionError if mm, dd, hh,
  //! mn, ss, mis and mics are not the components of the valid date.
  Standard_EXPORT Quantity_Date(const Standard_Integer mm, const Standard_Integer dd, const Standard_Integer yyyy, const Standard_Integer hh, const Standard_Integer mn, const Standard_Integer ss, const Standard_Integer mis = 0, const Standard_Integer mics = 0);
  
  //! Gets a complete Date.
  //! -   in mm - the month,
  //! -   in dd - the day,
  //! -   in yyyy - the year,
  //! -   in hh - the hour,
  //! -   in mn - the minute,
  //! -   in ss - the second,
  //! -   in mis - the millisecond, and
  //! -   in mics - the microsecond
  Standard_EXPORT void Values (Standard_Integer& mm, Standard_Integer& dd, Standard_Integer& yy, Standard_Integer& hh, Standard_Integer& mn, Standard_Integer& ss, Standard_Integer& mis, Standard_Integer& mics) const;
  
  //! Assigns to this date the year yyyy, the month
  //! mm, the day dd, the hour hh, the minute mn, the
  //! second ss, the millisecond mis (defaulted to 0)
  //! and the microsecond mics (defaulted to 0).
  //! Exceptions
  //! Quantity_DateDefinitionError if mm, dd, hh,
  //! mn, ss, mis and mics are not components of a valid date.
  Standard_EXPORT void SetValues (const Standard_Integer mm, const Standard_Integer dd, const Standard_Integer yy, const Standard_Integer hh, const Standard_Integer mn, const Standard_Integer ss, const Standard_Integer mis = 0, const Standard_Integer mics = 0);
  
  //! Subtracts one Date from another one to find the period
  //! between and returns the value.
  //! The result is the absolute value between the difference
  //! of two dates.
  Standard_EXPORT Quantity_Period Difference (const Quantity_Date& anOther);
  
  //! Subtracts a period from a Date and returns the new Date.
  //! Raises an exception if the result date is anterior to
  //! Jan 1, 1979.
  Standard_EXPORT Quantity_Date Subtract (const Quantity_Period& aPeriod);
Quantity_Date operator - (const Quantity_Period& aPeriod)
{
  return Subtract(aPeriod);
}
  
  //! Adds a Period to a Date and returns the new Date.
  Standard_EXPORT Quantity_Date Add (const Quantity_Period& aPeriod);
Quantity_Date operator + (const Quantity_Period& aPeriod)
{
  return Add(aPeriod);
}
  
  //! Returns year of a Date.
  Standard_EXPORT Standard_Integer Year();
  
  //! Returns month of a Date.
  Standard_EXPORT Standard_Integer Month();
  
  //! Returns Day of a Date.
  Standard_EXPORT Standard_Integer Day();
  
  //! Returns Hour of a Date.
  Standard_EXPORT Standard_Integer Hour();
  
  //! Returns minute of a Date.
  Standard_EXPORT Standard_Integer Minute();
  
  //! Returns seconde of a Date.
  Standard_EXPORT Standard_Integer Second();
  
  //! Returns millisecond of a Date.
  Standard_EXPORT Standard_Integer MilliSecond();
  
  //! Returns microsecond of a Date.
  Standard_EXPORT Standard_Integer MicroSecond();
  
  //! Returns TRUE if both <me> and <other> are equal.
  //! This method is an alias of operator ==.
  Standard_EXPORT Standard_Boolean IsEqual (const Quantity_Date& anOther) const;
Standard_Boolean operator == (const Quantity_Date& anOther) const
{
  return IsEqual(anOther);
}
  
  //! Returns TRUE if <me> is earlier than <other>.
  Standard_EXPORT Standard_Boolean IsEarlier (const Quantity_Date& anOther) const;
Standard_Boolean operator < (const Quantity_Date& anOther) const
{
  return IsEarlier(anOther);
}
  
  //! Returns TRUE if <me> is later then <other>.
  Standard_EXPORT Standard_Boolean IsLater (const Quantity_Date& anOther) const;
Standard_Boolean operator > (const Quantity_Date& anOther) const
{
  return IsLater(anOther);
}
  
  //! Checks the validity of a date - returns true if a
  //! date defined from the year yyyy, the month mm,
  //! the day dd, the hour hh, the minute mn, the
  //! second ss, the millisecond mis (defaulted to 0)
  //! and the microsecond mics (defaulted to 0) is valid.
  //! A date must satisfy the conditions above:
  //! -   yyyy is greater than or equal to 1979,
  //! -   mm lies within the range [1, 12] (with 1
  //! corresponding to January and 12 to December),
  //! -   dd lies within a valid range for the month mm
  //! (from 1 to 28, 29, 30 or 31 depending on
  //! mm and whether yyyy is a leap year or not),
  //! -   hh lies within the range [0, 23],
  //! -   mn lies within the range [0, 59],
  //! -   ss lies within the range [0, 59],
  //! -   mis lies within the range [0, 999],
  //! -   mics lies within the range [0, 999].C
  Standard_EXPORT static Standard_Boolean IsValid (const Standard_Integer mm, const Standard_Integer dd, const Standard_Integer yy, const Standard_Integer hh, const Standard_Integer mn, const Standard_Integer ss, const Standard_Integer mis = 0, const Standard_Integer mics = 0);
  
  //! Returns true if a year is a leap year.
  //! The leap years are divisable by 4 and not by 100 except
  //! the years divisable by 400.
  static Standard_Boolean IsLeap (const Standard_Integer yy)
  {
    return ((yy % 4 == 0) && (yy % 100 != 0))
         || (yy % 400) == 0;
  }

private:

  Standard_Integer mySec;
  Standard_Integer myUSec;

};

#endif // _Quantity_Date_HeaderFile
