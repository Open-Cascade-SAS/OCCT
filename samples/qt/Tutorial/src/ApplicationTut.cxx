#include "ApplicationTut.h"
#include "DocumentTut.h"

#include <QFileDialog>
#include <QStatusBar>
#include <QMdiSubWindow>

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
	connect( MakeBottleAction, SIGNAL( triggered() ) , this, SLOT( onMakeBottleAction() ) );
	
	myMakeBottleBar = addToolBar( tr( "Make Bottle" ) );
  insertToolBar( getCasCadeBar(), myMakeBottleBar );
  myMakeBottleBar->addAction( MakeBottleAction );
	myMakeBottleBar->hide();
}

void ApplicationTut::updateFileActions()
{
  if ( getWorkspace()->subWindowList().isEmpty() )
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
	QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
  DocumentTut* doc = (DocumentTut*)( qobject_cast<MDIWindow*>( ws->activeSubWindow()->widget() )->getDocument() );
	statusBar()->showMessage( QObject::tr("INF_MAKE_BOTTLE"), 5000 );
	doc->onMakeBottle();
	statusBar()->showMessage(QObject::tr("INF_DONE"));
}

QString ApplicationTut::getTutResourceDir()
{
  static QString resDir (qgetenv ("CSF_TutorialResourcesDefaults").constData());
  return resDir;
}

