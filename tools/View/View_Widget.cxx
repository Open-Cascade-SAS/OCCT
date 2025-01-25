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

#include <inspector/View_Widget.hxx>

#include <AIS_DisplayMode.hxx>
#include <AIS_ViewController.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <inspector/View_ToolButton.hxx>
#include <inspector/View_ViewActionType.hxx>
#include <inspector/View_Viewer.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColorDialog>
#include <QCursor>
#include <QFileInfo>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include <QStatusBar>
#include <QStyleFactory>
#include <Standard_WarningsRestore.hxx>

#include <stdio.h>

#ifdef _WIN32
  #include <WNT_Window.hxx>
#elif defined(HAVE_XLIB)
  #include <Xw_Window.hxx>
#elif defined(__APPLE__)
  #include <Cocoa_Window.hxx>
#else

#endif

// =======================================================================
// function :  Constructor
// purpose :
// =======================================================================
View_Widget::View_Widget(QWidget*                              theParent,
                         const Handle(AIS_InteractiveContext)& theContext,
                         const bool                            isFitAllActive)
    : QWidget(theParent),
      myFirst(true),
      myDefaultWidth(-1),
      myDefaultHeight(-1),
      myViewIsEnabled(true),
      myHasInitProj(Standard_False),
      myInitVx(0),
      myInitVy(0),
      myInitVz(0)
{
  myViewer = new View_Viewer(View_Viewer::DefaultColor());
  if (!theContext.IsNull())
  {
    myViewer->InitViewer(theContext);
  }
  else
  {
    myViewer->InitViewer(myViewer->CreateStandardViewer());
  }
  myController = new AIS_ViewController();

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  setMouseTracking(true);
  setBackgroundRole(QPalette::NoRole);
  // set focus policy to threat QContextMenuEvent from keyboard
  setFocusPolicy(Qt::StrongFocus);

  initViewActions();
  ((View_ToolButton*)myFitAllAction)->SetButtonChecked(isFitAllActive);
}

