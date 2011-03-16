#include "Material.h"

#include <QPushButton>
#include <QLayout>
#include <QSignalMapper>

#include <Graphic3d_NameOfMaterial.hxx>

DialogMaterial::DialogMaterial(QWidget * parent,
			        bool modal, Qt::WindowFlags f )
: QDialog(parent, f)
{
	setModal( modal );
	QPushButton* b;
  QVBoxLayout* vbl = new QVBoxLayout( this );
	vbl->setMargin( 8 );

	QSignalMapper *sm = new QSignalMapper( this );
	connect( sm, SIGNAL( mapped( int ) ), this, SIGNAL( sendMaterialChanged( int ) ) );

	b = new QPushButton( QObject::tr("BTN_PLASTER"), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_PLASTER );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_BRASS" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_BRASS );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_BRONZE" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_BRONZE );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_COPPER" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_COPPER );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_GOLD" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_GOLD );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_PEWTER" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_PEWTER );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_PLASTIC" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_PLASTIC );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );

	b = new QPushButton( QObject::tr( "BTN_SILVER" ), this );
  sm->setMapping( b, ( int )Graphic3d_NOM_SILVER );
  connect( b, SIGNAL( clicked() ), sm, SLOT( map() ) );	
	b->setCheckable( true );
	connect( b, SIGNAL( toggled( bool ) ), this, SLOT( updateButtons( bool ) ) );
	myButtons.append( b );
	vbl->addWidget( b );
}

DialogMaterial::~DialogMaterial()
{
}

void DialogMaterial::updateButtons( bool isOn )
{
	if( !isOn )
		return;
	
	QPushButton*sentBy = ( QPushButton* )sender();

	for ( int i = 0; i < myButtons.size(); i++ )
	{
	  QPushButton* b = myButtons.at( i );
	  if( b != sentBy ) {
      b->setEnabled( true );
		  b->setChecked( false );
	  } else {
	 	  b->setEnabled( false );
	  }
	}
}

