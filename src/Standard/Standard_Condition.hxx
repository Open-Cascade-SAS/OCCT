// Created by: Kirill Gavrilov
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _Standard_Condition_HeaderFile
#define _Standard_Condition_HeaderFile

#include <Standard.hxx>

#ifndef _WIN32
  #include <pthread.h>
#endif

//! This is boolean flag intended for communication between threads.
//! One thread sets this flag to TRUE to indicate some event happened
//! and another thread either waits this event or checks periodically its state to perform job.
//!
//! This class provides interface similar to WinAPI Event objects.
class Standard_Condition
{
public:

  //! Default constructor.
  //! @param theIsSet Initial flag state
  Standard_EXPORT Standard_Condition (bool theIsSet);

  //! Destructor.
  Standard_EXPORT ~Standard_Condition();

  //! Set event into signaling state.
  Standard_EXPORT void Set();

  //! Reset event (unset signaling state)
  Standard_EXPORT void Reset();

  //! Wait for Event (infinity).
  Standard_EXPORT void Wait();

  //! Wait for signal requested time.
  //! @param theTimeMilliseconds wait limit in milliseconds
  //! @return true if get event
  Standard_EXPORT bool Wait (int theTimeMilliseconds);

  //! Do not wait for signal - just test it state.
  //! @return true if get event
  Standard_EXPORT bool Check();

  //! Method perform two steps at-once - reset the event object
  //! and returns true if it was in signaling state.
  //! @return true if event object was in signaling state.
  Standard_EXPORT bool CheckReset();

#ifdef _WIN32
  //! Access native HANDLE to Event object.
  void* getHandle() const { return myEvent; }
#endif

private:
  //! This method should not be called (prohibited).
  Standard_Condition (const Standard_Condition& theCopy);
  //! This method should not be called (prohibited).
  Standard_Condition& operator= (const Standard_Condition& theCopy);

private:

#ifdef _WIN32
  void*           myEvent;
#else
  pthread_mutex_t myMutex;
  pthread_cond_t  myCond;
  bool            myFlag;
#endif

};

#endif // _Standard_Condition_HeaderFile
