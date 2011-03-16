#ifndef OBJECTDLG_H
#define OBJECTDLG_H

#include <qdialog.h>

#include <Aspect_TypeOfDegenerateModel.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class QLabel;
class QComboBox;
class QSpinBox;
class ObjectDlg : public QDialog
{
    Q_OBJECT

        enum { SolidEdgeId, DashEdgeId, DotEdgeId, DotDashEdgeId };

        enum { NoDegId, TinyDegId, WireDegId, MarkerDegId, BBoxDegId };

public:
	ObjectDlg( QWidget* parent, Handle(V3d_View)&, Handle(AIS_InteractiveContext)& );
	virtual ~ObjectDlg();

private slots:
    void        onOk();
    void        onCancel();
    void        onHelp();
    void        onShowEdges( bool );
    void        onDegenerateModel( int );
    void        onDegRatio( int );
    void        onEdgeStyle( int );
    void        onEdgeWidth( int );
    void        onAutoApply( bool );

protected:
    void        showEvent ( QShowEvent* );

private:
    void        updateEdgeAspect( bool edgeStyle ); 
                /* if edgeStyle - true then type of line will be changed;
                   width of line will be changed otherwise
                */
    void        updateDegenerationModel();

private:
    /* ----- common section ----- */
    bool                            myAutoApply;
    Handle_V3d_View                 myView;
    Handle_AIS_InteractiveContext   myContext;
    /* ----- edge section ----- */
    QLabel*                         myEdgeStyle;
    QLabel*                         myEdgeWidth;
    QComboBox*                      myEdgeBox;
    QSpinBox*                       myEdgeSpin;
    int                             myCurrentEdgeStyle;
    float                           myCurrentEdgeWidth;
    /* ----- degeneration section ----- */
    Aspect_TypeOfDegenerateModel    myDegModel;
    int                             myCurrentDegModel;
    float                           myCurrentDegRatio;
    QLabel*                         myDegModelLabel;
    QLabel*                         myDegRatioLabel;
    QComboBox*                      myDegModelBox;
    QSpinBox*                       myDegRatioSpin;
};

#endif
