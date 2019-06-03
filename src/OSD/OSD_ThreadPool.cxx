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
#include <Standard_Atomic.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_ThreadPool, Standard_Transient)

// =======================================================================
// function : Lock
// purpose  :
// =======================================================================
bool OSD_ThreadPool::EnumeratedThread::Lock()
{
  return Standard_Atomic_CompareAndSwap (&myUsageCounter, 0, 1);
}

// =======================================================================
// function : Free
// purpose  :
// =======================================================================
void OSD_ThreadPool::EnumeratedThread::Free()
{
  Standard_Atomic_CompareAndSwap (&myUsageCounter, 1, 0);
}

// =======================================================================
// function : WakeUp
// purpose  :
// =======================================================================
void OSD_ThreadPool::EnumeratedThread::WakeUp (JobInterface* theJob, bool theToCatchFpe)
{
  myJob = theJob;
  myToCatchFpe = theToCatchFpe;
  if (myIsSelfThread)
  {
    if (theJob != NULL)
    {
      OSD_ThreadPool::performJob (myFailure, myJob, myThreadIndex);
    }
    return;
  }

  myWakeEvent.Set();
  if (theJob != NULL && !myIsStarted)
  {
    myIsStarted = true;
    Run (this);
  }
}

// =======================================================================
// function : WaitIdle
// purpose  :
// =======================================================================
void OSD_ThreadPool::EnumeratedThread::WaitIdle()
{
  if (!myIsSelfThread)
  {
    myIdleEvent.Wait();
    myIdleEvent.Reset();
  }
}

// =======================================================================
// function : DefaultPool
// purpose  :
// =======================================================================
const Handle(OSD_ThreadPool)& OSD_ThreadPool::DefaultPool (int theNbThreads)
{
  static const Handle(OSD_ThreadPool) THE_GLOBAL_POOL = new OSD_ThreadPool (theNbThreads);
  return THE_GLOBAL_POOL;
}

// =======================================================================
// function : OSD_ThreadPool
// purpose  :
// =======================================================================
OSD_ThreadPool::OSD_ThreadPool (int theNbThreads)
: myNbDefThreads (0),
  myShutDown (false)
{
  Init (theNbThreads);
  myNbDefThreads = NbThreads();
}

// =======================================================================
// function : IsInUse
// purpose  :
// =======================================================================
bool OSD_ThreadPool::IsInUse()
{
  for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter (myThreads);
       aThreadIter.More(); aThreadIter.Next())
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

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OSD_ThreadPool::Init (int theNbThreads)
{
  const int aNbThreads = Max (0, (theNbThreads > 0 ? theNbThreads : OSD_Parallel::NbLogicalProcessors()) - 1);
  if (myThreads.Size() == aNbThreads)
  {
    return;
  }

  // release old threads
  if (!myThreads.IsEmpty())
  {
    NCollection_Array1<EnumeratedThread*> aLockThreads (myThreads.Lower(), myThreads.Upper());
    aLockThreads.Init (NULL);
    int aThreadIndex = myThreads.Lower();
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter (myThreads);
         aThreadIter.More(); aThreadIter.Next())
    {
      EnumeratedThread& aThread = aThreadIter.ChangeValue();
      if (!aThread.Lock())
      {
        for (NCollection_Array1<EnumeratedThread*>::Iterator aLockThreadIter (aLockThreads);
             aLockThreadIter.More() && aLockThreadIter.Value() != NULL; aLockThreadIter.Next())
        {
          aLockThreadIter.ChangeValue()->Free();
        }
        throw Standard_ProgramError ("Error: active ThreadPool is reinitialized");
      }
      aLockThreads.SetValue (aThreadIndex++, &aThread);
    }
  }
  release();

  myShutDown = false;
  if (aNbThreads > 0)
  {
    myThreads.Resize (0, aNbThreads - 1, false);
    int aLastThreadIndex = 0;
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter (myThreads);
         aThreadIter.More(); aThreadIter.Next())
    {
      EnumeratedThread& aThread = aThreadIter.ChangeValue();
      aThread.myPool        = this;
      aThread.myThreadIndex = aLastThreadIndex++;
      aThread.SetFunction (&OSD_ThreadPool::EnumeratedThread::runThread);
    }
  }
  else
  {
    NCollection_Array1<EnumeratedThread> anEmpty;
    myThreads.Move (anEmpty);
  }
}

// =======================================================================
// function : ~OSD_ThreadPool
// purpose  :
// =======================================================================
OSD_ThreadPool::~OSD_ThreadPool()
{
  release();
}

// =======================================================================
// function : release
// purpose  :
// =======================================================================
void OSD_ThreadPool::release()
{
  if (myThreads.IsEmpty())
  {
    return;
  }

  myShutDown = true;
  for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter (myThreads);
       aThreadIter.More(); aThreadIter.Next())
  {
    aThreadIter.ChangeValue().WakeUp (NULL, false);
    aThreadIter.ChangeValue().Wait();
  }
}

// =======================================================================
// function : perform
// purpose  :
// =======================================================================
void OSD_ThreadPool::Launcher::perform (JobInterface& theJob)
{
  run (theJob);
  wait();
}

