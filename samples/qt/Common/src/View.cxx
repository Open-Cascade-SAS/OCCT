#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "View.h"
#include "ApplicationCommon.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include <QColorDialog>
#include <QCursor>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QRubberBand>
#include <QMdiSubWindow>
#include <QStyleFactory>
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  #include <QX11Info>
#endif
#include <Standard_WarningsRestore.hxx>


#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>

#include <OcctWindow.h>
#include <Aspect_DisplayConnection.hxx>

namespace
{
  //! Map Qt buttons bitmask to virtual keys.
  Aspect_VKeyMouse qtMouseButtons2VKeys (Qt::MouseButtons theButtons)
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
  Aspect_VKeyFlags qtMouseModifiers2VKeys (Qt::KeyboardModifiers theModifiers)
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
}

static QCursor* defCursor     = NULL;
static QCursor* handCursor    = NULL;
static QCursor* panCursor     = NULL;
static QCursor* globPanCursor = NULL;
static QCursor* zoomCursor    = NULL;
static QCursor* rotCursor     = NULL;

View::View( Handle(AIS_InteractiveContext) theContext, QWidget* parent )
: QWidget( parent ),
  myIsRaytracing( false ),
  myIsShadowsEnabled (true),
  myIsReflectionsEnabled (false),
  myIsAntialiasingEnabled (false),
  myViewActions( 0 ),
  myRaytraceActions( 0 ),
  myBackMenu( NULL )
{
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  XSynchronize(x11Info().display(),true);
#endif
  myContext = theContext;

  myCurZoom = 0;
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_NativeWindow);

  myDefaultGestures = myMouseGestureMap;
  myCurrentMode = CurAction3d_Nothing;
  setMouseTracking( true );

  initViewActions();
  initCursors();

  setBackgroundRole( QPalette::NoRole );//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy( Qt::StrongFocus );
  init();
}

View::~View()
{
  delete myBackMenu;
}

void View::init()
{
  if ( myView.IsNull() )
    myView = myContext->CurrentViewer()->CreateView();

  Handle(OcctWindow) hWnd = new OcctWindow ( this );
  myView->SetWindow (hWnd);
  if ( !hWnd->IsMapped() )
  {
    hWnd->Map();
  }
  myView->SetBackgroundColor (Quantity_NOC_BLACK);
  myView->MustBeResized();

  if (myIsRaytracing)
    myView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
}

void View::paintEvent( QPaintEvent *  )
{
//  QApplication::syncX();
  myView->InvalidateImmediate();
  FlushViewEvents (myContext, myView, true);
}

void View::resizeEvent( QResizeEvent * )
{
//  QApplication::syncX();
  if( !myView.IsNull() )
  {
    myView->MustBeResized();
  }
}

void View::OnSelectionChanged (const Handle(AIS_InteractiveContext)& ,
                               const Handle(V3d_View)& )
{
  ApplicationCommonWindow::getApplication()->onSelectionChanged();
}

void View::fitAll()
{
  myView->FitAll();
  myView->ZFitAll();
  myView->Redraw();
}

void View::fitArea()
{
  setCurrentAction (CurAction3d_WindowZooming);
}

void View::zoom()
{
  setCurrentAction (CurAction3d_DynamicZooming);
}

void View::pan()
{
  setCurrentAction (CurAction3d_DynamicPanning);
}

void View::rotation()
{
  setCurrentAction (CurAction3d_DynamicRotation);
}

void View::globalPan()
{
  // save the current zoom value
  myCurZoom = myView->Scale();
  // Do a Global Zoom
  myView->FitAll();
  // Set the mode
  setCurrentAction (CurAction3d_GlobalPanning);
}

void View::front()
{
  myView->SetProj( V3d_Yneg );
}

void View::back()
{
  myView->SetProj( V3d_Ypos );
}

void View::top()
{
  myView->SetProj( V3d_Zpos );
}

void View::bottom()
{
  myView->SetProj( V3d_Zneg );
}

void View::left()
{
  myView->SetProj( V3d_Xneg );
}

void View::right()
{
  myView->SetProj( V3d_Xpos );
}

void View::axo()
{
  myView->SetProj( V3d_XposYnegZpos );
}

void View::reset()
{
  myView->Reset();
}

void View::hlrOff()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myView->SetComputedMode (false);
  myView->Redraw();
  QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myView->SetComputedMode (true);
  myView->Redraw();
  QApplication::restoreOverrideCursor();
}

