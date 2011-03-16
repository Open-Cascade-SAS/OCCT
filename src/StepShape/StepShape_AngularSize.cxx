// File:	StepShape_AngularSize.cxx
// Created:	Tue Apr 18 16:42:57 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_AngularSize.ixx>

//=======================================================================
//function : StepShape_AngularSize
//purpose  : 
//=======================================================================

StepShape_AngularSize::StepShape_AngularSize ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_AngularSize::Init (const Handle(StepRepr_ShapeAspect) &aDimensionalSize_AppliesTo,
                                  const Handle(TCollection_HAsciiString) &aDimensionalSize_Name,
                                  const StepShape_AngleRelator aAngleSelection)
{
  StepShape_DimensionalSize::Init(aDimensionalSize_AppliesTo,
                                  aDimensionalSize_Name);

  theAngleSelection = aAngleSelection;
}

//=======================================================================
//function : AngleSelection
//purpose  : 
//=======================================================================

StepShape_AngleRelator StepShape_AngularSize::AngleSelection () const
{
  return theAngleSelection;
}

//=======================================================================
//function : SetAngleSelection
//purpose  : 
//=======================================================================

void StepShape_AngularSize::SetAngleSelection (const StepShape_AngleRelator aAngleSelection)
{
  theAngleSelection = aAngleSelection;
}
