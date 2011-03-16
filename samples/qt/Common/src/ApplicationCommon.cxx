#include "ApplicationCommon.h"

#include "DocumentCommon.h"
#include "View.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QWorkspace>
#include <QMessageBox>
#include <QApplication>
#include <QSignalMapper>

#include <stdlib.h>

static ApplicationCommonWindow* stApp;
static QWorkspace* stWs;

ApplicationCommonWindow::ApplicationCommonWindow()
    : QMainWindow( 0 ),
myWindowPopup( 0 ),
myFilePopup( 0 ),
myCasCadeBar( 0 ),
myStdToolBar( 0 )
{
  myNbDocuments = 0;
  stApp = this;
  myIsDocuments = false;

  // create and define the central widget
  QFrame* vb = new QFrame( this );

  QVBoxLayout *layout = new QVBoxLayout( vb );
  layout->setMargin( 0 );

  vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  stWs = new QWorkspace( vb );
  layout->addWidget( stWs );
  setCentralWidget( vb );
	
  createStandardOperations();
  createCasCadeOperations();

  statusBar()->showMessage( QObject::tr("INF_READY"), 5000 );
  resize( 1000, 700 );
}

ApplicationCommonWindow::~ApplicationCommonWindow()
{
}

void ApplicationCommonWindow::createStandardOperations()
{
  QPixmap newIcon, helpIcon, closeIcon;

  QString dir = getResourceDir() + QString( "/" );

  newIcon = QPixmap( dir+QObject::tr("ICON_NEW") );
  helpIcon = QPixmap( dir+QObject::tr("ICON_HELP") );
  closeIcon = QPixmap( dir+QObject::tr("ICON_CLOSE") );

  QAction * fileNewAction, * fileCloseAction, * fileQuitAction,
          * viewToolAction, * viewStatusAction,
		  * helpAboutAction;

  fileNewAction = new QAction( newIcon, QObject::tr("MNU_NEW"), this );
  fileNewAction->setToolTip( QObject::tr("TBR_NEW") );
  fileNewAction->setStatusTip( QObject::tr("TBR_NEW") );
  fileNewAction->setShortcut( QObject::tr("CTRL+N") );
  connect( fileNewAction, SIGNAL( activated() ) , this, SLOT( onNewDoc() ) );
  myStdActions.insert( FileNewId, fileNewAction );

  fileCloseAction = new QAction( closeIcon, QObject::tr("MNU_CLOSE"), this );
  fileCloseAction->setToolTip( QObject::tr("TBR_CLOSE") );
  fileCloseAction->setStatusTip( QObject::tr("TBR_CLOSE") );
  fileCloseAction->setShortcut( QObject::tr("CTRL+W") );
  connect( fileCloseAction, SIGNAL( activated() ) , this, SLOT( onCloseWindow() ) );
  myStdActions.insert( FileCloseId, fileCloseAction );

  fileQuitAction = new QAction( QObject::tr("MNU_QUIT"), this );
  fileQuitAction->setToolTip( QObject::tr("TBR_QUIT") );
  fileQuitAction->setStatusTip( QObject::tr("TBR_QUIT") );
  fileQuitAction->setShortcut( QObject::tr("CTRL+Q") );
  connect( fileQuitAction, SIGNAL( activated() ) , qApp, SLOT( closeAllWindows() ) );
  myStdActions.insert( FileQuitId, fileQuitAction );

  viewToolAction = new QAction( QObject::tr("MNU_TOOL_BAR"), this );
  viewToolAction->setToolTip( QObject::tr("TBR_TOOL_BAR") );
  viewToolAction->setStatusTip( QObject::tr("TBR_TOOL_BAR") );
  connect( viewToolAction, SIGNAL( activated() ) , this, SLOT( onViewToolBar() ));
  viewToolAction->setCheckable( true );
  viewToolAction->setChecked( true );
  myStdActions.insert( ViewToolId, viewToolAction );

  viewStatusAction = new QAction( QObject::tr("MNU_STATUS_BAR"), this );
  viewStatusAction->setToolTip( QObject::tr("TBR_STATUS_BAR") );
  viewStatusAction->setStatusTip( QObject::tr("TBR_STATUS_BAR") );
  connect( viewStatusAction, SIGNAL( activated() ), this, SLOT( onViewStatusBar() ));
  viewStatusAction->setCheckable( true );
  viewStatusAction->setChecked( true );
  myStdActions.insert( ViewStatusId, viewStatusAction );

  helpAboutAction = new QAction( helpIcon, QObject::tr("MNU_ABOUT"), this );
  helpAboutAction->setToolTip( QObject::tr( "TBR_ABOUT" ) );
  helpAboutAction->setStatusTip( QObject::tr( "TBR_ABOUT" ) );
  helpAboutAction->setShortcut( QObject::tr( "F1" ) );
  connect( helpAboutAction, SIGNAL( activated() ) , this, SLOT( onAbout() ) );
  myStdActions.insert( HelpAboutId, helpAboutAction );

    // popuplate a menu with all actions
  myFilePopup = new QMenu( this );
  myFilePopup = menuBar()->addMenu( QObject::tr("MNU_FILE") );
  myFilePopup->addAction( fileNewAction );
  myFilePopup->addAction( fileCloseAction );
  myFileSeparator = myFilePopup->addSeparator();
  myFilePopup->addAction( fileQuitAction );
    
	// add a view menu
  QMenu * view = new QMenu( this );
	
  view = menuBar()->addMenu( QObject::tr("MNU_VIEW") );
  view->addAction( viewToolAction );
  view->addAction( viewStatusAction );
	
	// add a help menu

  QMenu * help = new QMenu( this );
  menuBar()->addSeparator();
  help = menuBar()->addMenu( QObject::tr("MNU_HELP") );
  help->addAction( helpAboutAction );

	// populate a tool bar with some actions
  myStdToolBar = addToolBar( tr( "File Operations" ) );
  myStdToolBar->addAction( fileNewAction );
  myStdToolBar->addAction( helpAboutAction );
	
  myStdActions.at(FileCloseId)->setEnabled(myDocuments.count() > 0);
}

