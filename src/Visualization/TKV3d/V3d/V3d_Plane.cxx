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
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Plane, Standard_Transient)

//=================================================================================================

V3d_Plane::V3d_Plane(const double theA, const double theB, const double theC, const double theD)
    : 
      myPlane(new Graphic3d_ClipPlane(gp_Pln(theA, theB, theC, theD)))
{
}

//=================================================================================================

void V3d_Plane::SetPlane(const double theA, const double theB, const double theC, const double theD)
{
  myPlane->SetEquation(gp_Pln(theA, theB, theC, theD));
  if (IsDisplayed())
  {
    Update();
  }
}

//=================================================================================================

void V3d_Plane::Display(const occ::handle<V3d_View>& theView, const Quantity_Color& theColor)
{
  occ::handle<V3d_Viewer> aViewer = theView->Viewer();
  if (!myGraphicStructure.IsNull())
  {
    myGraphicStructure->Clear();
  }

  myGraphicStructure = new Graphic3d_Structure(aViewer->StructureManager());
  occ::handle<Graphic3d_Group>            aGroup = myGraphicStructure->NewGroup();
  occ::handle<Graphic3d_AspectFillArea3d> anAsp  = new Graphic3d_AspectFillArea3d();
  Graphic3d_MaterialAspect                aPlastic(Graphic3d_NameOfMaterial_Plastified);
  aPlastic.SetColor(theColor);
  aPlastic.SetTransparency(0.5);
  anAsp->SetFrontMaterial(aPlastic);
  anAsp->SetInteriorStyle(Aspect_IS_HATCH);
  anAsp->SetHatchStyle(new Graphic3d_HatchStyle(Aspect_HS_GRID_DIAGONAL_WIDE));
  aGroup->SetGroupPrimitivesAspect(anAsp);

  const float aSize    = (float)(0.5 * aViewer->DefaultViewSize());
  const float anOffset = aSize / 5000.0f;

  occ::handle<Graphic3d_ArrayOfQuadrangles> aPrims = new Graphic3d_ArrayOfQuadrangles(4);
  aPrims->AddVertex(-aSize, -aSize, anOffset);
  aPrims->AddVertex(-aSize, aSize, anOffset);
  aPrims->AddVertex(aSize, aSize, anOffset);
  aPrims->AddVertex(aSize, -aSize, anOffset);
  aGroup->AddPrimitiveArray(aPrims);

  myGraphicStructure->SetDisplayPriority(Graphic3d_DisplayPriority_Bottom);
  myGraphicStructure->Display();
  Update();
}

//=================================================================================================

void V3d_Plane::Erase()
{
  if (!myGraphicStructure.IsNull())
  {
    myGraphicStructure->Erase();
  }
}

//=================================================================================================

void V3d_Plane::Plane(double& theA, double& theB, double& theC, double& theD) const
{
  const Graphic3d_ClipPlane::Equation& anEquation = myPlane->GetEquation();
  theA                                            = anEquation[0];
  theB                                            = anEquation[1];
  theC                                            = anEquation[2];
  theD                                            = anEquation[3];
}

//=================================================================================================

bool V3d_Plane::IsDisplayed() const
{
  if (myGraphicStructure.IsNull())
  {
    return false;
  }

  return myGraphicStructure->IsDisplayed();
}

//=================================================================================================

void V3d_Plane::Update()
{
  if (myGraphicStructure.IsNull())
  {
    return;
  }

  const gp_Pln aGeomPln = myPlane->ToPlane();
  gp_Trsf      aTransform;
  aTransform.SetTransformation(aGeomPln.Position());
  aTransform.Invert();
  myGraphicStructure->SetTransformation(new TopLoc_Datum3D(aTransform));
}
