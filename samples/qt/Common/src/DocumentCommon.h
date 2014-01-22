#ifndef DOCUMENTCOMMON_H
#define DOCUMENTCOMMON_H

#include "MDIWindow.h"
//#include "IESample.h"

#include <QObject>
#include <QList>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class ApplicationCommonWindow;

class COMMONSAMPLE_EXPORT DocumentCommon : public QObject
{
	Q_OBJECT

public:
	DocumentCommon( const int, ApplicationCommonWindow* );
	~DocumentCommon();

	ApplicationCommonWindow*       getApplication();
	Handle(AIS_InteractiveContext) getContext();
	void                           removeView( MDIWindow* );
  void                           removeViews();
	int                            countOfWindow();
	void                           fitAll();
	
protected:
  virtual MDIWindow*                   createNewMDIWindow();

signals:
  void                           selectionChanged();
	void                           sendCloseDocument( DocumentCommon* );

public slots:
	virtual void                   onCloseView( MDIWindow* );
	virtual void                   onCreateNewView();
	virtual void                   onMaterial();
  virtual void                   onMaterial( int );
	virtual void                   onDelete();

	void                           onWireframe();
	void                           onShading();
	void                           onColor();
	void                           onTransparency();
	void                           onTransparency( int );

private:
  Handle(V3d_Viewer)             Viewer( const Standard_CString aDisplay,
			                               const Standard_ExtString aName,
			                               const Standard_CString aDomain,
			                               const Standard_Real ViewSize,
			                               const V3d_TypeOfOrientation ViewProj,
			                               const Standard_Boolean ComputedMode,
			                               const Standard_Boolean aDefaultComputedMode );

protected:
	ApplicationCommonWindow*       myApp;
	QList<MDIWindow*>              myViews;
	Handle(V3d_Viewer)             myViewer;
	Handle(AIS_InteractiveContext) myContext;
	int                            myIndex;
  int                            myNbViews;
};

#endif


