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
OSD_Thread::OSD_Thread ()
  : myFunc(0), myThread(0), myThreadId(0), myPriority(0)
{}

//=============================================
// OSD_Thread::OSD_Thread
//=============================================

OSD_Thread::OSD_Thread (const OSD_ThreadFunction &func)
  : myFunc(func), myThread(0), myThreadId(0), myPriority(0)
{}

//=============================================
// OSD_Thread::OSD_Thread
//=============================================

OSD_Thread::OSD_Thread (const OSD_Thread &other)
  : myFunc(other.myFunc), myThread(0), myThreadId(0)
{
  Assign ( other );
}

//=============================================
// OSD_Thread::Assign
//=============================================

void OSD_Thread::Assign (const OSD_Thread &other)
{
  // copy function pointer
  myFunc = other.myFunc;
  myPriority = other.myPriority;

  // detach current thread
  Detach();

#ifdef _WIN32
  // duplicate the source handle
  if ( other.myThread ) {
    HANDLE hProc = GetCurrentProcess(); // we are always within the same process
    DuplicateHandle ( hProc, other.myThread, hProc, &myThread,
		      0, TRUE, DUPLICATE_SAME_ACCESS );
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
//function : SetPriority
//purpose  : Set the thread priority relative to the caller's priority
//=============================================

void OSD_Thread::SetPriority (const Standard_Integer thePriority)
{
  myPriority = thePriority;
#ifdef _WIN32
  if (myThread)
    SetThreadPriority (myThread, thePriority);
#endif
}

//=============================================
// OSD_Thread::SetFunction
//=============================================

void OSD_Thread::SetFunction (const OSD_ThreadFunction &func)
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
struct WNTthread_data { void *data; OSD_ThreadFunction func; };
static DWORD WINAPI WNTthread_func (LPVOID data)
{
  WNTthread_data *adata = (WNTthread_data*)data;
  void* ret = adata->func ( adata->data );
  free ( adata );
  return PtrToLong (ret);
}
#endif

Standard_Boolean OSD_Thread::Run (const Standard_Address data,
#ifdef _WIN32
                                  const Standard_Integer WNTStackSize
#else
                                  const Standard_Integer
#endif
				  )
{
  if ( ! myFunc ) return Standard_False;

  // detach current thread, if open
  Detach();

#ifdef _WIN32

  // allocate intermediate data structure to pass both data parameter and address
  // of the real thread function to Windows thread wrapper function
  WNTthread_data *adata = (WNTthread_data*)malloc ( sizeof(WNTthread_data) );
  if ( ! adata ) return Standard_False;
  adata->data = data;
  adata->func = myFunc;

  // then try to create a new thread

  myThread = CreateThread ( NULL, WNTStackSize, WNTthread_func,
                            adata, 0, &myThreadId );
  if ( myThread )
    SetThreadPriority (myThread, myPriority);
  else {
    memset ( adata, 0, sizeof(WNTthread_data) );
    free ( adata );
  }

#else

  if (pthread_create (&myThread, 0, myFunc, data) != 0)
  {
    myThread = 0;
  }
  else
  {
    myThreadId = myThread;
  }
#endif
  return myThread != 0;
}

//=============================================
// OSD_Thread::Detach
//=============================================

void OSD_Thread::Detach ()
{
#ifdef _WIN32

  // On Windows, close current handle
  if ( myThread )
    CloseHandle ( myThread );

#else

  // On Unix/Linux, detach a thread
  if ( myThread )
    pthread_detach ( myThread );

#endif

  myThread = 0;
  myThreadId = 0;
}

//=============================================
// OSD_Thread::Wait
//=============================================

Standard_Boolean OSD_Thread::Wait (Standard_Address& theResult)
{
  // check that thread handle is not null
  theResult = 0;
  if (!myThread)
  {
    return Standard_False;
  }

#ifdef _WIN32
  // On Windows, wait for the thread handle to be signaled
  if (WaitForSingleObject (myThread, INFINITE) != WAIT_OBJECT_0)
  {
    return Standard_False;
  }

  // and convert result of the thread execution to Standard_Address
  DWORD anExitCode;
  if (GetExitCodeThread (myThread, &anExitCode))
  {
    theResult = ULongToPtr (anExitCode);
  }

  CloseHandle (myThread);
  myThread   = 0;
  myThreadId = 0;
  return Standard_True;
#else
  // On Unix/Linux, join the thread
  if (pthread_join (myThread, &theResult) != 0)
  {
    return Standard_False;
  }

  myThread   = 0;
  myThreadId = 0;
  return Standard_True;
#endif
}

//=============================================
// OSD_Thread::Wait
//=============================================

Standard_Boolean OSD_Thread::Wait (const Standard_Integer theTimeMs,
                                   Standard_Address& theResult)
{
  // check that thread handle is not null
  theResult = 0;
  if (!myThread)
  {
    return Standard_False;
  }

#ifdef _WIN32
  // On Windows, wait for the thread handle to be signaled
  DWORD ret = WaitForSingleObject (myThread, theTimeMs);
  if (ret == WAIT_OBJECT_0)
  {
    DWORD anExitCode;
    if (GetExitCodeThread (myThread, &anExitCode))
    {
      theResult = ULongToPtr (anExitCode);
    }

    CloseHandle (myThread);
    myThread   = 0;
    myThreadId = 0;
    return Standard_True;
  }
  else if (ret == WAIT_TIMEOUT)
  {
    return Standard_False;
  }

  return Standard_False;
#else
  #if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
    #if __GLIBC_PREREQ(2,4)
      #define HAS_TIMED_NP
    #endif
  #endif

  #ifdef HAS_TIMED_NP
    struct timespec aTimeout;
    if (clock_gettime (CLOCK_REALTIME, &aTimeout) == -1)
    {
      return Standard_False;
    }

    time_t aSeconds      = (theTimeMs / 1000);
    long   aMicroseconds = (theTimeMs - aSeconds * 1000) * 1000;
    aTimeout.tv_sec  += aSeconds;
    aTimeout.tv_nsec += aMicroseconds * 1000;

    if (pthread_timedjoin_np (myThread, &theResult, &aTimeout) != 0)
    {
      return Standard_False;
    }

  #else
    // join the thread without timeout
    (void )theTimeMs;
    if (pthread_join (myThread, &theResult) != 0)
    {
      return Standard_False;
    }
  #endif
    myThread   = 0;
    myThreadId = 0;
    return Standard_True;
#endif
}

//=============================================
// OSD_Thread::GetId
//=============================================

Standard_ThreadId OSD_Thread::GetId () const
{
  return myThreadId;
}

//=============================================
// OSD_Thread::Current
//=============================================

Standard_ThreadId OSD_Thread::Current ()
{
#ifdef _WIN32
  return GetCurrentThreadId();
#else
  return pthread_self();
#endif
}
