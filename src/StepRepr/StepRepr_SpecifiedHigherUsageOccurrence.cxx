// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepRepr_SpecifiedHigherUsageOccurrence.ixx>

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
