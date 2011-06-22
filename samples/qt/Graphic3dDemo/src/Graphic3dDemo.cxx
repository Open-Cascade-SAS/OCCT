#include <stdlib.h>

#include <qmainwindow.h>
#include <qapplication.h>
#include <qtranslator.h>
#include <qpixmap.h>
#include <qtranslator.h>

#include "global.h"
#include "Application.h"

#define GLOBAL_CONTEXT "@default"

class Translator : public QTranslator
{
public:
  Translator( QObject* parent = 0 ) : QTranslator( parent ) {}
  virtual QString translate( const char* context, 
                             const char* sourceText,
                             const char* comment = 0 ) const
  {
    QString res = QTranslator::translate( context, sourceText, comment );
    if( res.isNull() )
      res = QTranslator::translate( GLOBAL_CONTEXT, sourceText, comment );
    return res;
  }
};

int main(int argc, char* argv[])
{    
  QApplication a( argc, argv );
	
  QString dir( getenv( "CSF_ResourcesDefaults" ) );

  Translator msg( 0 ), img( 0 );
  msg.load( "Sample_msg.qm", dir );
  a.installTranslator( &msg );
    
  img.load( "Sample_icons.qm", dir );
  a.installTranslator( &img );

  QPixmap pix( dir + QString( "/" ) + QObject::tr( "ICON_SAMPLE" ) );

	Application* app = new Application();
  app->setWindowTitle( QObject::tr( "TIT_SAMPLE" ) );
  app->setWindowIcon( pix );
    
	QApplication::setOverrideCursor( Qt::WaitCursor );
	app->show();
	app->InitApp();
	QApplication::restoreOverrideCursor();
    
  return a.exec();
}
