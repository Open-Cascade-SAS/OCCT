///////////////////////////////////////////////////////////////////////
// File     : Graph.cpp
// Created  : 26.03.02
// Author   : Vadim SANDLER
//////////////////////////////////////////////////////////////////////

#include <Graph.h>

#include <math.h>
#include <float.h>

#include <qpainter.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpolygon.h>
#include <qevent.h>
#include <qtooltip.h>

static const char* const zoomCursorImage[] = { 
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
".#######........................",
"..aaaaaaa.......................",
"................................",
".............#####..............",
"...........##.aaaa##............",
"..........#.aa.....a#...........",
".........#.a.........#..........",
".........#a..........#a.........",
"........#.a...........#.........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
".........#...........#.a........",
".........#a..........#a.........",
".........##.........#.a.........",
"........#####.....##.a..........",
".......###aaa#####.aa...........",
"......###aa...aaaaa.......#.....",
".....###aa................#a....",
"....###aa.................#a....",
"...###aa...............#######..",
"....#aa.................aa#aaaa.",
".....a....................#a....",
"..........................#a....",
"...........................a....",
"................................",
"................................",
"................................",
"................................"};

static const char* const zoomXCursorImage[] = { 
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
"................................",
"................................",
"................................",
".............#####..............",
"...........##.aaaa##............",
"..........#.aa.....a#...........",
"....a....#.a.........#.....a....",
"...aa....#a..........#a....aa...",
"..a#a...#.a...........#....a#a..",
".a##aa..#a............#a..aa##a.",
"a#####..#a............#a..#####a",
".a##aa..#a............#a..aa##a.",
"..a#a...#a............#a...a#a..",
"...aa....#...........#.a...aa...",
"....a....#a..........#a....a....",
".........##.........#.a.........",
"........#####.....##.a..........",
".......###aaa#####.aa...........",
"......###aa...aaaaa.............",
".....###aa......................",
"....###aa.......................",
"...###aa........................",
"....#aa.........................",
".....a..........................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................"};

static const char* const zoomYCursorImage[] = { 
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................a...............",
"...............a#a..............",
"..............a###a.............", 
".............a#####a............",
"............aaaa#aaaa...........",
"...............a#a..............",
"................................",
"................................",
".............#####..............",
"...........##.aaaa##............",
"..........#.aa.....a#...........",
".........#.a.........#..........",
".........#a..........#a.........",
"........#.a...........#.........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
".........#...........#.a........",
".........#a..........#a.........",
".........##.........#.a.........",
"........#####.....##.a..........",
".......###aaa#####.aa...........",
"......###aa...aaaaa.............",
".....###aa......................",
"....###aa.......................",
"...###aa.......a#a..............",
"....#aa.....aaaa#aaaa...........",
".....a.......a#####a............",
"..............a###a.............",
"...............a#a..............",
"................a..............."};

#define SPACING_SIZE          5  // spacing
#define TICK_SIZE             3  // axes tick size
#define ARROW_LENGTH         20  // axis arrow size
#define ARROW_WIDTH           3  // ...
#define MIN_GRID_STEP        20  // minimum grid size in pixels
#define MIN_SENSIBILITY_SIZE 10  // minimum sensibility size
#define MIN(x, y)            ( (x) < (y) ? (x) : (y) )
#define MAX(x, y)            ( (x) > (y) ? (x) : (y) )

//////////////////////////////////////////////////////////////////////
// GraphView
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : GraphView::GraphView
// Purpose  : Constructor
//================================================================
GraphView::GraphView(QWidget* parent)
: QWidget(parent), // WResizeNoErase | WRepaintNoErase
  myRubberBand(0)
{
  // reseting legend
  myLegend = 0;
  // setting default cursors
  setOperationCursor(voNone,  Qt::ArrowCursor);
  setOperationCursor(voZoom,  QCursor(QPixmap((const char**)zoomCursorImage)));
  setOperationCursor(voZoomX, QCursor(QPixmap((const char**)zoomXCursorImage)));
  setOperationCursor(voZoomY, QCursor(QPixmap((const char**)zoomYCursorImage)));
  setOperationCursor(voPan,   Qt::SizeAllCursor);
  setOperationCursor(voPanX,  Qt::SizeHorCursor);
  setOperationCursor(voPanY,  Qt::SizeVerCursor);
  setOperationCursor(voRect,  Qt::PointingHandCursor);
  // setting default mouse-keyboard hot combinations
  setOperationButton(voZoom,  Qt::LeftButton,  Qt::ControlModifier);
  setOperationButton(voZoomX, Qt::LeftButton,  Qt::AltModifier);
  setOperationButton(voZoomY, Qt::RightButton, Qt::AltModifier);
  setOperationButton(voPan,   Qt::RightButton, Qt::ControlModifier);
  setOperationButton(voPanX,  Qt::LeftButton , Qt::ShiftModifier);
  setOperationButton(voPanY,  Qt::RightButton, Qt::ShiftModifier);
  setOperationButton(voRect,  Qt::LeftButton,  Qt::ControlModifier | Qt::ShiftModifier);
  // enabling operations globally
  setOperationsEnabled(true);
  // disable view operations by default
  setOperationEnabled(voZoom,  false);
  setOperationEnabled(voZoomX, false);
  setOperationEnabled(voZoomY, false);
  setOperationEnabled(voPan,   false);
  setOperationEnabled(voPanX,  false);
  setOperationEnabled(voPanY,  false);
  setOperationEnabled(voRect,  false);
  // creating colors
  myColors.append (Qt::white);
  myColors.append (Qt::blue);
  myColors.append (Qt::gray);
  myColors.append (Qt::darkGreen);
  myColors.append (Qt::magenta);
  myColors.append (Qt::darkGray);
  myColors.append (Qt::red);
  myColors.append (Qt::darkBlue);
  myColors.append (Qt::darkYellow);
  myColors.append (Qt::cyan);
  myColors.append (Qt::darkRed);
  myColors.append (Qt::darkCyan);
  myColors.append (Qt::yellow);
  myColors.append (Qt::darkMagenta);
  myColors.append (Qt::green);
  myColors.append (Qt::black);
  // creating line styles
  myLines.append (Qt::SolidLine);
  myLines.append (Qt::DashLine);
  myLines.append (Qt::DotLine);
  myLines.append (Qt::DashDotLine);
  myLines.append (Qt::DashDotDotLine);
  // initializing graph view  
  myOperation = myForcedOp = voNone;          // clearing current operation
  myOrigin = GraphNode(0.0, 0.0);             // setting origin to (0,0)
  setScale(1.0);                              // setting scale to 1.0
  myShowGrid      = true;                     // enabling grid drawing
  myShowAxes      = true;                     // enabling axes drawing
  myShowTitle     = true;                     // enabling title drawing
  myShowAxesTitle = true;                     // enabling axes titles drawing
  myShowTickMarks = true;                     // enabling tick marks drawing 
  myShowTooltips  = true;                     // enabling tooltips
  myShowMarkers   = true;                     // enabling markers drawing
  setGridStep     (100.0, 100.0);             // setting default floating grid step
  setFixedGridStep(100,   100);               // setting default fixed grid step
  setGridIntervals(10, 10);                   // setting default grid intervals
  setGridMode(gmFixed);                       // default grid mode is FIXED
  myMarkerSize    = 9;                        // default marker size
  setSensibilitySize(MIN_SENSIBILITY_SIZE);   // default sensibility size
  // starting watching mouse events
  setMouseTracking  (true);  
  // setting background color
  changeBackgroundColor(0); 
}

//================================================================
// Function : GraphView::~GraphView
// Purpose  : Destructor
//================================================================
GraphView::~GraphView() {
  qDeleteAll( myItems );
}

//================================================================
// Function : GraphView::eventFilter
// Purpose  : event filter 
//================================================================
bool GraphView::eventFilter(QObject* object, QEvent* event) {
  if ( ( event->type() == QEvent::MouseButtonPress ||
         event->type() == QEvent::KeyPress ) &&
      object != this) {
    startOperation(voNone);
    qApp->removeEventFilter(this);
  }
  return QWidget::eventFilter(object, event);
}

//================================================================
// Function : GraphView::setOperationCursor
// Purpose  : sets cursor for certain view operation
//================================================================
void GraphView::setOperationCursor (const ViewOperation operation, 
                                    const QCursor&      newCursor) {
  switch(operation) {
  case voZoom:
    myZoomCursor = newCursor; 
    break;
  case voZoomX: 
    myZoomXCursor = newCursor; 
    break;
  case voZoomY:
    myZoomYCursor = newCursor; 
    break;
  case voPan:
    myPanCursor = newCursor; 
    break;
  case voPanX:
    myPanXCursor = newCursor; 
    break;
  case voPanY:
    myPanYCursor = newCursor; 
    break;
  case voRect:
    myRectCursor = newCursor; 
    break;
  case voNone:
  default:
    myDefCursor = newCursor; 
    break;
  }
}

//================================================================
// Function : GraphView::getOperationCursor
// Purpose  : gets cursor for certain view operation
//================================================================
QCursor GraphView::getOperationCursor (const ViewOperation operation) const {
  switch(operation) {
  case voZoom:
    return myZoomCursor;
  case voZoomX:
    return myZoomXCursor;
  case voZoomY:
    return myZoomYCursor;
  case voPan:
    return myPanCursor; 
  case voPanX:
    return myPanXCursor;
  case voPanY:
    return myPanYCursor;
  case voRect:
    return myRectCursor;
  case voNone:
  default:
    break;
  }
  return myDefCursor;
}

//================================================================
// Function : GraphView::setOperationsEnabled
// Purpose  : globally enable/disables operations
//================================================================
void GraphView::setOperationsEnabled(bool on) {
  myEnableGlobal = on;
}

//================================================================
// Function : GraphView::setOperationEnabled
// Purpose  : enables/disables view operation 
//================================================================
void GraphView::setOperationEnabled(const ViewOperation operation, 
                                    bool                enable) {
  switch(operation) {
  case voZoom:
    myEnableZoom  = enable;
    break;
  case voZoomX:
    myEnableZoomX = enable;
    break;
  case voZoomY:
    myEnableZoomY = enable;
    break;
  case voPan:
    myEnablePan   = enable;
    break;
  case voPanX:
    myEnablePanX  = enable;
    break;
  case voPanY:
    myEnablePanY = enable;
    break;
  case voRect:
    myEnableRect = enable;
    break;
  }
}

//================================================================
// Function : GraphView::isOperationEnabled
// Purpose  : returns 'true' if view operation is enabled 
//================================================================
bool GraphView::isOperationEnabled(const ViewOperation operation) const {
  switch(operation) {
  case voZoom:
    return myEnableZoom;
  case voZoomX:
    return myEnableZoomX;
  case voZoomY:
    return myEnableZoomY;
  case voPan:
    return myEnablePan;
  case voPanX:
    return myEnablePanX;
  case voPanY:
    return myEnablePanY;
  case voRect:
    return myEnableRect;
  }
  return false;
}

