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

#include <inspector/View_Widget.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Standard_Version.hxx>
#include <inspector/View_ViewActionType.hxx>
#include <inspector/View_Viewer.hxx>

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

#include <stdio.h>

#ifdef _WIN32
#include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
#include <Cocoa_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier
// the key for shortcut (use to activate dynamic onRotate, panning)
#define CASCADESHORTCUTKEY Qt::ControlModifier
// for elastic bean selection
#define ValZWMin 1

// =======================================================================
// function :  Constructor
// purpose :
// =======================================================================
View_Widget::View_Widget (QWidget* theParent)
: QWidget (theParent), myCurrentMode (View_CurrentAction3d_Nothing), myFirst (true), myDefaultWidth (-1),
  myDefaultHeight (-1), myViewIsEnabled (true), myXmin (0), myYmin (0), myXmax (0), myYmax (0), myDragButtonDownX (0),
  myDragButtonDownY (0), myDragMultiButtonDownX (0), myDragMultiButtonDownY (0), myIsRectVisible (false), myRectBand (0)
{
  myViewer = new View_Viewer (View_Viewer::DefaultColor());
  myViewer->InitStandardViewer();

  setAttribute (Qt::WA_PaintOnScreen);
  setAttribute (Qt::WA_NoSystemBackground);

  setMouseTracking (true);
  setBackgroundRole (QPalette::NoRole);
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy (Qt::StrongFocus);

  initViewActions();
  initCursors();
}

// =======================================================================
// function : SetPredefinedSize
// purpose :
// =======================================================================
void View_Widget::SetPredefinedSize (int theDefaultWidth, int theDefaultHeight)
{
  myDefaultWidth = theDefaultWidth;
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
  Aspect_Handle aWindowHandle = (Aspect_Handle)winId();
  Handle(Aspect_Window) aWnd = new WNT_Window (aWindowHandle);
#if OCC_VERSION_HEX <= 0x060901
  myViewer->GetView()->SetZClippingDepth (0.5);
  myViewer->GetView()->SetZClippingWidth (0.5);
#endif
#elif defined (__APPLE__) && !defined (MACOSX_USE_GLX)
  NSView* aViewHandle = (NSView*)winId();
  Handle(Aspect_Window) aWnd = new Cocoa_Window (aViewHandle);
#else
  Window aWindowHandle = (Window)winId();
  Handle(Aspect_DisplayConnection) aDispConnection = myViewer->GetContext()->CurrentViewer()->Driver()->GetDisplayConnection();
  Handle(Aspect_Window) aWnd = new Xw_Window (aDispConnection, aWindowHandle);
#endif
  myViewer->SetWindow (aWnd);

  myViewer->GetView()->SetBackgroundColor (View_Viewer::DefaultColor());
  myViewer->GetView()->MustBeResized();
}

// =======================================================================
// function : GetDisplayMode
// purpose :
// =======================================================================
int View_Widget::GetDisplayMode() const
{
  return myViewActions[View_ViewActionType_DisplayModeId]->isChecked() ? AIS_Shaded : AIS_WireFrame;
}

