// File:	StepShape_DimensionalCharacteristic.cxx
// Created:	Tue Apr 18 16:42:57 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_DimensionalCharacteristic.ixx>

//=======================================================================
//function : StepShape_DimensionalCharacteristic
//purpose  : 
//=======================================================================

StepShape_DimensionalCharacteristic::StepShape_DimensionalCharacteristic ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepShape_DimensionalCharacteristic::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 2;
  return 0;
}

//=======================================================================
//function : DimensionalLocation
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalLocation) StepShape_DimensionalCharacteristic::DimensionalLocation () const
{
  return Handle(StepShape_DimensionalLocation)::DownCast(Value());
}

//=======================================================================
//function : DimensionalSize
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalSize) StepShape_DimensionalCharacteristic::DimensionalSize () const
{
  return Handle(StepShape_DimensionalSize)::DownCast(Value());
}