//================================================================
// Function : GraphView::setOperationButton
// Purpose  : sets view operation key-mousebutton combination
//================================================================
void GraphView::setOperationButton(const ViewOperation         operation, 
                                   const Qt::MouseButtons      button,
                                   const Qt::KeyboardModifiers modifier) {
  switch (operation) {
  case voZoom:
    myZoomKey  = OperationButton(button, modifier);
    break;
  case voZoomX:
    myZoomXKey = OperationButton(button, modifier);
    break;
  case voZoomY:
    myZoomYKey = OperationButton(button, modifier);
    break;
  case voPan:
    myPanKey   = OperationButton(button, modifier);
    break;
  case voPanX:
    myPanXKey  = OperationButton(button, modifier);
    break;
  case voPanY:
    myPanYKey  = OperationButton(button, modifier);
    break;
  case voRect:
    myRectKey  = OperationButton(button, modifier);
    break;
  }
}

//================================================================
// Function : GraphView::getOperationButton
// Purpose  : gets operation key-mousebutton combination
//================================================================
void GraphView::getOperationButton(const ViewOperation    operation,
                                   Qt::MouseButtons&      button,
                                   Qt::KeyboardModifiers& modifier) const {
  switch (operation) {
  case voZoom:
    button   = myZoomKey.button;
    modifier = myZoomKey.modifier;
    break;
  case voZoomX:
    button   = myZoomXKey.button;
    modifier = myZoomXKey.modifier;
    break;
  case voZoomY:
    button   = myZoomYKey.button;
    modifier = myZoomYKey.modifier;
    break;
  case voPan:
    button   = myPanKey.button;
    modifier = myPanKey.modifier;
    break;
  case voPanX:
    button   = myPanXKey.button;
    modifier = myPanXKey.modifier;
    break;
  case voPanY:
    button   = myPanYKey.button;
    modifier = myPanYKey.modifier;
    break;
  case voRect:
    button   = myRectKey.button;
    modifier = myRectKey.modifier;
    break;
  case voNone:
  default:
    button   = Qt::NoButton;
    modifier = Qt::NoModifier;
    break;
  }
}

//================================================================
// Function : GraphView::testOperation
// Purpose  : tests for operation key-mousebutton combination
//================================================================
bool GraphView::testOperation (const ViewOperation         operation, 
                               const Qt::MouseButtons      button,
                               const Qt::KeyboardModifiers modifier) const {
  switch (operation) {
  case voZoom:
    return myZoomKey.button == button && myZoomKey.modifier == modifier;
  case voZoomX:
    return myZoomXKey.button == button && myZoomXKey.modifier == modifier;
  case voZoomY:
    return myZoomYKey.button == button && myZoomYKey.modifier == modifier;
  case voPan:
    return myPanKey.button == button && myPanKey.modifier == modifier;
  case voPanX:
    return myPanXKey.button == button && myPanXKey.modifier == modifier;
  case voPanY:
    return myPanYKey.button == button && myPanYKey.modifier == modifier;
  case voRect:
    return myRectKey.button == button && myRectKey.modifier == modifier;
  default:
    break;
  }
  return false;
}

//================================================================
// Function : GraphView::testOperation
// Purpose  : tests key-mousebutton combination and
//            returns view operation if any matches
//================================================================
GraphView::ViewOperation GraphView::testOperation(const Qt::MouseButtons      button,
                                                  const Qt::KeyboardModifiers modifier) const {
  if (testOperation(voZoom,  button, modifier))
    return voZoom;
  if (testOperation(voZoomX, button, modifier))
    return voZoomX;
  if (testOperation(voZoomY, button, modifier))
    return voZoomY;
  if (testOperation(voPan,   button, modifier))
    return voPan;
  if (testOperation(voPanX,  button, modifier))
    return voPanX;
  if (testOperation(voPanY,  button, modifier))
    return voPanY;
  if (testOperation(voRect,  button, modifier))
    return voRect;
  return voNone;
}