void View::SetRaytracedShadows (bool theState)
{
  myView->ChangeRenderingParams().IsShadowEnabled = theState;

  myIsShadowsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::SetRaytracedReflections (bool theState)
{
  myView->ChangeRenderingParams().IsReflectionEnabled = theState;

  myIsReflectionsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::onRaytraceAction()
{
  QAction* aSentBy = (QAction*)sender();
  
  if (aSentBy == myRaytraceActions->at (ToolRaytracingId))
  {
    bool aState = myRaytraceActions->at (ToolRaytracingId)->isChecked(); 

    QApplication::setOverrideCursor (Qt::WaitCursor);
    if (aState)
      EnableRaytracing();
    else
      DisableRaytracing();
    QApplication::restoreOverrideCursor();
  }

  if (aSentBy == myRaytraceActions->at (ToolShadowsId))
  {
    bool aState = myRaytraceActions->at (ToolShadowsId)->isChecked(); 
    SetRaytracedShadows (aState);
  }

  if (aSentBy == myRaytraceActions->at (ToolReflectionsId))
  {
    bool aState = myRaytraceActions->at (ToolReflectionsId)->isChecked();
    SetRaytracedReflections (aState);
  }

  if (aSentBy == myRaytraceActions->at (ToolAntialiasingId))
  {
    bool aState = myRaytraceActions->at (ToolAntialiasingId)->isChecked();
    SetRaytracedAntialiasing (aState);
  }
}

void View::SetRaytracedAntialiasing (bool theState)
{
  myView->ChangeRenderingParams().IsAntialiasingEnabled = theState;

  myIsAntialiasingEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::EnableRaytracing()
{
  if (!myIsRaytracing)
    myView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;

  myIsRaytracing = true;

  myContext->UpdateCurrentViewer();
}

void View::DisableRaytracing()
{
  if (myIsRaytracing)
    myView->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;

  myIsRaytracing = false;

  myContext->UpdateCurrentViewer();
}

void View::updateToggled( bool isOn )
{
  QAction* sentBy = (QAction*)sender();

  if( !isOn )
    return;

  for ( int i = ViewFitAllId; i < ViewHlrOffId; i++ )
  {
    QAction* anAction = myViewActions->at( i );
    
    if ( ( anAction == myViewActions->at( ViewFitAreaId ) ) ||
         ( anAction == myViewActions->at( ViewZoomId ) ) ||
         ( anAction == myViewActions->at( ViewPanId ) ) ||
         ( anAction == myViewActions->at( ViewGlobalPanId ) ) ||
         ( anAction == myViewActions->at( ViewRotationId ) ) )
    {
      if ( anAction && ( anAction != sentBy ) )
      {
        anAction->setCheckable( true );
        anAction->setChecked( false );
      }
      else
      {
        if ( sentBy == myViewActions->at( ViewFitAreaId ) )
          setCursor( *handCursor );
        else if ( sentBy == myViewActions->at( ViewZoomId ) )
          setCursor( *zoomCursor );
        else if ( sentBy == myViewActions->at( ViewPanId ) )
          setCursor( *panCursor );
        else if ( sentBy == myViewActions->at( ViewGlobalPanId ) )
          setCursor( *globPanCursor );
        else if ( sentBy == myViewActions->at( ViewRotationId ) )
          setCursor( *rotCursor );
        else
          setCursor( *defCursor );

        sentBy->setCheckable( false );
      }
    }
  }
}

void View::initCursors()
{
  if ( !defCursor )
    defCursor = new QCursor( Qt::ArrowCursor );
  if ( !handCursor )
    handCursor = new QCursor( Qt::PointingHandCursor );
  if ( !panCursor )
    panCursor = new QCursor( Qt::SizeAllCursor );
  if ( !globPanCursor )
    globPanCursor = new QCursor( Qt::CrossCursor );
  if ( !zoomCursor )
    zoomCursor = new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + QString( "/" ) + QObject::tr( "ICON_CURSOR_ZOOM" ) ) );
  if ( !rotCursor )
    rotCursor = new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + QString( "/" ) + QObject::tr( "ICON_CURSOR_ROTATE" ) ) );
}

QList<QAction*>* View::getViewActions()
{
  initViewActions();
  return myViewActions;
}

