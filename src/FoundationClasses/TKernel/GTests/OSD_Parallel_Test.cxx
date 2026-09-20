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

#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <cmath>
#include <random>

namespace
{

// SAXPY functor: Y[i] = scalar * X[i] + Y[i]
class SaxpyFunctor
{
public:
  SaxpyFunctor(const NCollection_Array1<double>& theX,
               NCollection_Array1<double>&       theY,
               double                            theScalar)
      : myX(theX),
        myY(theY),
        myScalar(theScalar)
  {
  }

  int Begin() const { return 0; }

  int End() const { return myX.Length(); }

  void operator()(int theIndex) const { myY(theIndex) = myScalar * myX(theIndex) + myY(theIndex); }

  void operator()(int /*theThreadIndex*/, int theIndex) const { (*this)(theIndex); }

private:
  SaxpyFunctor(const SaxpyFunctor&)      = delete;
  SaxpyFunctor& operator=(SaxpyFunctor&) = delete;

  const NCollection_Array1<double>& myX;
  NCollection_Array1<double>&       myY;
  double                            myScalar;
};

// Batched SAXPY functor for large vectors
class SaxpyBatchFunctor
{
public:
  static const int THE_BATCH_SIZE = 100000;

  SaxpyBatchFunctor(const NCollection_Array1<double>& theX,
                    NCollection_Array1<double>&       theY,
                    double                            theScalar)
      : myX(theX),
        myY(theY),
        myScalar(theScalar),
        myNbBatches((int)std::ceil((double)theX.Size() / THE_BATCH_SIZE))
  {
  }

  int Begin() const { return 0; }

  int End() const { return myNbBatches; }

  void operator()(int theBatchIndex) const
  {
    const int aLower  = theBatchIndex * THE_BATCH_SIZE;
    const int anUpper = std::min(aLower + THE_BATCH_SIZE - 1, myX.Upper());
    for (int i = aLower; i <= anUpper; ++i)
    {
      myY(i) = myScalar * myX(i) + myY(i);
    }
  }

  void operator()(int /*theThreadIndex*/, int theBatchIndex) const { (*this)(theBatchIndex); }

private:
  SaxpyBatchFunctor(const SaxpyBatchFunctor&)      = delete;
  SaxpyBatchFunctor& operator=(SaxpyBatchFunctor&) = delete;

  const NCollection_Array1<double>& myX;
  NCollection_Array1<double>&       myY;
  double                            myScalar;
  int                               myNbBatches;
};

// Matrix multiplication functor: result(i,j) = sum_k mat1(i,k)*mat2(k,j)
class MatMultFunctor
{
public:
  MatMultFunctor(const NCollection_Array2<double>& theMat1,
                 const NCollection_Array2<double>& theMat2,
                 NCollection_Array2<double>&       theResult,
                 int                               theSize)
      : myMat1(theMat1),
        myMat2(theMat2),
        myResult(theResult),
        mySize(theSize)
  {
  }

  int Begin() const { return 0; }

  int End() const { return mySize; }

  void operator()(int theIndex) const
  {
    for (int j = 0; j < mySize; ++j)
    {
      double aTmp = 0.0;
      for (int k = 0; k < mySize; ++k)
      {
        aTmp += myMat1(theIndex, k) * myMat2(k, j);
      }
      myResult(theIndex, j) = aTmp;
    }
  }

  void operator()(int /*theThreadIndex*/, int theIndex) const { (*this)(theIndex); }

private:
  MatMultFunctor(const MatMultFunctor&)      = delete;
  MatMultFunctor& operator=(MatMultFunctor&) = delete;

  const NCollection_Array2<double>& myMat1;
  const NCollection_Array2<double>& myMat2;
  NCollection_Array2<double>&       myResult;
  int                               mySize;
};

//! Restore process-wide dispatch settings after each thread-limit test.
class OSD_ParallelLimitTest : public testing::Test
{
protected:
  //! Select the OCCT dispatcher and restrict its default launch to the caller.
  void SetUp() override;
  //! Restore the pool limit and dispatcher selected by the application.
  void TearDown() override;

private:
  int  myNbThreads = 1;
  bool myUseOcct   = false;
};

//=================================================================================================

void OSD_ParallelLimitTest::SetUp()
{
  myNbThreads = OSD_ThreadPool::DefaultPool()->NbDefaultThreadsToLaunch();
  myUseOcct   = OSD_Parallel::ToUseOcctThreads();
  OSD_ThreadPool::DefaultPool()->SetNbDefaultThreadsToLaunch(1);
  OSD_Parallel::SetUseOcctThreads(true);
}

//=================================================================================================

void OSD_ParallelLimitTest::TearDown()
{
  OSD_ThreadPool::DefaultPool()->SetNbDefaultThreadsToLaunch(myNbThreads);
  OSD_Parallel::SetUseOcctThreads(myUseOcct);
}

//=================================================================================================

//! Check reservations without depending on worker wake-up timing.
class ThreadReservationCheck
{
public:
  //! Borrow the pool and the shared test result.
  ThreadReservationCheck(OSD_ThreadPool& thePool, std::atomic<bool>& theIsRespected);
  //! A caller-only outer loop must leave every pool worker available.
  void operator()(const int) const;

private:
  OSD_ThreadPool&    myPool;
  std::atomic<bool>& myIsRespected;
};

//=================================================================================================

ThreadReservationCheck::ThreadReservationCheck(OSD_ThreadPool&    thePool,
                                               std::atomic<bool>& theIsRespected)
    : myPool(thePool),
      myIsRespected(theIsRespected)
{
}

//=================================================================================================

void ThreadReservationCheck::operator()(const int) const
{
  OSD_ThreadPool::Launcher aReservation(myPool, myPool.NbThreads());
  if (aReservation.NbThreads() != myPool.NbThreads())
  {
    myIsRespected.store(false, std::memory_order_relaxed);
  }
}

} // namespace

