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

#include <RWStepVisual_RWSurfaceStyleReflectanceAmbient.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepVisual_RWSurfaceStyleReflectanceAmbient
//purpose  :
//=======================================================================

RWStepVisual_RWSurfaceStyleReflectanceAmbient::RWStepVisual_RWSurfaceStyleReflectanceAmbient() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbient::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepVisual_SurfaceStyleReflectanceAmbient)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"surface_style_reflectance_ambient") ) return;

  // Own fields of SurfaceStyleReflectanceAmbient

  Standard_Real aAmbientReflectance;
  data->ReadReal (num, 1, "ambient_reflectance", ach, aAmbientReflectance);

  // Initialize entity
  ent->Init(aAmbientReflectance);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbient::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepVisual_SurfaceStyleReflectanceAmbient)& ent) const
{

  // Own fields of SurfaceStyleReflectanceAmbient

  SW.Send (ent->AmbientReflectance());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleReflectanceAmbient::Share (const Handle(StepVisual_SurfaceStyleReflectanceAmbient)& ,
                                                           Interface_EntityIterator& ) const
{

  // Own fields of SurfaceStyleReflectanceAmbient
}