// =======================================================================
// function : paintEvent
// purpose :
// =======================================================================
void View_Widget::paintEvent (QPaintEvent* /*theEvent*/)
{
#if QT_VERSION < 0x050000
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
void View_Widget::resizeEvent (QResizeEvent* /*theEvent*/)
{
#if QT_VERSION > 0x050000
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
    return QSize (myDefaultWidth, myDefaultHeight);

  return QWidget::sizeHint();
}

// =======================================================================
// function : SetEnabledView
// purpose :
// =======================================================================
void View_Widget::SetEnabledView (const bool theIsEnabled)
{
  myViewIsEnabled = theIsEnabled;

  if (myViewer->GetView())
    myViewer->GetView()->SetBackgroundColor (theIsEnabled ? View_Viewer::DefaultColor()
                                                          : View_Viewer::DisabledColor());
  for (int anActionId = View_ViewActionType_FitAllId; anActionId <= View_ViewActionType_DisplayModeId; anActionId++)
    GetViewAction ((View_ViewActionType)anActionId)->setEnabled (theIsEnabled);
}

// =======================================================================
// function : OnUpdateToggled
// purpose :
// =======================================================================
void View_Widget::OnUpdateToggled (bool isOn)
{
  QAction* sentBy = (QAction*)sender();

  if (sentBy == myViewActions[View_ViewActionType_DisplayModeId])
  {
    sentBy->setIcon (isOn ? QIcon (":/icons/view_dm_wireframe.png")
                          : QIcon (":/icons/view_dm_shading.png"));
    return;
  }

  if (!isOn)
    return;

  for (int anActionId = View_ViewActionType_FitAllId; anActionId <= View_ViewActionType_RotationId; anActionId++)
  {
    QAction* anAction = myViewActions[(View_ViewActionType)anActionId];
    if ((anAction == myViewActions[View_ViewActionType_FitAreaId]) ||
        (anAction == myViewActions[View_ViewActionType_ZoomId]) ||
        (anAction == myViewActions[View_ViewActionType_PanId]) ||
        (anAction == myViewActions[View_ViewActionType_RotationId]))
    {
      if (anAction && (anAction != sentBy))
      {
        anAction->setChecked (false);
      }
      else
      {
        if (sentBy == myViewActions[View_ViewActionType_FitAreaId])
          setActiveCursor (View_CursorMode_HandCursor);
        else if (sentBy == myViewActions[View_ViewActionType_ZoomId])
          setActiveCursor (View_CursorMode_ZoomCursor);
        else if (sentBy == myViewActions[View_ViewActionType_PanId])
          setActiveCursor (View_CursorMode_PanCursor);
        else if (sentBy == myViewActions[View_ViewActionType_RotationId])
          setActiveCursor (View_CursorMode_RotationCursor);
        else
          setActiveCursor (View_CursorMode_DefaultCursor);
      }
    }
  }
}

// =======================================================================
// function : initViewActions
// purpose :
// =======================================================================
void View_Widget::initViewActions()
{
  if (!myViewActions.empty())
    return;

  createAction (View_ViewActionType_FitAllId, ":/icons/view_fitall.png", tr ("Fit All"), SLOT (OnFitAll()));
  createAction (View_ViewActionType_FitAreaId, ":/icons/view_fitarea.png", tr ("Fit Area"), SLOT (OnFitArea()), true);
  createAction (View_ViewActionType_ZoomId, ":/icons/view_zoom.png", tr ("Zoom"), SLOT (OnZoom()), true);
  createAction (View_ViewActionType_PanId, ":/icons/view_pan.png", tr ("Pan"), SLOT (OnPan()), true);
  createAction (View_ViewActionType_RotationId, ":/icons/view_rotate.png", tr ("Rotation"), SLOT (OnRotate()), true);
  createAction (View_ViewActionType_DisplayModeId, ":/icons/view_dm_shading.png", tr ("Display Mode"),
                SIGNAL (displayModeClicked()), true);

  for (int anActionId = View_ViewActionType_FitAreaId; anActionId <= View_ViewActionType_RotationId; anActionId++)
    connect (myViewActions[(View_ViewActionType)anActionId], SIGNAL (toggled(bool)), this, SLOT (OnUpdateToggled(bool)));
}

// =======================================================================
// function : initCursors
// purpose :
// =======================================================================
void View_Widget::initCursors()
{
  if (!myCursors.empty())
    return;

  myCursors[View_CursorMode_DefaultCursor] = QCursor (Qt::ArrowCursor);
  myCursors[View_CursorMode_HandCursor] = QCursor (Qt::PointingHandCursor);
  myCursors[View_CursorMode_PanCursor] = QCursor (Qt::SizeAllCursor);
  myCursors[View_CursorMode_ZoomCursor] = QCursor(QIcon (":/icons/cursor_zoom.png").pixmap (20, 20));
  myCursors[View_CursorMode_RotationCursor] = QCursor(QIcon (":/icons/cursor_rotate.png").pixmap (20, 20));
}

// =======================================================================
// function : mousePressEvent
// purpose :
// =======================================================================
void View_Widget::mousePressEvent (QMouseEvent* theEvent)
{
  if (theEvent->button() == Qt::LeftButton)
    processLeftButtonDown (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
  else if (theEvent->button() == Qt::MidButton)
    processMiddleButtonDown (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
  else if (theEvent->button() == Qt::RightButton)
    processRightButtonDown (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
}

// =======================================================================
// function : mouseReleaseEvent
// purpose :
// =======================================================================
void View_Widget::mouseReleaseEvent (QMouseEvent* theEvent)
{
  if (theEvent->button() == Qt::LeftButton)
    processLeftButtonUp (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
  else if (theEvent->button() == Qt::MidButton)
    processMiddleButtonUp (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
  else if (theEvent->button() == Qt::RightButton)
    processRightButtonUp (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
}

// =======================================================================
// function : mouseMoveEvent
// purpose :
// =======================================================================
void View_Widget::mouseMoveEvent (QMouseEvent* theEvent)
{
  processMouseMove (theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
}

// =======================================================================
// function : activateCursor
// purpose :
// =======================================================================
void View_Widget::activateCursor (const View_CurrentAction3d theMode)
{
  switch (theMode)
  {
    case View_CurrentAction3d_DynamicPanning:
    {
      setActiveCursor (View_CursorMode_PanCursor);
      break;
    }
    case View_CurrentAction3d_DynamicZooming:
    {
      setActiveCursor (View_CursorMode_ZoomCursor);
      break;
    }
    case View_CurrentAction3d_DynamicRotation:
    {
      setActiveCursor (View_CursorMode_RotationCursor);
      break;
    }
    case View_CurrentAction3d_WindowZooming:
    {
      setActiveCursor (View_CursorMode_HandCursor);
      break;
    }
    case View_CurrentAction3d_Nothing:
    default:
    {
      setActiveCursor (View_CursorMode_DefaultCursor);
      break;
    }
  }
}

// =======================================================================
// function : processLeftButtonDown
// purpose :
// =======================================================================
void View_Widget::processLeftButtonDown (const int theFlags, const QPoint thePoint)
{
  //  save the current mouse coordinate in min
  myXmin = thePoint.x();
  myYmin = thePoint.y();
  myXmax = thePoint.x();
  myYmax = thePoint.y();

  if (theFlags & CASCADESHORTCUTKEY)
  {
    myCurrentMode = View_CurrentAction3d_DynamicZooming;
    OnUpdateToggled(true);
  }
  else
  {
    switch (myCurrentMode)
    {
      case View_CurrentAction3d_Nothing:
      {
        if (theFlags & MULTISELECTIONKEY)
          processDragMultiEvent (myXmax, myYmax, View_DragMode_ButtonDown);
        else
          processDragEvent (myXmax, myYmax, View_DragMode_ButtonDown);
        break;
      }
      case View_CurrentAction3d_DynamicZooming:
      case View_CurrentAction3d_WindowZooming:
      case View_CurrentAction3d_DynamicPanning:
        break;
      case View_CurrentAction3d_DynamicRotation:
      {
        myViewer->GetView()->StartRotation (thePoint.x(), thePoint.y());
        break;
      }
      default:
      {
        throw Standard_ProgramError ("View_Widget::processLeftButtonDown : Incompatible Current Mode");
        break;
      }
    }
  }
  activateCursor (myCurrentMode);
}

// =======================================================================
// function : processMiddleButtonDown
// purpose :
// =======================================================================
void View_Widget::processMiddleButtonDown (const int theFlags, const QPoint /*thePoint*/)
{
  if (theFlags & CASCADESHORTCUTKEY) {
    myCurrentMode = View_CurrentAction3d_DynamicPanning;
    OnUpdateToggled(true);
  }
  activateCursor (myCurrentMode);
}

// =======================================================================
// function : processRightButtonDown
// purpose :
// =======================================================================
void View_Widget::processRightButtonDown (const int theFlags, const QPoint thePoint)
{
  if (theFlags & CASCADESHORTCUTKEY)
  {
    myCurrentMode = View_CurrentAction3d_DynamicRotation;
    myViewer->GetView()->StartRotation (thePoint.x(), thePoint.y());
    OnUpdateToggled(true);
  }
  else
  {
    popup (thePoint.x(), thePoint.y());
  }
  activateCursor (myCurrentMode);
}

// =======================================================================
// function : processLeftButtonUp
// purpose :
// =======================================================================
void View_Widget::processLeftButtonUp (const int theFlags, const QPoint thePoint)
{
  switch (myCurrentMode)
  {
    case View_CurrentAction3d_Nothing:
    {
      if (thePoint.x() == myXmin && thePoint.y() == myYmin)
      {
        // no offset between down and up --> selectEvent
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        if (theFlags & MULTISELECTIONKEY)
          processInputMultiEvent (thePoint.x(), thePoint.y());
        else
          processInputEvent (thePoint.x(), thePoint.y());
      }
      else
      {
        drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_False);
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        if (theFlags & MULTISELECTIONKEY)
          processDragMultiEvent (thePoint.x(), thePoint.y(), View_DragMode_ButtonUp);
        else
          processDragEvent (thePoint.x(), thePoint.y(), View_DragMode_ButtonUp);
      }
      break;
    }
    case View_CurrentAction3d_DynamicZooming:
    break;
    case View_CurrentAction3d_WindowZooming:
    {
      drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_False);
      myXmax = thePoint.x();
      myYmax = thePoint.y();
      if ((abs(myXmin - myXmax) > ValZWMin) ||
          (abs(myYmin - myYmax) > ValZWMin))
        myViewer->GetView()->WindowFitAll (myXmin, myYmin, myXmax, myYmax);
      break;
    }
    case View_CurrentAction3d_DynamicPanning:
    break;
    case View_CurrentAction3d_DynamicRotation:
    break;
    default:
    {
      throw Standard_ProgramError("View_Widget::processLeftButtonUp : Incompatible Current Mode");
      break;
    }
  }
  myDragButtonDownX = 0;
  myDragButtonDownY = 0;
  myDragMultiButtonDownX = 0;
  myDragMultiButtonDownY = 0;

  activateCursor (myCurrentMode);
  emit selectionChanged();
}

// =======================================================================
// function : processMiddleButtonUp
// purpose :
// =======================================================================
void View_Widget::processMiddleButtonUp (const int /*theFlags*/, const QPoint /*thePoint*/)
{
  myCurrentMode = View_CurrentAction3d_Nothing;
  activateCursor (myCurrentMode);
}

// =======================================================================
// function : processRightButtonUp
// purpose :
// =======================================================================
void View_Widget::processRightButtonUp (const int /*theFlags*/, const QPoint thePoint)
{
  if (myCurrentMode == View_CurrentAction3d_Nothing)
  {
    popup (thePoint.x(), thePoint.y());
  }
  else
    myCurrentMode = View_CurrentAction3d_Nothing;
  activateCursor (myCurrentMode);
}

// =======================================================================
// function : processMouseMove
// purpose :
// =======================================================================
void View_Widget::processMouseMove (const int theFlags, const QPoint thePoint)
{
  if (theFlags & Qt::LeftButton || theFlags & Qt::RightButton || theFlags & Qt::MidButton)
  {
    switch (myCurrentMode)
    {
      case View_CurrentAction3d_Nothing:
      {
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_False);
        if (theFlags & MULTISELECTIONKEY)
          processDragMultiEvent (myXmax, myYmax, View_DragMode_ButtonMove);
        else
          processDragEvent (myXmax, myYmax, View_DragMode_ButtonMove);
        drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_True);
        break;
      }
      case View_CurrentAction3d_DynamicZooming:
      {
        myViewer->GetView()->Zoom (myXmax, myYmax, thePoint.x(), thePoint.y());
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        break;
      }
      case View_CurrentAction3d_WindowZooming:
      {
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_False);
        drawRectangle (myXmin, myYmin, myXmax, myYmax, Standard_True);
        break;
      }
      case View_CurrentAction3d_DynamicPanning:
      {
        myViewer->GetView()->Pan (thePoint.x() - myXmax, myYmax - thePoint.y());
        myXmax = thePoint.x();
        myYmax = thePoint.y();
        break;
      }
      case View_CurrentAction3d_DynamicRotation:
      {
        myViewer->GetView()->Rotation (thePoint.x(), thePoint.y());
        myViewer->GetView()->Redraw();
        break;
      }
      default:
      {
        throw Standard_ProgramError("View_Widget::processMouseMove : Incompatible Current Mode");
        break;
      }
    }
  }
  else
  {
    myXmax = thePoint.x();
    myYmax = thePoint.y();
    if (theFlags & MULTISELECTIONKEY)
      processMoveMultiEvent (thePoint.x(), thePoint.y());
     else
      processMoveEvent (thePoint.x(), thePoint.y());
  }
}

// =======================================================================
// function : processDragEvent
// purpose :
// =======================================================================
void View_Widget::processDragEvent (const Standard_Integer theX, const Standard_Integer theY, const View_DragMode& theState)
{
  //myDragButtonDownX = 0;
  //myDragButtonDownY = 0;

  switch (theState)
  {
    case View_DragMode_ButtonDown:
    {
      myDragButtonDownX = theX;
      myDragButtonDownY = theY;
      break;
    }
    case View_DragMode_ButtonMove:
    break;
    case View_DragMode_ButtonUp:
    {
      myViewer->GetContext()->Select (myDragButtonDownX, myDragButtonDownY, theX, theY, myViewer->GetView(), Standard_True);
      emit selectionChanged();
      break;
    }
    default:
      break;
  }
}

// =======================================================================
// function : processInputEvent
// purpose :
// =======================================================================
void View_Widget::processInputEvent (const Standard_Integer/* theX*/, const Standard_Integer/* theY*/)
{
  myViewer->GetContext()->Select (Standard_True);
  emit selectionChanged();
}

// =======================================================================
// function : processMoveEvent
// purpose :
// =======================================================================
void View_Widget::processMoveEvent (const Standard_Integer theX, const Standard_Integer theY)
{
  myViewer->GetContext()->MoveTo (theX, theY, myViewer->GetView(), Standard_True);
}

// =======================================================================
// function : processDragMultiEvent
// purpose :
// =======================================================================
void View_Widget::processDragMultiEvent (const Standard_Integer theX, const Standard_Integer theY,
                                         const View_DragMode& theState)
{
  switch (theState)
  {
    case View_DragMode_ButtonDown:
    {
      myDragMultiButtonDownX = theX;
      myDragMultiButtonDownY = theY;
      break;
    }
    case View_DragMode_ButtonMove:
    {
      myViewer->GetContext()->ShiftSelect (myDragMultiButtonDownX, myDragMultiButtonDownY, theX, theY,
                                           myViewer->GetView(), Standard_True);
      emit selectionChanged();
      break;
    }
    case View_DragMode_ButtonUp:
    default:
      break;
  }
}

// =======================================================================
// function : processInputMultiEvent
// purpose :
// =======================================================================
void View_Widget::processInputMultiEvent (const Standard_Integer /*theX*/, const Standard_Integer /*theY*/)
{
  myViewer->GetContext()->ShiftSelect (Standard_True);
  emit selectionChanged();
}

// =======================================================================
// function : drawRectangle
// purpose :
// =======================================================================
void View_Widget::drawRectangle (const Standard_Integer theMinX, const Standard_Integer MinY,
                                 const Standard_Integer MaxX, const Standard_Integer MaxY,
                                 const Standard_Boolean theToDraw)
{
  Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;

  StoredMinX = (theMinX < MaxX) ? theMinX : MaxX;
  StoredMinY = (MinY < MaxY) ? MinY : MaxY;
  StoredMaxX = (theMinX > MaxX) ? theMinX : MaxX;
  StoredMaxY = (MinY > MaxY) ? MinY : MaxY;

  QRect aRect;
  aRect.setRect(StoredMinX, StoredMinY, abs (StoredMaxX-StoredMinX), abs (StoredMaxY-StoredMinY));

  if (!myRectBand) 
  {
    myRectBand = new QRubberBand (QRubberBand::Rectangle, this);
    myRectBand->setStyle (QStyleFactory::create ("windows"));
    myRectBand->setGeometry (aRect);
    myRectBand->show();
  }

  if (myIsRectVisible && !theToDraw) // move or up  : erase at the old position
  {
    myRectBand->hide();
    delete myRectBand;
    myRectBand = 0;
    myIsRectVisible = false;
  }

  if (theToDraw) // move : draw
  {
    myIsRectVisible = true;
    myRectBand->setGeometry (aRect);
  }
}

// =======================================================================
// function : createAction
// purpose :
// =======================================================================
void View_Widget::createAction (const View_ViewActionType theActionId, const QString& theIcon, const QString& theText,
                                const char* theSlot, const bool isCheckable, const QString& theToolBar,
                                const QString& theStatusBar)
{
  QAction* anAction = new QAction (QIcon (theIcon), theText, this);
  anAction->setToolTip (!theToolBar.isEmpty() ? theToolBar : theText);
  anAction->setStatusTip (!theStatusBar.isEmpty() ? theStatusBar : theText);
  if (isCheckable)
    anAction->setCheckable (true);
  connect(anAction, SIGNAL (triggered()) , this, theSlot);
  myViewActions[theActionId] = anAction;
}

// =======================================================================
// function : setActiveCursor
// purpose :
// =======================================================================
void View_Widget::setActiveCursor (const View_CursorMode& theMode)
{
  QCursor aCursor = myCursors[theMode];
  setCursor (myCursors[theMode]);
}
