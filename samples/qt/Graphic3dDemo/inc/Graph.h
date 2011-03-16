// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TEST_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TEST_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef GRAPH_EXPORTS
#define GRAPH_API __declspec(dllexport)
#else
#define GRAPH_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////
// File     : Graph.h
// Created  : 26.03.02
// Author   : Vadim SANDLER
//////////////////////////////////////////////////////////////////////

#if !defined(_Graph_h)
#define _Graph_h

#include <qwidget.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlist.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qcolor.h>
#include <qrubberband.h>

//================================================================
// Class        : GraphNode
// Description  : graph node
//================================================================
class GRAPH_API GraphNode {
public:
// default constructor
  GraphNode();
// constructor
  GraphNode(const double x, const double y);
// copy constructor
  GraphNode(const GraphNode& node);

// x coordinate
  inline double x() const { return myX; }
// y coordinate
  inline double y() const { return myY; }
// operator ==
  bool          operator== (const GraphNode& node);
// operator <
  bool          operator<  (const GraphNode& node);
// operator >
  bool          operator>  (const GraphNode& node);
// operator =
  GraphNode&    operator=  (const GraphNode& node);

private:
// node coordinates
  double        myX;
  double        myY;
};

typedef QList<GraphNode*> NodeList;
class   GraphView;

//================================================================
// Class        : GraphItem
// Description  : graph data set
//================================================================
class GRAPH_API GraphItem {
public:
// constructor
  GraphItem(GraphView* parent, QString name = QString::null);
// destructor
  virtual ~GraphItem();

// adds node
  void          addNode (GraphNode* node);
// adds node
  void          addNode (const double x, const double y);
// adds nodes
  void          addNodes(NodeList& nodes);
// removes node (and deletes it)
  void          removeNode(GraphNode* node);
// removes node by index (and deletes it)
  void          removeNode(int index);
// clears nodes
  void          clear ();
// gets number of nodes
  int           count ()   { return myNodes.count(); }
// returns true if list of nodes is empty
  bool          isEmpty () { return myNodes.isEmpty(); }
// returns true if item is valid, i.e. has valid attributes and not empty
  bool          isValid ();
// returns <Sorted> flag, true by default
  bool          isSorted() { return mySorted; }
// sets/clears <Sorted> flag (and reorders nodes)
  void          setSorted (bool sorted);
// gets node by index or NULL if index is out of range
  GraphNode*    getNode (const int index);
// gets node by index or NULL if index is out of range
  GraphNode*    operator[] (const int index);
// sets dynamic marker for item
  void          setMarker (const int marker, 
                           const int color, 
                           const int line);
// sets static marker for item 
// (with own (NOT CHANGEABLE) marker, color and type line)
  void          setMarker (const int      marker, 
                           const QColor&  color, 
                           const Qt::PenStyle line);
// gets item name
  QString       getName() const { return myName; }
// sets item name
  void          setName(const QString& name) { myName = name; }
// returns true if item has own color
  bool          hasOwnColor() const  { return myHasOwnColor; }
// returns true if item has own line type
  bool          hasOwnLine() const   { return myHasOwnLine; }
// returns true if item has own marker
  bool          hasOwnMarker() const { return myHasOwnMarker; }
// sets own color
  void          setColor(const QColor& color);
// returns item's color
  QColor        getColor();
// sets own line
  void          setLine(const Qt::PenStyle line);
// returns item's line type
  Qt::PenStyle  getLine();
// sets own marker 
  void          setMarker(const int marker);
// return item's marker type
  int           getMarker();
// gets marker attributes
  void          getMarker (int& marker, int& color, int& line);

protected:
// performs initialization
  void          init();

private:
// parent graph viewer
  GraphView*    myParent;
// node list  
  NodeList      myNodes;
// name of item
  QString       myName;
// attributes type of line, color and marker
  int           myMarker;
  int           myColor;
  int           myLine;
// own marker
  bool          myHasOwnMarker;
// own color
  bool          myHasOwnColor;
  QColor        myOwnColor;
// own line type
  bool          myHasOwnLine;
  Qt::PenStyle  myOwnLine;
// <Sort coordinates> flag
  bool          mySorted;
};

