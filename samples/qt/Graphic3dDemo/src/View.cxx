#if !defined WNT
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include <qapplication.h>
#include <qpainter.h>
#include <qmenu.h>
#include <qcolordialog.h>
#include <qcursor.h>
#include <qevent.h>
#include <qrubberband.h>

#include "View.h"
#include "global.h"
#include "Application.h"
#include "ViewOperations.h"


#include <V3d_PerspectiveView.hxx>
#include <Visual3d_View.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <gp_Pnt.hxx>
#include <gp.hxx>
#include <Geom_Axis2Placement.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Line.hxx>
#include <Geom_Line.hxx>
#include <AIS_Trihedron.hxx>
//#include <TopAbs_ShapeEnum.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>
#undef QT_CLEAN_NAMESPACE
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif


// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY Qt::ControlModifier

// for elastic bean selection
#define ValZWMin 1

View::View( Handle(AIS_InteractiveContext) theContext, QWidget* parent, MDIWindow* mdi )
: QWidget( parent ), myMDI( mdi ), myRubberBand( 0 )
{
  setBackgroundRole( QPalette::NoRole );//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_PaintOnScreen );
  setAttribute( Qt::WA_NoSystemBackground );

#ifndef WNT
  XSynchronize( x11Display(), true ); // it is possible to use QApplication::syncX();
#endif

	myObjDlg = NULL;
  myShapeDlg = NULL;
  myFirst = true;
  myContext = theContext;

  myCycleCounter = -1;
    
  myXmin=0;
  myYmin=0;
  myXmax=0;
  myYmax=0;
  myCurZoom=0;

#if !defined WNT
  XVisualInfo* pVisualInfo;
  if ( x11Display() ) {
    /* Initialization with the default VisualID
    */
    Visual *v = DefaultVisual( x11Display(), DefaultScreen( x11Display() ) );
    int visualID = XVisualIDFromVisual( v );

    /* Here we use the settings from
       Optimizer_ViewInfo::TxglCreateWindow()
    */
    int visualAttr[] = { GLX_RGBA, GLX_DEPTH_SIZE, 1,
                         GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                         GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER,
                         None };
    pVisualInfo = ::glXChooseVisual (x11Display(), DefaultScreen(x11Display()), visualAttr);

    if ( isVisible() )
      hide();

    XSetWindowAttributes a;

    Window p = RootWindow( x11Display(), DefaultScreen(x11Display()) );
    a.colormap = XCreateColormap( x11Display(), RootWindow(x11Display(),
                                      pVisualInfo->screen), pVisualInfo->visual, AllocNone );
    a.background_pixel = backgroundColor().pixel();
    a.border_pixel = black.pixel();
    if ( parentWidget() )
      p = parentWidget()->winId();

    Window w = XCreateWindow( x11Display(), p,  x(), y(), width(), height(),
                              0, pVisualInfo->depth, InputOutput,  pVisualInfo->visual,
		  	                      CWBackPixel | CWBorderPixel | CWColormap, &a );
    Window *cmw;
    Window *cmwret;
    int count;
    if ( XGetWMColormapWindows( x11Display(), topLevelWidget()->winId(), 
                                &cmwret, &count ) ) {
      cmw = new Window[count+1];
      memcpy( (char *)cmw, (char *)cmwret, sizeof(Window)*count );
      XFree( (char *)cmwret );
      int i;
      for (i = 0; i < count; i++) {
        if ( cmw[i] == winId() ) { /* replace old window */
          cmw[i] = w;
          break;
        }
      }
      if ( i >= count )			 /* append new window */
        cmw[count++] = w;
    } else {
      count = 1;
      cmw = new Window[count];
      cmw[0] = w;
    }
    /* Creating new window (with good VisualID) for this widget */
    create( w );
    XSetWMColormapWindows( x11Display(), topLevelWidget()->winId(), cmw, count );
    delete [] cmw;

    if ( isVisible() )
      show();
    if ( pVisualInfo ) {
      XFree( (char *)pVisualInfo );
    }
    XFlush( x11Display() );
  }
#endif	// !defined WNT
  
  // will be set in OnInitial update, but, for more security :
  myCurrentMode = CurAction3d_Nothing;
  setMouseTracking( true );
  myView = NULL;
}

