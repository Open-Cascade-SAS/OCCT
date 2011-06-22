#include <stdio.h>

#include <AIS_InteractiveContext.hxx>
#include <OSD_Timer.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_TFace.hxx>
#include <TopoDS.hxx>
#include <Sphere_Sphere.hxx>
#include <AIS_Trihedron.hxx>

#include <qapplication.h>
#include <qframe.h>
#include <qworkspace.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qtextedit.h> 
#include <qlayout.h>
#include <qevent.h>

#include "global.h"
#include "Application.h"
#include "Document.h"
#include "MDIWindow.h"
#include "Translate.h"
#include "ViewOperations.h" // to have access to its enumeration
#include "AutoTestDlg.h"

//#define OUTPUT_WINDOW

static Application* stApp;
static OSD_Timer timer;

Application::Application() : QMainWindow( 0 )
{
  myNbDocuments = 0;
  stApp = this;
  myIsDocuments = false;

  // create and define the central widget
  QFrame* vBox = new QFrame( this );
  vBox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QVBoxLayout* main = new QVBoxLayout( vBox );
  main->setMargin( 0 );

  myWorkSpace = new QWorkspace( vBox );
  main->addWidget( myWorkSpace );
  setCentralWidget( vBox );

	createActions();
	createCCActions();

  statusBar()->showMessage( tr("INF_READY"), 2001 );
  resize( 800, 650 );
  verify( connect( myWorkSpace, SIGNAL( windowActivated( QWidget* ) ),
                   this,        SLOT( onWindowActivated( QWidget* ) ) ) );
#ifdef OUTPUT_WINDOW
  myOutput = new OutputWindow( 0 );
  myOutput->show();
#else
  myOutput = 0;
#endif
	myViewDlg = NULL;
}

Application::~Application()
{
	if (myViewDlg) delete myViewDlg; 
  if (myOutput)  delete myOutput;
}

