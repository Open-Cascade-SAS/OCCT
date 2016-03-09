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

#include <Standard_Type.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_SpecifiedHigherUsageOccurrence,StepRepr_AssemblyComponentUsage)

//=======================================================================
//function : StepRepr_SpecifiedHigherUsageOccurrence
//purpose  : 
//=======================================================================
StepRepr_SpecifiedHigherUsageOccurrence::StepRepr_SpecifiedHigherUsageOccurrence ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_SpecifiedHigherUsageOccurrence::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                                    const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                                    const Standard_Boolean hasProductDefinitionRelationship_Description,
                                                    const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                                    const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatingProductDefinition,
                                                    const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatedProductDefinition,
                                                    const Standard_Boolean hasAssemblyComponentUsage_ReferenceDesignator,
                                                    const Handle(TCollection_HAsciiString) &aAssemblyComponentUsage_ReferenceDesignator,
                                                    const Handle(StepRepr_AssemblyComponentUsage) &aUpperUsage,
                                                    const Handle(StepRepr_NextAssemblyUsageOccurrence) &aNextUsage)
{
  StepRepr_AssemblyComponentUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition,
                                        hasAssemblyComponentUsage_ReferenceDesignator,
                                        aAssemblyComponentUsage_ReferenceDesignator);

  theUpperUsage = aUpperUsage;

  theNextUsage = aNextUsage;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_SpecifiedHigherUsageOccurrence::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                                    const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                                    const Standard_Boolean hasProductDefinitionRelationship_Description,
                                                    const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                                    const StepBasic_ProductDefinitionOrReference &aProductDefinitionRelationship_RelatingProductDefinition,
                                                    const StepBasic_ProductDefinitionOrReference &aProductDefinitionRelationship_RelatedProductDefinition,
                                                    const Standard_Boolean hasAssemblyComponentUsage_ReferenceDesignator,
                                                    const Handle(TCollection_HAsciiString) &aAssemblyComponentUsage_ReferenceDesignator,
                                                    const Handle(StepRepr_AssemblyComponentUsage) &aUpperUsage,
                                                    const Handle(StepRepr_NextAssemblyUsageOccurrence) &aNextUsage)
{
  StepRepr_AssemblyComponentUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition,
                                        hasAssemblyComponentUsage_ReferenceDesignator,
                                        aAssemblyComponentUsage_ReferenceDesignator);

  theUpperUsage = aUpperUsage;

  theNextUsage = aNextUsage;
}

//=======================================================================
//function : UpperUsage
//purpose  : 
//=======================================================================

Handle(StepRepr_AssemblyComponentUsage) StepRepr_SpecifiedHigherUsageOccurrence::UpperUsage () const
{
  return theUpperUsage;
}

//=======================================================================
//function : SetUpperUsage
//purpose  : 
//=======================================================================

void StepRepr_SpecifiedHigherUsageOccurrence::SetUpperUsage (const Handle(StepRepr_AssemblyComponentUsage) &aUpperUsage)
{
  theUpperUsage = aUpperUsage;
}

//=======================================================================
//function : NextUsage
//purpose  : 
//=======================================================================

Handle(StepRepr_NextAssemblyUsageOccurrence) StepRepr_SpecifiedHigherUsageOccurrence::NextUsage () const
{
  return theNextUsage;
}

//=======================================================================
//function : SetNextUsage
//purpose  : 
//=======================================================================

void StepRepr_SpecifiedHigherUsageOccurrence::SetNextUsage (const Handle(StepRepr_NextAssemblyUsageOccurrence) &aNextUsage)
{
  theNextUsage = aNextUsage;
}
