// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <SelectMgr_BVHThreadPool.hxx>

#include <Message.hxx>
#include <OSD.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ErrorHandler.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_BVHThreadPool, Standard_Transient)

//=================================================================================================

SelectMgr_BVHThreadPool::SelectMgr_BVHThreadPool(int theNbThreads)
    : myToStopBVHThread(false),
      myWakeEvent(false),
      myIdleEvent(true),
      myIsStarted(false)
{
  int aBVHThreadsNum = std::max(1, theNbThreads);
  myBVHThreads.Resize(1, aBVHThreadsNum, false);

  bool toCatchFpe = OSD::ToCatchFloatingSignals();

  for (int i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
  {
    BVHThread& aThread = myBVHThreads.ChangeValue(i);
    aThread.SetFunction(&BVHThread::runThread);
    aThread.myPool       = this;
    aThread.myToCatchFpe = toCatchFpe;
  }
}

//=================================================================================================

SelectMgr_BVHThreadPool::~SelectMgr_BVHThreadPool()
{
  StopThreads();
}

//=================================================================================================

void SelectMgr_BVHThreadPool::StopThreads()
{
  if (!myIsStarted)
  {
    return;
  }
  myToStopBVHThread = true;
  myWakeEvent.Set();
  for (int i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
  {
    myBVHThreads.ChangeValue(i).Wait();
  }
  myToStopBVHThread = false;
  myIsStarted       = false;
}

//=================================================================================================

void SelectMgr_BVHThreadPool::WaitThreads()
{
  myIdleEvent.Wait();

  Sentry aSentry(this);
}

//=================================================================================================

void SelectMgr_BVHThreadPool::AddEntity(const occ::handle<Select3D_SensitiveEntity>& theEntity)
{
  if (!theEntity->ToBuildBVH())
  {
    return;
  }

  {
    std::lock_guard<std::mutex> aLock(myBVHListMutex);
    myBVHToBuildList.Append(theEntity);
    myWakeEvent.Set();
    myIdleEvent.Reset();
  }

  if (!myIsStarted)
  {
    myIsStarted = true;
    for (int i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
    {
      myBVHThreads.ChangeValue(i).Run((void*)(&myBVHThreads.ChangeValue(i)));
    }
  }
}

//=================================================================================================

void SelectMgr_BVHThreadPool::BVHThread::performThread()
{
  OSD::SetThreadLocalSignal(OSD::SignalMode(), myToCatchFpe);

  for (;;)
  {
    myPool->myWakeEvent.Wait();

    if (myPool->myToStopBVHThread)
    {
      return;
    }

    occ::handle<Select3D_SensitiveEntity> anEntity;
    {
      std::lock_guard<std::mutex> aListLock(myPool->myBVHListMutex);
      if (myPool->myBVHToBuildList.IsEmpty())
      {
        myPool->myWakeEvent.Reset();
        myPool->myIdleEvent.Set();
        continue;
      }
      anEntity = myPool->myBVHToBuildList.First();
      myPool->myBVHToBuildList.RemoveFirst();
    }

    // Lock thread mutex while building BVH
    std::lock_guard<std::mutex> aThreadLock(myMutex);

    if (!anEntity.IsNull())
    {
      try
      {
        OCC_CATCH_SIGNALS
        anEntity->BVH();
      }
      catch (Standard_Failure const& aFailure)
      {
        TCollection_AsciiString aMsg = TCollection_AsciiString(aFailure.DynamicType()->Name())
                                       + ": " + aFailure.GetMessageString();
        Message::DefaultMessenger()->SendFail(aMsg);
      }
      catch (std::exception& anStdException)
      {
        TCollection_AsciiString aMsg =
          TCollection_AsciiString(typeid(anStdException).name()) + ": " + anStdException.what();
        Message::DefaultMessenger()->SendFail(aMsg);
      }
      catch (...)
      {
        Message::DefaultMessenger()->SendFail("Error: Unknown exception");
      }
    }
  }
}

//=================================================================================================

void* SelectMgr_BVHThreadPool::BVHThread::runThread(void* theTask)
{
  BVHThread* aThread = static_cast<BVHThread*>(theTask);
  aThread->performThread();
  return NULL;
}
