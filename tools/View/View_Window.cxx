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
  #define QT_CLEAN_NAMESPACE /* avoid definition of INT32 and INT8 */
#endif

#include <inspector/View_Window.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_DisplayPreview.hxx>
#include <inspector/View_PreviewParameters.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>

#include <V3d.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QComboBox>
#include <QDockWidget>
#include <QGridLayout>
#include <QMenu>
#include <QToolBar>
#include <Standard_WarningsRestore.hxx>

const int DEFAULT_SPACING = 3;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_Window::View_Window(QWidget*                              theParent,
                         const Handle(AIS_InteractiveContext)& theContext,
                         const bool                            isUseKeepView,
                         const bool                            isFitAllActive)
    : QWidget(theParent)
{
  QGridLayout* aViewLayout = new QGridLayout(this);
  aViewLayout->setContentsMargins(0, 0, 0, 0);
  aViewLayout->setSpacing(DEFAULT_SPACING);

  myView        = new View_Widget(this, theContext, isFitAllActive);
  myViewToolBar = new View_ToolBar(this, isUseKeepView);
  aViewLayout->addWidget(myViewToolBar->GetControl(), 0, 0, 1, 2);
  connect(myViewToolBar, SIGNAL(contextChanged()), this, SLOT(onViewSelectorActivated()));
  connect(myViewToolBar, SIGNAL(actionClicked(int)), this, SLOT(onToolBarActionClicked(int)));

  connect(myView,
          SIGNAL(checkedStateChanged(int, bool)),
          this,
          SLOT(onCheckedStateChanged(int, bool)));

  myView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(myView,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(onViewContextMenuRequested(const QPoint&)));

  myActionsToolBar = new QToolBar(this);
  myActionsToolBar->layout()->setContentsMargins(0, 0, 0, 0);
  myActionsToolBar->setOrientation(Qt::Vertical);

  myActionsToolBar->addWidget(myView->GetWidget(View_ViewActionType_FitAllId));
  myActionsToolBar->addAction(myView->ViewAction(View_ViewActionType_DisplayModeId));

  aViewLayout->addWidget(myActionsToolBar, 1, 0);
  aViewLayout->addWidget(myView, 1, 1);
  aViewLayout->setRowStretch(1, 1);

  Handle(AIS_InteractiveContext) aContext = myView->GetViewer()->GetContext();
  myViewToolBar->SetContext(View_ContextType_Own, aContext);

  myDisplayer = new View_Displayer();
  if (!isUseKeepView)
    myDisplayer->KeepPresentations(true);
  myDisplayer->SetFitAllActive(isFitAllActive);
  connect(myView, SIGNAL(displayModeClicked()), this, SLOT(onDisplayModeChanged()));
  onViewSelectorActivated();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void View_Window::SetContext(View_ContextType /*theType*/,
                             const Handle(AIS_InteractiveContext)& theContext)
{
  ViewToolBar()->SetContext(View_ContextType_External, theContext);
}

// =======================================================================
// function : SetPredefinedSize
// purpose :
// =======================================================================
void View_Window::SetPredefinedSize(int theDefaultWidth, int theDefaultHeight)
{
  myView->SetPredefinedSize(theDefaultWidth, theDefaultHeight);
}

// =======================================================================
// function : SetInitProj
// purpose :
// =======================================================================
void View_Window::SetInitProj(const Standard_Real theVx,
                              const Standard_Real theVy,
                              const Standard_Real theVz)
{
  myView->SetInitProj(theVx, theVy, theVz);
}

// =======================================================================
// function : View
// purpose :
// =======================================================================
Handle(V3d_View) View_Window::View() const
{
  return myView->GetViewer()->GetView();
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void View_Window::SaveState(View_Window*            theView,
                            QMap<QString, QString>& theItems,
                            const QString&          thePrefix)
{
  QStringList      aCameraDirection;
  Standard_Real    aVX, aVY, aVZ;
  Handle(V3d_View) aView = theView->View();
  if (aView.IsNull())
    return;

  aView->Proj(aVX, aVY, aVZ);
  aCameraDirection << QString::number(aVX) << QString::number(aVY) << QString::number(aVZ);

  theItems[thePrefix + "view_camera_direction"] = aCameraDirection.join(",");

  View_PreviewParameters::SaveState(theView->Displayer()->DisplayPreview()->GetPreviewParameters(),
                                    theItems,
                                    "preview_parameters_");
  View_ToolBar::SaveState(theView->ViewToolBar(), theItems, "view_toolbar_");
  View_Widget::SaveState(theView->ViewWidget(), theItems, "view_widget_");
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool View_Window::RestoreState(View_Window*   theView,
                               const QString& theKey,
                               const QString& theValue,
                               const QString& thePrefix)
{
  if (theKey == thePrefix + "view_camera_direction")
  {
    QStringList aValues = theValue.split(",");
    if (aValues.size() == 3)
    {
      Standard_Real aVX = aValues.at(0).toDouble();
      Standard_Real aVY = aValues.at(1).toDouble();
      Standard_Real aVZ = aValues.at(2).toDouble();

      theView->SetInitProj(aVX, aVY, aVZ);
    }
    return true;
  }
  else if (View_PreviewParameters::RestoreState(
             theView->Displayer()->DisplayPreview()->GetPreviewParameters(),
             theKey,
             theValue,
             "preview_parameters_"))
  {
    return true;
  }
  else if (View_ToolBar::RestoreState(theView->ViewToolBar(), theKey, theValue, "view_toolbar_"))
  {
    return true;
  }
  else if (View_Widget::RestoreState(theView->ViewWidget(), theKey, theValue, "view_widget_"))
  {
    // display mode is restored
    View_Displayer* aDisplayer = theView->Displayer();
    if (theView->ViewWidget()->DisplayMode() != aDisplayer->DisplayMode())
      aDisplayer->SetDisplayMode(theView->ViewWidget()->DisplayMode());

    bool toFitAll = theView->ViewWidget()->IsActionChecked(View_ViewActionType_FitAllId);
    if (toFitAll != aDisplayer->IsFitAllActive())
      aDisplayer->SetFitAllActive(toFitAll);

    return true;
  }

  return false;
}

// =======================================================================
// function : onViewSelectorActivated
// purpose :
// =======================================================================
void View_Window::onViewSelectorActivated()
{
  View_ContextType aType         = myViewToolBar->CurrentContextType();
  bool             isViewEnabled = aType == View_ContextType_Own;

  myView->SetEnabledView(isViewEnabled);

  Handle(AIS_InteractiveContext) aContext = myViewToolBar->CurrentContext();
  myDisplayer->EraseAllPresentations(true);
  emit eraseAllPerformed();

  myDisplayer->SetContext(aContext);
}

// =======================================================================
// function : onToolBarActionClicked
// purpose :
// =======================================================================
void View_Window::onToolBarActionClicked(const int theActionId)
{
  switch (theActionId)
  {
    case View_ToolActionType_Trihedron: {
      myDisplayer->DisplayDefaultTrihedron(myViewToolBar->IsActionChecked(theActionId),
                                           Standard_True);
      break;
    }
    case View_ToolActionType_ViewCube: {
      myDisplayer->DisplayViewCube(myViewToolBar->IsActionChecked(theActionId), Standard_True);
      break;
    }

    case View_ToolActionType_KeepViewId: {
      myDisplayer->KeepPresentations(myViewToolBar->IsActionChecked(theActionId));
      break;
    }
    case View_ToolActionType_KeepViewOffId: {
      myDisplayer->KeepPresentations(!myViewToolBar->IsActionChecked(theActionId));
      break;
    }
    case View_ToolActionType_ClearViewId: {
      myDisplayer->EraseAllPresentations(true);
      emit eraseAllPerformed();
      break;
    }
    default:
      break;
  }
}

// =======================================================================
// function : onCheckedStateChanged
// purpose :
// =======================================================================
void View_Window::onCheckedStateChanged(int theActionId, bool theState)
{
  if (theActionId == View_ViewActionType_FitAllId)
    myDisplayer->SetFitAllActive(theState);
}

// =======================================================================
// function : onViewContextMenuRequested
// purpose :
// =======================================================================
void View_Window::onViewContextMenuRequested(const QPoint& thePosition)
{
  QMenu* aMenu                = new QMenu(this);
  QMenu* anOrientationSubMenu = aMenu->addMenu("Set View Orientation");

  for (int i = 0; i < (int)V3d_XnegYnegZneg; i++)
  {
    V3d_TypeOfOrientation anOrientationType = (V3d_TypeOfOrientation)i;

    QAction* anAction = new QAction(V3d::TypeOfOrientationToString(anOrientationType), this);
    QObject::connect(anAction, SIGNAL(triggered(bool)), this, SLOT(onSetOrientation()));

    anOrientationSubMenu->addAction(anAction);
  }
  aMenu->addMenu(anOrientationSubMenu);

  QPoint aPoint = myView->mapToGlobal(thePosition);
  aMenu->exec(aPoint);
}

// =======================================================================
// function : onSetOrientation
// purpose :
// =======================================================================
void View_Window::onSetOrientation()
{
  QAction* anAction = (QAction*)(sender());

  TCollection_AsciiString anOrientationStr(anAction->text().toStdString().c_str());

  V3d_TypeOfOrientation anOrientationType;
  if (!V3d::TypeOfOrientationFromString(anOrientationStr.ToCString(), anOrientationType))
    return;

  Handle(V3d_View) aView = myView->GetViewer()->GetView();
  if (aView.IsNull())
    return;

  aView->SetProj(anOrientationType);
  aView->FitAll();
  aView->Redraw();
}

// =======================================================================
// function : onDisplayModeChanged
// purpose :
// =======================================================================
void View_Window::onDisplayModeChanged()
{
  int aDisplayMode = myView->DisplayMode();
  for (NCollection_DataMap<View_PresentationType,
                           NCollection_Shared<AIS_ListOfInteractive>>::Iterator
         anIterator(myDisplayer->GetDisplayed());
       anIterator.More();
       anIterator.Next())
    myDisplayer->SetDisplayMode(aDisplayMode, anIterator.Key(), false);
  myDisplayer->UpdateViewer();
}
