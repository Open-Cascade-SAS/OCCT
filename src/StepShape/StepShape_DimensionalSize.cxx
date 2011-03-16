// File:	StepShape_DimensionalSize.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_DimensionalSize.ixx>

//=======================================================================
//function : StepShape_DimensionalSize
//purpose  : 
//=======================================================================

StepShape_DimensionalSize::StepShape_DimensionalSize ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::Init (const Handle(StepRepr_ShapeAspect) &aAppliesTo,
                                      const Handle(TCollection_HAsciiString) &aName)
{

  theAppliesTo = aAppliesTo;

  theName = aName;
}

//=======================================================================
//function : AppliesTo
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalSize::AppliesTo () const
{
  return theAppliesTo;
}

//=======================================================================
//function : SetAppliesTo
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::SetAppliesTo (const Handle(StepRepr_ShapeAspect) &aAppliesTo)
{
  theAppliesTo = aAppliesTo;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepShape_DimensionalSize::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}
