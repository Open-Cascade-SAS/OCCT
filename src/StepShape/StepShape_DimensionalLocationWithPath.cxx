// File:	StepShape_DimensionalLocationWithPath.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_DimensionalLocationWithPath.ixx>

//=======================================================================
//function : StepShape_DimensionalLocationWithPath
//purpose  : 
//=======================================================================

StepShape_DimensionalLocationWithPath::StepShape_DimensionalLocationWithPath ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalLocationWithPath::Init (const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Name,
                                                  const Standard_Boolean hasShapeAspectRelationship_Description,
                                                  const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Description,
                                                  const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatingShapeAspect,
                                                  const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatedShapeAspect,
                                                  const Handle(StepRepr_ShapeAspect) &aPath)
{
  StepShape_DimensionalLocation::Init(aShapeAspectRelationship_Name,
                                      hasShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_RelatingShapeAspect,
                                      aShapeAspectRelationship_RelatedShapeAspect);

  thePath = aPath;
}

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalLocationWithPath::Path () const
{
  return thePath;
}

//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================

void StepShape_DimensionalLocationWithPath::SetPath (const Handle(StepRepr_ShapeAspect) &aPath)
{
  thePath = aPath;
}
