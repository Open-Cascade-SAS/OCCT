#include "ApplicationTut.h"

#include <OSD_Environment.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QTranslator>
#include <Standard_WarningsRestore.hxx>

int main ( int argc, char* argv[] )
{
#if QT_VERSION > 0x050000
    TCollection_AsciiString aPlugindsDirName = OSD_Environment ("QTDIR").Value();
    if (!aPlugindsDirName.IsEmpty())
      QApplication::addLibraryPath (QString (aPlugindsDirName.ToCString()) + "/plugins");
#endif
    QApplication a( argc, argv );

    QString resDir = ApplicationCommonWindow::getResourceDir();
    QString resTutDir = ApplicationTut::getTutResourceDir();

    QTranslator strTrans( 0 );
    Standard_Boolean isOK = strTrans.load( "Common-string", resDir );
    if( isOK )
      a.installTranslator( &strTrans );

    QTranslator iconTrans( 0 );
    isOK = iconTrans.load( "Common-icon", resDir );
    if( isOK )
      a.installTranslator( &iconTrans );

    QTranslator strTutTrans( 0 );
    isOK = strTutTrans.load( "Tutorial-string", resTutDir );
    if( isOK )
      a.installTranslator( &strTutTrans );

    QTranslator iconTutTrans( 0 );
    isOK = iconTutTrans.load( "Tutorial-icon", resTutDir );
    if( isOK )
      a.installTranslator( &iconTutTrans );

    QObject::connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    ApplicationTut* mw = new ApplicationTut();
    mw->setWindowTitle( QObject::tr( "TIT_SAMPLE" ) );
    QString aResName( resDir + QString( "/" ) + QObject::tr( "ICON_SAMPLE" ) );
    mw->setWindowIcon( QPixmap( aResName ) );

    mw->show();
    
    return a.exec();
}
