#include "ViewOperations.h"
#include "global.h"
//#include "Material.h"
//#include "Transparency.h"
#include "Application.h"
#include "Document.h"

#include <qcolordialog.h>
#include <qapplication.h>
#include <qsignalmapper.h>
#include <qworkspace.h>
#include <qpixmap.h>
#include <qcursor.h>  
#include <qaction.h>

#include <V3d_TypeOfView.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <Quantity_Color.hxx>


  static QCursor* defCursor = NULL;
  static QCursor* handCursor = NULL;
  static QCursor* panCursor = NULL;
  static QCursor* zoomCursor = NULL;
  static QCursor* rotCursor = NULL;

ViewOperations::ViewOperations( QObject* parent) 
: QObject( parent )
{
	myContext = NULL;
}

ViewOperations::ViewOperations( Handle(AIS_InteractiveContext) theContext,
                                QObject* parent)
: QObject( parent )
{
    myContext = theContext;
	initActions();
	initViewActions();
}

ViewOperations::~ViewOperations()
{
	myContext = NULL;
}

void ViewOperations::onWireframe()
{
    AIS_ListOfInteractive displayed;
    myContext->DisplayedObjects( displayed );
    AIS_ListIteratorOfListOfInteractive it( displayed );
    for ( ; it.More(); it.Next() )
        myContext->SetDisplayMode( it.Value(), 0 );
#ifdef a
  for(myContext->InitCurrent();myContext->MoreCurrent();myContext->NextCurrent())
        myContext->SetDisplayMode(myContext->Current(),0);
#endif
}

void ViewOperations::onShading()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    
    AIS_ListOfInteractive displayed;
    myContext->DisplayedObjects( displayed );
    AIS_ListIteratorOfListOfInteractive it( displayed );
    for ( ; it.More(); it.Next() )
        myContext->SetDisplayMode( it.Value(), 1 );

#ifdef a
    for(myContext->InitCurrent();myContext->MoreCurrent();myContext->NextCurrent())
        myContext->SetDisplayMode(myContext->Current(),1);
#endif
  QApplication::restoreOverrideCursor();                
}

void ViewOperations::initActions()
{
	initCursors();
	initToolActions();
}

void ViewOperations::initCursors()
{
    if( !defCursor ) 
        defCursor = new QCursor( Qt::ArrowCursor );
    if( !handCursor ) 
        handCursor = new QCursor( Qt::PointingHandCursor );
    if( !panCursor ) 
        panCursor = new QCursor( Qt::SizeAllCursor );
    
    QString dir( Application::getResourceDir() );
    if( !zoomCursor ) 
        zoomCursor = new QCursor( QPixmap( dir + tr( "ICON_CURSOR_ZOOM" ) ) );
    if( !rotCursor ) 
        rotCursor = new QCursor( QPixmap( dir + tr( "ICON_CURSOR_ROTATE" ) ) );
}
  
QList<QAction*> ViewOperations::getViewActions()
{
	return myViewActions;
}

