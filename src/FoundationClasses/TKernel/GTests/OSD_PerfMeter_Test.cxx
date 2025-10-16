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

#include <gtest/gtest.h>
#include <OSD_PerfMeter.hxx>
#include <TCollection_AsciiString.hxx>

#include <chrono>
#include <thread>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// Test fixture for OSD_PerfMeter tests
class OSD_PerfMeterTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Reset all meters before each test
    OSD_PerfMeter::ResetALL();
  }

  void TearDown() override
  {
    // Reset all meters after each test
    OSD_PerfMeter::ResetALL();
  }

  // Helper function to perform some measurable work
  static void DoSomeWork(const double theTimeInSec = 0.1)
  {
    OSD_PerfMeter meter("WorkMeter", true);
    while (meter.Elapsed() < theTimeInSec)
    {
      // Do some computational work that takes considerable time
      // compared with time of starting/stopping timers
      volatile double result = 0.0;
      for (int i = 0; i < 10000; ++i)
      {
        // Complex mathematical operations using only standard library
        result += std::sin(i * 0.001) * std::cos(i * 0.002);
        result += std::sqrt(i + 1.0);
        result += std::pow(i * 0.1, 1.5);

        // String operations to add more computational cost
        TCollection_AsciiString aStr("Test");
        aStr += TCollection_AsciiString(i);
        volatile int len = aStr.Length(); // volatile to prevent optimization
        (void)len;                        // Suppress unused variable warning
      }
      (void)result; // Suppress unused variable warning
    }
    meter.Kill();
  }
};

//==================================================================================================

// Test basic creation and auto-start functionality
TEST_F(OSD_PerfMeterTest, BasicCreationWithAutoStart)
{
  // Create an auto-started meter
  OSD_PerfMeter meter("TestMeter", true);

  // Do some work
  DoSomeWork();

  // Stop the meter
  meter.Stop();

  // Check elapsed time is positive
  double elapsed = meter.Elapsed();
  EXPECT_GT(elapsed, 0.0) << "Elapsed time should be positive";
}

//==================================================================================================

// Test manual start/stop functionality
TEST_F(OSD_PerfMeterTest, ManualStartStop)
{
  // Create a meter without auto-start
  OSD_PerfMeter meter("ManualMeter", false);

  // Start the meter
  meter.Start();

  // Do some work
  DoSomeWork();

  // Stop the meter
  meter.Stop();

  // Check elapsed time is positive
  double elapsed = meter.Elapsed();
  EXPECT_GT(elapsed, 0.0) << "Elapsed time should be positive";
}

//==================================================================================================

// Test default constructor and Init
TEST_F(OSD_PerfMeterTest, DefaultConstructorAndInit)
{
  // Create meter with default constructor
  OSD_PerfMeter meter;

  // Initialize it
  meter.Init("InitializedMeter");

  // Start it manually
  meter.Start();

  // Do some work
  DoSomeWork();

  // Stop and check elapsed time
  meter.Stop();
  EXPECT_GT(meter.Elapsed(), 0.0) << "Initialized meter should have positive elapsed time";
}

//==================================================================================================

// Test that two meters with the same name refer to the same internal meter
TEST_F(OSD_PerfMeterTest, SharedMetersByName)
{
  const char* meterName = "SharedMeter";

  // Create and start first meter
  OSD_PerfMeter meter1(meterName);

  // Do some work
  DoSomeWork();

  // Create second meter with the same name, but don't auto-start
  OSD_PerfMeter meter2;
  meter2.Init(meterName);

  // Do more work
  DoSomeWork();

  // Stop both meters
  meter1.Stop();

  // The elapsed time should account for both work segments
  double elapsed = meter1.Elapsed();
  EXPECT_GT(elapsed, 0.0) << "Elapsed time should be positive";

  // Stopping meter2 should have no effect since they share the same meter
  // and meter1 already stopped it
  meter2.Stop();
  double elapsed2 = meter2.Elapsed();
  EXPECT_DOUBLE_EQ(elapsed, elapsed2) << "Both meter instances should show same elapsed time";
}

//==================================================================================================

// Test restarting a meter
TEST_F(OSD_PerfMeterTest, RestartMeter)
{
  // Create and start meter
  OSD_PerfMeter meter("RestartMeter");

  // Do some work
  DoSomeWork(0.1);

  // Stop and get elapsed time
  meter.Stop();
  double elapsed1 = meter.Elapsed();
  EXPECT_GT(elapsed1, 0.0) << "First elapsed time should be positive";

  // Restart the meter
  meter.Start();

  // Do more work
  DoSomeWork(0.2);

  // Stop and get elapsed time again
  meter.Stop();
  double elapsed2 = meter.Elapsed();
  EXPECT_GT(elapsed2, 0.0) << "Second elapsed time should be positive";

  // Second elapsed time should be different from first
  EXPECT_NE(elapsed1, elapsed2) << "After restart, elapsed time should be different";
}

//==================================================================================================

// Test relative timing accuracy
TEST_F(OSD_PerfMeterTest, RelativeTimingAccuracy)
{
  // Create two meters
  OSD_PerfMeter meter1("ShortMeter");
  OSD_PerfMeter meter2("LongMeter");

  // Short work for meter1
  DoSomeWork(0.1);
  meter1.Stop();

  // Long work for meter2
  DoSomeWork(0.5);
  meter2.Stop();

  // Long work should take more time than short work
  double shortElapsed = meter1.Elapsed();
  double longElapsed  = meter2.Elapsed();

  EXPECT_GT(shortElapsed, 0.0) << "Short elapsed time should be positive";
  EXPECT_GT(longElapsed, 0.0) << "Long elapsed time should be positive";
  EXPECT_GT(longElapsed, shortElapsed) << "Long work should take more time than short work";
}