void Application::createActions()
{
	QString dir = getResourceDir();
  QPixmap newIcon( dir + tr( "ICON_NEW" ) ),
          helpIcon( dir + tr( "ICON_HELP" ) ),
          closeIcon( dir + tr( "ICON_CLOSE" ) ),
          autoIcon( dir + tr( "ICON_AUTO_TEST" ) ),
          optimIcon( dir + tr( "ICON_OPTIM" ) ),
			    stopIcon( dir + tr( "ICON_STOP" ) ),
          tileIcon( dir + tr( "ICON_TILE" ) ),
          cascadeIcon( dir + tr( "ICON_CASCADE" ) );

  // toolbar with standard actions: new, save...
  myStdToolBar = new QToolBar( this );
  addToolBar( Qt::TopToolBarArea, myStdToolBar );
    
  // menu with all actions
  myFilePopup = menuBar()->addMenu( tr( "MEN_FILE" ) );
    
	// view menu
	QMenu* view = menuBar()->addMenu( tr( "MEN_VIEW" ) );

  // Tools menu (it is shown or hidden dynamically)
  myToolsMenu = new QMenu( tr( "MEN_TOOLS" ), this );

  // Window menu
  myWinMenu = menuBar()->addMenu( tr( "MEN_WINDOWS" ) );
  verify(connect( myWinMenu, SIGNAL( aboutToShow() ),
                  this, SLOT( updateWindowList() ) ));

  // add a help menu
  QMenu* help = menuBar()->addMenu( tr( "MEN_HELP" ) );
  menuBar()->addSeparator();
    
  QAction* a = new QAction( newIcon, tr( "MEN_NEW" ), this );
  a->setToolTip( tr( "TBR_NEW" ) );
  a->setShortcut( Qt::CTRL+Qt::Key_N );
	verify( connect( a, SIGNAL( activated() ), this, SLOT( onNewDocument() ) ) );
	myStdActions.insert( FileNewId, a );	
  myStdToolBar->addAction( a );
  myFilePopup ->addAction( a );

  a = new QAction( tr( "MEN_QUIT" ), this );
  a->setShortcut( Qt::CTRL+Qt::Key_Q );
  a->setToolTip( tr( "TBR_QUIT" ) );
  verify( connect( a, SIGNAL( activated() ), qApp, SLOT( closeAllWindows() ) ) );
	myStdActions.insert( FileQuitId, a );	
  myFilePopup ->addAction( a );

	a = new QAction( tr( "MEN_TOOL_BAR" ), this );
  a->setToolTip( tr( "TBR_TOOL_BAR" ) );
	verify( connect( a, SIGNAL( activated() ), this, SLOT( onViewToolBar() ) ) );
	a->setCheckable(true);
	a->setChecked( true );
	myStdActions.insert( ViewToolId, a );	
  view->addAction( a );

	a = new QAction( tr("MEN_STATUS_BAR"), this );
  a->setToolTip( tr( "TBR_STATUS_BAR" ) );
	verify( connect( a, SIGNAL( activated() ), this, SLOT( onViewStatusBar() ) ) );
	a->setCheckable(true);
	a->setChecked( true );
	myStdActions.insert( ViewStatusId, a );	
  view->addAction( a );

  a = new QAction( helpIcon, tr( "MEN_ABOUT" ), this );
  a->setShortcut( Qt::Key_F1 );
  a->setToolTip( tr( "TBR_ABOUT" ) );
	verify( connect( a, SIGNAL( activated() ) , this, SLOT( onAbout() ) ) );
	myStdActions.insert( HelpAboutId, a );	
	myStdToolBar->addAction( a );
	help->addAction( a );

  myStdToolBar->addSeparator();

  // Tools menu actions
  a = new QAction( optimIcon, tr( "MEN_OPTIM" ), this );
  a->setToolTip( tr( "TBR_OPTIM" ) );
	verify( connect( a, SIGNAL( toggled(bool) ) , this, SLOT( onToggleOptim(bool) ) ) );
  a->setCheckable(true);
  a->setChecked(Graphic3d_ArrayOfPrimitives::IsEnable());
	myStdActions.insert( OptimId, a );	
  myToolsMenu->addAction( a );

  a = new QAction( autoIcon, tr( "TBR_AUTO_TEST" ), this );
  a->setToolTip( tr( "TBR_AUTO_TEST" ) );
	verify( connect( a, SIGNAL( activated() ) , this, SLOT( onAutoTest() ) ) );
	myStdActions.insert( AutoTestId, a );	
  myToolsMenu->addAction( a );
  
  a = new QAction( stopIcon, tr( "TBR_STOP" ), this );
  a->setToolTip( tr( "TBR_STOP" ) );
	verify( connect( a, SIGNAL( activated() ) , this, SLOT( onStop() ) ) );
	myStdActions.insert( StopId, a );	
  myToolsMenu->addAction( a );

  // Window menu action
  a = new QAction( cascadeIcon, tr( "MEN_CASCADE" ), this );
	verify( connect( a, SIGNAL( activated() ) , myWorkSpace, SLOT( cascade() ) ) );
	myStdActions.insert( CascadeId, a );	
  myWinMenu->addAction( a );

  a = new QAction( tileIcon, tr( "MEN_TILE" ), this );
	verify( connect( a, SIGNAL( activated() ), myWorkSpace, SLOT( tile() ) ) );
	myStdActions.insert( TileId, a );	
  myWinMenu->addAction( a );

}

void Application::createCCActions()
{
	createIEPopups();
}

void Application::createIEPopups()
{
	myImportPopup = new QMenu( tr( "MEN_FILE_IMPORT" ), this );

	QAction* a = new QAction( tr( "MEN_IMPORT_BREP" ), this );
  a->setToolTip( tr( "TBR_IMPORT_BREP" ) );
	verify( connect( a, SIGNAL( activated() ) , SLOT( onTranslate() ) ) );
	myCCActions.insert( FileImportBREPId, a );	
	myImportPopup->addAction( a );
}

