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

#include <OSD_Parallel.hxx>
#include <Standard_Boolean.hxx>

#include <atomic>
#include <gtest/gtest.h>

namespace
{
class IncrementerDecrementer
{
public:
  IncrementerDecrementer(std::atomic<int>* theVal, Standard_Boolean thePositive)
      : myVal(theVal),
        myPositive(thePositive)
  {
  }

  void operator()(const size_t) const
  {
    if (myPositive)
      ++(*myVal);
    else
      --(*myVal);
  }

private:
  std::atomic<int>* myVal;
  Standard_Boolean  myPositive;
};
} // namespace

TEST(Standard_AtomicTest, OCC22980_AtomicOperations)
{
  std::atomic<int> aSum(0);

  // Check returned value
  EXPECT_EQ(-1, aSum.fetch_sub(1) - 1);
  EXPECT_EQ(0, aSum.fetch_add(1) + 1);
  EXPECT_EQ(1, aSum.fetch_add(1) + 1);
  EXPECT_EQ(2, aSum.fetch_add(1) + 1);

  // Check atomicity
  aSum        = 0;
  const int N = 1 << 24; // big enough to ensure concurrency

  // Increment
  OSD_Parallel::For(0, N, IncrementerDecrementer(&aSum, Standard_True));
  EXPECT_EQ(N, aSum);

  // Decrement
  OSD_Parallel::For(0, N, IncrementerDecrementer(&aSum, Standard_False));
  EXPECT_EQ(0, aSum);
}
