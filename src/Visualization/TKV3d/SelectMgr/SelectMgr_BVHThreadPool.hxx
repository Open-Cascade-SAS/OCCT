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

#ifndef _SelectMgr_BVHThreadPool_HeaderFile
#define _SelectMgr_BVHThreadPool_HeaderFile

#include <Standard_Transient.hxx>
#include <OSD_Thread.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Standard_Condition.hxx>
#include <Message_Messenger.hxx>

#include <mutex>

//! Class defining a thread pool for building BVH for the list of Select3D_SensitiveEntity within
//! background thread(s).
class SelectMgr_BVHThreadPool : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_BVHThreadPool, Standard_Transient)
public:
  //! Main constructor
  Standard_EXPORT SelectMgr_BVHThreadPool(int theNbThreads);

  //! Destructor
  Standard_EXPORT ~SelectMgr_BVHThreadPool() override;

public:
  //! Thread with back reference to thread pool and thread mutex in it.
  class BVHThread : public OSD_Thread
  {
    friend class SelectMgr_BVHThreadPool;

  public:
    BVHThread()
        : myPool(nullptr),

          myToCatchFpe(false)
    {
    }

    BVHThread(const BVHThread& theOther)
        : OSD_Thread(theOther),
          myPool(theOther.myPool),

          myToCatchFpe(theOther.myToCatchFpe)
    {
    }

    //! Returns mutex used for BVH building
    std::mutex& BVHMutex() { return myMutex; }

    //! Assignment operator.
    BVHThread& operator=(const BVHThread& theCopy)
    {
      Assign(theCopy);
      return *this;
    }

    //! Assignment operator.
    void Assign(const BVHThread& theCopy)
    {
      OSD_Thread::Assign(theCopy);
      myPool       = theCopy.myPool;
      myToCatchFpe = theCopy.myToCatchFpe;
    }

  private:
    //! Method is executed in the context of thread.
    void performThread();

    //! Method is executed in the context of thread.
    static void* runThread(void* theTask);

  private:
    SelectMgr_BVHThreadPool* myPool;
    std::mutex               myMutex;
    bool                     myToCatchFpe;
  };

public:
  //! Queue a sensitive entity to build its BVH
  Standard_EXPORT void AddEntity(const occ::handle<Select3D_SensitiveEntity>& theEntity);

  //! Stops threads
  Standard_EXPORT void StopThreads();

  //! Waits for all threads finish their jobs
  Standard_EXPORT void WaitThreads();

  //! Returns array of threads
  NCollection_Array1<BVHThread>& Threads() { return myBVHThreads; }

public:
  //! Class providing a simple interface to mutexes for list of BVHThread
  class Sentry
  {
  public:
    //! Constructor - initializes the sentry object and locks list of mutexes immediately
    Sentry(const occ::handle<SelectMgr_BVHThreadPool>& thePool)
        : myPool(thePool)
    {
      Lock();
    }

    //! Destructor - unlocks list of mutexes if already locked.
    ~Sentry() { Unlock(); }

    //! Lock list of mutexes
    void Lock()
    {
      if (!myPool.IsNull())
      {
        for (int i = myPool->Threads().Lower(); i <= myPool->Threads().Upper(); ++i)
        {
          myPool->Threads().ChangeValue(i).BVHMutex().lock();
        }
      }
    }

    //! Unlock list of mutexes
    void Unlock()
    {
      if (!myPool.IsNull())
      {
        for (int i = myPool->Threads().Lower(); i <= myPool->Threads().Upper(); ++i)
        {
          myPool->Threads().ChangeValue(i).BVHMutex().unlock();
        }
      }
    }

    //! This method should not be called (prohibited).
    Sentry(const Sentry&);
    //! This method should not be called (prohibited).
    Sentry& operator=(const Sentry&);

  private:
    occ::handle<SelectMgr_BVHThreadPool> myPool;
  };

protected:
  // clang-format off
  NCollection_List<occ::handle<Select3D_SensitiveEntity>> myBVHToBuildList; //!< list of queued sensitive entities
  NCollection_Array1<BVHThread> myBVHThreads;                          //!< threads to build BVH
  bool myToStopBVHThread;                                  //!< flag to stop BVH threads
  std::mutex myBVHListMutex;                                           //!< mutex for interaction with myBVHToBuildList
  Standard_Condition myWakeEvent;                                      //!< raises when any sensitive is added to the BVH list
  Standard_Condition myIdleEvent;                                      //!< raises when BVH list become empty
  bool myIsStarted;                                        //!< indicates that threads are running
  // clang-format on
};

#endif
