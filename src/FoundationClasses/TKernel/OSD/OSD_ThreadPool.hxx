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

#ifndef _OSD_ThreadPool_HeaderFile
#define _OSD_ThreadPool_HeaderFile

#include <NCollection_Array1.hxx>
#include <OSD_Thread.hxx>
#include <Standard_Atomic.hxx>
#include <Standard_Condition.hxx>
#include <Standard_Mutex.hxx>

//! Class defining a thread pool for executing algorithms in multi-threaded mode.
//! Thread pool allocates requested amount of threads and keep them alive
//! (in sleep mode when unused) during thread pool lifetime.
//! The same pool can be used by multiple consumers,
//! including nested multi-threading algorithms and concurrent threads:
//! - Thread pool can be used either by multi-threaded algorithm by creating
//! OSD_ThreadPool::Launcher.
//!   The functor performing a job takes two parameters - Thread Index and Data Index:
//!     void operator(int theThreadIndex, int theDataIndex){}
//!   Multi-threaded algorithm may rely on Thread Index for allocating thread-local variables in
//!   array form, since the Thread Index is guaranteed to be within range OSD_ThreadPool::Lower()
//!   and OSD_ThreadPool::Upper().
//! - Default thread pool (OSD_ThreadPool::DefaultPool()) can be used in general case,
//!   but application may prefer creating a dedicated pool for better control.
//! - Default thread pool allocates the amount of threads considering concurrency
//!   level of the system (amount of logical processors).
//!   This can be overridden during OSD_ThreadPool construction or by calling OSD_ThreadPool::Init()
//!   (the pool should not be used!).
//! - OSD_ThreadPool::Launcher reserves specific amount of threads from the pool for executing
//! multi-threaded Job.
//!   Normally, single Launcher instance will occupy all threads available in thread pool,
//!   so that nested multi-threaded algorithms (within the same thread)
//!   and concurrent threads trying to use the same thread pool will run sequentially.
//!   This behavior is affected by OSD_ThreadPool::NbDefaultThreadsToLaunch() parameter
//!   and Launcher constructor, so that single Launcher instance will occupy not all threads
//!   in the pool allowing other threads to be used concurrently.
//! - OSD_ThreadPool::Launcher locks thread one-by-one from thread pool in a thread-safe way.
//! - Each working thread catches exceptions occurred during job execution, and Launcher will
//!   throw Standard_Failure in a caller thread on completed execution.
class OSD_ThreadPool : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OSD_ThreadPool, Standard_Transient)
public:
  //! Return (or create) a default thread pool.
  //! Number of threads argument will be considered only when called first time.
  Standard_EXPORT static const Handle(OSD_ThreadPool)& DefaultPool(int theNbThreads = -1);

public:
  //! Main constructor.
  //! Application may consider specifying more threads than actually
  //! available (OSD_Parallel::NbLogicalProcessors()) and set up NbDefaultThreadsToLaunch() to a
  //! smaller value so that concurrent threads will be able using single Thread Pool instance more
  //! efficiently.
  //! @param theNbThreads threads number to be created by pool
  //!                     (if -1 is specified then OSD_Parallel::NbLogicalProcessors() will be used)
  Standard_EXPORT OSD_ThreadPool(int theNbThreads = -1);

  //! Destructor.
  Standard_EXPORT virtual ~OSD_ThreadPool();

  //! Return TRUE if at least 2 threads are available (including self-thread).
  bool HasThreads() const { return NbThreads() >= 2; }

  //! Return the lower thread index.
  int LowerThreadIndex() const { return 0; }

  //! Return the upper thread index (last index is reserved for self-thread).
  int UpperThreadIndex() const { return LowerThreadIndex() + myThreads.Size(); }

  //! Return the number of threads; >= 1.
  int NbThreads() const { return myThreads.Size() + 1; }

  //! Return maximum number of threads to be locked by a single Launcher object by default;
  //! the entire thread pool size is returned by default.
  int NbDefaultThreadsToLaunch() const { return myNbDefThreads; }

  //! Set maximum number of threads to be locked by a single Launcher object by default.
  //! Should be set BEFORE first usage.
  void SetNbDefaultThreadsToLaunch(int theNbThreads) { myNbDefThreads = theNbThreads; }

  //! Checks if thread pools has active consumers.
  Standard_EXPORT bool IsInUse();

  //! Reinitialize the thread pool with a different number of threads.
  //! Should be called only with no active jobs, or exception Standard_ProgramError will be thrown!
  Standard_EXPORT void Init(int theNbThreads);

