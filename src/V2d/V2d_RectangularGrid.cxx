// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI

#define PRO9581 //GG_270298
//              Creer la grille dans un objet graphique a l'altitude 0
//              de facon a etre tracee en premier.

// Update 14/07/98 : DCB : PRO10988 SetColorIndices() method

#define BUC60639        //GG 15/03/00
//              -> Avoid to compute view minmax including grid origin marker.

#include <V2d_RectangularGrid.ixx>
#ifdef PRO9581
#include <V2d_BackgroundGraphicObject.hxx>
#else
#include <Graphic2d_GraphicObject.hxx>
#endif
#include <Graphic2d_Marker.hxx>
#include <V2d_Viewer.hxx>

V2d_RectangularGrid::V2d_RectangularGrid(const V2d_ViewerPointer& aViewer,
                                         const Standard_Integer aColorIndex1,
                                         const Standard_Integer aColorIndex2)
                                         : Aspect_RectangularGrid(1.,1.),
                                           myViewer(aViewer),
#ifdef PRO9581
    myGraphicObject(new V2d_BackgroundGraphicObject(aViewer->View())),
#else
    myGraphicObject(new Graphic2d_GraphicObject(aViewer->View())),
#endif
                                           myColorIndex1(aColorIndex1),
                                           myColorIndex2(aColorIndex2)
{
  Standard_Real l = 100;
  Standard_Integer n = (Standard_Integer)log10(l);
  Standard_Real step = Pow(10.,Standard_Real(n))/10.;
  SetXStep(step);
  SetYStep(step);
  Init();
}

void V2d_RectangularGrid::SetColorIndices (const Standard_Integer aColorIndex1,
                                           const Standard_Integer aColorIndex2)
{
  myColorIndex1 = aColorIndex1;
  myColorIndex2 = aColorIndex2;
}

void V2d_RectangularGrid::Display()
{
  myGraphicObject->Display();
}

void V2d_RectangularGrid::Erase() const
{
  myGraphicObject->Erase();
}

void V2d_RectangularGrid::UpdateDisplay()
{
  myGraphicObject->RemovePrimitives();
  myGrid = new V2d_RectangularGraphicGrid
    (myGraphicObject,
     XStep(),YStep(),FirstAngle()+RotationAngle(),SecondAngle()+RotationAngle()+Standard_PI/2.,XOrigin(),YOrigin(),myColorIndex2);
  myGrid->SetColorIndex(myColorIndex1);
  myGrid->SetDrawMode(DrawMode());
#ifndef BUC60639        //See V2d_RectangularGraphicGrid
  Handle(Graphic2d_Marker) MK1 = new Graphic2d_Marker(myGraphicObject,14,XOrigin(),YOrigin(),0.01,0.01,FirstAngle()+RotationAngle());
  Handle(Graphic2d_Marker) MK2 = new Graphic2d_Marker(myGraphicObject,14,XOrigin(),YOrigin(),0.01,0.01,SecondAngle()+RotationAngle()+Standard_PI/2.);
  MK1->SetColorIndex(myColorIndex2);
  MK2->SetColorIndex(myColorIndex2);
#endif

}

Standard_Boolean V2d_RectangularGrid::IsDisplayed () const
{
  return myGraphicObject->IsDisplayed();
}