// =======================================================================
// function : run
// purpose  :
// =======================================================================
void OSD_ThreadPool::Launcher::run (JobInterface& theJob)
{
  bool toCatchFpe = OSD::ToCatchFloatingSignals();
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter (myThreads);
       aThreadIter.More() && aThreadIter.Value() != NULL; aThreadIter.Next())
  {
    aThreadIter.ChangeValue()->WakeUp (&theJob, toCatchFpe);
  }
}

// =======================================================================
// function : wait
// purpose  :
// =======================================================================
void OSD_ThreadPool::Launcher::wait()
{
  int aNbFailures = 0;
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter (myThreads);
       aThreadIter.More() && aThreadIter.Value() != NULL; aThreadIter.Next())
  {
    aThreadIter.ChangeValue()->WaitIdle();
    if (!aThreadIter.Value()->myFailure.IsNull())
    {
      ++aNbFailures;
    }
  }
  if (aNbFailures == 0)
  {
    return;
  }

  TCollection_AsciiString aFailures;
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter (myThreads);
       aThreadIter.More() && aThreadIter.Value() != NULL; aThreadIter.Next())
  {
    if (!aThreadIter.Value()->myFailure.IsNull())
    {
      if (aNbFailures == 1)
      {
        aThreadIter.Value()->myFailure->Reraise();
      }

      if (!aFailures.IsEmpty())
      {
        aFailures += "\n";
      }
      aFailures += aThreadIter.Value()->myFailure->GetMessageString();
    }
  }

  aFailures = TCollection_AsciiString("Multiple exceptions:\n") + aFailures;
  throw Standard_ProgramError (aFailures.ToCString(), NULL);
}

// =======================================================================
// function : performJob
// purpose  :
// =======================================================================
void OSD_ThreadPool::performJob (Handle(Standard_Failure)& theFailure,
                                 OSD_ThreadPool::JobInterface* theJob,
                                 int theThreadIndex)
{
  try
  {
    OCC_CATCH_SIGNALS
    theJob->Perform (theThreadIndex);
  }
  catch (Standard_Failure const& aFailure)
  {
    TCollection_AsciiString aMsg = TCollection_AsciiString (aFailure.DynamicType()->Name())
                                 + ": " + aFailure.GetMessageString();
    theFailure = new Standard_ProgramError (aMsg.ToCString(), aFailure.GetStackString());
  }
  catch (std::exception& anStdException)
  {
    TCollection_AsciiString aMsg = TCollection_AsciiString (typeid(anStdException).name())
                                 + ": " + anStdException.what();
    theFailure = new Standard_ProgramError (aMsg.ToCString(), NULL);
  }
  catch (...)
  {
    theFailure = new Standard_ProgramError ("Error: Unknown exception", NULL);
  }
}

// =======================================================================
// function : performThread
// purpose  :
// =======================================================================
void OSD_ThreadPool::EnumeratedThread::performThread()
{
  OSD::SetThreadLocalSignal (OSD::SignalMode(), false);
  for (;;)
  {
    myWakeEvent.Wait();
    myWakeEvent.Reset();
    if (myPool->myShutDown)
    {
      return;
    }

    myFailure.Nullify();
    if (myJob != NULL)
    {
      OSD::SetThreadLocalSignal (OSD::SignalMode(), myToCatchFpe);
      OSD_ThreadPool::performJob (myFailure, myJob, myThreadIndex);
      myJob = NULL;
    }
    myIdleEvent.Set();
  }
}

// =======================================================================
// function : runThread
// purpose  :
// =======================================================================
Standard_Address OSD_ThreadPool::EnumeratedThread::runThread (Standard_Address theTask)
{
  EnumeratedThread* aThread = static_cast<EnumeratedThread*>(theTask);
  aThread->performThread();
  return NULL;
}

// =======================================================================
// function : Launcher
// purpose  :
// =======================================================================
OSD_ThreadPool::Launcher::Launcher (OSD_ThreadPool& thePool, Standard_Integer theMaxThreads)
: mySelfThread (true),
  myNbThreads (0)
{
  const int aNbThreads = theMaxThreads > 0
                       ? Min (theMaxThreads, thePool.NbThreads())
                       : (theMaxThreads < 0
                        ? Max (thePool.NbDefaultThreadsToLaunch(), 1)
                        : 1);
  myThreads.Resize (0, aNbThreads - 1, false);
  myThreads.Init (NULL);
  if (aNbThreads > 1)
  {
    for (NCollection_Array1<EnumeratedThread>::Iterator aThreadIter (thePool.myThreads);
         aThreadIter.More(); aThreadIter.Next())
    {
      if (aThreadIter.ChangeValue().Lock())
      {
        myThreads.SetValue (myNbThreads, &aThreadIter.ChangeValue());
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
  myThreads.SetValue (myNbThreads, &mySelfThread);
  mySelfThread.myThreadIndex = myNbThreads;
  ++myNbThreads;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OSD_ThreadPool::Launcher::Release()
{
  for (NCollection_Array1<EnumeratedThread*>::Iterator aThreadIter (myThreads);
       aThreadIter.More() && aThreadIter.Value() != NULL; aThreadIter.Next())
  {
    if (aThreadIter.Value() != &mySelfThread)
    {
      aThreadIter.Value()->Free();
    }
  }

  NCollection_Array1<EnumeratedThread*> anEmpty;
  myThreads.Move (anEmpty);
  myNbThreads = 0;
}