protected:
  //! Thread function interface.
  class JobInterface
  {
  public:
    virtual void Perform(int theThreadIndex) = 0;
  };

  //! Thread with back reference to thread pool and thread index in it.
  class EnumeratedThread : public OSD_Thread
  {
    friend class OSD_ThreadPool;

  public:
    //! Main constructor.
    EnumeratedThread(bool theIsSelfThread = false)
        : myPool(NULL),
          myJob(NULL),
          myWakeEvent(false),
          myIdleEvent(false),
          myThreadIndex(0),
          myUsageCounter(0),
          myIsStarted(false),
          myToCatchFpe(false),
          myIsSelfThread(theIsSelfThread)
    {
    }

    //! Occupy this thread for thread pool launcher.
    //! @return TRUE on success, or FALSE if thread has been already occupied
    Standard_EXPORT bool Lock();

    //! Release this thread for thread pool launcher; should be called only after successful
    //! OccupyThread().
    Standard_EXPORT void Free();

    //! Wake up the thread.
    Standard_EXPORT void WakeUp(JobInterface* theJob, bool theToCatchFpe);

    //! Wait the thread going into Idle state (finished jobs).
    Standard_EXPORT void WaitIdle();

  public:
    //! Copy constructor.
    EnumeratedThread(const EnumeratedThread& theCopy)
        : OSD_Thread(),
          myPool(NULL),
          myJob(NULL),
          myWakeEvent(false),
          myIdleEvent(false),
          myThreadIndex(0),
          myUsageCounter(0),
          myIsStarted(false),
          myToCatchFpe(false),
          myIsSelfThread(false)
    {
      Assign(theCopy);
    }

    //! Assignment operator.
    EnumeratedThread& operator=(const EnumeratedThread& theCopy)
    {
      Assign(theCopy);
      return *this;
    }

    //! Assignment operator.
    void Assign(const EnumeratedThread& theCopy)
    {
      OSD_Thread::Assign(theCopy);
      myPool         = theCopy.myPool;
      myJob          = theCopy.myJob;
      myThreadIndex  = theCopy.myThreadIndex;
      myToCatchFpe   = theCopy.myToCatchFpe;
      myIsSelfThread = theCopy.myIsSelfThread;
    }

  private:
    //! Method is executed in the context of thread.
    void performThread();

    //! Method is executed in the context of thread.
    static Standard_Address runThread(Standard_Address theTask);

  private:
    OSD_ThreadPool*          myPool;
    JobInterface*            myJob;
    Handle(Standard_Failure) myFailure;
    Standard_Condition       myWakeEvent;
    Standard_Condition       myIdleEvent;
    int                      myThreadIndex;
    volatile int             myUsageCounter;
    bool                     myIsStarted;
    bool                     myToCatchFpe;
    bool                     myIsSelfThread;
  };

