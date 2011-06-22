#include <qstatusbar.h>

#ifndef WNT
#include <Graphic3d_GraphicDevice.hxx>
#else
#include <Graphic3d_WNTGraphicDevice.hxx>
#endif

#include <V3d_View.hxx>

#include "Application.h"
#include "Document.h"
#include "global.h"



Handle(V3d_Viewer) Document::Viewer( const Standard_CString aDisplay,
                                     const Standard_ExtString aName,
                                     const Standard_CString aDomain,
                                     const Standard_Real ViewSize )
{
#ifndef WNT
  static Handle(Graphic3d_GraphicDevice) defaultdevice;

  if ( defaultdevice.IsNull() ) 
    defaultdevice = new Graphic3d_GraphicDevice( aDisplay );
    
  return new V3d_Viewer( defaultdevice, aName, aDomain, ViewSize, 
                         V3d_XposYnegZpos,Quantity_NOC_MIDNIGHTBLUE,
                    	   V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT);
#else
  static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;
  if ( defaultdevice.IsNull() )
     defaultdevice = new Graphic3d_WNTGraphicDevice();
    
  return new V3d_Viewer( defaultdevice, aName, aDomain, ViewSize,
                         V3d_XposYnegZpos,Quantity_NOC_MIDNIGHTBLUE,
                    	   V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT);
#endif  // WNT
}

Document::Document( int theIndex, Application* app ) 
: QObject( app )
{
  myIndex = theIndex;
  myNbViews = 0;
	myApp = app;

  TCollection_ExtendedString a3DName("Visual3D");
	myViewer = Viewer( getenv( "DISPLAY" ), a3DName.ToExtString(), "", 1000.0 );
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();
	myViewer->SetZBufferManagment(false);
	myContext = new AIS_InteractiveContext( myViewer );

	createNewView();

	myOperations = new ViewOperations( myContext );
}

Document::~Document()
{
  qDeleteAll( myViews );
}

Application* Document::getApplication()
{
	return myApp;
}

void Document::createNewView( V3d_TypeOfView type ) 
{
	QWorkspace* ws = myApp->getWorkspace();

  MDIWindow* w = new MDIWindow( this, ws, type );
  myViews.append( w );
  ws->addWindow( w );

	//connect( w, SIGNAL( message(const QString&, int) ), myApp->statusBar(), SLOT( message(const QString&, int )) );
	verify( connect( w, SIGNAL( sendCloseView( MDIWindow* ) ), 
                      SLOT( onCloseView( MDIWindow* ) ) ) );

  QString aName;
  w->setWindowTitle( aName.sprintf( "3D Viewer_%d:%d", myIndex, ++myNbViews ) );

  QString dir = Application::getResourceDir();
  w->setWindowIcon( QPixmap( dir + tr( "ICON_DOC" ) ) );

  // show the very first window in maximized mode
  if ( ws->windowList().isEmpty() )
		w->showMaximized();
  else
		w->show();
}

void Document::createMagView( int xMin, int yMin, int xMax, int yMax )
{
#ifdef old
  Handle(V3d_View) currentView = 
    ( (MDIWindow*) myApp->getWorkspace()->activeWindow() )->getView();
  createNewView( false );
  MDIWindow* newView = myViews.last();
  Handle(V3d_View) nView = newView->getView();
    
  newView->show();
	nView->FitAll();
	//nView->ZFitAll();
	//nView->Redraw();
  //nView->WindowFitAll( xMin, yMin, xMax, yMax );
  nView->SetMagnify( currentView->Window(), currentView, 10, 10, 100, 100 );
  //nView->SetMagnify( nView->Window(), currentView, xMin, yMin, xMax, yMax );
#endif
}

void Document::onCloseView(MDIWindow* theView)
{
  if ( countOfWindow() == 1 )
    emit sendCloseDocument( this );
  else
    removeView( theView );
}

void Document::removeView(MDIWindow* theView)
{
	myViews.removeAll( theView );
	//delete theView;
}

int Document::countOfWindow()
{
	return myViews.count();
}

Handle(AIS_InteractiveContext)& Document::getContext()
{
	return myContext;
}

void Document::onWireframe()
{
	myOperations->onWireframe();
}

void Document::onShading()
{
	myOperations->onShading();
}
