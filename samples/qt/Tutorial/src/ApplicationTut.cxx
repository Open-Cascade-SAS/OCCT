#include "ApplicationTut.h"
#include "DocumentTut.h"

#include <QFileDialog>
#include <QStatusBar>

ApplicationTut::ApplicationTut()
    : ApplicationCommonWindow( )
{
  createMakeBottleOperation();
}

ApplicationTut::~ApplicationTut()
{
}

void ApplicationTut::createMakeBottleOperation(){
	QPixmap MakeBottleIcon;
	QString dir = getTutResourceDir() + QString( "/" );
  MakeBottleIcon = QPixmap( dir+QObject::tr( "ICON_MAKE_BOTTLE" ) );
	
  QAction * MakeBottleAction = new QAction( MakeBottleIcon, QObject::tr("TBR_MAKEBOT"), this );
  MakeBottleAction->setToolTip( QObject::tr( "TBR_MAKEBOT" ) );
  MakeBottleAction->setStatusTip( QObject::tr("TBR_MAKEBOT") );
  MakeBottleAction->setShortcut( QObject::tr( "CTRL+M" ) );
	connect( MakeBottleAction, SIGNAL( activated() ) , this, SLOT( onMakeBottleAction() ) );
	
	myMakeBottleBar = addToolBar( tr( "Make Bottle" ) );
  insertToolBar( getCasCadeBar(), myMakeBottleBar );
  myMakeBottleBar->addAction( MakeBottleAction );
	myMakeBottleBar->hide();
}

void ApplicationTut::updateFileActions()
{
  if ( getWorkspace()->windowList().isEmpty() )
  {
	  if ( !isDocument() )
		{
      myMakeBottleBar->show();
    }
    else
    {
      myMakeBottleBar->hide();
    }
  }
  ApplicationCommonWindow::updateFileActions();
}

void ApplicationTut::onMakeBottleAction()
{
	QWorkspace* ws = ApplicationCommonWindow::getWorkspace();
  DocumentTut* doc = (DocumentTut*)((MDIWindow*)ws->activeWindow())->getDocument();
	statusBar()->showMessage( QObject::tr("INF_MAKE_BOTTLE"), 5000 );
	doc->onMakeBottle();
	statusBar()->showMessage(QObject::tr("INF_DONE"));
}

QString ApplicationTut::getTutResourceDir()
{
    static QString resDir( ::getenv( "CSF_TuturialResourcesDefaults" ) );
    return resDir;
}

