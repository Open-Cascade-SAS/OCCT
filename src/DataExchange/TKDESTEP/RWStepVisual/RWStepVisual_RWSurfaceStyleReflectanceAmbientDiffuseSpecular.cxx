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

#include "RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular.pxx"
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular.hxx>
#include <StepVisual_Colour.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular::
  RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular()
= default;

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                  theData,
  const int                                                                    theNum,
  occ::handle<Interface_Check>&                                                theAch,
  const occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum,
                              5,
                              theAch,
                              "surface_style_reflectance_ambient_diffuse_specular"))
    return;

  // Inherited fields of SurfaceStyleReflectanceAmbient
  double aAmbientReflectance;
  theData->ReadReal(theNum, 1, "ambient_reflectance", theAch, aAmbientReflectance);

  // Inherited fields of SurfaceStyleReflectanceAmbientDiffuse
  double aDiffuseReflectance;
  theData->ReadReal(theNum, 2, "diffuse_reflectance", theAch, aDiffuseReflectance);

  // Own fields of SurfaceStyleReflectanceAmbientDiffuseSpecular
  double aSpecularReflectance;
  theData->ReadReal(theNum, 3, "specular_reflectance", theAch, aSpecularReflectance);

  double aSpecularExponent;
  theData->ReadReal(theNum, 4, "specular_exponent", theAch, aSpecularExponent);

  occ::handle<StepVisual_Colour> aSpecularColour;
  theData->ReadEntity(theNum,
                      5,
                      "specular_colour",
                      theAch,
                      STANDARD_TYPE(StepVisual_Colour),
                      aSpecularColour);

  // Initialize entity
  theEnt->Init(aAmbientReflectance,
               aDiffuseReflectance,
               aSpecularReflectance,
               aSpecularExponent,
               aSpecularColour);
}

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular::WriteStep(
  StepData_StepWriter&                                                         theSW,
  const occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular>& theEnt) const
{
  // Inherited fields of SurfaceStyleReflectanceAmbient
  theSW.Send(theEnt->AmbientReflectance());

  // Inherited fields of SurfaceStyleReflectanceAmbientDiffuse
  theSW.Send(theEnt->DiffuseReflectance());

  // Own fields of SurfaceStyleReflectanceAmbientDiffuseSpecular
  theSW.Send(theEnt->SpecularReflectance());
  theSW.Send(theEnt->SpecularExponent());
  theSW.Send(theEnt->SpecularColour());
}

//=================================================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbientDiffuseSpecular::Share(
  const occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular>& theEnt,
  Interface_EntityIterator&                                                    theIter) const
{
  // Own fields of SurfaceStyleReflectanceAmbientDiffuseSpecular
  theIter.AddItem(theEnt->SpecularColour());
}
