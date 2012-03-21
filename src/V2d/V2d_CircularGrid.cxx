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

#define PRO9581 //GG_270298
//              Creer la grille dans un objet graphique a l'altitude 0
//              de facon a etre tracee en premier.

// Update 14/07/98 : DCB : PRO10988 SetColorIndices() method

#include <V2d_CircularGrid.ixx>
#ifdef PRO9581
#include <V2d_BackgroundGraphicObject.hxx>
#else
#include <Graphic2d_GraphicObject.hxx>
#endif
#include <V2d_Viewer.hxx>

V2d_CircularGrid::V2d_CircularGrid(const V2d_ViewerPointer& aViewer,
                                   const Standard_Integer aColorIndex1,
                                   const Standard_Integer aColorIndex2)
                                   : Aspect_CircularGrid(1.,8),
                                     myViewer(aViewer),
#ifdef PRO9581
    myGraphicObject(new V2d_BackgroundGraphicObject(aViewer->View())), 
#else
    myGraphicObject(new Graphic2d_GraphicObject(aViewer->View())),
#endif
                                     myColorIndex1(aColorIndex1),
                                     myColorIndex2(aColorIndex2)
{
  Standard_Real l=100.;
  Standard_Integer n = (Standard_Integer)log10(l);
  Standard_Real step = Pow(10.,Standard_Real(n))/10.;
  SetRadiusStep(step);
  Init();
}

void V2d_CircularGrid::SetColorIndices (const Standard_Integer aColorIndex1,
                                        const Standard_Integer aColorIndex2)
{
  myColorIndex1 = aColorIndex1;
  myColorIndex2 = aColorIndex2;
}

void V2d_CircularGrid::Display()
{
  myGraphicObject->Display();
}

void V2d_CircularGrid::Erase() const
{
  myGraphicObject->Erase();
}

void V2d_CircularGrid::UpdateDisplay()
{
  myGraphicObject->RemovePrimitives();
  myGrid = new V2d_CircularGraphicGrid
    (myGraphicObject,
     XOrigin(),YOrigin(),RotationAngle(),RadiusStep(),DivisionNumber(),myColorIndex2);
  myGrid->SetColorIndex(myColorIndex1);
  myGrid->SetDrawMode(DrawMode());
}

Standard_Boolean V2d_CircularGrid::IsDisplayed () const
{
  return myGraphicObject->IsDisplayed();
}
