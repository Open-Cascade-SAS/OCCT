#include "DocumentTut.h"

#include <Standard_WarningsDisable.hxx>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <Standard_WarningsRestore.hxx>

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

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
    Handle(AIS_InteractiveContext) aCtx = getContext();
    for (V3d_ListOfView::Iterator aViewIter (aCtx->CurrentViewer()->ActiveViews()); aViewIter.More(); aViewIter.Next())
    {
      const Handle(V3d_View)& aView = aViewIter.Value();
      Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
      aParams.RenderResolutionScale = 2.0f;
    }

    const Handle(Prs3d_Drawer)& aDefDrawer = aCtx->DefaultDrawer();
    aDefDrawer->SetIsoOnTriangulation (true);

    QApplication::setOverrideCursor( Qt::WaitCursor );
    TopoDS_Shape aBottle=MakeBottle(50,70,30);
    Handle(AIS_Shape) AISBottle=new AIS_Shape(aBottle);
    getContext()->SetMaterial (AISBottle, Graphic3d_NameOfMaterial_Gold, Standard_False);
    getContext()->SetDisplayMode(AISBottle, 1, Standard_False);
    getContext()->Display(AISBottle, Standard_False);	
    const Handle(AIS_InteractiveObject)& anIOAISBottle = AISBottle;
    getContext()->SetSelected(anIOAISBottle,Standard_False);
    emit selectionChanged();
    fitAll();
    QApplication::restoreOverrideCursor();
}