View::~View()
{
}

void View::init()
{
  myView = myContext->CurrentViewer()->CreateView();
  int windowHandle = (int) winId();
  short hi, lo;
  lo = (short) windowHandle;
  hi = (short) ( windowHandle >> 16 );
#ifdef WNT
  Handle(WNT_Window) hWnd = 
    new WNT_Window( Handle(Graphic3d_WNTGraphicDevice)::
    DownCast( myContext->CurrentViewer()->Device() ) , (int) hi, (int) lo );
#else
  Handle(Xw_Window) hWnd = 
    new Xw_Window( Handle(Graphic3d_GraphicDevice)::
    DownCast( myContext->CurrentViewer()->Device() ),
    (int) hi, (int) lo, Xw_WQ_SAMEQUALITY, Quantity_NOC_BLACK );
#endif // WNT
  myView->SetWindow( hWnd );
  if ( !hWnd->IsMapped() )
    hWnd->Map();
  myView->SetBackgroundColor(Quantity_NOC_GRAY40);
  myView->MustBeResized();

  Handle(Geom_Axis2Placement) anAxis = new Geom_Axis2Placement( gp::XOY() );
  myTrihedron = new AIS_Trihedron( anAxis );
  myContext->Display( myTrihedron );

  //myView->SetAntialiasingOn();
  myComputedModeIsOn = true;
  myView->SetScale(3.0316);

  // This is to update View properties dialog
  emit ViewInitialized();
}

void View::paintEvent ( QPaintEvent * e)
{
    if(myFirst) {
        init();
        myFirst = false;
    }
    QApplication::setOverrideCursor( Qt::WaitCursor );
    Application::startTimer();
    myView->Redraw();
    Application::stopTimer( 0, "View REDRAW", false );
    QApplication::restoreOverrideCursor();
}

void View::resizeEvent ( QResizeEvent * e) 
{
//  QApplication::syncX();
  if(!myView.IsNull()) {
      myView->MustBeResized();
    }
}

void View::mousePressEvent(QMouseEvent* e)
{
	if ( e->button() == Qt::LeftButton ) {
		onLButtonDown(e->modifiers(),e->pos());
	} else if(e->button() == Qt::RightButton ) {
		onRButtonDown(e->modifiers(),e->pos());
	}
}

void View::mouseReleaseEvent(QMouseEvent* e)
{
	if ( e->button() == Qt::LeftButton ) {
		onLButtonUp(e->modifiers(),e->pos());
	} else if(e->button() == Qt::RightButton ) {
		onRButtonUp(e->modifiers(),e->pos());
	}
}

void View::mouseMoveEvent(QMouseEvent* e)
{
	onMouseMove(e->buttons(),e->modifiers(),e->pos());
}

void View::onLButtonDown( Qt::KeyboardModifiers nFlags, const QPoint point )
{
    //  save the current mouse coordinate in min 
    myXmin=point.x();  myYmin=point.y();
    myXmax=point.x();  myYmax=point.y();

    if ( nFlags == CASCADESHORTCUTKEY ) {
        // Button MB1 down Control :start zomming 
        // SetCursor(AfxGetApp()->LoadStandardCursor());
    }
    else { // if ( Ctrl )
        switch (myCurrentMode) {
            
            case CurAction3d_Nothing : // start a drag
                if (nFlags == MULTISELECTIONKEY)
                    MultiDragEvent(myXmax,myYmax,-1);
                else
                    DragEvent(myXmax,myYmax,-1);
                break;
            case CurAction3d_DynamicZooming : // noting
                break;
            case CurAction3d_WindowZooming : 
            case CurAction3d_MagnifyView : 
                break;
            case CurAction3d_DynamicPanning :// noting
                break;
            case CurAction3d_GlobalPanning :// noting
                break;
            case  CurAction3d_DynamicRotation :
                if (!myComputedModeIsOn){
					myView->SetComputedMode(false);
				}
				myView->SetAnimationModeOn();
                myView->StartRotation(point.x(),point.y());  
                break;
            default :
                Standard_Failure::Raise(" incompatible Current Mode ");
                break;
        }
    }
}

