// Created on: 2000-04-18
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepShape_AngularLocation.ixx>

//=======================================================================
//function : StepShape_AngularLocation
//purpose  : 
//=======================================================================

StepShape_AngularLocation::StepShape_AngularLocation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_AngularLocation::Init (const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Name,
                                      const Standard_Boolean hasShapeAspectRelationship_Description,
                                      const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Description,
                                      const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatingShapeAspect,
                                      const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatedShapeAspect,
                                      const StepShape_AngleRelator aAngleSelection)
{
  StepShape_DimensionalLocation::Init(aShapeAspectRelationship_Name,
                                      hasShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_RelatingShapeAspect,
                                      aShapeAspectRelationship_RelatedShapeAspect);

  theAngleSelection = aAngleSelection;
}

//=======================================================================
//function : AngleSelection
//purpose  : 
//=======================================================================

StepShape_AngleRelator StepShape_AngularLocation::AngleSelection () const
{
  return theAngleSelection;
}

//=======================================================================
//function : SetAngleSelection
//purpose  : 
//=======================================================================

void StepShape_AngularLocation::SetAngleSelection (const StepShape_AngleRelator aAngleSelection)
{
  theAngleSelection = aAngleSelection;
}
