// Created on: 2007-09-04
// Created by: Andrey BETENEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

//! @file
//! Implementation of some atomic operations (elementary operations
//! with data that cannot be interrupted by parallel threads in the
//! multithread process) on various platforms
//!
//! By the moment, only operations necessary for reference counter
//! in Standard_Transient objects are implemented.
//!
//! This is preferred to use fixed size types "int32_t" / "int64_t" for
//! correct function declarations however we leave "int" assuming it is 32bits for now.

#ifndef _Standard_Atomic_HeaderFile
#define _Standard_Atomic_HeaderFile

//! Increments atomically integer variable pointed by theValue
//! and returns resulting incremented value.
inline int Standard_Atomic_Increment(volatile int* theValue);

//! Decrements atomically integer variable pointed by theValue
//! and returns resulting decremented value.
inline int Standard_Atomic_Decrement(volatile int* theValue);

//! Perform an atomic compare and swap.
//! That is, if the current value of *theValue is theOldValue, then write theNewValue into
//! *theValue.
//! @param theValue    pointer to variable to modify
//! @param theOldValue expected value to perform modification
//! @param theNewValue new value to set in case if *theValue was equal to theOldValue
//! @return TRUE if theNewValue has been set to *theValue
inline bool Standard_Atomic_CompareAndSwap(volatile int* theValue,
                                           int           theOldValue,
                                           int           theNewValue);

// Platform-dependent implementation
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) || defined(__EMSCRIPTEN__)
// gcc explicitly defines the macros __GCC_HAVE_SYNC_COMPARE_AND_SWAP_*
// starting with version 4.4+, although built-in functions
// are available since 4.1.x. However unless __GCC_HAVE_SYNC_COMPARE_AND_SWAP_*
// are defined, linking may fail without specifying -march option when
// building for 32bit architecture on 64bit (using -m32 option). To avoid
// making -march mandatory, check for __GCC_HAVE_SYNC_COMPARE_AND_SWAP_* is
// enforced.

int Standard_Atomic_Increment(volatile int* theValue)
{
  return __sync_add_and_fetch(theValue, 1);
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
  return __sync_sub_and_fetch(theValue, 1);
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
  return __sync_val_compare_and_swap(theValue, theOldValue, theNewValue) == theOldValue;
}

#elif defined(_WIN32)
extern "C"
{
  long _InterlockedIncrement(volatile long* lpAddend);
  long _InterlockedDecrement(volatile long* lpAddend);
  long _InterlockedCompareExchange(long volatile* Destination, long Exchange, long Comparand);
}

  #if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    // force intrinsic instead of WinAPI calls
    #pragma intrinsic(_InterlockedIncrement)
    #pragma intrinsic(_InterlockedDecrement)
    #pragma intrinsic(_InterlockedCompareExchange)
  #endif

// WinAPI function or MSVC intrinsic
// Note that we safely cast int* to long*, as they have same size and endian-ness

int Standard_Atomic_Increment(volatile int* theValue)
{
  return _InterlockedIncrement(reinterpret_cast<volatile long*>(theValue));
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
  return _InterlockedDecrement(reinterpret_cast<volatile long*>(theValue));
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
  return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(theValue),
                                     theNewValue,
                                     theOldValue)
         == theOldValue;
}

#elif defined(__APPLE__)
  // use atomic operations provided by MacOS

  #include <libkern/OSAtomic.h>

int Standard_Atomic_Increment(volatile int* theValue)
{
  return OSAtomicIncrement32Barrier(theValue);
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
  return OSAtomicDecrement32Barrier(theValue);
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
  return OSAtomicCompareAndSwapInt(theOldValue, theNewValue, theValue);
}

#elif defined(__ANDROID__)

  // Atomic operations that were exported by the C library didn't
  // provide any memory barriers, which created potential issues on
  // multi-core devices. Starting from ndk version r7b they are defined as
  // inlined calls to GCC sync builtins, which always provide a full barrier.
  // It is strongly recommended to use newer versions of ndk.
  #include <sys/atomics.h>

int Standard_Atomic_Increment(volatile int* theValue)
{
  return __atomic_inc(theValue) + 1; // analog of __sync_fetch_and_add
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
  return __atomic_dec(theValue) - 1; // analog of __sync_fetch_and_sub
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
  return __atomic_cmpxchg(theOldValue, theNewValue, theValue) == 0;
}

#else

  #ifndef IGNORE_NO_ATOMICS
    #error "Atomic operation isn't implemented for current platform!"
  #endif
int Standard_Atomic_Increment(volatile int* theValue)
{
  return ++(*theValue);
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
  return --(*theValue);
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
  if (*theValue == theOldValue)
  {
    *theValue = theNewValue;
    return true;
  }
  return false;
}

#endif

#endif //_Standard_Atomic_HeaderFile
