#include "DocumentCommon.h"

#include "ApplicationCommon.h"
#include "Transparency.h"
#include "Material.h"

#include <QStatusBar>
#include <QApplication>
#include <QColor>
#include <QColorDialog>

#include <Aspect_DisplayConnection.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TCollection_AsciiString.hxx>

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer (const Standard_ExtString theName,
                                           const Standard_CString theDomain,
                                           const Standard_Real theViewSize,
                                           const V3d_TypeOfOrientation theViewProj,
                                           const Standard_Boolean theComputedMode,
                                           const Standard_Boolean theDefaultComputedMode )
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;

  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection (qgetenv ("DISPLAY").constData());
#endif
    aGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
  }

  return new V3d_Viewer (aGraphicDriver,
                         theName,
                         theDomain,
                         theViewSize,
                         theViewProj,
                         Quantity_NOC_GRAY30,
                         V3d_ZBUFFER,
                         V3d_GOURAUD,
                         V3d_WAIT,
                         theComputedMode,
                         theDefaultComputedMode,
                         V3d_TEX_NONE);
}

DocumentCommon::DocumentCommon( const int theIndex, ApplicationCommonWindow* app )
: QObject( app ),
myApp( app ),
myIndex( theIndex ),
myNbViews( 0 )
{
  TCollection_ExtendedString a3DName ("Visu3D");

  myViewer = Viewer (a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  myContext = new AIS_InteractiveContext (myViewer);
}

DocumentCommon::~DocumentCommon()
{
}

ApplicationCommonWindow* DocumentCommon::getApplication()
{
  return myApp;
}

MDIWindow* DocumentCommon::createNewMDIWindow()
{
  QMdiArea* ws = myApp->getWorkspace();
  return new MDIWindow (this, ws, 0);
}

void DocumentCommon::onCreateNewView()
{
  QMdiArea* ws = myApp->getWorkspace();
  MDIWindow* w = createNewMDIWindow();
  
  if (!w)
    return;

  ws->addSubWindow (w);
  myViews.append (w);

  connect( w,    SIGNAL( selectionChanged() ),
           this, SIGNAL( selectionChanged() ) );
  connect( w, SIGNAL( message( const QString&, int ) ),
           myApp->statusBar(), SLOT( showMessage( const QString&, int ) ) );
  connect( w, SIGNAL( sendCloseView( MDIWindow* ) ),
           this, SLOT( onCloseView( MDIWindow* ) ) );

  QString aName;
  w->setWindowTitle( aName.sprintf( "Document %d:%d", myIndex, ++myNbViews ) );
  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  
  w->setWindowIcon( QPixmap( dir + QObject::tr("ICON_DOC") ) );

  if ( ws->subWindowList().isEmpty() )
  {
    // Due to strange Qt4.2.3 feature the child window icon is not drawn
    // in the main menu if showMaximized() is called for a non-visible child window
    // Therefore calling show() first...
    w->show();
    w->showMaximized();
  }
  else
    w->show();

  w->setFocus();

  getApplication()->onSelectionChanged();
}

void DocumentCommon::onCloseView(MDIWindow* theView)
{
    removeView(theView);
    if( countOfWindow() == 0 )
        emit sendCloseDocument( this );
}

void DocumentCommon::removeView(MDIWindow* theView)
{
    if ( myViews.count( theView ) )
    {
    myViews.removeAll(theView);
    delete theView;
  }
}
void DocumentCommon::removeViews()
{
  while( myViews.count() )
  {
    removeView( myViews.first() );
  }
}

int DocumentCommon::countOfWindow()
{
  return myViews.count();
}

Handle(AIS_InteractiveContext) DocumentCommon::getContext()
{
  return myContext;
}

void DocumentCommon::fitAll()
{
  QList<MDIWindow*>::iterator i;
  for ( i = myViews.begin(); i != myViews.end(); i++ )
    (*i)->fitAll();
}

void DocumentCommon::onWireframe()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextCurrent() )
        myContext->SetDisplayMode( myContext->Current(), 0, false );
    myContext->UpdateCurrentViewer();
    getApplication()->onSelectionChanged();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onShading()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextCurrent() )
        myContext->SetDisplayMode( myContext->Current(), 1, false );
    myContext->UpdateCurrentViewer();
    getApplication()->onSelectionChanged();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onColor()
{
    QColor aColor ;
    Quantity_Color aShapeColor;
    myContext->InitCurrent();
    Handle_AIS_InteractiveObject Current = myContext->Current() ;
    if ( Current->HasColor () )
    {
        aShapeColor = myContext->Color( Current );
        aColor.setRgb( aShapeColor.Red() * 255, aShapeColor.Green() * 255, aShapeColor.Blue() * 255 );
    }
    else
        aColor.setRgb( 255, 255, 255 );

    QColor aRetColor = QColorDialog::getColor( aColor );
    if ( aRetColor.isValid() )
    {
        Quantity_Color color( aRetColor.red() / 255., aRetColor.green() / 255.,
                        aRetColor.blue() / 255., Quantity_TOC_RGB );
        for (; myContext->MoreCurrent(); myContext->NextCurrent() )
            myContext->SetColor( myContext->Current(), color.Name() );
    }
}

void DocumentCommon::onMaterial( int theMaterial )
{
    for ( myContext->InitCurrent(); myContext->MoreCurrent (); myContext->NextCurrent () )
        myContext->SetMaterial( myContext->Current(), (Graphic3d_NameOfMaterial)theMaterial );
}

void DocumentCommon::onMaterial()
{
    DialogMaterial* m = new DialogMaterial();
    connect( m, SIGNAL( sendMaterialChanged( int ) ), this, SLOT( onMaterial( int ) ) );
    m->exec();
}

void DocumentCommon::onTransparency( int theTrans )
{
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextSelected() )
        myContext->SetTransparency( myContext->Current(), ((Standard_Real)theTrans) / 10.0 );
}

void DocumentCommon::onTransparency()
{
    DialogTransparency* aDialog = new DialogTransparency();
    connect( aDialog, SIGNAL( sendTransparencyChanged( int ) ), this, SLOT( onTransparency( int ) ) );
    aDialog->exec();
}

void DocumentCommon::onDelete()
{
    myContext->EraseSelected (Standard_False);
    myContext->ClearSelected();
    getApplication()->onSelectionChanged();
}