typedef QList<GraphItem*>   ItemList;
typedef QList<QColor>       ColorList;
typedef QList<Qt::PenStyle> LineList;
typedef QList<int>          IntList;

class GraphLegend;

//================================================================
// Class        : GraphView
// Description  : simple graph displayer
//================================================================
class GRAPH_API GraphView : public QWidget {
  struct OperationButton {
    Qt::MouseButtons      button;
    Qt::KeyboardModifiers modifier;
    OperationButton(const Qt::MouseButtons      btn = Qt::NoButton,
                    const Qt::KeyboardModifiers m   = Qt::NoModifier)
      : button(btn), modifier(m) {}
  };

public:
  enum ViewOperation { voNone, voZoom, voZoomX, voZoomY, 
                       voPan,  voPanX, voPanY,  voRect };
  enum GridMode      { gmFixed, gmIntervals, gmFloating };

#ifdef DEB
  void          drawItem(QPainter* painter, 
                         int       pxMin,
                         int       pxMax, 
                         int       pyMin, 
                         int       pyMax,
                         double    xMin, 
                         double    xMax, 
                         double    yMin, 
                         double    yMax, 
                         GraphItem* item);
#endif

/* ================== CONSTRUCTION/DESTRUCTION ================ */
// Constructor
  GraphView(QWidget* parent = 0);
// Destructor
  ~GraphView();

/* ======================= OVERRIDED ========================== */
// event filter 
  bool            eventFilter(QObject* object, QEvent* event);

/* ================== CURSORS HANDLING ======================== */
// sets cursor for certain view operation
  void            setOperationCursor (const ViewOperation operation, 
                                      const QCursor&      newCursor);
// gets cursor for certain view operation
  QCursor         getOperationCursor (const ViewOperation operation) const;

/* =================== VIEW OPERATIONS ======================== */
// returns global <Enable operations> flag state
  bool            isOperationsEnabled() const { return myEnableGlobal; }
// globally enables/disables operations
  void            setOperationsEnabled(bool on);
// returns 'true' if view operation is enabled 
  bool            isOperationEnabled (const ViewOperation operation) const;
// enables/disables view operation 
  void            setOperationEnabled(const ViewOperation operation, 
                                      bool                enable);
// sets operation key-mousebutton combination
  void            setOperationButton (const ViewOperation         operation, 
                                      const Qt::MouseButtons      button,
                                      const Qt::KeyboardModifiers modifier);
// gets operation key-mousebutton combination
  void            getOperationButton (const ViewOperation    operation,
                                      Qt::MouseButtons&      button,
                                      Qt::KeyboardModifiers& modifier) const;
// tests for operation key-mousebutton combination
  bool            testOperation      (const ViewOperation theOp, 
                                      const Qt::MouseButtons      button,
                                      const Qt::KeyboardModifiers modifier) const;
// tests key-mousebutton combination and returns view operation if any matches
  ViewOperation   testOperation      (const Qt::MouseButtons      button,
                                      const Qt::KeyboardModifiers modifier) const;

