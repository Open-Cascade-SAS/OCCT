// Created on : Thu May 14 15:13:19 2020
// Created by: Igor KHOZHANOV
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2020
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

#include <RWStepVisual_RWSurfaceStyleRenderingWithProperties.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>
#include <StepVisual_ShadingSurfaceMethod.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_HArray1OfRenderingPropertiesSelect.hxx>
#include <StepVisual_RenderingPropertiesSelect.hxx>

//=======================================================================
//function : RWStepVisual_RWSurfaceStyleRenderingWithProperties
//purpose  :
//=======================================================================

RWStepVisual_RWSurfaceStyleRenderingWithProperties::RWStepVisual_RWSurfaceStyleRenderingWithProperties() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRenderingWithProperties::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                   const Standard_Integer num,
                                                                   Handle(Interface_Check)& ach,
                                                                   const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_style_rendering_with_properties") ) return;

  // Inherited fields of SurfaceStyleRendering

  StepVisual_ShadingSurfaceMethod aSurfaceStyleRendering_RenderingMethod = StepVisual_ssmNormalShading;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".CONSTANT_SHADING.")) aSurfaceStyleRendering_RenderingMethod = StepVisual_ssmConstantShading;
    else if (strcmp(text, ".COLOUR_SHADING.")) aSurfaceStyleRendering_RenderingMethod = StepVisual_ssmColourShading;
    else if (strcmp(text, ".DOT_SHADING.")) aSurfaceStyleRendering_RenderingMethod = StepVisual_ssmDotShading;
    else if (strcmp(text, ".NORMAL_SHADING.")) aSurfaceStyleRendering_RenderingMethod = StepVisual_ssmNormalShading;
    else ach->AddFail("Parameter #1 (surface_style_rendering.rendering_method) has not allowed value");
  }
  else ach->AddFail("Parameter #1 (surface_style_rendering.rendering_method) is not enumeration");

  Handle(StepVisual_Colour) aSurfaceStyleRendering_SurfaceColour;
  data->ReadEntity (num, 2, "surface_style_rendering.surface_colour", ach, STANDARD_TYPE(StepVisual_Colour), aSurfaceStyleRendering_SurfaceColour);

  // Own fields of SurfaceStyleRenderingWithProperties

  Handle(StepVisual_HArray1OfRenderingPropertiesSelect) aProperties;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "properties", ach, sub3) ) {
    Standard_Integer nb0 = data->NbParams(sub3);
    aProperties = new StepVisual_HArray1OfRenderingPropertiesSelect (1, nb0);
    Standard_Integer num2 = sub3;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepVisual_RenderingPropertiesSelect anIt0;
      data->ReadEntity (num2, i0, "rendering_properties_select", ach, anIt0);
      aProperties->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aSurfaceStyleRendering_RenderingMethod,
            aSurfaceStyleRendering_SurfaceColour,
            aProperties);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRenderingWithProperties::WriteStep (StepData_StepWriter& SW,
                                                                    const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent) const
{

  // Own fields of SurfaceStyleRendering

  switch (ent->RenderingMethod()) {
    case StepVisual_ssmConstantShading: SW.SendEnum (".CONSTANT_SHADING."); break;
    case StepVisual_ssmColourShading: SW.SendEnum (".COLOUR_SHADING."); break;
    case StepVisual_ssmDotShading: SW.SendEnum (".DOT_SHADING."); break;
    case StepVisual_ssmNormalShading: SW.SendEnum (".NORMAL_SHADING."); break;
  }

  SW.Send (ent->SurfaceColour());

  // Own fields of SurfaceStyleRenderingWithProperties

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Properties()->Length(); i2++ ) {
    StepVisual_RenderingPropertiesSelect Var0 = ent->Properties()->Value(i2);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRenderingWithProperties::Share (const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent,
                                                                Interface_EntityIterator& iter) const
{

  // Inherited fields of SurfaceStyleRendering

  iter.AddItem (ent->StepVisual_SurfaceStyleRendering::SurfaceColour());

  // Own fields of SurfaceStyleRenderingWithProperties

  for (Standard_Integer i2=1; i2 <= ent->Properties()->Length(); i2++ ) {
    StepVisual_RenderingPropertiesSelect Var0 = ent->Properties()->Value(i2);
    iter.AddItem (Var0.Value());
  }
}
