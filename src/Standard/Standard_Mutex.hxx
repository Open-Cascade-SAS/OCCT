// Created on: 2005-04-10
// Created by: Andrey BETENEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Standard_Mutex_HeaderFile
#define _Standard_Mutex_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_ErrorHandler.hxx>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <pthread.h>
  ///#include <sys/errno.h>
  #include <unistd.h>
  #include <time.h>
#endif

/** 
  * @brief Mutex: a class to synchronize access to shared data. 
  *
  * This is simple encapsulation of tools provided by the
  * operating system to syncronize access to shared data 
  * from threads within one process.
  *
  * Current implementation is very simple and straightforward;
  * it is just a wrapper around POSIX pthread librray on UNIX/Linux,
  * and CRITICAL_SECTIONs on Windows NT. It does not provide any
  * advanced functionaly such as recursive calls to the same mutex from 
  * within one thread (such call will froze the execution).
  *
  * Note that all the methods of that class are made inline, in order
  * to keep maximal performance. This means that a library using the mutex
  * might need to be linked to threads library directly.
  *
  * The typical use of this class should be as follows:
  * - create instance of the class Standard_Mutex in the global scope
  *   (whenever possible, or as a field of your class)
  * - create instance of class Standard_Mutex::Sentry using that Mutex
  *   when entering critical section
  *
  * Note that this class provides one feature specific to Open CASCADE:
  * safe unlocking the mutex when signal is raised and converted to OCC
  * exceptions (Note that with current implementation of this functionality
  * on UNIX and Linux, C longjumps are used for that, thus destructors of 
  * classes are not called automatically).
  * 
  * To use this feature, call RegisterCallback() after Lock() or successful
  * TryLock(), and UnregisterCallback() before Unlock() (or use Sentry classes). 
  */

class Standard_Mutex : public Standard_ErrorHandler::Callback
{
public:
  /**
    * @brief Simple sentry class providing convenient interface to mutex.
    * 
    * Provides automatic locking and unlocking a mutex in its constructor
    * and destructor, thus ensuring correct unlock of the mutex even in case of 
    * raising an exception or signal from the protected code.
    *
    * Create instance of that class when entering critical section.
    */
  class Sentry 
  {
  public:

    //! Constructor - initializes the sentry object by reference to a
    //! mutex (which must be initialized) and locks the mutex immediately
    Sentry (Standard_Mutex& theMutex)
    : myMutex (&theMutex)
    {
      Lock();
    }
    
    //! Constructor - initializes the sentry object by pointer to a
    //! mutex and locks the mutex if its pointer is not NULL
    Sentry (Standard_Mutex* theMutex)
    : myMutex (theMutex)
    {
      if (myMutex != NULL)
      {
        Lock();
      }
    }
    //! Destructor - unlocks the mutex if already locked.
    ~Sentry()
    {
      if (myMutex != NULL)
      {
        Unlock();
      }
    }

  private:

    //! Lock the mutex
    void Lock()
    {
      myMutex->Lock();
      myMutex->RegisterCallback();
    }

    //! Unlock the mutex
    void Unlock()
    {
      myMutex->UnregisterCallback();
      myMutex->Unlock();
    }

    //! This method should not be called (prohibited).
    Sentry (const Sentry &);
    //! This method should not be called (prohibited).
    Sentry& operator = (const Sentry &);

  private:
    Standard_Mutex* myMutex;
  };
   
public:
  
  //! Constructor: creates a mutex object and initializes it.
  //! It is strongly recommended that mutexes were created as 
  //! static objects whenever possible.
  Standard_EXPORT Standard_Mutex ();
  
  //! Destructor: destroys the mutex object
  Standard_EXPORT ~Standard_Mutex ();
  
  //! Method to lock the mutex; waits until the mutex is released
  //! by other threads, locks it and then returns
  Standard_EXPORT void Lock ();

  //! Method to test the mutex; if the mutex is not hold by other thread,
  //! locks it and returns True; otherwise returns False without waiting
  //! mutex to be released.
  Standard_EXPORT Standard_Boolean TryLock ();

  //! Method to unlock the mutex; releases it to other users
  void Unlock ();

private:

  //! Callback method to unlock the mutex if OCC exception or signal is raised
  virtual void DestroyCallback ();
  
  //! This method should not be called (prohibited).
  Standard_Mutex (const Standard_Mutex &);
  //! This method should not be called (prohibited).
  Standard_Mutex& operator = (const Standard_Mutex &);
  
private:
#if (defined(_WIN32) || defined(__WIN32__))
  CRITICAL_SECTION myMutex;
#else
  pthread_mutex_t myMutex;
#endif  
};

// Implementation of the method Unlock is inline, since it is 
// just a shortcut to system function
inline void Standard_Mutex::Unlock ()
{
#if (defined(_WIN32) || defined(__WIN32__))
  LeaveCriticalSection (&myMutex);
#else
  pthread_mutex_unlock (&myMutex);
#endif
}

#endif /* _Standard_Mutex_HeaderFile */
