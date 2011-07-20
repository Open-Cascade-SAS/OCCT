// File:    Standard_Atomic.hxx
// Created: Tue Sep 04 08:52:43 2007
// Author:  Andrey BETENEV

//! @file 
//! Implementation of some atomic operations (elementary operations 
//! with data that cannot be interrupted by parallel threads in the
//! multithread process) on various platforms
//!
//! By the moment, only operations necessary for reference counter 
//! in Standard_Transient objects are implemented.
//! 
//! This is preffered to use fixed size types "int32_t" / "int64_t" for
//! correct function declarations however we leave "int" assuming it is 32bits for now.

#ifndef _Standard_Atomic_HeaderFile
#define _Standard_Atomic_HeaderFile

#include <Standard_Macro.hxx>

#if (defined(_WIN32) || defined(__WIN32__))
extern "C" {
  long _InterlockedIncrement(long volatile* lpAddend);
  long _InterlockedDecrement(long volatile* lpAddend);
}
#endif

#if defined(_MSC_VER)
  // force intrinsic instead of WinAPI calls
  #pragma intrinsic (_InterlockedIncrement)
  #pragma intrinsic (_InterlockedDecrement)
#endif

//! Increments atomically integer variable pointed by theValue
//! and returns resulting incremented value.
static int Standard_Atomic_Increment (volatile int* theValue)
{
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
  // mordern g++ compiler (gcc4.4+)
  // built-in functions available for appropriate CPUs (at least -march=i486 should be specified on x86 platform)
  return __sync_add_and_fetch (theValue, 1);
#elif (defined(_WIN32) || defined(__WIN32__))
  // WinAPI function or MSVC intrinsic
  return _InterlockedIncrement(reinterpret_cast<long volatile*>(theValue));
#elif defined(LIN)
  // use x86 / x86_64 inline assembly (compatibility with alien compilers / old GCC)
  int anIncResult;
  __asm__ __volatile__ (
  #if defined(_OCC64)
    "lock xaddl %%ebx, (%%rax) \n\t"
    "incl %%ebx                \n\t"
    : "=b" (anIncResult)
    : "a" (theValue), "b" (1)
    : "cc", "memory");
  #else
    "lock xaddl %%eax, (%%ecx) \n\t"
    "incl %%eax                \n\t"
    : "=a" (anIncResult)
    : "c" (theValue), "a" (1)
    : "memory");
  #endif
  return anIncResult;
#else
  //#error "Atomic operation doesn't implemented for current platform!"
  return ++(*theValue);
#endif
}

//! Decrements atomically integer variable pointed by theValue
//! and returns resulting decremented value.
static int Standard_Atomic_Decrement (volatile int* theValue)
{
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
  // mordern g++ compiler (gcc4.4+)
  // built-in functions available for appropriate CPUs (at least -march=i486 should be specified on x86 platform)
  return __sync_sub_and_fetch (theValue, 1);
#elif (defined(_WIN32) || defined(__WIN32__))
  // WinAPI function or MSVC intrinsic
  return _InterlockedDecrement(reinterpret_cast<long volatile*>(theValue));
#elif defined(LIN)
  // use x86 / x86_64 inline assembly (compatibility with alien compilers / old GCC)
  int aDecResult;
  __asm__ __volatile__ (
  #if defined(_OCC64)
    "lock xaddl %%ebx, (%%rax) \n\t"
    "decl %%ebx                \n\t"
    : "=b" (aDecResult)
    : "a" (theValue), "b" (-1)
    : "cc", "memory");
  #else
    "lock xaddl %%eax, (%%ecx) \n\t"
    "decl %%eax                \n\t"
    : "=a" (aDecResult)
    : "c" (theValue), "a" (-1)
    : "memory");
  #endif
  return aDecResult;
#else
  //#error "Atomic operation doesn't implemented for current platform!"
  return --(*theValue);
#endif
}

#endif //_Standard_Atomic_HeaderFile
