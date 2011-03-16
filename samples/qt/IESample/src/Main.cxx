#include "Application.h"

#include <QApplication>
#include <QTranslator>
#include <QPixmap>
#include <QLocale>
  


int main ( int argc, char* argv[] )
{
  
  QApplication a( argc, argv );

  QString resDir = ApplicationCommonWindow::getResourceDir();
  QString resIEDir = ApplicationWindow::getIEResourceDir();

  QTranslator strTrans( 0 );
  Standard_Boolean isOK = strTrans.load( "Common-string", resDir );
  if( isOK )
    a.installTranslator( &strTrans );

  QTranslator iconTrans( 0 );
  isOK = iconTrans.load( "Common-icon", resDir );
  if( isOK )
    a.installTranslator( &iconTrans );

  QTranslator strIETrans( 0 );
  isOK = strIETrans.load( "Interface-string", resIEDir );
  if( isOK )
    a.installTranslator( &strIETrans );

  QObject::connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

  ApplicationWindow* mw = new ApplicationWindow();
	mw->setWindowTitle( QObject::tr( "TIT_SAMPLE" ) );
  mw->setWindowIcon( QPixmap( resDir + QString( "/" ) + QObject::tr( "ICON_SAMPLE" ) ) );
  mw->show();
 
  return a.exec();
}

