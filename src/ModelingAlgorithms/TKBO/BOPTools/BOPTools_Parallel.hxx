// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _BOPTools_Parallel_HeaderFile
#define _BOPTools_Parallel_HeaderFile

#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Thread.hxx>

#include <algorithm>
#include <mutex>
#include <shared_mutex>

//! Implementation of Functors/Starters
class BOPTools_Parallel
{
  template <class TypeSolverVector>
  class Functor
  {
  public:
    //! Constructor.
    explicit Functor(TypeSolverVector& theSolverVec)
        : mySolvers(theSolverVec)
    {
    }

    //! Defines functor interface.
    void operator()(const int theIndex) const
    {
      typename TypeSolverVector::value_type& aSolver = mySolvers[theIndex];
      aSolver.Perform();
    }

  private:
    Functor(const Functor&)            = delete;
    Functor& operator=(const Functor&) = delete;

  private:
    TypeSolverVector& mySolvers;
  };

  //! Functor storing map of thread id -> algorithm context
  template <class TypeSolverVector, class TypeContext>
  class ContextFunctor
  {
  public:
    //! Constructor
    explicit ContextFunctor(TypeSolverVector& theVector)
        : mySolverVector(theVector)
    {
    }

    //! Binds main thread context
    void SetContext(const opencascade::handle<TypeContext>& theContext)
    {
      std::unique_lock<std::shared_mutex> aLock(myMutex);
      myContextMap.Bind(OSD_Thread::Current(), theContext);
    }

    //! Returns current thread context
    const opencascade::handle<TypeContext>& GetThreadContext() const
    {
      const Standard_ThreadId aThreadID = OSD_Thread::Current();
      {
        // Threads may look up their contexts together, but not while another thread binds
        // its own one, as binding may resize the map.
        std::shared_lock<std::shared_mutex> aLock(myMutex);
        if (const opencascade::handle<TypeContext>* aContextPtr = myContextMap.Seek(aThreadID))
        {
          if (!aContextPtr->IsNull())
          {
            return *aContextPtr;
          }
        }
      }

      // Create new context. Only this thread binds a context to its id, and the returned
      // reference stays valid after the lock is released, as resizing the map does not
      // move its items.
      opencascade::handle<TypeContext> aContext =
        new TypeContext(NCollection_BaseAllocator::CommonBaseAllocator());
      std::unique_lock<std::shared_mutex> aLock(myMutex);
      myContextMap.Bind(aThreadID, aContext);
      return myContextMap(aThreadID);
    }

    //! Defines functor interface
    void operator()(const int theIndex) const
    {
      const opencascade::handle<TypeContext>& aContext = GetThreadContext();
      typename TypeSolverVector::value_type&  aSolver  = mySolverVector[theIndex];

      aSolver.SetContext(aContext);
      aSolver.Perform();
    }

  private:
    ContextFunctor(const ContextFunctor&)            = delete;
    ContextFunctor& operator=(const ContextFunctor&) = delete;

  private:
    TypeSolverVector&                                                                mySolverVector;
    mutable NCollection_DataMap<Standard_ThreadId, opencascade::handle<TypeContext>> myContextMap;
    mutable std::shared_mutex                                                        myMutex;
  };

  //! Functor storing array of algorithm contexts per thread in pool
  template <class TypeSolverVector, class TypeContext>
  class ContextFunctor2
  {
  public:
    //! Constructor
    explicit ContextFunctor2(TypeSolverVector&               theVector,
                             const OSD_ThreadPool::Launcher& thePoolLauncher)
        : mySolverVector(theVector),
          myContextArray(thePoolLauncher.LowerThreadIndex(), thePoolLauncher.UpperThreadIndex())
    {
    }

    //! Binds main thread context
    void SetContext(const opencascade::handle<TypeContext>& theContext)
    {
      // clang-format off
      myContextArray.ChangeLast() = theContext; // OSD_ThreadPool::Launcher::UpperThreadIndex() is reserved for a main thread
      // clang-format on
    }

    //! Defines functor interface with serialized thread index.
    void operator()(int theThreadIndex, int theIndex) const
    {
      opencascade::handle<TypeContext>& aContext = myContextArray.ChangeValue(theThreadIndex);
      if (aContext.IsNull())
      {
        aContext = new TypeContext(NCollection_BaseAllocator::CommonBaseAllocator());
      }
      typename TypeSolverVector::value_type& aSolver = mySolverVector[theIndex];
      aSolver.SetContext(aContext);
      aSolver.Perform();
    }

  private:
    ContextFunctor2(const ContextFunctor2&)            = delete;
    ContextFunctor2& operator=(const ContextFunctor2&) = delete;