QWorkspace* Application::getWorkspace()
{
  return getApplication()->workspace();
}

Application* Application::getApplication()
{
	return stApp;
}

void Application::updateWindowList()
{
  while ( myWinMenu->actions().count() > 2)
    myWinMenu->removeAction(myWinMenu->actions().last());

  QWidgetList lst = myWorkSpace->windowList();
  if (!lst.isEmpty())
  {
    myWinMenu->addSeparator();
    for ( int i = 0; i < int(lst.count()); ++i )
    {
      QAction* a = myWinMenu->addAction( lst.at(i)->windowTitle(),
        this, SLOT( activateWindow() ) );
      a->setCheckable( true );
      a->setChecked( myWorkSpace->activeWindow() == lst.at(i) );
    }
  }

}

void Application::activateWindow()
{
  QAction* a = qobject_cast<QAction*>( sender() );
  if ( !a ) return;
  int idx = myWinMenu->actions().indexOf( a ) - 3; // tile, cascade and separator
  QWidget* w = myWorkSpace->windowList().at( idx );
  if ( w ) {
    w->showNormal();
    w->setFocus();
//	w->setActiveWindow();
  }
}

void Application::updateActions()
{
	int count = myFilePopup->actions().count() - 1;
	
	int popupCount = menuBar()->actions().count();
	while (myWinMenu->actions().count() > 2)
		myWinMenu->removeAction(myWinMenu->actions().last());

	if( myWorkSpace->windowList().isEmpty() )
  {
    QAction* optim    = myStdActions.at(OptimId);
    QAction* autoTest = myStdActions.at(AutoTestId);
	  QAction* stop = myStdActions.at(StopId);

    if( myNbDocuments ) {
      myFilePopup->insertMenu( myFilePopup->actions().last(), myImportPopup );
			myFilePopup->insertSeparator( myFilePopup->actions().last() );

      menuBar()->insertMenu( menuBar()->actions().at( popupCount - 3 ), myToolsMenu );

      if (optim) myStdToolBar->addAction( optim );
      if (autoTest) myStdToolBar->addAction( autoTest );
	    if (stop) myStdToolBar->addAction( stop );
	    stop->setEnabled(false);
			myIsDocuments = true;
		} 
    else {
      myFilePopup->actions().removeAt( --count );
			myFilePopup->actions().removeAt( --count );
			myFilePopup->actions().removeAt( --count );

      menuBar()->actions().removeAt( popupCount - 4 );

      if (optim) myStdToolBar->removeAction( optim );
      if (autoTest) myStdToolBar->removeAction( autoTest );
	    if (stop) myStdToolBar->removeAction( stop );
			myIsDocuments = false;
    }
  }
}

Document* Application::onNewDocument()
{
  ++myNbDocuments;

  updateActions();

  Document* aDoc = new Document( myNbDocuments, this );
  verify( connect( aDoc, SIGNAL( sendCloseDocument( Document* ) ),
                   SLOT( onCloseDocument( Document* ) ) ) );

  return aDoc;
}

void Application::onCloseDocument(Document* theDoc)
{
  --myNbDocuments;
  delete theDoc;
	updateActions();
}

void Application::onTranslate()
{
  const QObject* sentBy = sender();
  typedef void  (Translate::*LPFN_IMPORT) 
    ( const Handle( AIS_InteractiveContext ));//, const QString&  );

  static LPFN_IMPORT lpfnImport[] = { &Translate::importBREP };

  static QAction* actions[] = { myCCActions.at( FileImportBREPId ) };
	
  for ( int i = 0; i < sizeof(actions)/sizeof(QAction*); i++ )
  {
    if ( actions[i] == sentBy ) 
    {
      Translate* anTrans = new Translate( 0 );
      MDIWindow* win = myActiveMDI;//(MDIWindow*) myWorkSpace->activeWindow(); 
      Handle(AIS_InteractiveContext) aContext = win->getDocument()->getContext();
      try 
      {
        /* import */
        (anTrans->*lpfnImport[i])( aContext );
      } 
      catch ( Standard_Failure )
      {
			  /* WARNING: an exception raised but we eat it
           silently because the shape seems OK
        */										
      }
      delete anTrans;
      /* commit transaction */
      win->activateAction( ViewOperations::ViewFitAllId );
      break;
    }
  }
}

