#include <qapplication.h>
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
#include <Graphic3d_ArrayOfPrimitives.hxx>

#include <Sphere_Sphere.hxx>
#include <gp_Pnt.hxx>

#include "ShapeDlg.h"
#include "Application.h"
#include "MDIWindow.h"
#include "Document.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ShapeDlg::ShapeDlg( QWidget* parent, Handle(V3d_View)& view, 
                    Handle(AIS_InteractiveContext)& cxt )
: QDialog( parent ),
  myView( view ), myContext( cxt )
{
    setModal( false );
    setWindowTitle( tr( "TITLE_SHAPE_DLG" ) );

    myRadius = 100;
    myDeflection = 1.0;
    myNbPanes = 0;
    myNbItems = 0;

    QVBoxLayout* topLayout = new QVBoxLayout( this );
    topLayout->setMargin( 5 );

    // radius frame
    QFrame* rFrame = new QFrame( this );
    //rFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( rFrame );
    topLayout->addSpacing( 10 );

    QHBoxLayout* rBox = new QHBoxLayout( rFrame );
    rBox->setMargin( 5 );
    QLabel* rad = new QLabel( tr( "MEN_DLG_RADIUS" ), rFrame );
    QDoubleSpinBox* rSpin = new QDoubleSpinBox( rFrame  );
    rSpin->setRange( 1, 10000 );
    rSpin->setSingleStep( 1 );
//    rSpin->setValueDouble( myRadius );
    rSpin->setValue( myRadius );
//    verify( connect( rSpin, SIGNAL( sbdValueChanged( double ) ), SLOT( onRadiusChanged( double ) ) ) );
    verify( connect( rSpin, SIGNAL( valueChanged( double ) ), SLOT( onRadiusChanged( double ) ) ) );

    rBox->addWidget( rad );
    rBox->addWidget( rSpin );
    rBox->addStretch( 2 );

    // sphere position frame
    QFrame* pFrame = new QFrame( this );
    QHBoxLayout* pBox = new QHBoxLayout( pFrame );
    pBox->setMargin( 5 );

    pFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( pFrame );
    topLayout->addSpacing( 10 );
    
    QLabel *lX, *lY, *lZ;

    lX = new QLabel( tr( "MEN_DLG_X" ), pFrame );
    lY = new QLabel( tr( "MEN_DLG_Y" ), pFrame );
    lZ = new QLabel( tr( "MEN_DLG_Z" ), pFrame );

    myXSpin = new QDoubleSpinBox( pFrame );
    myXSpin->setRange( -10000, 10000 );
    myXSpin->setSingleStep( 1 );
    myXSpin->setValue( 0 );
	  connect(myXSpin, SIGNAL(valueChanged (double)), SLOT(onPositionChanged()));
    myYSpin = new QDoubleSpinBox( pFrame );
    myYSpin->setRange( -10000, 10000 );
    myYSpin->setSingleStep( 1 );
    myYSpin->setValue( 0 );
    connect(myYSpin, SIGNAL(valueChanged (double)), SLOT(onPositionChanged()));
    myZSpin = new QDoubleSpinBox( pFrame );
    myZSpin->setRange( -10000, 10000 );
    myZSpin->setSingleStep( 1 );
	  myZSpin->setValue( 0 );
	  connect(myZSpin, SIGNAL(valueChanged (double)), SLOT(onPositionChanged()));
    

    pBox->addStretch( 2 );
    pBox->addWidget( lX );
    pBox->addWidget( myXSpin );
    pBox->addStretch( 2 );
    pBox->addWidget( lY );
    pBox->addWidget( myYSpin );
    pBox->addStretch( 2 );
    pBox->addWidget( lZ );
    pBox->addWidget( myZSpin );
    pBox->addStretch( 2 );

    // sphere aspect frame
    QFrame* aFrame = new QFrame( this );
    QHBoxLayout* aBox = new QHBoxLayout( aFrame );
    aBox->setMargin( 5 );

    aFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( aFrame );
    topLayout->addSpacing( 10 );

    myVNormal = new QCheckBox( aFrame );
    myVNormal->setText( tr( "MEN_BTN_VNORMAL" ) );
    myVNormal->setChecked( true );
    //verify( connect( myVNormal, SIGNAL( toggled( bool ) ), SLOT( onShowEdges( bool ) ) ) );

    myVColor = new QCheckBox( aFrame );
    myVColor->setText( tr( "MEN_BTN_VCOLOR" ) );
    //verify( connect( myVColor, SIGNAL( toggled( bool ) ), SLOT( onShowEdges( bool ) ) ) );

    myVTexel = new QCheckBox( aFrame );
    myVTexel->setText( tr( "MEN_BTN_VTEXEL" ) );
    //verify( connect( myVTexel, SIGNAL( toggled( bool ) ), SLOT( onShowEdges( bool ) ) ) );

    myText = new QCheckBox( aFrame );
    myText->setText( tr( "MEN_BTN_TEXT" ) );
    verify( connect( myText, SIGNAL( clicked() ), SLOT( onText() ) ) );

    aBox->addStretch( 2 );
    aBox->addWidget( myVNormal );
    aBox->addStretch( 2 );
    aBox->addWidget( myVColor );
    aBox->addStretch( 2 );
    aBox->addWidget( myVTexel );
    aBox->addStretch( 2 );
    aBox->addWidget( myText );
    aBox->addStretch( 2 );

    // tesselation frame
    QFrame* tFrame = new QFrame( this );
    QHBoxLayout* tBox = new QHBoxLayout( tFrame );
    tBox->setMargin( 5 );

    tFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( tFrame );
    topLayout->addSpacing( 10 );

    QLabel *defl, *panes, *items;

    defl = new QLabel( tr( "MEN_DLG_DEFLECTION" ), tFrame );
    panes = new QLabel( tr( "MEN_DLG_PANES" ), tFrame );
    items = new QLabel( tr( "MEN_DLG_ITEMS" ), tFrame );

    myDefSpin = new QDoubleSpinBox( tFrame );
    myDefSpin->setRange( 0.1, 13 );
    myDefSpin->setSingleStep( 0.1 );
    myDefSpin->setValue( 1 );
    verify( connect( myDefSpin, SIGNAL( valueChanged( double ) ), SLOT( onDeflectionChanged( double ) ) ) );
    myPanesSpin = new QSpinBox( tFrame );
    myPanesSpin->setRange( 8, 10000 );
    myPanesSpin->setSingleStep( 1 );
    verify( connect( myPanesSpin, SIGNAL( valueChanged( int ) ), SLOT( onPanesChanged( int ) ) ) );
    myItemsSpin = new QSpinBox( tFrame );
    myItemsSpin->setRange( 25, 10000000 );
    myItemsSpin->setSingleStep( 1 );
    verify( connect( myItemsSpin, SIGNAL( valueChanged( int ) ), SLOT( onItemsChanged( int ) ) ) );

    tBox->addWidget( defl );
    tBox->addWidget( myDefSpin );
    tBox->addWidget( panes );
    tBox->addWidget( myPanesSpin );
    tBox->addWidget( items );
    tBox->addWidget( myItemsSpin );

    // control buttons
    QPushButton *bOk, *bCancel/*, *bHelp*/;
    bOk     = new QPushButton( tr( "BTN_OK" ), this );
    bOk->setDefault(true);
    verify( connect( bOk, SIGNAL( clicked() ), SLOT( onOk() ) ) );
    bCancel = new QPushButton( tr( "BTN_CANCEL" ), this );
    verify( connect( bCancel, SIGNAL( clicked() ), SLOT( onCancel() ) ) );
	verify( connect( this, SIGNAL( destroyed() ), SLOT( onCancel() ) ) );
//    bHelp   = new QPushButton( tr( "BTN_HELP" ), this );
//    verify( connect( bHelp, SIGNAL( clicked() ), SLOT( onHelp() ) ) );

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing( 5 );
	btnLayout->addStretch( 10 );
    btnLayout->addWidget( bOk );
    btnLayout->addWidget( bCancel );
    btnLayout->addStretch( 10 );
//    btnLayout->addWidget( bHelp );
    topLayout->addLayout( btnLayout );

    QSize s = topLayout->totalMinimumSize();
    s.setWidth( s.width() + topLayout->margin() + 10 );
    s.setHeight( s.height() + topLayout->margin() + 10 );
    setFixedSize( s );
}

