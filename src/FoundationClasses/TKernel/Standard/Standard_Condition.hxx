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
#include <Standard_Macro.hxx>

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <mutex>

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
  Standard_Condition(bool theIsSet = false)
      : myFlag(theIsSet)
  {
  }

  //! Destructor.
  ~Standard_Condition() {}

  //! Set event into signaling state.
  void Set()
  {
    {
      std::lock_guard<std::mutex> aLock(myMutex);
      myFlag = true;
    }
    myCondition.notify_all();
  }

  //! Reset event (unset signaling state)
  void Reset()
  {
    std::lock_guard<std::mutex> aLock(myMutex);
    myFlag = false;
  }

  //! Wait for Event (infinity).
  void Wait()
  {
    std::unique_lock<std::mutex> aLock(myMutex);
    myCondition.wait(aLock, [this] { return myFlag.load(); });
  }

  //! Wait for signal requested time.
  //! @param theTimeMilliseconds wait limit in milliseconds
  //! @return true if get event
  bool Wait(int theTimeMilliseconds)
  {
    std::unique_lock<std::mutex> aLock(myMutex);
    auto                         aTimeout = std::chrono::milliseconds(theTimeMilliseconds);
    return myCondition.wait_for(aLock, aTimeout, [this] { return myFlag.load(); });
  }

  //! Do not wait for signal - just test it state.
  //! @return true if get event
  bool Check() { return myFlag.load(); }

  //! Method perform two steps at-once - reset the event object
  //! and returns true if it was in signaling state.
  //! @return true if event object was in signaling state.
  bool CheckReset()
  {
    std::lock_guard<std::mutex> aLock(myMutex);
    bool                        wasSignalled = myFlag.load();
    myFlag                                   = false;
    return wasSignalled;
  }

private:
  //! This method should not be called (prohibited).
  Standard_Condition(const Standard_Condition& theCopy);
  //! This method should not be called (prohibited).
  Standard_Condition& operator=(const Standard_Condition& theCopy);

private:
  std::mutex              myMutex;
  std::condition_variable myCondition;
  std::atomic<bool>       myFlag;
};

#endif // _Standard_Condition_HeaderFile