void View::onRButtonDown( Qt::KeyboardModifiers nFlags, const QPoint point )
{
    if ( nFlags == CASCADESHORTCUTKEY ) {
        if ( !myComputedModeIsOn )
			myView->SetComputedMode(false);
//			Standard_Boolean anim, deg;
//			anim = myView->AnimationMode( deg );
//			if ( anim /*&& deg*/ ) {
            myCycleCounter = 0;
            Application::startTimer();
            myView->SetAnimationModeOn();
//			myView->SetAnimationMode(true,true);
//          myContext->DeactivateStandardMode( TopAbs_SHAPE );
//			}
        myView->StartRotation( point.x(),point.y() );
	}
	else
        Popup();
}

void View::onLButtonUp(Qt::KeyboardModifiers nFlags, const QPoint point)
{
   if ( nFlags == CASCADESHORTCUTKEY ) 
	  {
        return;
	  }
	else // if ( Ctrl )
	  {
        switch ( myCurrentMode )
        {
         case CurAction3d_Nothing :
         if (point.x() == myXmin && point.y() == myYmin)
         { // no offset between down and up --> selectEvent
            myXmax=point.x();  
            myYmax=point.y();
            if (nFlags == MULTISELECTIONKEY )
              MultiInputEvent(point.x(),point.y());
            else
              InputEvent     (point.x(),point.y());
         } else
         {
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
            myXmax=point.x();  
            myYmax=point.y();
		    if (nFlags == MULTISELECTIONKEY)
				MultiDragEvent(point.x(),point.y(),1);
			else
				DragEvent(point.x(),point.y(),1);
         }
         break;
         case CurAction3d_DynamicZooming :
             // SetCursor(AfxGetApp()->LoadStandardCursor());         
	       myCurrentMode = CurAction3d_Nothing;
		   emit noActiveActions();
         break;

         case CurAction3d_WindowZooming :
         case CurAction3d_MagnifyView :
             
             DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
             myXmax = point.x();  
             myYmax = point.y();
             
             if ( ( abs( myXmin - myXmax ) > ValZWMin ) || 
                  ( abs( myYmin - myYmax ) > ValZWMin ) ) {

                 if ( myCurrentMode == CurAction3d_MagnifyView )
                     myMDI->getDocument()->createMagView( myXmin, myYmin, myXmax, myYmax );
                 else
                     myView->WindowFitAll( myXmin, myYmin, myXmax, myYmax );  
             }  
             myCurrentMode = CurAction3d_Nothing;
             emit noActiveActions();
             break;

         case CurAction3d_DynamicPanning :
           myCurrentMode = CurAction3d_Nothing;
		   emit noActiveActions();
         break;
         case CurAction3d_GlobalPanning :
	       myView->Place(point.x(),point.y(),myCurZoom); 
	       myCurrentMode = CurAction3d_Nothing;
		   emit noActiveActions();
        break;
        case  CurAction3d_DynamicRotation :
	       myCurrentMode = CurAction3d_Nothing;
		   myView->SetAnimationModeOff();
		   emit noActiveActions();
        break;
        default :
           Standard_Failure::Raise(" incompatible Current Mode ");
        break;
        } //switch (myCurrentMode)
    } //	else // if ( Ctrl )
}

void View::onRButtonUp(Qt::KeyboardModifiers nFlags, const QPoint point)
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    // reset computed mode according to the stored one
    //   --> dynamic rotation may have change it 

    if (!myComputedModeIsOn)
    { 
      myView->SetComputedMode(false);
      myComputedModeIsOn = Standard_False;
    } else
    {
      myView->SetComputedMode(false);
      myComputedModeIsOn = Standard_True;
    }

    //if ( myView->AnimationModeIsOn() ) {
        myView->SetAnimationModeOff();
        Application::stopTimer( myCycleCounter, "Animation next UPDATE", false );
    //}
    myCycleCounter = -1;

    myView->Update();
   QApplication::restoreOverrideCursor();                
}