void Application::importBREP()
{
  Translate* anTrans = new Translate( 0 );
  MDIWindow* win = myActiveMDI;//(MDIWindow*) myWorkSpace->activeWindow(); 
  Handle(AIS_InteractiveContext) aContext = win->getDocument()->getContext();
  try {
    /* import */
    anTrans->importBREP( aContext );
  } 
  catch ( Standard_Failure ) {
    /* WARNING: an exception raised but we eat it
       silently because the shape seems OK
    */                    
  }
  delete anTrans;
  win->activateAction( ViewOperations::ViewFitAllId );
}

QString Application::getResourceDir()
{
  static QString resDir( getenv( "CSF_ResourcesDefaults" ) + QString( "/" ) );
  return resDir;
}

void Application::onViewToolBar()
{
  bool show = myStdActions.at( ViewToolId )->isChecked();
  if (  show == myStdToolBar->isVisible() )
    return;
  if ( show )
    myStdToolBar->show();
  else
    myStdToolBar->hide();
}

void Application::onViewStatusBar()
{
  bool show = myStdActions.at( ViewStatusId )->isChecked();
  if (  show == statusBar()->isVisible() )
    return;
  if ( show )
    statusBar()->show();
  else
    statusBar()->hide();
}

void Application::onAbout()
{
  QMessageBox::information(this,tr("TIT_ABOUT"),tr("INF_ABOUT"), tr("BTN_OK"),
		            		       QString::null, QString::null, 0, 0);
}

void Application::onQuit()
{
#ifdef aaa
  QWidgetList aList = stWs->windowList();
	for(MDIWindow* aWindow = (MDIWindow*) aList.first();aWindow;aWindow = (MDIWindow*) aList.next()) {
	  aWindow->closeEvent(0);
  }

	emit sendQuit();
#endif
}

MDIWindow* Application::getActiveMDI()
{
  return myActiveMDI;
}

void Application::onWindowActivated( QWidget* w )
{
  if ( w != NULL && w->inherits( "MDIWindow" ) ){
    myActiveMDI = (MDIWindow*)w;
		w->setFocus();
	}
	else if (myViewDlg)
	{
    delete myViewDlg; myViewDlg = NULL; 
  }
}

void Application::startTimer()
{
  timer.Reset();
  timer.Start();
}

void Application::stopTimer( int count, const char* mes, bool addToGraph, int aOpt ) 
{
  Standard_Integer m,h;
  Standard_Real fps,elaps,cpu;
  char title[256];
  const char* message = ( mes == NULL ) ? "" : mes;

  timer.Stop();
  timer.Show( elaps, m, h, cpu );
  elaps += m * 60. + h * 3600.;
  fps = Standard_Real( count ) / elaps;
  if( fps > 1. )
    sprintf( title, " ====> '%s'. speed is %f frames/second", message, fps );
  else {
    if( count > 1 ) { 
      elaps /= count; cpu /= count;
      sprintf( title, " ====> '%s'. average elapsed/cpu time is %f/%f seconds", message, elaps, cpu );
    }
    else {
      sprintf( title," ====> '%s'. elapsed/cpu time is %f/%f seconds.", message, elaps, cpu );
    }
  }
    
  if (addToGraph)
  {
    QString qmes(mes);
    MDIWindow::ResultType type = MDIWindow::Undefined;
    bool isOptOn = false;
	  if(aOpt >= 0) {
		  isOptOn |= (aOpt > 0);
	  } else {
		  isOptOn = Graphic3d_ArrayOfPrimitives::IsEnable();
	  }
    if (qmes.contains(tr("MES_DISPLAY"), Qt::CaseInsensitive))
      type = isOptOn ? MDIWindow::DisplayOpt : MDIWindow::DisplayNonOpt;
    else if (qmes.contains(tr("MES_UPDATE"), Qt::CaseInsensitive))
      type = isOptOn ? MDIWindow::UpdateOpt : MDIWindow::UpdateNonOpt;
    if (type != MDIWindow::Undefined)
    {
      addResult(type, elaps);
    }
  }

  QString str( title );
  cout << title << endl;
  Application::getApplication()->statusBar()->showMessage( str );
  Application::getApplication()->showMessage( str );
  //printf("%s\n",title);
}

