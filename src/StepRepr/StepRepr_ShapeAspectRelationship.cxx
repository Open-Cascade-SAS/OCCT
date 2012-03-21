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

#include <StepRepr_ShapeAspectRelationship.ixx>

//=======================================================================
//function : StepRepr_ShapeAspectRelationship
//purpose  : 
//=======================================================================

StepRepr_ShapeAspectRelationship::StepRepr_ShapeAspectRelationship ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ShapeAspectRelationship::Init (const Handle(TCollection_HAsciiString) &aName,
                                             const Standard_Boolean hasDescription,
                                             const Handle(TCollection_HAsciiString) &aDescription,
                                             const Handle(StepRepr_ShapeAspect) &aRelatingShapeAspect,
                                             const Handle(StepRepr_ShapeAspect) &aRelatedShapeAspect)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theRelatingShapeAspect = aRelatingShapeAspect;

  theRelatedShapeAspect = aRelatedShapeAspect;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ShapeAspectRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_ShapeAspectRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ShapeAspectRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_ShapeAspectRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_ShapeAspectRelationship::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : RelatingShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepRepr_ShapeAspectRelationship::RelatingShapeAspect () const
{
  return theRelatingShapeAspect;
}

//=======================================================================
//function : SetRelatingShapeAspect
//purpose  : 
//=======================================================================

void StepRepr_ShapeAspectRelationship::SetRelatingShapeAspect (const Handle(StepRepr_ShapeAspect) &aRelatingShapeAspect)
{
  theRelatingShapeAspect = aRelatingShapeAspect;
}

//=======================================================================
//function : RelatedShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepRepr_ShapeAspectRelationship::RelatedShapeAspect () const
{
  return theRelatedShapeAspect;
}

//=======================================================================
//function : SetRelatedShapeAspect
//purpose  : 
//=======================================================================

void StepRepr_ShapeAspectRelationship::SetRelatedShapeAspect (const Handle(StepRepr_ShapeAspect) &aRelatedShapeAspect)
{
  theRelatedShapeAspect = aRelatedShapeAspect;
}