void View::onMouseMove(Qt::MouseButtons btns, Qt::KeyboardModifiers nFlags, const QPoint point)
{
  //   ============================  LEFT BUTTON =======================
  if ( btns == Qt::LeftButton ) {
     if ( nFlags == CASCADESHORTCUTKEY ) 
	  {
	    // move with MB1 and Control : on the dynamic zooming  
	    // Do the zoom in function of mouse's coordinates  
	    myView->Zoom(myXmax,myYmax,point.x(),point.y()); 
	    // save the current mouse coordinate in min 
		myXmax = point.x(); 
        myYmax = point.y();	
	  }
	  else // if ( Ctrl )
	  {
        switch (myCurrentMode)
        {
         case CurAction3d_Nothing :
		   myXmax = point.x();       
		   myYmax = point.y();
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
           if (nFlags == MULTISELECTIONKEY)		
       	     MultiDragEvent(myXmax,myYmax,0);
           else
             DragEvent(myXmax,myYmax,0);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True);
          break;
         case CurAction3d_DynamicZooming :
	       myView->Zoom(myXmax,myYmax,point.x(),point.y()); 
	       // save the current mouse coordinate in min \n";
	       myXmax=point.x();  myYmax=point.y();
         break;
         case CurAction3d_WindowZooming :
         case CurAction3d_MagnifyView :
		   myXmax = point.x(); myYmax = point.y();	
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);//,LongDash);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True);//,LongDash);
         break;
         case CurAction3d_DynamicPanning :
		   myView->Pan(point.x()-myXmax,myYmax-point.y()); // Realize the panning
		   myXmax = point.x();
		   myYmax = point.y();	
         break;
         case CurAction3d_GlobalPanning : // nothing           
        break;
        case  CurAction3d_DynamicRotation :
          myView->Rotation(point.x(),point.y());
//	      myView->Redraw();
        break;
        default :
           Standard_Failure::Raise(" incompatible Current Mode ");
        break;
        }//  switch (myCurrentMode)
      }// if ( nFlags == CASCADESHORTCUTKEY )  else 
    } else //   if ( btns == MK_LBUTTON) 
    //   ============================  MIDDLE BUTTON =======================
	if ( btns == Qt::MidButton ) {
     if ( nFlags == CASCADESHORTCUTKEY ) 
	  {
		myView->Pan(point.x()-myXmax,myYmax-point.y()); // Realize the panning
		myXmax = point.x();
		myYmax = point.y();	

	  }
    } else //  if ( btns == MK_MBUTTON)
    //   ============================  RIGHT BUTTON =======================
	if ( btns == Qt::RightButton ) {
     if ( nFlags == CASCADESHORTCUTKEY ) 
	  {
      	 myView->Rotation(point.x(),point.y());
	  }
    }else //if ( btns == MK_RBUTTON)
    //   ============================  NO BUTTON =======================
    {  // No buttons 
	  myXmax = point.x(); myYmax = point.y();	
	  if (nFlags == MULTISELECTIONKEY)
		MultiMoveEvent(point.x(),point.y());
	  else
		MoveEvent(point.x(),point.y());
   }
    //Standard_Boolean anim, deg;
    //anim = myView->AnimationMode( deg );
    //if ( anim /*&& deg*/ ) {
        if ( myCycleCounter != -1 ) {
            if ( myCycleCounter == 0 )
                Application::showTimer( "Animation first UPDATE" );
            myCycleCounter++;
        }
    //}
}

void View::DragEvent( int x, int y, int TheState )
{
    // TheState == -1  button down
	// TheState ==  0  move
	// TheState ==  1  button up

    static int theButtonDownX=0;
    static int theButtonDownY=0;

	if (TheState == -1)
    {
      theButtonDownX=x;
      theButtonDownY=y;
    }

	if (TheState == 0) {
	  myContext->Select(theButtonDownX,theButtonDownY,x,y,myView);  
	}
}

void View::InputEvent( int x, int y )
{
    Application::startTimer();
	myContext->Select();
    Application::stopTimer( 0, "Single selection", false );
}