// Tests that OSD_Parallel::For and OSD_ThreadPool::Launcher produce the same
// result as sequential SAXPY Y[i] = scalar * X[i] + Y[i] (OCC24826).
TEST(OSD_ParallelTest, OCC24826_SaxpyParallelMatchesSequential)
{
  const int aLength = 500000;

  NCollection_Array1<double> aX(0, aLength - 1);
  NCollection_Array1<double> anYRef(0, aLength - 1);
  for (int i = 0; i < aLength; ++i)
  {
    aX(i) = anYRef(i) = static_cast<double>(i);
  }

  // Sequential reference
  {
    const SaxpyFunctor aFunctor(aX, anYRef, 1e-6);
    for (int i = 0; i < aLength; ++i)
    {
      aFunctor(i);
    }
  }

  // OSD_Parallel::For
  {
    NCollection_Array1<double> anY = aX;
    for (int i = 0; i < aLength; ++i)
    {
      anY(i) = static_cast<double>(i);
    }
    SaxpyFunctor aFunctor(aX, anY, 1e-6);
    OSD_Parallel::For(aFunctor.Begin(), aFunctor.End(), aFunctor);
    for (int i = 0; i < aLength; ++i)
    {
      EXPECT_DOUBLE_EQ(anY(i), anYRef(i)) << "Mismatch at index " << i;
    }
  }

  // OSD_ThreadPool::Launcher
  {
    NCollection_Array1<double> anY = aX;
    for (int i = 0; i < aLength; ++i)
    {
      anY(i) = static_cast<double>(i);
    }
    SaxpyFunctor             aFunctor(aX, anY, 1e-6);
    OSD_ThreadPool::Launcher aLauncher(*OSD_ThreadPool::DefaultPool());
    aLauncher.Perform(aFunctor.Begin(), aFunctor.End(), aFunctor);
    for (int i = 0; i < aLength; ++i)
    {
      EXPECT_DOUBLE_EQ(anY(i), anYRef(i)) << "Mismatch at index " << i;
    }
  }

  // OSD_Parallel::For with batched functor
  {
    NCollection_Array1<double> anY = aX;
    for (int i = 0; i < aLength; ++i)
    {
      anY(i) = static_cast<double>(i);
    }
    SaxpyBatchFunctor aFunctor(aX, anY, 1e-6);
    OSD_Parallel::For(aFunctor.Begin(), aFunctor.End(), aFunctor);
    for (int i = 0; i < aLength; ++i)
    {
      EXPECT_DOUBLE_EQ(anY(i), anYRef(i)) << "Mismatch at index " << i;
    }
  }
}

// Tests that OSD_Parallel::For and OSD_ThreadPool::Launcher produce the same
// result as sequential matrix multiplication (OCC29935).
TEST(OSD_ParallelTest, OCC29935_MatrixMultiplyParallelMatchesSequential)
{
  const int aSize = 50;

  std::mt19937               aGen(42);
  NCollection_Array2<double> aMat1(0, aSize - 1, 0, aSize - 1);
  NCollection_Array2<double> aMat2(0, aSize - 1, 0, aSize - 1);
  NCollection_Array2<double> aMatRef(0, aSize - 1, 0, aSize - 1);
  NCollection_Array2<double> aMatRes(0, aSize - 1, 0, aSize - 1);

  for (int i = 0; i < aSize; ++i)
  {
    for (int j = 0; j < aSize; ++j)
    {
      aMat1(i, j) = static_cast<double>(aGen() % 1000);
      aMat2(i, j) = static_cast<double>(aGen() % 1000);
    }
  }

  // Sequential reference
  {
    MatMultFunctor aFunctor(aMat1, aMat2, aMatRef, aSize);
    for (int i = aFunctor.Begin(); i < aFunctor.End(); ++i)
    {
      aFunctor(i);
    }
  }

  // OSD_Parallel::For
  {
    aMatRes.Init(0.0);
    MatMultFunctor aFunctor(aMat1, aMat2, aMatRes, aSize);
    OSD_Parallel::For(aFunctor.Begin(), aFunctor.End(), aFunctor);
    for (int i = 0; i < aSize; ++i)
    {
      for (int j = 0; j < aSize; ++j)
      {
        EXPECT_DOUBLE_EQ(aMatRes(i, j), aMatRef(i, j));
      }
    }
  }

  // OSD_ThreadPool::Launcher
  {
    aMatRes.Init(0.0);
    MatMultFunctor           aFunctor(aMat1, aMat2, aMatRes, aSize);
    OSD_ThreadPool::Launcher aLauncher(*OSD_ThreadPool::DefaultPool());
    aLauncher.Perform(aFunctor.Begin(), aFunctor.End(), aFunctor);
    for (int i = 0; i < aSize; ++i)
    {
      for (int j = 0; j < aSize; ++j)
      {
        EXPECT_DOUBLE_EQ(aMatRes(i, j), aMatRef(i, j));
      }
    }
  }
}

//=================================================================================================

TEST_F(OSD_ParallelLimitTest, IndexedLoopHonorsDefaultThreadLimit)
{
  const auto& aPool = OSD_ThreadPool::DefaultPool();
  if (aPool->NbThreads() < 2)
  {
    GTEST_SKIP() << "The pool has no worker thread to reserve.";
  }
  std::atomic<bool> aLimitRespected{true};
  OSD_Parallel::For(0, 2, ThreadReservationCheck(*aPool, aLimitRespected));
  EXPECT_TRUE(aLimitRespected.load(std::memory_order_relaxed));
}
