// Created on: 2015-05-29
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <BVH_BuildThread.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BVH_BuildThread,Standard_Transient)

// =======================================================================
// function : BVH_BuildThread
// purpose  : Creates new BVH build thread
// =======================================================================
BVH_BuildThread::BVH_BuildThread (BVH_BuildTool&  theBuildTool,
                                  BVH_BuildQueue& theBuildQueue)
: myBuildTool  (theBuildTool),
  myBuildQueue (theBuildQueue),
  myWorkThread (threadFunction)
{
  //
}

// =======================================================================
// function : execute
// purpose  : Executes BVH build thread
// =======================================================================
void BVH_BuildThread::execute()
{
  for (Standard_Boolean wasBusy = Standard_False; /**/; /**/)
  {
    const Standard_Integer aNode = myBuildQueue.Fetch (wasBusy);

    if (aNode == -1) // queue is empty
    {
      if (!myBuildQueue.HasBusyThreads())
      {
        break; // no active threads
      }
    }
    else
    {
      myBuildTool.Perform (aNode);
    }
  }
}

// =======================================================================
// function : threadFunction
// purpose  : Thread function for BVH build thread
// =======================================================================
Standard_Address BVH_BuildThread::threadFunction (Standard_Address theData)
{
  static_cast<BVH_BuildThread*> (theData)->execute();

  return NULL;
}
