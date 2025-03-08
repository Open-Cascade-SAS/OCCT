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

#ifndef _Quantity_Period_HeaderFile
#define _Quantity_Period_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>

//! Manages date intervals. For example, a Period object
//! gives the interval between two dates.
//! A period is expressed in seconds and microseconds.
class Quantity_Period
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a Period
  //! With:      0 <= dd
  //! 0 <= hh
  //! 0 <= mn
  //! 0 <= ss
  //! 0 <= mis
  //! 0 <= mics
  Standard_EXPORT Quantity_Period(const Standard_Integer dd,
                                  const Standard_Integer hh,
                                  const Standard_Integer mn,
                                  const Standard_Integer ss,
                                  const Standard_Integer mis  = 0,
                                  const Standard_Integer mics = 0);

  //! Creates a Period with a number of seconds and microseconds.
  //! Exceptions
  //! Quantity_PeriodDefinitionError:
  //! -   if the number of seconds expressed either by:
  //! -   dd days, hh hours, mn minutes and ss seconds, or
  //! -   Ss
  //! is less than 0.
  //! -   if the number of microseconds expressed either by:
  //! -   mis milliseconds and mics microseconds, or
  //! -   Mics
  //! is less than 0.
  Standard_EXPORT Quantity_Period(const Standard_Integer ss, const Standard_Integer mics = 0);

  //! Decomposes this period into a number of days,hours,
  //! minutes,seconds,milliseconds and microseconds
  //! Example of return values:
  //! 2 days, 15 hours, 0 minute , 0 second
  //! 0 millisecond and 0 microsecond
  Standard_EXPORT void Values(Standard_Integer& dd,
                              Standard_Integer& hh,
                              Standard_Integer& mn,
                              Standard_Integer& ss,
                              Standard_Integer& mis,
                              Standard_Integer& mics) const;

  //! Returns the number of seconds in Ss and the
  //! number of remainding microseconds in Mics of this period.
  //! Example of return values: 3600 seconds and 0 microseconds
  Standard_EXPORT void Values(Standard_Integer& ss, Standard_Integer& mics) const;

  //! Assigns to this period the time interval defined
  //! -   with dd days, hh hours, mn minutes, ss
  //! seconds, mis (defaulted to 0) milliseconds and
  //! mics (defaulted to 0) microseconds; or
  Standard_EXPORT void SetValues(const Standard_Integer dd,
                                 const Standard_Integer hh,
                                 const Standard_Integer mn,
                                 const Standard_Integer ss,
                                 const Standard_Integer mis  = 0,
                                 const Standard_Integer mics = 0);

  //! Assigns to this period the time interval defined
  //! -   with Ss seconds and Mics (defaulted to 0) microseconds.
  //! Exceptions
  //! Quantity_PeriodDefinitionError:
  //! -   if the number of seconds expressed either by:
  //! -   dd days, hh hours, mn minutes and ss seconds, or
  //! -   Ss
  //! is less than 0.
  //! -   if the number of microseconds expressed either by:
  //! -   mis milliseconds and mics microseconds, or
  //! -   Mics
  //! is less than 0.
  Standard_EXPORT void SetValues(const Standard_Integer ss, const Standard_Integer mics = 0);

  //! Subtracts one Period from another and returns the difference.
  Standard_EXPORT Quantity_Period Subtract(const Quantity_Period& anOther) const;

  Quantity_Period operator-(const Quantity_Period& anOther) const { return Subtract(anOther); }

  //! Adds one Period to another one.
  Standard_EXPORT Quantity_Period Add(const Quantity_Period& anOther) const;

  Quantity_Period operator+(const Quantity_Period& anOther) const { return Add(anOther); }

  //! Returns TRUE if both <me> and <other> are equal.
  Standard_EXPORT Standard_Boolean IsEqual(const Quantity_Period& anOther) const;

  Standard_Boolean operator==(const Quantity_Period& anOther) const { return IsEqual(anOther); }

  //! Returns TRUE if <me> is shorter than <other>.
  Standard_EXPORT Standard_Boolean IsShorter(const Quantity_Period& anOther) const;

  Standard_Boolean operator<(const Quantity_Period& anOther) const { return IsShorter(anOther); }

  //! Returns TRUE if <me> is longer then <other>.
  Standard_EXPORT Standard_Boolean IsLonger(const Quantity_Period& anOther) const;

  Standard_Boolean operator>(const Quantity_Period& anOther) const { return IsLonger(anOther); }

  //! Checks the validity of a Period in form (dd,hh,mn,ss,mil,mic)
  //! With:      0 <= dd
  //! 0 <= hh
  //! 0 <= mn
  //! 0 <= ss
  //! 0 <= mis
  //! 0 <= mics
  Standard_EXPORT static Standard_Boolean IsValid(const Standard_Integer dd,
                                                  const Standard_Integer hh,
                                                  const Standard_Integer mn,
                                                  const Standard_Integer ss,
                                                  const Standard_Integer mis  = 0,
                                                  const Standard_Integer mics = 0);

  //! Checks the validity of a Period in form (ss,mic)
  //! With:      0 <= ss
  //! 0 <= mics
  Standard_EXPORT static Standard_Boolean IsValid(const Standard_Integer ss,
                                                  const Standard_Integer mics = 0);

private:
  Standard_Integer mySec;
  Standard_Integer myUSec;
};

#endif // _Quantity_Period_HeaderFile
