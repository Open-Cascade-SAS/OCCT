// Created by: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <OSD_ThreadPool.hxx>

#include <OSD.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_ThreadPool, Standard_Transient)

//=================================================================================================

bool OSD_ThreadPool::EnumeratedThread::Lock()
{
  return myUsageCounter.exchange(1) == 0;
}

//=================================================================================================

void OSD_ThreadPool::EnumeratedThread::Free()
{
  myUsageCounter.store(0);
}

//=================================================================================================

void OSD_ThreadPool::EnumeratedThread::WakeUp(JobInterface* theJob, bool theToCatchFpe)
{
  myJob        = theJob;
  myToCatchFpe = theToCatchFpe;
  if (myIsSelfThread)
  {
    if (theJob != nullptr)
    {
      OSD_ThreadPool::performJob(myFailure, myJob, myThreadIndex);
    }
    return;
  }

  myWakeEvent.Set();
  if (theJob != nullptr && !myIsStarted)
  {
    myIsStarted = true;
    Run(this);
  }
}

//=================================================================================================

void OSD_ThreadPool::EnumeratedThread::WaitIdle()
{
  if (!myIsSelfThread)
  {
    myIdleEvent.Wait();
    myIdleEvent.Reset();
  }
}

//=================================================================================================

const occ::handle<OSD_ThreadPool>& OSD_ThreadPool::DefaultPool(int theNbThreads)
{
  static const occ::handle<OSD_ThreadPool> THE_GLOBAL_POOL = new OSD_ThreadPool(theNbThreads);
  return THE_GLOBAL_POOL;
}

//=================================================================================================

OSD_ThreadPool::OSD_ThreadPool(int theNbThreads)
    : myNbDefThreads(0),
      myShutDown(false)
{
  Init(theNbThreads);
  myNbDefThreads = NbThreads();
}

//=================================================================================================

bool OSD_ThreadPool::IsInUse()
{
  for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter(myThreads); aThreadIter.More();
       aThreadIter.Next())
  {
    EnumeratedThread& aThread = aThreadIter.ChangeValue();
    if (!aThread.Lock())
    {
      return true;
    }
    aThread.Free();
  }
  return false;
}

//=================================================================================================

void OSD_ThreadPool::Init(int theNbThreads)
{
  const int aNbThreads =
    std::max(0, (theNbThreads > 0 ? theNbThreads : OSD_Parallel::NbLogicalProcessors()) - 1);
  if (myThreads.Size() == aNbThreads)
  {
    return;
  }

  // release old threads
  if (!myThreads.IsEmpty())
  {
    NCollection_Array1<EnumeratedThread*> aLockThreads(myThreads.Lower(), myThreads.Upper());
    aLockThreads.Init(nullptr);
    int aThreadIndex = myThreads.Lower();
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter(myThreads); aThreadIter.More();
         aThreadIter.Next())
    {
      EnumeratedThread& aThread = aThreadIter.ChangeValue();
      if (!aThread.Lock())
      {
        for (NCollection_Array1<EnumeratedThread*>::Iterator aLockThreadIter(aLockThreads);
             aLockThreadIter.More() && aLockThreadIter.Value() != nullptr;
             aLockThreadIter.Next())
        {
          aLockThreadIter.ChangeValue()->Free();
        }
        throw Standard_ProgramError("Error: active ThreadPool is reinitialized");
      }
      aLockThreads.SetValue(aThreadIndex++, &aThread);
    }
  }
  release();

  myShutDown = false;
  if (aNbThreads > 0)
  {
    myThreads.Resize(0, aNbThreads - 1, false);
    int aLastThreadIndex = 0;
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter(myThreads); aThreadIter.More();
         aThreadIter.Next())
    {
      EnumeratedThread& aThread = aThreadIter.ChangeValue();
      aThread.myPool            = this;
      aThread.myThreadIndex     = aLastThreadIndex++;
      aThread.SetFunction(&OSD_ThreadPool::EnumeratedThread::runThread);
    }
  }
  else
  {
    NCollection_Array1<EnumeratedThread> anEmpty;
    myThreads.Move(anEmpty);
  }
}

//=================================================================================================

OSD_ThreadPool::~OSD_ThreadPool()
{
  release();
}

//=================================================================================================

void OSD_ThreadPool::release()
{
  if (myThreads.IsEmpty())
  {
    return;
  }

  myShutDown = true;
  for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter(myThreads); aThreadIter.More();
       aThreadIter.Next())
  {
    aThreadIter.ChangeValue().WakeUp(nullptr, false);
    aThreadIter.ChangeValue().Wait();
  }
}

//=================================================================================================

void OSD_ThreadPool::Launcher::perform(JobInterface& theJob)
{
  run(theJob);
  wait();
}

//=================================================================================================

void OSD_ThreadPool::Launcher::run(JobInterface& theJob)
{
  bool toCatchFpe = OSD::ToCatchFloatingSignals();
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter(myThreads);
       aThreadIter.More() && aThreadIter.Value() != nullptr;
       aThreadIter.Next())
  {
    aThreadIter.ChangeValue()->WakeUp(&theJob, toCatchFpe);
  }
}

