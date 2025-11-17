// Copyright (c) 1998-1999 Matra Datavision
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

// -------------------------------------------------------------
// C matra datavision 1993
// Updated :
// -------------------------------------------------------------

#include <Quantity_Date.hxx>
#include <Quantity_DateDefinitionError.hxx>
#include <Quantity_Period.hxx>
#include <Standard_OutOfRange.hxx>

#include "Quantity_TimeConstants.pxx"

namespace
{

static constexpr int month_table[12] = {31,  // January
                                        28,  // February
                                        31,  // March
                                        30,  // April
                                        31,  // May
                                        30,  // June
                                        31,  // July
                                        31,  // August
                                        30,  // September
                                        31,  // October
                                        30,  // November
                                        31}; // December

static constexpr int SecondsByYear     = 365 * SECONDS_PER_DAY; // Normal Year
static constexpr int SecondsByLeapYear = 366 * SECONDS_PER_DAY; // Leap Year

// Returns the number of days in a month for a given year (handles leap years)
constexpr Standard_Integer getDaysInMonth(const Standard_Integer theMonth,
                                          const Standard_Integer theYear) noexcept
{
  if (theMonth == 2)
  {
    return Quantity_Date::IsLeap(theYear) ? 29 : 28;
  }
  return month_table[theMonth - 1];
}
} // anonymous namespace

// -----------------------------------------
// Initialize a date to January,1 1979 00:00
// -----------------------------------------

Quantity_Date::Quantity_Date()
    : mySec(0),
      myUSec(0)
{
}

// -----------------------------------------------------------
// IsValid : Checks the validity of a date
// This is the complete way month, day, year, ... micro second
// -----------------------------------------------------------

Standard_Boolean Quantity_Date::IsValid(const Standard_Integer mm,
                                        const Standard_Integer dd,
                                        const Standard_Integer yy,
                                        const Standard_Integer hh,
                                        const Standard_Integer mn,
                                        const Standard_Integer ss,
                                        const Standard_Integer mis,
                                        const Standard_Integer mics)
{

  if (mm < 1 || mm > 12)
    return Standard_False;

  if (yy < 1979)
    return Standard_False;

  if (dd < 1 || dd > getDaysInMonth(mm, yy))
    return Standard_False;

  if (hh < 0 || hh > 23)
    return Standard_False;

  if (mn < 0 || mn > 59)
    return Standard_False;

  if (ss < 0 || ss > 59)
    return Standard_False;

  if (mis < 0 || mis > 999)
    return Standard_False;

  if (mics < 0 || mics > 999)
    return Standard_False;

  return Standard_True;
}

// -----------------------------------------------------------
// Initialize a Date :
// This is the complete way month, day, year, ... micro second
// -----------------------------------------------------------

Quantity_Date::Quantity_Date(const Standard_Integer mm,
                             const Standard_Integer dd,
                             const Standard_Integer yy,
                             const Standard_Integer hh,
                             const Standard_Integer mn,
                             const Standard_Integer ss,
                             const Standard_Integer mis,
                             const Standard_Integer mics)
    : mySec(0),
      myUSec(0)
{

  SetValues(mm, dd, yy, hh, mn, ss, mis, mics);
}

// ------------------------------------------------------------
// Set values of a Date :
// This is the complete way month, day, year, ... micro second
// ------------------------------------------------------------

void Quantity_Date::SetValues(const Standard_Integer mm,
                              const Standard_Integer dd,
                              const Standard_Integer yy,
                              const Standard_Integer hh,
                              const Standard_Integer mn,
                              const Standard_Integer ss,
                              const Standard_Integer mis,
                              const Standard_Integer mics)
{

  Standard_Integer i;

  if (!Quantity_Date::IsValid(mm, dd, yy, hh, mn, ss, mis, mics))
    throw Quantity_DateDefinitionError("Quantity_Date::Quantity_Date invalid parameters");

  mySec  = 0;
  myUSec = 0;
  for (i = 1979; i < yy; i++)
  {
    if (!Quantity_Date::IsLeap(i))
      mySec += SecondsByYear;
    else
      mySec += SecondsByLeapYear;
  }

  for (i = 1; i < mm; i++)
  {
    mySec += getDaysInMonth(i, yy) * SECONDS_PER_DAY;
  }

  mySec += SECONDS_PER_DAY * (dd - 1);

  mySec += SECONDS_PER_HOUR * hh;

  mySec += SECONDS_PER_MINUTE * mn;

  mySec += ss;

  myUSec += mis * USECS_PER_MSEC;

  myUSec += mics;
}

// ---------------------------------------------
// Values : Returns the values of a date
// ~~~~~~
// ---------------------------------------------

