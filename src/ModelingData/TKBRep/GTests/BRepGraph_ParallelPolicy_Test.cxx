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

#include <BRepGraph_ParallelPolicy.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_ParallelPolicyTest, WorkerCountAndAllowGateAreConsistent)
{
  EXPECT_GE(BRepGraph_ParallelPolicy::WorkerCount(), 1);
  EXPECT_FALSE(BRepGraph_ParallelPolicy::IsParallelAllowed(false));
  EXPECT_EQ(BRepGraph_ParallelPolicy::IsParallelAllowed(true),
            BRepGraph_ParallelPolicy::WorkerCount() > 1);
}

TEST(BRepGraph_ParallelPolicyTest, ShouldRunRejectsDisabledSingleWorkerAndTinyPrimaryRanges)
{
  using Workload = BRepGraph_ParallelPolicy::Workload;

  EXPECT_FALSE(BRepGraph_ParallelPolicy::ShouldRun(false, 8, Workload{100u, 100u, 100u}));
  EXPECT_FALSE(BRepGraph_ParallelPolicy::ShouldRun(true, 1, Workload{100u, 100u, 100u}));
  EXPECT_FALSE(BRepGraph_ParallelPolicy::ShouldRun(true, 4, Workload{1u, 100u, 100u}));
  EXPECT_FALSE(BRepGraph_ParallelPolicy::ShouldRun(true, 4, Workload{4u, 100u, 100u}));
}

TEST(BRepGraph_ParallelPolicyTest, ShouldRunUsesWorkerSquaredWorkThreshold)
{
  using Workload = BRepGraph_ParallelPolicy::Workload;

  EXPECT_FALSE(BRepGraph_ParallelPolicy::ShouldRun(true, 4, Workload{5u, 0u, 11u}));
  EXPECT_TRUE(BRepGraph_ParallelPolicy::ShouldRun(true, 4, Workload{5u, 0u, 12u}));
  EXPECT_TRUE(BRepGraph_ParallelPolicy::ShouldRun(true, 4, Workload{17u, 0u, 0u}));
}