//=================================================================================================

void OSD_ThreadPool::Launcher::wait()
{
  int aNbFailures = 0;
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter(myThreads);
       aThreadIter.More() && aThreadIter.Value() != nullptr;
       aThreadIter.Next())
  {
    aThreadIter.ChangeValue()->WaitIdle();
    if (aThreadIter.Value()->myFailure)
    {
      ++aNbFailures;
    }
  }
  if (aNbFailures == 0)
  {
    return;
  }

  TCollection_AsciiString aFailures;
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter(myThreads);
       aThreadIter.More() && aThreadIter.Value() != nullptr;
       aThreadIter.Next())
  {
    if (aThreadIter.Value()->myFailure)
    {
      if (aNbFailures == 1)
      {
        // Re-throw the single exception directly
        throw *aThreadIter.Value()->myFailure;
      }

      if (!aFailures.IsEmpty())
      {
        aFailures += "\n";
      }
      aFailures += aThreadIter.Value()->myFailure->what();
    }
  }

  aFailures = TCollection_AsciiString("Multiple exceptions:\n") + aFailures;
  throw Standard_ProgramError(aFailures.ToCString(), nullptr);
}

//=================================================================================================

void OSD_ThreadPool::performJob(std::optional<Standard_ProgramError>& theFailure,
                                OSD_ThreadPool::JobInterface*         theJob,
                                int                                   theThreadIndex)
{
  try
  {
    OCC_CATCH_SIGNALS
    theJob->Perform(theThreadIndex);
  }
  catch (Standard_Failure const& aFailure)
  {
    TCollection_AsciiString aMsg =
      TCollection_AsciiString(aFailure.ExceptionType()) + ": " + aFailure.what();
    theFailure.emplace(aMsg.ToCString(), aFailure.GetStackString());
  }
  catch (std::exception& anStdException)
  {
    TCollection_AsciiString aMsg =
      TCollection_AsciiString(typeid(anStdException).name()) + ": " + anStdException.what();
    theFailure.emplace(aMsg.ToCString(), nullptr);
  }
  catch (...)
  {
    theFailure.emplace("Error: Unknown exception", nullptr);
  }
}

//=================================================================================================

void OSD_ThreadPool::EnumeratedThread::performThread()
{
  OSD::SetThreadLocalSignal(OSD::SignalMode(), false);
  for (;;)
  {
    myWakeEvent.Wait();
    myWakeEvent.Reset();
    if (myPool->myShutDown)
    {
      return;
    }

    myFailure.reset();
    if (myJob != nullptr)
    {
      OSD::SetThreadLocalSignal(OSD::SignalMode(), myToCatchFpe);
      OSD_ThreadPool::performJob(myFailure, myJob, myThreadIndex);
      myJob = nullptr;
    }
    myIdleEvent.Set();
  }
}

//=================================================================================================

void* OSD_ThreadPool::EnumeratedThread::runThread(void* theTask)
{
  EnumeratedThread* aThread = static_cast<EnumeratedThread*>(theTask);
  aThread->performThread();
  return nullptr;
}

//=================================================================================================

OSD_ThreadPool::Launcher::Launcher(OSD_ThreadPool& thePool, int theMaxThreads)
    : mySelfThread(true),
      myNbThreads(0)
{
  const int aNbThreads =
    theMaxThreads > 0 ? std::min(theMaxThreads, thePool.NbThreads())
                      : (theMaxThreads < 0 ? std::max(thePool.NbDefaultThreadsToLaunch(), 1) : 1);
  myThreads.Resize(0, aNbThreads - 1, false);
  myThreads.Init(nullptr);
  if (aNbThreads > 1)
  {
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter(thePool.myThreads);
         aThreadIter.More();
         aThreadIter.Next())
    {
      if (aThreadIter.ChangeValue().Lock())
      {
        myThreads.SetValue(myNbThreads, &aThreadIter.ChangeValue());
        // make thread index to fit into myThreads range
        aThreadIter.ChangeValue().myThreadIndex = myNbThreads;
        if (++myNbThreads == aNbThreads - 1)
        {
          break;
        }
      }
    }
  }

  // self thread should be executed last
  myThreads.SetValue(myNbThreads, &mySelfThread);
  mySelfThread.myThreadIndex = myNbThreads;
  ++myNbThreads;
}

//=================================================================================================

void OSD_ThreadPool::Launcher::Release()
{
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter(myThreads);
       aThreadIter.More() && aThreadIter.Value() != nullptr;
       aThreadIter.Next())
  {
    if (aThreadIter.Value() != &mySelfThread)
    {
      aThreadIter.Value()->Free();
    }
  }

  NCollection_Array1<EnumeratedThread*> anEmpty;
  myThreads.Move(anEmpty);
  myNbThreads = 0;
}