void ApplicationCommonWindow::createCasCadeOperations()
{
  createWindowPopup();

  // populate a tool bar with some actions
  myCasCadeBar = addToolBar( tr( "Shape Operations" ) );

  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_WIRE") ), QObject::tr("MNU_TOOL_WIRE"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_WIRE") );
  a->setStatusTip( QObject::tr("TBR_TOOL_WIRE") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolWireframeId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_SHAD") ), QObject::tr("MNU_TOOL_SHAD"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_SHAD") );
  a->setStatusTip( QObject::tr("TBR_TOOL_SHAD") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolShadingId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_COLOR") ), QObject::tr("MNU_TOOL_COLOR"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_COLOR") );
  a->setStatusTip( QObject::tr("TBR_TOOL_COLOR") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolColorId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_MATER") ), QObject::tr("MNU_TOOL_MATER"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_MATER") );
  a->setStatusTip( QObject::tr("TBR_TOOL_MATER") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolMaterialId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_TRANS") ), QObject::tr("MNU_TOOL_TRANS"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_TRANS") );
  a->setStatusTip( QObject::tr("TBR_TOOL_TRANS") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolTransparencyId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_TOOL_DEL") ), QObject::tr("MNU_TOOL_DEL"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_DEL") );
  a->setStatusTip( QObject::tr("TBR_TOOL_DEL") );
  connect( a, SIGNAL( activated() ) , this, SLOT( onToolAction() ) );
  myToolActions.insert( ToolDeleteId, a );

  QSignalMapper* sm = new QSignalMapper( this );
  connect( sm, SIGNAL( mapped( int ) ), this, SLOT( onSetMaterial( int ) ) );

  a = new QAction( QObject::tr("MNU_BRASS"), this );
  a->setToolTip( QObject::tr("TBR_BRASS") );
  a->setStatusTip( QObject::tr("TBR_BRASS") );
  sm->setMapping( a,(int)Graphic3d_NOM_BRASS );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRASS, a );

  a = new QAction( QObject::tr("MNU_BRONZE"), this );
  a->setToolTip( QObject::tr("TBR_BRONZE") );
  a->setStatusTip( QObject::tr("TBR_BRONZE") );
  sm->setMapping( a, ( int )Graphic3d_NOM_BRONZE );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRONZE, a );

  a = new QAction( QObject::tr("MNU_COPPER"), this );
  a->setToolTip( QObject::tr("TBR_COPPER") );
  a->setStatusTip( QObject::tr("TBR_COPER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_COPPER );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_COPPER, a );

  a = new QAction( QObject::tr("MNU_GOLD"), this );
  a->setToolTip( QObject::tr("TBR_GOLD") );
  a->setStatusTip( QObject::tr("TBR_GOLD") );
  sm->setMapping( a, ( int )Graphic3d_NOM_GOLD );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_GOLD, a );

  a = new QAction( QObject::tr("MNU_PEWTER"), this );
  a->setToolTip( QObject::tr("TBR_PEWTER") );
  a->setStatusTip( QObject::tr("TBR_PEWTER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PEWTER );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PEWTER, a );

  a = new QAction( QObject::tr("MNU_PLASTER"), this );
  a->setToolTip( QObject::tr("TBR_PLASTER") );
  a->setStatusTip( QObject::tr("TBR_PLASTER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTER );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTER, a );

  a = new QAction( QObject::tr("MNU_PLASTIC"), this );
  a->setToolTip( QObject::tr("TBR_PLASTIC") );
  a->setStatusTip( QObject::tr("TBR_PLASTIC") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTIC );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTIC, a );

  a = new QAction( QObject::tr("MNU_SILVER"), this );
  a->setToolTip( QObject::tr("TBR_SILVER") );
  a->setStatusTip( QObject::tr("TBR_SILVER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_SILVER );
  connect( a, SIGNAL( activated() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_SILVER, a );

  for ( int i = 0; i < myToolActions.size(); i++ )
    myCasCadeBar->addAction( myToolActions.at( i ) );
  myCasCadeBar->hide();
}

QList<QAction*>* ApplicationCommonWindow::getToolActions()
{
    return &myToolActions;
}

QList<QAction*>* ApplicationCommonWindow::getMaterialActions()
{
    return &myMaterialActions;
}

void ApplicationCommonWindow::createWindowPopup()
{
    if ( !myWindowPopup )
    {
      myWindowPopup = new QMenu( QObject::tr( "MNU_WINDOW" ), this );
	    connect( myWindowPopup, SIGNAL( aboutToShow() ),
		         this, SLOT( windowsMenuAboutToShow() ) );
    }
}

void ApplicationCommonWindow::windowsMenuAboutToShow()
{
  myWindowPopup->clear();
  QAction* a;

  QString dir = getResourceDir() + QString( "/" );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_NEW3D" ) ), QObject::tr( "MNU_WINDOW_NEW3D" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_NEW3D" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_NEW3D" ) );
  connect( a, SIGNAL( activated() ), this, SLOT( onCreateNewView() ) );
  myWindowPopup->addAction( a );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_CASCADE" ) ), QObject::tr( "MNU_WINDOW_CASCADE" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_CASCADE" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_CASCADE" ) );
  connect( a, SIGNAL( activated() ), stWs, SLOT( cascade() ) );
  myWindowPopup->addAction( a );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_TILE" ) ), QObject::tr( "MNU_WINDOW_TILE" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_TILE" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_TILE" ) );
  connect( a, SIGNAL( activated() ), stWs, SLOT( tile() ) );
  myWindowPopup->addAction( a );

  myWindowPopup->addSeparator();
  QWidgetList windows = stWs->windowList();
  for ( uint i = 0; i < windows.count(); ++i )
  {
		
	  QAction* aAction = new QAction( windows.at(i)->windowTitle(), this );
    aAction->setCheckable( true );
    aAction->setData( i );
    myWindowPopup->addAction( aAction );
    connect( aAction, SIGNAL( toggled( bool ) ), this, SLOT( windowsMenuActivated( bool ) ) );
    aAction->setChecked( stWs->activeWindow() == windows.at(i) );
  }
}

void ApplicationCommonWindow::windowsMenuActivated( bool checked )
{
  QAction* aSender = qobject_cast<QAction*>( sender() );
  if ( !aSender )
	return;
  QWidget* w = stWs->windowList().at( aSender->data().toInt() );
  if ( w && checked )
    w->setFocus();
}

QWorkspace * ApplicationCommonWindow::getWorkspace()
{
  return stWs;
}

ApplicationCommonWindow* ApplicationCommonWindow::getApplication()
{
  return stApp;
}

void ApplicationCommonWindow::updateFileActions()
{
  if ( stWs->windowList().isEmpty() )
  {
    if ( !myIsDocuments )
    {
      QAction *fileQuitAction, *windowAction;
	    QList<QAction *> aListActions = myFilePopup->actions();
	    for ( int i = 0; i < aListActions.size(); i++ )
      {
        if( aListActions.at( i )->text() == QObject::tr("MNU_QUIT") )
        {
		      fileQuitAction = aListActions.at( i );
		      break;
        }
      }
    		
	    if( !fileQuitAction )
        return;
      
      myIsDocuments = true;
      myCasCadeBar->show();

	    QList<QAction *> aListMenuActions = menuBar()->actions();
	    for ( int i = 0; i < aListMenuActions.size(); i++ )
      {
        if( aListMenuActions.at( i )->text() == QObject::tr("MNU_HELP") )
        {
	         windowAction= aListMenuActions.at( i );
		       break;
        }
      }

      if( !windowAction )
        return;

      menuBar()->insertMenu( windowAction, myWindowPopup );
    }
    else
    {
	    myIsDocuments = false;
	    myCasCadeBar->hide();
	    menuBar()->removeAction( myWindowPopup->menuAction() );
    }
  }
}

DocumentCommon* ApplicationCommonWindow::createNewDocument()
{
  return new DocumentCommon( ++myNbDocuments, this );
}

int& ApplicationCommonWindow::getNbDocument()
{
  return myNbDocuments;
}

DocumentCommon* ApplicationCommonWindow::onNewDoc()
{
    updateFileActions();
    DocumentCommon* aDoc = createNewDocument();
    aDoc->onCreateNewView();
    onSelectionChanged();
    connect( aDoc, SIGNAL( sendCloseDocument( DocumentCommon* ) ),
             this, SLOT( onCloseDocument( DocumentCommon* ) ) );
    connect( stWs, SIGNAL( windowActivated( QWidget* ) ),
             this, SLOT( onWindowActivated( QWidget* ) ) );
    connect( aDoc, SIGNAL( selectionChanged() ),
             this, SLOT( onSelectionChanged() ) );
    myDocuments.append( aDoc );
    myStdActions.at( FileCloseId )->setEnabled( myDocuments.count() > 0 );
    return aDoc;
}

void ApplicationCommonWindow::onCloseWindow(){
    MDIWindow* m = (MDIWindow*)stWs->activeWindow();
    if ( m )
    {
        DocumentCommon* doc = m->getDocument();
        onCloseDocument( doc );
    }
}

void ApplicationCommonWindow::onCloseDocument(DocumentCommon* theDoc)
{
  myDocuments.removeAll( theDoc );
  theDoc->removeViews();
  delete theDoc;
  updateFileActions();
  myStdActions.at(FileCloseId)->setEnabled(myDocuments.count() > 0);
}

void ApplicationCommonWindow::onViewToolBar()
{
    bool show = myStdActions.at( ViewToolId )->isChecked();
    if (  show == myStdToolBar->isVisible() )
        return;
    if ( show )
        myStdToolBar->show();
    else
        myStdToolBar->hide();
}

void ApplicationCommonWindow::onViewStatusBar()
{
    bool show = myStdActions.at( ViewStatusId )->isChecked();
    if (  show == statusBar()->isVisible() )
        return;
    if ( show )
        statusBar()->show();
    else
        statusBar()->hide();
}

void ApplicationCommonWindow::onAbout()
{
    QMessageBox::information( this, QObject::tr( "TIT_ABOUT" ), QObject::tr( "INF_ABOUT" ), QObject::tr("BTN_OK" ),
		  		              QString::null, QString::null, 0, 0 );
}

void ApplicationCommonWindow::onCreateNewView()
{
    DocumentCommon* doc = ((MDIWindow*) stWs->activeWindow())->getDocument();
    doc->onCreateNewView();
}

void ApplicationCommonWindow::onWindowActivated ( QWidget * w )
{
    if ( w )
        ((MDIWindow*) w)->onWindowActivated();
}

void ApplicationCommonWindow::onToolAction()
{
    QAction* sentBy = (QAction*) sender();
    QWorkspace* ws = ApplicationCommonWindow::getWorkspace();
    DocumentCommon* doc = ((MDIWindow*)ws->activeWindow())->getDocument();

    if( sentBy == myToolActions.at( ToolWireframeId ) )
        doc->onWireframe();

    if( sentBy == myToolActions.at( ToolShadingId ) )
        doc->onShading();

    if( sentBy == myToolActions.at( ToolColorId ) )
        doc->onColor();

    if( sentBy == myToolActions.at( ToolMaterialId ) )
        doc->onMaterial();

    if( sentBy == myToolActions.at( ToolTransparencyId ) )
        doc->onTransparency();

    if( sentBy == myToolActions.at( ToolDeleteId ) )
        doc->onDelete();
}

void ApplicationCommonWindow::onSelectionChanged()
{
  QWorkspace* ws = ApplicationCommonWindow::getWorkspace();
	DocumentCommon* doc;

	if( !qobject_cast<MDIWindow*>( ws->activeWindow() ) )
	  return;
    doc = ((MDIWindow*)ws->activeWindow())->getDocument();
    Handle(AIS_InteractiveContext) context = doc->getContext();

    bool OneOrMoreInShading = false;
    bool OneOrMoreInWireframe = false;
    int numSel = context->NbSelected();
    if ( numSel )
    {
        for ( context->InitCurrent(); context->MoreCurrent(); context->NextCurrent() )
        {
            if ( context->IsDisplayed( context->Current(), 1 ) )
                OneOrMoreInShading = true;
            if ( context->IsDisplayed( context->Current(), 0 ) )
                OneOrMoreInWireframe = true;
        }
        myToolActions.at( ToolWireframeId )->setEnabled( OneOrMoreInShading );
        myToolActions.at( ToolShadingId )->setEnabled( OneOrMoreInWireframe );
        myToolActions.at( ToolColorId )->setEnabled( true );
        myToolActions.at( ToolMaterialId )->setEnabled( true );
        myToolActions.at( ToolTransparencyId )->setEnabled( OneOrMoreInShading );
        myToolActions.at( ToolDeleteId )->setEnabled( true );
    }
    else
    {
        myToolActions.at( ToolWireframeId )->setEnabled( false );
        myToolActions.at( ToolShadingId )->setEnabled( false );
        myToolActions.at( ToolColorId )->setEnabled( false );
        myToolActions.at( ToolMaterialId )->setEnabled( false );
        myToolActions.at( ToolTransparencyId )->setEnabled( false );
        myToolActions.at( ToolDeleteId )->setEnabled( false );
    }
}

void ApplicationCommonWindow::onSetMaterial( int theMaterial )
{
    QWorkspace* ws = getWorkspace();
    DocumentCommon* doc = ((MDIWindow*)ws->activeWindow())->getDocument();
    doc->onMaterial( theMaterial );
}

QString ApplicationCommonWindow::getResourceDir()
{
    static QString resDir( ::getenv( "CSF_ResourcesDefaults" ) );    
    return resDir;
}

void ApplicationCommonWindow::resizeEvent( QResizeEvent* e )
{
    QMainWindow::resizeEvent( e );
    statusBar()->setSizeGripEnabled( !isMaximized() );
}

bool ApplicationCommonWindow::isDocument()
{
  return myIsDocuments;
}

QMenu* ApplicationCommonWindow::getFilePopup()
{
  return myFilePopup;
}

QAction* ApplicationCommonWindow::getFileSeparator()
{
  return myFileSeparator;
}

QToolBar* ApplicationCommonWindow::getCasCadeBar()
{
  return myCasCadeBar;
}


