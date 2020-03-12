// Created on : Sat May 02 12:41:14 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
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

#include <RWStepRepr_RWRepresentationContextReference.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationContextReference.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : RWStepRepr_RWRepresentationContextReference
//purpose  : 
//=======================================================================

RWStepRepr_RWRepresentationContextReference::RWStepRepr_RWRepresentationContextReference() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                            const Standard_Integer theNum,
                                                            Handle(Interface_Check)& theAch,
                                                            const Handle(StepRepr_RepresentationContextReference)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,1,theAch,"representation_context_reference") ) return;

  // Own fields of RepresentationContextReference

  Handle(TCollection_HAsciiString) aContextIdentifier;
  theData->ReadString (theNum, 1, "context_identifier", theAch, aContextIdentifier);

  // Initialize entity
  theEnt->Init(aContextIdentifier);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::WriteStep (StepData_StepWriter& theSW,
                                                             const Handle(StepRepr_RepresentationContextReference)& theEnt) const
{

  // Own fields of RepresentationContextReference

  theSW.Send (theEnt->ContextIdentifier());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWRepresentationContextReference::Share (const Handle(StepRepr_RepresentationContextReference)& /*theEnt*/,
                                                         Interface_EntityIterator& /*iter*/) const
{

  // Own fields of RepresentationContextReference
}