void View::MoveEvent( int x, int y )
{
	myContext->MoveTo(x,y,myView);
}

void View::MultiMoveEvent( int x, int y )
{
	myContext->MoveTo(x,y,myView);
}

void View::MultiDragEvent( int x, int y, int TheState )
{
    static int theButtonDownX=0;
    static int theButtonDownY=0;

	if (TheState == -1)
    {  theButtonDownX=x;      theButtonDownY=y;}

	if (TheState == 0) {
	  myContext->ShiftSelect(theButtonDownX,theButtonDownY,x,y,myView);  
	  emit selectionChanged();
	}
}

void View::MultiInputEvent( int x, int y )
{
	myContext->ShiftSelect();
    emit selectionChanged();
}

void View::Popup()
{
    QMenu* popMenu = new QMenu( this );

    if( myContext->NbCurrents()) {
		QAction* a;
		myContext->InitCurrent();
		if (! myContext->Current()->IsKind(STANDARD_TYPE(AIS_Trihedron)))
		{
			a = new QAction( tr( "MEN_POP_OBJPROP" ), this );
			verify( connect( a, SIGNAL( activated() ), SLOT( onEditObjectProperties() ) ) );
      popMenu->addAction( a );
			
			QMenu* dMode = popMenu->addMenu( tr( "MEN_POP_DISPLAYMODE" ) );

			a = new QAction( tr( "MEN_POP_WIREFRAME" ), this );
			verify( connect( a, SIGNAL( activated() ), SLOT( onWireframeMode() ) ) );
      dMode->addAction( a );

			a = new QAction( tr( "MEN_POP_SHADING" ), this );
			verify( connect( a, SIGNAL( activated() ), SLOT( onShadingMode() ) ) );
      dMode->addAction( a );
		}
		a = new QAction( tr( "MEN_POP_REMOVEOBJECT" ), this );
		verify( connect( a, SIGNAL( activated() ), SLOT( onRemoveObject() ) ) );
    popMenu->addAction( a );
    }
    else {
        QAction* a = new QAction( tr( "MEN_POP_VIEWPROP" ), this );
        verify( connect( a, SIGNAL( activated() ), Application::getApplication(), SLOT( onEditViewProperties() ) ) );
        popMenu->addAction( a );

        a = new QAction( tr( "MEN_POP_IMPORT" ), this );
        verify( connect( a, SIGNAL( activated() ), SLOT( onImportObject() ) ) );
        popMenu->addAction( a );

        popMenu->addSeparator();
        if ( myContext->IsDisplayed( myTrihedron ) ) {
            a = new QAction( tr( "MEN_POP_ERASETRIHEDRON" ), this );
            verify( connect( a, SIGNAL( activated() ), SLOT( onEraseTrihedron() ) ) );
            popMenu->addAction( a );
        }
        else {
            a = new QAction( tr( "MEN_POP_DISPLAYTRIHEDRON" ), this );
            verify( connect( a, SIGNAL( activated() ), SLOT( onDisplayTrihedron() ) ) );
            popMenu->addAction( a );
        }

        popMenu->addSeparator();
        a = new QAction( tr( "MEN_POP_CREATESHAPE" ), this );
        verify( connect( a, SIGNAL( activated() ), SLOT( onCreateShape() ) ) );
        popMenu->addAction( a );

        QMenu* loadObj = popMenu->addMenu( tr( "MEN_POP_LOADOBJECT" ) );

        a = new QAction( tr( "MEN_POP_BOX" ), this );
        verify( connect( a, SIGNAL( activated() ), SLOT( onLoadBox() ) ) );
        loadObj->addAction( a );

        a = new QAction( tr( "MEN_POP_SPHERE" ), this );
        verify( connect( a, SIGNAL( activated() ), SLOT( onLoadSphere() ) ) );
        loadObj->addAction( a );

        a = new QAction( tr( "MEN_POP_TORUS" ), this );
        verify( connect( a, SIGNAL( activated() ), SLOT( onLoadTorus() ) ) );
        loadObj->addAction( a );

//        a = new QAction( tr( "MEN_POP_LINE" ), this );
//        verify( connect( a, SIGNAL( activated() ), SLOT( onLoadLine() ) ) );
//        loadObj->addAction( a );
	}
    popMenu->exec( QCursor::pos() );
}

