// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <Quantity_Date.hxx>
#include <Quantity_Period.hxx>

#include <gtest/gtest.h>

// Test fixture for Quantity_Date tests
class Quantity_DateTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test basic construction and default initialization
TEST_F(Quantity_DateTest, BasicConstruction)
{
  // Default constructor creates January 1, 1979, 00:00:00
  Quantity_Date    aDate1;
  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aDate1.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);    // January
  EXPECT_EQ(1, dd);    // 1st
  EXPECT_EQ(1979, yy); // 1979
  EXPECT_EQ(0, hh);    // 00 hours
  EXPECT_EQ(0, mn);    // 00 minutes
  EXPECT_EQ(0, ss);    // 00 seconds

  // Parameterized constructor
  Quantity_Date aDate2(6, 15, 2025, 14, 30, 45, 123, 456);
  aDate2.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(6, mm);
  EXPECT_EQ(15, dd);
  EXPECT_EQ(2025, yy);
  EXPECT_EQ(14, hh);
  EXPECT_EQ(30, mn);
  EXPECT_EQ(45, ss);
  EXPECT_EQ(123, mis);
  EXPECT_EQ(456, mics);
}

// Test constexpr comparison operators
TEST_F(Quantity_DateTest, ConstexprComparisons)
{
  Quantity_Date aDate1(1, 1, 2020, 0, 0, 0, 0, 0);
  Quantity_Date aDate2(1, 1, 2020, 0, 0, 0, 0, 0);
  Quantity_Date aDate3(1, 2, 2020, 0, 0, 0, 0, 0);

  // Test IsEqual (constexpr)
  EXPECT_TRUE(aDate1.IsEqual(aDate2));
  EXPECT_FALSE(aDate1.IsEqual(aDate3));

  // Test IsEarlier (constexpr)
  EXPECT_TRUE(aDate1.IsEarlier(aDate3));
  EXPECT_FALSE(aDate3.IsEarlier(aDate1));
  EXPECT_FALSE(aDate1.IsEarlier(aDate2));

  // Test IsLater (constexpr)
  EXPECT_TRUE(aDate3.IsLater(aDate1));
  EXPECT_FALSE(aDate1.IsLater(aDate3));
  EXPECT_FALSE(aDate1.IsLater(aDate2));
}

// Test leap year detection (constexpr)
TEST_F(Quantity_DateTest, LeapYearDetection)
{
  // Leap years
  EXPECT_TRUE(Quantity_Date::IsLeap(2000)); // Divisible by 400
  EXPECT_TRUE(Quantity_Date::IsLeap(2004)); // Divisible by 4, not by 100
  EXPECT_TRUE(Quantity_Date::IsLeap(2020));

  // Non-leap years
  EXPECT_FALSE(Quantity_Date::IsLeap(1900)); // Divisible by 100 but not 400
  EXPECT_FALSE(Quantity_Date::IsLeap(2001)); // Not divisible by 4
  EXPECT_FALSE(Quantity_Date::IsLeap(2100)); // Divisible by 100 but not 400
}

// Test validation with leap years (thread-safety regression test)
TEST_F(Quantity_DateTest, ValidationWithLeapYear)
{
  // Valid date in leap year (February 29)
  EXPECT_TRUE(Quantity_Date::IsValid(2, 29, 2020, 0, 0, 0, 0, 0));

  // Invalid date in non-leap year (February 29)
  EXPECT_FALSE(Quantity_Date::IsValid(2, 29, 2019, 0, 0, 0, 0, 0));

  // Valid February 28 in any year
  EXPECT_TRUE(Quantity_Date::IsValid(2, 28, 2019, 0, 0, 0, 0, 0));
  EXPECT_TRUE(Quantity_Date::IsValid(2, 28, 2020, 0, 0, 0, 0, 0));
}

// Test month boundary validation (uses constexpr getDaysInMonth)
TEST_F(Quantity_DateTest, MonthBoundaries)
{
  // January has 31 days
  EXPECT_TRUE(Quantity_Date::IsValid(1, 31, 2020, 0, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 32, 2020, 0, 0, 0, 0, 0));

  // April has 30 days
  EXPECT_TRUE(Quantity_Date::IsValid(4, 30, 2020, 0, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(4, 31, 2020, 0, 0, 0, 0, 0));

  // December has 31 days
  EXPECT_TRUE(Quantity_Date::IsValid(12, 31, 2020, 0, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(12, 32, 2020, 0, 0, 0, 0, 0));
}

// Test time validation (uses time constants)
TEST_F(Quantity_DateTest, TimeValidation)
{
  // Valid times
  EXPECT_TRUE(Quantity_Date::IsValid(1, 1, 2020, 23, 59, 59, 999, 999));
  EXPECT_TRUE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 0, 0, 0));

  // Invalid hours
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 24, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, -1, 0, 0, 0, 0));

  // Invalid minutes
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 60, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, -1, 0, 0, 0));

  // Invalid seconds
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 60, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, -1, 0, 0));

  // Invalid milliseconds
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 0, 1000, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 0, -1, 0));

  // Invalid microseconds
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 0, 0, 1000));
  EXPECT_FALSE(Quantity_Date::IsValid(1, 1, 2020, 0, 0, 0, 0, -1));
}

