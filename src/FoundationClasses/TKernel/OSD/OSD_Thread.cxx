// Created on: 2006-04-12
// Created by: Andrey BETENEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#include <OSD_Thread.hxx>

//=============================================
// OSD_Thread::OSD_Thread
//=============================================
OSD_Thread::OSD_Thread()
    : myFunc(nullptr),
      myThread(OSD_PTHREAD_NULL),
      myThreadId(0),
      myPriority(0)
{
}

//=============================================
// OSD_Thread::OSD_Thread
//=============================================

OSD_Thread::OSD_Thread(const OSD_ThreadFunction& func)
    : myFunc(func),
      myThread(OSD_PTHREAD_NULL),
      myThreadId(0),
      myPriority(0)
{
}

//=============================================
// OSD_Thread::OSD_Thread
//=============================================

OSD_Thread::OSD_Thread(const OSD_Thread& other)
    : myFunc(other.myFunc),
      myThread(OSD_PTHREAD_NULL),
      myThreadId(0)
{
  Assign(other);
}

//=============================================
// OSD_Thread::Assign
//=============================================

void OSD_Thread::Assign(const OSD_Thread& other)
{
  // copy function pointer
  myFunc     = other.myFunc;
  myPriority = other.myPriority;

  // detach current thread
  Detach();

#ifdef _WIN32
  // duplicate the source handle
  if (other.myThread)
  {
    HANDLE hProc = GetCurrentProcess(); // we are always within the same process
    if (!DuplicateHandle(hProc, other.myThread, hProc, &myThread, 0, TRUE, DUPLICATE_SAME_ACCESS))
    {
      // DuplicateHandle failed, ensure handle stays null
      myThread   = OSD_PTHREAD_NULL;
      myThreadId = 0;
      return;
    }
  }
#else
  // On Unix/Linux, just copy the thread id
  myThread = other.myThread;
#endif

  myThreadId = other.myThreadId;
}

//=============================================
// OSD_Thread::~OSD_Thread
//=============================================

OSD_Thread::~OSD_Thread()
{
  Detach();
}

//=============================================
// function : SetPriority
// purpose  : Set the thread priority relative to the caller's priority
//=============================================

void OSD_Thread::SetPriority(const int thePriority)
{
  myPriority = thePriority;
#ifdef _WIN32
  if (myThread)
    SetThreadPriority(myThread, thePriority);
#endif
}

//=============================================
// OSD_Thread::SetFunction
//=============================================

void OSD_Thread::SetFunction(const OSD_ThreadFunction& func)
{
  // close current handle if any
  Detach();
  myFunc = func;
}

//=============================================
// OSD_Thread::Run
//=============================================

#ifdef _WIN32
  #include <malloc.h>

// On Windows the signature of the thread function differs from that on UNIX/Linux.
// As we use the same definition of the thread function on all platforms (POSIX-like),
// we need to introduce appropriate wrapper function on Windows.
struct WNTthread_data
{
  void*              data;
  OSD_ThreadFunction func;
};

static DWORD WINAPI WNTthread_func(LPVOID data)
{
  WNTthread_data* adata = (WNTthread_data*)data;
  void*           ret   = adata->func(adata->data);
  free(adata);
  return PtrToLong(ret);
}
#endif

bool OSD_Thread::Run(void* const data,
#ifdef _WIN32
                     const int WNTStackSize
#else
                     const int
#endif
)
{
  if (!myFunc)
    return false;

  // detach current thread, if open
  Detach();

#ifdef _WIN32

  // allocate intermediate data structure to pass both data parameter and address
  // of the real thread function to Windows thread wrapper function
  WNTthread_data* adata = (WNTthread_data*)malloc(sizeof(WNTthread_data));
  if (!adata)
    return false;
  adata->data = data;
  adata->func = myFunc;

  // then try to create a new thread
  DWORD aThreadId = DWORD();
  myThread        = CreateThread(NULL, WNTStackSize, WNTthread_func, adata, 0, &aThreadId);
  myThreadId      = aThreadId;
  if (myThread)
    SetThreadPriority(myThread, myPriority);
  else
  {
    memset(adata, 0, sizeof(WNTthread_data));
    free(adata);
  }

#else

  if (pthread_create(&myThread, nullptr, myFunc, data) != 0)
  {
    myThread = OSD_PTHREAD_NULL;
  }
  else
  {
    myThreadId = (Standard_ThreadId)myThread;
  }
#endif
  return myThread != OSD_PTHREAD_NULL;
}

