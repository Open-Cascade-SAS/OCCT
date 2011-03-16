#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h> 

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Drawer.hxx>

#include "ObjectDlg.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjectDlg::ObjectDlg( QWidget* parent, Handle(V3d_View)& view, 
                      Handle(AIS_InteractiveContext)& cxt )
: QDialog( parent ),
  myAutoApply( true ), myView( view ), myContext( cxt )
{
    setModal( false );
    setWindowTitle( tr( "TITLE_OBJECT_DLG" ) );

    QVBoxLayout* topLayout = new QVBoxLayout( this );
    topLayout->setMargin( 5 );

    QFrame* mainFrame = new QFrame( this );
    mainFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( mainFrame );
    topLayout->addSpacing( 10 );

    // view properties
    QGridLayout* pLayout = new QGridLayout( mainFrame );
    pLayout->setMargin( 5 );
    myDegModelLabel = new QLabel( tr("MEN_DLG_DEGMODEL"), mainFrame );
    myDegRatioLabel = new QLabel( tr("MEN_DLG_DEGRATIO"), mainFrame );

    myDegModelBox = new QComboBox( mainFrame );
    myDegModelBox->addItem( tr( "MEN_DLG_NODEGENERATION" ) );
    myDegModelBox->addItem( tr( "MEN_DLG_TINYDEGENERATION" ) );
    myDegModelBox->addItem( tr( "MEN_DLG_WIREDEGENERATION" ) );
    myDegModelBox->addItem( tr( "MEN_DLG_MARKERDEGENERATION" ) );
    myDegModelBox->addItem( tr( "MEN_DLG_BBOXDEGENERATION" ) );
    myCurrentDegModel = NoDegId;
    myCurrentDegRatio = 0.;
    verify( connect( myDegModelBox, SIGNAL( activated( int ) ), SLOT( onDegenerateModel( int ) ) ) );

    //QSpinBox* degRatioSpin = new QSpinBox( mainFrame );
//QSpinBox ( QWidget * parent = 0, const char * name = 0 ) 
    myDegRatioSpin = new QSpinBox( mainFrame );
    myDegRatioSpin->setRange(0, 10);
    myDegRatioSpin->setSingleStep(1);
    verify( connect( myDegRatioSpin, SIGNAL( valueChanged( int ) ), SLOT( onDegRatio( int ) ) ) );
    
    QCheckBox* showEdges = new QCheckBox( mainFrame );
    showEdges->setText( tr( "BTN_SHOWEDGES" ) );
    verify( connect( showEdges, SIGNAL( toggled( bool ) ), SLOT( onShowEdges( bool ) ) ) );
    
    myEdgeStyle = new QLabel( tr("MEN_DLG_EDGESTYLE"), mainFrame );
    myEdgeWidth = new QLabel( tr("MEN_DLG_EDGEWIDTH"), mainFrame );
    
    myEdgeBox = new QComboBox( mainFrame );
    myEdgeBox->addItem( tr( "MEN_DLG_EDGESOLID" ) );
    myEdgeBox->addItem( tr( "MEN_DLG_EDGEDASH" ) );
    myEdgeBox->addItem( tr( "MEN_DLG_EDGEDOT" ) );
    myEdgeBox->addItem( tr( "MEN_DLG_EDGEDOTDASH" ) );
    myCurrentEdgeStyle = SolidEdgeId;
    verify( connect( myEdgeBox, SIGNAL( activated( int ) ), SLOT( onEdgeStyle( int ) ) ) );

    //myEdgeSpin = new QSpinBox( mainFrame );
    myEdgeSpin = new QSpinBox( mainFrame );
    myEdgeSpin->setRange(1, 10);
    myEdgeSpin->setSingleStep(10);
    verify( connect( myEdgeSpin, SIGNAL( valueChanged( int ) ), SLOT( onEdgeWidth( int ) ) ) );

    onShowEdges( false );

    pLayout->addWidget( myDegModelLabel, 0, 0 );
    pLayout->addWidget( myDegRatioLabel, 1, 0 );
    pLayout->addWidget( myDegModelBox, 0, 2 );
    pLayout->addWidget( myDegRatioSpin, 1, 2 );
    pLayout->addWidget( showEdges, 2, 0 );
    pLayout->addWidget( myEdgeStyle, 3, 0 );
    pLayout->addWidget( myEdgeWidth, 4, 0 );
    pLayout->addWidget( myEdgeBox, 3, 2 );
    pLayout->addWidget( myEdgeSpin, 4, 2 );
    pLayout->setRowStretch( 5, 10 );
    pLayout->setColumnStretch( 1, 10 );

    // apply controls
    QCheckBox *autoApply;
    autoApply = new QCheckBox( this );
    autoApply->setText( tr( "BTN_AUTOAPPLY" ) );
    autoApply->setChecked( true );
    verify( connect( autoApply, SIGNAL( toggled( bool ) ), SLOT( onAutoApply( bool ) ) ) );

    QHBoxLayout* applyLayout = new QHBoxLayout();
    applyLayout->addWidget( autoApply );
    applyLayout->addStretch( 10 );
    topLayout->addLayout(applyLayout);
    topLayout->addSpacing( 10 );

    // control buttons
    QPushButton *bOk, *bCancel/*, *bHelp*/;
    bOk     = new QPushButton( tr( "BTN_OK" ), this );
    verify( connect( bOk, SIGNAL( clicked() ), SLOT( onOk() ) ) );
    bCancel = new QPushButton( tr( "BTN_CANCEL" ), this );
    verify( connect( bCancel, SIGNAL( clicked() ), SLOT( onCancel() ) ) );
//    bHelp   = new QPushButton( tr( "BTN_HELP" ), this );
//    verify( connect( bHelp, SIGNAL( clicked() ), SLOT( onHelp() ) ) );

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing( 5 );
    btnLayout->addStretch( 10 );
	btnLayout->addWidget( bOk );
    btnLayout->addWidget( bCancel );
    btnLayout->addStretch( 10 );
 //    btnLayout->addWidget( bHelp );
    topLayout->addLayout(btnLayout);

    QSize s = topLayout->totalMinimumSize();
    s.setWidth( s.width() + topLayout->margin() + 10 );
    s.setHeight( s.height() + topLayout->margin() + 10 );
    setFixedSize( s );
}