// Test SetValues and Values round-trip (tests SECONDS_PER_* constants)
TEST_F(Quantity_DateTest, SetValuesRoundTrip)
{
  Quantity_Date aDate;
  aDate.SetValues(3, 15, 2021, 10, 25, 30, 500, 750);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aDate.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(3, mm);
  EXPECT_EQ(15, dd);
  EXPECT_EQ(2021, yy);
  EXPECT_EQ(10, hh);
  EXPECT_EQ(25, mn);
  EXPECT_EQ(30, ss);
  EXPECT_EQ(500, mis);
  EXPECT_EQ(750, mics);
}

// Test individual component getters
TEST_F(Quantity_DateTest, IndividualGetters)
{
  // Test with a simple date first: January 2, 1979
  Quantity_Date aDate1(1, 2, 1979, 0, 0, 0, 0, 0);
  EXPECT_EQ(1, aDate1.Month());
  EXPECT_EQ(2, aDate1.Day());
  EXPECT_EQ(1979, aDate1.Year());

  // Test with July 20, 2024 at midnight
  Quantity_Date aDate2(7, 20, 2024, 0, 0, 0, 0, 0);
  EXPECT_EQ(7, aDate2.Month());
  EXPECT_EQ(20, aDate2.Day());
  EXPECT_EQ(2024, aDate2.Year());

  // Test with July 20, 2024 with time components
  Quantity_Date aDate3(7, 20, 2024, 15, 45, 30, 123, 456);
  EXPECT_EQ(7, aDate3.Month());
  EXPECT_EQ(20, aDate3.Day());
  EXPECT_EQ(2024, aDate3.Year());
  EXPECT_EQ(15, aDate3.Hour());
  EXPECT_EQ(45, aDate3.Minute());
  EXPECT_EQ(30, aDate3.Second());
  EXPECT_EQ(123, aDate3.MilliSecond());
  EXPECT_EQ(456, aDate3.MicroSecond());
}

// Test date difference calculation (uses USECS_PER_SEC constant)
TEST_F(Quantity_DateTest, DateDifference)
{
  Quantity_Date aDate1(1, 1, 2020, 0, 0, 0, 0, 0);
  Quantity_Date aDate2(1, 1, 2020, 1, 0, 0, 0, 0); // 1 hour later

  Quantity_Period aPeriod = aDate2.Difference(aDate1);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(3600, ss); // 1 hour = 3600 seconds (SECONDS_PER_HOUR)
  EXPECT_EQ(0, mics);
}

// Test adding period to date (uses USECS_PER_SEC constant)
TEST_F(Quantity_DateTest, AddPeriod)
{
  Quantity_Date   aDate(1, 1, 2020, 0, 0, 0, 0, 0);
  Quantity_Period aPeriod(1, 0, 0, 0, 0, 0); // 1 day

  Quantity_Date aNewDate = aDate.Add(aPeriod);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aNewDate.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);
  EXPECT_EQ(2, dd); // Next day
  EXPECT_EQ(2020, yy);
}

// Test subtracting period from date
TEST_F(Quantity_DateTest, SubtractPeriod)
{
  Quantity_Date   aDate(1, 2, 2020, 0, 0, 0, 0, 0); // January 2
  Quantity_Period aPeriod(1, 0, 0, 0, 0, 0);        // 1 day

  Quantity_Date aNewDate = aDate.Subtract(aPeriod);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aNewDate.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);
  EXPECT_EQ(1, dd); // Previous day
  EXPECT_EQ(2020, yy);
}

// Test year boundary crossing
TEST_F(Quantity_DateTest, YearBoundary)
{
  Quantity_Date   aDate(12, 31, 2020, 23, 59, 59, 0, 0);
  Quantity_Period aPeriod(0, 0, 0, 1, 0, 0); // 1 second

  Quantity_Date aNewDate = aDate.Add(aPeriod);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aNewDate.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);    // January
  EXPECT_EQ(1, dd);    // 1st
  EXPECT_EQ(2021, yy); // Next year
  EXPECT_EQ(0, hh);
  EXPECT_EQ(0, mn);
  EXPECT_EQ(0, ss);
}

