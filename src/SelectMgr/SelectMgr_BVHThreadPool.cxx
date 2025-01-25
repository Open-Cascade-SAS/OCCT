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

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_BVHThreadPool, Standard_Transient)

//=================================================================================================

SelectMgr_BVHThreadPool::SelectMgr_BVHThreadPool(Standard_Integer theNbThreads)
    : myToStopBVHThread(Standard_False),
      myWakeEvent(Standard_False),
      myIdleEvent(Standard_True),
      myIsStarted(Standard_False)
{
  Standard_Integer aBVHThreadsNum = Max(1, theNbThreads);
  myBVHThreads.Resize(1, aBVHThreadsNum, Standard_False);

  Standard_Boolean toCatchFpe = OSD::ToCatchFloatingSignals();

  for (Standard_Integer i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
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
  myToStopBVHThread = Standard_True;
  myWakeEvent.Set();
  for (Standard_Integer i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
  {
    myBVHThreads.ChangeValue(i).Wait();
  }
  myToStopBVHThread = Standard_False;
  myIsStarted       = Standard_False;
}

//=================================================================================================

void SelectMgr_BVHThreadPool::WaitThreads()
{
  myIdleEvent.Wait();

  Sentry aSentry(this);
}

//=================================================================================================

void SelectMgr_BVHThreadPool::AddEntity(const Handle(Select3D_SensitiveEntity)& theEntity)
{
  if (!theEntity->ToBuildBVH())
  {
    return;
  }

  {
    Standard_Mutex::Sentry aSentry(myBVHListMutex);
    myBVHToBuildList.Append(theEntity);
    myWakeEvent.Set();
    myIdleEvent.Reset();
  }

  if (!myIsStarted)
  {
    myIsStarted = Standard_True;
    for (Standard_Integer i = myBVHThreads.Lower(); i <= myBVHThreads.Upper(); ++i)
    {
      myBVHThreads.ChangeValue(i).Run((Standard_Address)(&myBVHThreads.ChangeValue(i)));
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

    myPool->myBVHListMutex.Lock();
    if (myPool->myBVHToBuildList.IsEmpty())
    {
      myPool->myWakeEvent.Reset();
      myPool->myIdleEvent.Set();
      myPool->myBVHListMutex.Unlock();
      continue;
    }
    Handle(Select3D_SensitiveEntity) anEntity = myPool->myBVHToBuildList.First();
    myPool->myBVHToBuildList.RemoveFirst();

    Standard_Mutex::Sentry anEntry(myMutex);
    myPool->myBVHListMutex.Unlock();

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

// =======================================================================
// function : runThread
// purpose  :
// =======================================================================
Standard_Address SelectMgr_BVHThreadPool::BVHThread::runThread(Standard_Address theTask)
{
  BVHThread* aThread = static_cast<BVHThread*>(theTask);
  aThread->performThread();
  return NULL;
}