  private:
    TypeSolverVector&                                            mySolverVector;
    mutable NCollection_Array1<opencascade::handle<TypeContext>> myContextArray;
  };

  //! Order in which PerformGroupedByKey() performs the solvers: the solvers sorted
  //! by their key, then by their index, and cut into chunks of solvers with the same key.
  class GroupedSchedule
  {
  public:
    //! Sorts the solvers by key and cuts the groups of solvers having the same key into chunks.
    //! A group is cut into several chunks only if it is larger than a quarter of the share
    //! of one of theNbThreads threads, so that the threads can share the work of a large group.
    //! The chunks are sorted by decreasing size, to start the longest work first.
    template <class TypeKeyOf>
    GroupedSchedule(const int theNbSolvers, const TypeKeyOf& theKeyOf, const int theNbThreads)
        : mySolvers(0, theNbSolvers - 1)
    {
      NCollection_Array1<int> aKeys(0, theNbSolvers - 1);
      for (int i = 0; i < theNbSolvers; ++i)
      {
        aKeys(i)     = theKeyOf(i);
        mySolvers(i) = i;
      }
      std::sort(mySolvers.begin(), mySolvers.end(), [&aKeys](const int theI1, const int theI2) {
        return aKeys(theI1) < aKeys(theI2) || (aKeys(theI1) == aKeys(theI2) && theI1 < theI2);
      });

      const int aNbShares  = 4 * std::max(theNbThreads, 1);
      const int aChunkSize = std::max(1, (theNbSolvers + aNbShares - 1) / aNbShares);

      // Number of chunks
      int aNbChunks = 0;
      for (int aFirst = 0; aFirst < theNbSolvers;)
      {
        const int aLast = groupEnd(aFirst, aKeys);
        aNbChunks += (aLast - aFirst + aChunkSize - 1) / aChunkSize;
        aFirst = aLast;
      }

      // Chunks in the order of the keys; a cut group gives chunks of almost equal sizes
      NCollection_Array1<int> aFirsts(0, aNbChunks - 1), aSizes(0, aNbChunks - 1),
        aChunkKeys(0, aNbChunks - 1);
      int aChunk = 0;
      for (int aFirst = 0; aFirst < theNbSolvers;)
      {
        const int aLast     = groupEnd(aFirst, aKeys);
        const int aSize     = aLast - aFirst;
        const int aNbPieces = (aSize + aChunkSize - 1) / aChunkSize;
        for (int aPiece = 0, aPos = aFirst; aPiece < aNbPieces; ++aPiece, ++aChunk)
        {
          aFirsts(aChunk)    = aPos;
          aSizes(aChunk)     = aSize / aNbPieces + (aPiece < aSize % aNbPieces ? 1 : 0);
          aChunkKeys(aChunk) = aKeys(mySolvers(aFirst));
          aPos += aSizes(aChunk);
        }
        aFirst = aLast;
      }

      // The largest chunks first
      NCollection_Array1<int> anOrder(0, aNbChunks - 1);
      for (int i = 0; i < aNbChunks; ++i)
      {
        anOrder(i) = i;
      }
      std::stable_sort(anOrder.begin(), anOrder.end(), [&aSizes](const int theC1, const int theC2) {
        return aSizes(theC1) > aSizes(theC2);
      });
      myChunkFirst.Resize(0, aNbChunks - 1, false);
      myChunkSize.Resize(0, aNbChunks - 1, false);
      myChunkKey.Resize(0, aNbChunks - 1, false);
      for (int i = 0; i < aNbChunks; ++i)
      {
        myChunkFirst(i) = aFirsts(anOrder(i));
        myChunkSize(i)  = aSizes(anOrder(i));
        myChunkKey(i)   = aChunkKeys(anOrder(i));
      }
    }

    //! Returns the number of chunks
    int NbChunks() const { return myChunkKey.Length(); }

    //! Performs the solvers of the chunk theChunk with the context theContext,
    //! then calls theOnGroupDone(theContext, key of the chunk).
    template <class TypeSolverVector, class TypeContext, class TypeOnGroupDone>
    void PerformChunk(const int                               theChunk,
                      TypeSolverVector&                       theSolverVector,
                      const opencascade::handle<TypeContext>& theContext,
                      const TypeOnGroupDone&                  theOnGroupDone) const
    {
      const int anEnd = myChunkFirst(theChunk) + myChunkSize(theChunk);
      for (int i = myChunkFirst(theChunk); i < anEnd; ++i)
      {
        typename TypeSolverVector::value_type& aSolver = theSolverVector[mySolvers(i)];
        aSolver.SetContext(theContext);
        aSolver.Perform();
      }
      theOnGroupDone(theContext, myChunkKey(theChunk));
    }

