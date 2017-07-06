// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <V3d_Plane.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <gp_Pln.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Plane,Standard_Transient)

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

  myGraphicStructure = new Graphic3d_Structure (aViewer->StructureManager());
  Handle(Graphic3d_Group)            aGroup = myGraphicStructure->NewGroup();
  Handle(Graphic3d_AspectFillArea3d) anAsp  = new Graphic3d_AspectFillArea3d();
  Graphic3d_MaterialAspect aPlastic (Graphic3d_NOM_PLASTIC);
  aPlastic.SetColor (theColor);
  aPlastic.SetTransparency (0.5);
  anAsp->SetFrontMaterial (aPlastic);
  anAsp->SetInteriorStyle (Aspect_IS_HATCH);
  anAsp->SetHatchStyle (new Graphic3d_HatchStyle (Aspect_HS_GRID_DIAGONAL_WIDE));
  aGroup->SetGroupPrimitivesAspect (anAsp);

  const Standard_ShortReal aSize = (Standard_ShortReal)(0.5*aViewer->DefaultViewSize());
  const Standard_ShortReal anOffset = aSize/5000.0f;

  Handle(Graphic3d_ArrayOfQuadrangles) aPrims = new Graphic3d_ArrayOfQuadrangles(4);
  aPrims->AddVertex (-aSize,-aSize, anOffset);
  aPrims->AddVertex (-aSize, aSize, anOffset);
  aPrims->AddVertex ( aSize, aSize, anOffset);
  aPrims->AddVertex ( aSize,-aSize, anOffset);
  aGroup->AddPrimitiveArray(aPrims);

  myGraphicStructure->SetDisplayPriority (0);
  myGraphicStructure->Display();
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
  if (myGraphicStructure.IsNull())
  {
    return;
  }

  const gp_Pln aGeomPln = myPlane->ToPlane();
  gp_Trsf aTransform;
  aTransform.SetTransformation (aGeomPln.Position());
  aTransform.Invert();
  myGraphicStructure->SetTransformation (new Geom_Transformation (aTransform));
}
