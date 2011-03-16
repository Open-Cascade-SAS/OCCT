// File:	StepRepr_SpecifiedHigherUsageOccurrence.cxx
// Created:	Mon Jul  3 20:13:37 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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