  // resets view : sets origin to 0.0 and scales to 1.0
  void            reset();
// fits viewer so display all data  
  void            fitAll();
// fits viewer to display certain data
  void            fitData(const double xMin, const double xMax,
                          const double yMin, const double yMax);
// fits viewer to display certain data along horizontal axis
  void            fitDataX(const double xMin, const double xMax);
// fits viewer to display certain data along vertical axis
  void            fitDataY(const double yMin, const double yMax);
// performs pan operation by dx, dy pixels
  void            pan(const int dx, const int dy);
// performs zoom operation by dx, dy pixels
  void            zoom(const int dx, const int dy);
// starts view operation if it is enabled
  void            startViewOperation(const ViewOperation operation);

/* ================== ATRIBUTES HANDLING ====================== */
// enables/disables internal markers drawing
  void            showMarkers     (bool show);
// enables/disables tooltips
  void            showTooltips    (bool show);
// returns true is axes are shown
  bool            isAxesShown     () const { return myShowAxes; }
// shows/hides axes
  void            showAxes        (bool show);
// returns true is tick-marks are shown
  bool            isTickMarkShown () const { return myShowTickMarks; }
// shows/hides tick-marks
  void            showTickMark     (bool show);
// returns true is title is shown
  bool            isTitleShown    () const { return myShowTitle; }
// shows/hides title
  void            showTitle       (bool show);
// gets graph title
  const QString&  getTitle        () const { return myTitle; }
// sets graph title
  void            setTitle        (const QString& title);
// returns true is axes titles are shown
  bool            isAxesTitleShown() const { return myShowAxesTitle; }
// shows/hides axis titles
  void            showAxesTitle   (bool show);
// gets graph X axis title
  const QString&  getXTitle       () const { return myXTitle; }
// sets graph X axis title
  void            setXTitle       (const QString& title);
// gets graph Y axis title
  const QString&  getYTitle       () const { return myYTitle; }
// sets graph Y axis title
  void            setYTitle       (const QString& title);
// returns true is grid is shown
  bool            isGridShown     () const { return myShowGrid; }
// shows/hides grid
  void            showGrid        (bool show);
// sets grid step (FLOATING mode) for x, y axes, 
// doesn't change step if it is <= 0
  void            setGridStep (const double x, const double y, bool turn = true);
// gets grid step (FLOATING mode) for x, y axes 
// and returns true if grid mode is FLOATING
  bool            getGridStep (double& x, double& y) const;
// sets grid step (FIXED mode) for x, y axes in pixels, 
// doesn't change step if it is less <= 0
  void            setFixedGridStep (const int x, const int y, bool turn = true);
// gets grid step (FIXED mode) for x, y axes in pixels 
// and returns true if grid mode is FIXED
  bool            getFixedGridStep (int& x, int& y) const;
// sets number of grid intervals (INTERVALS mode) for x, y axes, 
// doesn't change if <= 0
  void            setGridIntervals (const int xInt, 
                                    const int yInt, 
                                    bool      turn = true);
// gets number of grid intervals (INTERVALS mode) for x, y axes, 
// and returns true if grid mode is INTERVALS
  bool            getGridIntervals (int &xInt, int &yInt) const;
// returns grid mode [FIXED, INTERVALS, FLOATING]
  GridMode        getGridMode() const { return myGridMode; }
// switches grid mode [FIXED, INTERVALS, FLOATING]
  void            setGridMode(const GridMode mode);
// sets X axis scale 
  void            setXScale(const double scale);
// gets X axis scale (data per pixel along X axis)
  double          getXScale() const { return myXScale; }
// sets Y axis scale
  void            setYScale(const double scale);
// gets Y axis scale (data per pixel along Y axis)
  double          getYScale() const { return myYScale; }
// sets X,Y axis scale
  void            setScale(const double scale);
// moves axes origin to point [x, y]
  void            setOrigin(const double x, const double y);
// gets axes origin
  void            getOrigin(double& x, double& y) const;
// changes backgroundColor, use it instead setBackgroundColor
  void            changeBackgroundColor(const int index = -1);
// gets main title font
  QFont           getTitleFont() { return myTitleFont; }
// sets main title font
  void            setTitleFont(QFont& font);
// gets axes title font
  QFont           getAxesTitleFont() { return myAxesTitleFont; }
// sets axes title font
  void            setAxesTitleFont(QFont& font);
// gets tick marks font
  QFont           getTickMarksFont() { return myTickMarksFont; }
// sets tick marks font
  void            setTickMarksFont(QFont& font);
// sets legend widget
  void            setLegend(GraphLegend* legend);
// gets legend widget
  GraphLegend*    getLegend() { return myLegend; }
// gets margins size
  void            getMargins(int& leftMargin, 
                             int& rightMargin, 
                             int& topMargin, 
                             int& bottomMargin);

/* ================== DATA HANDLING =========================== */
// inserts item into list
  void            insertItem(GraphItem* item);
// removes item from list
  void            removeItem(const int index);
// clears data list
  void            clear();
// gets number of items
  int             getNbItems() { return myItems.count(); }
// gets item by index, returns NULL if index is out of range
  GraphItem*      getItem(const int index);
// returns index of item in data list 
  int             findItem(GraphItem* item);
// sets/unsets items to be sorted by horizontal coordinate
  void            setSorted(bool sorted);
// gets full data range
  void            getRange(double& minX, double& minY, 
                           double& maxX, double& maxY);
// gets current data range
  void            getCurrentRange(double& minX, double& minY, 
                                  double& maxX, double& maxY);
// returns number of colors provided
  virtual int     getNbColors();
// returns number of markers provided
  virtual int     getNbMarkers();
// returns number of type lines provided
  virtual int     getNbTypeLines();
// gets marker size (default is 9 pixels)
  int             getMarkerSize() { return myMarkerSize; } 
// sets marker size (for good look it should be odd)
  void            setMarkerSize(const int size); 
// gets color by index
  virtual QColor  getColor(const int color);
// gets line
  virtual Qt::PenStyle getLine(const int line);
// returns true if colors seem to be the same (difference is less than gap)
  static bool     isSameColors(const QColor color1, 
                               const QColor color2, 
                               const int    gap = 10);

protected:
/* ================== VIEW OPERATIONS HANDLING ================ */
// starts/finishes view operation
  void            startOperation (ViewOperation theOp);
// called on view operation starting
  virtual void    onStartOperation  ();
// called on view operation finishing
  virtual void    onFinishOperation ();
// called on view operation running
  virtual void    onOperation (QPoint mousePos);
// called on mouse moving action (e.g. for highlighting item)
  virtual void    onMouseMove (QPoint mousePos);
// gets sensibility area size
  int             getSensibilitySize() const { return mySensibilitySize; }
// sets sensibility area size
  void            setSensibilitySize(const int size);
// handles tooltips for the view
  QRect           tip(const QPoint& point, QString& tipText);

/* ================== EVENTS HANDLING ========================= */
// MousePress event handler
  void            mousePressEvent (QMouseEvent* e);
// MouseMove event handler
  void            mouseMoveEvent (QMouseEvent* e);
// MouseRelease event handler
  void            mouseReleaseEvent (QMouseEvent* e);
// MouseDoubleClick event handler
  void            mouseDoubleClickEvent (QMouseEvent *e);
// KeyPress event handler
  void            keyPressEvent (QKeyEvent *e);
// KeyRelease event handler
  void            keyReleaseEvent (QKeyEvent *e);
// Resize event handler
  void            resizeEvent (QResizeEvent* e);
// Paint event handler
  void            paintEvent (QPaintEvent* e);
// global event handler
  bool            event (QEvent* e);

/* ================== DRAWING FUNCTIONS ======================= */
// gets graph painting area
  virtual QRect   getGraphRect ();
// draws grid
  virtual void    drawGrid (QPainter* painter);
// draws border (it is drawn when grid is not)
  virtual void    drawBorder (QPainter* painter);
// draws axes
  virtual void    drawAxes (QPainter* painter);
// draws tick marks
  virtual void    drawTickMarks(QPainter* painter);
// draws axes titles
  virtual void    drawAxesTitle(QPainter* painter);
// draws graph title
  virtual void    drawTitle(QPainter* painter);
// draws item
  virtual void    drawItem (QPainter* painter, GraphItem* item);
// draws marker at position
  virtual void    drawMarker(QPainter*     painter, 
                             const QPoint  point, 
                             const int     marker, 
                             const QColor& color);
// gets new unique marker for item if possible
  virtual void    getNextMarker(int& typeMarker, int& color, int& typeLine);
// checks if marker belongs to some entity
  bool            existMarker(const int typeMarker, 
                              const int color, 
                              const int typeLine);

private:
// returns true if line [x1,y1] - [x2,y2] intersect rectangle
  static bool     intersects(QRect rect, long x1, long y1, long x2, long y2);
// calculates points for grid and/or tick marks
  void            getTickPoints(IntList& xList, IntList& yList);

private:
// enable view operations flags
  bool            myEnableGlobal;
  bool            myEnableRect;
  bool            myEnablePan;
  bool            myEnablePanX;
  bool            myEnablePanY;
  bool            myEnableZoom;
  bool            myEnableZoomX;
  bool            myEnableZoomY;
// current view operation
  ViewOperation   myOperation;
// current view operation
  ViewOperation   myForcedOp;
// sensibility size for rect selection operation
  int             mySensibilitySize;
// view operation points (second is used for rubber rectangle)
  QPoint          myPoint;
  QPoint          myOtherPoint;
// view operations key-mouse combinations
  OperationButton myRectKey;
  OperationButton myPanKey;
  OperationButton myPanXKey;
  OperationButton myPanYKey;
  OperationButton myZoomKey;
  OperationButton myZoomXKey;
  OperationButton myZoomYKey;
// state cursors
  QCursor         myDefCursor;
  QCursor         myRectCursor;
  QCursor         myPanCursor;
  QCursor         myPanXCursor;
  QCursor         myPanYCursor;
  QCursor         myZoomCursor;
  QCursor         myZoomXCursor;
  QCursor         myZoomYCursor;
// axes origin (left-bottom point)
  GraphNode       myOrigin;
// X axis scale
  double          myXScale;
  double          myYScale;
// draw flags
  bool            myShowAxes;
  bool            myShowTitle;
  bool            myShowAxesTitle;
  bool            myShowTickMarks;
  bool            myShowGrid;
  bool            myShowTooltips;
  bool            myShowMarkers;
// grid mode
  GridMode        myGridMode;
// fixed grid size
  int             myGridPStepX;
  int             myGridPStepY;
// floating grid size
  double          myGridStepX;
  double          myGridStepY;
// number of grid intervals
  int             myGridIntX;
  int             myGridIntY;
// marker size
  int             myMarkerSize;
// titles
  QString         myTitle;
  QString         myXTitle;
  QString         myYTitle;
// colors
  ColorList       myColors;
// line types
  LineList        myLines;
// data
  ItemList        myItems;
  GraphLegend*    myLegend;
// fonts
  QFont           myTitleFont;
  QFont           myAxesTitleFont;
  QFont           myTickMarksFont;
// rubber band
  QRubberBand*    myRubberBand;
  