void View::DrawRectangle( int  MinX, int MinY, int MaxX, int MaxY, bool Draw )
{
  if ( !myRubberBand )
    myRubberBand = new QRubberBand( QRubberBand::Rectangle, this );
  
  myRubberBand->setGeometry( QRect( QPoint( MinX, MinY ), QPoint( MaxX, MaxY ) ).normalized() );
  myRubberBand->setVisible( Draw );
/*
	QPainter thePainter(this);
	thePainter.setRasterOp(Qt::XorROP);
	thePainter.setPen(Qt::white);
    static		int StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
    static		bool m_IsVisible;

	QRect aRect;
	if ( m_IsVisible && !Draw) // move or up  : erase at the old position 
    {
	 aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));
	 thePainter.drawRect(aRect);
	 m_IsVisible = false;
    }

    StoredMinX = (MinX < MaxX) ? MinX: MaxX ;
    StoredMinY = (MinY < MaxY) ? MinY: MaxY ;
    StoredMaxX = (MinX > MaxX) ? MinX: MaxX ;
    StoredMaxY = (MinY > MaxY) ? MinY: MaxY ;

    if (Draw) // move : draw
    {
	 aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));
	 thePainter.drawRect(aRect);
     m_IsVisible = true;
   }
	*/
}

void View::onImportObject()
{
    Application::getApplication()->importBREP();
}

void View::setCurrentAction()
{
    myCurrentMode = CurAction3d_MagnifyView;
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

	if(aRetColor.isValid()) {
		R1 = aRetColor.red()/255.;
		G1 = aRetColor.green()/255.;
		B1 = aRetColor.blue()/255.;
        myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	}
    myView->Redraw();
}

Handle(V3d_View)& View::getView()
{
    return myView;
}

void View::startAction( ViewOperations::Action action )
{
    switch( action ) {
        case ViewOperations::ViewFitAllId :
            myView->FitAll();
            break;
        case ViewOperations::ViewFitAreaId :
            myCurrentMode = CurAction3d_WindowZooming;
            break;
        case ViewOperations::ViewZoomId :
            myCurrentMode = CurAction3d_DynamicZooming;
            break;
        case ViewOperations::ViewPanId :
            myCurrentMode = CurAction3d_DynamicPanning;
            break;
        case ViewOperations::ViewGlobalPanId :
            myCurZoom = myView->Scale();
            // Do a Global Zoom 
            myView->FitAll();
            // Set the mode 
            myCurrentMode = CurAction3d_GlobalPanning;
            break;
        case ViewOperations::ViewFrontId :
            myView->SetProj( V3d_Xpos );
            myView->FitAll();
            break;
        case ViewOperations::ViewBackId :
            myView->SetProj( V3d_Xneg );
            myView->FitAll();
            break;
        case ViewOperations::ViewTopId :
            myView->SetProj( V3d_Zpos );
            myView->FitAll();
            break;
        case ViewOperations::ViewBottomId :
            myView->SetProj( V3d_Zneg );
            myView->FitAll();
            break;
        case ViewOperations::ViewLeftId :
            myView->SetProj( V3d_Ypos );
            myView->FitAll();
            break;
        case ViewOperations::ViewRightId :
            myView->SetProj( V3d_Yneg );
            myView->FitAll();
            break;
        case ViewOperations::ViewAxoId :
            myView->SetProj( V3d_XposYnegZpos );
            myView->FitAll();
            break;
        case ViewOperations::ViewRotationId :
            myCurrentMode = CurAction3d_DynamicRotation;
            break;
        case ViewOperations::ViewResetId :
            myView->Reset();
            break;
    }
}

void View::onCreateShape()
{
	myView->SetScale(3.0316);
    if ( !myShapeDlg )
        myShapeDlg = new ShapeDlg( Application::getApplication(), myView, myContext );
    myShapeDlg->show();
}

