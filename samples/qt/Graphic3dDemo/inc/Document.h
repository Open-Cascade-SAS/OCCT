#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <qobject.h>
#include <qlist.h>

#include "MDIWindow.h"
#include "ViewOperations.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_TypeOfView.hxx>

class Application;
class Document : public QObject
{
	Q_OBJECT
public:
	Document( int theIndex, Application* app );
	~Document();

	Application*                    getApplication();
	Handle_AIS_InteractiveContext&  getContext();

	void                            removeView( MDIWindow* theView );
	int                             countOfWindow();
  void                            createMagView( int, int, int, int );
	void                            createNewView( V3d_TypeOfView type = V3d_ORTHOGRAPHIC );

signals:
	void                            sendCloseDocument( Document* );
public slots:
	void                            onCloseView( MDIWindow* theView );
	void                            onWireframe();
	void                            onShading();

private:
	Handle_V3d_Viewer               Viewer( const Standard_CString aDisplay,
                                          const Standard_ExtString aName,
		                                      const Standard_CString aDomain,
                                          const Standard_Real ViewSize );
private:
	Application*                    myApp;
	QList<MDIWindow*>               myViews;
	Handle_V3d_Viewer               myViewer;
	Handle_AIS_InteractiveContext   myContext;
	ViewOperations*                 myOperations;
	int                             myIndex;
  int                             myNbViews;
};

#endif
