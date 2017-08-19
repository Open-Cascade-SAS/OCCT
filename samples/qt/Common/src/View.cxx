#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "View.h"
#include "ApplicationCommon.h"

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


#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>

#include <OcctWindow.h>
#include <Aspect_DisplayConnection.hxx>

// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY Qt::ControlModifier

// for elastic bean selection
#define ValZWMin 1

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

  myXmin = 0;
  myYmin = 0;
  myXmax = 0;
  myYmax = 0;
  myCurZoom = 0;
  myRectBand = 0;

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  myCurrentMode = CurAction3d_Nothing;
  myHlrModeIsOn = Standard_False;
  setMouseTracking( true );

  initViewActions();
  initCursors();

  setBackgroundRole( QPalette::NoRole );//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_PaintOnScreen );
  setAttribute( Qt::WA_NoSystemBackground );
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
  myView->Redraw();
}

void View::resizeEvent( QResizeEvent * )
{
//  QApplication::syncX();
  if( !myView.IsNull() )
  {
    myView->MustBeResized();
  }
}

void View::fitAll()
{
  myView->FitAll();
  myView->ZFitAll();
  myView->Redraw();
}

void View::fitArea()
{
  myCurrentMode = CurAction3d_WindowZooming;
}

void View::zoom()
{
  myCurrentMode = CurAction3d_DynamicZooming;
}

void View::pan()
{
  myCurrentMode = CurAction3d_DynamicPanning;
}

void View::rotation()
{
  myCurrentMode = CurAction3d_DynamicRotation;
}

void View::globalPan()
{
  // save the current zoom value
  myCurZoom = myView->Scale();
  // Do a Global Zoom
  myView->FitAll();
  // Set the mode
  myCurrentMode = CurAction3d_GlobalPanning;
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
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
  QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myHlrModeIsOn = Standard_True;
  myView->SetComputedMode (myHlrModeIsOn);
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

void View::mousePressEvent( QMouseEvent* e )
{
  if ( e->button() == Qt::LeftButton )
    onLButtonDown( ( e->buttons() | e->modifiers() ), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonDown( e->buttons() | e->modifiers(), e->pos() );
  else if ( e->button() == Qt::RightButton )
    onRButtonDown( e->buttons() | e->modifiers(), e->pos() );
}

void View::mouseReleaseEvent(QMouseEvent* e)
{
  if ( e->button() == Qt::LeftButton )
    onLButtonUp( e->buttons(), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonUp( e->buttons(), e->pos() );
  else if( e->button() == Qt::RightButton )
    onRButtonUp( e->buttons(), e->pos() );
}

void View::mouseMoveEvent(QMouseEvent* e)
{
  onMouseMove( e->buttons(), e->pos() );
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

void View::onLButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  //  save the current mouse coordinate in min
  myXmin = point.x();
  myYmin = point.y();
  myXmax = point.x();
  myYmax = point.y();

  if ( nFlags & CASCADESHORTCUTKEY )
  {
    myCurrentMode = CurAction3d_DynamicZooming;
  }
  else
  {
    switch ( myCurrentMode )
    {
      case CurAction3d_Nothing:
           if ( nFlags & MULTISELECTIONKEY )
             MultiDragEvent( myXmax, myYmax, -1 );
           else
             DragEvent( myXmax, myYmax, -1 );
           break;
      case CurAction3d_DynamicZooming:
           break;
      case CurAction3d_WindowZooming:
           break;
      case CurAction3d_DynamicPanning:
           break;
      case CurAction3d_GlobalPanning:
           break;
      case CurAction3d_DynamicRotation:
           if (myHlrModeIsOn)
           {
             myView->SetComputedMode (Standard_False);
           }
           myView->StartRotation( point.x(), point.y() );
           break;
      default:
           throw Standard_Failure( "incompatible Current Mode" );
           break;
    }
  }
  activateCursor( myCurrentMode );
}

void View::onMButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint /*point*/ )
{
  if ( nFlags & CASCADESHORTCUTKEY )
    myCurrentMode = CurAction3d_DynamicPanning;
  activateCursor( myCurrentMode );
}

void View::onRButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  if ( nFlags & CASCADESHORTCUTKEY )
  {
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (Standard_False);
    }
    myCurrentMode = CurAction3d_DynamicRotation;
    myView->StartRotation( point.x(), point.y() );
  }
  else
  {
    Popup( point.x(), point.y() );
  }
  activateCursor( myCurrentMode );
}

