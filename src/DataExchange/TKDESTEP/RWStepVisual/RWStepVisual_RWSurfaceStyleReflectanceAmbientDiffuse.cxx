// Copyright (c) Open CASCADE 2025
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

#include "RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse.pxx"
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuse.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse::
  RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse()
{
}

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse::ReadStep(
  const Handle(StepData_StepReaderData)&                          theData,
  const Standard_Integer                                          theNum,
  Handle(Interface_Check)&                                        theAch,
  const Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 2, theAch, "surface_style_reflectance_ambient_diffuse"))
    return;

  // Inherited fields of SurfaceStyleReflectanceAmbient
  Standard_Real aAmbientReflectance;
  theData->ReadReal(theNum, 1, "ambient_reflectance", theAch, aAmbientReflectance);

  // Own fields of SurfaceStyleReflectanceAmbientDiffuse
  Standard_Real aDiffuseReflectance;
  theData->ReadReal(theNum, 2, "diffuse_reflectance", theAch, aDiffuseReflectance);

  // Initialize entity
  theEnt->Init(aAmbientReflectance, aDiffuseReflectance);
}

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse::WriteStep(
  StepData_StepWriter&                                            theSW,
  const Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse)& theEnt) const
{
  // Inherited fields of SurfaceStyleReflectanceAmbient
  theSW.Send(theEnt->AmbientReflectance());

  // Own fields of SurfaceStyleReflectanceAmbientDiffuse
  theSW.Send(theEnt->DiffuseReflectance());
}

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuse::Share(
  const Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse)&,
  Interface_EntityIterator&) const
{
  // Own fields of SurfaceStyleReflectanceAmbientDiffuse
}
