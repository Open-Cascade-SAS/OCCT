// File:	StepRepr_ShapeAspectRelationship.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