//=============================================
// OSD_Thread::Detach
//=============================================

void OSD_Thread::Detach()
{
#ifdef _WIN32

  // On Windows, close current handle
  if (myThread)
    CloseHandle(myThread);

#else

  // On Unix/Linux, detach a thread
  if (myThread)
    pthread_detach(myThread);

#endif

  myThread   = OSD_PTHREAD_NULL;
  myThreadId = 0;
}

//=============================================
// OSD_Thread::Wait
//=============================================

bool OSD_Thread::Wait(void*& theResult)
{
  // check that thread handle is not null
  theResult = nullptr;
  if (!myThread)
  {
    return false;
  }

#ifdef _WIN32
  // On Windows, wait for the thread handle to be signaled
  if (WaitForSingleObject(myThread, INFINITE) != WAIT_OBJECT_0)
  {
    return false;
  }

  // and convert result of the thread execution to void*
  DWORD anExitCode;
  if (GetExitCodeThread(myThread, &anExitCode))
  {
    theResult = ULongToPtr(anExitCode);
  }

  CloseHandle(myThread);
  myThread   = OSD_PTHREAD_NULL;
  myThreadId = 0;
  return true;
#else
  // On Unix/Linux, join the thread
  if (pthread_join(myThread, &theResult) != 0)
  {
    return false;
  }

  myThread   = OSD_PTHREAD_NULL;
  myThreadId = 0;
  return true;
#endif
}

//=============================================
// OSD_Thread::Wait
//=============================================

bool OSD_Thread::Wait(const int theTimeMs, void*& theResult)
{
  // check that thread handle is not null
  theResult = nullptr;
  if (!myThread)
  {
    return false;
  }

#ifdef _WIN32
  // On Windows, wait for the thread handle to be signaled
  DWORD ret = WaitForSingleObject(myThread, theTimeMs);
  if (ret == WAIT_OBJECT_0)
  {
    DWORD anExitCode;
    if (GetExitCodeThread(myThread, &anExitCode))
    {
      theResult = ULongToPtr(anExitCode);
    }

    CloseHandle(myThread);
    myThread   = OSD_PTHREAD_NULL;
    myThreadId = 0;
    return true;
  }
  else if (ret == WAIT_TIMEOUT)
  {
    return false;
  }

  return false;
#else
  #if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
    #if __GLIBC_PREREQ(2, 4)
      #define HAS_TIMED_NP
    #endif
  #endif

  #ifdef HAS_TIMED_NP
  struct timespec aTimeout;
  if (clock_gettime(CLOCK_REALTIME, &aTimeout) == -1)
  {
    return false;
  }

  time_t aSeconds     = (theTimeMs / 1000);
  long   aNanoseconds = (theTimeMs - aSeconds * 1000) * 1000000L;
  aTimeout.tv_sec += aSeconds;
  aTimeout.tv_nsec += aNanoseconds;
  // Normalize nanoseconds overflow to seconds
  if (aTimeout.tv_nsec >= 1000000000L)
  {
    aTimeout.tv_sec += aTimeout.tv_nsec / 1000000000L;
    aTimeout.tv_nsec = aTimeout.tv_nsec % 1000000000L;
  }

  if (pthread_timedjoin_np(myThread, &theResult, &aTimeout) != 0)
  {
    return false;
  }

  #else
  // join the thread without timeout
  (void)theTimeMs;
  if (pthread_join(myThread, &theResult) != 0)
  {
    return false;
  }
  #endif
  myThread   = OSD_PTHREAD_NULL;
  myThreadId = 0;
  return true;
#endif
}

//=============================================
// OSD_Thread::GetId
//=============================================

Standard_ThreadId OSD_Thread::GetId() const
{
  return myThreadId;
}

//=============================================
// OSD_Thread::Current
//=============================================

Standard_ThreadId OSD_Thread::Current()
{
#ifdef _WIN32
  return GetCurrentThreadId();
#else
  return (Standard_ThreadId)pthread_self();
#endif
}