void Application::showTimer( const char* mes )
{
  Standard_Integer m,h;
  Standard_Real elaps,cpu;
  char title[256];
  const char* message = ( mes == NULL) ? "" : mes;

  timer.Show( elaps, m, h, cpu );
  elaps += m * 60. + h * 3600.;
  sprintf( title," ====> '%s'. elapsed/cpu time is %f/%f seconds", message, elaps, cpu );

  QString str( title );
  Application::getApplication()->statusBar()->showMessage( str );
  Application::getApplication()->showMessage( str );
  cout << title << endl;
}

void Application::showMessage( QString& message )
{
#ifdef OUTPUT_WINDOW
  myOutput->print( message );
#endif
}

void Application::onToggleOptim(bool state)
{
  state ? Graphic3d_ArrayOfPrimitives::Enable() : Graphic3d_ArrayOfPrimitives::Disable();
}

void Application::addResult(MDIWindow::ResultType item, double value)
{
  MDIWindow* activeWin = getApplication()->getActiveMDI();
  Handle(AIS_InteractiveContext) activeCtx = activeWin->getDocument()->getContext();

  // Counting triangles...
  int nbTriangles = 0;
  AIS_ListOfInteractive displayedObjs;
  activeCtx->DisplayedObjects(displayedObjs);
  AIS_ListIteratorOfListOfInteractive dispIt(displayedObjs);
  for ( ; dispIt.More(); dispIt.Next())
  {
    Handle(AIS_InteractiveObject) aisObj = dispIt.Value();
    if (aisObj->IsKind(STANDARD_TYPE(Sphere_Sphere)))
    {
      Handle(Sphere_Sphere) sphere = Handle(Sphere_Sphere)::DownCast(aisObj);
      Standard_Real rad            = sphere->Radius();
      Standard_Real defl           = sphere->Sphere_BasicShape::Deflection();

      nbTriangles += Sphere_Sphere::NbItems(Sphere_Sphere::NbPanes(rad, defl));
    }
    else if (aisObj->IsKind(STANDARD_TYPE(AIS_Shape)))
    {
      Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(aisObj);
      TopoDS_Shape topoShape = shape->Shape();
      TopExp_Explorer exp(topoShape, TopAbs_FACE);
      for ( ; exp.More(); exp.Next())
      {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        if (!face.IsNull())
        {
          Handle(Poly_Triangulation) triangles = 
            Handle(BRep_TFace)::DownCast(face.TShape())->Triangulation();
          if (!triangles.IsNull()) 
          {
            nbTriangles += triangles->NbTriangles();
          }
        }
      }
    }
  }
  activeWin->addResult(item, nbTriangles, value);
}

QWorkspace* Application::workspace() const
{
  return myWorkSpace;
}

void Application::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape)
    myEscPressed = true;
}

