// Created on: 2015-05-28
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BVH_BuildQueue_Header
#define _BVH_BuildQueue_Header

#include <BVH_Builder.hxx>

#include <NCollection_Sequence.hxx>

#include <atomic>
#include <mutex>

//! Command-queue for parallel building of BVH nodes.
class BVH_BuildQueue
{
  template <class T, int N>
  friend class BVH_QueueBuilder;

public:
  //! Creates new BVH build queue.
  BVH_BuildQueue()
      : myNbThreads(0),
        mySize(0)
  {
  }

  //! Releases resources of BVH build queue.
  ~BVH_BuildQueue() = default;

public:
  //! Returns current size of BVH build queue.
  //! Uses acquire semantics to synchronize with enqueue/dequeue operations.
  Standard_Integer Size() const { return mySize.load(std::memory_order_acquire); }

  //! Enqueues new work-item onto BVH build queue.
  void Enqueue(const Standard_Integer theWorkItem)
  {
    std::lock_guard<std::mutex> aLock(myMutex);
    myQueue.Append(theWorkItem);
    mySize.fetch_add(1, std::memory_order_release);
  }

  //! Fetches first work-item from BVH build queue.
  Standard_Integer Fetch(Standard_Boolean& wasBusy)
  {
    Standard_Integer aQuery = -1;

    // Fetch item from queue under lock
    {
      std::lock_guard<std::mutex> aLock(myMutex);
      if (!myQueue.IsEmpty())
      {
        aQuery = myQueue.First();
        myQueue.Remove(1);
        mySize.fetch_sub(1, std::memory_order_release);
      }
    }

    // Update thread counter atomically with release/acquire semantics
    // to ensure proper synchronization with HasBusyThreads()
    if (aQuery != -1)
    {
      if (!wasBusy)
      {
        myNbThreads.fetch_add(1, std::memory_order_release);
      }
    }
    else if (wasBusy)
    {
      myNbThreads.fetch_sub(1, std::memory_order_release);
    }

    wasBusy = (aQuery != -1);
    return aQuery;
  }

  //! Checks if there are active build threads.
  //! Uses acquire semantics to ensure visibility of thread counter updates.
  //! This is critical for termination detection: threads check this after
  //! finding an empty queue to determine if they should exit or wait.
  Standard_Boolean HasBusyThreads() const
  {
    return myNbThreads.load(std::memory_order_acquire) != 0;
  }

private:
  //! Queue of BVH nodes to build.
  NCollection_Sequence<Standard_Integer> myQueue;

  //! Manages access serialization for queue operations.
  std::mutex myMutex;

  //! Number of active build threads (atomic for lock-free reads).
  std::atomic<Standard_Integer> myNbThreads;

  //! Current queue size (atomic for lock-free reads).
  std::atomic<Standard_Integer> mySize;
};

#endif // _BVH_BuildQueue_Header
