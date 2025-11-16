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

#include <Quantity_Period.hxx>

#include <gtest/gtest.h>

// Test fixture for Quantity_Period tests
class Quantity_PeriodTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test basic construction
TEST_F(Quantity_PeriodTest, BasicConstruction)
{
  // Constructor with days, hours, minutes, seconds, milliseconds, microseconds
  Quantity_Period aPeriod1(1, 2, 3, 4, 5, 6);

  Standard_Integer dd, hh, mn, ss, mis, mics;
  aPeriod1.Values(dd, hh, mn, ss, mis, mics);

  EXPECT_EQ(1, dd);
  EXPECT_EQ(2, hh);
  EXPECT_EQ(3, mn);
  EXPECT_EQ(4, ss);
  EXPECT_EQ(5, mis);
  EXPECT_EQ(6, mics);

  // Constructor with seconds and microseconds
  Quantity_Period aPeriod2(3600, 500000); // 1 hour, 500000 microseconds

  Standard_Integer ss2, mics2;
  aPeriod2.Values(ss2, mics2);

  EXPECT_EQ(3600, ss2); // SECONDS_PER_HOUR
  EXPECT_EQ(500000, mics2);
}

// Test constexpr comparison operators
TEST_F(Quantity_PeriodTest, ConstexprComparisons)
{
  Quantity_Period aPeriod1(0, 0, 0, 100, 0, 0);
  Quantity_Period aPeriod2(0, 0, 0, 100, 0, 0);
  Quantity_Period aPeriod3(0, 0, 0, 200, 0, 0);

  // Test IsEqual (constexpr)
  EXPECT_TRUE(aPeriod1.IsEqual(aPeriod2));
  EXPECT_FALSE(aPeriod1.IsEqual(aPeriod3));

  // Test IsShorter (constexpr)
  EXPECT_TRUE(aPeriod1.IsShorter(aPeriod3));
  EXPECT_FALSE(aPeriod3.IsShorter(aPeriod1));
  EXPECT_FALSE(aPeriod1.IsShorter(aPeriod2));

  // Test IsLonger (constexpr)
  EXPECT_TRUE(aPeriod3.IsLonger(aPeriod1));
  EXPECT_FALSE(aPeriod1.IsLonger(aPeriod3));
  EXPECT_FALSE(aPeriod1.IsLonger(aPeriod2));
}

