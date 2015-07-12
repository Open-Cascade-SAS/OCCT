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
#include <RWStepBasic_RWProductCategoryRelationship.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWProductCategoryRelationship
//purpose  : 
//=======================================================================
RWStepBasic_RWProductCategoryRelationship::RWStepBasic_RWProductCategoryRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductCategoryRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                          const Standard_Integer num,
                                                          Handle(Interface_Check)& ach,
                                                          const Handle(StepBasic_ProductCategoryRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"product_category_relationship") ) return;

  // Own fields of ProductCategoryRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_ProductCategory) aCategory;
  data->ReadEntity (num, 3, "category", ach, STANDARD_TYPE(StepBasic_ProductCategory), aCategory);

  Handle(StepBasic_ProductCategory) aSubCategory;
  data->ReadEntity (num, 4, "sub_category", ach, STANDARD_TYPE(StepBasic_ProductCategory), aSubCategory);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aCategory,
            aSubCategory);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductCategoryRelationship::WriteStep (StepData_StepWriter& SW,
                                                           const Handle(StepBasic_ProductCategoryRelationship) &ent) const
{

  // Own fields of ProductCategoryRelationship

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Category());

  SW.Send (ent->SubCategory());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductCategoryRelationship::Share (const Handle(StepBasic_ProductCategoryRelationship) &ent,
                                                       Interface_EntityIterator& iter) const
{

  // Own fields of ProductCategoryRelationship

  iter.AddItem (ent->Category());

  iter.AddItem (ent->SubCategory());
}
