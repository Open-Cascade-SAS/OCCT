// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepRepr_MaterialPropertyRepresentation.ixx>

//=======================================================================
//function : StepRepr_MaterialPropertyRepresentation
//purpose  : 
//=======================================================================

StepRepr_MaterialPropertyRepresentation::StepRepr_MaterialPropertyRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_MaterialPropertyRepresentation::Init (const StepRepr_RepresentedDefinition &aPropertyDefinitionRepresentation_Definition,
                                                    const Handle(StepRepr_Representation) &aPropertyDefinitionRepresentation_UsedRepresentation,
                                                    const Handle(StepRepr_DataEnvironment) &aDependentEnvironment)
{
  StepRepr_PropertyDefinitionRepresentation::Init(aPropertyDefinitionRepresentation_Definition,
                                                  aPropertyDefinitionRepresentation_UsedRepresentation);

  theDependentEnvironment = aDependentEnvironment;
}

//=======================================================================
//function : DependentEnvironment
//purpose  : 
//=======================================================================

Handle(StepRepr_DataEnvironment) StepRepr_MaterialPropertyRepresentation::DependentEnvironment () const
{
  return theDependentEnvironment;
}

//=======================================================================
//function : SetDependentEnvironment
//purpose  : 
//=======================================================================

void StepRepr_MaterialPropertyRepresentation::SetDependentEnvironment (const Handle(StepRepr_DataEnvironment) &aDependentEnvironment)
{
  theDependentEnvironment = aDependentEnvironment;
}
