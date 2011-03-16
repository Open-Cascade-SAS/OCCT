#include <qapplication.h>
#include "application.h"

int main( int argc, char ** argv ) 
{
    QApplication a( argc, argv );

    Application *w = new Application();
    w->setWindowTitle( "Voxel demo-application" );
    w->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    return a.exec();
}
