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

#include <gtest/gtest.h>

#include <Interface_Static.hxx>
#include <TCollection_AsciiString.hxx>

#include <thread>
#include <vector>

namespace
{
// File scope, not locals of the test: a lambda reads a namespace-scope constant without capturing
// it, which is the one form that both MSVC (C3493 on an uncaptured constexpr local) and Clang with
// -Werror (-Wunused-lambda-capture on a captured one) accept.
constexpr int THE_THREAD_COUNT = 8;
constexpr int THE_ITERATIONS   = 200;
} // namespace

// Interface_Static's parameter table is process-global and every STEP or IGES read and write goes
// through it, so concurrent access to it is ordinary use rather than misuse.
//
// The table is an NCollection_DataMap keyed by parameter name, and the stored values are
// MoniTool_TypedValue objects whose string payload is mutated in place. Without synchronisation a
// concurrent Init or SetCVal could rehash the map or rewrite a string while another thread was
// reading it, which is a memory-safety hazard and not only a stale read.
//
// These tests exercise the table concurrently. They are not expected to catch a race by assertion
// on every run; their value is under ThreadSanitizer or ASAN, where an unsynchronised table faults
// or reports, and as a guarantee that adding the lock did not deadlock. The lock is recursive
// precisely because Init re-enters Static() on the same thread.
TEST(Interface_StaticTest, ConcurrentDefinitionAndReadDoNotCorruptTheTable)
{
  std::vector<std::thread> aThreads;
  std::vector<int>         aFailures(THE_THREAD_COUNT, 0);

  for (int aThreadIndex = 0; aThreadIndex < THE_THREAD_COUNT; ++aThreadIndex)
  {
    aThreads.emplace_back([aThreadIndex, &aFailures]() {
      TCollection_AsciiString aName("gtest.interface.static.");
      aName += TCollection_AsciiString(aThreadIndex);

      for (int i = 0; i < THE_ITERATIONS; ++i)
      {
        Interface_Static::Init("gtest", aName.ToCString(), 'i', "0");
        Interface_Static::SetIVal(aName.ToCString(), i);
        if (Interface_Static::IVal(aName.ToCString()) != i)
        {
          ++aFailures[aThreadIndex];
        }
      }
    });
  }
  for (auto& aThread : aThreads)
  {
    aThread.join();
  }

  // Each thread owns a parameter no other thread names, so a correct table returns what that
  // thread last wrote.
  for (int aThreadIndex = 0; aThreadIndex < THE_THREAD_COUNT; ++aThreadIndex)
  {
    EXPECT_EQ(0, aFailures[aThreadIndex])
      << "thread " << aThreadIndex << " read back a value it had not written";
  }
}

// Reading a parameter while other threads define new ones must not fault, which is the map-rehash
// case rather than the value case.
TEST(Interface_StaticTest, ConcurrentReadWhileOtherThreadsDefineParameters)
{
  Interface_Static::Init("gtest", "gtest.interface.static.shared", 'i', "0");
  Interface_Static::SetIVal("gtest.interface.static.shared", 42);

  std::vector<std::thread> aThreads;
  std::vector<int>         aMismatches(4, 0);

  for (int aThreadIndex = 0; aThreadIndex < 4; ++aThreadIndex)
  {
    aThreads.emplace_back([aThreadIndex, &aMismatches]() {
      for (int i = 0; i < 300; ++i)
      {
        if (aThreadIndex % 2 == 0)
        {
          TCollection_AsciiString aName("gtest.interface.static.filler.");
          aName += TCollection_AsciiString(aThreadIndex * 1000 + i);
          Interface_Static::Init("gtest", aName.ToCString(), 'i', "0");
        }
        else if (Interface_Static::IVal("gtest.interface.static.shared") != 42)
        {
          ++aMismatches[aThreadIndex];
        }
      }
    });
  }
  for (auto& aThread : aThreads)
  {
    aThread.join();
  }

  for (int aThreadIndex = 1; aThreadIndex < 4; aThreadIndex += 2)
  {
    EXPECT_EQ(0, aMismatches[aThreadIndex])
      << "a parameter nobody rewrote changed while the table was growing";
  }
}
