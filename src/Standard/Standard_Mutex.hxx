// Created on: 2005-04-10
// Created by: Andrey BETENEV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#ifndef _Standard_Mutex_HeaderFile
#define _Standard_Mutex_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_ErrorHandlerCallback.hxx>

#ifdef WNT
#include <windows.h>
#else
#include <pthread.h>
#include <sys/errno.h>
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

class Standard_Mutex : public Standard_ErrorHandlerCallback
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
    Sentry (Standard_Mutex &theMutex)
      : myMutex(theMutex)
    {
      myMutex.Lock();
      myMutex.RegisterCallback();
    }
    
    //! Destructor - unlocks the mutex if already locked.
    ~Sentry () { 
      myMutex.UnregisterCallback();
      myMutex.Unlock(); 
    }


  private:
    //! This method should not be called (prohibited).
    Sentry (const Sentry &);
    //! This method should not be called (prohibited).
    Sentry& operator = (const Sentry &);

  private:
    Standard_Mutex &myMutex;
  };
  
  /**
    * @brief Advanced Sentry class providing convenient interface to 
    *        manipulate a mutex from one thread.
    * 
    * As compared with simple Sentry class, provides possibility to 
    * lock and unlock mutex at any moment, and perform nested lock/unlock 
    * actions (using lock counter). However all calls must be from within
    * the same thread; this is to be ensured by the code using this class.
    */
  class SentryNested
  {
  public:

    //! Constructor - initializes the sentry object by reference to a
    //! mutex (which must be initialized). Locks the mutex immediately
    //! unless Standard_False is given as second argument.
    SentryNested (Standard_Mutex &theMutex, Standard_Boolean doLock = Standard_True)
      : myMutex(theMutex), nbLocked(0)
    {
      if ( doLock ) Lock();
    }
    
    //! Destructor - unlocks the mutex if already locked.
    ~SentryNested () 
    { 
      if ( nbLocked >0 ) { 
	nbLocked = 1; 
	Unlock(); 
      } 
    }
    
    //! Lock the mutex
    void Lock () { 
      if ( ! nbLocked ) {
	myMutex.Lock(); 
	myMutex.RegisterCallback();
      }
      nbLocked++; 
    }
    
    //! Unlock the mutex
    void Unlock () { 
      if ( nbLocked == 1 ) {
	myMutex.UnregisterCallback();
	myMutex.Unlock(); 
      }
      nbLocked--; 
    }
    
  private:
    //! This method should not be called (prohibited).
    SentryNested (const SentryNested &);
    //! This method should not be called (prohibited).
    SentryNested& operator = (const SentryNested &);

  private:
    Standard_Mutex &myMutex;
    Standard_Boolean nbLocked; //!< Note that we do not protect this field from 
                               //!< concurrent access, as it should always be accessed
                               //!< from within one thread, i.e. synchronously
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
  Standard_EXPORT void Unlock ();

private:

  //! Callback method to unlock the mutex if OCC exception or signal is raised
  virtual void DestroyCallback ();
  
  //! This method should not be called (prohibited).
  Standard_Mutex (const Standard_Mutex &);
  //! This method should not be called (prohibited).
  Standard_Mutex& operator = (const Standard_Mutex &);
  
private:
#ifdef WNT
  CRITICAL_SECTION myMutex;
#else
  pthread_mutex_t myMutex;
#endif  
};

// Implementation of the method Unlock is inline, since it is 
// just a shortcut to system function
inline void Standard_Mutex::Unlock ()
{
#ifdef WNT
  LeaveCriticalSection( &myMutex );
#else
  pthread_mutex_unlock( &myMutex );
#endif
}

#endif /* _Standard_Mutex_HeaderFile */