QList<QAction*>* View::getRaytraceActions()
{
  initRaytraceActions();
  return myRaytraceActions;
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* View::paintEngine() const
{
  return 0;
}

void View::initViewActions()
{
  if ( myViewActions )
    return;

  myViewActions = new QList<QAction*>();
  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FITALL") ), QObject::tr("MNU_FITALL"), this );
  a->setToolTip( QObject::tr("TBR_FITALL") );
  a->setStatusTip( QObject::tr("TBR_FITALL") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitAll() ) );
  myViewActions->insert(ViewFitAllId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FITAREA") ), QObject::tr("MNU_FITAREA"), this );
  a->setToolTip( QObject::tr("TBR_FITAREA") );
  a->setStatusTip( QObject::tr("TBR_FITAREA") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitArea() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled( bool ) ) , this, SLOT( updateToggled( bool ) ) );
  myViewActions->insert( ViewFitAreaId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_ZOOM") ), QObject::tr("MNU_ZOOM"), this );
  a->setToolTip( QObject::tr("TBR_ZOOM") );
  a->setStatusTip( QObject::tr("TBR_ZOOM") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( zoom() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewZoomId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_PAN") ), QObject::tr("MNU_PAN"), this );
  a->setToolTip( QObject::tr("TBR_PAN") );
  a->setStatusTip( QObject::tr("TBR_PAN") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( pan() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewPanId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_GLOBALPAN") ), QObject::tr("MNU_GLOBALPAN"), this );
  a->setToolTip( QObject::tr("TBR_GLOBALPAN") );
  a->setStatusTip( QObject::tr("TBR_GLOBALPAN") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( globalPan() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewGlobalPanId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FRONT") ), QObject::tr("MNU_FRONT"), this );
  a->setToolTip( QObject::tr("TBR_FRONT") );
  a->setStatusTip( QObject::tr("TBR_FRONT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( front() ) );
  myViewActions->insert( ViewFrontId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_BACK") ), QObject::tr("MNU_BACK"), this );
  a->setToolTip( QObject::tr("TBR_BACK") );
  a->setStatusTip( QObject::tr("TBR_BACK") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( back() ) );
  myViewActions->insert(ViewBackId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_TOP") ), QObject::tr("MNU_TOP"), this );
  a->setToolTip( QObject::tr("TBR_TOP") );
  a->setStatusTip( QObject::tr("TBR_TOP") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( top() ) );
  myViewActions->insert( ViewTopId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_BOTTOM") ), QObject::tr("MNU_BOTTOM"), this );
  a->setToolTip( QObject::tr("TBR_BOTTOM") );
  a->setStatusTip( QObject::tr("TBR_BOTTOM") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( bottom() ) );
  myViewActions->insert( ViewBottomId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_LEFT") ), QObject::tr("MNU_LEFT"), this );
  a->setToolTip( QObject::tr("TBR_LEFT") );
  a->setStatusTip( QObject::tr("TBR_LEFT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( left() ) );
  myViewActions->insert( ViewLeftId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_RIGHT") ), QObject::tr("MNU_RIGHT"), this );
  a->setToolTip( QObject::tr("TBR_RIGHT") );
  a->setStatusTip( QObject::tr("TBR_RIGHT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( right() ) );
  myViewActions->insert( ViewRightId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_AXO") ), QObject::tr("MNU_AXO"), this );
  a->setToolTip( QObject::tr("TBR_AXO") );
  a->setStatusTip( QObject::tr("TBR_AXO") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( axo() ) );
  myViewActions->insert( ViewAxoId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_ROTATION") ), QObject::tr("MNU_ROTATION"), this );
  a->setToolTip( QObject::tr("TBR_ROTATION") );
  a->setStatusTip( QObject::tr("TBR_ROTATION") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( rotation() ) );
  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewRotationId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_RESET") ), QObject::tr("MNU_RESET"), this );
  a->setToolTip( QObject::tr("TBR_RESET") );
  a->setStatusTip( QObject::tr("TBR_RESET") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( reset() ) );
  myViewActions->insert( ViewResetId, a );

  QActionGroup* ag = new QActionGroup( this );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_HLROFF") ), QObject::tr("MNU_HLROFF"), this );
  a->setToolTip( QObject::tr("TBR_HLROFF") );
  a->setStatusTip( QObject::tr("TBR_HLROFF") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( hlrOff() ) );
  a->setCheckable( true );
  ag->addAction(a);
  myViewActions->insert(ViewHlrOffId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_HLRON") ), QObject::tr("MNU_HLRON"), this );
  a->setToolTip( QObject::tr("TBR_HLRON") );
  a->setStatusTip( QObject::tr("TBR_HLRON") );
  connect( a, SIGNAL( triggered() ) ,this, SLOT( hlrOn() ) );
  
  a->setCheckable( true );
  ag->addAction(a);
  myViewActions->insert( ViewHlrOnId, a );
}

void View::initRaytraceActions()
{
  if ( myRaytraceActions )
    return;

  myRaytraceActions = new QList<QAction*>();
  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_RAYTRACING") ), QObject::tr("MNU_TOOL_RAYTRACING"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_RAYTRACING") );
  a->setStatusTip( QObject::tr("TBR_TOOL_RAYTRACING") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions->insert( ToolRaytracingId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_SHADOWS") ), QObject::tr("MNU_TOOL_SHADOWS"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_SHADOWS") );
  a->setStatusTip( QObject::tr("TBR_TOOL_SHADOWS") );
  a->setCheckable( true );
  a->setChecked( true );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions->insert( ToolShadowsId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_REFLECTIONS") ), QObject::tr("MNU_TOOL_REFLECTIONS"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
  a->setStatusTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions->insert( ToolReflectionsId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_ANTIALIASING") ), QObject::tr("MNU_TOOL_ANTIALIASING"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
  a->setStatusTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions->insert( ToolAntialiasingId, a );
}

void View::activateCursor( const CurrentAction3d mode )
{
  switch( mode )
  {
    case CurAction3d_DynamicPanning:
      setCursor( *panCursor );
      break;
    case CurAction3d_DynamicZooming:
      setCursor( *zoomCursor );
      break;
    case CurAction3d_DynamicRotation:
      setCursor( *rotCursor );
      break;
    case CurAction3d_GlobalPanning:
      setCursor( *globPanCursor );
      break;
    case CurAction3d_WindowZooming:
      setCursor( *handCursor );
      break;
    case CurAction3d_Nothing:
    default:
      setCursor( *defCursor );
      break;
  }
}

void View::mousePressEvent (QMouseEvent* theEvent)
{
  const Graphic3d_Vec2i aPnt (theEvent->pos().x(), theEvent->pos().y());
  const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys (theEvent->modifiers());
  if (!myView.IsNull()
    && UpdateMouseButtons (aPnt,
                           qtMouseButtons2VKeys (theEvent->buttons()),
                           aFlags,
                           false))
  {
    updateView();
  }
  myClickPos = aPnt;
}

void View::mouseReleaseEvent (QMouseEvent* theEvent)
{
  const Graphic3d_Vec2i aPnt (theEvent->pos().x(), theEvent->pos().y());
  const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys (theEvent->modifiers());
  if (!myView.IsNull()
    && UpdateMouseButtons (aPnt,
                           qtMouseButtons2VKeys (theEvent->buttons()),
                           aFlags,
                           false))
  {
    updateView();
  }

  if (myCurrentMode == CurAction3d_GlobalPanning)
  {
    myView->Place (aPnt.x(), aPnt.y(), myCurZoom);
  }
  if (myCurrentMode != CurAction3d_Nothing)
  {
    setCurrentAction (CurAction3d_Nothing);
  }
  if (theEvent->button() == Qt::RightButton
   && (aFlags & Aspect_VKeyFlags_CTRL) == 0
   && (myClickPos - aPnt).cwiseAbs().maxComp() <= 4)
  {
    Popup (aPnt.x(), aPnt.y());
  }
}

void View::mouseMoveEvent (QMouseEvent* theEvent)
{
  const Graphic3d_Vec2i aNewPos (theEvent->pos().x(), theEvent->pos().y());
  if (!myView.IsNull()
    && UpdateMousePosition (aNewPos,
                            qtMouseButtons2VKeys (theEvent->buttons()),
                            qtMouseModifiers2VKeys (theEvent->modifiers()),
                            false))
  {
    updateView();
  }
}

//==============================================================================
//function : wheelEvent
//purpose  :
//==============================================================================
void View::wheelEvent (QWheelEvent* theEvent)
{
  const Graphic3d_Vec2i aPos (theEvent->pos().x(), theEvent->pos().y());
  if (!myView.IsNull()
    && UpdateZoom (Aspect_ScrollDelta (aPos, theEvent->delta() / 8)))
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
  activateCursor (myCurrentMode);
  switch (myCurrentMode)
  {
    case CurAction3d_Nothing:
    {
      noActiveActions();
      myMouseGestureMap = myDefaultGestures;
      break;
    }
    case CurAction3d_DynamicZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
      break;
    }
    case CurAction3d_GlobalPanning:
    {
      break;
    }
    case CurAction3d_WindowZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
      break;
    }
    case CurAction3d_DynamicPanning:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
      break;
    }
    case CurAction3d_DynamicRotation:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, aRot);
      break;
    }
  }
}

void View::Popup( const int /*x*/, const int /*y*/ )
{
  ApplicationCommonWindow* stApp = ApplicationCommonWindow::getApplication();
  QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
  QMdiSubWindow* w = ws->activeSubWindow();
  if ( myContext->NbSelected() )
  {
    QList<QAction*>* aList = stApp->getToolActions();
    QMenu* myToolMenu = new QMenu( 0 );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolWireframeId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolShadingId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolColorId ) );
        
    QMenu* myMaterMenu = new QMenu( myToolMenu );

    QList<QAction*>* aMeterActions = ApplicationCommonWindow::getApplication()->getMaterialActions();
        
    QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
    myMaterMenu = myToolMenu->addMenu( QPixmap( dir+QObject::tr("ICON_TOOL_MATER")), QObject::tr("MNU_MATER") );
    for ( int i = 0; i < aMeterActions->size(); i++ )
      myMaterMenu->addAction( aMeterActions->at( i ) );
       
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolTransparencyId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolDeleteId ) );
    addItemInPopup(myToolMenu);
    myToolMenu->exec( QCursor::pos() );
    delete myToolMenu;
  }
  else
  {
    if (!myBackMenu)
    {
      myBackMenu = new QMenu( 0 );

      QAction* a = new QAction( QObject::tr("MNU_CH_BACK"), this );
      a->setToolTip( QObject::tr("TBR_CH_BACK") );
      connect( a, SIGNAL( triggered() ), this, SLOT( onBackground() ) );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);

      a = new QAction( QObject::tr("MNU_CH_ENV_MAP"), this );
      a->setToolTip( QObject::tr("TBR_CH_ENV_MAP") );
      connect( a, SIGNAL( triggered() ), this, SLOT( onEnvironmentMap() ) );
      a->setCheckable( true );
      a->setChecked( false );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);
    }

    myBackMenu->exec( QCursor::pos() );
  }
  if ( w )
    w->setFocus();
}

void View::addItemInPopup( QMenu* /*theMenu*/)
{
}

void View::noActiveActions()
{
    for ( int i = ViewFitAllId; i < ViewHlrOffId ; i++ )
    {
        QAction* anAction = myViewActions->at( i );
        if( ( anAction == myViewActions->at( ViewFitAreaId ) ) ||
            ( anAction == myViewActions->at( ViewZoomId ) ) ||
            ( anAction == myViewActions->at( ViewPanId ) ) ||
            ( anAction == myViewActions->at( ViewGlobalPanId ) ) ||
            ( anAction == myViewActions->at( ViewRotationId ) ) )
        {
            setCursor( *defCursor );
            anAction->setCheckable( true );
            anAction->setChecked( false );
        }
    }
}

void View::onBackground()
{
    QColor aColor ;
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    myView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

    QColor aRetColor = QColorDialog::getColor(aColor);

    if( aRetColor.isValid() )
    {
        R1 = aRetColor.red()/255.;
        G1 = aRetColor.green()/255.;
        B1 = aRetColor.blue()/255.;
        myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    }
    myView->Redraw();
}

void View::onEnvironmentMap()
{
  if (myBackMenu->actions().at(1)->isChecked())
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                           tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));
    
    const TCollection_AsciiString anUtf8Path (fileName.toUtf8().data());
    
    Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv( anUtf8Path );

    myView->SetTextureEnv (aTexture);
  }
  else
  {
    myView->SetTextureEnv (Handle(Graphic3d_TextureEnv)());
  }
  
  myView->Redraw();
}

bool View::dump(Standard_CString theFile)
{
  return myView->Dump(theFile);
}

Handle(V3d_View)& View::getView()
{
  return myView;
}

Handle(AIS_InteractiveContext)& View::getContext()
{
  return myContext;
}

View::CurrentAction3d View::getCurrentMode()
{
  return myCurrentMode;
}
