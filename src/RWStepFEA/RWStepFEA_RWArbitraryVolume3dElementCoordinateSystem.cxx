// File:	RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.ixx>

//=======================================================================
//function : RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem
//purpose  : 
//=======================================================================

RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"arbitrary_volume3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  Handle(StepFEA_FeaAxis2Placement3d) aCoordinateSystem;
  data->ReadEntity (num, 2, "coordinate_system", ach, STANDARD_TYPE(StepFEA_FeaAxis2Placement3d), aCoordinateSystem);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aCoordinateSystem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  SW.Send (ent->CoordinateSystem());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::Share (const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent,
                                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  iter.AddItem (ent->CoordinateSystem());
}
