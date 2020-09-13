// Created on: 2002-02-20
// Created by: Andrey BETENEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _Message_ProgressIndicator_HeaderFile
#define _Message_ProgressIndicator_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Handle.hxx>

DEFINE_STANDARD_HANDLE(Message_ProgressIndicator, Standard_Transient)

class Message_ProgressRange;
class Message_ProgressScope;

//! Defines abstract interface from program to the user.
//! This includes progress indication and user break mechanisms.
//!
//! The progress indicator controls the progress scale with range from 0 to 1.
//! 
//! Method Start() should be called once, at the top level of the call stack,
//! to reset progress indicator and get access to the root range:
//!
//! @code{.cpp}
//! Handle(Message_ProgressIndicator) aProgress = ...;
//! anAlgorithm.Perform (aProgress->Start());
//! @endcode
//!
//! To advance the progress indicator in the algorithm,
//! use the class Message_ProgressScope that provides iterator-like
//! interface for incrementing progress; see documentation of that
//! class for details.
//! The object of class Message_ProgressRange will automatically advance 
//! the indicator if it is not passed to any Message_ProgressScope.
//!
//! The progress indicator supports concurrent processing and 
//! can be used in multithreaded applications.
//!
//! The derived class should be created to connect this interface to 
//! actual implementation of progress indicator, to take care of visualization
//! of the progress (e.g. show total position at the graphical bar,
//! print scopes in text mode, or else), and for implementation
//! of user break mechanism (if necessary).
//!
//! See details in documentation of methods Show() and UserBreak().

class Message_ProgressIndicator : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_ProgressIndicator, Standard_Transient)
public:
  //!@name Initialization of progress indication

  //! Resets the indicator to zero, calls Reset(), and returns the range.
  //! This range refers to the scope that has no name and is initialized
  //! with max value 1 and step 1.
  //! Use this method to get the top level range for progress indication.
  Standard_EXPORT Message_ProgressRange Start();

  //! If argument is non-null handle, returns theProgress->Start().
  //! Otherwise, returns dummy range that can be safely used in the algorithms
  //! but not bound to progress indicator.
  Standard_EXPORT static Message_ProgressRange Start
                      (const Handle(Message_ProgressIndicator)& theProgress);

protected:
  //!@name Virtual methods to be defined by descendant.

  //! Should return True if user has sent a break signal.
  //!
  //! This method can be called concurrently, thus implementation should
  //! be thread-safe. It should not call Show() or Position() to
  //! avoid possible data races. The method should return as soon
  //! as possible to avoid delaying the calling algorithm.
  //!
  //! Default implementation returns False.
  virtual Standard_Boolean UserBreak()
  {
    return Standard_False;
  }

  //! Virtual method to be defined by descendant.
  //! Should update presentation of the progress indicator.
  //!
  //! It is called whenever progress position is changed.
  //! Calls to this method from progress indicator are protected by mutex so that
  //! it is never called concurrently for the same progress indicator instance.
  //! Show() should return as soon as possible to reduce thread contention
  //! in multithreaded algorithms.
  //!
  //! It is recommended to update (redraw, output etc.) only if progress is
  //! advanced by at least 1% from previous update.
  //!
  //! Flag isForce is intended for forcing update in case if it is required 
  //! at particular step of the algorithm; all calls to it from inside the core 
  //! mechanism (Message_Progress... classes) are done with this flag equal to False.
  //!
  //! The parameter theScope is the current scope being advanced;
  //! it can be used to show the names and ranges of the on-going scope and
  //! its parents, providing more visibility of the current stage of the process.
  virtual void Show (const Message_ProgressScope& theScope, 
                     const Standard_Boolean isForce) = 0;

  //! Call-back method called by Start(), can be redefined by descendants
  //! if some actions are needed when the indicator is restarted.
  virtual void Reset() {}
  
public:
  //!@name Auxiliary methods

  //! Returns total progress position ranged from 0 to 1.
  //! Should not be called concurrently while the progress is advancing,
  //! except from implementation of method Show().
  Standard_Real GetPosition() const
  {
    return myPosition;
  }

  //! Destructor
  Standard_EXPORT ~Message_ProgressIndicator();

protected:
  
  //! Constructor
  Standard_EXPORT Message_ProgressIndicator();

private:

  //! Increment the progress value by the specified step, 
  //! then calls Show() to update presentation.
  //! The parameter theScope is reference to the caller object;
  //! it is passed to Show() where can be used to track context of the process.
  void Increment (const Standard_Real theStep, const Message_ProgressScope& theScope);

private:

  Standard_Real myPosition;            //!< Total progress position ranged from 0 to 1
  Standard_Mutex myMutex;              //!< Protection of myPosition from concurrent increment
  Message_ProgressScope* myRootScope;  //!< The root progress scope

private:
  friend class Message_ProgressScope;  //!< Friend: can call Increment()
  friend class Message_ProgressRange;  //!< Friend: can call Increment()
};

#include <Message_ProgressScope.hxx>

//=======================================================================
//function : Increment
//purpose  :
//=======================================================================
inline void Message_ProgressIndicator::Increment(const Standard_Real theStep,
                                                 const Message_ProgressScope& theScope)
{
  // protect incrementation by mutex to avoid problems in multithreaded scenarios
  Standard_Mutex::Sentry aSentry(myMutex);

  myPosition = Min(myPosition + theStep, 1.);

  // show progress indicator; note that this call is protected by
  // the same mutex to avoid concurrency and ensure that this call
  // to Show() will see the position exactly as it was just set above
  Show(theScope, Standard_False);
}

#endif // _Message_ProgressIndicator_HeaderFile
