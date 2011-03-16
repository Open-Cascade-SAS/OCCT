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
//! Currently only two x86-based configurations (Windows NT with 
//! MS VC++ compiler and Linix with GCC) are really supported.
//! Other configurations use non-atomic C equivalent.

//! @fn     void Standard_Atomic_Increment (int volatile* var)
//! @brief  Increments atomically integer variable pointed by var

//! @fn     int Standard_Atomic_DecrementTest (int volatile* var)
//! @brief  Decrements atomically integer variable pointed by var;
//!         returns 1 if result is zero, 0 otherwise

//===================================================
// Windows NT, MSVC++ compiler
//===================================================
#if defined(WNT)

extern "C" {
long _InterlockedIncrement(long volatile* lpAddend);
long _InterlockedDecrement(long volatile* lpAddend);
}

#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)

inline void Standard_Atomic_Increment (int volatile* var)
{
  _InterlockedIncrement (reinterpret_cast<long volatile*>(var));
}

inline int Standard_Atomic_DecrementTest (int volatile* var)
{
  return _InterlockedDecrement (reinterpret_cast<long volatile*>(var)) == 0;
}

//===================================================
// Linux, GCC compiler
// Note: Linux kernel 2.6x provides definitions for atomic operators
//       in the header file /usr/include/asm/atomic.h,
//       however these definitions involve specific type atomic_t
// Note: The same code probably would work for Intel compiler
//===================================================
#elif defined(LIN)

inline void Standard_Atomic_Increment (int volatile* var)
{
  // C equivalent:
  // ++(*var);

  __asm__ __volatile__
  (
    "lock incl %0"
  : "=m"(*var) // out
  : "m" (*var) // in 
  );
}

inline int Standard_Atomic_DecrementTest (int volatile* var)
{
  // C equivalent:
  // return --(*var) == 0;

  unsigned char c;
  __asm__ __volatile__
  (
    "lock decl %0; sete %1"
  : "=m"(*var), "=qm"(c) // out
  : "m" (*var)           // in
  : "memory"
  );
  return c != 0;
}

//===================================================
// Default stub implementation, not atomic actually
//===================================================
#else

inline void Standard_Atomic_Increment (int volatile* var)
{
  ++(*var);
}

inline int Standard_Atomic_DecrementTest (int volatile* var)
{
  return --(*var) == 0;
}

#endif