ObjectDlg::~ObjectDlg()
{
}

void ObjectDlg::showEvent ( QShowEvent* e )
{
    QWidget::showEvent( e );

    Standard_Boolean isDeg, isAnim, isEnabled;
    isAnim = myView->AnimationMode( isDeg );
    isEnabled = isDeg && isAnim;
    myDegModelLabel->setEnabled( isEnabled );
    myDegRatioLabel->setEnabled( isEnabled );
    myDegModelBox->setEnabled( isEnabled );
    myDegRatioSpin->setEnabled( isEnabled );
}

void ObjectDlg::onOk()
{
    hide();
}
 
void ObjectDlg::onCancel()
{
    hide();
}

void ObjectDlg::onHelp()
{
}

void ObjectDlg::onShowEdges( bool on )
{
    if ( myAutoApply ) {
        myEdgeStyle->setEnabled( on );
        myEdgeWidth->setEnabled( on );
        myEdgeBox->setEnabled( on );
        myEdgeSpin->setEnabled( on );

        Handle(AIS_InteractiveObject) object;
        Handle(Prs3d_ShadingAspect) aspect;
        myContext->InitSelected();
        for( ; myContext->MoreSelected(); myContext->NextSelected() ) {
            object = myContext->SelectedInteractive();
            aspect = object->Attributes()->ShadingAspect();
	        if ( on ) 
                aspect->Aspect()->SetEdgeOn();
            else	
                aspect->Aspect()->SetEdgeOff();
            object->SetAspect( aspect );
        }
        //myContext->UpdateCurrentViewer();
        myView->Update();
    }
}

void ObjectDlg::onDegenerateModel( int index )
{
    if ( myCurrentDegModel != index ) {

        switch ( index ) {
            case NoDegId :
                myDegModel = Aspect_TDM_NONE;
                break; 
            case TinyDegId :
                myDegModel = Aspect_TDM_TINY;
                break;
            case WireDegId : 
                myDegModel = Aspect_TDM_WIREFRAME;
                break;
            case MarkerDegId :
                myDegModel = Aspect_TDM_MARKER;
                break;
            case BBoxDegId :
                myDegModel = Aspect_TDM_BBOX;
                break;
        }
        myCurrentDegModel = index;
        updateDegenerationModel();
    }
}

void ObjectDlg::onDegRatio( int ratio )
{
    myCurrentDegRatio = ratio / 10.;
    updateDegenerationModel(); 
}

void ObjectDlg::onEdgeStyle( int index )
{
    if ( myCurrentEdgeStyle != index ) {
        myCurrentEdgeStyle = index;
        updateEdgeAspect( true );
    }
}

void ObjectDlg::onEdgeWidth( int value )
{
    myCurrentEdgeWidth = (float) value;
    updateEdgeAspect( false );
}

void ObjectDlg::onAutoApply( bool on )
{
    myAutoApply = on;
}

void ObjectDlg::updateEdgeAspect( bool edgeStyle )
{
    Aspect_TypeOfLine type;
    if ( edgeStyle ) 
        switch( myCurrentEdgeStyle ) {
            case SolidEdgeId :
                type = Aspect_TOL_SOLID;
                break;
            case DashEdgeId :
                type = Aspect_TOL_DASH;
                break;
            case DotEdgeId :
                type = Aspect_TOL_DOT;
                break;
            case DotDashEdgeId :
                type = Aspect_TOL_DOTDASH;
                break;
        }
    Handle(AIS_InteractiveObject) object;
    Handle(Prs3d_ShadingAspect) aspect;
    myContext->InitSelected();
    for( ; myContext->MoreSelected(); myContext->NextSelected() ) {
        object = myContext->SelectedInteractive();
        aspect = object->Attributes()->ShadingAspect();
        if ( edgeStyle )
            aspect->Aspect()->SetEdgeLineType( type );
        else
            aspect->Aspect()->SetEdgeWidth( myCurrentEdgeWidth );
        object->SetAspect( aspect );
    }
    if ( myAutoApply )
            //myContext->UpdateCurrentViewer();
            myView->Viewer()->Update();
}

void ObjectDlg::updateDegenerationModel()
{
    //myView->SetAnimationModeOff();
    Handle(AIS_InteractiveObject) object;
    for( myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected() ) {
        object = myContext->SelectedInteractive();
        object->SetDegenerateModel( myDegModel, myCurrentDegRatio );
    }
    //myView->SetAnimationModeOn();
  
    if ( myAutoApply )
        //myView->Update();
        myContext->UpdateCurrentViewer();
}
