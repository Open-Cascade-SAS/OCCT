// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWProductConcept.hxx>
#include <StepBasic_ProductConceptContext.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ProductConcept.hxx>

//=======================================================================
//function : RWStepRepr_RWProductConcept
//purpose  : 
//=======================================================================
RWStepRepr_RWProductConcept::RWStepRepr_RWProductConcept ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductConcept::ReadStep (const Handle(StepData_StepReaderData)& data,
                                            const Standard_Integer num,
                                            Handle(Interface_Check)& ach,
                                            const Handle(StepRepr_ProductConcept) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"product_concept") ) return;

  // Own fields of ProductConcept

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_ProductConceptContext) aMarketContext;
  data->ReadEntity (num, 4, "market_context", ach, STANDARD_TYPE(StepBasic_ProductConceptContext), aMarketContext);

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aMarketContext);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductConcept::WriteStep (StepData_StepWriter& SW,
                                             const Handle(StepRepr_ProductConcept) &ent) const
{

  // Own fields of ProductConcept

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->MarketContext());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWProductConcept::Share (const Handle(StepRepr_ProductConcept) &ent,
                                         Interface_EntityIterator& iter) const
{

  // Own fields of ProductConcept

  iter.AddItem (ent->MarketContext());
}