void Quantity_Date::Values(Standard_Integer& mm,
                           Standard_Integer& dd,
                           Standard_Integer& yy,
                           Standard_Integer& hh,
                           Standard_Integer& mn,
                           Standard_Integer& ss,
                           Standard_Integer& mis,
                           Standard_Integer& mics) const
{

  Standard_Integer carry;

  for (yy = 1979, carry = mySec;; yy++)
  {
    if (!Quantity_Date::IsLeap(yy))
    {
      if (carry >= SecondsByYear)
        carry -= SecondsByYear;
      else
        break;
    }
    else
    {
      if (carry >= SecondsByLeapYear)
        carry -= SecondsByLeapYear;
      else
        break;
    }
  }

  for (mm = 1;; mm++)
  {
    Standard_Integer i = getDaysInMonth(mm, yy) * SECONDS_PER_DAY;
    if (carry >= i)
      carry -= i;
    else
      break;
  }

  // Extract day within the month
  // carry holds seconds since the beginning of the current month
  dd = carry / SECONDS_PER_DAY + 1;    // Convert 0-based to 1-based day
  carry -= (dd - 1) * SECONDS_PER_DAY; // Remove day component from carry

  extractTimeComponents(carry, hh, mn, ss);
  extractMillisAndMicros(myUSec, mis, mics);
}

// ---------------------------------------------------------------------
// Difference : Subtract a date to a given date; the result is a period
// ~~~~~~~~~~   of time
// ---------------------------------------------------------------------

Quantity_Period Quantity_Date::Difference(const Quantity_Date& OtherDate)
{
  Standard_Integer i1, i2;

  // Special case: if this date is the epoch (Jan 1, 1979 00:00),
  // return OtherDate as a period (time elapsed since epoch)
  if (mySec == 0 && myUSec == 0)
  {
    i1 = OtherDate.mySec;
    i2 = OtherDate.myUSec;
  }
  else
  {
    i1 = mySec - OtherDate.mySec;
    i2 = myUSec - OtherDate.myUSec;
  }

  // Normalize: handle microsecond underflow
  normalizeSubtractionBorrow(i1, i2);

  // Period is always absolute value, convert negative result
  if (i1 < 0)
  {
    i1 = -i1;
    if (i2 > 0)
    {
      i1--;
      i2 = USECS_PER_SEC - i2;
    }
  }

  Quantity_Period result(i1, i2);
  return (result);
}

// ------------------------------------------------------------------
// Subtract : subtracts a period to a date and returns a date.
// ~~~~~~~~
// ------------------------------------------------------------------

Quantity_Date Quantity_Date::Subtract(const Quantity_Period& During)
{

  Standard_Integer ss, mics;
  Quantity_Date    result;
  result.mySec  = mySec;
  result.myUSec = myUSec;
  During.Values(ss, mics);

  result.mySec -= ss;
  result.myUSec -= mics;

  normalizeSubtractionBorrow(result.mySec, result.myUSec);

  if (result.mySec < 0)
    throw Quantity_DateDefinitionError(
      "Quantity_Date::Subtract : The result date is anterior to Jan,1 1979");

  return (result);
}

// ----------------------------------------------------------------------
// Add : Adds a period of time to a date
// ~~~
// ----------------------------------------------------------------------
Quantity_Date Quantity_Date::Add(const Quantity_Period& During)
{

  Quantity_Date result;
  During.Values(result.mySec, result.myUSec);
  result.mySec += mySec;
  result.myUSec += myUSec;
  normalizeAdditionOverflow(result.mySec, result.myUSec);
  return (result);
}

// ----------------------------------------------------------------------
// Year : Return the year of a date
// ~~~~
// ----------------------------------------------------------------------
Standard_Integer Quantity_Date::Year()
{
  Standard_Integer mm, dd, year, hh, mn, ss, mis, mics;
  Values(mm, dd, year, hh, mn, ss, mis, mics);
  return (year);
}

// ----------------------------------------------------------------------
// Month : Return the month of a date
// ~~~~~
// ----------------------------------------------------------------------
Standard_Integer Quantity_Date::Month()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (mm);
}

// ----------------------------------------------------------------------
// Day : Return the day of a date
// ~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Day()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (dd);
}

// ----------------------------------------------------------------------
// hour : Return the hour of a date
// ~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Hour()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (hh);
}

// ----------------------------------------------------------------------
// Minute : Return the minute of a date
// ~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Minute()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (mn);
}

// ----------------------------------------------------------------------
// Second : Return the second of a date
// ~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Second()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (ss);
}

// ----------------------------------------------------------------------
// millisecond : Return the millisecond of a date
// ~~~~~~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::MilliSecond()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (mis);
}

// ----------------------------------------------------------------------
// Day : Return the day of a date
// ~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::MicroSecond()
{
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  Values(mm, dd, yy, hh, mn, ss, mis, mics);
  return (mics);
}
