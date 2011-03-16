#ifndef APPLICATION_H
#define APPLICATION_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qmenu.h>
#include <qworkspace.h>
#include <qlist.h>

#include "MDIWindow.h"
#include "ViewDlg.h"

class Document;
class OutputWindow;
class QTextEdit;

class Application: public QMainWindow
{
  Q_OBJECT

  enum { FileNewId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId, OptimId,
         AutoTestId, StopId, TileId, CascadeId };
	
  enum { FileImportBREPId};
	
public:
  Application();
  ~Application();

	static QWorkspace*      getWorkspace();
	static Application*     getApplication();
	static QString          getResourceDir();
  static void             startTimer();
  static void             stopTimer( int, const char*, bool addToGraph = true, int aOpt = -1 );
  static void             showTimer( const char* );
  static void             addResult(MDIWindow::ResultType, double);
  void                    showMessage( QString& );

  MDIWindow*              getActiveMDI();

	void                    updateActions();
  void                    importBREP();
signals:
	void                    sendQuit();

public slots:
	void InitApp();
  Document*               onNewDocument();
	void                    onCloseDocument( Document* theDoc );
	void                    onViewToolBar();
	void                    onViewStatusBar();
	void                    onAbout();
	void                    onQuit();
	void                    onTranslate();
	void                    onToggleOptim(bool);
	void                    onAutoTest();
	void                    onStop();
	void                    updateWindowList();
	void                    activateWindow();
	void			              onEditViewProperties();
	void                    updateViewDlg();

protected:
  QWorkspace*             workspace() const;
  void                    keyPressEvent( QKeyEvent* e );

private slots:
  void                    onWindowActivated( QWidget* );

private:
	void                    createActions();
	void                    createCCActions();
	void                    createIEPopups();

private:
  QWorkspace*             myWorkSpace;
  MDIWindow*              myActiveMDI;
  int                     myNbDocuments;
  bool                    myIsDocuments;
  QList<QAction*>         myStdActions;
  QList<QAction*>         myCCActions;
  QToolBar*               myStdToolBar;
  QMenu*                  myFilePopup;
  QMenu*                  myImportPopup;
  QMenu*                  myToolsMenu;
  QMenu*                  myWinMenu;
  OutputWindow*           myOutput;
  
  bool                    myEscPressed;
  bool					          myStopPressed;
  
  ViewDlg*                myViewDlg;

};

class OutputWindow : public QWidget
{
public:
  OutputWindow( QWidget* parent = 0 );
  ~OutputWindow();

  void          print( QString& );

private:
  QTextEdit*    myLineEdit;
  int           myLineCounter;
};

#endif
