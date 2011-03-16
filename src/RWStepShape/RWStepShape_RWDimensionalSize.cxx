// File:	RWStepShape_RWDimensionalSize.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepShape_RWDimensionalSize.ixx>

//=======================================================================
//function : RWStepShape_RWDimensionalSize
//purpose  : 
//=======================================================================

RWStepShape_RWDimensionalSize::RWStepShape_RWDimensionalSize ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepShape_DimensionalSize) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"dimensional_size") ) return;

  // Own fields of DimensionalSize

  Handle(StepRepr_ShapeAspect) aAppliesTo;
  data->ReadEntity (num, 1, "applies_to", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aAppliesTo);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  // Initialize entity
  ent->Init(aAppliesTo,
            aName);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepShape_DimensionalSize) &ent) const
{

  // Own fields of DimensionalSize

  SW.Send (ent->AppliesTo());

  SW.Send (ent->Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::Share (const Handle(StepShape_DimensionalSize) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of DimensionalSize

  iter.AddItem (ent->AppliesTo());
}
