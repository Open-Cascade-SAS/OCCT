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

#include <NCollection_List.hxx>
#include <OSD_Parallel.hxx>

#include <atomic>

// Test: For with large range ensures all indices are visited (chunked distribution).
// Verifying via sum of unique indices — correct sum proves each index visited exactly once.
TEST(OSD_ParallelTest, For_LargeRange_AllIndicesVisited)
{
  const int              THE_N = 100000;
  std::atomic<long long> aSum(0);
  std::atomic<int>       aCount(0);

  OSD_Parallel::For(0, THE_N, [&aSum, &aCount](int theIndex) {
    aSum.fetch_add(static_cast<long long>(theIndex));
    aCount.fetch_add(1);
  });

  const long long anExpectedSum = static_cast<long long>(THE_N - 1) * THE_N / 2;
  EXPECT_EQ(anExpectedSum, aSum.load());
  EXPECT_EQ(THE_N, aCount.load());
}

// Test: For with explicit grain size produces correct results.
TEST(OSD_ParallelTest, For_WithGrainSize_CorrectResults)
{
  const int              THE_N = 50000;
  std::atomic<long long> aSum(0);

  OSD_Parallel::For(
    0,
    THE_N,
    [&aSum](int theIndex) { aSum.fetch_add(theIndex); },
    /* theGrainSize = */ 1000);

  const long long anExpected = static_cast<long long>(THE_N - 1) * THE_N / 2;
  EXPECT_EQ(anExpected, aSum.load());
}

// Test: ForEach with list iterator processes all elements correctly.
TEST(OSD_ParallelTest, ForEach_ListIterator_CorrectResults)
{
  const int             THE_N = 10000;
  NCollection_List<int> aList;
  for (int i = 0; i < THE_N; ++i)
  {
    aList.Append(i);
  }

  std::atomic<long long> aSum(0);
  OSD_Parallel::ForEach(
    aList.begin(),
    aList.end(),
    [&aSum](int theValue) { aSum.fetch_add(static_cast<long long>(theValue)); },
    false,
    THE_N);

  const long long anExpected = static_cast<long long>(THE_N - 1) * THE_N / 2;
  EXPECT_EQ(anExpected, aSum.load());
}

// Test: Reduce computes a parallel sum correctly.
TEST(OSD_ParallelTest, Reduce_Sum_CorrectResult)
{
  const int       THE_N      = 100000;
  const long long anExpected = static_cast<long long>(THE_N - 1) * THE_N / 2;

  const long long aResult = OSD_Parallel::Reduce<long long>(
    0,
    THE_N,
    0LL, // identity
    [](int theIndex, long long& theAccum) { theAccum += theIndex; },
    [](const long long& thePartial, long long& theTotal) { theTotal += thePartial; });

  EXPECT_EQ(anExpected, aResult);
}

// Test: For with empty range does not execute functor.
TEST(OSD_ParallelTest, For_EmptyRange_NoWork)
{
  std::atomic<int> aCounter(0);

  OSD_Parallel::For(5, 5, [&aCounter](int) { aCounter.fetch_add(1); });

  EXPECT_EQ(0, aCounter.load());
}

// Test: For with single item executes exactly once.
TEST(OSD_ParallelTest, For_SingleItem_Executes)
{
  std::atomic<int> aCounter(0);

  OSD_Parallel::For(7, 8, [&aCounter](int theIndex) {
    EXPECT_EQ(7, theIndex);
    aCounter.fetch_add(1);
  });

  EXPECT_EQ(1, aCounter.load());
}

// Test: Reduce with single thread mode.
TEST(OSD_ParallelTest, Reduce_SingleThread_CorrectResult)
{
  const int THE_N      = 1000;
  const int anExpected = (THE_N - 1) * THE_N / 2;

  const int aResult = OSD_Parallel::Reduce<int>(
    0,
    THE_N,
    0,
    [](int theIndex, int& theAccum) { theAccum += theIndex; },
    [](const int& thePartial, int& theTotal) { theTotal += thePartial; },
    true); // force single thread

  EXPECT_EQ(anExpected, aResult);
}

// Test: Reduce with empty range returns identity.
TEST(OSD_ParallelTest, Reduce_EmptyRange_ReturnsIdentity)
{
  const double aResult = OSD_Parallel::Reduce<double>(
    10,
    10,
    42.0,
    [](int, double& theAccum) { theAccum += 1.0; },
    [](const double& thePartial, double& theTotal) { theTotal += thePartial; });

  EXPECT_NEAR(42.0, aResult, 1e-15);
}
