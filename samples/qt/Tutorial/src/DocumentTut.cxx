#include "DocumentTut.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

TopoDS_Shape
MakeBottle(const Standard_Real myWidth , const Standard_Real myHeight , const Standard_Real myThickness);

DocumentTut::DocumentTut( const int theIndex, ApplicationCommonWindow* app )
: DocumentCommon( theIndex, app )
{
}

DocumentTut::~DocumentTut()
{
}

void DocumentTut::onMakeBottle()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    TopoDS_Shape aBottle=MakeBottle(50,70,30);
    Handle(AIS_Shape) AISBottle=new AIS_Shape(aBottle);
    getContext()->SetMaterial(AISBottle,Graphic3d_NOM_GOLD);
    getContext()->SetDisplayMode(AISBottle,1,Standard_False);
    getContext()->Display(AISBottle, Standard_False);	
    getContext()->SetCurrentObject(AISBottle,Standard_False);
    emit selectionChanged();
    fitAll();
    QApplication::restoreOverrideCursor();
}


