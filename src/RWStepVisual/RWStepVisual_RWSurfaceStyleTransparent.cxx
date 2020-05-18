// Created on : Tue May 12 14:11:46 2020
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

#include <RWStepVisual_RWSurfaceStyleTransparent.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceStyleTransparent.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepVisual_RWSurfaceStyleTransparent
//purpose  :
//=======================================================================

RWStepVisual_RWSurfaceStyleTransparent::RWStepVisual_RWSurfaceStyleTransparent() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
#include <iostream>

void RWStepVisual_RWSurfaceStyleTransparent::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepVisual_SurfaceStyleTransparent)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"surface_style_transparent") ) return;

  // Own fields of SurfaceStyleTransparent

  Standard_Real aTransparency;
  data->ReadReal (num, 1, "transparency", ach, aTransparency);

  // Initialize entity
  ent->Init(aTransparency);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleTransparent::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepVisual_SurfaceStyleTransparent)& ent) const
{

  // Own fields of SurfaceStyleTransparent

  SW.Send (ent->Transparency());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================

void RWStepVisual_RWSurfaceStyleTransparent::Share (const Handle(StepVisual_SurfaceStyleTransparent)& ,
                                                    Interface_EntityIterator& ) const
{

}
