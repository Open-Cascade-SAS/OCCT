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

#include <inspector/DFBrowser_Thread.hxx>
#include <inspector/DFBrowser_ThreadItemSearch.hxx>
#include <inspector/DFBrowser_ThreadItemUsedShapesMap.hxx>
#include <inspector/DFBrowser_TreeLevelLine.hxx>
#include <inspector/DFBrowser_SearchLine.hxx>

#include <inspector/DFBrowser_Window.hxx>

#include <QThread>

//! \class DFBrowser_QThread
//! Internal class to cover QThread in order to process ThreadItem.
class DFBrowser_QThread : public QThread
{
public:

  //! Constructor
  DFBrowser_QThread (QObject* theParent) : QThread (theParent), myItem (0) {}

  //! Destructor
  virtual ~DFBrowser_QThread() Standard_OVERRIDE {}

  //! Sets thread item to be processed
  //! \param theItem a thread item
  void setItem (DFBrowser_ThreadItem* theItem) { myItem = theItem; }

  //! Returns the current processing thread item
  DFBrowser_ThreadItem* getItem() const { return myItem; }

protected:

  //! Starts thread item
  virtual void run() Standard_OVERRIDE
  {
    if (myItem)
      myItem->Run();
  }

private:

  DFBrowser_ThreadItem* myItem;
};

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_Thread::DFBrowser_Thread (DFBrowser_Window* theWindow)
: QObject (theWindow), myPostponedItem (0), myIsFinishProcessing (false)
{
  DFBrowser_SearchLine* aSearchLine = theWindow->GetTreeLevelLine()->GetSearchLine();
  myItems.append (new DFBrowser_ThreadItemSearch(aSearchLine));
  myItems.append (new DFBrowser_ThreadItemUsedShapesMap());
}

// =======================================================================
// function : SetModule
// purpose :
// =======================================================================
void DFBrowser_Thread::SetModule (DFBrowser_Module* theModule)
{
  for (int anItemId = 0, aSize = myItems.size(); anItemId < aSize; anItemId++)
  {
    DFBrowser_ThreadItemUsedShapesMap* aShapesItem = dynamic_cast<DFBrowser_ThreadItemUsedShapesMap*> (myItems[anItemId]);
    if (aShapesItem)
      aShapesItem->SetModule (theModule);
  }
}

// =======================================================================
// function : ProcessApplication
// purpose :
// =======================================================================
void DFBrowser_Thread::ProcessApplication()
{
  for (int anItemId = 0, aSize = myItems.size(); anItemId < aSize; anItemId++)
    startThread (myItems[anItemId]);
}

// =======================================================================
// function : startThread
// purpose :
// =======================================================================
void DFBrowser_Thread::startThread (DFBrowser_ThreadItem* theItem)
{
  DFBrowser_QThread* aThread = new DFBrowser_QThread (this);
  aThread->setItem (theItem);
  aThread->start();
  connect (aThread, SIGNAL (finished()), this, SLOT (onFinished()), Qt::QueuedConnection);
  myStartedThreads.append (aThread);
}

// =======================================================================
// function : TerminateThread
// purpose :
// =======================================================================
void DFBrowser_Thread::TerminateThread()
{
  for (int aThreadsId = 0, aCount = myStartedThreads.size(); aThreadsId < aCount; aThreadsId++)
  {
    QThread* aThread = myStartedThreads[aThreadsId];
    if (aThread->isRunning())
      aThread->terminate();
  }
}

// =======================================================================
// function : onFinished
// purpose :
// =======================================================================
void DFBrowser_Thread::onFinished()
{
  DFBrowser_QThread* aThread = (DFBrowser_QThread*)(sender());
  if (myIsFinishProcessing)
  {
    // if thread send signal when other finished signal is processed
    if (aThread)
      myPostponedItem = aThread->getItem();
    return;
  }

  myIsFinishProcessing = true;
  if (aThread)
  {
    myStartedThreads.removeAll (aThread);
    DFBrowser_ThreadItem* anItem = aThread->getItem();
    if (anItem)
      anItem->ApplyValues();
  }

  myIsFinishProcessing = false;
  if (myPostponedItem)
  {
    myPostponedItem->ApplyValues();
    myPostponedItem = 0;
  }
}
