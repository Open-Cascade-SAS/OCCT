// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "View.h"

#include "ApplicationCommon.h"
#include "OcctWindow.h"
#include "Transparency.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QMdiSubWindow>
#include <QColorDialog>
#include <QCursor>
#include <QPainter>
#include <QStyleFactory>
#include <Standard_WarningsRestore.hxx>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
#include <QX11Info>
#endif

#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>

namespace
{
  //! Map Qt buttons bitmask to virtual keys.
  static Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons)
  {
    Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
    if ((theButtons & Qt::LeftButton) != 0)
    {
      aButtons |= Aspect_VKeyMouse_LeftButton;
    }
    if ((theButtons & Qt::MiddleButton) != 0)
    {
      aButtons |= Aspect_VKeyMouse_MiddleButton;
    }
    if ((theButtons & Qt::RightButton) != 0)
    {
      aButtons |= Aspect_VKeyMouse_RightButton;
    }
    return aButtons;
  }

  //! Map Qt mouse modifiers bitmask to virtual keys.
  static Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers)
  {
    Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
    if ((theModifiers & Qt::ShiftModifier) != 0)
    {
      aFlags |= Aspect_VKeyFlags_SHIFT;
    }
    if ((theModifiers & Qt::ControlModifier) != 0)
    {
      aFlags |= Aspect_VKeyFlags_CTRL;
    }
    if ((theModifiers & Qt::AltModifier) != 0)
    {
      aFlags |= Aspect_VKeyFlags_ALT;
    }
    return aFlags;
  }

  static QCursor* defCursor = NULL;
  static QCursor* handCursor = NULL;
  static QCursor* panCursor = NULL;
  static QCursor* globPanCursor = NULL;
  static QCursor* zoomCursor = NULL;
  static QCursor* rotCursor = NULL;

}

View::View (const Handle(AIS_InteractiveContext)& theContext, bool theIs3dView, QWidget* theParent)
: QWidget(theParent),
  myIsRaytracing(false),
  myIsShadowsEnabled(true),
  myIsReflectionsEnabled(false),
  myIsAntialiasingEnabled(false),
  myIs3dView (theIs3dView),
  myBackMenu(NULL)
{
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  XSynchronize(x11Info().display(), true);
#endif
  myContext = theContext;
  myCurZoom = 0;

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  myDefaultGestures = myMouseGestureMap;
  myCurrentMode = CurrentAction3d_Nothing;
  setMouseTracking(true);

  initViewActions();
  initCursors();

  setBackgroundRole(QPalette::NoRole);//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard
  setFocusPolicy(Qt::StrongFocus);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  init();
}

void View::init()
{
  if (myV3dView.IsNull())
  {
    myV3dView = myContext->CurrentViewer()->CreateView();
  }

  Handle(OcctWindow) hWnd = new OcctWindow(this);
  myV3dView->SetWindow(hWnd);
  if (!hWnd->IsMapped())
  {
    hWnd->Map();
  }

  if (myIs3dView)
  {
    SetAllowRotation(Standard_True);
    myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.0, 0.3, Quantity_TOC_RGB));
  }
  else
  {
    SetAllowRotation(Standard_False);
    myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.2, 0.0, Quantity_TOC_RGB));
    myV3dView->SetProj(V3d_Zpos);
  }

  myV3dView->MustBeResized();

  if (myIsRaytracing)
  {
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
  }
}

void View::paintEvent(QPaintEvent *)
{
  myV3dView->InvalidateImmediate();
  FlushViewEvents(myContext, myV3dView, true);
}

void View::resizeEvent(QResizeEvent *)
{
  if (!myV3dView.IsNull())
  {
    myV3dView->MustBeResized();
  }
}

void View::OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx,
                              const Handle(V3d_View)& theView)
{
  Q_UNUSED(theCtx)
  Q_UNUSED(theView)
}

void View::fitAll()
{
  myV3dView->FitAll();
  myV3dView->ZFitAll();
  myV3dView->Redraw();
}

void View::axo()
{
  if (myIs3dView)
  {
    myV3dView->SetProj(V3d_XposYnegZpos);
  }
}

void View::hlrOff()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myV3dView->SetComputedMode(Standard_False);
  myV3dView->Redraw();
  QAction* aShadingAction = getViewAction(ViewAction_Shading);
  aShadingAction->setEnabled(true);
  QAction* aWireframeAction = getViewAction(ViewAction_Wireframe);
  aWireframeAction->setEnabled(true);
  QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myV3dView->SetComputedMode(Standard_True);
  myV3dView->Redraw();
  QAction* aShadingAction = getViewAction(ViewAction_Shading);
  aShadingAction->setEnabled(false);
  QAction* aWireframeAction = getViewAction(ViewAction_Wireframe);
  aWireframeAction->setEnabled(false);
  QApplication::restoreOverrideCursor();
}

