// Created on: 2000-04-18
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ShapeAspectRelationship,Standard_Transient)

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
