// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Standard_CString.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Parallel.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>

namespace
{

struct TestCase
{
  const char* Input;
  double      Expected;
};

// A representative set of numeric strings that exercise different code paths
// inside Strtod: simple integers, fractions, exponents, large/small values,
// and edge cases that trigger the Bigint memory pool (Balloc/Bfree).
static const TestCase THE_TEST_CASES[] = {
  {"0.0", 0.0},
  {"1.0", 1.0},
  {"3.141592653589793", 3.141592653589793},
  {"-273.15", -273.15},
  {"1e10", 1e10},
  {"1.23456789e-15", 1.23456789e-15},
  {"123456789.987654321", 123456789.987654321},
  {"1.0e20", 1.0e20},
  {"1.0e-20", 1.0e-20},
  {"9.999999999999999e300", 9.999999999999999e300},
  {"5e-324", 5e-324},
  {"2.2250738585072014e-308", 2.2250738585072014e-308},
  {"17976931348623157.0", 17976931348623157.0},
  {"0.000001", 0.000001},
  {"999999999999999.0", 999999999999999.0},
};

static const int THE_NB_CASES = sizeof(THE_TEST_CASES) / sizeof(THE_TEST_CASES[0]);

} // namespace

// Basic sanity: Strtod parses known values correctly in a single thread.
TEST(Standard_StrtodTest, SingleThread_Correctness)
{
  for (int i = 0; i < THE_NB_CASES; ++i)
  {
    char*  anEnd = nullptr;
    double aVal  = Strtod(THE_TEST_CASES[i].Input, &anEnd);
    EXPECT_TRUE(anEnd != THE_TEST_CASES[i].Input)
      << "No digits parsed for: " << THE_TEST_CASES[i].Input;
    EXPECT_DOUBLE_EQ(aVal, THE_TEST_CASES[i].Expected)
      << "Mismatch for input: " << THE_TEST_CASES[i].Input;
  }
}

// The core regression test: concurrent Strtod calls must not corrupt each
// other's results. Prior to the fix, the Bigint memory pool (private_mem,
// pmem_next, TI0) was a single global static shared across threads without
// synchronization, causing random mis-parses.
TEST(Standard_StrtodTest, Parallel_ConsistentResults)
{
  constexpr int aNbThreads = 200;

  NCollection_Array1<double> aResults(0, aNbThreads - 1);
  for (int i = 0; i < aNbThreads; ++i)
  {
    aResults(i) = -1.0;
  }

  // Use a string long enough to force use of the Bigint memory pool
  // (values with many significant digits exercise Balloc paths).
  const char* aTestInput = "3.14159265358979323846264338327950288419716939937510";

  OSD_Parallel::For(0, aNbThreads, [&](int theIndex) {
    char*  anEnd       = nullptr;
    double aVal        = Strtod(aTestInput, &anEnd);
    aResults(theIndex) = aVal;
  });

  // All results must be identical to the single-threaded reference.
  char*  aRefEnd = nullptr;
  double aRef    = Strtod(aTestInput, &aRefEnd);

  for (int i = 0; i < aNbThreads; ++i)
  {
    EXPECT_DOUBLE_EQ(aResults(i), aRef)
      << "Thread " << i << " produced a different result: " << aResults(i);
  }
}

// Run many iterations to increase the chance of detecting intermittent races.
TEST(Standard_StrtodTest, Parallel_RepeatedRuns)
{
  constexpr int aNbThreads = 100;
  constexpr int aNbRuns    = 10;
  const char*   aTestInput = "2.71828182845904523536028747135266249775724709369995";

  char*  aRefEnd = nullptr;
  double aRef    = Strtod(aTestInput, &aRefEnd);

  for (int aRun = 0; aRun < aNbRuns; ++aRun)
  {
    NCollection_Array1<double> aResults(0, aNbThreads - 1);

    OSD_Parallel::For(0, aNbThreads, [&](int theIndex) {
      char*  anEnd       = nullptr;
      double aVal        = Strtod(aTestInput, &anEnd);
      aResults(theIndex) = aVal;
    });

    for (int i = 0; i < aNbThreads; ++i)
    {
      EXPECT_DOUBLE_EQ(aResults(i), aRef) << "Run " << aRun << ", thread " << i << " mismatch";
    }
  }
}

// Test with many distinct inputs processed in parallel to exercise different
// code paths (small values, large values, many-digit values) concurrently.
TEST(Standard_StrtodTest, Parallel_ManyDistinctInputs)
{
  constexpr int aNbThreads = 100;

  NCollection_Array1<double> aResults(0, aNbThreads - 1);
  NCollection_Array1<double> aExpected(0, aNbThreads - 1);

  // Compute expected values single-threaded first.
  for (int i = 0; i < aNbThreads; ++i)
  {
    const char* anInput = THE_TEST_CASES[i % THE_NB_CASES].Input;
    aExpected(i)        = Strtod(anInput, nullptr);
  }

  // Now compute in parallel.
  OSD_Parallel::For(0, aNbThreads, [&](int theIndex) {
    const char* anInput = THE_TEST_CASES[theIndex % THE_NB_CASES].Input;
    aResults(theIndex)  = Strtod(anInput, nullptr);
  });

  for (int i = 0; i < aNbThreads; ++i)
  {
    EXPECT_DOUBLE_EQ(aResults(i), aExpected(i))
      << "Thread " << i << " mismatch for input: " << THE_TEST_CASES[i % THE_NB_CASES].Input;
  }
}
