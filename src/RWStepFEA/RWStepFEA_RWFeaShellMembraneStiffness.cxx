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
#include <RWStepFEA_RWFeaShellMembraneStiffness.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaShellMembraneStiffness.hxx>

//=======================================================================
//function : RWStepFEA_RWFeaShellMembraneStiffness
//purpose  : 
//=======================================================================
RWStepFEA_RWFeaShellMembraneStiffness::RWStepFEA_RWFeaShellMembraneStiffness ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepFEA_FeaShellMembraneStiffness) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_shell_membrane_stiffness") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaShellMembraneStiffness

  StepFEA_SymmetricTensor42d aFeaConstants;
  data->ReadEntity (num, 2, "fea_constants", ach, aFeaConstants);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstants);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepFEA_FeaShellMembraneStiffness) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaShellMembraneStiffness

  SW.Send (ent->FeaConstants().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::Share (const Handle(StepFEA_FeaShellMembraneStiffness) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaShellMembraneStiffness

  iter.AddItem (ent->FeaConstants().Value());
}