ShapeDlg::~ShapeDlg()
{
}

void ShapeDlg::showEvent ( QShowEvent* e )
{
    QWidget::showEvent( e );
//    myXSpin->setValueDouble( 0 );
//    myYSpin->setValueDouble( 0 );
//    myZSpin->setValueDouble( 0 );
    myXSpin->setValue( 0 );
    myYSpin->setValue( 0 );
    myZSpin->setValue( 0 );
    onRadiusChanged( myRadius );
}

void ShapeDlg::onOk()
{
    hide();
    updateSphere();
    myShape.Nullify();
}
 
void ShapeDlg::onCancel()
{
    hide();
    myContext->Remove( myShape );
    myShape.Nullify();
}

void ShapeDlg::onHelp()
{
}

void ShapeDlg::onRadiusChanged( double value )
{
    myRadius = value;

    myNbPanes = Sphere_Sphere::NbPanes( myRadius, myDeflection );
    myNbItems = Sphere_Sphere::NbItems( myNbPanes );
    
    myPanesSpin->blockSignals( true );
    myItemsSpin->blockSignals( true );

    myPanesSpin->setValue( myNbPanes );
    myItemsSpin->setValue( myNbItems );

    myPanesSpin->blockSignals( false );
    myItemsSpin->blockSignals( false );

    updateSphere();
}

