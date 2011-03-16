// File:	StepRepr_QuantifiedAssemblyComponentUsage.cxx
// Created:	Mon Jul  3 20:13:37 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepRepr_QuantifiedAssemblyComponentUsage.ixx>

//=======================================================================
//function : StepRepr_QuantifiedAssemblyComponentUsage
//purpose  : 
//=======================================================================

StepRepr_QuantifiedAssemblyComponentUsage::StepRepr_QuantifiedAssemblyComponentUsage ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_QuantifiedAssemblyComponentUsage::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                                      const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                                      const Standard_Boolean hasProductDefinitionRelationship_Description,
                                                      const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                                      const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatingProductDefinition,
                                                      const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatedProductDefinition,
                                                      const Standard_Boolean hasAssemblyComponentUsage_ReferenceDesignator,
                                                      const Handle(TCollection_HAsciiString) &aAssemblyComponentUsage_ReferenceDesignator,
                                                      const Handle(StepBasic_MeasureWithUnit) &aQuantity)
{
  StepRepr_AssemblyComponentUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition,
                                        hasAssemblyComponentUsage_ReferenceDesignator,
                                        aAssemblyComponentUsage_ReferenceDesignator);

  theQuantity = aQuantity;
}

//=======================================================================
//function : Quantity
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepRepr_QuantifiedAssemblyComponentUsage::Quantity () const
{
  return theQuantity;
}

//=======================================================================
//function : SetQuantity
//purpose  : 
//=======================================================================

void StepRepr_QuantifiedAssemblyComponentUsage::SetQuantity (const Handle(StepBasic_MeasureWithUnit) &aQuantity)
{
  theQuantity = aQuantity;
}
