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

#ifndef _BVH_BuildThread_Header
#define _BVH_BuildThread_Header

#include <OSD_Thread.hxx>
#include <BVH_BuildQueue.hxx>

//! Tool object to call BVH builder subroutines.
struct BVH_BuildTool
{
  //! Performs splitting of the given BVH node.
  virtual void Perform (const Standard_Integer theNode) = 0;
};

//! Wrapper for BVH build thread.
class BVH_BuildThread : public Standard_Transient
{
  template <class T, int N> friend class BVH_QueueBuilder;

public:

  //! Creates new BVH build thread.
  Standard_EXPORT BVH_BuildThread (BVH_BuildTool& theBuildTool, BVH_BuildQueue& theBuildQueue);

  //! Starts execution of BVH build thread.
  void Run()
  {
    myWorkThread.Run (this);
  }

  //! Waits till the thread finishes execution.
  void Wait()
  {
    myWorkThread.Wait();
  }

protected:

  //! Executes BVH build thread.
  Standard_EXPORT void execute();

  //! Thread function for BVH build thread.
  static Standard_Address threadFunction (Standard_Address theData);

  //! Assignment operator (to remove VC compile warning).
  BVH_BuildThread& operator= (const BVH_BuildThread&);

protected:

  //! Data needed to build the BVH.
  BVH_BuildTool& myBuildTool;

  //! Reference to BVH build queue.
  BVH_BuildQueue& myBuildQueue;

  //! Thread to execute work items.
  OSD_Thread myWorkThread;

public:

  DEFINE_STANDARD_RTTIEXT(BVH_BuildThread,Standard_Transient)
};

DEFINE_STANDARD_HANDLE (BVH_BuildThread, Standard_Transient)

#endif // _BVH_BuildThread_Header
