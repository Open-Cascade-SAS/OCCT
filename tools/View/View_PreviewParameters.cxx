// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/View_PreviewParameters.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_PreviewParameters::View_PreviewParameters(const Standard_Boolean theToTransparent)
{
  myDrawer = new Prs3d_Drawer();

  Quantity_Color aColor(Quantity_NOC_TOMATO);

  // point parameters
  myDrawer->SetPointAspect(new Prs3d_PointAspect(Aspect_TOM_O_PLUS, aColor, 3.0));

  // shading parameters
  Graphic3d_MaterialAspect aShadingMaterial;
  aShadingMaterial.SetMaterialType(Graphic3d_MATERIAL_ASPECT);

  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myDrawer->ShadingAspect()->SetColor(aColor);
  myDrawer->ShadingAspect()->SetMaterial(aShadingMaterial);

  myDrawer->SetLineAspect(new Prs3d_LineAspect(aColor, Aspect_TOL_SOLID, 1.));

  if (theToTransparent)
  {
    Standard_ShortReal aTransparency = 0.8f;

    myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency(aTransparency);
    myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial().SetTransparency(aTransparency);
    myDrawer->SetTransparency(aTransparency);
  }
  // common parameters
  myDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
}
