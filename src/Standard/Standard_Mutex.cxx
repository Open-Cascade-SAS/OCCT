// Created on: 2006-04-13
// Created by: Andrey BETENEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
