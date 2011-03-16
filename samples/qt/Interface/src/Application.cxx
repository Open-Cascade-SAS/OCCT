#include "Application.h"

#include "Translate.h"

#include <QMessageBox>
#include <stdlib.h>

ApplicationWindow::ApplicationWindow()
    : ApplicationCommonWindow( ),
      myImportPopup( 0 ),
      myExportPopup( 0 )
{
  createTranslatePopups();
}

ApplicationWindow::~ApplicationWindow()
{
}

void ApplicationWindow::createTranslatePopups()
{
    if ( !myImportPopup )
        myImportPopup = new QMenu( QObject::tr( "MNU_FILE_IMPORT" ), this );

    if ( !myExportPopup )
	    myExportPopup = new QMenu( QObject::tr( "MNU_FILE_EXPORT" ), this );

	QAction* a;
	a = new QAction( QObject::tr("MNU_IMPORT_BREP"), this );
	a->setStatusTip( QObject::tr("TBR_IMPORT_BREP") );
	connect( a, SIGNAL( activated() ), this, SLOT( onImport() ) );
	myCasCadeTranslateActions.insert( FileImportBREPId, a );
	myImportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_BREP"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_BREP") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportBREPId, a );
	myExportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_IMPORT_CSFDB"), this );
	a->setStatusTip( QObject::tr("TBR_IMPORT_CSFDB") );
	connect( a, SIGNAL( activated() ), this, SLOT( onImport() ) );
	myCasCadeTranslateActions.insert( FileImportCSFDBId, a );
	myImportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_CSFDB"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_CSFDB") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportCSFDBId, a );
	myExportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_IMPORT_IGES"), this );
	a->setStatusTip( QObject::tr("TBR_IMPORT_IGES") );
	connect( a, SIGNAL( activated() ), this, SLOT( onImport() ) );
	myCasCadeTranslateActions.insert( FileImportIGESId, a );
	myImportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_IGES"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_IGES") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportIGESId, a );
	myExportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_IMPORT_STEP"), this );
	a->setStatusTip( QObject::tr("TBR_IMPORT_STEP") );
	connect( a, SIGNAL( activated() ), this, SLOT( onImport() ) );
	myCasCadeTranslateActions.insert( FileImportSTEPId, a );
	myImportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_STEP"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_STEP") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportSTEPId, a );
	myExportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_STL"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_STL") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportSTLId, a );
	myExportPopup->addAction( a );

	a = new QAction( QObject::tr("MNU_EXPORT_VRML"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_VRML") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExport() ) );
	myCasCadeTranslateActions.insert( FileExportVRMLId, a );
	myExportPopup->addAction( a );

	myExportPopup->addSeparator();

	a = new QAction( QObject::tr("MNU_EXPORT_IMAGE"), this );
	a->setStatusTip( QObject::tr("TBR_EXPORT_IMAGE") );
	connect( a, SIGNAL( activated() ), this, SLOT( onExportImage() ) );
	myExportPopup->addAction( a );
}

void ApplicationWindow::updateFileActions()
{
  if ( getWorkspace()->windowList().isEmpty() )
  {
    if ( !isDocument() )
    {
      getFilePopup()->insertMenu( getFileSeparator(), myExportPopup );
	    getFilePopup()->insertMenu( myExportPopup->menuAction(), myImportPopup );
      mySeparator = getFilePopup()->insertSeparator( myImportPopup->menuAction() );
	  }
    else
    {
	    getFilePopup()->removeAction( myImportPopup->menuAction() );
	    getFilePopup()->removeAction( myExportPopup->menuAction() );
	    getFilePopup()->removeAction( mySeparator );
	  }
  }
  ApplicationCommonWindow::updateFileActions();
}

void ApplicationWindow::onImport()
{
    QAction* a = (QAction*)sender();
    int type = translationFormat( a );
    if ( type < 0 )
        return;

    bool stat = translate( type, true );
    if ( stat )
    {
        DocumentCommon* doc = ((MDIWindow*) getWorkspace()->activeWindow())->getDocument();
        doc->fitAll();
    }
}

void ApplicationWindow::onExport()
{
    QAction* a = (QAction*)sender();
    int type = translationFormat( a );
    if ( type < 0 )
        return;

    bool stat = translate( type, false );
}

int ApplicationWindow::translationFormat( const QAction* a )
{
    int type = -1;
    for ( int i = FileImportBREPId; i <= FileExportVRMLId; i++ )
    {
        if ( myCasCadeTranslateActions.at( i ) == a )
        {
            type = i;
            break;
        }
    }
    switch ( type )
    {
    case FileImportBREPId:
    case FileExportBREPId:
        type = Translate::FormatBREP;
        break;
    case FileImportIGESId:
    case FileExportIGESId:
        type = Translate::FormatIGES;
        break;
    case FileImportSTEPId:
    case FileExportSTEPId:
        type =  Translate::FormatSTEP;
        break;
    case FileImportCSFDBId:
    case FileExportCSFDBId:
        type = Translate::FormatCSFDB;
        break;
    case FileExportSTLId:
        type = Translate::FormatSTL;
        break;
    case FileExportVRMLId:
        type = Translate::FormatVRML;
        break;
    }
    return type;
}

bool ApplicationWindow::translate( const int format, const bool import )
{
    static Translate* anTrans = createTranslator();
    DocumentCommon* doc = ((MDIWindow*) getWorkspace()->activeWindow())->getDocument();
    Handle(AIS_InteractiveContext) context = doc->getContext();
    bool status;
    if ( import )
        status = anTrans->importModel( format, context );
    else
        status = anTrans->exportModel( format, context );

    if ( !status )
    {
        QString msg = QObject::tr( "INF_TRANSLATE_ERROR" );
        if ( !anTrans->info().isEmpty() )
            msg += QString( "\n" ) + anTrans->info();
        QMessageBox::critical( this, QObject::tr( "TIT_ERROR" ), msg, QObject::tr( "BTN_OK" ), QString::null, QString::null, 0, 0 );
    }
    
    return status;
}

Translate* ApplicationWindow::createTranslator()
{
  Translate* anTrans = new Translate( this/*, "Translator"*/ );
  return anTrans;
}

void ApplicationWindow::onSelectionChanged()
{
  ApplicationCommonWindow::onSelectionChanged();

  QWorkspace* ws = getWorkspace();
  DocumentCommon* doc = ((MDIWindow*)ws->activeWindow())->getDocument();
  Handle(AIS_InteractiveContext) context = doc->getContext();
  int numSel = context->NbSelected();

  myCasCadeTranslateActions.at( FileExportBREPId )->setEnabled( numSel );
  myCasCadeTranslateActions.at( FileExportCSFDBId )->setEnabled( numSel );
  myCasCadeTranslateActions.at( FileExportIGESId )->setEnabled( numSel );
  myCasCadeTranslateActions.at( FileExportSTEPId )->setEnabled( numSel );
  myCasCadeTranslateActions.at( FileExportSTLId )->setEnabled( numSel );
  myCasCadeTranslateActions.at( FileExportVRMLId )->setEnabled( numSel );
}

QString ApplicationWindow::getIEResourceDir()
{
    static QString resDir( ::getenv( "CSF_IEResourcesDefaults" ) );
    return resDir;
}

void ApplicationWindow::onExportImage()
{
    MDIWindow* w = (MDIWindow*)getWorkspace()->activeWindow();
    if ( w )
        w->dump();
}


