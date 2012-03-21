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
