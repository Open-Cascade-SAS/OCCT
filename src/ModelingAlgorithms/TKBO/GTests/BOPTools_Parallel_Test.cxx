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

#include <BOPTools_Parallel.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_Transient.hxx>

#include <gtest/gtest.h>

namespace
{
//! Context recording the solvers performed with it and the groups declared done with it.
//! A context is used by one thread at a time, so the records need no lock.
class BOPTools_ParallelTestContext : public Standard_Transient
{
public:
  BOPTools_ParallelTestContext() = default;

  //! Constructor used by BOPTools_Parallel for the contexts it creates
  BOPTools_ParallelTestContext(const occ::handle<NCollection_BaseAllocator>&) {}

  //! Records of the context: index of a performed solver, or -1 - key for a done group
  NCollection_DynamicArray<int> Records;
};

//! Solver recording its performance in its context
class BOPTools_ParallelTestSolver
{
public:
  void SetKey(const int theIndex, const int theKey)
  {
    myIndex = theIndex;
    myKey   = theKey;
  }

  int Key() const { return myKey; }

  void SetContext(const occ::handle<BOPTools_ParallelTestContext>& theContext)
  {
    myContext = theContext;
  }

  const occ::handle<BOPTools_ParallelTestContext>& Context() const { return myContext; }

  int NbPerformed() const { return myNbPerformed; }

  void Perform()
  {
    ++myNbPerformed;
    myContext->Records.Append(myIndex);
  }

private:
  int                                       myIndex       = -1;
  int                                       myKey         = -1;
  int                                       myNbPerformed = 0;
  occ::handle<BOPTools_ParallelTestContext> myContext;
};

typedef NCollection_DynamicArray<BOPTools_ParallelTestSolver> BOPTools_ParallelTestSolvers;

//! Fills theSolvers with theNbSmallGroups groups of 1 to 8 solvers with keys 0, 1, ...
//! and one group of theNbLarge solvers with the key theNbSmallGroups, mixed with the others.
void fillSolvers(BOPTools_ParallelTestSolvers& theSolvers,
                 const int                     theNbSmallGroups,
                 const int                     theNbLarge)
{
  NCollection_DynamicArray<int> aKeys;
  for (int aKey = 0; aKey < theNbSmallGroups; ++aKey)
  {
    for (int i = 0; i <= aKey % 8; ++i)
    {
      aKeys.Append(aKey);
    }
  }
  // Interleave the keys, so that the solvers of a group are not consecutive
  const int aNbSmall = aKeys.Length();
  for (int i = 0, j = 0; i < aNbSmall + theNbLarge; ++i)
  {
    const bool isLarge = (i % 3 == 0 && i / 3 < theNbLarge) || j == aNbSmall;
    const int  aKey    = isLarge ? theNbSmallGroups : aKeys((j++ * 7919) % aNbSmall);
    theSolvers.Appended().SetKey(i, aKey);
  }
}

//! Group-done callback recording the key in the context
struct BOPTools_ParallelTestOnGroupDone
{
  void operator()(const occ::handle<BOPTools_ParallelTestContext>& theContext,
                  const int                                        theKey) const
  {
    theContext->Records.Append(-1 - theKey);
  }
};

//! Collects the distinct contexts used by the solvers
NCollection_DynamicArray<occ::handle<BOPTools_ParallelTestContext>> usedContexts(
  const BOPTools_ParallelTestSolvers& theSolvers)
{
  NCollection_DynamicArray<occ::handle<BOPTools_ParallelTestContext>> aContexts;
  NCollection_Map<occ::handle<BOPTools_ParallelTestContext>>          aFence;
  for (const BOPTools_ParallelTestSolver& aSolver : theSolvers)
  {
    if (aFence.Add(aSolver.Context()))
    {
      aContexts.Append(aSolver.Context());
    }
  }
  return aContexts;
}
} // namespace

//=================================================================================================

TEST(BOPTools_ParallelTest, PerformGroupedByKey_Serial_RunsInOrderWithGivenContext)
{
  BOPTools_ParallelTestSolvers aSolvers;
  fillSolvers(aSolvers, 50, 100);
  occ::handle<BOPTools_ParallelTestContext> aContext = new BOPTools_ParallelTestContext();

  BOPTools_Parallel::PerformGroupedByKey(
    false,
    aSolvers,
    [&aSolvers](const int theIndex) { return aSolvers(theIndex).Key(); },
    BOPTools_ParallelTestOnGroupDone(),
    aContext);

  // All solvers once, in the order of their indices, with the given context and no group done
  ASSERT_EQ(aContext->Records.Length(), aSolvers.Length());
  for (int i = 0; i < aSolvers.Length(); ++i)
  {
    EXPECT_EQ(aSolvers(i).NbPerformed(), 1);
    EXPECT_EQ(aSolvers(i).Context(), aContext);
    EXPECT_EQ(aContext->Records(i), i);
  }
}