// Test leap year boundary (Feb 28/29)
TEST_F(Quantity_DateTest, LeapYearBoundary)
{
  // Leap year: Feb 28 + 1 day = Feb 29
  Quantity_Date   aDate1(2, 28, 2020, 0, 0, 0, 0, 0);
  Quantity_Period aPeriod1(1, 0, 0, 0, 0, 0);
  Quantity_Date   aNewDate1 = aDate1.Add(aPeriod1);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aNewDate1.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(2, mm);
  EXPECT_EQ(29, dd); // Feb 29 exists in 2020
  EXPECT_EQ(2020, yy);

  // Non-leap year: Feb 28 + 1 day = Mar 1
  Quantity_Date   aDate2(2, 28, 2021, 0, 0, 0, 0, 0);
  Quantity_Period aPeriod2(1, 0, 0, 0, 0, 0);
  Quantity_Date   aNewDate2 = aDate2.Add(aPeriod2);

  aNewDate2.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(3, mm); // March
  EXPECT_EQ(1, dd); // 1st
  EXPECT_EQ(2021, yy);
}

// Test microsecond overflow (uses USECS_PER_SEC constant)
TEST_F(Quantity_DateTest, MicrosecondOverflow)
{
  Quantity_Date   aDate(1, 1, 2020, 0, 0, 0, 999, 999);
  Quantity_Period aPeriod(0, 0, 0, 0, 0, 1); // 1 microsecond

  Quantity_Date aNewDate = aDate.Add(aPeriod);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aNewDate.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);
  EXPECT_EQ(1, dd);
  EXPECT_EQ(2020, yy);
  EXPECT_EQ(0, hh);
  EXPECT_EQ(0, mn);
  EXPECT_EQ(1, ss); // Should overflow to next second
  EXPECT_EQ(0, mis);
  EXPECT_EQ(0, mics);
}

// Test specific date calculations (regression test for time constants)
TEST_F(Quantity_DateTest, SpecificDateCalculations)
{
  // Test that 24 hours = 1 day (SECONDS_PER_DAY = 86400)
  Quantity_Date   aDate1(1, 1, 2020, 0, 0, 0, 0, 0);
  Quantity_Period aPeriod24h(0, 24, 0, 0, 0, 0); // 24 hours

  Quantity_Date aDate2 = aDate1.Add(aPeriod24h);

  Standard_Integer mm, dd, yy, hh, mn, ss, mis, mics;
  aDate2.Values(mm, dd, yy, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, mm);
  EXPECT_EQ(2, dd); // Next day
  EXPECT_EQ(2020, yy);
  EXPECT_EQ(0, hh);
}

// Test minimum date (January 1, 1979)
TEST_F(Quantity_DateTest, MinimumDate)
{
  EXPECT_TRUE(Quantity_Date::IsValid(1, 1, 1979, 0, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Date::IsValid(12, 31, 1978, 23, 59, 59, 999, 999));
}

// Test thread-safety of getDaysInMonth (implicit test)
TEST_F(Quantity_DateTest, MultipleLeapYearChecks)
{
  // Repeatedly check different months and years
  // This would catch thread-safety issues with mutable month_table
  for (int year = 2000; year <= 2024; ++year)
  {
    for (int month = 1; month <= 12; ++month)
    {
      int maxDay;
      if (month == 2)
      {
        maxDay = Quantity_Date::IsLeap(year) ? 29 : 28;
      }
      else if (month == 4 || month == 6 || month == 9 || month == 11)
      {
        maxDay = 30;
      }
      else
      {
        maxDay = 31;
      }

      EXPECT_TRUE(Quantity_Date::IsValid(month, maxDay, year, 0, 0, 0, 0, 0));
      EXPECT_FALSE(Quantity_Date::IsValid(month, maxDay + 1, year, 0, 0, 0, 0, 0));
    }
  }
}

// Test date difference with epoch (Jan 1, 1979 00:00) - special case
TEST_F(Quantity_DateTest, DifferenceFromEpoch)
{
  Quantity_Date aEpoch;                           // Defaults to Jan 1, 1979 00:00
  Quantity_Date aDate(1, 2, 1979, 0, 0, 0, 0, 0); // Jan 2, 1979

  Quantity_Period aPeriod = aEpoch.Difference(aDate);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(86400, ss); // 1 day = 86400 seconds
  EXPECT_EQ(0, mics);
}

// Test date difference with microsecond underflow
TEST_F(Quantity_DateTest, DifferenceWithMicrosecondUnderflow)
{
  Quantity_Date aDate1(1, 1, 2020, 0, 0, 1, 200, 0); // 1.2 seconds
  Quantity_Date aDate2(1, 1, 2020, 0, 0, 0, 500, 0); // 0.5 seconds

  Quantity_Period aPeriod = aDate1.Difference(aDate2);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(0, ss);
  EXPECT_EQ(700000, mics); // 1.2 - 0.5 = 0.7 seconds = 700000 microseconds
}

// Test date difference with reversed dates (absolute value)
TEST_F(Quantity_DateTest, DifferenceReversed)
{
  Quantity_Date aDate1(1, 1, 2020, 1, 0, 0, 0, 0); // 1 hour
  Quantity_Date aDate2(1, 1, 2020, 3, 0, 0, 0, 0); // 3 hours

  // Difference should be absolute value
  Quantity_Period aPeriod = aDate1.Difference(aDate2);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(7200, ss); // 2 hours = 7200 seconds
  EXPECT_EQ(0, mics);
}