  private:
    //! Returns the position following the group of sorted solvers starting at theFirst
    int groupEnd(const int theFirst, const NCollection_Array1<int>& theKeys) const
    {
      const int aKey  = theKeys(mySolvers(theFirst));
      int       aLast = theFirst + 1;
      while (aLast < mySolvers.Length() && theKeys(mySolvers(aLast)) == aKey)
      {
        ++aLast;
      }
      return aLast;
    }

  private:
    NCollection_Array1<int> mySolvers;    //!< indices of the solvers sorted by key
    NCollection_Array1<int> myChunkFirst; //!< position in mySolvers of the first solver of chunks
    NCollection_Array1<int> myChunkSize;  //!< numbers of solvers of chunks
    NCollection_Array1<int> myChunkKey;   //!< keys of the solvers of chunks
  };

  //! Functor performing the chunks of a GroupedSchedule with an external threads library,
  //! using a new algorithm context in each thread.
  template <class TypeSolverVector, class TypeContext, class TypeOnGroupDone>
  class GroupedContextFunctor
  {
  public:
    //! Constructor
    GroupedContextFunctor(TypeSolverVector&      theVector,
                          const GroupedSchedule& theSchedule,
                          const TypeOnGroupDone& theOnGroupDone)
        : mySolverVector(theVector),
          mySchedule(theSchedule),
          myOnGroupDone(theOnGroupDone),
          myThreadContexts(theVector)
    {
    }

    //! Performs the chunk theChunk
    void operator()(const int theChunk) const
    {
      mySchedule.PerformChunk(theChunk,
                              mySolverVector,
                              myThreadContexts.GetThreadContext(),
                              myOnGroupDone);
    }

  private:
    GroupedContextFunctor(const GroupedContextFunctor&)            = delete;
    GroupedContextFunctor& operator=(const GroupedContextFunctor&) = delete;

  private:
    TypeSolverVector&                             mySolverVector;
    const GroupedSchedule&                        mySchedule;
    const TypeOnGroupDone&                        myOnGroupDone;
    ContextFunctor<TypeSolverVector, TypeContext> myThreadContexts; //!< contexts of the threads
  };

  //! Functor performing the chunks of a GroupedSchedule in the threads of the OCCT pool,
  //! using a new algorithm context in each thread.
  template <class TypeSolverVector, class TypeContext, class TypeOnGroupDone>
  class GroupedContextFunctor2
  {
  public:
    //! Constructor
    GroupedContextFunctor2(TypeSolverVector&               theVector,
                           const GroupedSchedule&          theSchedule,
                           const TypeOnGroupDone&          theOnGroupDone,
                           const OSD_ThreadPool::Launcher& thePoolLauncher)
        : mySolverVector(theVector),
          mySchedule(theSchedule),
          myOnGroupDone(theOnGroupDone),
          myContextArray(thePoolLauncher.LowerThreadIndex(), thePoolLauncher.UpperThreadIndex())
    {
    }

    //! Performs the chunk theChunk in the thread theThreadIndex
    void operator()(const int theThreadIndex, const int theChunk) const
    {
      opencascade::handle<TypeContext>& aContext = myContextArray.ChangeValue(theThreadIndex);
      if (aContext.IsNull())
      {
        aContext = new TypeContext(NCollection_BaseAllocator::CommonBaseAllocator());
      }
      mySchedule.PerformChunk(theChunk, mySolverVector, aContext, myOnGroupDone);
    }

  private:
    GroupedContextFunctor2(const GroupedContextFunctor2&)            = delete;
    GroupedContextFunctor2& operator=(const GroupedContextFunctor2&) = delete;

  private:
    TypeSolverVector&                                            mySolverVector;
    const GroupedSchedule&                                       mySchedule;
    const TypeOnGroupDone&                                       myOnGroupDone;
    mutable NCollection_Array1<opencascade::handle<TypeContext>> myContextArray;
  };

public:
  //! Pure version
  template <class TypeSolverVector>
  static void Perform(bool theIsRunParallel, TypeSolverVector& theSolverVector)
  {
    Functor<TypeSolverVector> aFunctor(theSolverVector);
    OSD_Parallel::For(0, theSolverVector.Length(), aFunctor, !theIsRunParallel);
  }

