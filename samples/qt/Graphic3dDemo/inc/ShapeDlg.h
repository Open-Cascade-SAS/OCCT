#ifndef SHAPEDLG_H
#define SHAPEDLG_H

#include <qdialog.h>

#include <Aspect_TypeOfDegenerateModel.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
//#include <GUIA_Sphere.hxx>
#include <Sphere_Sphere.hxx>

class QLabel;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class ShapeDlg : public QDialog  
{
    Q_OBJECT

        enum { SolidEdgeId, DashEdgeId, DotEdgeId, DotDashEdgeId };

        enum { NoDegId, TinyDegId, WireDegId, MarkerDegId, BBoxDegId };

public:
	ShapeDlg( QWidget* parent, Handle(V3d_View)&, Handle(AIS_InteractiveContext)& );
	virtual ~ShapeDlg();

private slots:
    void        onOk();
    void        onCancel();
    void        onHelp();
    void        onRadiusChanged( double );
    void        onItemsChanged( int );
    void        onPanesChanged( int );
    void        onDeflectionChanged( double );
	void		onText();
	void		onPositionChanged();
protected:
    void        closeEvent ( QCloseEvent* );
    void        showEvent ( QShowEvent* );

private:
    void        updateSphere();

private:
    /* ----- common section ----- */
    Handle_V3d_View                 myView;
    Handle_AIS_InteractiveContext   myContext;
    Handle_Sphere_Sphere            myShape;
    double                          myRadius;
    double                          myDeflection;
    int                             myNbPanes;
    int                             myNbItems;
    QDoubleSpinBox*                 myDefSpin;
    QSpinBox*                       myPanesSpin;
    QSpinBox*                       myItemsSpin;
    QDoubleSpinBox*                 myXSpin;
    QDoubleSpinBox*                 myYSpin;
    QDoubleSpinBox*                 myZSpin;
    QCheckBox*                      myVNormal;
    QCheckBox*                      myVColor;
    QCheckBox*                      myVTexel;
    QCheckBox*                      myText;
};

#endif
