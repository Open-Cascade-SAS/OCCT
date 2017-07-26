// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef DFBrowser_Thread_H
#define DFBrowser_Thread_H

#include <Standard.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QList>
#include <QObject>

class DFBrowser_Module;
class DFBrowser_ThreadItem;
class DFBrowser_Window;

class QThread;

//! /class DFBrowser_Thread
//! Starts algorithm item in a separate thread and perform some functionality by the algorithm(thread) is finished
class DFBrowser_Thread : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_Thread (DFBrowser_Window* theWindow);

  //! Destructor
  virtual ~DFBrowser_Thread() {}

  //! Sets module to the started thread items
  //! \param theModule a current module
  Standard_EXPORT void SetModule (DFBrowser_Module* theModule);

  //! Starts all candidate thread items
  Standard_EXPORT void ProcessApplication();

  //! Terminates all started threads.
  Standard_EXPORT void TerminateThread();

protected:

  //! Creates new Qt thread and starts the item. Connects to finished signal of thread.
  //! \param theItem a thread item
  void startThread (DFBrowser_ThreadItem* theItem);

protected slots:

  //! Removes finished thread from the thread items and apply values of this thread
  //! If this signal is come when another thread is processed, the current thread is stored in a cache and
  //! is processed after the previous thread is processed.
  void onFinished();

private:

  QList<DFBrowser_ThreadItem*> myItems; //!< candidates to be processed in a thread
  QList<QThread*> myStartedThreads; //!< container of started threads
  DFBrowser_ThreadItem* myPostponedItem; //!< currently processed item in onFinished()
  bool myIsFinishProcessing; //!< blocking state if onFinished() is started but has not been finished yet
};


#endif