  friend class GraphLegend;
};

//================================================================
// Class        : GraphLegend
// Description  : Legend widget
//================================================================
class GRAPH_API GraphLegend : public QWidget {
public:
// constructor
  GraphLegend(QWidget* parent, GraphView* graph);

// updates legend contents
  void            updateMe();
// shows/hides legend title
  void            showTitle(bool show);
// sets legend title
  void            setTitle(const QString& title);
// gets legend title
  QString         getTitle() const { return myTitle; }
// returns graph item which is below the point or NULL
  GraphItem*      itemAt(const QPoint& point);
// gets legend title font
  QFont           getTitleFont() { return myTitleFont; }
// sets legend title font
  void            setTitleFont(QFont& font);
// gets item font
  QFont           getItemFont() { return font(); }
// sets item font
  void            setItemFont(QFont& font);
// get minimum size
  QSize           minimumSizeHint() const;
  QSize           sizeHint() const;

protected:
// handles tooltips for the view
  QRect           tip(const QPoint& point, QString& tipText);
// returns rect which contains item's name
  QRect           textRect(GraphItem* item, QString& tipText);
// draws contents
  void            paintEvent(QPaintEvent* e);
// global event handler
  bool            event(QEvent* e);

private:
// parent graph
  GraphView*      myGraph;
  bool            myShowTitle;
  QString         myTitle;
  QFont           myTitleFont;
};

//================================================================
// Class        : GraphSplitView
// Description  : Graph view with a legend in splitted window
//================================================================
class GRAPH_API GraphSplitView : public QWidget {
public:
// constructor
  GraphSplitView(QWidget* parent);

// returns graph view
  GraphView*      getGraph()  { return myGraph; }
// returns legend view
  GraphLegend*    getLegend();
// shows/hides legend view
  void            showLegend(bool show);
// returns true if legend is being shown
  bool            isLegendShown();

private:
// graph view
  GraphView*      myGraph;
// splitter window
  QSplitter*      mySplitter;
};

#endif // !defined(_Graph_h)