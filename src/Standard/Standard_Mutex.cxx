// File:	Standard_Mutex.cxx
// Created:	Thu Apr 13 08:21:40 2006
// Author:	Andrey BETENEV
// Copyright:	Open CASCADE S.A.S. 2006

// On Windows, function TryEnterCriticalSection has appeared in Windows NT
// and is surrounded by #ifdef in MS VC++ 7.1 headers.
// Thus to use it we need to define appropriate macro saying that we wil
// run on Windows NT 4.0 at least
#if defined(WNT) && ! defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif

#include <Standard_Mutex.hxx>
#include <Standard_OStream.hxx>

//=============================================
// Standard_Mutex::Standard_Mutex
//=============================================

Standard_Mutex::Standard_Mutex () 
{
#ifdef WNT
  InitializeCriticalSection( &myMutex );
#else
  pthread_mutex_init( &myMutex, 0 );
#endif
}

//=============================================
// Standard_Mutex::~Standard_Mutex
//=============================================

Standard_Mutex::~Standard_Mutex () 
{
#ifdef WNT
  DeleteCriticalSection( &myMutex );
#else
  pthread_mutex_destroy( &myMutex );
#endif
}

//=============================================
// Standard_Mutex::Lock
//=============================================

void Standard_Mutex::Lock ()
{
#ifdef WNT
  EnterCriticalSection( &myMutex );
#else
  pthread_mutex_lock( &myMutex );
#endif
}

//=============================================
// Standard_Mutex::TryLock
//=============================================

Standard_Boolean Standard_Mutex::TryLock ()
{
#ifdef WNT
  return ( TryEnterCriticalSection( &myMutex ) != 0 );
#else
  return ( pthread_mutex_trylock( &myMutex ) != EBUSY );
#endif
}

//=============================================
// Standard_Mutex::DestroyCallback
//=============================================

void Standard_Mutex::DestroyCallback ()
{
  UnregisterCallback();
  Unlock();
}
