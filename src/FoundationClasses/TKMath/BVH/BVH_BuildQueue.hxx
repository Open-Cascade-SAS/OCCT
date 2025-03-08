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

#include <Standard_Mutex.hxx>
#include <NCollection_Sequence.hxx>

//! Command-queue for parallel building of BVH nodes.
class BVH_BuildQueue
{
  template <class T, int N>
  friend class BVH_QueueBuilder;

public:
  //! Creates new BVH build queue.
  BVH_BuildQueue()
      : myNbThreads(0)
  {
    //
  }

  //! Releases resources of BVH build queue.
  ~BVH_BuildQueue()
  {
    //
  }

public:
  //! Returns current size of BVH build queue.
  Standard_EXPORT Standard_Integer Size();

  //! Enqueues new work-item onto BVH build queue.
  Standard_EXPORT void Enqueue(const Standard_Integer& theNode);

  //! Fetches first work-item from BVH build queue.
  Standard_EXPORT Standard_Integer Fetch(Standard_Boolean& wasBusy);

  //! Checks if there are active build threads.
  Standard_Boolean HasBusyThreads() { return myNbThreads != 0; }

protected:
  //! Queue of BVH nodes to build.
  NCollection_Sequence<Standard_Integer> myQueue;

protected:
  //! Manages access serialization of working threads.
  Standard_Mutex myMutex;

  //! Number of active build threads.
  Standard_Integer myNbThreads;
};

#endif // _BVH_BuildQueue_Header