void View::onLoadBox()
{
	myView->SetScale(1.0);
    Standard_Real size = myContext->CurrentViewer()->DefaultViewSize() / 4.;
    gp_Pnt origin( -size / 2., -size / 2., -size / 2. );
    //myContext->DefaultDrawer()->ShadingAspect()->SetColor(Quantity_NOC_RED);
    Handle(AIS_Shape) box = 
        new AIS_Shape( BRepPrimAPI_MakeBox( origin, size, size, size ).Shape() );
    myContext->SetDisplayMode( box, myMDI->getDisplayMode(), false );

    Application::startTimer();
    myContext->Display( box, false );
    Application::stopTimer( 0, "Display Box" );
    
    Application::startTimer();
    myView->Update();
    Application::stopTimer( 0, "UPDATE" );
}

void View::onLoadSphere()
{
	myView->SetScale(1.0);
    Standard_Real radius = myContext->CurrentViewer()->DefaultViewSize() / 4.;
    Handle(AIS_Shape) sphere = 
	    new AIS_Shape( BRepPrimAPI_MakeSphere( radius ).Shape() );
    myContext->SetDisplayMode( sphere, myMDI->getDisplayMode(), false );

    Application::startTimer();
    myContext->Display( sphere, false );
    Application::stopTimer( 0, "Display Sphere" );
    
    Application::startTimer();
    //myContext->UpdateCurrentViewer();
    myView->Update();
    Application::stopTimer( 0, "UPDATE" );
}

void View::onLoadTorus()
{
	myView->SetScale(1.0);
    Handle(V3d_Viewer) viewer = myContext->CurrentViewer();
    Standard_Real r1 = viewer->DefaultViewSize() / 3.;
    Standard_Real r2 = viewer->DefaultViewSize() / 6.;
    Handle(AIS_Shape) torus = new AIS_Shape( BRepPrimAPI_MakeTorus( r1,r2 ).Shape() );
    myContext->SetDisplayMode( torus, myMDI->getDisplayMode(), false );
    
    Application::startTimer();
    myContext->Display( torus, false );
    Application::stopTimer( 0, "Display Torus" );

    Application::startTimer();
    //myContext->UpdateCurrentViewer();
    myView->Update();
    Application::stopTimer( 0, "UPDATE" );
}

void View::onLoadLine()
{
	myView->SetScale(1.0);
    Handle(AIS_Line) line = 
        new AIS_Line( new Geom_Line( gp_Pnt( 0, 0, 0), gp_Dir( 0, 0, 1) ) );
    myContext->Display( line );
    myContext->UpdateCurrentViewer();
}

void View::onEditObjectProperties()
{
    if ( !myObjDlg )
        myObjDlg = new ObjectDlg( Application::getApplication(), myView, myContext );
    myObjDlg->show();
}

void View::onRemoveObject()
{
    myContext->InitSelected();
    for( ; myContext->MoreSelected(); myContext->InitSelected() )
        myContext->Remove( myContext->SelectedInteractive(), false );
    myContext->UpdateCurrentViewer();
}

void View::onWireframeMode()
{
    myContext->InitSelected();
    for( ; myContext->MoreSelected(); myContext->NextSelected() ) {
        Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
        //myContext->Erase( obj, false );
        //myContext->Display( obj, /*0, 0,*/ false );
        //myContext->Activate( obj, 0 );
        myContext->SetDisplayMode( obj, 0 );
    }
    myContext->UpdateCurrentViewer();
}

void View::onShadingMode()
{
	
    myContext->InitSelected();
    for( ; myContext->MoreSelected(); myContext->NextSelected() ) {
        Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
        //myContext->Erase( obj, false );
        //myContext->Display( obj /*, 1, 0*/, false );
        //myContext->Activate( obj, 0 );
        myContext->SetDisplayMode( obj, 1 );
    }
    myContext->UpdateCurrentViewer();
	
}

void View::onDisplayTrihedron()
{
    myContext->Display( myTrihedron );
}

void View::onEraseTrihedron()
{
    myContext->Erase( myTrihedron );
}

