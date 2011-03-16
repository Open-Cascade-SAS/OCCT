// File:	RWStepBasic_RWMassUnit.cxx
// Created:	Thu Dec 12 15:38:08 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepBasic_RWMassUnit.ixx>

//=======================================================================
//function : RWStepBasic_RWMassUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWMassUnit::RWStepBasic_RWMassUnit ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMassUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
                                       const Standard_Integer num,
                                       Handle(Interface_Check)& ach,
                                       const Handle(StepBasic_MassUnit) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"mass_unit") ) return;

  // Inherited fields of NamedUnit

  Handle(StepBasic_DimensionalExponents) aNamedUnit_Dimensions;
  data->ReadEntity (num, 1, "named_unit.dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aNamedUnit_Dimensions);

  // Initialize entity
  ent->Init(aNamedUnit_Dimensions);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMassUnit::WriteStep (StepData_StepWriter& SW,
                                        const Handle(StepBasic_MassUnit) &ent) const
{

  // Inherited fields of NamedUnit

  SW.Send (ent->StepBasic_NamedUnit::Dimensions());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWMassUnit::Share (const Handle(StepBasic_MassUnit) &ent,
                                    Interface_EntityIterator& iter) const
{

  // Inherited fields of NamedUnit

  iter.AddItem (ent->StepBasic_NamedUnit::Dimensions());
}