  //! Context dependent version
  template <class TypeSolverVector, class TypeContext>
  static void Perform(bool                              theIsRunParallel,
                      TypeSolverVector&                 theSolverVector,
                      opencascade::handle<TypeContext>& theContext)
  {
    if (OSD_Parallel::ToUseOcctThreads())
    {
      const occ::handle<OSD_ThreadPool>&             aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher                       aPoolLauncher(*aThreadPool,
                                             theIsRunParallel ? theSolverVector.Length() : 0);
      ContextFunctor2<TypeSolverVector, TypeContext> aFunctor(theSolverVector, aPoolLauncher);
      aFunctor.SetContext(theContext);
      aPoolLauncher.Perform(0, theSolverVector.Length(), aFunctor);
    }
    else
    {
      ContextFunctor<TypeSolverVector, TypeContext> aFunctor(theSolverVector);
      aFunctor.SetContext(theContext);
      OSD_Parallel::For(0, theSolverVector.Length(), aFunctor, !theIsRunParallel);
    }
  }

  //! Context dependent version for solvers sharing data cached in the context,
  //! e.g. the intersections of many edges with the same face. Running such solvers
  //! in any order makes each thread context cache the data of almost every shape.
  //! Here, the solvers are grouped by an integer key, e.g. the index of the face:
  //! - the solvers with the same key are performed by one thread, one after another
  //!   in the order of their indices, so that the data of the key is cached once
  //!   instead of once per thread (a group larger than a quarter of the share of one
  //!   thread is cut into chunks of almost equal sizes given to different threads;
  //!   the largest chunks are performed first);
  //! - after performing a chunk of solvers with the key theKey, the thread calls
  //!   theOnGroupDone(aContext, theKey) with its context, e.g. to release the data
  //!   cached for the key. The calls are concurrent;
  //! - each thread, including the calling one, uses a new context,
  //!   so the parallel run leaves theContext unchanged.
  //! Without parallel run, or if less than two threads are available, the solvers are
  //! performed as by the context dependent Perform() with theContext, in the order of
  //! their indices, and theOnGroupDone is not called.
  //! @param[in] theIsRunParallel defines whether to run in parallel
  //! @param[in] theSolverVector  the solvers
  //! @param[in] theKeyOf         functor "int (int theSolverIndex)" returning the key of a solver
  //! @param[in] theOnGroupDone   functor "void (const opencascade::handle<TypeContext>& theContext,
  //!                             int theKey)" called after performing a chunk of solvers
  //! @param[in] theContext       context of the calling thread, used if not run in parallel
  template <class TypeSolverVector, class TypeContext, class TypeKeyOf, class TypeOnGroupDone>
  static void PerformGroupedByKey(bool                              theIsRunParallel,
                                  TypeSolverVector&                 theSolverVector,
                                  const TypeKeyOf&                  theKeyOf,
                                  const TypeOnGroupDone&            theOnGroupDone,
                                  opencascade::handle<TypeContext>& theContext)
  {
    const int aNbSolvers = theSolverVector.Length();
    if (!theIsRunParallel || aNbSolvers < 2)
    {
      Perform(theIsRunParallel, theSolverVector, theContext);
      return;
    }

    if (OSD_Parallel::ToUseOcctThreads())
    {
      const occ::handle<OSD_ThreadPool>& aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher           aPoolLauncher(*aThreadPool, aNbSolvers);
      if (!aPoolLauncher.HasThreads())
      {
        // e.g. the threads of the pool are used by an enclosing parallel job
        ContextFunctor2<TypeSolverVector, TypeContext> aFunctor(theSolverVector, aPoolLauncher);
        aFunctor.SetContext(theContext);
        aPoolLauncher.Perform(0, aNbSolvers, aFunctor);
        return;
      }
      const GroupedSchedule aSchedule(aNbSolvers, theKeyOf, aPoolLauncher.NbThreads());
      GroupedContextFunctor2<TypeSolverVector, TypeContext, TypeOnGroupDone> aFunctor(
        theSolverVector,
        aSchedule,
        theOnGroupDone,
        aPoolLauncher);
      aPoolLauncher.Perform(0, aSchedule.NbChunks(), aFunctor);
    }
    else
    {
      const int aNbThreads = OSD_Parallel::NbLogicalProcessors();
      if (aNbThreads < 2)
      {
        Perform(theIsRunParallel, theSolverVector, theContext);
        return;
      }
      const GroupedSchedule aSchedule(aNbSolvers, theKeyOf, aNbThreads);
      GroupedContextFunctor<TypeSolverVector, TypeContext, TypeOnGroupDone> aFunctor(
        theSolverVector,
        aSchedule,
        theOnGroupDone);
      OSD_Parallel::For(0, aSchedule.NbChunks(), aFunctor);
    }
  }
};

#endif
