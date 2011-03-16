// ViewDlg.cpp: implementation of the ViewDlg class.
//
//////////////////////////////////////////////////////////////////////

#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>

#include <V3d_TypeOfVisualization.hxx>

#include "Application.h"
#include "ViewDlg.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ViewDlg::ViewDlg( QWidget* parent/*, Handle(V3d_View)& view*/ )
:QDialog( parent ),
  myAutoApply( true ), /*myView( view ),*/ myApplyAllViews(false)
{
    setModal( false );
    setWindowTitle( tr( "TITLE_VIEW_DLG" ) );

    QVBoxLayout* topLayout = new QVBoxLayout( this );
    topLayout->setMargin( 5 );

    QFrame* mainFrame = new QFrame( this );
    mainFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    topLayout->addWidget( mainFrame );
    topLayout->addSpacing( 10 );

    // view properties
    QGridLayout* pLayout = new QGridLayout( mainFrame );
    pLayout->setMargin( 5 );
    myZBufferBox = new QCheckBox( mainFrame );;
    myZBufferBox->setText( tr( "BTN_ZBUFFER" ) );
    verify( connect( myZBufferBox, SIGNAL( toggled( bool ) ), SLOT( onZBuffer( bool ) ) ) );

    myAnimBox = new QCheckBox( mainFrame );
    myAnimBox->setText( tr( "BTN_ANIMATION" ) );
    verify( connect( myAnimBox, SIGNAL( toggled( bool ) ), SLOT( onAnimation( bool ) ) ) );

    myDegBox = new QCheckBox( mainFrame );
    myDegBox->setText( tr( "BTN_DEGENERATION" ) );
    verify( connect( myDegBox, SIGNAL( toggled( bool ) ), SLOT( onDegeneration( bool ) ) ) );

    pLayout->addWidget( myZBufferBox, 0, 0 );
    pLayout->addWidget( myAnimBox, 1, 0 );
    pLayout->addWidget( myDegBox, 2, 0 );
    pLayout->setRowStretch( 3, 10 );
    pLayout->setColumnStretch( 1, 10 );

    // apply controls
    QCheckBox *autoApply, *applyAllViews;
    autoApply = new QCheckBox( this );
    autoApply->setText( tr( "BTN_AUTOAPPLY" ) );
    autoApply->setChecked( true );
    verify( connect( autoApply, SIGNAL( toggled( bool ) ), SLOT( onAutoApply( bool ) ) ) );
    applyAllViews = new QCheckBox( this );
    applyAllViews->setText( tr( "BTN_APPLYALLVIEWS" ) );
    verify( connect( applyAllViews, SIGNAL( toggled( bool ) ), 
                                    SLOT( onApplyAllViews( bool ) ) ) );

    QHBoxLayout* applyLayout = new QHBoxLayout();
    applyLayout->addWidget( autoApply );
    applyLayout->addStretch( 10 );
    applyLayout->addWidget( applyAllViews );
    topLayout->addLayout( applyLayout );
    topLayout->addSpacing( 10 );

    // control buttons
    QPushButton *bOk, *bCancel/*, *bHelp*/;
	
    bOk     = new QPushButton( tr( "BTN_OK" ), this );
    verify( connect( bOk, SIGNAL( clicked() ), SLOT( onOk() ) ) );
	bOk->setDefault(true); 
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
    topLayout->addLayout( btnLayout );

    QSize s = topLayout->totalMinimumSize();
    s.setWidth( s.width() + topLayout->margin() + 10 );
    s.setHeight( s.height() + topLayout->margin() + 10 );
    setFixedSize( s );

	Handle(V3d_View) aView = Application::getApplication()->getActiveMDI()->getView();
	myIsAnim = aView->AnimationMode(myIsDeg);
}

ViewDlg::~ViewDlg()
{
}

void ViewDlg::showEvent ( QShowEvent* e )
{
	Update();
	QWidget::showEvent( e );
}   

