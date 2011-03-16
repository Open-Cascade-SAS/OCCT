#ifndef VIEWOPERATIONS_H
#define VIEWOPERATIONS_H

#include <qobject.h>
#include <qlist.h>
#include <qaction.h>

#include <AIS_InteractiveContext.hxx>

class ViewOperations : public QObject
{
	Q_OBJECT

public:
  enum ViewActions { /*MagViewId, AxoViewId, PerViewId,*/ ToolWireframeId, ToolShadingId };

  enum Action { ViewFitAllId, ViewFitAreaId, ViewZoomId, ViewPanId, 
	  		        ViewGlobalPanId, ViewFrontId, ViewBackId, 
			          ViewTopId, ViewBottomId, ViewLeftId, ViewRightId,ViewAxoId, 
			          ViewRotationId, ViewResetId };

	ViewOperations( QObject * parent=0 ) ;
	ViewOperations( Handle(AIS_InteractiveContext) theContext, QObject * parent=0 ) ;
	~ViewOperations();

	QList<QAction*>     getViewActions();
	QList<QAction*>     getToolActions();
	void                initActions();
	int                 getDisplayMode();

signals:
  void                setCursor( const QCursor& );

private slots:
  void                onActionActivated();

public slots:
	void                onWireframe();
	void                onShading();
	void                onToolAction();
	void                updateToggled( bool );
	void                onNoActiveActions();

private:
  void                initCursors();
	void                initViewActions();
	void                initToolActions();

private:
	Handle_AIS_InteractiveContext   myContext;
	QList<QAction*>                 myViewActions;
  QList<QAction*>                 myToolActions;
};

#endif
