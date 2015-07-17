// Created on: 2015-05-27
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

#include <BVH_BuildQueue.hxx>

// =======================================================================
// function : Size
// purpose  : Returns current size of BVH build queue
// =======================================================================
Standard_Integer BVH_BuildQueue::Size()
{
  Standard_Integer aSize;

  myMutex.Lock();
  {
    aSize = myQueue.Size();
  }
  myMutex.Unlock();

  return aSize;
}

// =======================================================================
// function : Enqueue
// purpose  : Enqueues new work-item onto BVH build queue
// =======================================================================
void BVH_BuildQueue::Enqueue (const Standard_Integer& theWorkItem)
{
  myMutex.Lock();
  {
    myQueue.Append (theWorkItem);
  }
  myMutex.Unlock();
}

// =======================================================================
// function : Fetch
// purpose  : Fetches first work-item from BVH build queue
// =======================================================================
Standard_Integer BVH_BuildQueue::Fetch (Standard_Boolean& wasBusy)
{
  Standard_Integer aQuery = -1;
  {
    Standard_Mutex::Sentry aSentry (myMutex);

    if (!myQueue.IsEmpty())
    {
      aQuery = myQueue.First();

      myQueue.Remove (1); // remove item from queue
    }

    if (aQuery != -1)
    {
      if (!wasBusy)
      {
        ++myNbThreads;
      }
    }
    else if (wasBusy)
    {
      --myNbThreads;
    }

    wasBusy = aQuery != -1;
  }

  return aQuery;
}