void View::onLButtonUp( Qt::MouseButtons nFlags, const QPoint point )
{
    switch( myCurrentMode )
    {
        case CurAction3d_Nothing:
            if ( point.x() == myXmin && point.y() == myYmin )
            {
              // no offset between down and up --> selectEvent
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiInputEvent( point.x(), point.y() );
              else
                  InputEvent( point.x(), point.y() );
            }
            else
            {
              DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiDragEvent( point.x(), point.y(), 1 );
              else
                  DragEvent( point.x(), point.y(), 1 );
            }
            break;
        case CurAction3d_DynamicZooming:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_WindowZooming:
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );//,LongDash);
            myXmax = point.x();
            myYmax = point.y();
            if ( (abs( myXmin - myXmax ) > ValZWMin ) ||
                 (abs( myYmin - myYmax ) > ValZWMin ) )
              myView->WindowFitAll( myXmin, myYmin, myXmax, myYmax );
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_DynamicPanning:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_GlobalPanning :
            myView->Place( point.x(), point.y(), myCurZoom );
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_DynamicRotation:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        default:
            throw Standard_Failure(" incompatible Current Mode ");
            break;
    }
    activateCursor( myCurrentMode );
    ApplicationCommonWindow::getApplication()->onSelectionChanged();
}

void View::onMButtonUp( Qt::MouseButtons /*nFlags*/, const QPoint /*point*/ )
{
    myCurrentMode = CurAction3d_Nothing;
    activateCursor( myCurrentMode );
}

void View::onRButtonUp( Qt::MouseButtons /*nFlags*/, const QPoint point )
{
    if ( myCurrentMode == CurAction3d_Nothing )
        Popup( point.x(), point.y() );
    else
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        // reset tyhe good Degenerated mode according to the strored one
        //   --> dynamic rotation may have change it
        if (myHlrModeIsOn)
        {
          myView->SetComputedMode (myHlrModeIsOn);
          myView->Redraw();
        }
        QApplication::restoreOverrideCursor();
        myCurrentMode = CurAction3d_Nothing;
    }
    activateCursor( myCurrentMode );
}

void View::onMouseMove( Qt::MouseButtons nFlags, const QPoint point )
{
    if ( nFlags & Qt::LeftButton || nFlags & Qt::RightButton || nFlags & Qt::MidButton )
    {
    switch ( myCurrentMode )
    {
        case CurAction3d_Nothing:
          myXmax = point.x();
          myYmax = point.y();
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          if ( nFlags & MULTISELECTIONKEY )
              MultiDragEvent( myXmax, myYmax, 0 );
          else
            DragEvent( myXmax, myYmax, 0 );
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );
            break;
        case CurAction3d_DynamicZooming:
          myView->Zoom( myXmax, myYmax, point.x(), point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurAction3d_WindowZooming:
          myXmax = point.x();
          myYmax = point.y();
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );
          break;
        case CurAction3d_DynamicPanning:
          myView->Pan( point.x() - myXmax, myYmax - point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurAction3d_GlobalPanning:
          break;
        case CurAction3d_DynamicRotation:
          myView->Rotation( point.x(), point.y() );
          myView->Redraw();
          break;
        default:
          throw Standard_Failure( "incompatible Current Mode" );
          break;
    }
    }
    else
    {
        myXmax = point.x();
        myYmax = point.y();
      if ( nFlags & MULTISELECTIONKEY )
          MultiMoveEvent( point.x(), point.y() );
      else
          MoveEvent( point.x(), point.y() );
    }
}

void View::DragEvent( const int x, const int y, const int TheState )
{
    // TheState == -1  button down
    // TheState ==  0  move
    // TheState ==  1  button up

    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }

    if ( TheState == 1 )
    {
        myContext->Select( theButtonDownX, theButtonDownY, x, y, myView, Standard_True );
        emit selectionChanged();
    }
}

void View::InputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->Select (Standard_True);
  emit selectionChanged();
}

void View::MoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myView, Standard_True );
}

void View::MultiMoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myView, Standard_True );
}

void View::MultiDragEvent( const int x, const int y, const int TheState )
{
    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }
    if ( TheState == 0 )
    {
        myContext->ShiftSelect( theButtonDownX, theButtonDownY, x, y, myView, Standard_True );
        emit selectionChanged();
    }
}

void View::MultiInputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->ShiftSelect (Standard_True);
  emit selectionChanged();
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

void View::DrawRectangle(const int MinX, const int MinY,
       const int MaxX, const int MaxY, const bool Draw)
{ 
  static Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
  static Standard_Boolean m_IsVisible;

  StoredMinX = (MinX < MaxX) ? MinX: MaxX ;
  StoredMinY = (MinY < MaxY) ? MinY: MaxY ;
  StoredMaxX = (MinX > MaxX) ? MinX: MaxX ;
  StoredMaxY = (MinY > MaxY) ? MinY: MaxY ;

  QRect aRect;
  aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));

  if ( !myRectBand ) 
  {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, this );
    myRectBand->setStyle( QStyleFactory::create("windows") );
    myRectBand->setGeometry( aRect );
    myRectBand->show();

    /*QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);*/
  }

  if ( m_IsVisible && !Draw ) // move or up  : erase at the old position
  {
    myRectBand->hide();
    delete myRectBand;
    myRectBand = 0;
    m_IsVisible = false;
  }

  if (Draw) // move : draw
  {
    //aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));
    m_IsVisible = true;
    myRectBand->setGeometry( aRect );
    //myRectBand->show();
  }
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
    aColor.setRgb(R1*255,G1*255,B1*255);

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



