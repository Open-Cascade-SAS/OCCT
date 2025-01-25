// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <OpenGl_Aspects.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_TypeOfReflection.hxx>
#include <Graphic3d_MaterialAspect.hxx>

namespace
{
//! Initialize default material in this way for backward compatibility.
inline Graphic3d_MaterialAspect initDefaultMaterial()
{
  Graphic3d_MaterialAspect aMat;
  aMat.SetMaterialType(Graphic3d_MATERIAL_ASPECT);
  aMat.SetAmbientColor(Quantity_Color(Graphic3d_Vec3(0.2f)));
  aMat.SetDiffuseColor(Quantity_Color(Graphic3d_Vec3(0.8f)));
  aMat.SetEmissiveColor(Quantity_Color(Graphic3d_Vec3(0.1f)));
  aMat.SetSpecularColor(Quantity_NOC_BLACK);
  aMat.SetShininess(10.0f / 128.0f);
  aMat.SetRefractionIndex(1.0f);
  return aMat;
}

static const Graphic3d_MaterialAspect THE_DEFAULT_MATERIAL = initDefaultMaterial();
} // namespace

// =======================================================================
// function : OpenGl_Aspects
// purpose  :
// =======================================================================
OpenGl_Aspects::OpenGl_Aspects()
    : myAspect(new Graphic3d_Aspects()),
      myShadingModel(Graphic3d_TypeOfShadingModel_Unlit)
{
  myAspect->SetInteriorStyle(Aspect_IS_SOLID);
  myAspect->SetInteriorColor(Quantity_NOC_WHITE);
  myAspect->SetEdgeColor(Quantity_NOC_WHITE);
  myAspect->SetFrontMaterial(THE_DEFAULT_MATERIAL);
  myAspect->SetBackMaterial(THE_DEFAULT_MATERIAL);
  myAspect->SetShadingModel(myShadingModel);
  myAspect->SetHatchStyle(Handle(Graphic3d_HatchStyle)());
}

// =======================================================================
// function : OpenGl_Aspects
// purpose  :
// =======================================================================
OpenGl_Aspects::OpenGl_Aspects(const Handle(Graphic3d_Aspects)& theAspect)
    : myShadingModel(Graphic3d_TypeOfShadingModel_DEFAULT)
{
  SetAspect(theAspect);
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_Aspects::SetAspect(const Handle(Graphic3d_Aspects)& theAspect)
{
  myAspect = theAspect;

  const Graphic3d_MaterialAspect& aMat = theAspect->FrontMaterial();
  myShadingModel =
    theAspect->ShadingModel() != Graphic3d_TypeOfShadingModel_Unlit
        && (aMat.ReflectionMode(Graphic3d_TOR_AMBIENT) || aMat.ReflectionMode(Graphic3d_TOR_DIFFUSE)
            || aMat.ReflectionMode(Graphic3d_TOR_SPECULAR)
            || aMat.ReflectionMode(Graphic3d_TOR_EMISSION))
      ? theAspect->ShadingModel()
      : Graphic3d_TypeOfShadingModel_Unlit;

  // invalidate resources
  myResTextureSet.UpdateRediness(myAspect);
  myResSprite.UpdateRediness(myAspect);
  myResProgram.UpdateRediness(myAspect);
  if (!myResSprite.IsReady())
  {
    myResTextureSet.Invalidate();
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Aspects::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  theWorkspace->SetAspects(this);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Aspects::Release(OpenGl_Context* theContext)
{
  myResTextureSet.Release(theContext);
  myResSprite.Release(theContext);
  myResProgram.Release(theContext);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Aspects::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, OpenGl_Aspects)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, OpenGl_Element)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAspect.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myShadingModel)
}
