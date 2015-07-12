// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepElement_RWSurfaceSection.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_SurfaceSection.hxx>

//=======================================================================
//function : RWStepElement_RWSurfaceSection
//purpose  : 
//=======================================================================
RWStepElement_RWSurfaceSection::RWStepElement_RWSurfaceSection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepElement_SurfaceSection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_section") ) return;

  // Own fields of SurfaceSection

  StepElement_MeasureOrUnspecifiedValue aOffset;
  data->ReadEntity (num, 1, "offset", ach, aOffset);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMass;
  data->ReadEntity (num, 2, "non_structural_mass", ach, aNonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMassOffset;
  data->ReadEntity (num, 3, "non_structural_mass_offset", ach, aNonStructuralMassOffset);

  // Initialize entity
  ent->Init(aOffset,
            aNonStructuralMass,
            aNonStructuralMassOffset);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepElement_SurfaceSection) &ent) const
{

  // Own fields of SurfaceSection

  SW.Send (ent->Offset().Value());

  SW.Send (ent->NonStructuralMass().Value());

  SW.Send (ent->NonStructuralMassOffset().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::Share (const Handle(StepElement_SurfaceSection)&,
                                            Interface_EntityIterator&) const
{

  // Own fields of SurfaceSection
/*  CKY 17JUN04. Content is made of REAL and ENUM. No entity !
  iter.AddItem (ent->Offset().Value());

  iter.AddItem (ent->NonStructuralMass().Value());

  iter.AddItem (ent->NonStructuralMassOffset().Value());
*/
}
