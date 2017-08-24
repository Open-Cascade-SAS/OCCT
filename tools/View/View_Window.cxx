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

#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include <inspector/View_Window.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>

#include <QComboBox>
#include <QDockWidget>
#include <QGridLayout>
#include <QToolBar>

const int DEFAULT_SPACING = 3;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_Window::View_Window (QWidget* theParent)
: QWidget (theParent)
{
  QGridLayout* aViewLayout = new QGridLayout (this);
  aViewLayout->setContentsMargins (0, 0, 0, 0);
  aViewLayout->setSpacing (DEFAULT_SPACING);

  myView = new View_Widget (this);
  myViewToolBar = new View_ToolBar (this);
  aViewLayout->addWidget (myViewToolBar->GetControl(), 0, 0, 1, 2);
  connect (myViewToolBar, SIGNAL (contextChanged()), this, SLOT (onViewSelectorActivated()));
  connect (myViewToolBar, SIGNAL (actionClicked (int)),
          this, SLOT (onToolBarActionClicked (int)));

  myActionsToolBar = new QToolBar (this);
  myActionsToolBar->layout()->setContentsMargins (0, 0, 0, 0);
  myActionsToolBar->setOrientation (Qt::Vertical);

  for (int anActionId = View_ViewActionType_FitAllId; anActionId <= View_ViewActionType_DisplayModeId; anActionId++)
    myActionsToolBar->addAction (myView->GetViewAction ((View_ViewActionType)anActionId));
  aViewLayout->addWidget (myActionsToolBar, 1, 0);
  aViewLayout->addWidget (myView, 1, 1);
  aViewLayout->setRowStretch (1, 1);

  Handle(AIS_InteractiveContext) aContext = myView->GetViewer()->GetContext();
  myViewToolBar->SetContext (View_ContextType_Own, aContext);

  myDisplayer = new View_Displayer();
  connect (myView, SIGNAL (displayModeClicked()), this, SLOT (onDisplayModeChanged()));
  onViewSelectorActivated();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void View_Window::SetContext (View_ContextType /*theType*/, const Handle(AIS_InteractiveContext)& theContext)
{
  GetViewToolBar()->SetContext (View_ContextType_External, theContext);
}

// =======================================================================
// function : onViewSelectorActivated
// purpose :
// =======================================================================
void View_Window::onViewSelectorActivated()
{
  View_ContextType aType = myViewToolBar->GetCurrentContextType();
  bool isViewEnabled = aType == View_ContextType_Own;

  myView->SetEnabledView (isViewEnabled);

  Handle(AIS_InteractiveContext) aContext = myViewToolBar->GetCurrentContext();
  myDisplayer->EraseAllPresentations (true);
  myDisplayer->SetContext (aContext);
}

// =======================================================================
// function : onToolBarActionClicked
// purpose :
// =======================================================================
void View_Window::onToolBarActionClicked (const int theActionId)
{
  switch (theActionId)
  {
    case View_ToolActionType_KeepViewId:
    {
      myDisplayer->KeepPresentations (myViewToolBar->IsActionChecked (theActionId));
      break;
    }
    case View_ToolActionType_KeepViewOffId:
    {
      myDisplayer->KeepPresentations (!myViewToolBar->IsActionChecked (theActionId));
      break;
    }
    case View_ToolActionType_ClearViewId:
    {
      myDisplayer->EraseAllPresentations (true);
      break;
    }
    default:
      break;
  }
}

// =======================================================================
// function : onDisplayModeChanged
// purpose :
// =======================================================================
void View_Window::onDisplayModeChanged()
{
  int aDisplayMode = myView->GetDisplayMode();
  for (NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive> >::Iterator
       anIterator(myDisplayer->GetDisplayed()); anIterator.More(); anIterator.Next())
    myDisplayer->SetDisplayMode (aDisplayMode, anIterator.Key(), false);
  myDisplayer->UpdateViewer();
}