void Application::onAutoTest()
{
  AutoTestDlg autoDlg(getApplication());
  if (autoDlg.exec() == QDialog::Accepted)
  {
    QApplication::setOverrideCursor( Qt::WaitCursor );
    myEscPressed = false;
	  myStopPressed = false;
	  QAction* stop = myStdActions.at(StopId);

	  stop->setEnabled(true);

    // Define the range for testing
    int startItems = autoDlg.getStartNbItems(); 
    int stopItems  = autoDlg.getStopNbItems();
    int step       = autoDlg.getStep();
	  bool isTe	   = autoDlg.isText();
    if (stopItems < startItems) stopItems = startItems;

    Standard_Real radius = 100; // default sphere radius
    gp_Pnt        pos(0, 0, 0); // default sphere center position
    
    Handle(AIS_InteractiveContext) ctx = getActiveMDI()->getDocument()->getContext();
    // Main loop
    for (int currItems = startItems; currItems <= stopItems; currItems += step)
    {
      for (int useOptim = 0; useOptim < 2; useOptim++)
      {
        qApp->processEvents();
        if (myEscPressed || myStopPressed)
        {
          QApplication::restoreOverrideCursor();
          stop->setEnabled(false);
          return;
        }

        // Toggle optimization
        useOptim || isTe ? Graphic3d_ArrayOfPrimitives::Enable() : Graphic3d_ArrayOfPrimitives::Disable();
        
        // Remove all old objects
        AIS_ListOfInteractive displayedObjs;
        ctx->DisplayedObjects(displayedObjs);
        AIS_ListIteratorOfListOfInteractive dispIt(displayedObjs);
        for ( ; dispIt.More(); dispIt.Next())
        {
          Handle(AIS_InteractiveObject) obj = dispIt.Value();
          if (!obj->IsKind(STANDARD_TYPE(AIS_Trihedron)))
            ctx->Remove(obj, false);
        }
        ctx->UpdateCurrentViewer();

		    getActiveMDI()->getView()->SetScale(3.0316);
        
        // Generate new sphere
        Standard_Integer nbPanes     = Sphere_Sphere::NbPanes(currItems);
        Standard_Real    deflection  = Sphere_Sphere::Deflection(radius, nbPanes);
        Handle(Sphere_Sphere) sphere = new Sphere_Sphere(pos, radius, deflection, 
          1/*myVNormalsFlag*/, 0/*myVColorsFlag*/, 0/*myVTexelsFlag*/, isTe, useOptim);
        
        // Display the sphere
        ctx->SetDisplayMode( sphere, getActiveMDI()->getDisplayMode(), false );
        Application::startTimer();
        ctx->Display( sphere, false );
        Application::stopTimer( 0, "Display Sphere::Sphere", true, useOptim);

        qApp->processEvents();
        if (myEscPressed || myStopPressed)
        {
          QApplication::restoreOverrideCursor();
		      stop->setEnabled(false);
          return;
        }

        // Update the viewer
        Application::startTimer();
        getActiveMDI()->getView()->Update();
        Application::stopTimer( 0, "UPDATE", true, useOptim);
      }
    }
    QApplication::restoreOverrideCursor();
	  stop->setEnabled(false);
  }
}

/* ------------------ class OutputWindow ----------------- */
OutputWindow::OutputWindow( QWidget* parent ) : QWidget( parent ), myLineCounter( 0 )
{
  setWindowTitle( "Output window" );
  resize( 400, 300 );

  QVBoxLayout* topLayout = new QVBoxLayout( this );
  myLineEdit = new QTextEdit( this );

  topLayout->addWidget( myLineEdit );
}

OutputWindow::~OutputWindow()
{
}

void OutputWindow::print( QString& s )
{
  myLineEdit->append( QString().setNum( myLineCounter ) + s );
  myLineCounter++;
}

void Application::InitApp()
{
	onNewDocument();
}

void Application::onStop()
{
	myStopPressed = true;
}

void Application::onEditViewProperties()
{
  if ( !myViewDlg ) {
    myViewDlg = new ViewDlg(this);
    verify( connect( myWorkSpace, SIGNAL( windowActivated( QWidget* ) ),
                     myViewDlg,   SLOT( Update() ) ) );
	}
	myViewDlg->raise();
	myViewDlg->show();
}

void Application::updateViewDlg()
{
	if(myViewDlg) myViewDlg->Update();
}