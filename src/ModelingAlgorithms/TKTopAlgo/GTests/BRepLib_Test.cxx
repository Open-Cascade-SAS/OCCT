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

#include <BRepLib.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <cstdlib>
#include <thread>
#include <vector>

TEST(BRepLib_Test, Plane_NullRestoresDefault)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 5.0), gp::DZ()));
  BRepLib::Plane(aPlane);
  EXPECT_EQ(BRepLib::Plane(), aPlane);

  BRepLib::Plane(occ::handle<Geom_Plane>());
  ASSERT_FALSE(BRepLib::Plane().IsNull());
  EXPECT_NE(BRepLib::Plane(), aPlane);
  EXPECT_TRUE(BRepLib::Plane()->Pln().Location().IsEqual(gp::Origin(), 1.e-12));
  EXPECT_TRUE(BRepLib::Plane()->Pln().Axis().Direction().IsEqual(gp::DZ(), 1.e-12));
}

namespace
{
// The default plane is created on first use. Clearing it re-arms that first use, so each round
// has all threads reach it together and every one of them must be handed the same plane.
// Returns the number of rounds in which they were not.
int countPlaneMismatches(const int theNbThreads, const int theNbRounds)
{
  int aNbMismatches = 0;
  for (int aRound = 0; aRound < theNbRounds; ++aRound)
  {
    BRepLib::Plane(occ::handle<Geom_Plane>());

    std::atomic<int>               aNbReady(0);
    std::atomic<bool>              aGo(false);
    std::vector<const Geom_Plane*> aSeen(theNbThreads, nullptr);
    std::vector<std::thread>       aThreads;
    for (int aThreadIter = 0; aThreadIter < theNbThreads; ++aThreadIter)
    {
      aThreads.emplace_back([&, aThreadIter] {
        ++aNbReady;
        while (!aGo.load())
        {
          std::this_thread::yield();
        }
        aSeen[aThreadIter] = BRepLib::Plane().get();
      });
    }
    while (aNbReady.load() < theNbThreads)
    {
      std::this_thread::yield();
    }
    aGo = true;
    for (std::thread& aThread : aThreads)
    {
      aThread.join();
    }

    const Geom_Plane* aFinal = BRepLib::Plane().get();
    for (const Geom_Plane* aPlane : aSeen)
    {
      if (aPlane != aFinal)
      {
        ++aNbMismatches;
        break;
      }
    }
  }
  return aNbMismatches;
}
} // namespace

// The unsynchronised race can corrupt the heap, so it runs in a child process.
TEST(BRepLib_Test, Plane_ConcurrentFirstUse)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_EXIT(std::exit(countPlaneMismatches(16, 500) == 0 ? 0 : 1),
              ::testing::ExitedWithCode(0),
              "");
}
