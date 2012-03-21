// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
     XStep(),YStep(),FirstAngle()+RotationAngle(),SecondAngle()+RotationAngle() + M_PI / 2.,XOrigin(),YOrigin(),myColorIndex2);
  myGrid->SetColorIndex(myColorIndex1);
  myGrid->SetDrawMode(DrawMode());
#ifndef BUC60639        //See V2d_RectangularGraphicGrid
  Handle(Graphic2d_Marker) MK1 = new Graphic2d_Marker(myGraphicObject,14,XOrigin(),YOrigin(),0.01,0.01,FirstAngle()+RotationAngle());
  Handle(Graphic2d_Marker) MK2 = new Graphic2d_Marker(myGraphicObject,14,XOrigin(),YOrigin(),0.01,0.01,SecondAngle()+RotationAngle() + M_PI / 2.);
  MK1->SetColorIndex(myColorIndex2);
  MK2->SetColorIndex(myColorIndex2);
#endif

}

Standard_Boolean V2d_RectangularGrid::IsDisplayed () const
{
  return myGraphicObject->IsDisplayed();
}
