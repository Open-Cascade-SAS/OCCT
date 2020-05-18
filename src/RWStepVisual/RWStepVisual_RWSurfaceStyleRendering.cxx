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

#include <RWStepVisual_RWSurfaceStyleRendering.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleRendering.hxx>
#include <StepVisual_ShadingSurfaceMethod.hxx>
#include <StepVisual_Colour.hxx>

//=======================================================================
//function : RWStepVisual_RWSurfaceStyleRendering
//purpose  :
//=======================================================================

RWStepVisual_RWSurfaceStyleRendering::RWStepVisual_RWSurfaceStyleRendering() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRendering::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepVisual_SurfaceStyleRendering)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"surface_style_rendering") ) return;

  // Own fields of SurfaceStyleRendering

  StepVisual_ShadingSurfaceMethod aRenderingMethod = StepVisual_ssmNormalShading;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".CONSTANT_SHADING.")) aRenderingMethod = StepVisual_ssmConstantShading;
    else if (strcmp(text, ".COLOUR_SHADING.")) aRenderingMethod = StepVisual_ssmColourShading;
    else if (strcmp(text, ".DOT_SHADING.")) aRenderingMethod = StepVisual_ssmDotShading;
    else if (strcmp(text, ".NORMAL_SHADING.")) aRenderingMethod = StepVisual_ssmNormalShading;
    else ach->AddFail("Parameter #1 (rendering_method) has not allowed value");
  }
  else ach->AddFail("Parameter #1 (rendering_method) is not enumeration");

  Handle(StepVisual_Colour) aSurfaceColour;
  data->ReadEntity (num, 2, "surface_colour", ach, STANDARD_TYPE(StepVisual_Colour), aSurfaceColour);

  // Initialize entity
  ent->Init(aRenderingMethod,
            aSurfaceColour);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRendering::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepVisual_SurfaceStyleRendering)& ent) const
{

  // Own fields of SurfaceStyleRendering

  switch (ent->RenderingMethod()) {
    case StepVisual_ssmConstantShading: SW.SendEnum (".CONSTANT_SHADING."); break;
    case StepVisual_ssmColourShading: SW.SendEnum (".COLOUR_SHADING."); break;
    case StepVisual_ssmDotShading: SW.SendEnum (".DOT_SHADING."); break;
    case StepVisual_ssmNormalShading: SW.SendEnum (".NORMAL_SHADING."); break;
  }

  SW.Send (ent->SurfaceColour());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleRendering::Share (const Handle(StepVisual_SurfaceStyleRendering)& ent,
                                                  Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceStyleRendering

  iter.AddItem (ent->SurfaceColour());
}