void View::shading()
{
  myContext->SetDisplayMode(1, Standard_True);
}

void View::wireframe()
{
  myContext->SetDisplayMode(0, Standard_True);
}

void View::SetRaytracedShadows(bool theState)
{
  myV3dView->ChangeRenderingParams().IsShadowEnabled = theState;
  myIsShadowsEnabled = theState;
  myContext->UpdateCurrentViewer();
}

void View::SetRaytracedReflections(bool theState)
{
  myV3dView->ChangeRenderingParams().IsReflectionEnabled = theState;
  myIsReflectionsEnabled = theState;
  myContext->UpdateCurrentViewer();
}

void View::onRaytraceAction()
{
  QAction* aSentBy = (QAction*)sender();

  if (aSentBy == myRaytraceActions.value(RaytraceAction_Raytracing))
  {
    bool aState = myRaytraceActions.value(RaytraceAction_Raytracing)->isChecked();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (aState)
      EnableRaytracing();
    else
      DisableRaytracing();
    QApplication::restoreOverrideCursor();
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction_Shadows))
  {
    bool aState = myRaytraceActions.value(RaytraceAction_Shadows)->isChecked();
    SetRaytracedShadows(aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction_Reflections))
  {
    bool aState = myRaytraceActions.value(RaytraceAction_Reflections)->isChecked();
    SetRaytracedReflections(aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction_Antialiasing))
  {
    bool aState = myRaytraceActions.value(RaytraceAction_Antialiasing)->isChecked();
    SetRaytracedAntialiasing(aState);
  }
}

void View::SetRaytracedAntialiasing(bool theState)
{
  myV3dView->ChangeRenderingParams().IsAntialiasingEnabled = theState;
  myIsAntialiasingEnabled = theState;
  myContext->UpdateCurrentViewer();
}

void View::EnableRaytracing()
{
  if (!myIsRaytracing)
  {
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
  }
  myIsRaytracing = true;
  myContext->UpdateCurrentViewer();
}

void View::DisableRaytracing()
{
  if (myIsRaytracing)
  {
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;
  }
  myIsRaytracing = false;
  myContext->UpdateCurrentViewer();
}

void View::updateToggled(bool isOn)
{
  QAction* sentBy = (QAction*)sender();
  if (!isOn)
  {
    return;
  }

  foreach (QAction* anAction, myViewActions)
  {
    if (anAction && (anAction != sentBy))
    {
      anAction->setCheckable(true);
      anAction->setChecked(false);
    }
    else
    {
      if (sentBy == myViewActions.value(ViewAction_FitArea))
        setCursor(*handCursor);
      else if (sentBy == myViewActions.value(ViewAction_Zoom))
        setCursor(*zoomCursor);
      else if (sentBy == myViewActions.value(ViewAction_Pan))
        setCursor(*panCursor);
      else if (sentBy == myViewActions.value(ViewAction_GlobalPan))
        setCursor(*globPanCursor);
      else if (sentBy == myViewActions.value(ViewAction_Rotation))
        setCursor(*rotCursor);
      else
        setCursor(*defCursor);

      sentBy->setCheckable(false);
    }
  }
}

void View::initCursors()
{
  if (!defCursor)
    defCursor = new QCursor(Qt::ArrowCursor);
  if (!handCursor)
    handCursor = new QCursor(Qt::PointingHandCursor);
  if (!panCursor)
    panCursor = new QCursor(Qt::SizeAllCursor);
  if (!globPanCursor)
    globPanCursor = new QCursor(Qt::CrossCursor);
  if (!zoomCursor)
    zoomCursor = new QCursor(QPixmap(":/icons/cursor_zoom.png"));
  if (!rotCursor)
    rotCursor = new QCursor(QPixmap(":/icons/cursor_rotate.png"));
}

QList<QAction*> View::getViewActions()
{
  initViewActions();
  return myViewActions.values();
}

QList<QAction*>  View::getRaytraceActions()
{
  initRaytraceActions();
  return myRaytraceActions.values();
}

QAction* View::getViewAction(ViewAction theAction)
{
  return myViewActions.value(theAction);
}

