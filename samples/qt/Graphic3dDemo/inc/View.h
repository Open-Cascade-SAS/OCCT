#ifndef VIEW_H
#define VIEW_H

#include <qwidget.h>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>

#include "MDIWindow.h"
#include "Document.h"
#include "ViewOperations.h"
#include "ViewDlg.h"
#include "ObjectDlg.h"
#include "ShapeDlg.h"

class QRubberBand;

class View: public QWidget
{
	Q_OBJECT
	enum CurrentAction3d { CurAction3d_Nothing,
		                     CurAction3d_DynamicZooming,
		                     CurAction3d_WindowZooming,
                         CurAction3d_DynamicPanning,
                         CurAction3d_GlobalPanning,
                         CurAction3d_DynamicRotation,
                         CurAction3d_MagnifyView
  };

public:
  View( Handle(AIS_InteractiveContext), QWidget* parent, MDIWindow* );
  ~View();
	
  void               setCurrentAction();
  Handle_V3d_View&   getView();
  void               startAction( ViewOperations::Action );

signals:
	void        selectionChanged();
	void        noActiveActions();
	void        ViewInitialized();

public slots:
	void        onBackground();
	void        paintEvent( QPaintEvent * e);
	void        resizeEvent( QResizeEvent* e);

private slots:
  void        onImportObject();
  void        onEditObjectProperties();
  void        onCreateShape();
  void        onLoadBox();
  void        onLoadSphere();
  void        onLoadTorus();
  void        onLoadLine();
  void        onRemoveObject();
  void        onWireframeMode();
  void        onShadingMode();
  void        onDisplayTrihedron();
  void        onEraseTrihedron();


protected:
	void        mousePressEvent ( QMouseEvent* e);
	void        mouseReleaseEvent ( QMouseEvent* e);
	void        mouseMoveEvent ( QMouseEvent* e);
  void        init();


private:
  void        onLButtonDown( Qt::KeyboardModifiers nFlags, const QPoint point );
  void        onRButtonDown( Qt::KeyboardModifiers nFlags, const QPoint point );
  void        onLButtonUp( Qt::KeyboardModifiers nFlags, const QPoint point );
  void        onRButtonUp( Qt::KeyboardModifiers nFlags, const QPoint point );
  void        onMouseMove( Qt::MouseButtons btns, Qt::KeyboardModifiers nFlags, const QPoint point );
  void        DragEvent( int x, int y, int TheState );
  void        InputEvent( int x, int y );  
  void        MoveEvent( int x, int y ); 
  void        MultiMoveEvent( int x, int y ); 
  void        MultiDragEvent( int x, int y, int TheState ); 
  void        MultiInputEvent( int x, int y ); 
  void        Popup(); 
  void        DrawRectangle ( int MinX, int MinY, int MaxX, int MaxY, bool Draw );

private:
  MDIWindow*                      myMDI;
  QRubberBand*                    myRubberBand;
  ObjectDlg*                      myObjDlg;
  ShapeDlg*                       myShapeDlg;
	Standard_Integer                myFirst;
	Handle_V3d_View                 myView;
	Handle_AIS_InteractiveContext   myContext;
  Handle_AIS_Trihedron            myTrihedron;
	CurrentAction3d                 myCurrentMode;
  Standard_Integer                myXmin;
  Standard_Integer                myYmin;  
  Standard_Integer                myXmax;
  Standard_Integer                myYmax;
  Quantity_Factor                 myCurZoom;
  Standard_Boolean                myComputedModeIsOn;
  Standard_Integer                myCycleCounter;
};

#endif