//=================================================================================================

TEST(BOPTools_ParallelTest, PerformGroupedByKey_Parallel_GroupsSolversByKey)
{
  if (OSD_Parallel::NbLogicalProcessors() < 2
      || (OSD_Parallel::ToUseOcctThreads() && OSD_ThreadPool::DefaultPool()->NbThreads() < 2))
  {
    GTEST_SKIP() << "At least two threads are needed to run in parallel.";
  }
  const int                    aNbSmallGroups = 1000;
  const int                    aNbLarge       = 3000;
  BOPTools_ParallelTestSolvers aSolvers;
  fillSolvers(aSolvers, aNbSmallGroups, aNbLarge);
  occ::handle<BOPTools_ParallelTestContext> aContext = new BOPTools_ParallelTestContext();

  BOPTools_Parallel::PerformGroupedByKey(
    true,
    aSolvers,
    [&aSolvers](const int theIndex) { return aSolvers(theIndex).Key(); },
    BOPTools_ParallelTestOnGroupDone(),
    aContext);

  // The context of the calling thread is not used
  EXPECT_TRUE(aContext->Records.IsEmpty());

  // Each solver is performed once
  for (const BOPTools_ParallelTestSolver& aSolver : aSolvers)
  {
    ASSERT_EQ(aSolver.NbPerformed(), 1);
    ASSERT_FALSE(aSolver.Context().IsNull());
    EXPECT_NE(aSolver.Context(), aContext);
  }

  // In each context, the solvers come in chunks of the same key, by increasing index,
  // each chunk followed by the call declaring its key done in the same context
  NCollection_DataMap<int, int> aNbChunks;
  int                           aNbSolvers = 0;
  for (const occ::handle<BOPTools_ParallelTestContext>& aCtx : usedContexts(aSolvers))
  {
    const NCollection_DynamicArray<int>& aRecords = aCtx->Records;
    ASSERT_FALSE(aRecords.IsEmpty());
    ASSERT_LT(aRecords.Last(), 0) << "a chunk is not declared done";
    for (int aFirst = 0; aFirst < aRecords.Length();)
    {
      int aLast = aFirst;
      while (aRecords(aLast) >= 0)
      {
        ++aLast;
      }
      ASSERT_GT(aLast, aFirst) << "a group is declared done without solvers";
      const int aKey = -1 - aRecords(aLast);
      for (int i = aFirst; i < aLast; ++i)
      {
        EXPECT_EQ(aSolvers(aRecords(i)).Key(), aKey);
        if (i > aFirst)
        {
          EXPECT_LT(aRecords(i - 1), aRecords(i));
        }
      }
      aNbSolvers += aLast - aFirst;
      if (int* aNb = aNbChunks.ChangeSeek(aKey))
      {
        ++(*aNb);
      }
      else
      {
        aNbChunks.Bind(aKey, 1);
      }
      aFirst = aLast + 1;
    }
  }
  EXPECT_EQ(aNbSolvers, aSolvers.Length());

  // The small groups are not cut (each one is far below the share of a thread),
  // while the group of a quarter of all solvers is shared by several threads.
  for (int aKey = 0; aKey <= aNbSmallGroups; ++aKey)
  {
    const int* aNb = aNbChunks.Seek(aKey);
    ASSERT_NE(aNb, nullptr) << "group " << aKey << " is never declared done";
    if (aKey < aNbSmallGroups)
    {
      EXPECT_EQ(*aNb, 1) << "group " << aKey;
    }
    else
    {
      EXPECT_GT(*aNb, 1);
    }
  }
}

//=================================================================================================

TEST(BOPTools_ParallelTest, PerformGroupedByKey_FewSolvers)
{
  for (int aNb = 0; aNb < 4; ++aNb)
  {
    BOPTools_ParallelTestSolvers aSolvers;
    for (int i = 0; i < aNb; ++i)
    {
      aSolvers.Appended().SetKey(i, 0);
    }
    occ::handle<BOPTools_ParallelTestContext> aContext = new BOPTools_ParallelTestContext();
    BOPTools_Parallel::PerformGroupedByKey(
      true,
      aSolvers,
      [&aSolvers](const int theIndex) { return aSolvers(theIndex).Key(); },
      BOPTools_ParallelTestOnGroupDone(),
      aContext);
    for (const BOPTools_ParallelTestSolver& aSolver : aSolvers)
    {
      EXPECT_EQ(aSolver.NbPerformed(), 1);
    }
  }
}
