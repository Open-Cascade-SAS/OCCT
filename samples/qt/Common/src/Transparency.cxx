#include "Transparency.h"

#include <QHBoxLayout>
#include <QSpinBox>

DialogTransparency::DialogTransparency( QWidget* parent, Qt::WindowFlags f, bool modal )
: QDialog( parent, f )
{
    setModal( modal );
    QHBoxLayout* base = new QHBoxLayout( this );
	  base->setMargin( 3 );
    base->setSpacing( 3 );
	  QSpinBox* aSpin = new QSpinBox( this );
	  aSpin->setRange( 0, 10 );
	  aSpin->setSingleStep( 1 );
    base->addWidget( aSpin );
	  connect( aSpin, SIGNAL( valueChanged( int ) ), this, SIGNAL( sendTransparencyChanged( int ) ) );
}

DialogTransparency::~DialogTransparency()
{
}
