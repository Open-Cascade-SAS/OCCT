#ifndef VIEW_H
#define VIEW_H

#include <Standard_WarningsDisable.hxx>
#include <QWidget>
#include <QAction>
#include <QList>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

class TopoDS_Shape;
class QRubberBand;

//class COMMONSAMPLE_EXPORT View: public QWidget
class View: public QWidget, protected AIS_ViewController
{
    Q_OBJECT
protected:
    enum CurrentAction3d { CurAction3d_Nothing, CurAction3d_DynamicZooming,
                           CurAction3d_WindowZooming, CurAction3d_DynamicPanning,
                           CurAction3d_GlobalPanning, CurAction3d_DynamicRotation };

public:
    enum ViewAction { ViewFitAllId, ViewFitAreaId, ViewZoomId, ViewPanId, ViewGlobalPanId,
                      ViewFrontId, ViewBackId, ViewTopId, ViewBottomId, ViewLeftId, ViewRightId,
                      ViewAxoId, ViewRotationId, ViewResetId, ViewHlrOffId, ViewHlrOnId };
    enum RaytraceAction { ToolRaytracingId, ToolShadowsId, ToolReflectionsId, ToolAntialiasingId };

    View( Handle(AIS_InteractiveContext) theContext, QWidget* parent );

    ~View();

    virtual void                  init();
    bool                          dump( Standard_CString theFile );
    QList<QAction*>*              getViewActions();
    QList<QAction*>*              getRaytraceActions();
    void                          noActiveActions();
    bool                          isShadingMode();

    void                          EnableRaytracing();
    void                          DisableRaytracing();

    void                          SetRaytracedShadows (bool theState);
    void                          SetRaytracedReflections (bool theState);
    void                          SetRaytracedAntialiasing (bool theState);

    bool                          IsRaytracingMode() const { return myIsRaytracing; }
    bool                          IsShadowsEnabled() const { return myIsShadowsEnabled; }
    bool                          IsReflectionsEnabled() const { return myIsReflectionsEnabled; }
    bool                          IsAntialiasingEnabled() const { return myIsAntialiasingEnabled; }

    static QString                GetMessages( int type,TopAbs_ShapeEnum aSubShapeType,
                                               TopAbs_ShapeEnum aShapeType );
    static QString                GetShapeType( TopAbs_ShapeEnum aShapeType );

    Standard_EXPORT static void   OnButtonuseraction( int ExerciceSTEP,
                                                      Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void   DoSelection( int Id,
                                               Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void   OnSetSelectionMode( Handle(AIS_InteractiveContext)&,
                                                      Standard_Integer&,
                                                      TopAbs_ShapeEnum& SelectionMode,
                                                      Standard_Boolean& );
    virtual QPaintEngine*         paintEngine() const;
signals:
    void                          selectionChanged();

public slots:
    void                          fitAll();
    void                          fitArea();
    void                          zoom();
    void                          pan();
    void                          globalPan();
    void                          front();
    void                          back();
    void                          top();
    void                          bottom();
    void                          left();
    void                          right();
    void                          axo();
    void                          rotation();
    void                          reset();
    void                          hlrOn();
    void                          hlrOff();
    void                          updateToggled( bool );
    void                          onBackground();
    void                          onEnvironmentMap();
    void                          onRaytraceAction();

protected:
    virtual void                  paintEvent( QPaintEvent* );
    virtual void                  resizeEvent( QResizeEvent* );
    virtual void                  mousePressEvent( QMouseEvent* );
    virtual void                  mouseReleaseEvent(QMouseEvent* );
    virtual void                  mouseMoveEvent( QMouseEvent* );
    virtual void                  wheelEvent (QWheelEvent* );

    virtual void                  addItemInPopup( QMenu* );

    Handle(V3d_View)&                     getView();
    Handle(AIS_InteractiveContext)&       getContext();
    void                                  activateCursor( const CurrentAction3d );
    void                                  Popup( const int x, const int y );
    CurrentAction3d                       getCurrentMode();
    void                                  updateView();

    //! Setup mouse gestures.
    void defineMouseGestures();

    //! Set current action.
    void setCurrentAction (CurrentAction3d theAction)
    {
      myCurrentMode = theAction;
      defineMouseGestures();
    }

    //! Handle selection changed event.
    void OnSelectionChanged (const Handle(AIS_InteractiveContext)& theCtx,
                             const Handle(V3d_View)& theView) Standard_OVERRIDE;

private:
    void                          initCursors();
    void                          initViewActions();
    void                          initRaytraceActions();
private:
    bool                            myIsRaytracing;
    bool                            myIsShadowsEnabled;
    bool                            myIsReflectionsEnabled;
    bool                            myIsAntialiasingEnabled;

    Handle(V3d_View)                myView;
    Handle(AIS_InteractiveContext)  myContext;
    AIS_MouseGestureMap             myDefaultGestures;
    Graphic3d_Vec2i                 myClickPos;
    CurrentAction3d                 myCurrentMode;
    Standard_Real                   myCurZoom;
    QList<QAction*>*                myViewActions;
    QList<QAction*>*                myRaytraceActions;
    QMenu*                          myBackMenu;
};

#endif