QAction* View::getRaytraceAction(RaytraceAction theAction)
{
  return myRaytraceActions.value(theAction);
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* View::paintEngine() const
{
  return 0;
}

QAction* View::RegisterAction(QString theIconPath, QString thePromt)
{
  QAction* anAction = new QAction(QPixmap(theIconPath), thePromt, this);
  anAction->setToolTip(thePromt);
  anAction->setStatusTip(thePromt);
  return anAction;
}

void View::initViewActions()
{
  if (!myViewActions.empty())
    return;
  myViewActions[ViewAction_FitAll] = RegisterAction(":/icons/view_fitall.png", tr("Fit all"));
  connect(myViewActions[ViewAction_FitAll], SIGNAL(triggered()), this, SLOT(fitAll()));
  if (myIs3dView)
  {
    myViewActions[ViewAction_Axo] = RegisterAction(":/icons/view_axo.png", tr("Isometric"));
    connect(myViewActions[ViewAction_Axo], SIGNAL(triggered()), this, SLOT(axo()));

    QActionGroup* aShadingActionGroup = new QActionGroup(this);
    QAction* aShadingAction = RegisterAction(":/icons/tool_shading.png", tr("Shading"));
    connect(aShadingAction, SIGNAL(triggered()), this, SLOT(shading()));
    aShadingAction->setCheckable(true);
    aShadingActionGroup->addAction(aShadingAction);
    myViewActions[ViewAction_Shading] = aShadingAction;

    QAction* aWireframeAction = RegisterAction(":/icons/tool_wireframe.png", tr("Wireframe"));
    connect(aWireframeAction, SIGNAL(triggered()), this, SLOT(wireframe()));
    aWireframeAction->setCheckable(true);
    aShadingActionGroup->addAction(aWireframeAction);
    myViewActions[ViewAction_Wireframe] = aWireframeAction;

    QActionGroup* aHlrActionGroup = new QActionGroup(this);
    QAction* aHlrOffAction = RegisterAction(":/icons/view_comp_off.png", tr("HLR off"));
    connect(aHlrOffAction, SIGNAL(triggered()), this, SLOT(hlrOff()));
    aHlrOffAction->setCheckable(true);
    aHlrActionGroup->addAction(aHlrOffAction);
    myViewActions[ViewAction_HlrOff] = aHlrOffAction;

    QAction* aHlrOnAction = RegisterAction(":/icons/view_comp_on.png", tr("HLR on"));
    connect(aHlrOnAction, SIGNAL(triggered()), this, SLOT(hlrOn()));
    aHlrOnAction->setCheckable(true);
    aHlrActionGroup->addAction(aHlrOnAction);
    myViewActions[ViewAction_HlrOn] = aHlrOnAction;

    QAction* aTransparencyAction = RegisterAction(":/icons/tool_transparency.png", tr("Transparency"));
    connect(aTransparencyAction, SIGNAL(triggered()), this, SLOT(onTransparency()));
    myViewActions[ViewAction_Transparency] = aTransparencyAction;
  }
}

void View::initRaytraceActions()
{
  if (!myRaytraceActions.empty())
  {
    return;
  }

  QAction* aRayTraceAction = RegisterAction(":/icons/raytracing.png", tr("Ray-tracing"));
  connect(aRayTraceAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
  myRaytraceActions[RaytraceAction_Raytracing] = aRayTraceAction;
  aRayTraceAction->setCheckable(true);
  aRayTraceAction->setChecked(false);

  QAction* aShadowAction = RegisterAction(":/icons/shadows.png", tr("Shadows"));
  connect(aShadowAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
  myRaytraceActions[RaytraceAction_Shadows] = aShadowAction;
  aShadowAction->setCheckable(true);
  aShadowAction->setChecked(true);

  QAction* aReflectAction = RegisterAction(":/icons/reflections.png", tr("Reflections"));
  connect(aReflectAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
  myRaytraceActions[RaytraceAction_Reflections] = aReflectAction;
  aReflectAction->setCheckable(true);
  aReflectAction->setChecked(false);

  QAction* anAntiAliasingAction = RegisterAction(":/icons/antialiasing.png", tr("Anti-aliasing"));
  connect(anAntiAliasingAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
  myRaytraceActions[RaytraceAction_Antialiasing] = anAntiAliasingAction;
  anAntiAliasingAction->setCheckable(true);
  anAntiAliasingAction->setChecked(false);
}

void View::activateCursor(const CurrentAction3d theMode)
{
  QCursor* aCursor = defCursor;
  switch (theMode)
  {
    case CurrentAction3d_DynamicPanning:  aCursor = panCursor; break;
    case CurrentAction3d_DynamicZooming:  aCursor = zoomCursor; break;
    case CurrentAction3d_DynamicRotation: aCursor = rotCursor; break;
    case CurrentAction3d_GlobalPanning:   aCursor = globPanCursor; break;
    case CurrentAction3d_WindowZooming:   aCursor = handCursor; break;
    case CurrentAction3d_Nothing:         aCursor = defCursor; break;
    default:
      break;
  }
  setCursor(*aCursor);
}

void View::mousePressEvent(QMouseEvent* theEvent)
{
  Qt::MouseButtons aMouseButtons = theEvent->buttons();
  const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
  const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(theEvent->modifiers());
  if (!myV3dView.IsNull()
    && UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(aMouseButtons), aFlags, false))
  {
    updateView();
  }
  myClickPos = aPnt;
}

void View::mouseReleaseEvent(QMouseEvent* theEvent)
{
  Qt::MouseButtons aMouseButtons = theEvent->buttons();
  const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
  const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(theEvent->modifiers());
  if (!myV3dView.IsNull()
    && UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(aMouseButtons), aFlags, false))
  {
    updateView();
  }

  if (myCurrentMode == CurrentAction3d_GlobalPanning)
  {
    myV3dView->Place(aPnt.x(), aPnt.y(), myCurZoom);
  }
  if (myCurrentMode != CurrentAction3d_Nothing)
  {
    setCurrentAction(CurrentAction3d_Nothing);
  }
}

void View::mouseMoveEvent(QMouseEvent* theEvent)
{
  Qt::MouseButtons aMouseButtons = theEvent->buttons();
  const Graphic3d_Vec2i aNewPos(theEvent->pos().x(), theEvent->pos().y());
  if (!myV3dView.IsNull()
    && UpdateMousePosition(aNewPos, qtMouseButtons2VKeys(aMouseButtons), qtMouseModifiers2VKeys(theEvent->modifiers()), false))
  {
    updateView();
  }
}

//==============================================================================
//function : wheelEvent
//purpose  :
//==============================================================================
void View::wheelEvent(QWheelEvent* theEvent)
{
  const Graphic3d_Vec2i aPos(theEvent->pos().x(), theEvent->pos().y());
  if (!myV3dView.IsNull()
    && UpdateZoom(Aspect_ScrollDelta(aPos, theEvent->delta() / 8)))
  {
    updateView();
  }
}

// =======================================================================
// function : updateView
// purpose  :
// =======================================================================
void View::updateView()
{
  update();
}

void View::defineMouseGestures()
{
  myMouseGestureMap.Clear();
  AIS_MouseGesture aRot = AIS_MouseGesture_RotateOrbit;
  activateCursor(myCurrentMode);
  switch (myCurrentMode)
  {
    case CurrentAction3d_Nothing:
    {
      myMouseGestureMap = myDefaultGestures;
      break;
    }
    case CurrentAction3d_DynamicZooming:
    {
      myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
      break;
    }
    case CurrentAction3d_GlobalPanning:
    {
      break;
    }
    case CurrentAction3d_WindowZooming:
    {
      myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
      break;
    }
    case CurrentAction3d_DynamicPanning:
    {
      myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
      break;
    }
    case CurrentAction3d_DynamicRotation:
    {
      myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, aRot);
      break;
    }
    default:
    {
      break;
    }
  }
}

void View::addItemInPopup(QMenu* theMenu)
{
  Q_UNUSED(theMenu)
}

void View::onBackground()
{
  QColor aColor;
  Standard_Real R1;
  Standard_Real G1;
  Standard_Real B1;
  myV3dView->BackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
  aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

  QColor aRetColor = QColorDialog::getColor(aColor);
  if (aRetColor.isValid())
  {
    R1 = aRetColor.red() / 255.;
    G1 = aRetColor.green() / 255.;
    B1 = aRetColor.blue() / 255.;
    myV3dView->SetBackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
  }
  myV3dView->Redraw();
}

void View::onEnvironmentMap()
{
  if (myBackMenu->actions().at(1)->isChecked())
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
      tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));

    const TCollection_AsciiString anUtf8Path(fileName.toUtf8().data());
    Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv(anUtf8Path);
    myV3dView->SetTextureEnv(aTexture);
  }
  else
  {
    myV3dView->SetTextureEnv(Handle(Graphic3d_TextureEnv)());
  }

  myV3dView->Redraw();
}

void View::onTransparency()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  if (anAisObjectsList.Extent() == 0)
  {
    return;
  }

  double aTranspValue = anAisObjectsList.First()->Transparency();
  DialogTransparency aDlg(this);
  aDlg.setValue(int(aTranspValue * 10));
  connect(&aDlg, SIGNAL(sendTransparencyChanged(int)), SLOT(onTransparencyChanged(int)));
  aDlg.exec();
}

void View::onTransparencyChanged(int theVal)
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  double aTranspValue = theVal / 10.;  
  for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList);
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetTransparency(anAisObject, aTranspValue, Standard_False);
  }
  myContext->UpdateCurrentViewer();
}