void ViewDlg::onOk()
{
    hide();
	bool Auto = myAutoApply;

	myAutoApply = true;
	onZBuffer(myZBufferBox->isChecked());
	onAnimation(myAnimBox->isChecked());
	onDegeneration(myDegBox->isChecked());
	myAutoApply = Auto;
}
 
void ViewDlg::onCancel()
{
    hide();
}

void ViewDlg::onHelp()
{
}

void ViewDlg::onZBuffer( bool on )
{
	QWidgetList aWidgetList;
	Handle(V3d_View) aView;
	QWidget* aWidget;

	if (!myApplyAllViews)
		aWidgetList.append(Application::getApplication()->getActiveMDI());
	else
		aWidgetList = Application::getWorkspace()->windowList();

	if ( myAutoApply ){
		foreach(aWidget, aWidgetList){
      if ( !aWidget ) continue;
			MDIWindow* aMDIWindow = (MDIWindow*)aWidget;
			aView = aMDIWindow->getView();
			if ( on )
				aView->SetVisualization( V3d_ZBUFFER );
			else
				aView->SetVisualization( V3d_WIREFRAME );
		}
	}
}

void ViewDlg::onAnimation( bool on )
{
	QWidgetList aWidgetList;
	Handle(V3d_View) aView;

	myIsAnim = on;
    myDegBox->setEnabled( myIsAnim );
	if (!myIsAnim) myDegBox->setChecked(false);

	if (!myApplyAllViews)
		aWidgetList.append(Application::getApplication()->getActiveMDI());
	else
		aWidgetList = Application::getWorkspace()->windowList();
	
	if ( myAutoApply ){
		QWidget* aWidget;
		foreach(aWidget, aWidgetList){
      if ( !aWidget ) continue;
			MDIWindow* aMDIWindow = (MDIWindow*)aWidget;
			aView = aMDIWindow->getView();
			aView->SetAnimationMode( myIsAnim, myIsDeg );
			aView->Redraw();
			aView->SetImmediateUpdate(true);
			aView->Update();
		}
	}	
}

void ViewDlg::onDegeneration( bool on )
{
	QWidgetList aWidgetList;
	Handle(V3d_View) aView;
	QWidget* aWidget;

	if (!myApplyAllViews)
		aWidgetList.append(Application::getApplication()->getActiveMDI());
	else
		aWidgetList = Application::getWorkspace()->windowList();


    myIsDeg = on;
    if ( myAutoApply ){
		foreach(aWidget, aWidgetList){
      if ( !aWidget ) continue;
			MDIWindow* aMDIWindow = (MDIWindow*)aWidget;
			aView = aMDIWindow->getView();
	    aView->SetAnimationMode( myIsAnim, myIsDeg );
		}
	}
}

void ViewDlg::onAutoApply( bool on )
{
  myAutoApply = on;
}

void ViewDlg::onApplyAllViews( bool on )
{
	myApplyAllViews = on;
}

void ViewDlg::Update()
{
	MDIWindow* anActiveMDIWindow = Application::getApplication()->getActiveMDI();
	if (anActiveMDIWindow){
		Handle(V3d_View) aView = anActiveMDIWindow->getView();
		if(!aView.IsNull()){
		
		myIsAnim = aView->AnimationMode( myIsDeg );
		myAnimBox->blockSignals( true );
		myDegBox->blockSignals( true );
		myAnimBox->setChecked( myIsAnim );
		myDegBox->setChecked( myIsDeg );
		myDegBox->setEnabled( myIsAnim );
		myAnimBox->blockSignals( false );
		myDegBox->blockSignals( false );

		myZBufferBox->blockSignals(true);
		V3d_TypeOfVisualization aVisuType = aView->Visualization();
		if(aVisuType == V3d_WIREFRAME)
			myZBufferBox -> setChecked(false);
		else
			myZBufferBox -> setChecked(true);
		myZBufferBox->blockSignals(false);
		}
	}
}
