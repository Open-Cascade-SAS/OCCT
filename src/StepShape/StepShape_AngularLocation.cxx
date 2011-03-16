// File:	StepShape_AngularLocation.cxx
// Created:	Tue Apr 18 16:42:57 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
