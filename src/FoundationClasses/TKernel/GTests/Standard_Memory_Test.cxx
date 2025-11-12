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

#include <Standard.hxx>
#include <Standard_Address.hxx>
#include <Standard_Failure.hxx>
#include <NCollection_List.hxx>

#include <gtest/gtest.h>

namespace
{
// Helper namespace for OCC24836 out-of-memory test
// The test is based on occupying all available virtual memory.
// Obviously it has no sense on 64-bit platforms.

enum AllocTestStatus
{
  NotApplicable = 0x1,
  OUMCatchOK    = 0x2,
  OUMCatchFail  = 0x4
};

template <int>
int testOutOfMemory()
{
  // non-32-bit implementation
  return NotApplicable;
}

template <>
int testOutOfMemory<4>()
{
  // 32-bit implementation
  NCollection_List<Standard_Address> aList;
  const Standard_Integer             aBlockSizes[] = {100000, 10000, 10};
  int                                aStatus       = 0;

  // Start populating memory with blocks of large size, then
  // smaller ones and so on according to content of the array aBlockSizes
  for (size_t i = 0; i < sizeof(aBlockSizes) / sizeof(int); i++)
  {
    try
    {
      for (;;)
        aList.Append(Standard::Allocate(aBlockSizes[i]));
    }
    catch (Standard_Failure const&)
    {
      aStatus |= OUMCatchOK;
    }
    catch (...)
    {
      aStatus |= OUMCatchFail;
      break;
    }
  }

  // Release all allocated blocks
  for (NCollection_List<Standard_Address>::Iterator anIt(aList); anIt.More(); anIt.Next())
  {
    Standard::Free(anIt.Value());
  }

  return aStatus;
}
} // namespace

TEST(StandardMemoryTest, OCC24836_OutOfMemoryExceptionHandling)
{
  // Bug OCC24836: Stack overflow when raising exception in low memory condition
  // Test that out-of-memory exceptions are properly caught on 32-bit platforms

  int aStatus = testOutOfMemory<sizeof(size_t)>();

  if (aStatus & NotApplicable)
  {
    // Test is not applicable for 64-bit and higher platforms
    GTEST_SKIP() << "This test is not applicable for 64-bit and higher platforms";
  }

  // On 32-bit platforms, verify that out-of-memory is caught
  EXPECT_TRUE(aStatus & OUMCatchOK) << "Out-of-memory should be caught via Standard_Failure";
  EXPECT_FALSE(aStatus & OUMCatchFail)
    << "Out-of-memory should not be caught via generic catch(...)";
}