void ViewOperations::initViewActions()
{
  if ( myViewActions.count() )
    return;

	QString dir = Application::getResourceDir();

	QAction* a;

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_FITALL" ) ), "", this );
  a->setToolTip( tr( "TBR_FITALL" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewFitAllId, a );
	
	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_FITAREA" ) ), "", this );
  a->setToolTip( tr( "TBR_FITAREA" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	a->setCheckable( true );
	verify( connect( a, SIGNAL( toggled( bool ) ), SLOT( updateToggled( bool ) ) ) );
	myViewActions.insert( ViewFitAreaId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_ZOOM" ) ), "", this );
  a->setToolTip( tr( "TBR_ZOOM" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	a->setCheckable( true );
	verify( connect( a, SIGNAL( toggled( bool ) ), SLOT( updateToggled( bool ) ) ) );
	myViewActions.insert( ViewZoomId, a );
	
	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_PAN" ) ), "", this );
  a->setToolTip( tr( "TBR_PAN" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	a->setCheckable( true );
	verify( connect( a, SIGNAL( toggled( bool ) ), SLOT( updateToggled( bool ) ) ) );
	myViewActions.insert( ViewPanId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_GLOBALPAN" ) ), "", this );
  a->setToolTip( tr( "TBR_GLOBALPAN" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	a->setCheckable( true );
	verify( connect( a, SIGNAL( toggled( bool ) ), SLOT( updateToggled( bool ) ) ) );
	myViewActions.insert( ViewGlobalPanId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_FRONT" ) ), "", this );
  a->setToolTip( tr( "TBR_FRONT" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewFrontId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_BACK" ) ), "", this );
  a->setToolTip( tr( "TBR_BACK" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewBackId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_TOP" ) ), "", this );
  a->setToolTip( tr( "TBR_TOP" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewTopId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_BOTTOM" ) ), "", this );
  a->setToolTip( tr( "TBR_BOTTOM" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewBottomId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_LEFT" ) ), "", this );
  a->setToolTip( tr( "TBR_LEFT" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewLeftId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_RIGHT" ) ), "", this );
  a->setToolTip( tr( "TBR_RIGHT" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewRightId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_AXO" ) ), "", this );
  a->setToolTip( tr( "TBR_AXO" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewAxoId, a );
	  
	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_ROTATION" ) ), "", this );
  a->setToolTip( tr( "TBR_ROTATION" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	a->setCheckable( true );
	verify( connect( a, SIGNAL( toggled( bool ) ), SLOT( updateToggled( bool ) ) ) );
	myViewActions.insert( ViewRotationId, a );

	a = new QAction( QPixmap( dir + tr( "ICON_VIEW_RESET" ) ), "", this );
  a->setToolTip( tr( "TBR_RESET" ) );
  verify( connect( a, SIGNAL( activated() ), SLOT( onActionActivated() ) ) );
	myViewActions.insert( ViewResetId, a );
}

void ViewOperations::updateToggled( bool isOn ) 
{
	if( !isOn )
		return;

    QAction* sentBy = (QAction*) sender();
	
    for ( int i = ViewFitAllId;i <= ViewResetId ; i++)
    {
		QAction* anAction = myViewActions.at(i);
		if( (anAction == myViewActions.at(ViewFitAreaId)) ||
                (anAction == myViewActions.at(ViewZoomId)) ||
		(anAction == myViewActions.at(ViewPanId)) ||
                (anAction == myViewActions.at(ViewGlobalPanId)) ||
                (anAction == myViewActions.at(ViewRotationId))) {
		  if (anAction && (anAction != sentBy)) {
		    anAction->setCheckable(true);
		    anAction->setChecked(false);
		  } else {
		    if(sentBy  == myViewActions.at(ViewFitAreaId))
		      emit setCursor(*handCursor);
		    else if	(sentBy == myViewActions.at(ViewZoomId))
		      emit setCursor(*zoomCursor);
		    else if	(sentBy == myViewActions.at(ViewPanId))
		      emit setCursor(*panCursor);
		    else if	(sentBy == myViewActions.at(ViewGlobalPanId))
		      emit setCursor(*defCursor);
		    else if	(sentBy == myViewActions.at(ViewRotationId))
		      emit setCursor(*rotCursor);
		    sentBy->setCheckable(false);
		  }
		}
	}
}

QList<QAction*> ViewOperations::getToolActions()
{
	initToolActions();
	return myToolActions;
}

void ViewOperations::initToolActions()
{
  if( myToolActions.count() )
		return;

	QString dir = Application::getResourceDir();

    //QAction* a = new QAction( tr( "TBR_TOOL_MAGVIEW" ), QPixmap( dir + tr( "ICON_TOOL_WIRE" ) ),
    //                          "", 0, this );
	//verify( connect( a, SIGNAL( activated() ), SLOT( onToolAction() ) ) );
	//myToolActions->insert( MagViewId, a );	

//    QAction* a = new QAction( tr( "TBR_TOOL_AXOVIEW" ), QPixmap( dir + tr( "ICON_TOOL_WIRE" ) ),
//                     "", 0, this );
//	verify( connect( a, SIGNAL( activated() ), SLOT( onToolAction() ) ) );
//	myToolActions->insert( AxoViewId, a );	

//    a = new QAction( tr( "TBR_TOOL_PERVIEW" ), QPixmap( dir + tr( "ICON_TOOL_WIRE" ) ),
//                     "", 0, this );
//	verify( connect( a, SIGNAL( activated() ), SLOT( onToolAction() ) ) );
//	myToolActions->insert( PerViewId, a );	

	QAction* a = new QAction( QPixmap( dir + tr( "ICON_TOOL_WIRE" ) ), "", this );
  a->setToolTip( tr( "TBR_TOOL_WIRE" ) );
  a->setCheckable( true );
  verify( connect( a, SIGNAL( activated() ), SLOT( onToolAction() ) ) );
	myToolActions.insert( ToolWireframeId, a );	

	a = new QAction( QPixmap( dir + tr( "ICON_TOOL_SHAD") ),  "", this );
  a->setToolTip( tr( "TBR_TOOL_SHAD" ) );
  a->setCheckable( true );
	a->setChecked( true );
	verify( connect( a, SIGNAL( activated() ), SLOT( onToolAction() ) ) );
	myToolActions.insert( ToolShadingId, a );	
}

void ViewOperations::onToolAction()
{
    QAction* sentBy = (QAction*) sender();
    //MDIWindow* win = (MDIWindow*) Application::getWorkspace()->activeWindow(); 
    MDIWindow* win = (MDIWindow*)Application::getApplication()->getActiveMDI();
	Document* doc = win->getDocument();
	
//	if( sentBy == myToolActions->at( MagViewId ) )
//		win->defineMagView();

//    if( sentBy == myToolActions->at( PerViewId ) )
//		doc->createNewView( V3d_PERSPECTIVE );

//    if( sentBy == myToolActions->at( AxoViewId ) )
//		doc->createNewView();

	if( sentBy == myToolActions.at( ToolWireframeId ) )
		doc->onWireframe();

	if( sentBy == myToolActions.at( ToolShadingId ) )
		doc->onShading();
}

int ViewOperations::getDisplayMode()
{
    bool on = myToolActions.at( ToolWireframeId )->isChecked();
    return on ? 0 : 1;
}
  
void ViewOperations::onNoActiveActions()
{
    for ( int i = ViewFitAllId;i <= ViewResetId ; i++ ) {
        QAction* anAction = myViewActions.at( i );
        if( ( anAction == myViewActions.at( ViewFitAreaId ) ) ||
	        ( anAction == myViewActions.at( ViewZoomId ) ) ||
	        ( anAction == myViewActions.at( ViewPanId ) ) ||
	        ( anAction == myViewActions.at( ViewGlobalPanId ) ) ||
	        ( anAction == myViewActions.at( ViewRotationId ) ) ) {
            
            emit setCursor( *defCursor );
            anAction->setCheckable( true );
            anAction->setChecked( false );
        }
    }
}

void ViewOperations::onActionActivated()
{
    QAction* a = (QAction*) sender();
    //MDIWindow* win = (MDIWindow*) Application::getWorkspace()->activeWindow();	
    MDIWindow* win = (MDIWindow*) Application::getApplication()->getActiveMDI();
    int index = myViewActions.indexOf( a );
    if ( index != -1 )
        win->activateAction( (Action)index );
}