// Test validation
TEST_F(Quantity_PeriodTest, Validation)
{
  // Valid periods
  EXPECT_TRUE(Quantity_Period::IsValid(1, 2, 3, 4, 5, 6));
  EXPECT_TRUE(Quantity_Period::IsValid(0, 0, 0, 0, 0, 0));

  // Invalid periods (negative values)
  EXPECT_FALSE(Quantity_Period::IsValid(-1, 0, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Period::IsValid(0, -1, 0, 0, 0, 0));
  EXPECT_FALSE(Quantity_Period::IsValid(0, 0, -1, 0, 0, 0));
  EXPECT_FALSE(Quantity_Period::IsValid(0, 0, 0, -1, 0, 0));
  EXPECT_FALSE(Quantity_Period::IsValid(0, 0, 0, 0, -1, 0));
  EXPECT_FALSE(Quantity_Period::IsValid(0, 0, 0, 0, 0, -1));

  // Validation with seconds and microseconds only
  EXPECT_TRUE(Quantity_Period::IsValid(100, 500));
  EXPECT_FALSE(Quantity_Period::IsValid(-1, 500));
  EXPECT_FALSE(Quantity_Period::IsValid(100, -1));
}

// Test SetValues and Values round-trip (tests time constants)
TEST_F(Quantity_PeriodTest, SetValuesRoundTrip)
{
  Quantity_Period aPeriod(0, 0); // Initialize with zero period
  aPeriod.SetValues(2, 3, 4, 5, 6, 7);

  Standard_Integer dd, hh, mn, ss, mis, mics;
  aPeriod.Values(dd, hh, mn, ss, mis, mics);

  EXPECT_EQ(2, dd);
  EXPECT_EQ(3, hh);
  EXPECT_EQ(4, mn);
  EXPECT_EQ(5, ss);
  EXPECT_EQ(6, mis);
  EXPECT_EQ(7, mics);
}

// Test conversion between formats (uses SECONDS_PER_DAY, etc.)
TEST_F(Quantity_PeriodTest, FormatConversion)
{
  // 1 day should equal 86400 seconds (SECONDS_PER_DAY)
  Quantity_Period aPeriod1(1, 0, 0, 0, 0, 0);

  Standard_Integer ss, mics;
  aPeriod1.Values(ss, mics);

  EXPECT_EQ(86400, ss); // SECONDS_PER_DAY = 24 * 3600

  // 1 hour should equal 3600 seconds (SECONDS_PER_HOUR)
  Quantity_Period aPeriod2(0, 1, 0, 0, 0, 0);
  aPeriod2.Values(ss, mics);

  EXPECT_EQ(3600, ss); // SECONDS_PER_HOUR

  // 1 minute should equal 60 seconds (SECONDS_PER_MINUTE)
  Quantity_Period aPeriod3(0, 0, 1, 0, 0, 0);
  aPeriod3.Values(ss, mics);

  EXPECT_EQ(60, ss); // SECONDS_PER_MINUTE
}

// Test millisecond to microsecond conversion (uses USECS_PER_MSEC)
TEST_F(Quantity_PeriodTest, MillisecondConversion)
{
  // 1 millisecond = 1000 microseconds (USECS_PER_MSEC)
  Quantity_Period aPeriod(0, 0, 0, 0, 1, 0);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(0, ss);
  EXPECT_EQ(1000, mics); // USECS_PER_MSEC

  // Mixed milliseconds and microseconds
  Quantity_Period aPeriod2(0, 0, 0, 0, 5, 250);
  aPeriod2.Values(ss, mics);

  EXPECT_EQ(0, ss);
  EXPECT_EQ(5250, mics); // 5 * 1000 + 250
}

// Test microsecond overflow (uses USECS_PER_SEC constant)
TEST_F(Quantity_PeriodTest, MicrosecondOverflow)
{
  // Create period with > 1,000,000 microseconds
  Quantity_Period aPeriod(0, 0); // Initialize with zero period
  aPeriod.SetValues(0, 1500000); // 1.5 seconds in microseconds

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(1, ss);        // 1 second
  EXPECT_EQ(500000, mics); // 0.5 second = 500000 microseconds
}

// Test adding periods (uses USECS_PER_SEC constant)
TEST_F(Quantity_PeriodTest, AddPeriods)
{
  Quantity_Period aPeriod1(0, 1, 0, 0, 0, 0); // 1 hour
  Quantity_Period aPeriod2(0, 2, 0, 0, 0, 0); // 2 hours

  Quantity_Period aResult = aPeriod1.Add(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  EXPECT_EQ(10800, ss); // 3 hours = 3 * 3600 = 10800 seconds
  EXPECT_EQ(0, mics);
}

// Test adding with microsecond overflow
TEST_F(Quantity_PeriodTest, AddWithMicrosecondOverflow)
{
  Quantity_Period aPeriod1(0, 0, 0, 0, 0, 600000); // 0.6 seconds
  Quantity_Period aPeriod2(0, 0, 0, 0, 0, 600000); // 0.6 seconds

  Quantity_Period aResult = aPeriod1.Add(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  EXPECT_EQ(1, ss);        // Should overflow to 1 second
  EXPECT_EQ(200000, mics); // Remaining 0.2 seconds
}

// Test subtracting periods (uses USECS_PER_SEC constant)
TEST_F(Quantity_PeriodTest, SubtractPeriods)
{
  Quantity_Period aPeriod1(0, 3, 0, 0, 0, 0); // 3 hours
  Quantity_Period aPeriod2(0, 1, 0, 0, 0, 0); // 1 hour

  Quantity_Period aResult = aPeriod1.Subtract(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  EXPECT_EQ(7200, ss); // 2 hours = 2 * 3600
  EXPECT_EQ(0, mics);
}

// Test subtracting with microsecond underflow
TEST_F(Quantity_PeriodTest, SubtractWithMicrosecondUnderflow)
{
  Quantity_Period aPeriod1(0, 0, 0, 1, 0, 200000); // 1.2 seconds
  Quantity_Period aPeriod2(0, 0, 0, 0, 0, 500000); // 0.5 seconds

  Quantity_Period aResult = aPeriod1.Subtract(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  EXPECT_EQ(0, ss);
  EXPECT_EQ(700000, mics); // 1.2 - 0.5 = 0.7 seconds
}

// Test subtracting larger from smaller (absolute value)
TEST_F(Quantity_PeriodTest, SubtractNegative)
{
  Quantity_Period aPeriod1(0, 1, 0, 0, 0, 0); // 1 hour
  Quantity_Period aPeriod2(0, 3, 0, 0, 0, 0); // 3 hours

  Quantity_Period aResult = aPeriod1.Subtract(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  // Result should be absolute value (2 hours)
  EXPECT_EQ(7200, ss); // 2 * 3600
  EXPECT_EQ(0, mics);
}

// Test complex period calculations
TEST_F(Quantity_PeriodTest, ComplexCalculations)
{
  // 1 day, 2 hours, 30 minutes, 45 seconds, 500 milliseconds, 250 microseconds
  Quantity_Period aPeriod(1, 2, 30, 45, 500, 250);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  // Calculate expected seconds:
  // 1 day = 86400 seconds
  // 2 hours = 7200 seconds
  // 30 minutes = 1800 seconds
  // 45 seconds = 45 seconds
  // Total = 95445 seconds
  EXPECT_EQ(95445, ss);

  // Calculate expected microseconds:
  // 500 milliseconds = 500000 microseconds
  // 250 microseconds = 250 microseconds
  // Total = 500250 microseconds
  EXPECT_EQ(500250, mics);
}

// Test extraction back to components (uses all time constants)
TEST_F(Quantity_PeriodTest, ComponentExtraction)
{
  Quantity_Period aPeriod(2, 3, 45, 30, 123, 456);

  Standard_Integer dd, hh, mn, ss, mis, mics;
  aPeriod.Values(dd, hh, mn, ss, mis, mics);

  EXPECT_EQ(2, dd);
  EXPECT_EQ(3, hh);
  EXPECT_EQ(45, mn);
  EXPECT_EQ(30, ss);
  EXPECT_EQ(123, mis);
  EXPECT_EQ(456, mics);
}

// Test zero period
TEST_F(Quantity_PeriodTest, ZeroPeriod)
{
  Quantity_Period aPeriod(0, 0, 0, 0, 0, 0);

  Standard_Integer dd, hh, mn, ss, mis, mics;
  aPeriod.Values(dd, hh, mn, ss, mis, mics);

  EXPECT_EQ(0, dd);
  EXPECT_EQ(0, hh);
  EXPECT_EQ(0, mn);
  EXPECT_EQ(0, ss);
  EXPECT_EQ(0, mis);
  EXPECT_EQ(0, mics);

  // Zero period should be equal to itself
  Quantity_Period aZero2(0, 0, 0, 0, 0, 0);
  EXPECT_TRUE(aPeriod.IsEqual(aZero2));
}

// Test large values (stress test for constants)
TEST_F(Quantity_PeriodTest, LargeValues)
{
  // 100 days
  Quantity_Period aPeriod(100, 0, 0, 0, 0, 0);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(8640000, ss); // 100 * 86400
  EXPECT_EQ(0, mics);
}

// Test specific time constant values (regression test)
TEST_F(Quantity_PeriodTest, TimeConstantValues)
{
  // Test that 24 hours = 1 day
  Quantity_Period aPeriod24h(0, 24, 0, 0, 0, 0);
  Quantity_Period aPeriod1d(1, 0, 0, 0, 0, 0);

  Standard_Integer ss1, mics1, ss2, mics2;
  aPeriod24h.Values(ss1, mics1);
  aPeriod1d.Values(ss2, mics2);

  EXPECT_EQ(ss1, ss2); // Should both be 86400 seconds

  // Test that 60 seconds = 1 minute
  Quantity_Period aPeriod60s(0, 0, 0, 60, 0, 0);
  Quantity_Period aPeriod1m(0, 0, 1, 0, 0, 0);

  aPeriod60s.Values(ss1, mics1);
  aPeriod1m.Values(ss2, mics2);

  EXPECT_EQ(ss1, ss2); // Should both be 60 seconds

  // Test that 60 minutes = 1 hour
  Quantity_Period aPeriod60m(0, 0, 60, 0, 0, 0);
  Quantity_Period aPeriod1h(0, 1, 0, 0, 0, 0);

  aPeriod60m.Values(ss1, mics1);
  aPeriod1h.Values(ss2, mics2);

  EXPECT_EQ(ss1, ss2); // Should both be 3600 seconds
}

// Test that 1000 milliseconds = 1000000 microseconds = 1 second
TEST_F(Quantity_PeriodTest, MillisecondToSecondConversion)
{
  Quantity_Period aPeriod(0, 0, 0, 0, 1000, 0);

  Standard_Integer ss, mics;
  aPeriod.Values(ss, mics);

  EXPECT_EQ(1, ss);   // Should overflow to 1 second
  EXPECT_EQ(0, mics); // No remaining microseconds
}

// Test period subtraction with large microsecond underflow (verifies O(1) optimization)
TEST_F(Quantity_PeriodTest, SubtractLargeMicrosecondUnderflow)
{
  Quantity_Period aPeriod1(0, 0, 0, 10, 0, 200000); // 10.2 seconds
  Quantity_Period aPeriod2(0, 0, 0, 0, 0, 2700000); // 2.7 seconds

  Quantity_Period aResult = aPeriod1.Subtract(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  EXPECT_EQ(7, ss); // 10.2 - 2.7 = 7.5 seconds
  EXPECT_EQ(500000, mics);
}

// Test period subtraction with negative seconds and positive microseconds
TEST_F(Quantity_PeriodTest, SubtractNegativeWithMicroseconds)
{
  Quantity_Period aPeriod1(0, 0, 0, 5, 0, 300000); // 5.3 seconds
  Quantity_Period aPeriod2(0, 0, 0, 8, 0, 100000); // 8.1 seconds

  Quantity_Period aResult = aPeriod1.Subtract(aPeriod2);

  Standard_Integer ss, mics;
  aResult.Values(ss, mics);

  // Absolute value: 8.1 - 5.3 = 2.8 seconds
  EXPECT_EQ(2, ss);
  EXPECT_EQ(800000, mics);
}