//================================================================
// Function : GraphView::reset
// Purpose  : resets view - sets origin to 0.0 and scales to 1.0
//================================================================
void GraphView::reset() {
  myOrigin = GraphNode(0.0, 0.0);
  myXScale = 1.0;
  myYScale = 1.0;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::fitAll
// Purpose  : fits viewer so display all data  
//================================================================
void GraphView::fitAll() {
  QRect pRect = getGraphRect();
  if (!pRect.isValid() || 
       pRect.right() == pRect.left() || 
       pRect.top()   == pRect.bottom())
    return;
  GraphItem* item;
  double xMin =  DBL_MAX;
  double xMax = -DBL_MAX;
  double yMin =  DBL_MAX;
  double yMax = -DBL_MAX;
  int cnt = 0;
  foreach (item, myItems ) {
    int i;
    cnt += item->count();
    for (i = 0; i < item->count(); i++) {
      GraphNode* node = item->getNode(i);
      if (node->x() < xMin)
        xMin = node->x();
      if (node->x() > xMax)
        xMax = node->x();
      if (node->y() < yMin)
        yMin = node->y();
      if (node->y() > yMax)
        yMax = node->y();
    }
  }
  if (cnt > 0) {
    double xOr = xMin;
    double yOr = yMin;
    double dx  = xMax - xMin;
    double dy  = yMax - yMin;
    int    idx = pRect.width();
    int    idy = pRect.height();
    if (dx > 0) {
      myXScale = dx * 1.001 / idx;
    }
    else {
      myXScale = 10 * fabs(xMin) / idx;
      xOr = xMin - 5 * fabs(xMin);
    }
    if (dy > 0) {
      myYScale = dy * 1.001 / idy;
    }
    else {
      myYScale = 10 * fabs(yMin) / idy;
      yOr = yMin - 5 * fabs(yMin);
    }
    myOrigin = GraphNode(xOr, yOr);
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::fitData
// Purpose  : fits viewer to display certain data
//================================================================
void GraphView::fitData(const double xMin, const double xMax,
                        const double yMin, const double yMax) {
  QRect pRect = getGraphRect();
  if (xMin == xMax || yMin == yMax)
    return;
  double xOr = (xMin < xMax) ? xMin : xMax;
  double yOr = (yMin < yMax) ? yMin : yMax;
  double dx  = fabs(xMax - xMin);
  double dy  = fabs(yMax - yMin);
  int    idx = pRect.width();
  int    idy = pRect.height();
  
  myXScale = dx * 1.001 / idx;
  myYScale = dy * 1.001 / idy;
  myOrigin = GraphNode(xOr, yOr);
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::fitDataX
// Purpose  : fits viewer to display certain data along horizontal axis
//================================================================
void GraphView::fitDataX(const double xMin, const double xMax) {
  QRect pRect = getGraphRect();
  if (xMin == xMax)
    return;
  double xOr = (xMin < xMax) ? xMin : xMax;
  double dx  = fabs(xMax - xMin);
  int    idx = pRect.width();
  myXScale = dx * 1.001 / idx;
  myOrigin = GraphNode(xOr, myOrigin.y());
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::fitDataY
// Purpose  : fits viewer to display certain data along vertical axis
//================================================================
void GraphView::fitDataY(const double yMin, const double yMax) {
  QRect pRect = getGraphRect();
  if (yMin == yMax)
    return;
  double yOr = (yMin < yMax) ? yMin : yMax;
  double dy  = fabs(yMax - yMin);
  int    idy = pRect.height();
  myYScale = dy * 1.001 / idy;
  myOrigin = GraphNode(myOrigin.x(), yOr);
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::pan
// Purpose  : performs pan operation by dx, dy pixels
//================================================================
void GraphView::pan(const int dx, const int dy) {
  myOrigin = GraphNode(myOrigin.x() - dx * myXScale,
                       myOrigin.y() + dy * myYScale);
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::zoom
// Purpose  : performs zoom operation by dx, dy pixels
//================================================================
void GraphView::zoom(const int dx, const int dy) {
  double dl = sqrt((double)dx * dx + (double)dy * dy);
  bool   sn = (abs(dx) > abs(dy)) ? (dx > 0) : (dy > 0);
  double pw = pow( 2.0, (dl / 100.0 * ( sn ? (1.0) : (-1.0)) ) );

  if (dx != 0 && dy != 0) {
    myXScale /= pw;
    myYScale /= pw;
  }
  else
  if (dy == 0)
    myXScale /= pow( 2.0, ((double)dx)/100 );
  else
  if (dx == 0)
    myYScale /= pow( 2.0, ((double)dy)/100 );

  update(); //repaint(false);
}

//================================================================
// Function : GraphView::startViewOperation
// Purpose  : starts operation if it is enabled
//================================================================
void GraphView::startViewOperation(const ViewOperation operation) {
  if (myOperation != voNone) {
    onFinishOperation();
    startOperation(voNone);
  }
  if (isOperationsEnabled() && isOperationEnabled(operation)) {
    setCursor(getOperationCursor(operation));
    myForcedOp = operation;
    qApp->installEventFilter(this);
  }
}

//================================================================
// Function : GraphView::showMarkers
// Purpose  : enables/disables internal markers drawing
//================================================================
void GraphView::showMarkers(bool show) {
  myShowMarkers = show;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::showTooltips
// Purpose  : enables/disables tooltips
//================================================================
void GraphView::showTooltips(bool show) {
  myShowTooltips = show;
}

//================================================================
// Function : GraphView::showAxes
// Purpose  : shows/hides axes
//================================================================
void GraphView::showAxes(bool show) {
  if (myShowAxes != show) {
    myShowAxes = show;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::showTickMark
// Purpose  : shows/hides tick-marks
//================================================================
void GraphView::showTickMark(bool show) {
  if (myShowTickMarks != show) {
    myShowTickMarks = show;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::showTitle
// Purpose  : shows/hides title
//================================================================
void GraphView::showTitle (bool show) {
  if (myShowTitle != show) {
    myShowTitle = show;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setTitle
// Purpose  : sets graph title
//================================================================
void GraphView::setTitle(const QString& title) {
  myTitle = title;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::showAxesTitle
// Purpose  : shows/hides axes title
//================================================================
void GraphView::showAxesTitle (bool show) {
  if (myShowAxesTitle != show) {
    myShowAxesTitle = show;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setXTitle
// Purpose  : sets graph X axis title
//================================================================
void GraphView::setXTitle(const QString& title) {
  myXTitle = title;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::setYTitle
// Purpose  : sets graph Y axis title
//================================================================
void GraphView::setYTitle(const QString& title) {
  myYTitle = title;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::showGrid
// Purpose  : shows/hides grid
//================================================================
void GraphView::showGrid(bool show) {
  if (myShowGrid != show) {
    myShowGrid = show;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setGridStep
// Purpose  : sets grid step by x, y axes, 
//            doesn't change step if it is <= 0
//================================================================
void GraphView::setGridStep(const double x, const double y, bool turn) {
  if (x > 0) {
    myGridStepX = x;
  }
  if (y > 0) {
    myGridStepY = y;
  }
  if (turn)
    myGridMode = gmFloating;
  if (myShowGrid)
    update(); //repaint(false);
}

//================================================================
// Function : GraphView::getGridStep
// Purpose  : gets grid step for x, y axes and returns true if grid is not fixed
//================================================================
bool GraphView::getGridStep(double& x, double& y) const {
  x = myGridStepX;
  y = myGridStepY;
  return (myGridMode == gmFloating);
}

//================================================================
// Function : GraphView::setFixedGridStep
// Purpose  : sets fixed grid step for x, y axes in pixels, 
//            doesn't change step if it is less 0
//================================================================
void GraphView::setFixedGridStep(const int x, const int y, bool turn) {
  if (x > 0) {
    myGridPStepX = x;
    if (myGridPStepX < MIN_GRID_STEP)
      myGridPStepX = MIN_GRID_STEP;
  }
  if (y > 0) {
    myGridPStepY = y;
    if (myGridPStepY < MIN_GRID_STEP)
      myGridPStepY = MIN_GRID_STEP;
  }
  if (turn)
    myGridMode = gmFixed;
  if (myShowGrid)
    update(); //repaint(false);
}

//================================================================
// Function : GraphView::getFixedGridStep
// Purpose  : gets fixed grid step for x, y axes in pixels 
//            and returns true if grid is fixed
//================================================================
bool GraphView::getFixedGridStep(int& x, int& y) const {
  x = myGridPStepX;
  y = myGridPStepY;
  return (myGridMode == gmFixed);
}

//================================================================
// Function : GraphView::setGridIntervals
// Purpose  : sets number of grid intervals (INTERVALS mode) for x, y axes, 
//            doesn't change if <= 0
//================================================================
void GraphView::setGridIntervals(const int xInt, 
                                 const int yInt, 
                                 bool      turn) {
  if (xInt > 0) {
    myGridIntX = xInt;
  }
  if (yInt > 0) {
    myGridIntY = yInt;
  }
  if (turn)
    myGridMode = gmIntervals;
  if (myShowGrid)
    update(); //repaint(false);
}

//================================================================
// Function : GraphView::getGridIntervals
// Purpose  : gets number of grid intervals (INTERVALS mode) for x, y axes, 
//            and returns true if grid mode is INTERVALS
//================================================================
bool GraphView::getGridIntervals(int &xInt, int &yInt) const {
  xInt = myGridIntX;
  yInt = myGridIntY;
  return (myGridMode == gmIntervals);
}

//================================================================
// Function : GraphView::setGridMode
// Purpose  : switches grid mode [FIXED, INTERVALS, FLOATING]
//================================================================
void GraphView::setGridMode(const GridMode mode) {
  if (myGridMode != mode) {
    myGridMode = mode;
    if (myShowGrid)
      update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setXScale
// Purpose  : sets X axis scale
//================================================================
void GraphView::setXScale(const double scale) {
  if (scale > 0) {
    myXScale = scale;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setYScale
// Purpose  : sets Y axis scale
//================================================================
void GraphView::setYScale(const double scale) {
  if (scale > 0) {
    myYScale = scale;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setScale
// Purpose  : sets X, Y axis scale
//================================================================
void GraphView::setScale(const double scale) {
  if (scale > 0) {
    myXScale = scale;
    myYScale = scale;
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::setOrigin
// Purpose  : moves axes origin to point [x, y]
//================================================================
void GraphView::setOrigin(const double x, const double y) {
  myOrigin = GraphNode(x, y);
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::getOrigin
// Purpose  : gets axes origin
//================================================================
void GraphView::getOrigin(double& x, double& y) const {
  x = myOrigin.x();
  y = myOrigin.y();
}

//================================================================
// Function : GraphView::changeBackgroundColor
// Purpose  : 
//================================================================
void GraphView::changeBackgroundColor(const int index) {
  int shift = 1;
  if (index >= 0 && index < getNbColors())
    shift = index;
  int i;
  unsigned j;
  unsigned aNb = myColors.count();
  if (aNb > 1) {
    for (i = 0; i < shift; i++) {
      QColor tmp = myColors.first();
      for (j = 0; j < aNb - 1; j++) {
        myColors[j] = myColors[j+1];
      }
      myColors[aNb - 1] = tmp;
    }
  }
  QPalette pal = palette();
  pal.setColor(QPalette::Window, myColors.first()); // QPalette::Active, 
  pal.setColor(QPalette::Text,   myColors.last()); //QPalette::Active, 
  setPalette(pal);
  update(); //repaint(false);
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::setTitleFont
// Purpose  : sets main title font
//================================================================
void GraphView::setTitleFont(QFont& font) {
  myTitleFont = font;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::setAxesTitleFont
// Purpose  : sets axes title font
//================================================================
void GraphView::setAxesTitleFont(QFont& font) {
  myAxesTitleFont = font;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::setTickMarksFont
// Purpose  : sets tick marks font
//================================================================
void GraphView::setTickMarksFont(QFont& font) {
  myTickMarksFont = font;
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::setLegend
// Purpose  : sets legend widget
//================================================================
void GraphView::setLegend(GraphLegend* legend) {
  myLegend = legend;
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::getBorders
// Purpose  : gets borders size
//================================================================
void GraphView::getMargins(int& leftMargin, 
                           int& rightMargin, 
                           int& topMargin, 
                           int& bottomMargin) {
  int db = MAX(SPACING_SIZE, (getMarkerSize() + 1)/2);
  topMargin    = db;
  bottomMargin = db;
  leftMargin   = db;
  rightMargin  = db;

  if (myShowTitle && !myTitle.isEmpty()) {
    QFontMetrics fm(myTitleFont);
    topMargin += ( fm.height() + SPACING_SIZE );
  }
  if (myShowAxesTitle && !myXTitle.isEmpty()) {
    QFontMetrics fm(myAxesTitleFont);
    bottomMargin += ( fm.height() + SPACING_SIZE );
  }
  if (myShowAxesTitle && !myYTitle.isEmpty()) {
    QFontMetrics fm(myAxesTitleFont);
    topMargin += ( fm.height() + SPACING_SIZE );
  }
  if (myShowTickMarks) {
    QFontMetrics fm(myTickMarksFont);
    rightMargin  += SPACING_SIZE * 3;
    leftMargin   += SPACING_SIZE * 10;
    bottomMargin += ( fm.height() + SPACING_SIZE );
    topMargin    += fm.height() / 2 + SPACING_SIZE;
  }
}

//================================================================
// Function : GraphView::insertItem
// Purpose  : inserts item into list
//================================================================
void GraphView::insertItem(GraphItem* item) {
  int aMarker, aColor, aLine;
  getNextMarker(aMarker, aColor, aLine);
  item->setMarker(aMarker, aColor, aLine);
  myItems.append(item);
  update(); //repaint(false);
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::removeItem
// Purpose  : removes item from list
//================================================================
void GraphView::removeItem(const int index) {
  myItems.removeAt(index);
  update(); //repaint(false);
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::clear
// Purpose  : clears data list
//================================================================
void GraphView::clear() {
  myItems.clear();
  update(); //repaint(false);
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::getItem
// Purpose  : gets item by index, returns NULL if index is out of range
//================================================================
GraphItem* GraphView::getItem(const int index) {
  return myItems.at(index);
}

//================================================================
// Function : GraphView::findItem
// Purpose  : returns index of item in data list 
//================================================================
int GraphView::findItem(GraphItem* item) {
  return myItems.indexOf(item);
}

//================================================================
// Function : GraphView::setSorted
// Purpose  : sets/unsets items to be sorted by horizontal coordinate
//================================================================
void GraphView::setSorted(bool sorted) {
  GraphItem* item;
  foreach (item, myItems) {
    item->setSorted(sorted);
  }
  update(); //repaint(false);
}

//================================================================
// Function : GraphView::getRange
// Purpose  : gets full data range
//================================================================
void GraphView::getRange(double& minX, double& minY, 
                         double& maxX, double& maxY) {
  minX =  DBL_MAX;
  maxX = -DBL_MAX;
  minY =  DBL_MAX;
  maxY = -DBL_MAX;
  GraphItem* item;
  int cnt = 0;
  foreach (item, myItems) {
    int i;
    cnt += item->count();
    for (i = 0; i < item->count(); i++) {
      GraphNode* node = item->getNode(i);
      if (node->x() < minX)
        minX = node->x();
      if (node->x() > maxX)
        maxX = node->x();
      if (node->y() < minY)
        minY = node->y();
      if (node->y() > maxY)
        maxY = node->y();
    }
  }
  if (cnt <= 0) {
    minX = 0.0;
    minY = 0.0;
    maxX = 0.0;
    maxY = 0.0;
  }
}

//================================================================
// Function : GraphView::getCurrentRange
// Purpose  : gets current data range
//================================================================
void GraphView::getCurrentRange(double& minX, double& minY,
                                double& maxX, double& maxY) {
  minX = myOrigin.x();
  minY = myOrigin.y();
  QRect pRect = getGraphRect();
  maxX = minX + myXScale * (pRect.width()  - 1); 
  maxY = minY + myYScale * (pRect.height() - 1); 
}

//================================================================
// Function : GraphView::getNbColors
// Purpose  : returns number of colors provided
//            default value is 16
//================================================================
int GraphView::getNbColors(){
  return myColors.count();
}

//================================================================
// Function : GraphView::getNbMarkers
// Purpose  : returns number of markers provided
//            default value is 11 : 
//            0 - square
//            1 - circle
//            2 - triangle (looks up)
//            3 - triangle (looks down)
//            4 - cross
//            5 - rhomb
//            6 - empty square
//            7 - empty circle
//            8 - empty rhomb
//            9 - empty triangle (looks up)
//            9 - empty triangle (looks down)
//================================================================
int GraphView::getNbMarkers() {
  return 11;
}

//================================================================
// Function : GraphView::getNbTypeLines
// Purpose  : returns number of type lines provided
//            default value is 5 : 
//            0 - SolidLine
//            1 - DashLine
//            2 - DotLine 
//            3 - DashDotLine
//            4 - DashDotDotLine
//================================================================
int GraphView::getNbTypeLines() {
  return myLines.count();
}

//================================================================
// Function : GraphView::setMarkerSize
// Purpose  : sets marker size
//================================================================
void GraphView::setMarkerSize(const int size) {
  myMarkerSize = size;
  update(); //repaint(false);
  if (myLegend)
    myLegend->updateMe();
}

//================================================================
// Function : GraphView::tip
// Purpose  : handles tooltips for the view
//================================================================
QRect GraphView::tip(const QPoint& point, QString& tipText) {
  if (!myShowTooltips/* || !myShowMarkers */)
    return QRect(0, 0, -1, -1);
  QRect where(QPoint(0,0), QSize(getMarkerSize(), getMarkerSize()));
  where.moveCenter(point);
  unsigned i;
  QRect pRect = this->getGraphRect();
  long minLength = -1;
  QListIterator<GraphItem*> iter(myItems);
  iter.toBack();
  while (iter.hasPrevious()) {
    GraphItem* aItem = iter.previous();
    unsigned aNbNodes = aItem->count();
    for (i = 0; i < aNbNodes; i++) {
      GraphNode* node = aItem->getNode(i);
      long x = (long)((node->x() - myOrigin.x()) / myXScale);
      long y = (long)((node->y() - myOrigin.y()) / myYScale);
      x = pRect.left()   + x;
      y = pRect.bottom() - y;
      QPoint dst = point - QPoint(x, y);
      long length = dst.x() * dst.x() + dst.y() * dst.y();
      if (where.contains(x, y) && (minLength < 0 || minLength > length)) {
        tipText = QString("(") + QString::number(node->x()) + ", " + 
                  QString::number(node->y()) + QString(")");
        if (!(aItem->getName().isEmpty()))
          tipText = aItem->getName() + " : " + tipText;
        minLength = length;
      }
    }
  }
  if (!tipText.isEmpty())
    return where;
  else
    return QRect(0, 0, -1, -1);
}

//================================================================
// Function : GraphView::isSameColors [static]
// Purpose  : returns true if colors seem to be the same 
//            (i.e. difference is less than gap)
//================================================================
bool GraphView::isSameColors(const QColor color1, 
                             const QColor color2, 
                             const int    gap) {
  int rC1, bC1, gC1;
  int rC2, bC2, gC2;
  color1.getRgb(&rC1, &gC1, &bC1);
  color2.getRgb(&rC2, &gC2, &bC2);
  return ( abs(rC1 - rC2) + abs(gC1 - gC2) + abs(bC1 - bC2) ) < gap;
}

//================================================================
// Function : GraphView::startOperation
// Purpose  : starts/finishes view operation
//================================================================
void GraphView::startOperation(ViewOperation theOp) {
  switch (theOp) {
  case voZoom:
  case voZoomX:
  case voZoomY:
  case voPan:
  case voPanX:
  case voPanY:
  case voRect:
    if (myOperation != voNone) {
      startOperation(voNone);
    }
    myOperation = theOp;
    setCursor(getOperationCursor(myOperation));
    onStartOperation();
    break;
  case voNone:
  default:
    setCursor(myDefCursor);
    myOperation = myForcedOp = voNone;
    break;
  }
}

//================================================================
// Function : GraphView::onStartOperation
// Purpose  : called on view operation starting
//================================================================
void GraphView::onStartOperation() {
  if (myOperation == voRect) {
    if (!myRubberBand)
      myRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    QRect r = QRect(myPoint, myOtherPoint).normalized();
    myRubberBand->setGeometry( r );
    myRubberBand->setVisible(r.isValid());
  }
}

//================================================================
// Function : GraphView::onFinishOperation
// Purpose  : called on view operation finishing
//================================================================
void GraphView::onFinishOperation() 
{
  if (myOperation == voRect) {
    QRect rect(myPoint, myOtherPoint);
    rect = rect.normalized();
    if (myRubberBand) myRubberBand->hide();
    if (!rect.isValid())
      return;
    if (rect.width() < mySensibilitySize || rect.height() < mySensibilitySize)
      return;
    QRect pRect = this->getGraphRect();
    int dx = rect.left()   - pRect.left();
    int dy = rect.bottom() - pRect.bottom();
    myOrigin = GraphNode(myOrigin.x() + dx * myXScale,
                         myOrigin.y() - dy * myYScale);
    myXScale = myXScale * rect.width()  / pRect.width();
    myYScale = myYScale * rect.height() / pRect.height();
    update(); //repaint(false);
  }
}

//================================================================
// Function : GraphView::onOperation
// Purpose  : called on view operation running
//================================================================
void GraphView::onOperation(QPoint mousePos) {
  int    dx = mousePos.x() - myPoint.x();
  int    dy = mousePos.y() - myPoint.y();
  double dl = sqrt((double)dx * dx + (double)dy * dy);
  bool   sn = (abs(dx) > abs(dy)) ? (dx > 0) : (dy > 0);
  double pw = pow( 2.0, (dl / 100.0 * ( sn ? (1.0) : (-1.0)) ) );

  switch (myOperation) {
  case voPan: 
    {
      myOrigin = GraphNode(myOrigin.x() - dx * myXScale,
                           myOrigin.y() + dy * myYScale);
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voPanX: 
    {
      myOrigin = GraphNode(myOrigin.x() - dx * myXScale,
                           myOrigin.y());
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voPanY: 
    {
      myOrigin = GraphNode(myOrigin.x(),
                           myOrigin.y() + dy * myYScale);
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voZoom: 
    {    
      myXScale /= pw;
      myYScale /= pw;
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voZoomX: 
    {    
      myXScale /= pow( 2.0, ((double)dx)/100 );
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voZoomY: 
    {    
      myYScale /= pow( 2.0, ((double)dy)/100 );
      myPoint = mousePos;
      update(); //repaint(false);
      break;
    }
  case voRect:
    {
      myOtherPoint = mousePos;
      QRect r = QRect(myPoint, myOtherPoint).normalized();
      myRubberBand->setGeometry( r );
      myRubberBand->setVisible(r.isValid());
      break;
    }
  }
}

//================================================================
// Function : GraphView::onMouseMove
// Purpose  : called on mouse moving action (e.g. for highlighting item)
//================================================================
void GraphView::onMouseMove(QPoint mousePos) {
}

//================================================================
// Function : GraphView::setSensibilitySize
// Purpose  : sets sensibility area size
//================================================================
void GraphView::setSensibilitySize(const int size) {
  mySensibilitySize = size;
  if (mySensibilitySize < MIN_SENSIBILITY_SIZE)
    mySensibilitySize = MIN_SENSIBILITY_SIZE;
}

//================================================================
// Function : GraphView::mousePressEvent
// Purpose  : MousePress event handler
//================================================================
void GraphView::mousePressEvent( QMouseEvent *e ) { 
  Qt::MouseButtons      aBtn = e->buttons();
  Qt::KeyboardModifiers aKey = e->modifiers();

  // finishing current viewer operation
  if (myOperation != voNone) {
    onFinishOperation();
    startOperation(voNone);
  }
  myOtherPoint = myPoint = e->pos();
  if (myForcedOp != voNone) {
    startOperation(myForcedOp);
  }
  else {
    if (isOperationsEnabled() && isOperationEnabled(testOperation(aBtn, aKey)))
      startOperation(testOperation(aBtn, aKey));
  }
}

//================================================================
// Function : GraphView::mouseMoveEvent
// Purpose  : MouseMove event handler
//================================================================
void GraphView::mouseMoveEvent(QMouseEvent* e) {
  if (myOperation != voNone)
    onOperation(e->pos());
  else
    onMouseMove(e->pos());
}

//================================================================
// Function : GraphView::mouseReleaseEvent
// Purpose  : MouseRelease event handler
//================================================================
void GraphView::mouseReleaseEvent(QMouseEvent *e) {
  // finishing current viewer operation
  if (myOperation != voNone) {
    onFinishOperation();
    startOperation(voNone);
  }
}

//================================================================
// Function : GraphView::mouseDoubleClickEvent
// Purpose  : MouseDoubleClick event handler
//================================================================
void GraphView::mouseDoubleClickEvent(QMouseEvent *e) {
}

//================================================================
// Function : GraphView::keyPressEvent
// Purpose  : KeyPress event handler
//================================================================
void GraphView::keyPressEvent( QKeyEvent *e ) {
//  emit vpKeyEvent( e );
}

//================================================================
// Function : GraphView::keyReleaseEvent
// Purpose  : KeyRelease event handler
//================================================================
void GraphView::keyReleaseEvent( QKeyEvent *e ) {
//  emit vpKeyEvent( e );
}

//================================================================
// Function : GraphView::resizeEvent
// Purpose  : Resize event handler
//================================================================
void GraphView::resizeEvent(QResizeEvent* e) {
  QWidget::resizeEvent(e);
}

//================================================================
// Function : GraphView::paintEvent
// Purpose  : Paint event handler
//================================================================
void GraphView::paintEvent(QPaintEvent* e) {
  QPixmap aBuffer = QPixmap(width(), height());
  aBuffer.fill(this, QPoint(0,0));
  
  QRect pRect = getGraphRect();
  if ( pRect.isValid() ||
       pRect.right() == pRect.left() || 
       pRect.top()   == pRect.bottom() ) {
    QPainter p(&aBuffer);
    // drawing grid
    if (myShowGrid) {
      p.save();
      drawGrid(&p);
      p.restore();
    }
    // or border
    else {
      p.save();
      drawBorder(&p);
      p.restore();
    }
    // drawing axes 
    if (myShowAxes) {
      p.save();
      drawAxes(&p);
      p.restore();
    }
    // drawing tick marks
    if (myShowTickMarks) {
      p.save();
      drawTickMarks(&p);
      p.restore();
    }
    // drawing axes titles 
    if (myShowAxesTitle) {
      p.save();
      drawAxesTitle(&p);
      p.restore();
    }
    // drawing title
    if (myShowTitle) {
      p.save();
      drawTitle(&p);
      p.restore();
    }
    // drawing data
    GraphItem* aItem;
    foreach (aItem, myItems) {
      p.save();
      drawItem(&p, aItem);
      p.restore();
    }
  }
  QPainter painter(this);
  painter.drawPixmap(QPoint(0, 0), aBuffer);
}

//================================================================
// Function : GraphView::event
// Purpose  : global event handler
//================================================================
bool GraphView::event(QEvent* e)
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
    QString tipText;
    QRect r = tip(helpEvent->pos(), tipText);
    if (r.isValid() && !tipText.isEmpty() )
      QToolTip::showText(helpEvent->globalPos(), tipText, this, r);
    else
      QToolTip::hideText();
  }
  return QWidget::event(e);
}

//================================================================
// Function : GraphView::getMargins
// Purpose  : gets graph painting area
//================================================================
QRect GraphView::getGraphRect() {
  int topMargin;
  int bottomMargin;
  int leftMargin;
  int rightMargin;

  getMargins(leftMargin, rightMargin, topMargin, bottomMargin);
  return QRect(QPoint(leftMargin, topMargin), 
               QPoint(width() - rightMargin, height() - bottomMargin));
}

//================================================================
// Function : GraphView::drawGrid
// Purpose  : draws grid
//================================================================
void GraphView::drawGrid(QPainter* painter) {
  if (!myShowGrid)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen
  painter->setPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  IntList xList, yList;
  this->getTickPoints(xList, yList);
  unsigned i;
  for (i = 0; i < xList.count(); i++) {
    painter->drawLine(pRect.left() + xList[i],
                      pRect.top(), 
                      pRect.left() + xList[i], 
                      pRect.bottom());
  }
  for (i = 0; i < yList.count(); i++) {
    painter->drawLine(pRect.left(),
                      pRect.bottom() - yList[i], 
                      pRect.right(), 
                      pRect.bottom() - yList[i]);
  }
  // forcing immediate painting
  //painter->flush();
}
 
//================================================================
// Function : GraphView::drawBorder
// Purpose  : draws border (it is drawn when grid is not)
//================================================================
void GraphView::drawBorder(QPainter* painter) {
  if (myShowGrid)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen
  painter->setPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  painter->drawLine(pRect.left(),
                    pRect.top(), 
                    pRect.left(), 
                    pRect.bottom());
  painter->drawLine(pRect.left(),
                    pRect.bottom(), 
                    pRect.right(), 
                    pRect.bottom());
  painter->drawLine(pRect.right(),
                    pRect.top(), 
                    pRect.right(), 
                    pRect.bottom());
  painter->drawLine(pRect.left(),
                    pRect.top(), 
                    pRect.right(), 
                    pRect.top());
  // forcing immediate painting
  //painter->flush();
}
 
//================================================================
// Function : GraphView::drawAxes
// Purpose  : draws axes
//================================================================
void GraphView::drawAxes(QPainter* painter) {
  if (!myShowAxes)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen, brush...  
  painter->setPen(QPen(palette().color(QPalette::Text), 1)); // QPalette::Active, 
  painter->setBrush(palette().color(QPalette::Text)); // QPalette::Active, 
  // drawing axes lines
  QPoint p1(pRect.left(),  pRect.top());
  QPoint p2(pRect.left(),  pRect.bottom());
  QPoint p3(pRect.right(), pRect.bottom());
  painter->drawLine(p1, p2);
  painter->drawLine(p2, p3);
  // drawing arrows
  QPolygon ar1(3), ar2(3);
  ar1[0] = p1;
  ar1[1] = QPoint(pRect.left() - ARROW_WIDTH, pRect.top() + ARROW_LENGTH);
  ar1[2] = QPoint(pRect.left() + ARROW_WIDTH, pRect.top()  + ARROW_LENGTH);
  ar2[0] = p3;
  ar2[1] = QPoint(pRect.right() - ARROW_LENGTH, pRect.bottom() + ARROW_WIDTH);
  ar2[2] = QPoint(pRect.right() - ARROW_LENGTH, pRect.bottom() - ARROW_WIDTH);
  if (pRect.height() > ARROW_LENGTH)
    painter->drawPolygon(ar1);
  if (pRect.width() > ARROW_LENGTH)
    painter->drawPolygon(ar2);
  // forcing immediate painting
  //painter->flush();
  // drawing axes marks
  IntList xList, yList;
  this->getTickPoints(xList, yList);
  unsigned i;
  // X axis marks
  for (i = 0; i < xList.count(); i++) {
    if (pRect.left() + xList[i] != pRect.right() && 
        xList[i] != 0) {
      painter->drawLine(pRect.left() + xList[i], pRect.bottom() - TICK_SIZE,
                        pRect.left() + xList[i], pRect.bottom() + TICK_SIZE);
    }
  }
  // Y axis marks
  for (i = 0; i < yList.count(); i++) {
    if (pRect.bottom() - yList[i] != pRect.top() && 
        yList[i] != 0) {
      painter->drawLine(pRect.left() - TICK_SIZE, pRect.bottom() - yList[i],
                        pRect.left() + TICK_SIZE, pRect.bottom() - yList[i]);
    }
  }
  // forcing immediate painting
  //painter->flush();
}


//================================================================
// Function : GraphView::drawTickMarks
// Purpose  : draws tick marks
//================================================================
void GraphView::drawTickMarks(QPainter* painter) {
  if (!myShowTickMarks)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen
  painter->setPen(QPen(palette().color(QPalette::Text), 1)); //QPalette::Active, 
  QFontMetrics fm(myTickMarksFont);
  painter->setFont(myTickMarksFont);
  int aTH = fm.height();
  IntList xList, yList;
  this->getTickPoints(xList, yList);
  unsigned i;
  // X axis tick marks
  int aLast = 0;
  int aFromX, aFromY;
  for (i = 0; i < xList.count(); i++) {
    QString xOr = QString::number(myOrigin.x() + myXScale * xList[i]);
    int aTW = fm.width(xOr);
    aFromY = pRect.bottom() + SPACING_SIZE;
    aFromX = pRect.left() + xList[i]  - aTW/2;
    if (aFromX + aTW + SPACING_SIZE > width())
      aFromX = width() - aTW - SPACING_SIZE;
    if (aFromX < SPACING_SIZE)
      aFromX = SPACING_SIZE;
    if (aFromX >= aLast) {
      painter->drawText(aFromX, aFromY, aTW, aTH, Qt::AlignCenter, xOr);
      aLast = aFromX + aTW + SPACING_SIZE * 2;
    }
  }
  // Y axis tick marks
  aLast = pRect.bottom() + SPACING_SIZE;
  for (i = 0; i < yList.count(); i++) {
    QString yOr = QString::number(myOrigin.y() + myYScale * yList[i]);
    int aTW = fm.width(yOr);
    aFromX = pRect.left() - aTW - SPACING_SIZE;
    if (aFromX < SPACING_SIZE)
      aFromX = SPACING_SIZE;
    aFromY = pRect.bottom() - yList[i] - aTH/2;
    if (aFromY + aTH > pRect.bottom() + SPACING_SIZE)
      aFromY = pRect.bottom() + SPACING_SIZE - aTH;
    if (aFromY < SPACING_SIZE)
      aFromY = SPACING_SIZE;
    if (aFromY + aTH <= aLast) {
      painter->drawText(aFromX, aFromY, aTW, aTH, Qt::AlignCenter, yOr);
      aLast = aFromY - SPACING_SIZE * 2;
    }
  }
  // forcing immediate painting
  //painter->flush();
}


//================================================================
// Function : GraphView::drawAxesTitle
// Purpose  : draws axes titles
//================================================================
void GraphView::drawAxesTitle(QPainter* painter) {
  if (!myShowAxesTitle)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen
  painter->setPen(palette().color(QPalette::Text)); //QPalette::Active, 
  QFontMetrics fm(myAxesTitleFont);
  painter->setFont(myAxesTitleFont);
  int aTH = fm.height();
  int aFromX, aFromY;
  if (!myXTitle.isEmpty()) {
    int aTW = fm.width(myXTitle);
    aFromX = pRect.right() - aTW/2;
    if (aFromX + aTW + SPACING_SIZE > width())
      aFromX = width() - aTW - SPACING_SIZE;
    aFromY = pRect.bottom() + SPACING_SIZE;
    if (myShowTickMarks)
      aFromY += (aTH + SPACING_SIZE);
    painter->drawText(aFromX, aFromY, aTW, aTH, Qt::AlignCenter, myXTitle);
  }
  if (!myYTitle.isEmpty()) {
    int aTW = fm.width(myYTitle);
    aFromX = pRect.left() - aTW/2;
    if (aFromX < SPACING_SIZE)
      aFromX = SPACING_SIZE;
    aFromY = pRect.top() - aTH/2 - SPACING_SIZE - aTH;
    painter->drawText(aFromX, aFromY, aTW, aTH, Qt::AlignCenter, myYTitle);
  }
  // forcing immediate painting
  //painter->flush();
}

//================================================================
// Function : GraphView::drawTitle
// Purpose  : draws graph title
//================================================================
void GraphView::drawTitle(QPainter* painter) {
  if (!myShowTitle)
    return;
  // getting painting region
  QRect pRect = getGraphRect();
  // setting pen
  painter->setPen(palette().color(QPalette::Text));//QPalette::Active, 
  QFontMetrics fm(myTitleFont);
  painter->setFont(myTitleFont);
  int aTH = fm.height();
  int aFromX, aFromY;
  if (!myTitle.isEmpty()) {
    int aTW = fm.width(myTitle);
    aFromX = pRect.left() + pRect.width() / 2 - aTW / 2;
    if (aFromX + aTW + SPACING_SIZE > width())
      aFromX = width() - aTW - SPACING_SIZE;
    if (aFromX < SPACING_SIZE)
      aFromX = SPACING_SIZE;
    aFromY = SPACING_SIZE;
    painter->drawText(aFromX, aFromY, aTW, aTH, Qt::AlignCenter, myTitle);
  }
  // forcing immediate painting
  //painter->flush();
}

//================================================================
// Function : GraphView::drawItem
// Purpose  : draws item
//================================================================
void GraphView::drawItem(QPainter* painter, GraphItem* item) {
  // checking item for validity
  if (!item->isValid())
    return;
  // getting marker, type and color of line
  int          aMarker = item->getMarker();
  QColor       aColor  = item->getColor();
  Qt::PenStyle aLine   = item->getLine();

  // getting painting region
  QRect pRect = getGraphRect();

  // setting pen for line
  painter->setPen(QPen(aColor, 0, aLine));
  int i;
  if (item->count() > 1) {
    // drawing lines
    painter->setClipRect(pRect);
    painter->setClipping(true);
    for (i = 0; i < item->count() - 1; i++) {
      GraphNode* node1 = item->getNode(i);
      GraphNode* node2 = item->getNode(i + 1);
      long x1 = (long)((node1->x() - myOrigin.x()) / myXScale);
      long y1 = (long)((node1->y() - myOrigin.y()) / myYScale);
      x1 = pRect.left()   + x1;
      y1 = pRect.bottom() - y1;
      long x2 = (long)((node2->x() - myOrigin.x()) / myXScale);
      long y2 = (long)((node2->y() - myOrigin.y()) / myYScale);
      x2 = pRect.left()   + x2;
      y2 = pRect.bottom() - y2;
      if (intersects(pRect, x1, y1, x2, y2)) {
        painter->drawLine(x1, y1, x2, y2);
      }
    }
    
  }

  painter->setClipping(false);
  // drawing markers
  painter->save();
  if (myShowMarkers) {
    for (i = 0; i < item->count(); i++) {
      GraphNode* node = item->getNode(i);
      long x = (long)((node->x() - myOrigin.x()) / myXScale);
      long y = (long)((node->y() - myOrigin.y()) / myYScale);
      x = pRect.left()   + x;
      y = pRect.bottom() - y;
      if (pRect.contains(x, y)) {
        drawMarker(painter, QPoint(x, y), aMarker, aColor);
      }
    }
  }
  else {
    bool fLeft = false, fRight = false;
    QPoint pLeft, pRight;
    for (i = 0; i < item->count(); i++) {
      GraphNode* node = item->getNode(i);
      long x = (long)((node->x() - myOrigin.x()) / myXScale);
      long y = (long)((node->y() - myOrigin.y()) / myYScale);
      x = pRect.left()   + x;
      y = pRect.bottom() - y;
      if (pRect.contains(x, y)) {
        pLeft = QPoint(x, y);
        fLeft = true;
        break;
      }
    }
    for (i = item->count()-1; i >= 0; i--) {
      GraphNode* node = item->getNode(i);
      long x = (long)((node->x() - myOrigin.x()) / myXScale);
      long y = (long)((node->y() - myOrigin.y()) / myYScale);
      x = pRect.left()   + x;
      y = pRect.bottom() - y;
      if (pRect.contains(x, y)) {
        pRight = QPoint(x, y);
        fRight = true;
        break;
      }
    }
    if (fLeft)
      drawMarker(painter, pLeft, aMarker, aColor);
    if (fRight && pLeft != pRight)
      drawMarker(painter, pRight, aMarker, aColor);
  }
  painter->restore();
  // forcing immediate painting
  //painter->flush();
}

#ifdef DEB
//================================================================
// Function : GraphView::drawItem
// Purpose  : draws item
//================================================================
void GraphView::drawItem(QPainter* painter, 
                         int       pxMin,
                         int       pxMax, 
                         int       pyMin, 
                         int       pyMax,
                         double    xMin, 
                         double    xMax, 
                         double    yMin, 
                         double    yMax, 
                         GraphItem* item) {
  // checking item for validity
  if (!item->isValid())
    return;
  // getting marker, type and color of line
  int      aMarker = item->getMarker();
  QColor   aColor  = item->getColor();
  PenStyle aLine   = item->getLine();

  int ml, mr, mt, mb;
  getMargins(ml, mr, mt, mb);
  // getting painting region
  QRect pRect = QRect(QPoint(pxMin+ml, pyMin+mt), QSize(pxMax-pxMin-ml-mr, pyMax-pyMin-mt-mb));
  double XScale = (xMax-xMin)/(pxMax-pxMin);
  double YScale = (yMax-yMin)/(pyMax-pyMin);

  // setting pen for line
  painter->setPen(QPen(aColor, 0, aLine));
  int i;
  if (item->count() > 1) {
    // drawing lines
    painter->setClipRect(pRect);
    painter->setClipping(true);
    for (i = 0; i < item->count() - 1; i++) {
      GraphNode* node1 = item->getNode(i);
      GraphNode* node2 = item->getNode(i + 1);
      long x1 = (long)((node1->x() - xMin) / XScale);
      long y1 = (long)((node1->y() - yMin) / YScale);
      x1 = pRect.left()   + x1;
      y1 = pRect.bottom() - y1;
      long x2 = (long)((node2->x() - xMin) / XScale);
      long y2 = (long)((node2->y() - yMin) / YScale);
      x2 = pRect.left()   + x2;
      y2 = pRect.bottom() - y2;
      if (intersects(pRect, x1, y1, x2, y2)) {
        painter->drawLine(x1, y1, x2, y2);
      }
    }
    painter->setClipping(false);
    // drawing markers
    painter->save();
    if (myShowMarkers) {
      for (i = 0; i < item->count(); i++) {
        GraphNode* node = item->getNode(i);
        long x = (long)((node->x() - xMin) / XScale);
        long y = (long)((node->y() - yMin) / YScale);
        x = pRect.left()   + x;
        y = pRect.bottom() - y;
        if (pRect.contains(x, y)) {
          drawMarker(painter, QPoint(x, y), aMarker, aColor);
        }
      }
    }
    else {
      bool fLeft = false, fRight = false;
      QPoint pLeft, pRight;
      for (i = 0; i < item->count(); i++) {
        GraphNode* node = item->getNode(i);
        long x = (long)((node->x() - xMin) / XScale);
        long y = (long)((node->y() - yMin) / YScale);
        x = pRect.left()   + x;
        y = pRect.bottom() - y;
        if (pRect.contains(x, y)) {
          pLeft = QPoint(x, y);
          fLeft = true;
          break;
        }
      }
      for (i = item->count()-1; i >= 0; i--) {
        GraphNode* node = item->getNode(i);
        long x = (long)((node->x() - xMin) / XScale);
        long y = (long)((node->y() - yMin) / YScale);
        x = pRect.left()   + x;
        y = pRect.bottom() - y;
        if (pRect.contains(x, y)) {
          pRight = QPoint(x, y);
          fRight = true;
          break;
        }
      }
      if (fLeft)
        drawMarker(painter, pLeft, aMarker, aColor);
      if (fRight && pLeft != pRight)
        drawMarker(painter, pRight, aMarker, aColor);
    }
    painter->restore();
  }
  // forcing immediate painting
  //painter->flush();
}
#endif

//================================================================
// Function : GraphView::drawMarker
// Purpose  : draws marker at position
//================================================================
void GraphView::drawMarker(QPainter*     painter, 
                           const QPoint  point, 
                           const int     marker, 
                           const QColor& color) {
  if (marker < 0)
    return;
  // pen
  QPen pen(color, 0, Qt::SolidLine);
  // drawing brush
  QBrush brush(pen.color());
  // marker rect
  int size = getMarkerSize();
  QRect  mRect(QPoint(0,0), QSize(size, size));
  mRect.moveCenter(point);

  painter->setPen(pen);
  painter->setBrush(pen.color());
  switch(marker) {
  case 0:  // 0 - square
    {
     painter->drawRect(mRect);
    }
    break;
  case 1:  // 1 - circle
    {
     painter->drawEllipse(mRect);
    }
    break;
  case 2:  // 2 - triangle (looks up)
    {
      QPolygon ar(3);
      double d = sqrt(3.0);
      double d1 = (double)size / (2 * d);
      double d2 = (double)size * d / 3;
      ar[0] = QPoint(point.x() - size/2, (int)((double)point.y() + d1));
      ar[1] = QPoint(point.x(),          (int)((double)point.y() - d2));
      ar[2] = QPoint(point.x() + size/2, (int)((double)point.y() + d1));
      painter->drawPolygon(ar);
    }
    break;
  case 3:  // 3 - triangle (looks down)
    {
      QPolygon ar(3);
      double d = sqrt(3.0);
      double d1 = (double)size / (2 * d);
      double d2 = (double)size * d / 3;
      ar[0] = QPoint(point.x() - size/2, (int)((double)point.y() - d1));
      ar[1] = QPoint(point.x(),          (int)((double)point.y() + d2));
      ar[2] = QPoint(point.x() + size/2, (int)((double)point.y() - d1));
      painter->drawPolygon(ar);
    }
    break;
  case 4:  // 4 - cross
    {
      painter->drawLine(mRect.left(),  mRect.top(),
                        mRect.right(), mRect.bottom());
      painter->drawLine(mRect.left(),  mRect.bottom(),
                        mRect.right(), mRect.top());
    }
    break;
  case 5:  // 5 - rhomb
    {
      QPolygon ar(4);
      ar[0] = QPoint(point.x(), mRect.top());
      ar[1] = QPoint(mRect.right(), point.y());
      ar[2] = QPoint(point.x(), mRect.bottom());
      ar[3] = QPoint(mRect.left(), point.y());
      painter->drawPolygon(ar);
    }
    break;
  case 6:  // 6 - empty square
    {
      painter->drawLine(mRect.left(),  mRect.top(),
                        mRect.right(), mRect.top());
      painter->drawLine(mRect.right(), mRect.top(),
                        mRect.right(), mRect.bottom());
      painter->drawLine(mRect.right(), mRect.bottom(),
                        mRect.left(),  mRect.bottom());
      painter->drawLine(mRect.left(),  mRect.bottom(),
                        mRect.left(),  mRect.top());
    }
    break;
  case 7:  // 7 - empty circle
    {
      painter->drawArc(mRect, 0, 16 * 360);
    }
    break;
  case 8:  // 8 - empty rhomb
    {
      painter->drawLine(point.x(),     mRect.top(),
                        mRect.right(), point.y());
      painter->drawLine(mRect.right(), point.y(),
                        point.x(),     mRect.bottom());
      painter->drawLine(point.x(),     mRect.bottom(),
                        mRect.left(),  point.y());
      painter->drawLine(mRect.left(),  point.y(),
                        point.x(),     mRect.top());
    }
    break;
  case 9:  // 9 - empty triangle (looks up)
    {
      QPolygon ar(3);
      double d = sqrt(3.0);
      double d1 = (double)size / (2 * d);
      double d2 = (double)size * d / 3;
      painter->drawLine(point.x() - size/2, (int)((double)point.y() + d1), 
                        point.x(),          (int)((double)point.y() - d2));
      painter->drawLine(point.x(),          (int)((double)point.y() - d2), 
                        point.x() + size/2, (int)((double)point.y() + d1));
      painter->drawLine(point.x() + size/2, (int)((double)point.y() + d1), 
                        point.x() - size/2, (int)((double)point.y() + d1));
    }
    break;
  case 10:  // 10 - empty triangle (looks down)
    {
      QPolygon ar(3);
      double d = sqrt(3.0);
      double d1 = (double)size / (2 * d);
      double d2 = (double)size * d / 3;
      painter->drawLine(point.x() + size/2, (int)((double)point.y() - d1), 
                        point.x(),          (int)((double)point.y() + d2));
      painter->drawLine(point.x(),          (int)((double)point.y() + d2), 
                        point.x() - size/2, (int)((double)point.y() - d1));
      painter->drawLine(point.x() - size/2, (int)((double)point.y() - d1), 
                        point.x() + size/2, (int)((double)point.y() - d1));
    }
    break;
  default:
    break;
  }
  // forcing immediate painting
  //painter->flush();
}

//================================================================
// Function : GraphView::getNextMarker
// Purpose  : gets new unique marker for item if possible
//================================================================
void GraphView::getNextMarker(int& typeMarker, int& color, int& typeLine) {
  static int aMarker = -1;
  static int aColor  = -1;
  static int aLine   = -1;

  aMarker = (aMarker + 1) % getNbMarkers();
  aColor  = (aColor  + 1) % getNbColors();
  if (aColor == 0)
    aColor++;
  aLine   = (aLine   + 1) % getNbTypeLines();
  typeMarker = aMarker;
  color      = aColor;
  typeLine   = aLine;
  if (!existMarker(aMarker, aColor, aLine))
    return;
  int i, j, k;
  for (i = 0; i < getNbMarkers(); i++) {
    aMarker = (aMarker + 1) % getNbMarkers();
    for (j = 0; j < getNbColors(); j++) {
      aColor  = (aColor  + 1) % getNbColors();
      if (aColor == 0)
        aColor++;
      for (k = 0; k < getNbTypeLines(); k++) {
        aLine = (aLine + 1) % getNbTypeLines();
        if (!existMarker(aMarker, aColor, aLine)) {
          typeMarker = aMarker;
          color      = aColor;
          typeLine   = aLine;
          return;
        }
      }
    }
  }
}

//================================================================
// Function : GraphView::existMarker
// Purpose  : checks if marker belongs to any enitity
//================================================================
bool GraphView::existMarker(const int typeMarker, const int color, const int typeLine) {
  GraphItem* aItem;
  int aMarker, aColor, aLine;
  QColor aRgbColor;

  foreach (aItem, myItems) {
    if (aItem->isValid()) {
      aItem->getMarker(aMarker, aColor, aLine);
      aRgbColor = aItem->getColor();
      if (aItem->hasOwnColor()) {
/* UNCOMMENT THIS CODE TO ALLOW COMPARE DIFFERENT COLORS
        if (isSameColors(getColor(aColor), aRgbColor))
          return true;
*/
      }
      else {
        if (aMarker == typeMarker &&
            aColor  == color      &&
            aLine   == typeLine)
          return true;
      }
    }
  }
  return false;
}

//================================================================
// Function : GraphView::getColor
// Purpose  : gets color
//================================================================
QColor GraphView::getColor(const int color) {
  if (color >= 0 && color < (int)myColors.count())
    return myColors[color];
  return QColor(0, 0, 0);
}

//================================================================
// Function : GraphView::getLine
// Purpose  : gets line
//================================================================
Qt::PenStyle GraphView::getLine(const int line) {
  if (line >= 0 && line < (int)myLines.count())
    return myLines[line];
  return Qt::NoPen;
}

//================================================================
// Function : intersects
// Purpose  : returns true if line [x1,y1] - [x2,y2] intersects rectangle
//            or lies inside it
//================================================================
bool GraphView::intersects(QRect rect, long x1, long y1, long x2, long y2)
{
  // 1. check for intersecting (boundary rule)
  rect = rect.normalized();
  QRect lineRect(QPoint(x1, y1), QPoint(x2, y2));
  lineRect = lineRect.normalized();
  if (!rect.intersects(lineRect))
    return false;
  // 2. check for hor/vert line
  if (x1 == x2 || y1 == y2)
    return true;
  // 3. check for rectangle corners
  int tlX, tlY, trX, trY, blX, blY, brX, brY;
  // ... top-left
  tlX = (int)((double)(rect.top()  - y1) * (x2 - x1) / (y2 - y1) + x1) - rect.left();
  tlY = (int)((double)(rect.left() - x1) * (y2 - y1) / (x2 - x1) + y1) - rect.top();
  if (tlX == 0 || tlY == 0)
    return true;
  // ... top-right
  trX = (int)((double)(rect.top()   - y1) * (x2 - x1) / (y2 - y1) + x1) - rect.right();
  trY = (int)((double)(rect.right() - x1) * (y2 - y1) / (x2 - x1) + y1) - rect.top();
  if (trX == 0 || trY == 0)
    return true;
  // ... bottom-left
  blX = (int)((double)(rect.bottom() - y1) * (x2 - x1) / (y2 - y1) + x1) - rect.left();
  blY = (int)((double)(rect.left()   - x1) * (y2 - y1) / (x2 - x1) + y1) - rect.bottom();
  if (blX == 0 || blY == 0)
    return true;
  // ... bottom-right
  brX = (int)((double)(rect.bottom() - y1) * (x2 - x1) / (y2 - y1) + x1) - rect.right();
  brY = (int)((double)(rect.right()  - x1) * (y2 - y1) / (x2 - x1) + y1) - rect.bottom();
  if (brX == 0 || brY == 0)
    return true;
  bool btlX = tlX < 0;
  bool btlY = tlY < 0;
  bool btrX = trX < 0;
  bool btrY = trY < 0;
  bool bblX = blX < 0;
  bool bblY = blY < 0;
  bool bbrX = brX < 0;
  bool bbrY = brY < 0;
  return !(btlX == btrX && btlX == bblX && btlX == bbrX &&
           btlY == btrY && btlY == bblY && btlY == bbrY);
}

//================================================================
// Function : GraphView::getTickPoints
// Purpose  : calculates points for grid and/or tick marks
//================================================================
void GraphView::getTickPoints(IntList& xList, IntList& yList) {
  // getting painting region
  QRect pRect = getGraphRect();
  int i;
  // for fixed grid
  if (myGridMode == gmFixed) {
    // calculating X axis ticks
    if (myGridPStepX >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.left() + myGridPStepX * i <= pRect.right()) {
        xList.append(myGridPStepX * i);
        i++;
      }
    }
    // calculating Y axis ticks
    if (myGridPStepY >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.bottom() - myGridPStepY * i >= pRect.top()) {
        yList.append(myGridPStepY * i);
        i++;
      }
    }
  }
  // for floating grid
  else if (myGridMode == gmFloating) {
    // drawing vertical lines
    double xStep = myGridStepX / myXScale;
    if ((int)xStep >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.left() + (int)(xStep * i) <= pRect.right()) {
        xList.append((int)(xStep * i));
        i++;
      }
    }
    else {
      i = 0;
      while(pRect.left() + MIN_GRID_STEP * i <= pRect.right()) {
        xList.append(MIN_GRID_STEP * i);
        i++;
      }
    }
    // drawing horizontal lines
    double yStep = myGridStepY / myYScale;
    if ((int)yStep >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.bottom() - (int)(yStep * i) >= pRect.top()) {
        yList.append((int)(yStep * i));
        i++;
      }
    }
    else {
      i = 0;
      while(pRect.bottom() - MIN_GRID_STEP * i >= pRect.top()) {
        yList.append(MIN_GRID_STEP * i);
        i++;
      }
    }
  }
  // for intervals mode
  else if (myGridMode == gmIntervals) {
    // drawing vertical lines
    double xStep = (double)(pRect.width()) / myGridIntX;
    if ((int)xStep >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.left() + (int)(xStep * i) <= pRect.right()) {
        xList.append((int)(xStep * i));
        i++;
      }
    }
    // drawing horizontal lines
    double yStep = (double)(pRect.height()) / myGridIntY;
    if ((int)yStep >= MIN_GRID_STEP) {
      i = 0;
      while(pRect.bottom() - (int)(yStep * i) >= pRect.top()) {
        yList.append((int)(yStep * i));
        i++;
      }
    }
  }
  if (pRect.left() + xList.last() != pRect.right())
    xList.append(pRect.right() - pRect.left());
  if (pRect.bottom() - yList.last() != pRect.top())
    yList.append(pRect.bottom() - pRect.top());
}

//////////////////////////////////////////////////////////////////////
// GraphNode
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : GraphNode::GraphNode
// Purpose  : default constructor
//================================================================
GraphNode::GraphNode()
         : myX(0), 
           myY(0) {
}

//================================================================
// Function : GraphNode::GraphNode
// Purpose  : constructor
//================================================================
GraphNode::GraphNode(const double x, const double y) 
         : myX(x), 
           myY(y) {
}

//================================================================
// Function : GraphNode::GraphNode
// Purpose  : copy constructor
//================================================================
GraphNode::GraphNode(const GraphNode& node) {
  myX = node.x();
  myY = node.y();
}

//================================================================
// Function : GraphNode::operator==
// Purpose  : operator==
//================================================================
bool GraphNode::operator== (const GraphNode& node) { 
  return x() == node.x();
}

//================================================================
// Function : GraphNode::operator<
// Purpose  : operator<
//================================================================
bool GraphNode::operator< (const GraphNode& node) { 
  return x() < node.x();
}

//================================================================
// Function : GraphNode::operator>
// Purpose  : operator>
//================================================================
bool GraphNode::operator> (const GraphNode& node) { 
  return x() > node.x();
}

//================================================================
// Function : GraphNode::operator=
// Purpose  : operator=
//================================================================
GraphNode& GraphNode::operator= (const GraphNode& node) { 
  myX = node.x(); 
  myY = node.y(); 
  return *this; 
}

//////////////////////////////////////////////////////////////////////
// GraphItem
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : GraphItem::GraphItem
// Purpose  : constructor
//================================================================
GraphItem::GraphItem(GraphView* parent, QString name) 
         : myParent(parent) {
  init();
  mySorted = false;
  setName(name);
  if (myParent)
    myParent->insertItem(this);
}

//================================================================
// Function : GraphItem::~GraphItem
// Purpose  : destructor
//================================================================
GraphItem::~GraphItem() {
  qDeleteAll(myNodes);
}

//================================================================
// Function : GraphItem::addNode
// Purpose  : adds node
//================================================================
void GraphItem::addNode(GraphNode* node) {
  unsigned i;
  if (mySorted) {
    for (i = 0; i < myNodes.count(); i++) {
      if (*(myNodes.at(i)) > *node) {
        myNodes.insert(i, node);
        return;
      }
    }
  }
  myNodes.append(node);
}

//================================================================
// Function : GraphItem::addNode
// Purpose  : adds node
//================================================================
void GraphItem::addNode(const double x, const double y) {
  addNode(new GraphNode(x, y));
}

//================================================================
// Function : GraphItem::addNodes
// Purpose  : adds nodes
//================================================================
void GraphItem::addNodes(NodeList& nodes) {
  for (unsigned i = 0; i < nodes.count(); i++)
    addNode(nodes.at(i));
}

//================================================================
// Function : GraphItem::removeNode
// Purpose  : removes node (and deletes it)
//================================================================
void GraphItem::removeNode(GraphNode* node) {
  myNodes.removeAll(node);
}

//================================================================
// Function : GraphItem::removeNode
// Purpose  : removes node by index (and deletes it)
//================================================================
void GraphItem::removeNode(int index) {
  removeNode(myNodes.at(index));
}

//================================================================
// Function : GraphItem::clear
// Purpose  : clears list of nodes
//================================================================
void GraphItem::clear() {
  myNodes.clear();
}

//================================================================
// Function : GraphItem::isValid
// Purpose  : returns true if item is valid, 
//            i.e. has valid attributes and not empty
//================================================================
bool GraphItem::isValid() {
  if (isEmpty())
    return false;
  if (myMarker < 0)
    return false;
  if (!myHasOwnLine && myLine < 0)
    return false;
  if (myHasOwnColor  && !myOwnColor.isValid() || 
      !myHasOwnColor && myColor < 0)
    return false;
  return true;
}

//================================================================
// Function : GraphItem::setSorted
// Purpose  : sets/clears <Sorted> flag (and reorders nodes)
//================================================================
void GraphItem::setSorted(bool sorted) {
  if (sorted != mySorted) {
    mySorted = sorted;
    if (mySorted) {
      NodeList oldList(myNodes);
      myNodes.clear();
      for (unsigned i = 0; i < oldList.count(); i++)
        addNode(oldList.at(i));
    }
  }
}

//================================================================
// Function : GraphItem::getNode
// Purpose  : gets node by index
//================================================================
GraphNode* GraphItem::getNode(const int index) {
  return myNodes.at(index);
}

//================================================================
// Function : GraphItem::operator[]
// Purpose  : gets node by index
//================================================================
GraphNode* GraphItem::operator[](const int index) {
  return myNodes.at(index);
}

//================================================================
// Function : GraphItem::setMarker
// Purpose  : sets dynamic marker for item
//================================================================
void GraphItem::setMarker(const int marker, 
                          const int color, 
                          const int line) {
  myMarker       = marker;
  myHasOwnMarker = false;
  myColor        = color;
  myHasOwnColor  = false;
  myLine         = line;
  myHasOwnLine   = false;
}

//================================================================
// Function : GraphItem::setMarker
// Purpose  : sets static marker for item
//            (with own (NOT CHANGEABLE) marker, color and type line)
//================================================================
void GraphItem::setMarker(const int          marker, 
                          const QColor&      color, 
                          const Qt::PenStyle line) {
  myMarker       = marker;
  myHasOwnMarker = true;
  myOwnColor     = color;
  myHasOwnColor  = true;
  myOwnLine      = line;
  myHasOwnLine   = true;
}

//================================================================
// Function : GraphItem::setColor
// Purpose  : sets own color
//================================================================
void GraphItem::setColor(const QColor& color) {
  myOwnColor    = color;
  myHasOwnColor = true;
}

//================================================================
// Function : GraphItem::getColor
// Purpose  : returns item's color
//================================================================
QColor GraphItem::getColor() {
  if (myHasOwnColor)
    return myOwnColor;
  else 
  if (myParent)
    return myParent->getColor(myColor);
  return QColor();
}

//================================================================
// Function : GraphItem::setLine
// Purpose  : sets own line
//================================================================
void GraphItem::setLine(const Qt::PenStyle line) {
  myOwnLine    = line;
  myHasOwnLine = true;
}

//================================================================
// Function : GraphItem::getLine
// Purpose  : returns item's line type
//================================================================
Qt::PenStyle GraphItem::getLine() {
  if (myHasOwnLine)
    return myOwnLine;
  else
  if (myParent)
    return myParent->getLine(myLine);
  return Qt::SolidLine;
}

//================================================================
// Function : GraphItem::setmarker
// Purpose  : sets own marker
//================================================================
void GraphItem::setMarker(const int marker) {
  myMarker       = marker;
  myHasOwnMarker = true;
}

//================================================================
// Function : GraphItem::getMarker
// Purpose  : return item's marker type
//================================================================
int GraphItem::getMarker() {
  return myMarker;
}

//================================================================
// Function : GraphItem::getMarker
// Purpose  : gets marker atributes
//================================================================
void GraphItem::getMarker(int& marker, int& color, int& line) {
  marker = myMarker;
  color  = myColor;
  line   = myLine;
}

//================================================================
// Function : GraphItem::init
// Purpose  : performs initialization
//================================================================
void GraphItem::init() {
  // setting invalid marker
  setMarker(-1, -1, -1);
  myHasOwnMarker = false;
  myOwnColor     = QColor(255, 255, 255);
  myHasOwnColor  = false;
  myOwnLine      = Qt::SolidLine;
  myHasOwnLine   = false;
}

//////////////////////////////////////////////////////////////////////
// GraphLegend
//////////////////////////////////////////////////////////////////////
#include <qlabel.h>

#define LEGEND_ITEM_SIZE  100

//================================================================
// Function : GraphLegend::GraphLegend
// Purpose  : constructor
//================================================================
GraphLegend::GraphLegend(QWidget* parent, GraphView* graph) 
           : QWidget(parent), 
             myGraph(graph) {
  myGraph->setLegend(this);
  setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  setMinimumSize(1, 1);

  setTitle("LEGEND");
  myShowTitle = true;
  
  updateMe();
} 

//================================================================
// Function : GraphLegend::drawContents
// Purpose  : draws contents
//================================================================
void GraphLegend::paintEvent(QPaintEvent* e)
{
  QPainter painter(this);
  QColor bgcolor = myGraph->palette().color(myGraph->backgroundRole());
  painter.fillRect(e->rect(), bgcolor);
  QFontMetrics fm(font());
  int aMSize = myGraph->getMarkerSize();
  int aTH = MAX(fm.height(), aMSize);
  int i;
  int aNb = myGraph->getNbItems();
  int aFromX;
  int aFromY = SPACING_SIZE;
  painter.setPen(palette().color(QPalette::Text));//QPalette::Active, 
  if (myShowTitle && !myTitle.isEmpty()) {
    QFontMetrics fmt(myTitleFont);
    int aTWT = fmt.width(myTitle);
    int aTHT = fmt.height();
    aFromX = (width() - aTWT) / 2;
    painter.setFont(myTitleFont);
    painter.drawText(aFromX,
                      aFromY, 
                      aTWT, 
                      aTHT, 
                      Qt::AlignCenter, 
                      myTitle);
    aFromY += (aTHT + SPACING_SIZE);
  }
  aFromX = SPACING_SIZE * 2 + MAX(LEGEND_ITEM_SIZE, aMSize);
  painter.setFont(font());
  for (i = 0; i < aNb; i++) {
    GraphItem* item = myGraph->getItem(i);
    QColor       color  = item->getColor();
    Qt::PenStyle line   = item->getLine();
    int      marker = item->getMarker();
    painter.setPen(QPen(color, 0, line));
    painter.drawLine(SPACING_SIZE, 
                      (SPACING_SIZE + aTH ) * i + aFromY + aTH/2,
                      SPACING_SIZE + LEGEND_ITEM_SIZE, 
                      (SPACING_SIZE + aTH ) * i + aFromY + aTH/2);
    myGraph->drawMarker(&painter, 
                        QPoint(SPACING_SIZE + LEGEND_ITEM_SIZE/2, 
                               (SPACING_SIZE + aTH ) * i + aFromY + aTH/2), 
                        marker, 
                        color);
    QString aName = item->getName();
    painter.setPen(palette().color(QPalette::Text)); //QPalette::Active, 
    if (aName.isEmpty())
      aName = QString("[ Item ") + QString::number(i+1) + QString(" ]");
    int aTW = fm.width(aName);
    painter.drawText(aFromX, 
                      (SPACING_SIZE + aTH ) * i + aFromY, 
                      aTW, 
                      aTH, 
                      Qt::AlignLeft | Qt::AlignVCenter, 
                      aName);
  }
  //painter.flush();
}

//================================================================
// Function : GraphLegend::updateMe
// Purpose  : updates legend contents
//================================================================
void GraphLegend::updateMe() {
  setPalette(myGraph->palette());
  //setFont(myGraph->font());
  QFont aFont = font();
  QFontMetrics fm(aFont);
  int i;
  int aNb = myGraph->getNbItems();
  int xSize = 0;
  int ySize = (MAX(fm.height(), myGraph->getMarkerSize()) + SPACING_SIZE) * aNb + SPACING_SIZE;
  for (i = 0; i < aNb; i++) {
    QString aName = myGraph->getItem(i)->getName();
    if (aName.isEmpty())
      aName = QString("[ Item ") + QString::number(i+1) + QString(" ]");
    int aTW = fm.width(aName);
    if (aTW > xSize)
      xSize = aTW;
  }
  xSize += SPACING_SIZE * 3 + MAX(LEGEND_ITEM_SIZE, myGraph->getMarkerSize());
  if (myShowTitle && !myTitle.isEmpty()) {
    QFontMetrics fmt(myTitleFont);
    int aTWT = fmt.width(myTitle);
    int aTHT = fmt.height();
    xSize = MAX(xSize, (aTWT + SPACING_SIZE * 2));
    ySize += (aTHT + SPACING_SIZE);
  }
  resize(xSize, ySize);
  update();
}

//================================================================
// Function : GraphLegend::showTitle
// Purpose  : shows/hides legend title
//============================= ===================================
void GraphLegend::showTitle(bool show) {
  myShowTitle = show;
  updateMe();
}

//================================================================
// Function : GraphLegend::setTitle
// Purpose  : sets legend title 
//================================================================
void GraphLegend::setTitle(const QString& title) {
  myTitle = title;
  updateMe();
}

//================================================================
// Function : GraphLegend::tip
// Purpose  : handles tooltips for the view
//================================================================
QRect GraphLegend::tip(const QPoint& point, QString& tipText) {
  QPoint cPoint   = point;//= viewportToContents(point);
  GraphItem* item = itemAt(cPoint);
  if (!item)
    return QRect(0, 0, -1, -1);
  QRect tRect = textRect(item, tipText);
  QPoint tl = tRect.topLeft();//contentsToViewport(tRect.topLeft());
  QPoint br = tRect.bottomRight();//contentsToViewport(tRect.bottomRight());
  if (tl.x() < 0 ||
      tl.y() < 0 ||
      br.x() > width() ||
      br.y() > height()) {
    return QRect(0, tl.y(), width(), tRect.height());
  }
  return QRect(0, 0, -1, -1);
}

//================================================================
// Function : GraphLegend::itemAt
// Purpose  : returns graph item which is below the point or NULL
//================================================================
GraphItem* GraphLegend::itemAt(const QPoint& point) {
  QFont aFont = font();
  QFontMetrics fm(aFont);
  int aMSize = myGraph->getMarkerSize();
  int aTH = MAX(fm.height(), myGraph->getMarkerSize());
  int i;
  int aFromY = SPACING_SIZE;
  if (myShowTitle && !myTitle.isEmpty()) {
    QFontMetrics fmt(myTitleFont);
    int aTHT = fmt.height();
    aFromY += (aTHT + SPACING_SIZE);
  }
  if (point.y() <= aFromY)
    return 0;
  for (i = 0; i < myGraph->getNbItems(); i++) {
    GraphItem* item = myGraph->getItem(i);
    if (point.y() < aFromY + aTH)
      return item;
    aFromY += (aTH + SPACING_SIZE);
  }
  return 0;
}
 
//================================================================
// Function : GraphLegend::textRect
// Purpose  : returns rect which contains item's name
//================================================================
QRect GraphLegend::textRect(GraphItem* item, QString& tipText) {
  QString aName = item->getName();
  int index = myGraph->findItem(item);
  if (aName.isEmpty()) {
    if (index < 0)
      return QRect(0, 0, -1, -1);
    aName = QString("[ Item ") + QString::number(index + 1) + QString(" ]");
  }
  tipText = aName;
  QFont aFont = font();
  QFontMetrics fm(aFont);
  int aTH = MAX(fm.height(), myGraph->getMarkerSize());
  int aTW = fm.width(aName);
  int aFromY = SPACING_SIZE;
  if (myShowTitle && !myTitle.isEmpty()) {
    QFontMetrics fmt(myTitleFont);
    int aTHT = fmt.height();
    aFromY += (aTHT + SPACING_SIZE);
  }
  aFromY += (aTH + SPACING_SIZE) * index;
  return QRect(SPACING_SIZE * 2 + MAX(LEGEND_ITEM_SIZE, myGraph->getMarkerSize()), 
               aFromY, 
               aTW, 
               aTH);
}

//================================================================
// Function : GraphLegend::setTitleFont
// Purpose  : sets legend title font
//================================================================
void GraphLegend::setTitleFont(QFont& font) {
  myTitleFont = font;
  update(); //repaint(false);
}

//================================================================
// Function : GraphLegend::setItemFont
// Purpose  : sets item font
//================================================================
void GraphLegend::setItemFont(QFont& font) {
  setFont(font);
  update(); //repaint(false);
}

//================================================================
// Function : GraphLegend::event
// Purpose  : global event handler
//================================================================
bool GraphLegend::event(QEvent* e)
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
    QString tipText;
    QRect r = tip(helpEvent->pos(), tipText);
    if (r.isValid() && !tipText.isEmpty() )
      QToolTip::showText(helpEvent->globalPos(), tipText, this, r);
    else
      QToolTip::hideText();
  }
  return QWidget::event(e);
}

//================================================================
// Function : GraphLegend::minimumSizeHint
// Purpose  : minimum size
//================================================================
QSize GraphLegend::minimumSizeHint() const
{
  return QSize(100, 100);
}

//================================================================
// Function : GraphLegend::sizeHint
// Purpose  : size hint
//================================================================
QSize GraphLegend::sizeHint() const
{
  return minimumSizeHint();
}

//////////////////////////////////////////////////////////////////////
// GraphSplitView
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : GraphSplitView::GraphSplitView
// Purpose  : constructor
//================================================================
GraphSplitView::GraphSplitView(QWidget* parent) 
: QWidget(parent), myGraph(0)
{
  // creating window layout
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setMargin(0);

  mySplitter = new QSplitter(Qt::Horizontal, this);
  aLayout->addWidget(mySplitter);

  myGraph = new GraphView(mySplitter);
  GraphLegend* legend = new GraphLegend(mySplitter, myGraph);
  
  mySplitter->setStretchFactor(0, 5);
}

//================================================================
// Function : GraphSplitView::getLegend
// Purpose  : returns legend view
//================================================================
GraphLegend* GraphSplitView::getLegend()
{
  return myGraph ? myGraph->getLegend() : 0;
}

//================================================================
// Function : GraphSplitView::showLegend
// Purpose  : shows/hides legend view
//================================================================
void GraphSplitView::showLegend(bool show)
{
  getLegend()->setVisible(show);
}

//================================================================
// Function : GraphSplitView::isLegendShown
// Purpose  : returns true if legend is being shown
//================================================================
bool GraphSplitView::isLegendShown()
{
  return getLegend()->isVisible();
}
