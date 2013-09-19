// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#include <V3d_Plane.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <gp_Pln.hxx>

IMPLEMENT_STANDARD_HANDLE(V3d_Plane, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(V3d_Plane, MMgt_TShared)

// =======================================================================
// function : V3d_Plane
// purpose  :
// =======================================================================
V3d_Plane::V3d_Plane (const Standard_Real theA,
                      const Standard_Real theB,
                      const Standard_Real theC,
                      const Standard_Real theD)
: myGraphicStructure(),
  myPlane (new Graphic3d_ClipPlane (gp_Pln (theA, theB, theC, theD)))
{
}

// =======================================================================
// function : V3d_Plane
// purpose  :
// =======================================================================
void V3d_Plane::SetPlane (const Standard_Real theA,
                          const Standard_Real theB,
                          const Standard_Real theC,
                          const Standard_Real theD)
{
  myPlane->SetEquation (gp_Pln (theA, theB, theC, theD));
  if (IsDisplayed())
  {
    Update();
  }
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void V3d_Plane::Display (const Handle(V3d_View)& theView,
                         const Quantity_Color& theColor)
{
  Handle(V3d_Viewer) aViewer = theView->Viewer();
  if (!myGraphicStructure.IsNull())
  {
    myGraphicStructure->Clear();
  }

  myGraphicStructure = new Graphic3d_Structure (aViewer->Viewer());
  Handle(Graphic3d_Group) aGroup = new Graphic3d_Group (myGraphicStructure);
  Handle(Graphic3d_AspectFillArea3d) anAsp = new Graphic3d_AspectFillArea3d();
  Graphic3d_MaterialAspect aPlastic (Graphic3d_NOM_PLASTIC);
  aPlastic.SetColor (theColor);
  aPlastic.SetTransparency (0.5);
  theView->SetTransparency (Standard_True);
  anAsp->SetFrontMaterial (aPlastic);
  anAsp->SetInteriorStyle (Aspect_IS_HATCH);
  anAsp->SetHatchStyle (Aspect_HS_GRID_DIAGONAL_WIDE);
  myGraphicStructure->SetPrimitivesAspect (anAsp);

  const Standard_ShortReal aSize = (Standard_ShortReal)(0.5*aViewer->DefaultViewSize());
  const Standard_ShortReal anOffset = aSize/5000.0f;

  Handle(Graphic3d_ArrayOfQuadrangles) aPrims = new Graphic3d_ArrayOfQuadrangles(4);
  aPrims->AddVertex (-aSize,-aSize, anOffset);
  aPrims->AddVertex (-aSize, aSize, anOffset);
  aPrims->AddVertex ( aSize, aSize, anOffset);
  aPrims->AddVertex ( aSize,-aSize, anOffset);
  aGroup->AddPrimitiveArray(aPrims);

  myGraphicStructure->Display(0);
  Update();
}

// =======================================================================
// function : Erase
// purpose  :
// =======================================================================
void V3d_Plane::Erase()
{
  if (!myGraphicStructure.IsNull())
  {
    myGraphicStructure->Erase();
  }
}

// =======================================================================
// function : Plane
// purpose  :
// =======================================================================
void V3d_Plane::Plane (Standard_Real& theA, Standard_Real& theB, Standard_Real& theC, Standard_Real& theD) const
{
  const Graphic3d_ClipPlane::Equation& anEquation = myPlane->GetEquation();
  theA = anEquation[0];
  theB = anEquation[1];
  theC = anEquation[2];
  theD = anEquation[3];
}

// =======================================================================
// function : IsDisplayed
// purpose  :
// =======================================================================
Standard_Boolean V3d_Plane::IsDisplayed() const
{
  if (myGraphicStructure.IsNull())
  {
    return Standard_False;
  }

  return myGraphicStructure->IsDisplayed();
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void V3d_Plane::Update()
{
  if(!myGraphicStructure.IsNull())
  {
    TColStd_Array2OfReal aMatrix (1, 4, 1, 4);
    Standard_Real theA, theB, theC, theD;
    this->Plane(theA, theB, theC, theD);
    gp_Pln aGeomPln (theA, theB, theC, theD);
    gp_Trsf aTransform;
    aTransform.SetTransformation (aGeomPln.Position());
    aTransform.Invert();
    for (Standard_Integer i = 1; i <= 3; i++)
    {
      for (Standard_Integer j = 1; j <= 4; j++)
      {
        aMatrix.SetValue (i, j, aTransform.Value (i,j));
      }
    }

    aMatrix.SetValue (4,1,0.);
    aMatrix.SetValue (4,2,0.);
    aMatrix.SetValue (4,3,0.);
    aMatrix.SetValue (4,4,1.);
    myGraphicStructure->SetTransform (aMatrix, Graphic3d_TOC_REPLACE);
  }
}
