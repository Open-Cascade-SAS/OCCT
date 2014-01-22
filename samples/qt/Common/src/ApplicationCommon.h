#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMdiArea>
#include <QList>


class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
	enum { FileNewId, FilePrefUseVBOId, FileCloseId, FilePreferencesId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId };
  enum { ToolWireframeId, ToolShadingId, ToolColorId, ToolMaterialId, ToolTransparencyId, ToolDeleteId };

  ApplicationCommonWindow();
  ~ApplicationCommonWindow();

	static QMdiArea*              getWorkspace();
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
  void                            onCloseWindow();
  void                            onUseVBO();
	virtual void                    onCloseDocument( DocumentCommon* theDoc );
  virtual void                    onSelectionChanged();
  virtual void                    onAbout();
  void                            onViewToolBar();
	void                            onViewStatusBar();
  void                            onToolAction();
	void                            onCreateNewView();
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
  QList<QAction*>                 myMaterialActions;
  //QList<DocumentCommon*>          myDocuments;

	QToolBar*                       myStdToolBar;
	QToolBar*                       myCasCadeBar;
	QMenu*                          myFilePopup;
	QMenu*                          myWindowPopup;
  QAction*                        myFileSeparator;

protected:
  QList<DocumentCommon*>          myDocuments;
};

#endif