// =======================================================================
// function : SetPredefinedSize
// purpose :
// =======================================================================
void View_Widget::SetPredefinedSize(int theDefaultWidth, int theDefaultHeight)
{
  myDefaultWidth  = theDefaultWidth;
  myDefaultHeight = theDefaultHeight;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void View_Widget::Init()
{
  myViewer->CreateView();

#ifdef _WIN32
  Aspect_Handle         aWindowHandle = (Aspect_Handle)winId();
  Handle(Aspect_Window) aWnd          = new WNT_Window(aWindowHandle);
#elif defined(HAVE_XLIB)
  Aspect_Drawable                  aWindowHandle = (Aspect_Drawable)winId();
  Handle(Aspect_DisplayConnection) aDispConnection =
    myViewer->GetContext()->CurrentViewer()->Driver()->GetDisplayConnection();
  Handle(Aspect_Window) aWnd = new Xw_Window(aDispConnection, aWindowHandle);
#elif defined(__APPLE__)
  NSView*               aViewHandle = (NSView*)winId();
  Handle(Aspect_Window) aWnd        = new Cocoa_Window(aViewHandle);
#else
  //
#endif
  myViewer->SetWindow(aWnd);

  myViewer->GetView()->SetBackgroundColor(View_Viewer::DefaultColor());
  myViewer->GetView()->MustBeResized();

  if (myHasInitProj)
    myViewer->GetView()->SetProj(myInitVx, myInitVy, myInitVz);
}

// =======================================================================
// function : DisplayMode
// purpose :
// =======================================================================
int View_Widget::DisplayMode() const
{
  return myViewActions[View_ViewActionType_DisplayModeId]->isChecked() ? AIS_Shaded : AIS_WireFrame;
}

// =======================================================================
// function : SetDisplayMode
// purpose :
// =======================================================================
void View_Widget::SetDisplayMode(const int theMode)
{
  myViewActions[View_ViewActionType_DisplayModeId]->setChecked(theMode ? AIS_Shaded
                                                                       : AIS_WireFrame);
}

// =======================================================================
// function : paintEvent
// purpose :
// =======================================================================
void View_Widget::paintEvent(QPaintEvent* /*theEvent*/)
{
#if (QT_VERSION < 0x050000 || QT_VERSION >= 0x050700)
  if (myFirst)
  {
    Init();
    myFirst = false;
  }
#endif

  if (myViewer->GetView())
    myViewer->GetView()->Redraw();
}

// =======================================================================
// function : resizeEvent
// purpose :
// =======================================================================
void View_Widget::resizeEvent(QResizeEvent* /*theEvent*/)
{
#if (QT_VERSION > 0x050000 && QT_VERSION < 0x050700)
  if (myFirst)
  {
    Init();
    myFirst = false;
  }
#endif
  if (myViewer->GetView())
    myViewer->GetView()->MustBeResized();
}

// =======================================================================
// function : sizeHint
// purpose :
// =======================================================================
QSize View_Widget::sizeHint() const
{
  if (myDefaultWidth > 0 && myDefaultHeight > 0)
    return QSize(myDefaultWidth, myDefaultHeight);

  return QWidget::sizeHint();
}

// =======================================================================
// function : SetEnabledView
// purpose :
// =======================================================================
void View_Widget::SetEnabledView(const bool theIsEnabled)
{
  myViewIsEnabled = theIsEnabled;

  if (myViewer->GetView())
    myViewer->GetView()->SetBackgroundColor(theIsEnabled ? View_Viewer::DefaultColor()
                                                         : View_Viewer::DisabledColor());
  ViewAction(View_ViewActionType_DisplayModeId)->setEnabled(theIsEnabled);
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void View_Widget::SaveState(View_Widget*            theWidget,
                            QMap<QString, QString>& theItems,
                            const QString&          thePrefix)
{
  theItems[thePrefix + "fitall"] = theWidget->ViewAction(View_ViewActionType_FitAllId)->isChecked();
  theItems[thePrefix + "dispmode"] = QString::number(theWidget->DisplayMode());
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool View_Widget::RestoreState(View_Widget*   theWidget,
                               const QString& theKey,
                               const QString& theValue,
                               const QString& thePrefix)
{
  if (theKey == thePrefix + "fitall")
  {
    theWidget->SetActionChecked(View_ViewActionType_FitAllId, theValue.toInt() > 0);
  }
  else if (theKey == thePrefix + "dispmode")
  {
    theWidget->SetDisplayMode(theValue.toInt());
  }
  else
    return false;

  return true;
}

// =======================================================================
// function : onCheckedStateChanged
// purpose :
// =======================================================================
void View_Widget::onCheckedStateChanged(bool isOn)
{
  QWidget* aSentByAction = (QWidget*)sender();

  if (aSentByAction == myFitAllAction)
    emit checkedStateChanged(View_ViewActionType_FitAllId, isOn);
}

// =======================================================================
// function : initViewActions
// purpose :
// =======================================================================
void View_Widget::initViewActions()
{
  if (!myViewActions.empty())
    return;

  myFitAllAction = new View_ToolButton(this); // action for automatic fit all
  connect(myFitAllAction,
          SIGNAL(checkedStateChanged(bool)),
          this,
          SLOT(onCheckedStateChanged(bool)));
  createAction(View_ViewActionType_FitAllId,
               ":/icons/view_fitall.png",
               tr("Fit All"),
               SLOT(OnFitAll()));
  myFitAllAction->setDefaultAction(ViewAction(View_ViewActionType_FitAllId));

  createAction(View_ViewActionType_DisplayModeId,
               ":/icons/view_dm_shading.png",
               tr("Display Mode"),
               SIGNAL(displayModeClicked()),
               true);
}

// =======================================================================
// function : mousePressEvent
// purpose :
// =======================================================================
void View_Widget::mousePressEvent(QMouseEvent* theEvent)
{
  if (myController->PressMouseButton(Graphic3d_Vec2i(theEvent->x(), theEvent->y()),
                                     keyMouse(theEvent->button()),
                                     keyFlag(theEvent->modifiers()),
                                     Standard_False))
  {
    myController->FlushViewEvents(myViewer->GetContext(), myViewer->GetView(), Standard_True);
  }
}

// =======================================================================
// function : mouseReleaseEvent
// purpose :
// =======================================================================
void View_Widget::mouseReleaseEvent(QMouseEvent* theEvent)
{
  if (myController->ReleaseMouseButton(Graphic3d_Vec2i(theEvent->x(), theEvent->y()),
                                       keyMouse(theEvent->button()),
                                       keyFlag(theEvent->modifiers()),
                                       Standard_False))
  {
    myController->FlushViewEvents(myViewer->GetContext(), myViewer->GetView(), Standard_True);
  }
}

// =======================================================================
// function : mouseMoveEvent
// purpose :
// =======================================================================
void View_Widget::mouseMoveEvent(QMouseEvent* theEvent)
{
  if (myViewer->GetView().IsNull())
  {
    return;
  }

  myController->UpdateMousePosition(Graphic3d_Vec2i(theEvent->x(), theEvent->y()),
                                    keyMouse(theEvent->button()),
                                    keyFlag(theEvent->modifiers()),
                                    Standard_False);

  myController->FlushViewEvents(myViewer->GetContext(), myViewer->GetView(), Standard_True);
}

// =======================================================================
// function : createAction
// purpose :
// =======================================================================
void View_Widget::createAction(const View_ViewActionType theActionId,
                               const QString&            theIcon,
                               const QString&            theText,
                               const char*               theSlot,
                               const bool                isCheckable,
                               const QString&            theToolBar,
                               const QString&            theStatusBar)
{
  QAction* anAction = new QAction(QIcon(theIcon), theText, this);
  anAction->setToolTip(!theToolBar.isEmpty() ? theToolBar : theText);
  anAction->setStatusTip(!theStatusBar.isEmpty() ? theStatusBar : theText);
  if (isCheckable)
    anAction->setCheckable(true);
  connect(anAction, SIGNAL(triggered()), this, theSlot);
  myViewActions[theActionId] = anAction;
}

// =======================================================================
// function : keyFlag
// purpose :
// =======================================================================
Aspect_VKeyFlags View_Widget::keyFlag(const int theModifierId)
{
  switch (theModifierId)
  {
    case Qt::NoModifier:
      return Aspect_VKeyFlags_NONE;
    case Qt::ShiftModifier:
      return Aspect_VKeyFlags_SHIFT;
    case Qt::ControlModifier:
      return Aspect_VKeyFlags_CTRL;
    default:
      break;
  }
  return Aspect_VKeyFlags_NONE;
}

// =======================================================================
// function : keyMouse
// purpose :
// =======================================================================
Aspect_VKeyMouse View_Widget::keyMouse(const int theButtonId)
{
  switch (theButtonId)
  {
    case Qt::NoButton:
      return Aspect_VKeyMouse_NONE;
    case Qt::LeftButton:
      return Aspect_VKeyMouse_LeftButton;
    case Qt::RightButton:
      return Aspect_VKeyMouse_RightButton;
    case Qt::MidButton:
      return Aspect_VKeyMouse_MiddleButton;
    default:
      break;
  }
  return Aspect_VKeyMouse_NONE;
}
