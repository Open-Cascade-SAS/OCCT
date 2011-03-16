#ifndef MDIWINDOW_H
#define MDIWINDOW_H

#include <qmainwindow.h>
#include <V3d_View.hxx>
#include <V3d_TypeOfView.hxx>

#include "ViewOperations.h"

class Document;
class View;
class GraphSplitView;

class MDIWindow : public QMainWindow
{
  Q_OBJECT

friend class Document;

public:

  enum ResultType
  {
    DisplayNonOpt, DisplayOpt, UpdateNonOpt, UpdateOpt, Undefined
  };

public:
  MDIWindow( Document* aDocument, QWidget* parent, V3d_TypeOfView );
  ~MDIWindow();

  Document*           getDocument();
  int                 getDisplayMode();
  void                defineMagView();
  Handle_V3d_View&    getView();
  void                activateAction( ViewOperations::Action action );
  void                addResult(ResultType, int, double);
    
  bool                eventFilter(QObject*, QEvent*);
  void                graphPopup(const QPoint&);

signals:
  void                message( const QString&, int );
	void                sendCloseView( MDIWindow* theView );

public slots:
  void                closeEvent( QCloseEvent* e );
  void                setCursor( const QCursor& );
  void                onGraphClear();
  void                onGraphFitAll();

protected:
  void                createViewActions();
  void                createGraphActions();

private:
  Document*           myDocument;
  View*               myView;
  GraphSplitView*     myGraphView;
  QMenu*              myGraphPopup;
  ViewOperations*     myOperations;
};

#endif
