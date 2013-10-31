#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QWorkspace>
#include <QList>


class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
	enum { FileNewId, FilePrefUseVBOId, FileCloseId, FilePreferencesId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId };
  enum { ToolWireframeId, ToolShadingId, ToolColorId, ToolMaterialId, ToolTransparencyId, ToolDeleteId };
  enum { ToolShadowsId, ToolReflectionsId, ToolAntialiasingId };

  ApplicationCommonWindow();
  ~ApplicationCommonWindow();

	static QWorkspace*              getWorkspace();
	static ApplicationCommonWindow* getApplication();
	static QString                  getResourceDir();

	virtual void                     updateFileActions();
  QList<QAction*>*                 getToolActions();
  QList<QAction*>*                 getMaterialActions();
	
protected:
  virtual DocumentCommon*          createNewDocument();
  int&                             getNbDocument();

public slots:
	
  DocumentCommon*                 onNewDoc();
  DocumentCommon*                 onNewDocRT();
  void                            onCloseWindow();
  void                            onUseVBO();
	virtual void                    onCloseDocument( DocumentCommon* theDoc );
  virtual void                    onSelectionChanged();
  virtual void                    onAbout();
  void                            onViewToolBar();
	void                            onViewStatusBar();
  void                            onToolAction();
#ifdef HAVE_OPENCL
  void                            onRaytraceAction();
#endif
	void                            onCreateNewView();
#ifdef HAVE_OPENCL
  void                            onCreateNewViewRT();
#endif
  void                            onWindowActivated ( QWidget * w );
	void                            windowsMenuAboutToShow();
  void                            windowsMenuActivated( bool checked/*int id*/ );
	void                            onSetMaterial( int theMaterial );

protected:
  virtual void                    resizeEvent( QResizeEvent* );
  bool                            isDocument();
  QMenu*                          getFilePopup();
  QAction*                        getFileSeparator();
  QToolBar*                       getCasCadeBar();

private:
	void                            createStandardOperations();
	void                            createCasCadeOperations();
	void                            createWindowPopup();

private:
  int                             myNbDocuments;
	bool                            myIsDocuments;

	QList<QAction*>                 myStdActions;
  QList<QAction*>                 myToolActions;
  QList<QAction*>                 myRaytraceActions;
  QList<QAction*>                 myMaterialActions;
  QList<DocumentCommon*>          myDocuments;

	QToolBar*                       myStdToolBar;
	QToolBar*                       myCasCadeBar;
  QToolBar*                       myRaytraceBar;
	QMenu*                          myFilePopup;
	QMenu*                          myWindowPopup;
  QAction*                        myFileSeparator;
};

#endif


