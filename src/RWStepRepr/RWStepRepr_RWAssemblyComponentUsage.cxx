// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWAssemblyComponentUsage.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=======================================================================
//function : RWStepRepr_RWAssemblyComponentUsage
//purpose  : 
//=======================================================================
RWStepRepr_RWAssemblyComponentUsage::RWStepRepr_RWAssemblyComponentUsage ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepRepr_AssemblyComponentUsage) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"assembly_component_usage") ) return;

  // Inherited fields of ProductDefinitionRelationship

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Id;
  data->ReadString (num, 1, "product_definition_relationship.id", ach, aProductDefinitionRelationship_Id);

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Name;
  data->ReadString (num, 2, "product_definition_relationship.name", ach, aProductDefinitionRelationship_Name);

  Handle(TCollection_HAsciiString) aProductDefinitionRelationship_Description;
  Standard_Boolean hasProductDefinitionRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "product_definition_relationship.description", ach, aProductDefinitionRelationship_Description);
  }
  else {
    hasProductDefinitionRelationship_Description = Standard_False;
  }

  StepBasic_ProductDefinitionOrReference aProductDefinitionRelationship_RelatingProductDefinition;
  data->ReadEntity (num, 4, "product_definition_relationship.relating_product_definition", ach, aProductDefinitionRelationship_RelatingProductDefinition);
  if (aProductDefinitionRelationship_RelatingProductDefinition.Value().IsNull())
  {
    Handle(StepRepr_ProductDefinitionShape) aProductDefinitionShape;
    data->ReadEntity (num, 4, "product_definition_relationship.relating_product_definition_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aProductDefinitionShape);
    if (!aProductDefinitionShape.IsNull())
    {
        aProductDefinitionRelationship_RelatingProductDefinition.SetValue(aProductDefinitionShape->Definition().ProductDefinition());
    }
  }

  StepBasic_ProductDefinitionOrReference aProductDefinitionRelationship_RelatedProductDefinition;
  data->ReadEntity (num, 5, "product_definition_relationship.related_product_definition", ach, aProductDefinitionRelationship_RelatedProductDefinition);
  if (aProductDefinitionRelationship_RelatedProductDefinition.Value().IsNull())
  {
    Handle(StepRepr_ProductDefinitionShape) aProductDefinitionShape;
    data->ReadEntity (num, 5, "product_definition_relationship.related_product_definition_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aProductDefinitionShape);
    if (!aProductDefinitionShape.IsNull())
    {
        aProductDefinitionRelationship_RelatedProductDefinition.SetValue(aProductDefinitionShape->Definition().ProductDefinition());
    }
  }

  // Own fields of AssemblyComponentUsage

  Handle(TCollection_HAsciiString) aReferenceDesignator;
  Standard_Boolean hasReferenceDesignator = Standard_True;
  if ( data->IsParamDefined (num,6) ) {
    data->ReadString (num, 6, "reference_designator", ach, aReferenceDesignator);
  }
  else {
    hasReferenceDesignator = Standard_False;
  }

  // Initialize entity
  ent->Init(aProductDefinitionRelationship_Id,
            aProductDefinitionRelationship_Name,
            hasProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_Description,
            aProductDefinitionRelationship_RelatingProductDefinition,
            aProductDefinitionRelationship_RelatedProductDefinition,
            hasReferenceDesignator,
            aReferenceDesignator);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepRepr_AssemblyComponentUsage) &ent) const
{

  // Inherited fields of ProductDefinitionRelationship

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::Id());

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::Name());

  if ( ent->StepBasic_ProductDefinitionRelationship::HasDescription() ) {
    SW.Send (ent->StepBasic_ProductDefinitionRelationship::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinitionAP242().Value());

  SW.Send (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinitionAP242().Value());

  // Own fields of AssemblyComponentUsage

  if ( ent->HasReferenceDesignator() ) {
    SW.Send (ent->ReferenceDesignator());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWAssemblyComponentUsage::Share (const Handle(StepRepr_AssemblyComponentUsage) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of ProductDefinitionRelationship

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatingProductDefinitionAP242().Value());

  iter.AddItem (ent->StepBasic_ProductDefinitionRelationship::RelatedProductDefinitionAP242().Value());

  // Own fields of AssemblyComponentUsage
}