void ShapeDlg::onItemsChanged( int value )
{
    myNbItems = value;
    myNbPanes = Sphere_Sphere::NbPanes( myNbItems );
    myDeflection = Sphere_Sphere::Deflection( myRadius, myNbPanes );

    myDefSpin->blockSignals( true );
    myPanesSpin->blockSignals( true );

//    myDefSpin->setValueDouble( myDeflection );
    myDefSpin->setValue( myDeflection );
    myPanesSpin->setValue( myNbPanes );

    myDefSpin->blockSignals( false );
    myPanesSpin->blockSignals( false );

    updateSphere();
}

void ShapeDlg::onPanesChanged( int value )
{
    myNbPanes = value;
    myNbItems = Sphere_Sphere::NbItems( myNbPanes );
    myDeflection = Sphere_Sphere::Deflection( myRadius, myNbPanes );

    myDefSpin->blockSignals( true );
    myItemsSpin->blockSignals( true );

//    myDefSpin->setValueDouble( myDeflection );
    myDefSpin->setValue( myDeflection );
    myItemsSpin->setValue( myNbItems );

    myDefSpin->blockSignals( false );
    myItemsSpin->blockSignals( false );

    updateSphere();
}

void ShapeDlg::onDeflectionChanged( double value )
{
    myDeflection = value;
    myNbPanes = Sphere_Sphere::NbPanes( myRadius, myDeflection );
    myNbItems = Sphere_Sphere::NbItems( myNbPanes );

    myPanesSpin->blockSignals( true );
    myItemsSpin->blockSignals( true );

    myPanesSpin->setValue( myNbPanes );
    myItemsSpin->setValue( myNbItems );

    myPanesSpin->blockSignals( false );
    myItemsSpin->blockSignals( false );

    updateSphere();
}

void ShapeDlg::onText()
{
    updateSphere();
}


void ShapeDlg::updateSphere()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    bool visible = isVisible();
    gp_Pnt myPosition( myXSpin->value(), myYSpin->value(), myZSpin->value() );
    if ( !myShape.IsNull() ) {
        myContext->Remove( myShape, false );
        myShape.Nullify();
    }

    myShape = 
        new Sphere_Sphere( myPosition, myRadius, myDeflection, 1/*myVNormalsFlag*/,
                         0/*myVColorsFlag*/, 0/*myVTexelsFlag*/, myText->isChecked(), Graphic3d_ArrayOfPrimitives::IsEnable());
    myContext->SetDisplayMode( myShape, Application::getApplication()->getActiveMDI()->getDisplayMode(), false );
    Application::startTimer();
    myContext->Display( myShape, false );
    Application::stopTimer( 0, "Display Sphere::Sphere", !visible );
    Application::startTimer();
    myView->Update();
    Application::stopTimer( 0, "UPDATE", !visible );
    QApplication::restoreOverrideCursor();
}

void ShapeDlg::onPositionChanged()
{
	updateSphere();
}

void ShapeDlg::closeEvent ( QCloseEvent* e )
{
	onCancel();
}