//==================================================================================================

// Test the static PrintALL method
TEST_F(OSD_PerfMeterTest, PrintALL)
{
  // Create and use several meters
  OSD_PerfMeter meter1("Meter1");
  DoSomeWork(0.1);
  meter1.Stop();

  OSD_PerfMeter meter2("Meter2");
  DoSomeWork(0.2);
  meter2.Stop();

  OSD_PerfMeter meter3("Meter3");
  DoSomeWork(0.3);
  meter3.Stop();

  // Get the printed output
  std::string output = OSD_PerfMeter::PrintALL().ToCString();

  // Check that the output contains all meter names
  EXPECT_TRUE(output.find("Meter1") != std::string::npos)
    << "PrintALL output should contain Meter1";
  EXPECT_TRUE(output.find("Meter2") != std::string::npos)
    << "PrintALL output should contain Meter2";
  EXPECT_TRUE(output.find("Meter3") != std::string::npos)
    << "PrintALL output should contain Meter3";
}

//==================================================================================================

// Test the static ResetALL method
TEST_F(OSD_PerfMeterTest, ResetALL)
{
  // Create and use several meters
  OSD_PerfMeter meter1("ResetMeter1");
  DoSomeWork(0.1);
  meter1.Stop();

  OSD_PerfMeter meter2("ResetMeter2");
  DoSomeWork(0.2);
  meter2.Stop();

  // Both meters should have positive elapsed times
  EXPECT_GT(meter1.Elapsed(), 0.0);
  EXPECT_GT(meter2.Elapsed(), 0.0);

  // Reset all meters
  OSD_PerfMeter::ResetALL();

  // After reset, all meters should have zero elapsed time
  EXPECT_EQ(meter1.Elapsed(), 0.0) << "Elapsed time should be zero after ResetALL";
  EXPECT_EQ(meter2.Elapsed(), 0.0) << "Elapsed time should be zero after ResetALL";
}

//==================================================================================================

// Test unused meter behavior
TEST_F(OSD_PerfMeterTest, UnusedMeter)
{
  // Create a meter but don't start it
  OSD_PerfMeter meter("UnusedMeter", false);

  // Elapsed time should be zero
  EXPECT_EQ(meter.Elapsed(), 0.0) << "Unused meter should have zero elapsed time, actual";

  // Stopping not started meter should have no effect
  meter.Stop();
  EXPECT_EQ(meter.Elapsed(), 0.0) << "Stopping an unused meter should leave elapsed time at zero";
}

//==================================================================================================

// Test the Print() method
TEST_F(OSD_PerfMeterTest, PrintMethod)
{
  // Create and start the meter
  const TCollection_AsciiString meterName = "PrintTestMeter";
  OSD_PerfMeter                 meter(meterName);

  // Do some work
  DoSomeWork();

  // Stop the meter
  meter.Stop();

  // Get the printed output
  std::string output = meter.Print().ToCString();

  // Verify output contains necessary information
  EXPECT_TRUE(output.find(meterName.ToCString()) != std::string::npos)
    << "Print output should contain meter name";

  // Verify output contains elapsed time info (we can't check exact time,
  // but there should be numbers)
  EXPECT_TRUE(output.find_first_of("0123456789") != std::string::npos)
    << "Print output should contain elapsed time values";

  EXPECT_TRUE(output.find("sec") != std::string::npos)
    << "Print output should contain millisecond units";
}

//==================================================================================================

// Test the Kill() method
TEST_F(OSD_PerfMeterTest, KillMethod)
{
  // Create and start the meter
  OSD_PerfMeter meter("KillTestMeter");

  // Do some work
  DoSomeWork();

  // Stop the meter
  meter.Stop();

  // Verify we have non-zero elapsed time
  double elapsed = meter.Elapsed();
  EXPECT_GT(elapsed, 0.0) << "Meter should have recorded time before Kill";

  // Kill the meter
  meter.Kill();

  // Verify elapsed time is reset
  double elapsedAfterKill = meter.Elapsed();
  EXPECT_EQ(elapsedAfterKill, 0.0) << "Elapsed time should be reset after Kill";

  // Verify killing a meter makes it disappear from the global list
  std::string allMeters = OSD_PerfMeter::PrintALL().ToCString();
  EXPECT_TRUE(allMeters.find("KillTestMeter") == std::string::npos)
    << "Killed meter should not appear in PrintALL output";
}

//==================================================================================================

// Test Kill method on a running meter
TEST_F(OSD_PerfMeterTest, KillRunningMeter)
{
  // Create and start the meter
  OSD_PerfMeter meter("KillRunningMeter");

  // Don't stop the meter, kill it while running
  meter.Kill();

  // Verify elapsed time is reset
  double elapsedAfterKill = meter.Elapsed();
  EXPECT_EQ(elapsedAfterKill, 0.0) << "Elapsed time should be reset after Kill";

  // Start the meter again to verify it's usable after being killed
  meter.Init("KillRunningMeter");
  meter.Start();
  DoSomeWork();
  meter.Stop();

  // Verify it records time correctly after being killed and restarted
  EXPECT_GT(meter.Elapsed(), 0.0) << "Meter should record time after Kill and restart";
}
