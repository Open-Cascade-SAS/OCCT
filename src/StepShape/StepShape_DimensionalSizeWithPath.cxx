// File:	StepShape_DimensionalSizeWithPath.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_DimensionalSizeWithPath.ixx>

//=======================================================================
//function : StepShape_DimensionalSizeWithPath
//purpose  : 
//=======================================================================

StepShape_DimensionalSizeWithPath::StepShape_DimensionalSizeWithPath ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalSizeWithPath::Init (const Handle(StepRepr_ShapeAspect) &aDimensionalSize_AppliesTo,
                                              const Handle(TCollection_HAsciiString) &aDimensionalSize_Name,
                                              const Handle(StepRepr_ShapeAspect) &aPath)
{
  StepShape_DimensionalSize::Init(aDimensionalSize_AppliesTo,
                                  aDimensionalSize_Name);

  thePath = aPath;
}

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalSizeWithPath::Path () const
{
  return thePath;
}

//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================

void StepShape_DimensionalSizeWithPath::SetPath (const Handle(StepRepr_ShapeAspect) &aPath)
{
  thePath = aPath;
}