public:
  //! Launcher object locking a subset of threads (or all threads)
  //! in a thread pool to perform parallel execution of the job.
  class Launcher
  {
  public:
    //! Lock specified number of threads from the thread pool.
    //! If thread pool is already locked by another user,
    //! Launcher will lock as many threads as possible
    //! (if none will be locked, then single threaded execution will be done).
    //! @param thePool       thread pool to lock the threads
    //! @param theMaxThreads number of threads to lock;
    //!                      -1 specifies that default number of threads
    //!                      to be used OSD_ThreadPool::NbDefaultThreadsToLaunch()
    Standard_EXPORT Launcher(OSD_ThreadPool& thePool, int theMaxThreads = -1);

    //! Release threads.
    ~Launcher() { Release(); }

    //! Return TRUE if at least 2 threads have been locked for parallel execution (including
    //! self-thread); otherwise, the functor will be executed within the caller thread.
    bool HasThreads() const { return myNbThreads >= 2; }

    //! Return amount of locked threads; >= 1.
    int NbThreads() const { return myNbThreads; }

    //! Return the lower thread index.
    int LowerThreadIndex() const { return 0; }

    //! Return the upper thread index (last index is reserved for the self-thread).
    int UpperThreadIndex() const { return LowerThreadIndex() + myNbThreads - 1; }

    //! Simple primitive for parallelization of "for" loops, e.g.:
    //! @code
    //!   for (int anIter = theBegin; anIter < theEnd; ++anIter) {}
    //! @endcode
    //! @param theBegin   the first data index (inclusive)
    //! @param theEnd     the last  data index (exclusive)
    //! @param theFunctor functor providing an interface
    //!                   "void operator(int theThreadIndex, int theDataIndex){}" performing task
    //!                   for specified index
    template <typename Functor>
    void Perform(int theBegin, int theEnd, const Functor& theFunctor)
    {
      JobRange     aData(theBegin, theEnd);
      Job<Functor> aJob(theFunctor, aData);
      perform(aJob);
    }

    //! Release threads before Launcher destruction.
    Standard_EXPORT void Release();

  protected:
    //! Execute job.
    Standard_EXPORT void perform(JobInterface& theJob);

    //! Initialize job and start threads.
    Standard_EXPORT void run(JobInterface& theJob);

    //! Wait threads execution.
    Standard_EXPORT void wait();

  private:
    Launcher(const Launcher& theCopy);
    Launcher& operator=(const Launcher& theCopy);

  private:
    // clang-format off
    NCollection_Array1<EnumeratedThread*> myThreads; //!< array of locked threads (including self-thread)
    // clang-format on
    EnumeratedThread mySelfThread;
    int              myNbThreads; //!< amount of locked threads
  };

protected:
  //! Auxiliary class which ensures exclusive access to iterators of processed data pool.
  class JobRange
  {
  public:
    //! Constructor
    JobRange(const int& theBegin, const int& theEnd)
        : myBegin(theBegin),
          myEnd(theEnd),
          myIt(theBegin)
    {
    }

    //! Returns const link on the first element.
    const int& Begin() const { return myBegin; }

    //! Returns const link on the last element.
    const int& End() const { return myEnd; }

    //! Returns first non processed element or end.
    //! Thread-safe method.
    int It() const { return Standard_Atomic_Increment(reinterpret_cast<volatile int*>(&myIt)) - 1; }

  private:
    JobRange(const JobRange& theCopy);
    JobRange& operator=(const JobRange& theCopy);

  private:
    const int&  myBegin; //!< First element of range
    const int&  myEnd;   //!< Last  element of range
    mutable int myIt;    //!< First non processed element of range
  };

  //! Auxiliary wrapper class for thread function.
  template <typename FunctorT>
  class Job : public JobInterface
  {
  public:
    //! Constructor.
    Job(const FunctorT& thePerformer, JobRange& theRange)
        : myPerformer(thePerformer),
          myRange(theRange)
    {
    }

    //! Method is executed in the context of thread.
    virtual void Perform(int theThreadIndex) Standard_OVERRIDE
    {
      for (Standard_Integer anIter = myRange.It(); anIter < myRange.End(); anIter = myRange.It())
      {
        myPerformer(theThreadIndex, anIter);
      }
    }

  private:
    Job(const Job& theCopy);
    Job& operator=(const Job& theCopy);

  private:                       //! @name private fields
    const FunctorT& myPerformer; //!< Link on functor
    const JobRange& myRange;     //!< Link on processed data block
  };

  //! Release threads.
  void release();

  //! Perform the job and catch exceptions.
  static void performJob(Handle(Standard_Failure)&     theFailure,
                         OSD_ThreadPool::JobInterface* theJob,
                         int                           theThreadIndex);

private:
  //! This method should not be called (prohibited).
  OSD_ThreadPool(const OSD_ThreadPool& theCopy);
  //! This method should not be called (prohibited).
  OSD_ThreadPool& operator=(const OSD_ThreadPool& theCopy);

private:
  // clang-format off
  NCollection_Array1<EnumeratedThread> myThreads; //!< array of defined threads (excluding self-thread)
  // clang-format on
  int  myNbDefThreads; //!< maximum number of threads to be locked by a single Launcher by default
  bool myShutDown;     //!< flag to shut down (destroy) the thread pool
};

#endif // _OSD_ThreadPool_HeaderFile
