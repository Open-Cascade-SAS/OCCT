// File:	RWStepElement_RWSurfaceSection.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWSurfaceSection.ixx>

//=======================================================================
//function : RWStepElement_RWSurfaceSection
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceSection::RWStepElement_RWSurfaceSection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepElement_SurfaceSection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_section") ) return;

  // Own fields of SurfaceSection

  StepElement_MeasureOrUnspecifiedValue aOffset;
  data->ReadEntity (num, 1, "offset", ach, aOffset);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMass;
  data->ReadEntity (num, 2, "non_structural_mass", ach, aNonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMassOffset;
  data->ReadEntity (num, 3, "non_structural_mass_offset", ach, aNonStructuralMassOffset);

  // Initialize entity
  ent->Init(aOffset,
            aNonStructuralMass,
            aNonStructuralMassOffset);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepElement_SurfaceSection) &ent) const
{

  // Own fields of SurfaceSection

  SW.Send (ent->Offset().Value());

  SW.Send (ent->NonStructuralMass().Value());

  SW.Send (ent->NonStructuralMassOffset().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::Share (const Handle(StepElement_SurfaceSection) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceSection
/*  CKY 17JUN04. Content is made of REAL and ENUM. No entity !
  iter.AddItem (ent->Offset().Value());

  iter.AddItem (ent->NonStructuralMass().Value());

  iter.AddItem (ent->NonStructuralMassOffset().Value());
*/
}
