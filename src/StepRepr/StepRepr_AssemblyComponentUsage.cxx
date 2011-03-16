// File:	StepRepr_AssemblyComponentUsage.cxx
// Created:	Mon Jul  3 19:47:50 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepRepr_AssemblyComponentUsage.ixx>

//=======================================================================
//function : StepRepr_AssemblyComponentUsage
//purpose  : 
//=======================================================================

StepRepr_AssemblyComponentUsage::StepRepr_AssemblyComponentUsage ()
{
  defReferenceDesignator = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_AssemblyComponentUsage::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                            const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                            const Standard_Boolean hasProductDefinitionRelationship_Description,
                                            const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                            const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatingProductDefinition,
                                            const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatedProductDefinition,
                                            const Standard_Boolean hasReferenceDesignator,
                                            const Handle(TCollection_HAsciiString) &aReferenceDesignator)
{
  StepRepr_ProductDefinitionUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition);

  defReferenceDesignator = hasReferenceDesignator;
  if (defReferenceDesignator) {
    theReferenceDesignator = aReferenceDesignator;
  }
  else theReferenceDesignator.Nullify();
}

//=======================================================================
//function : ReferenceDesignator
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_AssemblyComponentUsage::ReferenceDesignator () const
{
  return theReferenceDesignator;
}

//=======================================================================
//function : SetReferenceDesignator
//purpose  : 
//=======================================================================

void StepRepr_AssemblyComponentUsage::SetReferenceDesignator (const Handle(TCollection_HAsciiString) &aReferenceDesignator)
{
  theReferenceDesignator = aReferenceDesignator;
}

//=======================================================================
//function : HasReferenceDesignator
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_AssemblyComponentUsage::HasReferenceDesignator () const
{
  return defReferenceDesignator;
}
