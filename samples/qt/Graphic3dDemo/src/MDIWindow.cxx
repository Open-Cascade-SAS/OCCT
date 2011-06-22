#include <qlayout.h>
#include <qframe.h>
#include <qtoolbar.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qsplitter.h>
#include <qmenu.h>
#include <qevent.h>

#include "MDIWindow.h"
#include "global.h"
#include "Document.h"
#include "View.h"
#include "ViewOperations.h"
#include "Graph.h"
#include "Application.h"

MDIWindow::MDIWindow( Document* aDocument, QWidget* parent, V3d_TypeOfView type )
: QMainWindow( parent )
{
	resize( 500, 300 );
  QSplitter* central = new QSplitter(Qt::Vertical, this);

	myDocument = aDocument;
  Handle(AIS_InteractiveContext) cxt = myDocument->getContext();
  cxt->CurrentViewer()->SetDefaultTypeOfView( type );
	myView = new View( cxt, central, this );
	verify(connect(myView, SIGNAL(ViewInitialized()), 
  Application::getApplication(), SLOT(updateViewDlg())));

  // Graph initialization
  myGraphView = new GraphSplitView(central);
  GraphView* pGraph = myGraphView->getGraph();
  GraphItem* pItem = new GraphItem(pGraph, tr("LEG_NONOPT_DISPLAY"));
  pItem->setSorted(true);
  pItem->setMarker(0, QColor( Qt::blue ), Qt::SolidLine);
  pItem->addNode(0, 0);
  pItem = new GraphItem(pGraph, tr("LEG_OPT_DISPLAY"));
  pItem->setSorted(true);
  pItem->setMarker(1, QColor( Qt::red ), Qt::SolidLine);
  pItem->addNode(0, 0);
  pItem = new GraphItem(pGraph, tr("LEG_NONOPT_UPDATE"));
  pItem->setSorted(true);
  pItem->setMarker(2, QColor( Qt::blue ), Qt::DotLine);
  pItem->addNode(0, 0);
  pItem = new GraphItem(pGraph, tr("LEG_OPT_UPDATE"));
  pItem->setSorted(true);
  pItem->setMarker(3, QColor( Qt::red ), Qt::DotLine);
  pItem->addNode(0, 0);
  pGraph->setXTitle(tr("LAB_TRIANGLES"));
  pGraph->setYTitle(tr("LAB_SECONDS"));
  pGraph->installEventFilter(this);
  pGraph->setOperationEnabled(GraphView::voRect,  true);
  pGraph->setOperationEnabled(GraphView::voZoom,  true);
  pGraph->setOperationEnabled(GraphView::voZoomX, true);
  pGraph->setOperationEnabled(GraphView::voZoomY, true);
  pGraph->setOperationEnabled(GraphView::voPan,   true);
  pGraph->setOperationEnabled(GraphView::voPanX,  true);
  pGraph->setOperationEnabled(GraphView::voPanY,  true);

  central->setStretchFactor(0, 5);
  central->setStretchFactor(1, 1);
	setCentralWidget( central );

  myGraphPopup = new QMenu(pGraph);

	myOperations = new ViewOperations( myDocument->getContext() );

	verify( connect( myView,       SIGNAL( noActiveActions() ), 
                   myOperations, SLOT( onNoActiveActions() ) ) );
	verify( connect( myOperations, SIGNAL( setCursor( const QCursor& ) ),
                                 SLOT( setCursor( const QCursor& ) ) ) );
	
	createViewActions();
  createGraphActions();
}

MDIWindow::~MDIWindow()
{
  delete myGraphView;
}

Document* MDIWindow::getDocument()
{
  return myDocument;
}

int MDIWindow::getDisplayMode()
{
  return myOperations->getDisplayMode();
}

void MDIWindow::closeEvent(QCloseEvent* e)
{
  emit sendCloseView( this );
}

void MDIWindow::createViewActions()
{
  QToolBar* aToolBar = new QToolBar( tr( "View Operations" ), this );
  addToolBar( Qt::TopToolBarArea, aToolBar );
	
	QList<QAction*> aList = myOperations->getToolActions();

  QActionGroup* dModeGroup = new QActionGroup( aToolBar );
  QAction* a;

  int i = 0;
  foreach( a, aList ) {
    aToolBar->addAction( a );
//        if ( i == 1 )
//            aToolBar->addSeparator();
    if ( i == ViewOperations::ToolWireframeId || i == ViewOperations::ToolShadingId ) {
      dModeGroup->addAction( a );
    }
    i++;
  }
  aToolBar->addSeparator();

  aList = myOperations->getViewActions();
  foreach( a, aList ) aToolBar->addAction( a );
}

void MDIWindow::setCursor(const QCursor& aCursor)
{
  myView->setCursor(aCursor);
}

void MDIWindow::defineMagView()
{
  myView->setCurrentAction();
}

Handle(V3d_View)& MDIWindow::getView()
{
  return myView->getView();
}

void MDIWindow::activateAction( ViewOperations::Action action )
{
  myView->startAction( action );
}

void MDIWindow::addResult(ResultType item, int x, double y)
{
  if (item == Undefined) return;

  GraphView* pGraph = myGraphView->getGraph();
  pGraph->getItem((int)item)->addNode(x, y);
  double xMin, xMax, yMin, yMax;
  pGraph->getRange(xMin, yMin, xMax, yMax);
  pGraph->fitData(0, xMax, 0, yMax);
}

bool MDIWindow::eventFilter(QObject* dstObj, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent* mEvent = (QMouseEvent*)event;
    if (mEvent->button() == Qt::RightButton &&
      !(mEvent->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)))
      graphPopup(mEvent->globalPos());
  }
  return FALSE;
}

void MDIWindow::createGraphActions()
{
  QAction* a = new QAction( tr( "MEN_GRAPH_FIT_ALL" ), this );
  a->setToolTip( tr( "TBR_GRAPH_FIT_ALL" ) );
	verify( connect( a, SIGNAL( activated() ) , this, SLOT( onGraphFitAll() ) ) );
  myGraphPopup->addAction( a );

  myGraphPopup->addSeparator();

  a = new QAction( tr( "MEN_GRAPH_CLEAR" ), this );
  a->setToolTip( tr( "TBR_GRAPH_CLEAR" ) );
	verify( connect( a, SIGNAL( activated() ) , this, SLOT( onGraphClear() ) ) );
  myGraphPopup->addAction( a );
}

void MDIWindow::graphPopup(const QPoint& pos)
{
  myGraphPopup->popup(pos);
}

void MDIWindow::onGraphClear()
{
  GraphView* graph = myGraphView->getGraph();
  for (int i = 0; i < graph->getNbItems(); i++)
  {
    graph->getItem(i)->clear();
    graph->getItem(i)->addNode(0, 0);
  }
  graph->repaint();
}

void MDIWindow::onGraphFitAll()
{
  GraphView* graph = myGraphView->getGraph();
  double xMin, xMax, yMin, yMax;
  graph->getRange(xMin, yMin, xMax, yMax);
  graph->fitData(0, xMax, 0, yMax);
}
